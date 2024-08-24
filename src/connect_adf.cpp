#include "connect_adf.h"

static size_t NORET null_read(void *ptr, size_t size, size_t nitems,
                              Rconnection con) {
  Rf_error("'read' not enabled for this connection");
}

static size_t NORET null_write(const void *ptr, size_t size, size_t nitems,
                               Rconnection con) {
  Rf_error("'write' not enabled for this connection");
}

static Rboolean file_open(Rconnection con)
{
  Rf_error("Cannot reopen this connection");
  return (Rboolean)false;
}

static void file_close(Rconnection con)
{
  adf_conn * ac = (adf_conn *)con->private_ptr;

  if(con->isopen && strcmp(con->description, "stdin")) {
    auto get_connection = cpp11::package("base")["getConnection"];
    std::vector<int> * fc = & ac->file_connections;
    // close all registered file_connections:
    for (int & i : * fc) {
      Rconnection con_in = R_GetConnection(get_connection(Rf_ScalarInteger(i)));
      con_in->close(con_in);
    }

    fc->clear();
    con->status = fclose(ac->fc->fp);
  }
  con->isopen = (Rboolean)false;
  
#ifdef Win32
  if(ac->fc->anon_file) unlink(ac->fc->name);
#endif
}

void adf_con_destroy(Rconnection con) {
  if (con->isopen) file_close(con);
  adf_conn  * ac  = (adf_conn *)con->private_ptr;
  AdfDevice * dev = ac->dev;
  adfCloseDev2(dev);
  delete dev;
  delete ac;
  return;
}

SEXP connect_adf_(cpp11::strings filename, bool write_protected = true) {
  auto connect     = cpp11::package("base")["file"];
  std::string   fn = (std::string)filename[0];
  auto open_method = write_protected ? "rb" : "r+b";
  SEXP scon        = connect(fn, open_method);
  Rconnection  con = R_GetConnection(scon);
  fileconn  * fcon = (fileconn *)con->private_ptr;
  FILE         * f = (FILE *)fcon->fp;
  AdfDevice  * dev = (AdfDevice *)new AdfDevice;
  adfMountDev2(f, write_protected, dev);
  for (int i = 0; i < dev->nVol; i++) {
    adfMount(dev, i, write_protected);
  }
  
  adf_conn   * ac  = (adf_conn *)new adf_conn;
  ac->currentVol   = (dev->nVol > 0) ? 0 : -1;
  ac->fc           = fcon;
  ac->dev          = dev;
  
  con->private_ptr = ac;
  con->read        = &null_read;
  con->write       = &null_write;
  con->close       = &file_close;
  con->open        = &file_open;
  con->destroy     = &adf_con_destroy;

  cpp11::sexp sscon = scon;
  cpp11::writable::strings cls = cpp11::strings(sscon.attr("class"));
  cls.insert(0, "adf_device");
  sscon.attr("class") = cls;

  return scon;
}

AdfDevice * get_adf_dev(Rconnection con) {
  adf_conn *ac = (adf_conn *)con->private_ptr;
  return ac->dev;
}

AdfDevice * get_adf_dev_internal(SEXP con) {
  check_adf_con(con);
  Rconnection con2 = R_GetConnection(con);
  
  return (get_adf_dev(con2));
}

int get_adf_vol(Rconnection con) {
  adf_conn *ac = (adf_conn *)con->private_ptr;
  return ac->currentVol;
}

int get_adf_vol_internal(SEXP con) {
  check_adf_con(con);
  Rconnection con2 = R_GetConnection(con);
  return get_adf_vol(con2);
}

void set_adf_vol(Rconnection con, int cur_vol) {
  adf_conn *ac = (adf_conn *)con->private_ptr;
  ac->currentVol = cur_vol;
}

void set_adf_vol_internal(SEXP con, int cur_vol) {
  check_adf_con(con);
  Rconnection con2 = R_GetConnection(con);
  return set_adf_vol(con2, cur_vol);
}

void adf_register_file_con(adf_conn * ac, int con_id) {
  std::vector<int> * fcs = &ac->file_connections;
  fcs->push_back(con_id);
  return;
}

[[cpp11::register]] SEXP adf_check_file_reg(SEXP con, int file_header) {
  auto get_connection = cpp11::package("base")["getConnection"];
  Rconnection con2 = R_GetConnection(con);
  adf_conn *ac = (adf_conn *)con2->private_ptr;
  std::vector<int> * fc = & ac->file_connections;
  for (int & i : * fc) {
    Rprintf("i: %i\n", i);
    Rconnection con_in = R_GetConnection(get_connection(Rf_ScalarInteger(i)));

    if (con_in->isopen) {
      auto * afc  = (adf_file_con_str *)con_in->private_ptr;
      Rprintf("i: %i vol %i file header %i\n", i,
              ac->currentVol,
              afc->adf_file->fileHdr->headerKey);
    } else {
      Rprintf("Not open\n");
    }
  }

  cpp11::writable::integers result((R_xlen_t)1);
  result.at(0) = (int)fc->size();
  return(result);
}

void check_adf_con (SEXP con) {
  auto inherits = cpp11::package("base")["inherits"];
  if (! cpp11::logicals(inherits(con, "adf_device")).at(0))
    Rf_error("Connection should inherit 'adf_device'.");
}

void check_adf_name(std::string name) {
  if (name.find(':') != std::string::npos ||
      name.find('/') != std::string::npos)
    Rf_error("File, directory or volume names cannot contain `/` or `:`.");
}
