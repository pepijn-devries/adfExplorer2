#include <cpp11.hpp>
#include "open_adf.h"
using namespace cpp11;

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
  AdfContainer * ac = (AdfContainer *)con->private_ptr;
  
  if(con->isopen && strcmp(con->description, "stdin")) {
    auto getallcons     = package("base")["getAllConnections"];
    auto get_connection = cpp11::package("base")["getConnection"];
    auto closecon       = cpp11::package("base")["close"];
    
    writable::integers connections = integers(getallcons());
    // Check all connections and close those referring to this device
    for (int i = 0; i < connections.size(); i++) {
      SEXP test_con = get_connection(Rf_ScalarInteger(i));
      if (Rf_inherits(test_con, "adf_file_con")) {
        Rconnection fcon = R_GetConnection(test_con);
        AdfFile * af = (AdfFile * )fcon->private_ptr;
        // Test if current connection refers to the device being closed;
        if (af && af->volume->dev == ac->dev && fcon->isopen)
          closecon(test_con);
      }
    }

  }
  con->isopen = (Rboolean)false;
  
#ifdef Win32
  if(ac->fc->anon_file) unlink(ac->fc->name);
#endif
}

void adf_con_destroy(Rconnection con) {
  if (con->isopen) file_close(con);
  AdfContainer  * ac  = (AdfContainer *)con->private_ptr;
  AdfDevice * dev = ac->dev;
  adfCloseDev(dev); // This closes the adf file and frees all allocated mem for dev
  delete ac;
  return;
}

[[cpp11::register]]
SEXP open_adf_(std::string filename, bool write_protected) {
  AdfDevice * dev;
  dev = adfMountDev(filename.c_str(), write_protected);
  if (!dev) {
    dev = adfOpenDev(filename.c_str(), write_protected);
    if (! dev) Rf_error("Could not mount virtual device");
  }
  
  if (dev->nVol > 0) {
    for (int i = 0; i < dev->nVol; i++) {
      adfMount(dev, i, write_protected);
    }
  }
  
  Rconnection con;
  auto open_method = write_protected ? "rb" : "r+b";
  SEXP result = R_new_custom_connection(filename.c_str(), open_method, "adf_device", &con);
  
  AdfContainer * ac = (AdfContainer *)new AdfContainer;
  ac->currentVol    = (dev->nVol > 0) ? 0 : -1;
  ac->dev           = dev;
  con->private_ptr  = ac;
  con->read         = &null_read;
  con->write        = &null_write;
  con->close        = &file_close;
  con->open         = &file_open;
  con->destroy      = &adf_con_destroy;
  con->incomplete   = (Rboolean)false;
  con->canseek      = (Rboolean)false;
  con->canwrite     = (Rboolean)!write_protected;
  con->isopen       = (Rboolean)true;
  con->blocking     = (Rboolean)true;
  con->text         = (Rboolean)false;

  return result;
}
