#pragma once
#ifndef __OPEN_ADF__
#define __OPEN_ADF__

#include <vector>
#include "adflib/adflib.h"

/* R connection definitions: */
#define class class_name
#define private private_ptr
/* the RConnection API is experimental and subject to change */
#include <R_ext/Connections.h>
#if ! defined(R_CONNECTIONS_VERSION) || R_CONNECTIONS_VERSION != 1
#error "Unsupported connections API version"
#endif
#undef class
#undef private

struct AdfContainer {
  AdfDevice * dev;
  int currentVol;
};

#endif /* __OPEN_ADF__ */