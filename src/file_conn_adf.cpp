#include "file_conn_adf.h"

SEXP adf_file_con_(SEXP connection, cpp11::strings filename, bool writable) {
  SEXP result;
  Rconnection con_in = R_GetConnection(connection);
  adf_conn  * ac  = (adf_conn *)con_in->private_ptr;
  AdfDevice * dev = ac->dev;
  
  if (!con_in->canwrite && writable)
    Rf_error("Cannot open a writable connection from a write-protected disk");
  
  int mode = ADF_FI_EXPECT_FILE | ADF_FI_EXPECT_VALID_CHECKSUM;
  if (!writable) mode = mode | ADF_FI_EXPECT_EXIST | ADF_FI_THROW_ERROR;

  cpp11::list entry_pos = adf_path_to_entry(connection, filename, mode);
  int vol_num  = cpp11::integers(entry_pos["volume"]).at(0);
  SECTNUM sect = cpp11::integers(entry_pos["sector"]).at(0);
  SECTNUM parent = cpp11::integers(entry_pos["parent"]).at(0);
  
  logicals file_reg_check = adf_check_file_reg(
    connection, vol_num, cpp11::integers(entry_pos["sector"]).at(0));
  if (file_reg_check.at(0))
    Rf_error("Can only open 1 connection per file on a virtual device");
  
  auto vol = dev->volList[vol_num];
  int vol_old = get_adf_vol_internal(connection);
  SECTNUM cur_dir = vol->curDirPtr;

  adf_change_dir_internal(connection, parent, vol_num);

  std::string fns;
  AdfFileMode fmode = ADF_FILE_MODE_READ;
  if (writable) fmode = ADF_FILE_MODE_WRITE;

  if (writable && sect == -1) {
    // The remainder of the path that does not exist:
    fns = (std::string)
      (cpp11::strings(entry_pos["remainder"]).at(0));
    auto fhead = new bFileHeaderBlock;
    check_adf_name(fns);
    const char * fn2 = fns.c_str();
    RETCODE fcret = adfCreateFile(vol, parent, fn2, fhead);
    delete(fhead);
    if (fcret != RC_OK) Rf_error("Failed to create file for writing.");
  } else {
    fns = (std::string)cpp11::strings(entry_pos["name"]).at(0);
  }
  const char* fn = fns.c_str();

  AdfFile * adf_file = adfFileOpen (vol, fn, fmode);
  adf_file_con_str * afc = (adf_file_con_str *)new adf_file_con_str;
  afc->adf_file   = adf_file;
  afc->vol_num    = vol_num;
  afc->parent_con = cpp11::as_cpp<int>(connection);

  Rconnection con;
  result = R_new_custom_connection(
    "adf_file_con", "rb", "adf_file_con", &con);
  
  con->private_ptr    = afc;
  con->destroy        = &adf_fcon_destroy;
  con->incomplete     = (Rboolean)FALSE;
  con->canseek        = (Rboolean)!writable;
  con->canread        = (Rboolean)!writable;
  con->canwrite       = (Rboolean)writable;
  con->isopen         = (Rboolean)TRUE;
  con->blocking       = (Rboolean)TRUE;
  con->text           = (Rboolean)FALSE;
  con->UTF8out        = (Rboolean)TRUE;
  con->read           = &adf_file_read;
  con->write          = &adf_file_write;
  con->fgetc          = &adf_getc;
  con->fgetc_internal = &adf_getc;
  if (con->canseek)
    con->seek         = &adf_seek;
  else
      con->seek       = &adf_seek_null;

  adf_change_dir_internal(connection, cur_dir, vol_old);
  adf_register_file_con(ac, cpp11::as_cpp<int>(result));
  
  return result;
}

static size_t adf_file_read(void *target, size_t sz, size_t ni, Rconnection con) {
  adf_file_con_str *afc = (adf_file_con_str *) con->private_ptr;
  AdfFile *af = afc->adf_file;
  
  size_t request_size = sz*ni;
  int filesize = af->fileHdr->byteSize;
  int pos      = af->pos;
  size_t get_size = min(filesize - pos, (int)request_size);
  adfFileRead(af, get_size, (uint8_t *)target);
  return get_size;
}

static size_t adf_file_write(const void *ptr, size_t sz, size_t ni,
                             Rconnection con) {
  adf_file_con_str *afc = (adf_file_con_str *) con->private_ptr;
  AdfFile *af = afc->adf_file;
  
  size_t request_size = sz*ni;
  size_t got_size = adfFileWrite(af, request_size, (uint8_t *)ptr);
  return got_size;
}

static int adf_getc(Rconnection con) {
  int x = 0;
#ifdef WORDS_BIGENDIAN
  return adf_file_read(&x, 1, 1, con) ? BSWAP_32(x) : R_EOF;
#else
  return adf_file_read(&x, 1, 1, con) ? x : R_EOF;
#endif
}

void adf_fcon_destroy(Rconnection con) {
  auto get_connection = cpp11::package("base")["getConnection"];
  adf_file_con_str * afc = (adf_file_con_str *) con->private_ptr;
  Rconnection parent = R_GetConnection(get_connection(
    Rf_ScalarInteger(afc->parent_con)));
  adf_conn *ac_parent = (adf_conn *)parent->private_ptr;
  std::vector<int> * file_register = & ac_parent->file_connections;
  
  for (auto it = file_register->begin(); it < file_register->end(); it++) {
    Rconnection con_in = R_GetConnection(get_connection(Rf_ScalarInteger(*it)));
    if (con == con_in) {
      file_register->erase(it);
      break;
    }
  }
  
  AdfFile *af = afc->adf_file;
  
  adfFileClose(af);
  delete afc;
}

static double adf_seek_null(Rconnection con, double where, int origin, int rw) {
  return 0;
}

static double adf_seek(Rconnection con, double where, int origin, int rw) {

  // origin 1 = start;
  // origin 2 = current;
  // origin 3 = end of file;
  adf_file_con_str *afc = (adf_file_con_str *) con->private_ptr;
  AdfFile *af = afc->adf_file;
  
  int filesize = af->fileHdr->byteSize;
  int pos      = af->pos;
  
  switch(origin) {
  case 2: break;
  case 3: pos += (int)where; break;
  default: pos = (int)where;
  }
  if (pos < 0) pos = 0;
  if (pos > filesize) pos = filesize;
  adfFileSeek(af, pos);
  return (double)pos;
}

r_string adf_file_con_info(SEXP connection) {
  adf_file_con_test_class(connection);
  Rconnection con = R_GetConnection(connection);
  if (!con->isopen) return (r_string("Closed ADF file connection"));

  adf_file_con_str *afc = (adf_file_con_str *) con->private_ptr;
  AdfFile *af = afc->adf_file;
  AdfDevice *parent = af->volume->dev;

  std::string path = adf_entry_to_path_internal(
    parent, afc->vol_num, af->fileHdr->headerKey, TRUE);
  
  std::string access = "read only";
  if (con->canwrite) access = "writable";
  
  std::string result = "A " + access + " connection to virtual file:\n" + path;
  return r_string(result);
}

void adf_file_con_test_class(SEXP connection) {
  if (! Rf_inherits(connection, "adf_file_con"))
    Rf_error("Connection should inherit 'adf_file_con'.");
}
