#ifndef __CONNECT_ADF__
#define __CONNECT_ADF__

#include <cpp11.hpp>
#include <vector>
#include "adflib.h"
#include "adfbridge/adfbridge.h"

#define class class_name
#define private private_ptr
/* the RConnection API is experimental and subject to change */
#include <R_ext/Connections.h>
#if ! defined(R_CONNECTIONS_VERSION) || R_CONNECTIONS_VERSION != 1
#error "Unsupported connections API version"
#endif
#undef class
#undef private

struct adf_file_con_str {
  AdfFile * adf_file;
  int vol_num;
  int parent_con;
};

struct fileconn {
  FILE *fp;
  long rpos, wpos;
  Rboolean last_was_write;
  Rboolean raw;
#ifdef Win32
  Rboolean anon_file;
  char name[PATH_MAX+1];
#endif
};

struct adf_conn {
  AdfDevice * dev;
  fileconn * fc;
// File connections registered to this 'adf_conn'.
// These need to be closed before 'adf_conn' is closed/destroyed
  std::vector<int> file_connections;
  int currentVol;
  adf_conn(): file_connections(0) {}
};

[[cpp11::register]] SEXP connect_adf_(cpp11::strings filename,
                                      bool write_protected);
[[cpp11::register]] cpp11::logicals adf_check_file_reg(SEXP con, int vol_num, int file_header);
AdfDevice * get_adf_dev(Rconnection con);
AdfDevice * get_adf_dev_internal(SEXP con);
int get_adf_vol(Rconnection con);
int get_adf_vol_internal(SEXP con);
void set_adf_vol(Rconnection con, int cur_vol);
void set_adf_vol_internal(SEXP con, int cur_vol);
void adf_register_file_con(adf_conn * ac, int con_id);
void check_adf_con (SEXP con);
void check_adf_name(std::string name);

#endif /* __CONNECT_ADF__ */