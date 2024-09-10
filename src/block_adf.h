#ifndef __BLOCK_ADF__
#define __BLOCK_ADF__
#pragma once

#include "dev_info.h"
using namespace cpp11;

r_string headerKey_to_str(int headerKey);
r_string secType_to_str(int secType);
SEXP dmt_to_POSIXct(int days, int minutes, int ticks);
logicals access_from_int(int access);
list interpret_root_header_internal(AdfDevice *dev, int vol_num);
list interpret_file_header_internal(AdfDevice *dev, int vol_num, int sectnum);
list interpret_dir_header_internal(AdfDevice *dev, int vol_num, int sectnum);
[[cpp11::register]] SEXP read_adf_block_(SEXP connection, int sector);
[[cpp11::register]] SEXP write_adf_block_(SEXP connection, int sector, raws block);
[[cpp11::register]] list interpret_root_header(SEXP connection, int vol_num);
[[cpp11::register]] list interpret_file_header(SEXP connection, int vol_num, int sectnum);
[[cpp11::register]] list interpret_dir_header(SEXP connection, int vol_num, int sectnum);
raws adf_bootable_code(void);

#endif /* __BLOCK_ADF__ */