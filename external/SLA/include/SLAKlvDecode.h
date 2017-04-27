/*
 * Copyright (C)2007-2016 SightLine Applications Inc
 * SightLine Applications Library of signal, vision, and speech processing
 * http://www.sightlineapplications.com
 *------------------------------------------------------------------------*/

#pragma once

#include "sltypes.h"
#include "SLAKlv.h"


// Implements UAS Datalink Local Set MISB ST 0601.7
#define UAS_LOCAL_SET_VERSION 7

// Implements Security Local Set MISB ST 0102.10
#define SECURITY_METADATA_VERSION 0x000A

// Implements Video Moving Target Indicator and Track Local Set MISB ST 0903.3
#define VMTI_VERSION 3

#define MISSION_ID_LENGTH 127
#define PLATFORM_TAIL_NUMBER_LENGTH 127
#define IMAGE_COORDINATE_SYSTEM_LENGTH 127
#define PLATFORM_DESIGNATION_LENGTH 127
#define IMAGE_SOURCE_SENSOR_LENGTH 127
#define CLASSIFYING_COUNTRY_LENGTH 6
#define SCISHI_INFORMATION_LENGTH 40
#define CAVEATS_LENGTH 32
#define RELEASING_INSTRUCTIONS_LENGTH 40
#define OBJECT_COUNTRY_CODES_LENGTH 40
#define PLATFORM_CALL_SIGN_LENGTH 127
#define MIIS_CORE_IDENTIFIER_LENGTH 127


u16 Checksum(const u8 *src, u32 len);

//u32 ReadBer(const u8 *buffer, u16 *length);
s32 ReadElement(const u8 *buffer, KLVData *dest, const u8 *csSrc, u32 len);

// Scan buffer for 16-byte universal key.
// buf points to the encoded bytestream, len indicates that stream's size in bytes
// (len should always be at least 19 to ensure room for a 16-byte key and a 3-byte len)
// headerStart indicates the buffer offset to the 16-byte key
// bytesRead indicates number of bytes in buffer that were processed (pad plus key plus length elements)
// klvLen indicates number of bytes remaining in klv data frame
// returns 1 if successful, 0 otherwise
s32 ReadKeyLength(const u8* buf, u16 len, u16 *bytesRead, u16 *klvLen, u16 *headerStart = 0);
s32 ReadKlvElements(KLVData *klv, const u8* buf, u16 ldsOffset, u16 klvLen);

s32 ReadKlvFrame(KLVData *klv, const u8* buf, u16 len, s32 bufStartOffset = 5);
void SLCopyChangedKLV(KLVData *d, KLVData *s);

