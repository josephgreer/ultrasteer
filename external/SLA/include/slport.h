/*
 * Copyright (C)2007-2010 SightLine Applications LLC
 * SightLine Applications Library of signal, vision, and speech processing
 * http://www.sightlineapplication.com
 *
 *------------------------------------------------------------------------*/
#pragma once

#include "sltypes.h"

#ifndef _TMS320C6X
#pragma pack(push)
#pragma pack(1)
#endif

#define PORT_TIMEOUT_FOREVER -1  //0xFFFFFFFF?
class SLPort {
public:
  // Read up to maxLen bytes into data. Try for up to timeout ms (-1 => forever)
  // Returns number of bytes read, or -1 for error
  virtual s32 Read(void *data, u32 maxLen, s32 timeout) = 0;

  // Write dataLen bytes to port.
  // Returns number of bytes written, or -1 for error
  virtual s32 Write(const void *data, u32 dataLen) = 0;

  // Close port
  // Returns -1 for error, 0 for success
  virtual s32 Close() = 0;
};

#ifndef _TMS320C6X
#pragma pack(pop)
#endif
