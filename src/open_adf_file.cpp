#include <cpp11.hpp>
#include "open_adf.h"
#include "dev_info.h"
#include "adf_file_info.h"
#include "open_adf_file.h"
using namespace cpp11;

void adf_fcon_destroy(Rconnection con) {
  AdfFile * af = (AdfFile *) con->private_ptr;
  adfFileClose(af);
  con->private_ptr = NULL;
}

static double adf_seek_null(Rconnection con, double where, int origin, int rw) {
  return 0;
}

static double adf_seek(Rconnection con, double where, int origin, int rw) {
  
  // origin 1 = start;
  // origin 2 = current;
  // origin 3 = end of file;
  AdfFile *af = (AdfFile *) con->private_ptr;
  
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

static size_t adf_file_read(void *target, size_t sz, size_t ni, Rconnection con) {
  AdfFile *af = (AdfFile *) con->private_ptr;
  
  size_t request_size = sz*ni;
  int filesize = af->fileHdr->byteSize;
  int pos      = af->pos;
  size_t get_size = min(filesize - pos, (int)request_size);
  adfFileRead(af, get_size, (uint8_t *)target);
  return get_size;
}

static size_t adf_file_write(const void *ptr, size_t sz, size_t ni,
                             Rconnection con) {
  AdfFile *af = (AdfFile *) con->private_ptr;
  
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

int get_adf_file_volnum(AdfFile * adf_file) {
  AdfVolume * vol = adf_file->volume;
  AdfDevice * dev = vol->dev;
  if (dev->nVol <= 0) return -1;
  int result = -1;
  for (int i = 0; i < dev->nVol; i++) {
    AdfVolume * test_vol = dev->volList[i];

    if (test_vol == vol) {
      result = i;
      break;
    }
  }
  return result;
}

bool adf_check_file_state(AdfDevice *dev, int vol, SECTNUM sect) {
  auto getallcons = package("base")["getAllConnections"];
  auto get_connection = cpp11::package("base")["getConnection"];
  
  writable::integers connections = integers(getallcons());
  // Check all connections and test if requested file is already opened
  for (int i = 0; i < connections.size(); i++) {
    SEXP test_con = get_connection(Rf_ScalarInteger(i));
    if (Rf_inherits(test_con, "adf_file_con")) {
      Rconnection con = R_GetConnection(test_con);
      AdfFile * af = (AdfFile * )con->private_ptr;
      if (af && con->isopen) {
        int testvol = get_adf_file_volnum(af);
        if (dev == af->volume->dev && testvol == vol && af->fileHdr->headerKey == sect)
          return true;
      }
    }
  }
  return false;
}

[[cpp11::register]]
SEXP adf_file_con_(SEXP connection, std::string filename, bool writable) {
  AdfDevice * dev = get_adf_dev(connection);
  Rconnection con_in = R_GetConnection(connection);
  if (!con_in->canwrite && writable)
    Rf_error("Cannot open a writable connection from a write-protected disk");
  int mode = ADF_FI_EXPECT_FILE | ADF_FI_EXPECT_VALID_CHECKSUM;
  if (!writable) mode = mode | ADF_FI_EXPECT_EXIST | ADF_FI_THROW_ERROR;
  
  list entry_pos = adf_path_to_entry(connection, filename, mode);
  int vol_num  = integers(entry_pos["volume"]).at(0);
  SECTNUM sect = integers(entry_pos["sector"]).at(0);
  SECTNUM parent = integers(entry_pos["parent"]).at(0);
  
  check_volume_number(dev, vol_num);
  bool file_check = adf_check_file_state(dev, vol_num, sect);
  if (file_check)
    Rf_error("Can only open 1 connection per file on a virtual device");

  auto vol = dev->volList[vol_num];
  int vol_old = get_adf_vol(connection);
  SECTNUM cur_dir = vol->curDirPtr;
  
  adf_change_dir_internal(connection, parent, vol_num);
  
  AdfFileMode fmode = writable ? ADF_FILE_MODE_WRITE : ADF_FILE_MODE_READ;
  std::string fns;
  
  if (writable && sect == -1) {
    // The remainder of the path that does not exist:
    fns = (std::string)(cpp11::strings(entry_pos["remainder"]).at(0));
    check_adf_name(fns);
    const char * fn2 = fns.c_str();
    auto fhead = new bFileHeaderBlock;
    RETCODE fcret = adfCreateFile(vol, parent, fn2, fhead);
    delete(fhead);
    if (fcret != RC_OK) Rf_error("Failed to create file for writing.");
  } else {
    fns = (std::string)cpp11::strings(entry_pos["name"]).at(0);
  }
  const char* fn = fns.c_str();
  AdfFile * adf_file = adfFileOpen (vol, fn, fmode);
  if (!adf_file) Rf_error("Failed to open file connection");
  
  Rconnection con;
  SEXP result = R_new_custom_connection(
    fn, "rb", "adf_file_con", &con);
  
  con->private_ptr    = adf_file;
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
    con->seek         = &adf_seek_null;
  
  adf_change_dir_internal(connection, cur_dir, vol_old);
  
  return result;
}

void adf_file_con_test_class(SEXP connection) {
  if (! Rf_inherits(connection, "adf_file_con"))
    Rf_error("Connection should inherit 'adf_file_con'.");
}

[[cpp11::register]]
std::string adf_file_con_info(SEXP connection) {
  adf_file_con_test_class(connection);
  Rconnection con = R_GetConnection(connection);
  if (!con->isopen) return (r_string("Closed ADF file connection"));
  
  AdfFile *af = (AdfFile *) con->private_ptr;
  AdfDevice *parent = af->volume->dev;
  int vol_num = get_adf_file_volnum(af);
  
  std::string path = adf_entry_to_path_internal(
    parent, vol_num, af->fileHdr->headerKey, TRUE);
  
  std::string access = "read only";
  if (con->canwrite) access = "writable";
  
  std::string result = "A " + access + " connection to virtual file:\n" + path;
  return result;
}
