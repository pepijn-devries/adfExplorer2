#ifndef __DEV_FORMAT_ADF__
#define __DEV_FORMAT_ADF__

#include "connect_adf.h"

[[cpp11::register]] SEXP adf_dev_format(
    SEXP connection, SEXP name,
    cpp11::logicals ffs, cpp11::logicals intl, cpp11::logicals dircache,
    cpp11::logicals bootable);

#endif