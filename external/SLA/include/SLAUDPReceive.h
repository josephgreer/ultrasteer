/*
 * Copyright (C)2007-2016 SightLine Applications Inc
 * SightLine Applications Library of signal, vision, and speech processing
 * http://www.sightlineapplications.com
 *------------------------------------------------------------------------*/

#pragma once

#include "sltypes.h"
#include <stdio.h>

// Packet Identifier (PID) - 
// Each table or elementary stream in a transport stream is identified by a unique 13-bit packet identifier (PID)
// All the PID’s constituting a program are listed in a Program Map Table (PMT), which has its own assigned PID.
// needs to be a parameter, but #define for now
//#define PROGRAM_MAP_TABLE_PID 0x42
//#define VIDEO_PID 0x44
//#define METADATA_PID 0x45
#define PROGRAM_MAP_TABLE_PID 0x10
#define VIDEO_PID             0x11
#define METADATA_PID          0x21
#define CUSTDATA_PID          0x22    // Default PID value for SLA private data.
#define CUSTDATA_TYPE         0x15    // Default stream type for SLA private data.
#define NULL_PACKET           0x1FFF  // used to form a constant bit rate stream. A Null packet is ignored by a receiver.

#define TSPacketSize 188  //!< Bytes

typedef enum {
  SLA_UDP_VIDEO_PROTOCOL_NONE        = -1,
  SLA_UDP_VIDEO_PROTOCOL_RTPMJPEG,
  SLA_UDP_VIDEO_PROTOCOL_RTPH264,
  SLA_UDP_VIDEO_PROTOCOL_RTPMP2H264,
  SLA_UDP_VIDEO_PROTOCOL_RTPMJPEGRAW,
  SLA_UDP_VIDEO_PROTOCOL_RTPMP2MPEG4,
  SLA_UDP_VIDEO_PROTOCOL_MPEG2,
  SLA_UDP_VIDEO_PROTOCOL_MPEG4,
  SLA_UDP_VIDEO_PROTOCOL_H264,
  SLA_UDP_VIDEO_PROTOCOL_END,

  SLA_UDP_VIDEO_PROTOCOL_KLV_METADATA,
  SLA_UDP_VIDEO_PROTOCOL_SLA_METADATA    // SightLine private metadata.
} SLAUdpVideoProtocol;

#define SLA_DG_STREAM_TYPE_MIN 0x88
#define SLA_DG_STREAM_TYPE_MAX 0x8f

typedef struct {
  u8 *buffer;       // Filled in by caller
  u32 maxBufferLen; // Filled in by caller
  u32 len;
  u8  PID;
  u8  missedPacket;
  u32 quality;
  s32 high;
  s32 wide;
  s32 type;
  s64 PTS;
  SLAUdpVideoProtocol streamType;
  // Set to 1 if UDP receiver determines that video+klv+sla_metadata at single PTS has been received
  int frameDataComplete;         
} SLA_COMPRESSED_FRAME;

typedef struct {
  s32 backedUp;  // Is receive buffer getting backed up?
} SLA_UDP_STATUS;

// Return opaque state structure
void *SLAInitUDPReceive(u32 nPackets, char *hostname, int port, 
                        bool useSlDemux=false // Use SLUDPReceive demux instead of FFMPEG internal version. Decoding SLALIB diag data works better with SLUDPReceive. 
                        );

void SLAReinitUDPReceive(void *_data, char *hostname, int port);
void SLADestroyUDPReceive(void *UDPReceiveData);

SLStatus SLAStartSavingUDP(void *_data, char *fname);
SLStatus SLAStopSavingUDP(void *_data);

// Fills in frame
// returns result of blocking call (SLA_ERROR implies shutdown request)
SLStatus SLADemuxNextFrame(void *UDPReceiveData, SLA_COMPRESSED_FRAME *frame, u32 timeout);

SLStatus SLAUDPStatus(void *UDPReceiveData, SLA_UDP_STATUS *status);

SLINLINE static bool SLAIsMetaDataProtocol(SLAUdpVideoProtocol prt) {
  return prt == SLA_UDP_VIDEO_PROTOCOL_KLV_METADATA || prt == SLA_UDP_VIDEO_PROTOCOL_SLA_METADATA;
}
