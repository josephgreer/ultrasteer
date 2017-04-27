/*
 * Copyright (C)2007-2016 SightLine Applications, Inc
 * SightLine Applications Library of signal, vision, and speech processing
 *                http://www.sightlineapplications.com
 * Confidential & Proprietary
 * See the SightLine Applications Native Communications Protocol document 
 * for additional information.
 * http://sightlineapplications.com/docs/IDD-SLA-Protocol.pdf
 *------------------------------------------------------------------------*/
#pragma once
// This File is released to customers.  Please do not add includes.

/*! 
  * \section SightLine Applications Native Communication Protocol
  * Implements the correct data packing when generating outbound packets.
  * Not all commands are supported on all versions of the SightLine Hardware.
  *
  * Example:
  * \code
  * #include "slfip.h"
  * u8 buffer[MAX_SLFIP_PACKET] = {0}
  * u8 dataLen = 0;
  *
  * MyOpenSerialPort(COM1, 57600, 8, 1, NO_HW_HANDSHAKE);
  *
  * dataLen = SLFIPGetVersionNumber(buffer);
  * MySerialPortWrite(buffer, dataLen);
  *
  * dataLen = SLFIPStartTracking(buffer, 320, 240, 1);
  * MySerialPortWrite(buffer, dataLen);
  * 
  **
  */

#include "sltypes.h"

#ifndef _TMS320C6X
#pragma pack(push)
#pragma pack(1)
#endif

/*!@defgroup SLFIP Native Communication Protocol
 */
//@{

/*! Byte offsets 
 */
#define SLFIP_OFFSET_HDR0   0
#define SLFIP_OFFSET_HDR1   1
#define SLFIP_OFFSET_LENGTH 2
#define SLFIP_OFFSET_TYPE   3

typedef u8 * SLPacketType;

// Payload length of longest packet plus checksum
#if 1 // 20130408 TT: Since now we support 2 bytes length field, the length of a packet with a single byte length field must be smaller.
#define MAX_SLFIP_PACKET      130 //!< Maximum number of bytes in a single packet
#define MAX_SLFIP_PAYLOAD     126 //!< Maximum value that the length byte can hold (1 byte for checksum)
#else
#define MAX_SLFIP_PACKET      258 //!< Maximum number of bytes in a single packet
#define MAX_SLFIP_PAYLOAD     254 //!< Maximum value that the length byte can hold (1 byte in reserve)
#endif
// Extended FIP pacekt (bit7 of the first LEN field == 1)
#define MAX_SLFIPEX_PACKET   (1024*16) //!< Maximum number of bytes in a single packet
                                       //!< NOTE:  Match with MAX_READ_BUFFER_SIZE in slsock.h
#define MAX_SLFIPEX_PAYLOAD  (MAX_SLFIPEX_PACKET-5) //!< Maximum value that the length byte can hold (-5 = header(2bytes)+lengthField+chksum)
#define MAX_SLFIPEX_BYTEMASK  (0x80) //!< Maximum length for a single byte length field for FIPEX.

#define MAX_SLFIP_NUM_FILES  1024 //!< Maximum number of files to return from get directory contents

#define SLFIP_TO_BOARD_PORT     14001 //!< UDP port for sending fip to the video processing board from external programs
#define SLFIP_FROM_BOARD_PORT   14002 //!< UDP port for receiving fip from the video processing board
#define SLFIP_TO_BOARD_PORT2    14003 //!< UDP port for sending fip command from other programs(except sla panel) to board.

#define SL_RTP_RTSP_VIDEO_PORT_NET0  8008  //!< UDP port for receiving RTP stream from Net0 on VideoTrack to RTSP Server
#define SL_RTP_RTSP_VIDEO_PORT_NET1  8010  //!< UDP port for receiving RTP stream from Net1 on VideoTrack to RTSP Server

#define SL_RTP_VIDEO_PORT 5004 //!< Default port for mjpeg network display video (@see SLFIPSetNetworkDisplay)
#define SL_H264_VIDEO_PORT 15004 //!< Default port for h264 network display video (@see SLFIPSetNetworkDisplay)
#define SLWatchdogPort     52000 //!< Watchdog timer port for diagnostics information

#define SL_MAX_NUM_ALIGNMENTS 5 //!< number of alignment sets for different zoom levels (@see SLFIPSetMultipleAlignment)

enum { // Never change these!
  SLFIP_VER_BOARD_TYPE     = 2017007,  // Never change this: version which supports BoardType in VersionNumber (2.17.07).
  SLFIP_VER_3000_SET_VIDEO = 2021001,  // 3000 Set Video Mode command change (2.21.01).
  SLFIP_VER_3000_CMD_CAM   = 2021004,  // 3000 Added Set Command Camera (2.21.04).
  SLFIP_VER_dummy
};

// Macro to get a value from version numbers.
#define SLFIP_VER_VAL(maj,min,rev)        (u32)((maj*1000000 + min*1000 + rev))
#define SLFIP_VER_NEWER(maj,min,rev,old)  (SLFIP_VER_VAL(maj,min,rev) >= old)

typedef struct {
  u8 header1;
  u8 header2;
  u8 length;
  u8 type;
} SLFIPHeader;

#define MIN_FIP_PACKET_SIZE   5  //<! HEADER + HEADER + LENGTH + TYPE + CHECKSUM

typedef enum {
  SLFIP_ACK = 1,
  SLFIP_NAK = 2
} SLFIP_ACK_NAK;

/*! List of Commands and Replies
 */
typedef enum SLFIP {
  // Header signature byte 1
  HeaderByte1 = 0x51,
  HeaderByte2 = 0xAC,

  None = -1,

  // Commands
  FirstFipNumber                                    = 0x00,
  GetVersionNumber                                  = 0x00,
  ResetAllParameters                                = 0x01,
  GetConfiguration                                  = 0x01, // Used as ID in Get Parameters Function(0x28) and its response.
  SetStabilizationParameters                        = 0x02,
  GetStabilizationParameters                        = 0x03,
  ResetStabilizationMode                            = 0x04,
  ResetStabilizationParameters                      = 0x04,
  ModifyTracking                                    = 0x05,
  SetOverlayMode                                    = 0x06,
  GetOverlayMode                                    = 0x07,
  StartTracking                                     = 0x08,
  StopTracking                                      = 0x09,
  NudgeTrackingCoordinate                           = 0x0A,
  SetCoordinateReportingMode                        = 0x0B,
  CoordinateReportingMode                           = 0x0B, // Alias
  SetTrackingParameters                             = 0x0C,
  GetTrackingParameters                             = 0x0D,
  SetRegistrationParameters                         = 0x0E,
  GetRegistrationParameters                         = 0x0F,
  SetVideoParameters                                = 0x10,
  SetAVideoParameters                               = SetVideoParameters, // Deprecated, use SetVideoParameters
  GetVideoParameters                                = 0x11,
  GetAVideoParameters                               = GetVideoParameters, // Deprecated, use GetVideoParameters
  SetStabilizationBias                              = 0x12,
  SetMetadataValues                                 = 0x13,
  MetadataStaticValues                              = 0x14,
  SetMetadataFrameValues                            = 0x15,
  SetDisplayParameters                              = 0x16,
  SetDisplayRotation                                = SetDisplayParameters,  // Deprecated, use SetDisplayParameters
  ModifyTrackIndex                                  = 0x17,
  SetADCParameters                                  = 0x18,
  GetADCParameters                                  = 0x19,
  SetEthernetVideoParameters                        = 0x1A,
  GetEthernetVideoParameters                        = 0x1B,
  SetNetworkParameters                              = 0x1C,
  GetNetworkParameters                              = 0x1D,
  SetSDRecordingParameters                          = 0x1E,
  SetVideoMode                                      = 0x1F,
  GetVideoMode                                      = 0x20,
  SetVideoEnhancementParameters                     = 0x21,
  GetVideoEnhancementParameters                     = 0x22,
  SetH264Parameters                                 = 0x23,
  GetH264Parameters                                 = 0x24,
  SaveParameters                                    = 0x25,
  SetRGB565Conversion                               = 0x26,
  CurrentPrimaryTrackIndex                          = 0x27,
  GetParameters                                     = 0x28,
  SetEthernetDisplayParameters                      = 0x29,
  SetDisplayAdjustments                             = 0x2A,
  SendStitchParams                                  = 0x2B,
  SetStitchParameters                               = 0x2B, // Alias
  GetStitchParams                                   = 0x2C,
  GetStitchParameters                               = 0x2C, // Alias
  SetMovingTargetDetectionParameters                = 0x2D,
  SetDetectionParameters                            = 0x2D, // Alias
  GetMovingTargetDetectionParameters                = 0x2E,
  GetDetectionParameters                            = 0x2E, // Alias
  SendBlendParams                                   = 0x2F,
  SetBlendParameters                                = 0x2F, // Alias
  GetBlendParams                                    = 0x30,
  GetBlendParameters                                = 0x30, // Alias
  GetImageSize                                      = 0x31,
  DesignateSelectedTrackPrimary                     = 0x32,
  ShiftSelectedTrack                                = 0x33,
  GetPrimaryTrackIndex                              = 0x34,   
  SetNuc                                            = 0x35,
  GetNuc                                            = 0x36,
  SetAcqParams                                      = 0x37,
  SetAcquisitionParameters                          = 0x37, // Alias 
  GetAcqParams                                      = 0x38,
  GetAcquisitionParameters                          = 0x38, // Alias
  GetEthernetDisplayParameters                      = 0x39,
  GetDisplayParameters                              = 0x3A,
  GetDisplayRotation                                = GetDisplayParameters, // Deprecated, use GetDisplayParameters
  DrawObject                                        = 0x3B,
  StopSelectedTrack                                 = 0x3C,
  CommandPassThrough                                = 0x3D,
  ConfigureCommunicationsPort                       = 0x3E,
  SetSerialPassthrough                              = 0x3E, // Alias
  GetPortConfig                                     = 0x3F,
  GetSerialPassthrough                              = 0x3F, // Alias
  VersionNumber                                     = 0x40,
  CurrentStabilizationMode                          = 0x41,
  CurrentStabilizationParameters                    = 0x41, // Alias
  CurrentOverlayMode                                = 0x42,
  TrackingPosition                                  = 0x43,
  CurrentTrackingParameters                         = 0x44,
  CurrentRegistrationParameters                     = 0x45,
  CurrentVideoParameters                            = 0x46,
  CurrentAVideoParameters                           = CurrentVideoParameters, // Deprecated, use CurrentVideoParameters
  CurrentADCParameters                              = 0x47,
  CurrentEthernetVideoParameters                    = 0x48,
  CurrentNetworkParameters                          = 0x49,
  CurrentVideoEnhancementParameters                 = 0x4A,
  CurrentVideoModeParameters                        = 0x4B,
  CurrentStitchParameters                           = 0x4C,
  CurrentBlendParameters                            = 0x4D,
  CurrentImageSize                                  = 0x4E,
  CurrentAcqParams                                  = 0x4F,
  CurrentAcquisitionParameters                      = 0x4F, // Alias
  GetHardwareID                                     = 0x50,
  TrackingPositions                                 = 0x51,
  CurrentEthernetDisplayParameters                  = 0x52,
  CurrentPortConfiguration                          = 0x53,
  CurrentMovingTargetDetectionParameters            = 0x54,
  CurrentDetectionParameters                        = 0x54,
  FocusStats                                        = 0x55,
  CurrentH264Parameters                             = 0x56,
  CurrentDisplayParameters                          = 0x57,
  CurrentDisplayRotation                            = CurrentDisplayParameters, // Deprecated, use CurrentDisplayParameters
  CurrentSDCardRecordingStatus                      = 0x58,
  CurrentSDCardDirectoryInfo                        = 0x59,
  SendTraceStr                                      = 0x5A,
  SetCommandCamera                                  = 0x5B, // was SendMTIDebugInfo
  CommandCamera                                     = 0x5B, // Alias
  SetDisplayAngle                                   = 0x5C,
  DisplayAngle                                      = 0x5C, // Alias
  CurrentSnapShot                                   = 0x5D,
  SetSnapShot                                       = 0x5E,
  GetSnapShot                                       = 0x5F,
  DoSnapShot                                        = 0x60,
  SetKlvData                                        = 0x61,
  SetMetadataRate                                   = 0x62,
  SetSystemType                                     = 0x63,  // was SetDualBoardMode
  SetTelemetryDestination                           = 0x64,  // was GetDualBoardMode
  CurrentSystemType                                 = 0x65,  // was CurrentDualBoardMode
  NetworkList                                       = 0x66,
  GetNetworkList                                    = 0x66,  // Alias
  CurrentNetworkList                                = 0x67,
  CurrentOverlayObjectsIds                          = 0x68, // returns the IDs of displayed objects.
  SetParameterBlock                                 = 0x69,
  ParameterBlock                                    = 0x6A,
  CurrentOverlayObjectParams                        = 0x6B, // all the data associated with a displayed object
  CurrentOverlayObjectParameters                    = 0x6B, // Alias
  SetLensMode                                       = 0x6C,
  CurrentLensStatus                                 = 0x6D,
  SetLensParams                                     = 0x6E,
  SetLensParameters                                 = 0x6E, // Alias
  CurrentLensParams                                 = 0x6F,
  CurrentLensParameters                             = 0x6F, // Alias
  SetDigCamParams                                   = 0x70,
  SetDigitalCameraParameters                        = 0x70, // Alias
  CurrentDigCamParams                               = 0x71,
  CurrentDigitalCameraParameters                    = 0x71, // Alias
  SetUserPalette                                    = 0x72,
  CurrentUserPalette                                = 0x73,
  GetUserPalette                                    = 0x73, // Alias
  SetMultipleAlignment                              = 0x74,
  CurrentMultipleAlignment                          = 0x75,
  GetMultipleAlignment                              = 0x75, // Alias
  SetAdvancedMoTDetParameters                       = 0x76,
  SetAdvancedDetectionParameters                    = 0x76, // Alias
  CurrentAdvancedMoTDetParameters                   = 0x77,
  CurrentAdvancedDetectionParameters                = 0x77, // Alias
  GetAdvancedDetectionParameters                    = 0x77, // Alias
  TrackingBoxPixelStats                             = 0x78,
  DirectoryStatisticsReply                          = 0x79,
  CurrentStabilizationBias                          = 0x7A,
  SetAdvancedCaptureParams                          = 0x7B,
  AdvancedCaptureParameters                         = 0x7B, // Alias
  SetMotDetRegionOfInterestParams                   = 0x7C,
  SetDetectionRegionOfInterestParameters            = 0x7C, // Alias
  CurrentMotDetRegionOfInterestParams               = 0x7D,
  CurrentDetectionRegionOfInterestParameters        = 0x7D, // Alias
  GetDetectionRegionOfInterestParameters            = 0x7D, // Alias
  AnalyzeRenderSync                                 = 0x7E,
  UserWarningLevel                                  = 0x7F,
  SetUserWarningLevel                               = 0x7F, // Alias
  SystemStatusMode                                  = 0x80,
  LandingAid                                        = 0x81,
  HtCamChange                                       = 0x82,
  LandingPosition                                   = 0x83,
  MetadataOverlays                                  = 0x84,
  SetGeneric                                        = 0x85,
  UserWarningMessage                                = 0x86,
  SystemStatusMessage                               = 0x87,
  DetailedTimingMessage                             = 0x88,
  AppendedMetadata                                  = 0x89,
  FrameIndex                                        = 0x8A,
  CurrentMetadataValues                             = 0x8B,
  CurrentMetadataFrameValues                        = 0x8C,
  CurrentMetadataRate                               = 0x8D,
  CurrentConfiguration                              = 0x8E,
  ExternalProgram                                   = 0x8F,
  StreamingControl                                  = 0x90,
  DigiVidParseParams                                = 0x91,
  
  LastFipNumber,                                            // NOTE: No more individual "getter" functions are needed @see GetParameters
                                                            // However, different codes for "Set*" and "Current*" messages ARE needed!
  FIP_EXTENDED_TYPE                                 = 0xFF, // Will need to add extended type handling beyond this point
  NumberOfFipMessages = LastFipNumber-FirstFipNumber
} SLA_FIP_RESPONSES;

