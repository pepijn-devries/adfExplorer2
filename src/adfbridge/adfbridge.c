#include "adfbridge.h"

RETCODE adfMountDev2 ( FILE * f, const BOOL ro,
                       struct AdfDevice * dev )
{

  if (adfOpenDev2 ( f, ro, dev ) != RC_OK) return RC_ERROR;
  
  if ( ! dev ) return RC_ERROR;

  switch( dev->devType ) {
  case DEVTYPE_FLOPDD:
  case DEVTYPE_FLOPHD:
    if ( adfMountFlop ( dev ) != RC_OK ) return RC_ERROR;
    break;
    
  case DEVTYPE_HARDDISK:
    if (adfMountHdFile(dev) != RC_OK) return RC_ERROR;
    if (adfMountHdFileFix(dev) != RC_OK) return RC_ERROR;
    break;
    
  default:
    // unknown device
    return RC_ERROR;
  }
  
  return RC_OK;
}

RETCODE adfOpenDev2 ( FILE * f, const BOOL ro,
                   struct AdfDevice * dev)
{
  dev->readOnly = ro;
  dev->isNativeDev = FALSE;
  
  RETCODE rc;
  rc = adfInitDumpDevice2 ( dev, f, ro );
  if ( rc != RC_OK ) {
    free ( dev );
    return rc;
  }
  
  dev->devType = adfDevType ( dev );
  dev->nVol    = 0;
  dev->volList = NULL;
  
  return RC_OK;
}

RETCODE adfInitDumpDevice2 ( struct AdfDevice * const dev,
                             FILE *             f,
                             const BOOL         ro )
{
  dev->readOnly = ro;
  dev->fd = f;
  
  /* determines size */
  fseek ( dev->fd, 0, SEEK_END );
  dev->size = (uint32_t) ftell ( dev->fd );
  fseek ( dev->fd, 0, SEEK_SET );

  if ( dev->devType == DEVTYPE_FLOPDD ) {
    dev->sectors = 11;
    dev->heads = 2;
  } else if ( dev->devType == DEVTYPE_FLOPHD ) {
    dev->sectors = 22;
    dev->heads = 2;
  } else {
    dev->sectors = 1;
    dev->heads = 1;
  }
  dev->cylinders = dev->size / ( dev->sectors * dev->heads * 512 );
  
  return RC_OK;
}

void adfCloseDev2 ( struct AdfDevice * const dev )
{
  if ( ! dev )
    return;
  
  if ( dev->nVol > 0 ) {
    for ( int i = 0 ; i < dev->nVol ; i++ ) {
      free ( dev->volList[i]->volName );
      free ( dev->volList[i] );
    }
    free ( dev->volList );
    dev->nVol = 0;
  }
  
}

RETCODE adfMountHdFileFix ( struct AdfDevice * const dev )
{
  // Fix to make sure volname and blocksize are added to volume.
  struct AdfVolume *vol;
  struct bRootBlock root;
  char diskName[35];
  
  if ( ! dev )
    return RC_ERROR;
  if ( dev->nVol > 0 ) {
    for ( int i = 0 ; i < dev->nVol ; i++ ) {
      vol = dev->volList[i];
      if (vol == NULL) return RC_ERROR;
      vol->blockSize = 512;
      vol->dev = dev;
      
      if ( adfReadRootBlock ( vol, (uint32_t) vol->rootBlock, &root ) != RC_OK ) {
        return RC_ERROR;
      }
      memset(diskName, 0, 35);
      memcpy(diskName, root.diskName, root.nameLen);
      
      vol->volName = strdup(diskName);
      
    }
  }
  return RC_OK;
}

RETCODE updateBootSum ( struct AdfVolume * const vol ) {
  uint8_t buf[1024];
  RETCODE rc = RC_OK;
  
  rc = adfReadBlock (vol, 0, buf);
  if (rc == RC_OK) rc = adfReadBlock (vol, 1, buf + LOGICAL_BLOCK_SIZE);
  if (rc != RC_OK) return rc;
  
  uint32_t calc_checksum = adfBootSum(buf);
  swLong(buf+4, calc_checksum);
  
  rc = adfWriteBlock ( vol, 0, buf );
  if ( rc != RC_OK )
    return rc;
  
  rc = adfWriteBlock ( vol, 1, buf + 512 );
  return rc;
}