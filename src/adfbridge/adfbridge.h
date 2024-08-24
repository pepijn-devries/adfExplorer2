#ifndef __BRIDGE_ADF__
#define __BRIDGE_ADF__

#include "adflib.h"
#include <string.h>

/*
 * This is a rewrite of the adflib function which takes 'FILE * f'
 * as input instead of const char * filename
 */

#ifdef __cplusplus
extern "C" {
#endif
  
RETCODE adfInitDumpDevice2 ( struct AdfDevice * const dev, FILE * f,
                             const BOOL ro );
RETCODE adfOpenDev2 ( FILE * f, const BOOL ro, struct AdfDevice * dev );
RETCODE adfMountDev2 ( FILE * f, const BOOL ro, struct AdfDevice * dev );
void adfCloseDev2 ( struct AdfDevice * const dev );
RETCODE adfMountHdFileFix ( struct AdfDevice * const dev );
RETCODE updateBootSum ( struct AdfVolume * const vol );

#ifdef __cplusplus
}
#endif

#endif /* __BRIDGE_ADF__ */