/*! return a human readable name for a fip enum above.
 * @param enumVal packet type e.g. 0x6F (111)
 * @return the packet type name  e.g. "CurrentLensParams".
 */
const char * SLCommandString( u8 enumVal );

typedef enum {
  // Header signature byte 1
  HeaderByteMTI1 = 0x51,
  HeaderByteMTI2 = 0xAE, 

  FirstCommandNumberMTI = 0,
  SendMTIConfig = 1,
  LastCommandNumberMTI,
  NumberOfCommandsMTI = LastCommandNumberMTI-FirstCommandNumberMTI
} SLA_FIP_MTI_COMMANDS;

/*! Calculates the FIP packet checksum
 * @param data array of characters recevied
 * @param len number of characters in the array used to calculate the checksum
 * @return the 1 byte checksum
 */
u8 SLComputeFIPChecksum(const u8 *data, u32 len);

/* Write the two-byte sightline packet ID
 */
SLINLINE static u8 SLFIPAddHeader(u8 * dst)
{
  dst[SLFIP_OFFSET_HDR0] = HeaderByte1;
  dst[SLFIP_OFFSET_HDR1] = HeaderByte2;
  return 2;
}

SLINLINE static u8 SLFIPAddMTIHeader(u8 *dst)
{
  dst[SLFIP_OFFSET_HDR0] = HeaderByteMTI1;
  dst[SLFIP_OFFSET_HDR1] = HeaderByteMTI2;
  return 2;
}

