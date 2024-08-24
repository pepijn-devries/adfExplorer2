#ifndef __FILE_CONNECT_ADF__
#define __FILE_CONNECT_ADF__
#pragma once

#include "file_info_adf.h"

#define R_EOF -1

/* Define BSWAP_32 on Big Endian systems */
#ifdef WORDS_BIGENDIAN
#if (defined(__sun) && defined(__SVR4))
#include <sys/byteorder.h>
#elif (defined(__APPLE__) && defined(__ppc__) || defined(__ppc64__))
#include <libkern/OSByteOrder.h>
#define BSWAP_32 OSSwapInt32
#elif (defined(__OpenBSD__))
#define BSWAP_32(x) swap32(x)
#elif (defined(__NetBSD__))
#include <sys/types.h>
#include <machine/bswap.h>
#define BSWAP_32(x) bswap32(x)
#elif (defined(__GLIBC__))
#include <byteswap.h>
#define BSWAP_32(x) bswap_32(x)
#elif (defined(_AIX))
#define BSWAP_32(x) __builtin_bswap32(x)
#endif
#endif

SEXP adf_file_con_(SEXP connection, cpp11::strings filename, bool writable);
static size_t adf_file_read(void *target, size_t sz, size_t ni, Rconnection con);
static size_t adf_file_write(const void *ptr, size_t sz, size_t ni, Rconnection con);
static int adf_getc(Rconnection con);
void adf_fcon_destroy(Rconnection con);
static double adf_seek(Rconnection con, double where, int origin, int rw);
static double adf_seek_null(Rconnection con, double where, int origin, int rw);
[[cpp11::register]] r_string adf_file_con_info(SEXP connection);
void adf_file_con_test_class(SEXP connection);

#endif /* __FILECONNECT_ADF__ */