///////////////////////////////////////////////////////////////////////////////
// Test to see if packet corresonds to a command (as opposed to invalid, or telemetry)
SLINLINE static bool SLisFIPCommand(u8 *bytes, s32 len)
{
  u8 type;
  if(len<4 || bytes[0] != HeaderByte1)
    return false;
  if(bytes[1] != HeaderByte2)
    return false;
  if(bytes[2] & 0x80)
    type = bytes[4];  // Extended length packet
  else
    type = bytes[3];

  if(type==AnalyzeRenderSync || type == TrackingPositions ||
      type == TrackingPosition || type == TrackingBoxPixelStats ||
      type == FocusStats)
    return false;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
//            Extended FIP
///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

  typedef struct FIPEX_PKT {
    u8 header1;
    u8 header2;
    u8 length1;
    u8 length2;
    u8 type;
    u8 data[MAX_SLFIPEX_PAYLOAD-1];
    u8 dummyChksum;
  } FIPEX_PKT;

  typedef struct FIP_PKT {
    u8 header1;
    u8 header2;
    u8 length;
    u8 type;
    u8 data[MAX_SLFIP_PAYLOAD-1];
    u8 dummyChksum;
  } FIP_PKT;


  typedef struct FIPEX_PACKET {
    union {
      FIPEX_PKT fpx;        //!< Extended FIP.
      FIP_PKT fip;
      u8 bytes[MAX_SLFIPEX_PACKET];
    };
  } FIPEX_PACKET;

#ifdef __cplusplus
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Functions for SL Internal Use
///////////////////////////////////////////////////////////////////////////////
SLINLINE static u32 SlFipGetPktLen(const u8 *data, s32 *extraLenBytes, bool fipEx=false)
{ 
  u32 len = data[0];
  *extraLenBytes = 0;
  if (fipEx) {
    if (len & MAX_SLFIPEX_BYTEMASK) {
      u32 len2 = (data[1] << 7);
      len = (len & ~MAX_SLFIPEX_BYTEMASK) + len2;
      *extraLenBytes = 1;
    }
  }
  return len;
}

///////////////////////////////////////////////////////////////////////////////
SLINLINE static bool SlFipCheckLen(u32 len, bool fipEx=false)
{
  if (fipEx)
    return len <= MAX_SLFIPEX_PAYLOAD;
#ifdef SLDebugAssert
  SLDebugAssert(len < 128); // to see if there is any case where the length exceeds 7bit value.
#endif
  return len <= MAX_SLFIP_PAYLOAD;
}

///////////////////////////////////////////////////////////////////////////////
SLINLINE static s32 SlFipSetLen(u8 *data, u32 payloadLen, bool fipEx=false)
{
#ifdef SLDebugAssert
  SLDebugAssert(SlFipCheckLen(payloadLen, fipEx));
#endif
  if (fipEx) {
    data[0] = (u8)payloadLen;
    if (payloadLen >= MAX_SLFIPEX_BYTEMASK) {
      data[0] |= MAX_SLFIPEX_BYTEMASK;
      data[1] = payloadLen >> 7;
      return 2;
    }
    return 1;
  }
  data[0] = payloadLen;
  return 1;
}

///////////////////////////////////////////////////////////////////////////////
s32 SLFIPGetVersionNumber(SLPacketType buffer);
s32 SLFIPGetHardwareID(SLPacketType buffer);

typedef enum {
  SL_RESET_PARAMETERS = 0,
  SL_RESET_VIDEO      = 1,    //!< Reset the video decoder
  SL_RESET_BOARD      = 2,
  SL_RESET_APP        = 3,    //!< Terminate application
  SL_RESET_FACTORY    = 4,    //!< deletes any parameters from file system/flash, soft resets the board
  SL_RESET_CAMERA     = 5,    //!< Send reset command to any known camera attached (TAU, SONY, etc.) - NYI
  SL_RESET_PARAMETERS_SOFT = 6, //!< Resets software parameters (not serial port, network, camera type)
  SL_RESET_LAST               
} SLA_RESET_TYPE;

s32 SLFIPResetAllParameters(SLPacketType buffer, u8 resetType);
s32 SLFIPResetAllParametersExtended(SLPacketType buffer, u8 resetType, u64 swID);

s32 SLFIPSetStabilizationParameters(SLPacketType buffer, u8 mode, u8 rate, u8 limit, u8 angleLimit);
s32 SLFIPGetStabilizationParameters(SLPacketType buffer);
s32 SLFIPResetStabilizationMode(SLPacketType buffer);

s32 SLFIPSetOverlayMode(SLPacketType buffer, u8 primaryReticle, u8 secondaryReticle, u16 graphics, u8 mtiReticle, u8 mtiSelectableReticle);
s32 SLFIPCurrentOverlayMode(SLPacketType buffer, u8 primaryReticle, u8 secondaryReticle, u16 graphics, u8 mtiReticle, u8 mtiSelectableReticle);
s32 SLFIPGetOverlayMode(SLPacketType buffer);
s32 SLFIPSetOverlayMode3000(SLPacketType buffer, u8 primaryReticle, u8 secondaryReticle, u16 graphics, u8 mtiColor, u8 mtiSelectableColor, u8 camIdx);
s32 SLFIPGetOverlayMode3000(SLPacketType buffer, u8 camIdx);
s32 SLFIPCurrentOverlayMode3000(SLPacketType buffer, u8 primaryReticle, u8 secondaryReticle, u16 graphics, u8 mtiReticle, u8 mtiSelectableReticle, u8 camIdx);

s32 SLFIPModifyTracking(SLPacketType buffer, u16 col, u16 row, u8 flags);

s32 SLFIPStartTracking(SLPacketType buffer, u16 col, u16 row, u8 flags);
s32 SLFIPStopTracking(SLPacketType buffer);
s32 SLFIPNudgeTrackingCoordinate(SLPacketType buffer, s8 offsetCol, s8 offsetRow, u8 rotate);

/// Or these bits together to determine what is reported
/// @see SLFIPSetCoordinateReportingPeriod
typedef enum {
  SL_COORD_REPORT_NONE                  = 0x0000,  //!< No reporting ***Not really. Sending this as a flag means Send Tracking Position and Tracking Positions of primary track*** see IDD.
  SL_COORD_REPORT_POSITION              = 0x0001,  //!< Report position (0x43)
  SL_COORD_REPORT_POSITIONS_PRIMARY     = 0x0002,  //!< Report positions (0x51) of primary
  SL_COORD_REPORT_POSITIONS_NON_PRIMARY = 0x0004,  //!< Report positions (0x51) of non-primary
  SL_COORD_REPORT_PRIMARY               = SL_COORD_REPORT_POSITION | SL_COORD_REPORT_POSITIONS_PRIMARY,
  SL_COORD_REPORT_POSITIONS             = SL_COORD_REPORT_POSITIONS_PRIMARY | SL_COORD_REPORT_POSITIONS_NON_PRIMARY,
  SL_COORD_REPORT_ALL                   = SL_COORD_REPORT_POSITION | SL_COORD_REPORT_POSITIONS,  //!< Everything, not including the follwoing: LAnding Aid, Stats, Focus...
  SL_COORD_REPORT_LANDING_AID           = 0x0010,  //!< landing aid detection results
  SL_COORD_REPORT_PRIORITIZE_TELEMETRY  = 0x0020,  //!< Prioritize frame rate analyze/telemetry over render/compress
  SL_COORD_REPORT_TRACKBOX_STATS        = 0x0040,  //!< report stats for all active track boxes (mean,max,min (14 bit data only)
  SL_COORD_REPORT_FOCUS_STATS           = 0x0080,  //!< report auto-focus statistics
  SL_COORD_REPORT_AUX_DATA              = 0x8000   //!< report aux data in all telem packets (except focus which already has it)  
} SL_REPORT_FLAGS;

// reportFlags is SL_REPORT_FLAGS above. be careful with SL_COORD_REPORT_NONE - if this is what you want, set the frame period to 0.
// Note that setting the SL_COORD_REPORT_AUX_DATA bit of reportFlags only makes
// sense if any telemetry is being sent. The same auxiliary data will be added to all telemetry packets being sent, as selected
// in the reportFlags value (Position | Positions | Landing etc).
s32 SLFIPSetCoordinateReportingPeriod(SLPacketType buffer, u8 framePeriod, u16 reportFlags=0);

s32 SLFIPSetTrackingParameters(SLPacketType buffer, u8 objectSize, u8 mode, u8 noiseMode, u8 maxMisses, u8 reserved1 = 0, u16 nearVal=0, u8 acqAssist = 0, u8 intellAssist = 0);
s32 SLFIPGetTrackingParameters(SLPacketType buffer);

s32 SLFIPSetRegistrationParameters(SLPacketType buffer, u16 maxTranslation, u8 maxRotation,
                                   u8 zoomRange=0, u8 lft=0, u8 rgt=0, u8 top=0, u8 bot=0);
s32 SLFIPGetRegistrationParameters(SLPacketType buffer);
s32 SLFIPSetVideoParameters(SLPacketType buffer, u8 autoChop, u8 chopTop, u8 chopBot, u8 chopLft, u8 chopRgt, 
                             u8 deinterlace, u8 autoReset);
s32 SLFIPSetAVideoParameters(SLPacketType buffer, u8 autoChop, u8 chopTop, u8 chopBot, u8 chopLft, u8 chopRgt, 
                             u8 deinterlace, u8 autoReset);
s32 SLFIPGetVideoParameters(SLPacketType buffer);
s32 SLFIPGetAVideoParameters(SLPacketType buffer);
s32 SLFIPSetStabilizationBias(SLPacketType buffer, s8 colBias, s8 rowBias, bool autoBias, u8 updateRate );
s32 SLFIPSetStabilizationBias3000(SLPacketType buffer, s8 biasCol, s8 biasRow, bool autoBias, u8 updateRate, u8 camIdx);
s32 SLFIPCurrentStabilizationBias(SLPacketType buffer, s8 colBias, s8 rowBias, bool autoBias, u8 updateRate);

// note: for palette packet, buffer size must be 256*3+overhead(6) minimum.
s32 SLFIPSetUserFalseColorPalette(FIPEX_PACKET *fpx, const u8* YValues, const u8* UValues, const u8* VValues, u8 type = SetUserPalette);  
s32 SLFIPCurrentUserFalseColorPalette(FIPEX_PACKET *fpx, const u8* YValues, const u8* UValues, const u8* VValues);  

s32 SLFIPSetDisplayParameters(SLPacketType buffer, u16 rotationDegrees, u16 rotationLimit, u8 decayRate, u8 falseColor, u8 displayZoom, u8 zoomToTrack, s16 panCol, s16 tiltRow, u8 idx=0);
s32 SLFIPSetDisplayRotation(SLPacketType buffer, u16 rotationDegrees, u16 rotationLimit, u8 decayRate, u8 falseColor, u8 displayZoom, u8 zoomToTrack, s16 panCol, s16 tiltRow, u8 idx=0);
s32 SLFIPGetDisplayParameters(SLPacketType buffer, u8 idx=0);
s32 SLFIPGetDisplayRotation(SLPacketType buffer, u8 idx=0);
s32 SLFIPCurrentDisplayParameters(SLPacketType buffer, u16 rotationDegrees, 
                                  u16 rotationLimit, u8 decayRate, u8 falseColor,
                                  u8 zoom, u8 zoomToTrack, s16 panCol, s16 tiltRow, u8 idx);
s32 SLFIPCurrentDisplayRotation(SLPacketType buffer, u16 rotationDegrees, 
                                  u16 rotationLimit, u8 decayRate, u8 falseColor,
                                  u8 zoom, u8 zoomToTrack, s16 panCol, s16 tiltRow, u8 idx);
s32 SLFIPSetDisplayAngle(SLPacketType buffer, u8 idx, u16 rotationDegrees, u16 rotationLimit, u8 decayRate);

typedef struct {
  s8 vertical; 
  s8 horizontal;
  u8 rotate;
  u8 zoom;
  u8 hzoom;       // default: 128 multiplier (maps to 1), 255 ->1.1, 1->0.9, 0-> don't change.
} SLFIPAlignData; // similar to the blending params
s32 SLFIPSetMultipleAlignment(SLPacketType buffer, SLFIPAlignData* alignments, u8 length);
s32 SLFIPCurrentMultipleAlignment(SLPacketType buffer, SLFIPAlignData* alignments, u8 length);

typedef struct {
  s16 MinVel8;        // -1: not set.
  s16 MaxVel8;        // -1: not set.
  s16 MaxAccel8;      // -1: not set.
  s16 MinWide;        // -1: not set.
  s16 MaxWide;        // -1: not set.
  s16 MinHigh;        // -1: not set.
  s16 MaxHigh;        // -1: not set.
  u8  hideOverlapTrks;// Hide any tracks that overlap with object tracks.  
  u8  NFramesBack;    // How many frames back do we go when we perform frame differencing (1..15, default: 15)
  u8  MergeRadius;    // Radius at which blobs close together are merged (pixels)
  u8  MergeDirTol;    // How much tolerance do we accept between blob velocity vector direction and object velocity vector direction? (degrees)
  u8  UseRegistration; // For HistDetect, whether to use registration information to infer object motion
  u8  DetUpdateRate;   // For Hist, Blob, and Maritime Detect - update rate of the detect surface.
  s16 Reserved3;      // Reserved
  u16 BgTimeConst;    // Maximum time constant in frames for averaging into background
  u8  BgEdgePenalty6;  // Amount to penalize edges, 0 = no edge penalty, 64 =  double edge penalty (default).
  u8  BgResetConf;
  u8  BgResetOff;
  u8  BgResetAng;
  u8  BgResetFrames;
  u8  BgWarpConf;
  u8  BgWarpOff;
  u8  BgWarpAng;
  u8  BgWarpFrames;
  u8  MaxTrackFrames; // Number of frames to keep tracking once target has disappeared  0 = auto, 255 = forever
  u8  Reserved4;      // Reserved - used to be flag to display indices in 2.21 
  u8  Downsample;     // MSB set if auto based on frame size, 0: None, 1:2x2, 2:4x4, 3:8x8
  u8  MaxTelemTrks;   // Max number of MTI tracks to include in telemetry messages
  u8  MaxKlvTrks;     // Max number of MTI tracks to include in KLV messages
}SLFIPAdvancedDetectionParams;
typedef SLFIPAdvancedDetectionParams SLFIPAdvancedMoTDetParams;
//! Deprecated - Use SLFIPSetAdvancedDetectionParameters
s32 SLFIPSetAdvancedMoTDetParameters(SLPacketType buffer, SLFIPAdvancedMoTDetParams params);
//! Deprecated - Use SLFIPCurrentDetectionParameters
s32 SLFIPCurrentAdvancedMoTDetParameters(SLPacketType buffer, SLFIPAdvancedMoTDetParams params);
s32 SLFIPSetAdvancedDetectionParameters(SLPacketType buffer, SLFIPAdvancedDetectionParams params);
s32 SLFIPCurrentDetectionParameters(SLPacketType buffer, SLFIPAdvancedDetectionParams params);

typedef struct {
  u8 flags;
  u8 reserved[3];
  u16 searchRowUl;
  u16 searchColUl;
  u16 searchWidth;
  u16 searchHeight;
  u16 detectRowUl;
  u16 detectColUl;
  u16 detectWidth;
  u16 detectHeight;
}SLFIPDetectionRoiParams;
typedef SLFIPDetectionRoiParams SLFIPMotDetRoiParams;
//! Deprecated - Use SLFIPCurrentMotDetRegionOfInterestParameters
s32 SLFIPSetMotDetRegionOfInterestParameters(SLPacketType buffer, SLFIPMotDetRoiParams params);
//! Deprecated - Use SLFIPCurrentDetectionRegionOfInterestParameters
s32 SLFIPSetDetectionRegionOfInterestParameters(SLPacketType buffer, SLFIPMotDetRoiParams params);
s32 SLFIPCurrentMotDetRegionOfInterestParameters(SLPacketType buffer, SLFIPDetectionRoiParams params);
s32 SLFIPCurrentDetectionRegionOfInterestParameters(SLPacketType buffer, SLFIPDetectionRoiParams params);

typedef enum {
  SL_ADC_AUTOAGC  = 0x02   //!< ENABLE AGC functionality
} SL_ADC_MODE;

typedef enum {
   SL_AGC_HOLD = 0x01   //!< hold current AGC levels  
} SL_AGC_MODE;

s32 SLFIPSetADCParameters(SLPacketType buffer, u8 brightness, u8 contrast, u8 saturation, s8 hue, u8 luma1, u8 luma2, u8 luma3, u8 chroma1, u8 chroma2, u8 mode);
s32 SLFIPGetADCParameters(SLPacketType buffer);
s32 SLFIPCurrentADCParameters(SLPacketType buffer, u8 brightness, u8 contrast, u8 saturation, s8 hue, u8 luma1, u8 luma2, u8 luma3, u8 chroma1, 
                              u8 chroma2, u8 mode);

s32 SLFIPSetDigCamParameters(SLPacketType buffer, u8 camIdx, u8 mode, u16 AGHoldmax, u16 AGHoldmin, 
                                 u8 rowROIPct, u8 colROIPct, u8 highROIPct,  u8 wideROIPct);
s32 SLFIPGetDigCamParameters(SLPacketType buffer);
s32 SLFIPCurrentDigCamParameters(SLPacketType buffer, u8 mode, u16 maxAGC, u16 minAGC, 
                                 u8 rowROIPct, u8 colROIPct, u8 highROIPct,  u8 wideROIPct);


s32 SLFIPSetVideoEnhancementParameters(SLPacketType buffer, u8 enhancementMode, u8 sharpening, u8 param1, u8 param2, u8 param3);
s32 SLFIPGetVideoEnhancementParameters(SLPacketType buffer);

s32 SLFIPGetImageSize(SLPacketType buffer);
s32 SLFIPCurrentImageSize(SLPacketType buffer, s16 capWide, s16 capHigh, s16 disWide, s16 disHigh,
                          s16 disRectCol, s16 disRectRow, s16 disRectWide, s16 disRectHigh);


typedef enum {
  SL_RECORD_H264             = 0x01,
  SL_RECORD_JPEG             = 0x02,
  SL_RECORD_FIP              = 0x04,
  SL_RECORD_TELEMETRY        = 0x08,
  SL_RECORD_PASS_THROUGH_LOG = 0x10
} SL_RECORD_TYPE;

typedef enum {
  SDR_DONT_CHANGE               = 0,
  SDR_START_RECORDING           = 1,
  SDR_STOP_RECORDING            = 2,
  SDR_START_NET_TRACE_COMMANDS  = 3,
  SDR_STOP_NET_TRACE_COMMANDS   = 4,
  SDR_START_NET_TRACE_TELEMETRY = 5,
  SDR_STOP_NET_TRACE_TELEMETRY  = 6
} SD_RECORDING_STATE;

s32 SLFIPSetSDCardRecordingParameters(SLPacketType buffer, u8 recordMode, u8 clearFlash, u8 getStatus, u8 getDirectory,
                                      u16 markFrame, u8 recordType, u8 reserved, u8 lblLen, const char *lbl);

s32 SLFIPSDCardDirectoryContents(SLPacketType buffer, const char **files, s16 nFiles, s16 startIndex, u16 *endIndex);
s32 SLFIPExSDCardDirectoryContents(SLPacketType buffer, const char **files, s16 nFiles, s16 startIndex, u16 *endIndex, u32 maxBufLen);

s32 SLFIPSDCardRecordingStatus(SLPacketType buffer, u8 state, u32 length, u32 remSpace);

s32 SLFIPDirectoryStatisticsReply(SLPacketType buffer, u32 totalSize, u32 spaceUsed);

s32 SLFIPSendTraceString(SLPacketType buffer, const u8 *str, u8 len);
s32 SLFIPSetRGB565Conversion(SLPacketType buffer, u16 chromaRot, u8 redScale, u8 greenScale, u8 blueScale);

enum {
  SL_SYSTYPE_ANALYZE    = 0x01, //!< System will perform image processing
  SL_SYSTYPE_RENDER     = 0x02, //!< System will perform rendering/display
  SL_SYSTYPE_SENDSYNCH  = 0x04  //!< System will send synchronization messages (forced with analyze only, N/A for render only)
};

s32 SLFIPSetSystemType(SLPacketType buffer, u16 systemType);
s32 SLFIPCurrentSystemType(SLPacketType buffer, u16 systemType);

/*!< Set of operations that can be performed when using SLSetTelemetryDestination
 * @see  SLSetTelemetryDestination
 */
typedef enum {
  SL_SETEL_UNKNONW    = 0,
  SL_SETEL_ADD        = 1,  //!< Add this IP address as telemetry output destination
  SL_SETEL_DELETE     = 2,  //!< Remeove this IP address from receiveing telemetry output
  SL_SETEL_DELETEALL  = 3   //!< Remove all IP address from telemetry output list
} SL_SETEL_MODE;

/*!< Flags to modify behavior of SLSetTelemetryDestination
 * @see  SLSetTelemetryDestination
 */
enum {
  SL_SETEL_FLAGS_SAVE  = 0x01,  //!< Indicates to save the telemetry destination with parameters
  SL_SETEL_FLAGS_CAMID = 0x02,  //!< Indicates to apply the camera ID or not
};

/*! Configure additional destinations for telemetry output
 * @param mode Operations that you want performed at this address
 * @param cameraID Results from this hardware are reported as cameraID (default 0xFF)
 * @param ipAddr will be sent to this IP address (A.B.C.D)
 * @param udpDestinationPort at this port (default 14002)
 */
s32 SLFIPSetPacketDestination(SLPacketType buffer, SL_SETEL_MODE mode, u8 cameraID, u32 ipAddr, u16 udpDestinationPort, u8 flags=0);

// Deprecated, use SLFIPSetPacketDestination
s32 SLFIPSetTelemetryDestination(SLPacketType buffer, SL_SETEL_MODE mode, u8 cameraID, u32 ipAddr, u16 udpDestinationPort, u8 flags=0);

typedef struct
{
  s16 centerR;
  s16 centerC;
  u16 radius;
} SL_CIRCLE_DRAW_PARAMS;

typedef struct
{
  s16 ulR;
  s16 ulC;
  u16 high;
  u16 wide;
} SL_RECT_DRAW_PARAMS;

typedef struct
{
  s16 p1R;
  s16 p1C;
  s16 p2R;
  s16 p2C;
} SL_LINE_DRAW_PARAMS;

#define MAX_TEXT_LEN 64 // TODO:  Also in slstabtrack_c.h
typedef struct
{
  s16 row;
  s16 col;
  u16 len;
  char text[MAX_TEXT_LEN];
} SL_TEXT_DRAW_PARAMS;

typedef struct
{
  s16 row;
  s16 col;
  u8  fontId;       // set bit7 == 1 to add black Shadow @see SLFONT_TYPE
  u8  fontWidth;
  u8 xscale5;      // 32 == x1 scale
  u8 yscale5;      // 32 == x1 scale
  u16 len;
  char text[MAX_TEXT_LEN];
} SL_TEXTEX_DRAW_PARAMS;

typedef struct
{
  s16 row;
  s16 col;
  s8  xscale5;      // 32 == x1 scale
  s8  yscale5;      // 32 == x1 scale
  s16 angle5;       // 32 == +1 degree.
  char text[MAX_TEXT_LEN];
} SL_CUSTOM_DRAW_PARAMS;

typedef enum {
  // NOTE: Must match with slDrawObj.cpp/idxToColor[] order.
  // Warning: unfortunatelly, we have a few different set of idxToColor[]s defined.
  // The definitions here are used for drawing graphics primitives, such as circles, lines.
  // Also used for 'Custom' reticles for SLA-lib.
  SL_DRAW_COLOR_WHITE        = 0,
  SL_DRAW_COLOR_BLACK        = 1,
  SL_DRAW_COLOR_LIGHT_GRAY   = 2,
  SL_DRAW_COLOR_GRAY         = 3,
  SL_DRAW_COLOR_DARK_GRAY    = 4,
  SL_DRAW_COLOR_LIGHT_BLUE   = 5,
  SL_DRAW_COLOR_BLUE         = 6,
  SL_DRAW_COLOR_DARK_BLUE    = 7,
  SL_DRAW_COLOR_LIGHT_GREEN  = 8,
  SL_DRAW_COLOR_GREEN        = 9,
  SL_DRAW_COLOR_DARK_GREEN   = 10,
  SL_DRAW_COLOR_RED          = 11,
  SL_DRAW_COLOR_ORANGE       = 12,
  SL_DRAW_COLOR_YELLOW       = 13,
  SL_DRAW_COLOR_TRANSPARENT  = 14, 
  SL_DRAW_COLOR_AUTO         = 15
} SL_DRAW_COLOR;

enum {
  // TODO:  Are some of these really an enum?  Not all combinations are supported
  SL_DRAW_FLAGS_SOURCE_COORDS     = 0x01,    //!< 
  SL_DRAW_FLAGS_MOVE_WITH_CAMERA  = 0x02,    //!< Object moves with camera motion
  SL_DRAW_FLAGS_STATIC            = 0x04,    //!< Static graphics object.
  //todo: Implement 'move with tack' mode.
  //SL_DRAW_FLAGS_MOVE_WITH_TRACK  = 0x08,  //!< Object moves with track

  SL_DRAW_FLAGS_ORIGIN_UPPER_LEFT = 0x80    //!< Coordinate origins at upper-left
};

typedef enum {
  SLFIP_DO_CIRCLE            = 0,
  SLFIP_DO_RECT              = 1,
  SLFIP_DO_LINE              = 2,
  SLFIP_DO_TEXT              = 3,  // Classic text in 2x size.
  SLFIP_DO_CIRCLE_FILL       = 4,
  SLFIP_DO_RECT_FILL         = 5,
  SLFIP_DO_TEXT_EX           = 6,  // New text with more options (change font, scale, etc).
  SLFIP_DO_MULTI_LINE        = 7,  // Used for defining custom reticle for SLALIB interface.
  SLFIP_DO_STD_TYPE_LAST = SLFIP_DO_MULTI_LINE,

  SLFIP_DO_CUSTOM_0          = 128, // User defiend Graphics Object #0.
  SLFIP_DO_CUSTOM_1,
  SLFIP_DO_CUSTOM_2,
  SLFIP_DO_CUSTOM_3,
  SLFIP_DO_CUSTOM_4,
  SLFIP_DO_CUSTOM_5,
  SLFIP_DO_CUSTOM_6,
  SLFIP_DO_CUSTOM_7,
  SLFIP_DO_MAX
} SLFIP_DRAW_OBJECT_TYPE;

s32 SLFIPDrawObject(SLPacketType buffer, u8 objId, u8 action, u8 sourceCoords, u8 moveWithCamera, u8 staticCoordinates, 
                    u8 upperLeftOrigin, u8 type, void *params, u8 backgroundColor, u8 foregroundColor);

s32 SLFIPDrawObject(SLPacketType buffer, u8 objId, u8 action, u8 propertyFlags, u8 type, void *params, u8 backgroundColor, u8 foregroundColor);

// a single set of parametrs of a currently drawn object.
s32 SLFIPCurrentOverlayObjectData( SLPacketType buffer, u8 id, u8 type, s32 a, s32 b, s32 c, s32 d, 
                                   u8 fgColor, u8 bgColor, u8 moveWithCam, u8 staticObject, u8 ULCoords, char* text); 

// a bit array (4X64 bits, 256 total) of active overlay objects.
s32 SLFIPCurrentOverlayObjectsIDs( SLPacketType buffer,  u64* bitArray );           

//!< Structure encapsulating all the data returned for a single track position.
// This struct represents the order in which the values are packed into the buffer
// which differs from the order in which the parameters are passed to SLFIPTrackingPosition
// @see TrackingPosition, 
typedef struct {
  s16 trackCol;
  s16 trackRow;
  f32 sceneCol;
  f32 sceneRow;
  s16 displayCol;
  s16 displayRow;
  u8 trackingConfidence;
  u8 sceneConfidence;
  u16 displayAngle7;
  u8 idx;
  u8 deltaTime;
  s16 sceneAngle7;
  u16 sceneScale8;
} SLTelemetryData;

typedef struct {
  u8 idx;
  u8 numTracks;
} SLTrackPositionsHdr;

//!< Structure encapsulating all the data returned for a single track pixel stats.
typedef struct {
  u16 mean;
  u16 max;
  u16 min;
  u8 idx;
} SLTelemetryPixelStats;

#define SL_MAX_TRACK_POINTS 5 //!< Maximum number points that can be returned from SightLine (@see SL_MAX_TRACKS)

//!< Flag values indicating different properties about the track (bit wise)
typedef enum {
  SL_FLAG_PRIMARY   = 0x01,
  SL_FLAG_SELECTED  = 0x02
} SLTrackFlags;

//!< Track position / properties
typedef struct {
	u8 trackId;   //!< Unique track Identifier [0..5)
  u16 col;      //!< pixel column    
  u16 row;      //!< pixel row
  u16 width;    //!< track width
  u16 height;   //!< track height
  s16 dc;       //!< horizontal momentum "direction in columns"
  s16 dr;       //!< vertical momentum "direction in rows"
  u8 conf;      //!< confidence value
  u8 flags;     //!< track properties (@see SLTrackFlags)
} SLTrackPoint;

#define SLFIP_MAX_DRAW_OBJECTS 64
#define SLFIP_MTI_MAX_TRACKED_OBJECTS 100
#define SLFIP_MAX_TRACK_RES 10
#define SLFIP_MAX_TOTAL_TRACK_RES (SLFIP_MAX_TRACK_RES + SLFIP_MTI_MAX_TRACKED_OBJECTS)

typedef enum {
  SLFIP_BM_FRAME_BLEND_WARP   = 0, //!< Global frame blend
  SLFIP_BM_THERMAL_BLEND_WARP = 1,
  SLFIP_BM_NIGHT_BLEND_WARP   = 2,
  SLFIP_BM_COLOR_BLEND_WARP   = 3,  // Warp retains color info, Fixed is B&W                
  SLFIP_BM_RGB_BLEND          = 4,
  SLFIP_BM_FRAME_BLEND_FIX    = 5,
  SLFIP_BM_THERMAL_BLEND_FIX  = 6,
  SLFIP_BM_NIGHT_BLEND_FIX    = 7,
  SLFIP_BM_COLOR_BLEND_FIX    = 8,  // Fixed color is used.
  SLFIP_BM_COLOR_IR_BLEND_FIX = 9, // like ColorBlend, with UserPallet for IR input.
  SLFIP_BM_COLOR_IR_BLEND_WARP = 10,
  SLFIP_BM_END
} SLFipBlendMode;

typedef enum {
  SLFIP_TRACK_PRIMARY   = 0x01,
  SLFIP_TRACK_SELECTED  = 0x02
} SLFIP_TRACK_FLAGS;

typedef enum {
  SL_BOARD_TYPE_SLA2000     = 0,
  SL_BOARD_TYPE_SLA2100     = 1,
  SL_BOARD_TYPE_UNKNOWN     = 2,
  SL_BOARD_TYPE_BAD         = 3,
  SL_BOARD_TYPE_SLA1000     = 4,
  SL_BOARD_TYPE_WIN2000     = 5,  // VideoTrack (2000) emulation.
  SL_BOARD_TYPE_LINUXPC     = 6,
  SL_BOARD_TYPE_SLA1500     = 7,
  SL_BOARD_TYPE_SLA1501     = 8,
  SL_BOARD_TYPE_WIN1500     = 9,  // VideoTrack1500 emulation.
  SL_BOARD_TYPE_UPGRD       = 10, // Upgrade server (any hardware).
  SL_BOARD_TYPE_GIMBAL      = 11, // Generic Gimbal Controller
  SL_BOARD_TYPE_SLA3000     = 12,
  SL_BOARD_TYPE_WIN3000     = 13,
  SL_BOARD_TYPE_Last
} SL_BOARD_TYPE;

//!< A set of known lenses for which command and control has been implemented.
typedef enum {
  SL_LENS_TYPE_NONE     = 0,
  SL_LENS_TYPE_TAMRON   = 1,  //!< Tamron SC001
  SL_LENS_TYPE_SONY     = 2,  //!< SONY
  SL_LENS_TYPE_HITACHI  = 3   //!< Hitachi DS120
} SLLensType;

typedef enum {
  SL_LENS_NOOP              = 0,
  SL_LENS_TYPE              = 1,
  SL_LENS_RESET             = 2,
  SL_LENS_STATUS            = 3,
  SL_LENS_ZOOM_WIDE         = 4,
  SL_LENS_ZOOM_NRW          = 5,
  SL_LENS_ZOOM_STOP         = 6,
  SL_LENS_FOCUS_FAR         = 7,
  SL_LENS_FOCUS_NEAR        = 8,
  SL_LENS_FOCUS_STOP        = 9,
  SL_LENS_FOCUS_AUTO        = 10,
  SL_LENS_ZOOM_TRACK_FOCUS  = 11,
  SL_LENS_NUC               = 12,
  SL_LENS_ZOOM_POS          = 13,
  SL_LENS_FOCUS_POS         = 14,
  SL_LENS_END               = SL_LENS_FOCUS_POS  // Last value
} SLLensMode;

typedef enum {
  SL_NUC_SHOW_RAW       = 0,  // Don't nuc
  SL_NUC_SHOW_DODEADNUC = 1,  // Do dead and nuc
  SL_NUC_SHOW_DONUC     = 2,  // Do nuc only
  SL_NUC_SHOW_DODEAD    = 3,  // Do dead only
  SL_NUC_SHOW_GAIN      = 4,  // Show nuc gain
  SL_NUC_SHOW_OFF       = 5,  // Show nuc offset
  SL_NUC_SHOW_DEAD      = 6,  // Show dead pixels
  SL_NUC_SHOW_END       = SL_NUC_SHOW_DEAD  // Last value
} SLNUCShow;

typedef enum {
  SL_NUC_MODE_NONE          = 0,
  SL_NUC_MODE_ADD           = 1,
  SL_NUC_MODE_CLEAR         = 2,
  SL_NUC_MODE_RESET_GAIN    = 3,
  SL_NUC_MODE_RESET_OFF     = 4,
  SL_NUC_MODE_RESET_DEAD    = 5,
  SL_NUC_MODE_RESET_ALL     = 6,
  SL_NUC_MODE_CALC_1PT      = 7,
  SL_NUC_MODE_CALC_2PT      = 8,
  SL_NUC_MODE_CALC_DEAD     = 9,
  SL_NUC_MODE_CALC_DEAD_MAP = 10,
  SL_NUC_MODE_WRITE         = 11,
  SL_NUC_MODE_READ          = 12,
  SL_NUC_MODE_END           = SL_NUC_MODE_READ // Last value
} SLNUCRunMode;

typedef enum {
  SL_TABLE_TYPE_UNKNOWN = 0,
  SL_TABLE_TYPE_GAIN    = 1,
  SL_TABLE_TYPE_OFFSET  = 2,
  SL_TABLE_TYPE_DEAD    = 3
} SLNUCTableType;



s32 SLFIPVersionNumber(SLPacketType buffer, u8 swMajor, u8 swMinor, u8 hwVersion, u8 degreesF, u64 hardwareID, u32 appBits, 
                       u8 swRelease, SL_BOARD_TYPE boardType=SL_BOARD_TYPE_UNKNOWN, u16 otherVersion = 0);
s32 SLFIPHardwareID(SLPacketType buffer,u64 hwID);
s32 SLFIPCurrentStabilizationParameters(SLPacketType buffer, u8 mode, u8 rate, u8 limit, u8 angleLimit, u8 autoBias);

s32 SLFIPTrackingPosition(SLPacketType buffer, 
                          s16 trackingRow, s16 trackingCol, u8 trackingConfidence, 
                          f32 sceneRow, f32 sceneCol, f32 sceneAngle, f32 sceneScale, u8 sceneConfidence,
                          s16 offsetRow, s16 offsetCol, u16 rotation, u8 idx, u8 frame, u16 flags=0, u32 frameId=0xFFFFFFFF, u64 timestamp=0);
s32 SLFIPUnpackTrackingPosition(SLTelemetryData *trackPosition, const SLPacketType buffer, u64 *timeStamp = 0, u32 *frameIdx = 0 );

s32 SLFIPTrackingPositions(SLPacketType buffer, void *trks, u8 numTracks, u8 cameraIndex, u8 mti=0, u16 flags=0, u32 frameId=0xFFFFFFFF, u64 timestamp=0);
s32 SLFIPUnpackTrackingPositions(u8 *cameraIndex, void *trks, u8 maxTracks, const SLPacketType buffer, u64 *timeStamp = 0, u32 *frameIdx = 0);

s32 SLFIPCurrentTrackingParameters(SLPacketType buffer, u8 objectSize, u8 mode, u16 nearVal, u8 maxMisses);
s32 SLFIPCurrentRegistrationParameters(SLPacketType buffer, u16 maxTranslation, u8 maxRotation,
                                       u8 zoomRange=0, u8 lft=0, u8 rgt=0, u8 top=0, u8 bot=0);
s32 SLFIPCurrentVideoParameters(SLPacketType buffer, u8 autoChop, u8 chopTop, u8 chopBot, u8 chopLft, u8 chopRgt, 
                                 u8 deinterlace, u8 resetAnalogOnError);
s32 SLFIPCurrentAVideoParameters(SLPacketType buffer, u8 autoChop, u8 chopTop, u8 chopBot, u8 chopLft, u8 chopRgt, 
                                 u8 deinterlace, u8 resetAnalogOnError);

s32 SLFIPTrackingPixelStats(SLPacketType buffer, void *trks, u8 numTracks, u8 cameraIndex, u16 flags=0, u32 frameId=0xFFFFFFFF, u64 timestamp=0);
s32 SLFIPUnpackTrackingPixelStats(u8 *cameraIndex, void *trks, u8 maxTracks, const SLPacketType buffer, u64 *timeStamp = 0, u32 *frameIdx = 0);

s32 SLFIPFocusStats(SLPacketType buffer, f32 metric, u64 timeStamp, u8 cameraIndex, u16 flags=0, u32 frameId=0xFFFFFFFF);
s32 SLFIPUnpackFocusStats(u8* cameraIndex, f32 *metric, u64 *timeStamp, const SLPacketType buffer, u32 *frameIdx = 0);

s32 SLFIPCurrentVideoEnhancementParameters(SLPacketType buffer, u8 enhancementMode, u8 sharpening, u8 param1, u8 param2, u8 param3);

s32 SLFIPMotDetParams(SLPacketType buffer, u8 mode, u8 threshold, u8 manualThreshold=0, u8 manualWatchFrames=0, 
                      u8 suspScore=0, u8 debug=0, u8 frameStep=1, 
                      u8 modeUpperByte = 0, u16 minTemperature = 0, u16 maxTemperature = 0);

s32 SLFIPParameterBlock(FIPEX_PACKET *fpx, const char * data, u16 len);

s32 SLFIPSetFrameIndex(SLPacketType buffer, const u8 camIdx, const u32 index);
s32 SLFIPCurrentFrameIndex(SLPacketType buffer, u8 camIdx, u32 frameIndex);

typedef enum {
  SL_NETWORK_DHCP   = 0, 
  SL_NETWORK_STATIC = 1
} SLNetworkAllocationMode;

enum {
  SL_RESPMODE_DISABLELIFO    = 0x04
};

/*!
 * @param mode 0 = DHCP, 1 = Static IP
 * @param ipAddr IP address to assign to device (IN_ADDR format)
 * @param subnet subnet mask (IN_ADDR format)
 * @param gateway gateway address (IN_ADDR format)
 * @param fipMode
 * @param replyPort
 * @return 0 on success, -1 on error
 */
s32 SLFIPSetNetworkParameters(SLPacketType buffer, u8 mode, u32 ipAddr, u32 subnet, u32 gateway, 
                              u16 c2replyPort = SLFIP_FROM_BOARD_PORT, u16 telemetryReplyPort = SLFIP_FROM_BOARD_PORT, u8 modes_reserved = 0, u8 index_reserved = 0, u16 c2listenPort = 0, u16 c2listenPort2 = 0);
s32 SLFIPGetNetworkParameters(SLPacketType buffer, u8 index=0);
s32 SLFIPCurrentNetworkParameters(SLPacketType buffer, u8 mode, u32 ipAddr, u32 subnet, u32 gateway, u16 c2replyPort, u16 telemetryReplyPort, u8 modes_reserved, u8 index_reserved, u16 c2listenPort, u16 c2listenPort2);

/** List of network names (e.g. "eth0, wifi") 
 * @param numIds Number of entries in the list
 * @param list of string names for each network interface null char delimited
 */
s32 SLFIPCurrentNetworkList(SLPacketType buffer, u8 numIds, const char * idNames);

/*! Generic getter
 * Results in reply packet (CurrentXXXParams) for that id.
 * Replaces individual GetXXXParam functions
 * Example:
 * GetDualBoardMode -> GetParamer(SetDualBoardMode)
 * GetVersionNumber -> GetParamter( GetVersionNumber ) 
 * GetTrackingParameters -> GetParameter( GetTrackingParameters )
 * where a Getter requires and input parameter, use the explicity getter
 * Example:
 * GetDisplayParameters
 * GetNetworkParameters
 * @param id Command ID (@see SLA_FIP_RESPONSES)
 */
s32 SLFIPGetParameters(SLPacketType buffer, u8 id);

s32 SLFIPGetParameters(SLPacketType buffer, u8 id, u8 arg);

s32 SLFIPGetParameters(SLPacketType buffer, u8 id, u16 arg);

s32 SLFIPGetParameters(SLPacketType buffer, u8 id, const u8 *arg, u32 argLen);

s32 SLFIPSaveParameters(SLPacketType buffer);
s32 SLFIPResetParameters(SLPacketType buffer);

/*!
 * TODO:  Bits or enumeration?
 * 0    0 = RTP MJPEG, 1 = H.264
 */
typedef enum {
  SL_VIDEO_PROTOCOL_NONE        = 0x0F,
  SL_VIDEO_PROTOCOL_MJPEG       = 0x00,
  SL_VIDEO_PROTOCOL_H264        = 0x01,
  SL_VIDEO_PROTOCOL_RTPMJPEGRAW = 0x02,
  SL_VIDEO_PROTOCOL_H264_ALT    = 0x03,
  SL_VIDEO_PROTOCOL_MPEG4       = 0x04,
  SL_VIDEO_PROTOCOL_RTPH264     = 0x05,
  SL_VIDEO_PROTOCOL_RTPTSH264   = 0x06,
  SL_VIDEO_PROTOCOL_RTPTSMPEG4  = 0x07,
  SL_VIDEO_PROTOCOL_END         = 0x07,

  SL_VIDEO_PROTOCOL_KLV_METADATA = 0x40,
  SL_VIDEO_PROTOCOL_BROADCAST    = 0x80
} SLVideoProtocol;

/*!
 * Upper 4-bits for Set Ethernet Video Parameters (0x1A) / Down Sample
 */
typedef enum {  
  SL_ENET_VID_SIZE_DEFAULT      = 0,  // Platform specific: 1500/2000=SD, 3000=input size
  SL_ENET_VID_SIZE_SD           = 1,  // SD (1500/2000=NTSC/PAL, 3000=640x480 NTSC)
  SL_ENET_VID_SIZE_PAL          = 2,  // 720x576 PAL (3000 only)
  SL_ENET_VID_SIZE_720          = 3,  // 1280x720
  SL_ENET_VID_SIZE_720_CROPPED  = 4,  // 1280x720 cropped to 4:3 960x720
  SL_ENET_VID_SIZE_1080         = 5,  // 1920x1080
  SL_ENET_VID_SIZE_1080_CROPPED = 6,  // 1920x1080 cropped to 4:3 1440x1080
  SL_ENET_VID_SIZE_1280x960     = 7,  // 1280x960 (3000 only)
  SL_ENET_VID_SIZE_END
} SLEnetVidSize;

/*!
 * @param address (ABCD) where a.b.c.d ==> reverse IN_ADDR format
 */
s32 SLFIPSetEthernetDisplay(SLPacketType buffer, SLVideoProtocol protocol, u32 ipAddr, u16 port, s32 displayId=0);
s32 SLFIPCurrentEthernetDisplay(SLPacketType buffer, SLVideoProtocol protocol, u32 ipAddr, u16 port, s32 displayId=0);

s32 SLFIPSetEthernetVideoParameters(SLPacketType buffer, u8 quality, u8 foveal, u8 frameStep, u8 frameSize, s32 displayId=0);
s32 SLFIPGetEthernetVideoParameters(SLPacketType buffer);
s32 SLFIPCurrentEthernetVideoParameters(SLPacketType buffer, u8 quality, u8 foveal, u8 frameStep, u8 downSample, s32 displayId=0);

typedef enum {
  SL_DISP_DEST_ANALOG0      =     0,  // SLA-2x00: analog output only.
  SL_DISP_DEST_NONE         =     0,  // Other than SLA-2x00: no display output.
  SL_DISP_DEST_ANALOG       =     1,  // analog output only.
  SL_DISP_DEST_NETWORK      =     2,  // H.264 or MJPEG - digital output only.
  SL_DISP_DEST_ANALOG_AND_NETWORK=3,  // both analog and digital output - slight performance penalty.
  SL_DISP_HDSDI_720P        =     4,  // SLA-2100 only.
  SL_DISP_HDSDI_1080P       =     5,  // SLA-2100 only.
  SL_DISP_HDSDI_1080I       =     6,  // SLA-2100 only.
  SL_DISP_DEST_HDMI         =  0x40,  // SLA-3000 only: HDMI output
  SL_DISP_DEST_NETWORK1     = 0x080,  // SLA-3000 only: Secondary network display.
  SL_DISP_DEST_NETWORK2     = 0x100,  // reserved
  SL_DISP_DEST_NETWORK3     = 0x200,  // reserved
  SL_DISP_DEST_HDSDI        = 0x400   // hdsdi
} SLDisplayDestination;

s32 SLFIPSetVideoMode( SLPacketType buffer, u8 numCamera, u8 numDisplay, u8 displayMode, u8 displayDest,
                      u8 camIdx0, u8 camIdx1, u8 camIdx2, u8 camIdx3, u8 pipScale, u8 pipQuadrant, u8 disIdx0,
                      u8 disIdx1, u8 disIdx2, u8 disIdx3);
s32 SLFIPGetVideoMode( SLPacketType buffer );
s32 SLFIPCurrentVideoModeParameters( SLPacketType buffer, u8 numCamera, u8 numDisplay, u8 displayMode, u8 displayDest, 
                                     u8 camIdx0, u8 camIdx1, u8 camIdx2, u8 camIdx3, u8 pipScale, u8 pipQuadrant, u8 disIdx0, 
                                     u8 disIdx1, u8 disIdx2, u8 disIdx3);

s32 SLFIPSetVideoMode3000( SLPacketType buffer, u8 numCamera, u8 displayMode, u8 displayDest, 
                       u8 camIdx0, u8 pipScale, u8 pipQuadrant, u8 disIdx0, u8 disIdx1, u8 disIdx2, u8 disIdx3,
                       const u8 *camDisp);
/* for internal use */
s32 SLFIPCurrentVideoModeParameters3000( SLPacketType buffer, u8 numCamera, u8 displayMode, u8 displayDest, 
                       u8 camIdx0, u8 pipScale, u8 pipQuadrant, u8 disIdx0, u8 disIdx1, u8 disIdx2, u8 disIdx3,
                       const u8 *camDisp);
// Obsolete: before 2.21.1.
s32 SLFIPSetVideoMode_2_21_0(SLPacketType buffer, u8 numCamera, u8 displayMode, u8 displayDest, 
                       u8 camIdx0, u8 pipScale, u8 pipQuadrant, u8 disIdx0, u8 disIdx1, u8 disIdx2, u8 disIdx3,
                       const u16 *camDisp);

s32 SLFIPSetCommandCamera( SLPacketType buffer, u8 camIdx);

/// Indicates different display output options
typedef enum {
 SL_DISP_ONE          = 0,
 SL_DISP_PIP          = 1,
 SL_DISP_TWO          = 2,
 SL_DISP_FOUR         = 3,
 SL_DISP_BLEND        = 4,
 SL_DISP_STITCH       = 5,
 SL_DISP_SIDE_BY_SIDE = 6,
 SL_DISP_NONE         = 7,  //!< Don't display image
 SL_DISPLAY_MODE_END
} SL_DISPLAY_MODE;

/// Quadrant in which to display PiP 
typedef enum {
  SL_DISP_TR = 0,	//!< TOP RIGHT
  SL_DISP_BR = 1,	//!< BOTTOM RIGHT
  SL_DISP_BL = 2,	//!< BOTTOM LEFT
  SL_DISP_TL = 3, //!< TOP LEFT
  SL_DISP_QUADRANT_END
} SL_DISP_QUADRANT;

/// Size of PiP window
typedef enum {
  PIP_SCALE_NONE = 0,      //!< No pip
  PIP_SCALE_1_4 = 1,       //!< Quarter sized full image
  PIP_SCALE_3_8 = 2,       //!< Three Eights sized full image
  PIP_SCALE_1_2 = 3,       //!< Half sized full image
  PIP_SCALE_TRACK_1_4 = 4, //!< Quarter sized track region
  PIP_SCALE_TRACK_3_8 = 5, //!< Three Eights sized track region
  PIP_SCALE_TRACK_1_2 = 6  //!< Half sized track region
} SL_PIP_SCALE;

s32 SLFIPSendStitchParams(SLPacketType buffer, u8 cameraIndex, 
                          u8 up, u8 right, u8 down, u8 left, u8 rotation, u8 zoom, bool reset = false);
s32 SLFIPGetStitchParams(SLPacketType buffer);
s32 SLFIPCurrentStitchParams(SLPacketType buffer, u8 cameraIndex, u16 up, u16 right, u16 down, u16 left,
                          u8 rotation, u8 zoom);

//! Deprecated - Use SLFIPSetDetectionParams
s32 SLFIPSetMotDetParams(SLPacketType buffer, u8 mode, int debugCode, u8 threshold, 
                         u8 manualThreshold = 0, u8 manualWatchFrames = 0, u8 suspScore = 0, u8 frameStep = 1, 
                         u8 modeUpperByte = 0, u16 minTemperature = 0, u16 maxTemperature = 0);
//! Deprecated - Use SLFIPGetDetectionParams
s32 SLFIPGetMotDetParams(SLPacketType buffer);
s32 SLFIPSetDetectionParams(SLPacketType buffer, u8 mode, int debugCode, u8 threshold, 
                         u8 manualThreshold = 0, u8 manualWatchFrames = 0, u8 suspScore = 0, u8 frameStep = 1, 
                         u8 modeUpperByte = 0, u16 minTemperature = 0, u16 maxTemperature = 0);
s32 SLFIPGetDetectionParams(SLPacketType buffer);


/* Set Blend Parameters (0x2F) */
s32 SLFIPSetBlendParams(SLPacketType buffer, u8 mode, bool absOff, s8 vertical, s8 horizontal,
                        u8 rotation, u8 zoom, u8 delay, u8 amt, u8 hue, u8 grey, bool reset, u8 warpIdx,
                        u8 fixedIdx, u8 usePresetAlign = 0, u8 presetAlignIndex = 0, bool absZoom = false, u8 hzoom = 0);
s32 SLFIPGetBlendParams(SLPacketType buffer);
s32 SLFIPCurrentBlendParams(SLPacketType buffer, u8 mode, u8 absZoom, u8 up, u8 right, u8 down, u8 left,
                            u8 rotation, u8 zoom, u8 delay, u8 amt, u8 hue, u8 grey, u8 eoIdx, u8 irIdx, u8 hzoom);

s32 SLFIPShiftSelectedTarget(SLPacketType buffer);
s32 SLFIPGetPrimaryTrackIndex(SLPacketType buffer);
s32 SLFIPCurrentPrimaryTrackIndex(SLPacketType buffer, u8 index);
s32 SLFIPDesignateSelectedTrackPrimary(SLPacketType buffer);
s32 SLFIPStopSelectedTrack(SLPacketType buffer);
s32 SLFIPModifyTargetIndex(SLPacketType buffer, u8 index, u8 flags);

s32 SLFIPSetDisplayAdjustments(SLPacketType buffer, s16 ratio8, s16 pan, s16 tilt);

//!< Communication Port Indexes
typedef enum {
  SL_PORT_SERIAL0                     = 0, //!< Serial port 0
  SL_PORT_SERIAL1                     = 1, //!< Serial port 1
  SL_PORT_ETHERNET_C2                 = 2, //!< Ethernet port for command and control responses (and telemetry in LIFO mode)
  SL_PORT_I2C                         = 3, //!< The I2C port  
  SL_PORT_SERIAL2                     = 4, //!< Serial port 2 (SLA1500 only, can be used to control TAU if connected)
  SL_PORT_RESERVED_ETHERNET_TELEMETRY = 5, //!< Just a placeholder for SL_PF_ETHERNET_TELEMETRY
  SL_PORT_SERIAL3                     = 6, //!< 3000: VIP1 serial (used if SONY is connected to VIP1 (close to Ethernet)).
  SL_PORT_SERIAL4                     = 7, //!< 3000: 3000 has total 5 serials(?).
  SL_PORT_SERIAL5                     = 8, //!< Reserved for future.
  SL_PORT_SERIAL6                     = 9, //!< Reserved for future.
  SL_PORT_SERIAL7                     = 10,//!< Reserved for future.
  SL_PORT_PASS_THROUGH_LOG            = 11,//!< Pass through log file opened with Set SD Card Recording Parameters (0x1E)
  SL_PORT_FIP2                        = 12,//!< Reserved for external programs to do FIP with VideoTrack
  SL_PORT_DIGI_VIDEO_POST_LINES       = 13,//!< 3000: reading 2 lines of y buffer after yhigh and parsing fip commands
  SL_PORT_END
} SLPortID;

SLINLINE static bool SLFIPIsSerial(SLPortID port)
{
  switch (port) {
    case SL_PORT_SERIAL0:
    case SL_PORT_SERIAL1:
    case SL_PORT_SERIAL2:
    case SL_PORT_SERIAL3:
    case SL_PORT_SERIAL4:
    case SL_PORT_SERIAL5:
    case SL_PORT_SERIAL6:
    case SL_PORT_SERIAL7:
      return true;
    default:
      break;
  }
  return false;
}

//!< Describes which ports a message will go out on
typedef enum {
  SL_PF_NONE        = 0,                      // Use the port id that the command came from
  SL_PF_SERIAL0     = (1<<SL_PORT_SERIAL0),
  SL_PF_SERIAL1     = (1<<SL_PORT_SERIAL1),
  SL_PF_ETHERNET_C2 = (1<<SL_PORT_ETHERNET_C2),
  SL_PF_I2C         = (1<<SL_PORT_I2C),
  SL_PF_SERIAL2     = (1<<SL_PORT_SERIAL2),
  SL_PF_ETHERNET_TELEMETRY = (1<<SL_PORT_RESERVED_ETHERNET_TELEMETRY),
  SL_PF_SERIAL3     = (1<<SL_PORT_SERIAL3),
  SL_PF_SERIAL4     = (1<<SL_PORT_SERIAL4),
  SL_PF_SERIAL5     = (1<<SL_PORT_SERIAL5),
  SL_PF_SERIAL6     = (1<<SL_PORT_SERIAL6),
  SL_PF_SERIAL7     = (1<<SL_PORT_SERIAL7),
  SL_PF_SERIAL_ALL  = SL_PF_SERIAL0 | SL_PF_SERIAL1 | SL_PF_SERIAL2 | SL_PF_SERIAL3 | SL_PF_SERIAL4 | SL_PF_SERIAL5 | SL_PF_SERIAL6 | SL_PF_SERIAL7,
  SL_PF_DEFAULT_SEND_PORTS = (1<<SL_PORT_SERIAL0)|(1<<SL_PORT_ETHERNET_C2),
  SL_PF_LATENCY     = (0x8000)  // Internally used to test latency.
} SLPortFlag;

//!< Descibes different functionality for a port
typedef enum {
 SL_PORTMODE_NONE = 0x00, //!< Default port operation
 SL_PORTMODE_ECHO = 0x01  //!< Indicates port will echo all data to another port
} SLPortMode;

/*!< Common baud rates for serial port communications
 */
typedef enum {
  SL_BAUD_4800    = 0,  // 20131203 TT: Only for 1500/3000.  
  SL_BAUD_9600    = 1,
  SL_BAUD_38400   = 2,
  SL_BAUD_57600   = 3,
  SL_BAUD_115200  = 4,
  SL_BAUD_19200   = 5
} SLBaudRate;

typedef enum {
  SL_PARITY_NONE = 0,
  SL_PARITY_ODD  = 1,
  SL_PARITY_EVEN = 2
} SLParity;

//!< Describes what kind of command and control or data is allowable on a port
typedef enum {
  SL_PROTOCOL_DEFAULT       = 0,   //!< FIP or for No protocol -- such as SONY camera
  SL_PROTOCOL_AQUARIUS      = 1,   //!< Acquarius & SLA FIP
  SL_PROTOCOL_FLIR_TAU      = 2,
  SL_PROTOCOL_RESERVED      = 3,
  SL_PROTOCOL_NOT_USED      = 4,   //!< Port is not used by VideoTrack (so can be used by other programs, such as gimble control).
  SL_PROTOCOL_TCP           = 5,   //!< Used for TCP-Serial pass-through mode.
  SL_PROTOCOL_PASS_THROUGH  = 6,
  SL_PROTOCOL_KLV           = 7,   //!< KLV data received on serial or udp receive port injected into MP2TS stream
  SL_PROTOCOL_SER_TO_SER    = 8,   //!< Serial-to-serial pass through (pipe 2 serial ports).
  SL_PROTOCOL_NMEA          = 9    //!< NMEA 0138 parsing and convert to Metadata
} SLProtocolType;

enum {
  SL_ENABLE_STABILIZATION = 0x01, //!< Bit to ENable stabilization
  SL_DISABLE_REGISTRATION = 0x02, //!< Bit to DISable registration, stabilization and tracking
  SL_DISABLE_BACKGROUND   = 0x04, //!< 0 - Previous History is used as background (gray), 1 - Black background
  SL_RESERVED_AUTO_BIAS   = 0x08, //!< Reserved bit in stabilization mode for return in Current Stabilization Parameters
  SL_COLOR_BACKGROUND     = 0x10, //!< 1 - Previous History is used as background with color and no blur
  SL_SM_NO_ZTT_PANTILT    = 0x20, //!< 1 - don't apply pan-tilt when in zoom to track mode, 0 - do
  SL_SM_RESERVED          = 0x40,
  SL_SM_RESERVED2         = 0x80, 
  SL_BACKGROUND           = SL_DISABLE_BACKGROUND | SL_COLOR_BACKGROUND  //!< Mask for background bits
}; // SM = Stabilization Mode

s32 SLFIPCommandPassthrough(SLPacketType buffer, SLPortID destPort, u8 * data, u8 numBytes);
s32 SLFIPConfigSerialPassthrough(SLPacketType buffer, SLPortID destPort, 
                                 SLBaudRate baud, u8 dataBits, u8 stopBits, SLParity parity,
                                 u8 maxPacket, u8 maxDelay, SLProtocolType protocol,
                                 u16 inputUdpPort, u32 udpDestinationAddr, u16 udpDestinationPort,
                                 u16 udpAttNavPort=65100);
s32 SLFIPGetSerialPassthrough(SLPacketType buffer, SLPortID destPort);
s32 SLFIPCurrentSerialPassthrough(SLPacketType buffer, SLPortID destPort,
                                   SLBaudRate baud, u8 dataBits, u8 stopBits, SLParity parity,
                                   u8 maxPacket, u8 maxDelay, SLProtocolType protocol,
                                   u16 inputUdpPort, u32 udpDestinationAddr, u16 udpDestinationPort,
                                   u16 udpAttNavPort=65100);

// Constants for 'flags' in SLFIPSetAcqParams (1500 and 3000)
enum {
  SLFIP_ACQPARAM_FLG_INPUT_DATA_MASK= 0x0007,   // Bit[0:2] Input Data Mode (platform dependent)
  SLFIP_ACQPARAM_FLG_INTERLACE      = 0x0008,   // Bit[3]   1=interlace, 0=progressive
  SLFIP_ACQPARAM_FLG_VSYNC_POL      = 0x0010,   // Bit[4]   Vertical Sync polarity
  SLFIP_ACQPARAM_FLG_HSYNC_POL      = 0x0020,   // Bit[5]   Horizontal Sync polarity
  SLFIP_ACQPARAM_FLG_INIT_CODE_MAWK = 0x03C0,   // Bit[9:6] Camera Initialization Routine
  SLFIP_ACQPARAM_FLG_INIT_SONY      = 0x0001,   // Bit[9:6] Init Sony
  SLFIP_ACQPARAM_FLG_INIT_DRS       = 0x0002,   // Bit[9:6] Init DRS
  SLFIP_ACQPARAM_FLG_INIT_QUARK_FFC = 0x0003,   // Bit[9:6] Init Quark and other FFC non Tau cameras
  SLFIP_ACQPARAM_FLG_INIT_PHOTON    = 0x0004,   // Bit[9:6] Init Photon
  SLFIP_ACQPARAM_FLG_INIT_CAMERALINK_LOWSPEED= 0x0005,   // Bit[9:6] Init Lowspeed Cameralink camera (slower pixel clock)
  SLFIP_ACQPARAM_FLG_INIT_TAU       = 0x0006,   // Bit[9:6] Init TAU
  SLFIP_ACQPARAM_FLG_INIT_ZAFIRO    = 0x0007,   // Bit[9:6] Init DRS Zafiro

  // Bit [11:10] - one of the following three, or 0 (none of these)
  SLFIP_ACQPARAM_FLG_DATA_VALID     = 0x0400,   // Bit[11:10] 01b = use data-valid siganl (3000 only)
  SLFIP_ACQPARAM_FLG_EMBEDDED       = 0x0800,   // Bit[11:10] 10b = use embedded sync signal (3000 only)
  SLFIP_ACQPARAM_FLG_HW_CROP        = 0x0C00,   // Bit[11:10] 11b = use hardware crop (3000 only)
                                                // Bit[11:10] 00b = none of the above (3000 only)

  SLFIP_ACQPARAM_FLG_3000_BYTE_SWAP = 0x4000,   // Bit[14]  Reserved for 3000 -- swap upper & lower bytes.
  SLFIP_ACQPARAM_FLG_3000_RSRVD     = 0x8000    // Bit[15]  Reserved for 3000 -- Shift 16bit data right to get 8bit data.
};

s32 SLFIPSetAcqParams( SLPacketType buffer, u8 cameraIndex, u8 cameraType, 
                        u16 high, u16 wide, u8 bitdepth, u16 vertFrontPorch=0, u16 horzFrontPorch=0, u16 flags=0, u8 frameStep = 0,
                        u16 validRow = 0, u16 validCol = 0, u16 validHigh = 0, u16 validWide = 0);
s32 SLFIPSetAdvancedCaptureParams( SLPacketType buffer, u16 horzControl, s16 xStart, s16 yStart );
s32 SLFIPCurrentAdvancedCaptureParams( SLPacketType buffer, u16 horzControl, s16 xStart, s16 yStart );
s32 SLFIPGetAcqParams( SLPacketType buffer, u8 cameraIndex );
s32 SLFIPCurrentAcqParams( SLPacketType buffer, u8 numCamera, u8 cameraIndex, u8 cameraType, u16 high, u16 wide, u8 bitdepth,
                          u16 vertFrontPorch = 0, u16 horzFrontPorch=0, u16 flags=0, u8 frameStep = 0,
                          u16 validRow = 0, u16 validCol = 0, u16 validHigh = 0, u16 validWide = 0);
enum {
  SLFIP_METADATA_STATIC_MISSION_ID = 0,
  SLFIP_METADATA_STATIC_PLATFORM_DESIGNATION = 1,
  SLFIP_METADATA_STATIC_IMAGE_SOURCE_SENSOR = 2,
  SLFIP_METADATA_STATIC_IMAGE_COORDINATE_SYSTEM = 3,
  SLFIP_METADATA_STATIC_SECURITY_CLASSIFICATION = 4,
  SLFIP_METADATA_STATIC_SECURITY_CLASSIFYING_COUNTRY_CODING_METHOD = 5,
  SLFIP_METADATA_STATIC_SECURITY_CLASSIFYING_COUNTRY = 6,
  SLFIP_METADATA_STATIC_SECURITY_SCISHI_INFORMATION = 7,
  SLFIP_METADATA_STATIC_SECURITY_CAVEATS = 8,
  SLFIP_METADATA_STATIC_SECURITY_RELEASING_INSTRUCTIONS = 9,
  SLFIP_METADATA_STATIC_SECURITY_OBJECT_COUNTRY_CODING_METHOD = 10,
  SLFIP_METADATA_STATIC_SECURITY_OBJECT_COUNTRY = 11,
  SLFIP_METADATA_STATIC_MOTION_IMAGERY_CORE_IDENTIFIER = 12,
  SLFIP_METADATA_STATIC_PLATFORM_TAIL_NUMBER = 13,
  SLFIP_METADATA_STATIC_TARGET_ERROR_ESTIMATE_CE90 = 14,
  SLFIP_METADATA_STATIC_TARGET_ERROR_ESTIMATE_LE90 = 15,
  SLFIP_METADATA_STATIC_GENERIC_FLAG_DATA_01 = 16,
  SLFIP_METADATA_STATIC_PLATFORM_CALL_SIGN = 17
};

s32 SLFIPSetMetadataValues(SLPacketType buffer, 
                          u16 enables, u64 utcTime, u16 heading, s16 pitch, s16 roll,
                          s32 lat, s32 lon, u16 alt, u16 hfov, u16 vfov,
                          u32 az, s32 el, u32 sensorRoll, s32 dispId = 0);
s32 SLFIPCurrentMetadataValues(SLPacketType buffer, 
                          u64 utcTime, u16 heading, s16 pitch, s16 roll,
                          s32 lat, s32 lon, u16 alt, u16 hfov, u16 vfov,
                          u32 az, s32 el, u32 sensorRoll, s32 dispId = 0);

s32 SLFIPSetMetadataStaticValues(SLPacketType buffer, 
                                u8 type, u8 len, char *str, s32 dispId = 0);
s32 SLFIPCurrentMetadataStaticValue(SLPacketType buffer, 
                                    u8 type, u8 len, char *str, s32 dispId = 0);

s32 SLFIPSetMetadataFrameValues(SLPacketType buffer, 
                          u16 enables, s32 frameCenterLat, s32 frameCenterLon, u16 frameCenterEl, u16 frameWidth, u32 slantRange);
s32 SLFIPSetMetadataFrameValues(SLPacketType buffer, 
                          u16 enables, s32 frameCenterLat, s32 frameCenterLon, u16 frameCenterEl, u16 frameWidth, u32 slantRange,
                          u8 userSuppliedFlags);
s32 SLFIPSetMetadataFrameValues(SLPacketType buffer, 
                          u16 enables, s32 frameCenterLat, s32 frameCenterLon, u16 frameCenterEl, u16 frameWidth, u32 slantRange,
                          u8 userSuppliedFlags, s32 targetLat, s32 targetLon, u16 targetEl, u8 targetHeight, u8 targetWidth);
s32 SLFIPSetMetadataFrameValues(SLPacketType buffer, 
                          u16 enables, s32 frameCenterLat, s32 frameCenterLon, u16 frameCenterEl, u16 frameWidth, u32 slantRange,
                          u8 userSuppliedFlags, s32 targetLat, s32 targetLon, u16 targetEl, u8 targetHeight, u8 targetWidth,
                          s16 offsetCornerLat1, s16 offsetCornerLon1, s16 offsetCornerLat2, s16 offsetCornerLon2,
                          s16 offsetCornerLat3, s16 offsetCornerLon3, s16 offsetCornerLat4, s16 offsetCornerLon4, s32 dispId = 0);
s32 SLFIPCurrentMetadataFrameValues(SLPacketType buffer, s32 frameCenterLat, s32 frameCenterLon,
                                    u16 frameCenterEl, u16 frameWidth, u32 slantRange,
                                    u8 userSuppliedFlags, s32 targetLat, s32 targetLon, u16 targetEl, u8 targetHeight, u8 targetWidth,
                                    s16 offsetCornerLat1, s16 offsetCornerLon1, s16 offsetCornerLat2, s16 offsetCornerLon2,
                                    s16 offsetCornerLat3, s16 offsetCornerLon3, s16 offsetCornerLat4, s16 offsetCornerLon4, s32 dispId = 0);

s32 SLFIPSetMetadataRate(SLPacketType buffer, u64 enables, u8 frameStep, s32 dispId = 0);
s32 SLFIPCurrentMetadataRate(SLPacketType buffer, u8 idx, u8 frameStep, s32 dispId = 0);

// Metadata Overlays (0x84)
s32 SLFIPMetadataOverlays(SLPacketType buffer, u8 group0, u8 group1, u8 group2, u8 group3, u8 group4, u8 group5, u8 group6, u8 group7, u8 color = 0xFE, u8 fontID = 0, u8 hScale = 32, u8 vScale = 32); 

s32 SLFIPMTISetParameters(SLPacketType buffer, u8 debugLog, u8 thresh, u8 threshMode, u8 runningRate8, u8 viewRunningImage, u8 postProcessPeaks, u8 freakoutDetect);

//Hack to solve this problem:
//target bit rate parameter changes don't take affect unless iframe interval changes.
//solution:  when user commands target bit rate paramter change:
// 1.  Change iframe interval to something random and set target bit rate to desired target bit rate
// 2.  Change iframe interval back to old iframe interval.
// Bit 2 used to force I-frame generation, and is reset following application
typedef enum {
  HPS_NORMAL = 0,
  HPS_ALTERED = 1,
  HPS_FORCE_IFRAME = 2
} SL_H264_PARAM_STATE;

// TODO:  Remove this and pass without the struct?
// CAUTION: Regarding SLH264Params, do not use downSampleFactor and frameStep in here, as they are actualy being ignored.
// only parameters that are passed on are:
// targetBitRate
// intraFrameInterval
// lfDisableIdc
// airMbPeriod
// sliceRefreshRowNumber
//>>>Even then, be wary of the SIGNED bitrate..
typedef struct {
  s16 /* ignored */ high;
  s16 /* ignored */ wide;
  s32 targetBitRate;
  u8  intraFrameInterval;
  u8  lfDisableIdc;           // 0 filter all edges, 1 disable all filtering, 2 disable slice edge filtering
  u8  airMbPeriod;            // Adaptive Intra Refesh MB Period
  u8  sliceRefreshRowNumber;  // Number of rows to be coded as each intra slice
  u8  /* ignored */ downSampleFactor; // downsample factor (valid 0,1,2,4) 0,1=>no downsample, 2,3=>2x downsample, 4,...=>4x downsample
  u8  /* ignored */ frameStep;        // step between frames.  1=every frame, 2=every other, 3=every 3rd, etc.
  u8  /* ignored */ state;
  u8  flags;  // for profile and bit rate control (3000 only).
} SLH264Params;

#if 0
/*! Bit mask of specific modes */
typedef enum {
  SL_H264_ENABLE_UNKOWN       = 0x00,
  SL_H264_ENABLE_LOOPFILTER   = 0x01,
  SL_H264_ENABLE_QUARTERPEL   = 0x02,
  SL_H264_ENABLE_SLICEREFRESH = 0x04
  // max 16 modes
} SLH264Modes;

typedef enum {
  SL_H264_FPS_30    = 0,
  SL_H264_FPS_15    = 1,
  SL_H264_FPS_10    = 2,
  SL_H264_FPS_7_5   = 3
  // max 16 different frame rates
} SLH264FrameRates;

typedef enum {
  SL_H264_DS_1    = 0,  // no down sample
  SL_H264_DS_1_5  = 1,  // 1.5x 
  SL_H264_DS_2    = 2,  // 2x
  SL_H264_DS_4    = 3   // 4x
  // max 16 different down samples
} SLH264DownSample;
#endif

s32 SLFIPSetH264Params(SLPacketType buffer, SLH264Params *params, int dispId = 0);
s32 SLFIPCurrentH264Params(SLPacketType buffer, SLH264Params *params, int dispId = 0);
s32 SLFIPGetH264Params(SLPacketType buffer );

s32 SLFIPAckNak(SLPacketType buffer, u8 id, SLFIP_ACK_NAK ackNak);

typedef enum {
  SLSNAP_MODE_FTP     = 0,
  SLSNAP_MODE_MICROSD = 1
} SLSNAP_MODE;

typedef enum {
  SLSNAP_JPEG = 0,
  SLSNAP_PNG  = 1
} SLSnapShotFormat;

//!< Type of image to be sent as snap shot
typedef enum {
  SL_SNAP_SRC = 1, //!< The source or original captured image
  SL_SNAP_DST = 2 //!< The sabilized or processed image.
} SLSnapShotSource;

s32 SLFIPSetSnapShot(SLPacketType buffer, SLSNAP_MODE mode, SLSnapShotFormat format, SLSnapShotSource source, u8 quality, u8 downSample,
                     u32 ipAddr, u32 port, const char * userName, const char * password);
s32 SLFIPDoSnapShot(SLPacketType buffer, u8 frameStep, u8 numFrames, const char * fileName, u8 snapAllCamerasMask=0);
s32 SLFIPCurrentSnapShot(SLPacketType buffer, SLSNAP_MODE mode, SLSnapShotFormat format, u8 source, u8 quality, u8 downSample,
                         u32 ipAddr, u32 port, const char * userName, const char * password,
                         u8 frameStep, u8 numFrames, const char *fileName);

/*!
 * Construct a FIPEX message for setting binary KLV data from outside
 * The resulting FIPEX message may use 1 or 2 bytes for length depending on the klvLen.
 */
s32 SLFIPEXBinaryKlv(FIPEX_PACKET *fpx, const u8* klvData, u32 klvLen, s32 dispId=0);

/*!
 * Construct a message for setting appended KLV data from outside
 * this is not yet officially a MISB standard
 */
s32 SLFIPSetAppendedMetadata(SLPacketType buffer, const u8* data, u8 length, u16 dispId=0);
s32 SLFIPCurrentAppendedMetadata(SLPacketType buffer, const u8* data, u8 length, u16 dispId);

////////////////////////////////////////////////////////////////////////////////////////////////
// Lens control commands, zoom, focus, mechanism reset and autofocus
////////////////////////////////////////////////////////////////////////////////////////////////



s32 SLFIPSetLensMode(SLPacketType buffer, SLLensMode lensMode, u8* data, u16 dataLen);
s32 SLFIPCurrentLensStatus(SLPacketType buffer,  u16 focusPos, u16 zoomPos, u8 focusLimits, u8 zoomLimits);
s32 SLFIPSetLensParams(SLPacketType buffer, u8 lensType, u8 AFMetricRegionSize,
                                   u8 zoomTrackFocus, u8 autofocusMethod, u16 autofocusRateAdjust,
                                   u8 autofocusChangePct, u8 zoomSpeed=0, u8 focusSpeed=0, u8 comPortNum = 0);

s32 SLFIPCurrentLensParams(SLPacketType buffer, u8 lensType, u8 AFMetricRegionSize,
                                   u8 zoomTrackFocus, u8 autofocusMethod, u16 autofocusRateAdjust,
                                   u8 autofocusChangePct, u8 zoomSpeed=0, u8 focusSpeed = 0, 
                                   u8 comPortNum = 0);

typedef enum {
  SL_SYNC_FAST = 0,
  SL_SYNC_TELEMETRY = 1
} SLSyncType;

s32 SLFIPAnalyzeRenderSync(SLPacketType buffer, SLSyncType type, u8 idx, u64 ts);

#ifndef _TMS320C6X
#pragma pack(pop)
#endif

///////////////////////////////////////////////////////////////////////////////
// NOTE:  Don't put this inside of the pragma pack or it will not match with SLTrackRes
// NOTE:  Must match with SLTrackRes in slstabtrack_c.h
/*! Single track result from Tracking Module
 */
typedef struct {
  f32 row;              //!< Row at which model was found
  f32 col;              //!< Column at which model was found
  s16 high;             //!< Model height (may differ from startArgs when searching in scale)
  s16 wide;             //!< Model width (may differ from startArgs when searching in scale)
  s16 angle7;           //!< Angle at which the model was found (-180 to +180 degrees)*128
  s16 scale8;           //!< Scale at which the model was found * 256.  (1.0*256 inidicates no change)
  s16 velRow8;          //!< Track momentum row - velocity or position depending on track mode
  s16 velCol8;          //!< Track momentum row - velocity or position depending on track mode
  s16 frame;            //!< Nth frame processed
  u8 confidence;        //!< Confidence level (0...100) of model found
  u8 resultState;       //!< indicates current state of track result
  u8 flags;             //!< Primary/selected flags - 0x01 primary bit, 0x02 selected bit
  u8 idx;               // Index of original track
  u8 trainState;        //!< indicates current state of track training
  u8 mti;               // Mti payload
} SLFIPTrackRes;

///////////////////////////////////////////////////////////////////////////////
// NOTE:  Must match with SLTrackPixStats in slstabtrack_c.h
typedef struct {
  u16 mean;
  u16 max;
  u16 min;
  u8 idx;   // Identifier (different than index)
  u8 pad;   // For alignment
} SLFIPSLTrackPixStatsPt;

// display Resolution for HDMI and HD-SDI output, Analog format
typedef enum {
  SL_DISPLAY_RES_NTSC720 = 1,  //!< 720 wide non-square pixel NTSC image.
  SL_DISPLAY_RES_NTSC640 = 2,  //!< 640 wide square pixel NTSC image.
  SL_DISPLAY_RES_720P60  = 4,    // display resolution 720P with refresh rate of 60
  SL_DISPLAY_RES_1080P30 = 5,    // display resolution 1080P with refresh rate of 30
  SL_DISPLAY_RES_1080P25 = 6,    // display resolution 1080P with refresh rate of 25
  SL_DISPLAY_RES_1080P60 = 7,    // display resolution 1080P with refresh rate of 60
  SL_DISPLAY_RES_720P50  = 8,    // display resolution 720P with refresh rate of 50
  SL_DISPLAY_RES_1080P50 = 9,    // display resolution 1080P with refresh rate of 50
  SL_DISPLAY_RES_PAL720  = 10,   //!< PAL image
  SL_DISPLAY_RES_720P59  = 11,   // display resolution 720P with refresh rate of 59.94
  SL_DISPLAY_RES_1080P29 = 12,   // display resolution 1080P with refresh rate of 29.97
  SL_DISPLAY_RES_1080P59 = 13,   // display resolution 1080P with refresh rate of 59.94
  SL_DISPLAY_RES_1080I60 = 14,   // display resolution 1080P with refresh rate of 60
  SL_DISPLAY_RES_1080I59 = 15,   // display resolution 1080P with refresh rate of 59.94
} SLDisplayRes;
//@} End of group


//!< Bit field describing which types are warnings are enabled
typedef enum {
  SL_USER_WARN_NONE     = 0x0000,
  SL_USER_WARN_FYI      = 0x0001,
  SL_USER_WARN_WARN     = 0x0004,
  SL_USER_WARN_CRITICAL = 0x0008,
  SL_USER_WARN_ALL      = 0xFFFF  
} SLUserWarnings;

//!< a set of flags enabling specific system status fields (@see SystemStatus)
typedef enum {
  SL_SYSSTAT_ENABLE_NONE     = 0x0000,
  SL_SYSSTAT_ENABLE_ALL      = 0xFFFF  
} SLSystemStatusFields;

// string length is maxiumum 130 characters? 
s32 SLFIPSendUserWarningMessage(SLPacketType buffer, SLUserWarnings level, const char *str);
s32 SLFIPSetUserWarningLevel(SLPacketType buffer, SLUserWarnings warningLevelBits);

s32 SLFIPSendDetailedTimingMessageEx(SLPacketType buffer, s16 reserved, const u8 *str, u32 len);

s32 SLFIPSendSystemStatusMessage(SLPacketType buffer, u64 errorFlags, s16 temperature, u8 load0, u8 load1, u8 load2, u8 load3);
s32 SLFIPSetSystemStatusMode(SLPacketType buffer, u16 systemStatusBits, u32 systemDebugBits);

s32 SLFIPLandingAid(SLPacketType buffer, u8 mode, u16 camHFovDeg8, u32 blackTargetSize16, u32 whiteTargetSize16,
                    u8 matchThresh, u8 keepOutMode, u32 keepOutRadius16, 
                    u16 ctrlParam0, u16 ctrlParam1, u16 ctrlParam2, u16 ctrlParam3);
s32 SLFIPUnpackLandingAid(u8 *mode, u16 *camHFovDeg8, u32 *blackTargetSize16, u32 *whiteTargetSize16,
                          u8 *matchThresh, u8 *keepOutMode, u32 *keepOutRadius16,
                          u16 *ctrlParam0, u16 *ctrlParam1, u16 *ctrlParam2, u16 *ctrlParam3, const SLPacketType buffer);
s32 SLFIPLandingPosition(SLPacketType buffer, u8 camIdx, s16 col, s16 row, u16 angleDeg7, u32 distance16, u8 confidence, u16 camHFovDeg8, 
                         s16 capWide, s16 capHigh, u16 ctrlParam0, u16 ctrlParam1, u16 ctrlParam2, u16 ctrlParam3, 
                         u8 keepOutState, u8 keepOutConfidence, u16 keepOutSz, u32 keepOutDist16, u16 flags=0, u32 frameId=0xFFFFFFFF, u64 timestamp=0);
s32 SLFIPUnpackLandingPosition(SLPacketType buffer, u8 *camIdx, s16 *col, s16 *row, u16 *angleDeg7, u32 *distance16, u8 *confidence,
                               u16* camHFovDeg8, s16* capWide, s16* capHigh, u16* ctrlParam0, u16* ctrlParam1, u16* ctrlParam2, u16* ctrlParam3,    
                               u8* keepOutState, u8* keepOutConfidence, u16* keepOutSz, u32* keepOutDist16, u64* timeStamp = 0, u32* frameIdx = 0);

s32 SLFIPSetGeneric(SLPacketType buffer, u32 cmdIndex, u16 extraLen, const u8 *extraBuf);


s32 SLFIPDigiVidParseParams(SLPacketType buffer, const u8 camIdx, const bool enable, const bool startAt0, const bool oneFipPerImage, const bool usePixelValue);


s32 SLFIPStreamingControl(SLPacketType buffer, u16 streamingMask);

s32 SLFIPSetExternalProgram(SLPacketType buffer, const u8 programType, const char* prog1 = "", const char* prog2 = "", const char* prog3 = "");


