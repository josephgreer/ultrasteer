/*
 * Copyright (C)2007-2016 SightLine Applications Inc
 * SightLine Applications Library of signal, vision, and speech processing
 * http://www.sightlineapplications.com
 *------------------------------------------------------------------------*/

#pragma once
#include "sltypes.h"

/*!@file
 *  SLAImage.
 */

#define KLV_MAX_BYTE_ARRAY 127
#define KLV_MAX_NUMBER_OF_TARGETS 110

#ifdef __cplusplus
extern "C" {
#endif

/* KEY = MISB TAG */
enum {
  SL_LDS_KEY_CHECKSUM = 1,
  SL_LDS_KEY_UTCTIME = 2,
  SL_LDS_KEY_MISSIONID = 3,
  SL_LDS_KEY_PLATFORMTAILNUMBER = 4,
  SL_LDS_KEY_PLATFORMHEADINGANGLE = 5,
  SL_LDS_KEY_PLATFORMPITCHANGLE = 6,
  SL_LDS_KEY_PLATFORMROLLANGLE = 7,
  SL_LDS_KEY_PLATFORMTRUEAIRSPEED = 8,
  SL_LDS_KEY_PLATFORMINDICATEDAIRSPEED = 9,
  SL_LDS_KEY_PLATFORMDESIGNATION = 10,
  SL_LDS_KEY_IMAGESOURCESENSOR = 11,
  SL_LDS_KEY_IMAGECOORDINATESYSTEM = 12,
  SL_LDS_KEY_SENSORLATITUDE = 13,
  SL_LDS_KEY_SENSORLONGITUDE = 14,
  SL_LDS_KEY_SENSORALTITUDE = 15,
  SL_LDS_KEY_SENSORHORIZONTALFIELDOFVIEW = 16,
  SL_LDS_KEY_SENSORVERTICALFIELDOFVIEW = 17,
  SL_LDS_KEY_SENSORRELATIVEAZIMUTHANGLE = 18,
  SL_LDS_KEY_SENSORRELATIVEELEVATIONANGLE = 19,
  SL_LDS_KEY_SENSORRELATIVEROLLANGLE = 20,
  SL_LDS_KEY_SLANTRANGE = 21,
  SL_LDS_KEY_TARGETWIDTH = 22,
  SL_LDS_KEY_FRAMECENTERLATITUDE = 23,
  SL_LDS_KEY_FRAMECENTERLONGITUDE = 24,
  SL_LDS_KEY_FRAMECENTERELEVATION = 25,
  SL_LDS_KEY_OFFSETCORNERLATITUDEPOINT1 = 26,
  SL_LDS_KEY_OFFSETCORNERLONGITUDEPOINT1 = 27,
  SL_LDS_KEY_OFFSETCORNERLATITUDEPOINT2 = 28,
  SL_LDS_KEY_OFFSETCORNERLONGITUDEPOINT2 = 29,
  SL_LDS_KEY_OFFSETCORNERLATITUDEPOINT3 = 30,
  SL_LDS_KEY_OFFSETCORNERLONGITUDEPOINT3 = 31,
  SL_LDS_KEY_OFFSETCORNERLATITUDEPOINT4 = 32,
  SL_LDS_KEY_OFFSETCORNERLONGITUDEPOINT4 = 33,
  SL_LDS_KEY_ICINGDETECTED = 34,
  SL_LDS_KEY_WINDDIRECTION = 35,
  SL_LDS_KEY_WINDSPEED = 36,
  SL_LDS_KEY_STATICPRESSURE = 37,
  SL_LDS_KEY_DENSITYALTITUDE = 38,
  SL_LDS_KEY_OUTSIDEAIRTEMP = 39,
  SL_LDS_KEY_TARGETLOCATIONLATITUDE = 40,
  SL_LDS_KEY_TARGETLOCATIONLONGITUDE = 41,
  SL_LDS_KEY_TARGETLOCATIONELEVATION = 42,
  SL_LDS_KEY_TARGETTRACKGATEWIDTH = 43,
  SL_LDS_KEY_TARGETTRACKGATEHEIGHT = 44,
  SL_LDS_KEY_TARGETERRORESTIMATECE90 = 45,
  SL_LDS_KEY_TARGETERRORESTIMATELE90 = 46,
  SL_LDS_KEY_GENERICFLAGDATA01 = 47,
  SL_LDS_KEY_SECURITYLOCALMETADATASET = 48,
  SL_LDS_KEY_DIFFERENTIALPRESSURE = 49,
  SL_LDS_KEY_PLATFORMANGLEOFATTACK = 50,
  SL_LDS_KEY_PLATFORMVERTICALSPEED = 51,
  SL_LDS_KEY_PLATFORMSIDESLIPANGLE = 52,
  SL_LDS_KEY_AIRFIELDBAROMETRICPRESSURE = 53,
  SL_LDS_KEY_ELEVATION = 54,
  SL_LDS_KEY_RELATIVEHUMIDITY = 55,
  SL_LDS_KEY_PLATFORMGROUNDSPEED = 56,
  SL_LDS_KEY_GROUNDRANGE = 57,
  SL_LDS_KEY_PLATFORMFUELREMAINING = 58,
  SL_LDS_KEY_PLATFORMCALLSIGN = 59,
  SL_LDS_KEY_WEAPONLOAD = 60,
  SL_LDS_KEY_WEAPONFIRED = 61,
  SL_LDS_KEY_LASERPRFCODE = 62,
  SL_LDS_KEY_SENSORFIEDLOFVIEWNAME = 63,
  SL_LDS_KEY_PLATFORMMAGNETICHEADING = 64,
  SL_LDS_KEY_UASLDSVERSIONNUMBER = 65,
  SL_LDS_KEY_TARGETLOCATIONCOVARIANCEMATRIX = 66,
  SL_LDS_KEY_ALTERNATEPLATFORMLATITUDE = 67,
  SL_LDS_KEY_ALTERNATEPLATFORMLONGITUDE = 68,
  SL_LDS_KEY_ALTERNATEPLATFORMALTITUDE = 69,
  SL_LDS_KEY_ALTERNATEPLATFORMNAME = 70,
  SL_LDS_KEY_ALTERNATEPLATFORMHEADING = 71,
  SL_LDS_KEY_EVENTSTARTTIME = 72,
  SL_LDS_KEY_RVTLOCALDATASET = 73,
  SL_LDS_KEY_VMTILOCALDATASET = 74,
  SL_LDS_KEY_SENSORELLIPSOIDHEIGHT = 75,
  SL_LDS_KEY_ALTERNATEPLATFORMELLIPSOIDHEIGHT = 76,
  SL_LDS_KEY_OPERATIONALMODE = 77,
  SL_LDS_KEY_FRAMECENTERHEIGHTABOVEELLIPSOID = 78,
  SL_LDS_KEY_SENSORNORTHVELOCITY = 79,
  SL_LDS_KEY_SENSOREASTVELOCITY = 80,
  SL_LDS_KEY_IMAGEHORIZONPIXELPACK = 81,
  SL_LDS_KEY_CORNERLATITUDEPOINT1FULL = 82,
  SL_LDS_KEY_CORNERLONGITUDEPOINT1FULL = 83,
  SL_LDS_KEY_CORNERLATITUDEPOINT2FULL = 84,
  SL_LDS_KEY_CORNERLONGITUDEPOINT2FULL = 85,
  SL_LDS_KEY_CORNERLATITUDEPOINT3FULL = 86,
  SL_LDS_KEY_CORNERLONGITUDEPOINT3FULL = 87,
  SL_LDS_KEY_CORNERLATITUDEPOINT4FULL = 88,
  SL_LDS_KEY_CORNERLONGITUDEPOINT4FULL = 89,
  SL_LDS_KEY_PLATFORMPITCHANGLEFULL = 90,
  SL_LDS_KEY_PLATFORMROLLANGLEFULL = 91,
  SL_LDS_KEY_PLATFORMANGLEOFATTACKFULL = 92,
  SL_LDS_KEY_PLATFORMSIDESLIPANGLEFULL = 93,
  SL_LDS_KEY_COREIDENTIFIER = 94,
  SL_LDS_KEY_SARMOTIONIMAGERYMETADATA = 95,

  SL_LDS_KEY_SECURITY_CLASSIFICATION = 1,
  SL_LDS_KEY_SECURITY_CLASSIFYINGCOUNTRYCODINGMETHOD = 2,
  SL_LDS_KEY_SECURITY_CLASSIFYINGCOUNTRY = 3,
  SL_LDS_KEY_SECURITY_SCISHIINFORMATION = 4,
  SL_LDS_KEY_SECURITY_CAVEATS = 5,
  SL_LDS_KEY_SECURITY_RELEASINGINSTRUCTIONS = 6,
  SL_LDS_KEY_SECURITY_OBJECTCOUNTRYCODINGMETHOD = 12,
  SL_LDS_KEY_SECURITY_OBJECTCOUNTRYCODES = 13,
  SL_LDS_KEY_SECURITY_METADATAVERSION = 22,

  SL_LDS_KEY_VMTI_VERSION = 4,
  SL_LDS_KEY_VMTI_FRAME_WIDTH = 8,
  SL_LDS_KEY_VMTI_FRAME_HEIGHT = 9,
  SL_LDS_KEY_VMTI_VTARGET_SERIES = 101,

  SL_LDS_KEY_VTARGET_BOUNDING_BOX_TOP_LEFT = 2,
  SL_LDS_KEY_VTARGET_BOUNDING_BOX_BOTTOM_RIGHT = 3,
  SL_LDS_KEY_VTARGET_TARGET_CONFIDENCE_LEVEL = 5,
  SL_LDS_KEY_VTARGET_TARGET_HISTORY = 6,

  // ASM Appending blob tag (not yet an official tag number, AD 6.22.2016):
  SL_LDS_KEY_APPEND_METADATA = 100,

};


typedef struct KLVBytes {
  u8 len;
  u8 data[KLV_MAX_BYTE_ARRAY];
} KLVBytes;

typedef struct KLVVTargetPack {
  u32 TargetIDNumber;
  u32 BoundingBoxTopLeftPixelNumber;
  u32 BoundingBoxBottomRightPixelNumber;
  u8  TargetConfidenceNumber;
} KLVVTargetPack;

typedef struct KLVVmtiLocalSet {
  u16 nTargets;   // number of targets received up to KLV_MAX_NUMBER_OF_TARGETS
  u16 Version;
  u16 FrameWidth;
  u16 FrameHeight;
  KLVVTargetPack Target[KLV_MAX_NUMBER_OF_TARGETS];
} KLVVmti;

typedef struct KLVSecurityLocalSet {
  u8 Classification;
  u8 ClassifyingCountryCodingMethod;
  KLVBytes ClassifyingCountry;
  KLVBytes SCISHIInformation;
  KLVBytes Caveats;
  KLVBytes ReleasingInstructions;
  u8 ObjectCountryCodingMethod;
  KLVBytes ObjectCountryCodes;
  u16 SecurityMetadataVersion;
} KLVSecurity;


/// KLV data structure
typedef struct KLVData {
                                        // #1 checksum.
  u64 Utctime;                          // #2
  KLVBytes Missionid;                   // #3
  KLVBytes PlatformTailNumber;          // #4
  u32 PlatformHeadingAngle;             // #5 (ext)
  s16 PlatformPitchAngle;               // #6
  s16 PlatformRollAngle;                // #7
  u16  PlatformTrueAirSpeed;            // #8 (ext)
  u16 PlatformIndicatedAirSpeed;        // #9 (ext)
  KLVBytes PlatformDesignation;         // #10
  KLVBytes ImageSourceSensor;           // #11
  KLVBytes ImageCoordinateSystem;       // #12
  s32 SensorLatitude;                   // #13
  s32 SensorLongitude;                  // #14
  u32 SensorAltitude;                   // #15 (ext)
  u32 SensorHorizontalFieldOfView;      // #16 (ext)
  u32 SensorVerticalFieldOfView;        // #17 (ext)
  u64 SensorRelativeAzimuthAngle;       // #18 (ext)
  s32 SensorRelativeElevationAngle;     // #19
  u64 SensorRelativeRollAngle;          // #20 (ext)
  u64 SlantRange;                       // #21 (ext)
  u32 TargetWidth;                      // #22 (ext)
  s32 FrameCenterLatitude;              // #23
  s32 FrameCenterLongitude;             // #24
  u32 FrameCenterElevation;             // #25 (ext)
  s16 OffsetCornerLatitudePoint1;       // #26
  s16 OffsetCornerLongitudePoint1;      // #27
  s16 OffsetCornerLatitudePoint2;       // #28
  s16 OffsetCornerLongitudePoint2;      // #29
  s16 OffsetCornerLatitudePoint3;       // #30
  s16 OffsetCornerLongitudePoint3;      // #31
  s16 OffsetCornerLatitudePoint4;       // #32
  s16 OffsetCornerLongitudePoint4;      // #33
  u8  IcingDetected;                    // #34
  u32 WindDirection;                    // #35 (ext)
  u32 WindSpeed;                        // #36 (ext)
  u32 StaticPressure;                   // #37 (ext)
  u32 DensityAltitude;                  // #38 (ext)
  s32 OutsideAirTemp;                   // #39 (ext)
  s32 TargetLocationLatitude;           // #40
  s32 TargetLocationLongitude;          // #41
  u32 TargetLocationElevation;          // #42 (ext)
  u16 TargetTrackGateWidth;             // #43 (ext)
  u16 TargetTrackGateHeight;            // #44 (ext)
  u32 TargetErrorEstimateCE90;          // #45 (ext)
  u32 TargetErrorEstimateLE90;          // #46 (ext)
  u16 GenericFlagData;                  // #47 (ext)
  KLVSecurityLocalSet SecurityLDS;      // #48
  u32 DifferentialPressure;             // #49 (ext)
  s16 PlatformAngleOfAttack;            // #50
  s16 PlatformVerticalSpeed;            // #51
  s16 PlatformSideSlipAngle;            // #52
  u32 AirfieldBarometricPressure;       // #53 (ext)
  u32 Elevation;                        // #54 (ext)
  u16 RelativeHumidity;                 // #55 (ext)
  u16 PlatformGroundSpeed;              // #56 (ext)
  u64 GroundRange;                      // #57 (ext)
  u32 PlatformFuelRemaining;            // #58 (ext)
  KLVBytes PlatformCallSign;            // #59
  u32 WeaponLoad;                       // #60 (ext)
  u16 WeaponFired;                      // #61 (ext)
  u16 LaserPRFCode;                     // #62
  u8  SensorFieldOfViewName;            // #63
  u32 PlatformMagneticHeading;          // #64 (ext)
  u16  UasLDSVersionNumber;             // #65 (ext)
                                        // #66  TBD per ST 0601.7
  s32 AlternatePlatformLatitude;        // #67
  s32 AlternatePlatformLongitude;       // #68
  u32 AlternatePlatformAltitude;        // #69 (ext)
  KLVBytes AlternatePlatformName;       // #70
  u32 AlternatePlatformHeading;         // #71 (ext)
  u64 EventStartTime;                   // #72
  KLVBytes Rvt;                         // #73  Not parsed in this version
  KLVVmtiLocalSet VMti;                 // #74
  u32 SensorEllipsoidHeight;            // #75 (ext)
  u32 AlternatePlatformEllipsoidHeight; // #76 (ext)
  u8  OperationalMode;                  // #77
  u32 FrameCenterHeightAboveEllipsoid;  // #78 (ext)
  s16 SensorNorthVelocity;              // #79
  s16 SensorEastVelocity;               // #80
  KLVBytes ImageHorizonPixelPack;       // #81  Not parsed in this version
  s32 CornerLatitudePoint1Full;         // #82
  s32 CornerLongitudePoint1Full;        // #83
  s32 CornerLatitudePoint2Full;         // #84
  s32 CornerLongitudePoint2Full;        // #85
  s32 CornerLatitudePoint3Full;         // #86
  s32 CornerLongitudePoint3Full;        // #87
  s32 CornerLatitudePoint4Full;         // #88
  s32 CornerLongitudePoint4Full;        // #89
  s32 PlatformPitchAngleFull;           // #90
  s32 PlatformRollAngleFull;            // #91
  s32 PlatformAngleOfAttackFull;        // #92
  s32 PlatformSideSlipAngleFull;        // #93
  KLVBytes MotionImageryCoreIdentifier; // #94
  KLVBytes SARMotionImageryMetadata;    // #95  Not parsed in this version
  KLVBytes ASMAappend;                  // #100 App Specific Metadata Append tag. Not yet official (ad 6.22.2016)
} KLVData;

// Values to indicate data elements are "Unknown" per MISB 0601
const KLVData KLVUnknown = {
  0,                    // #2
  {0},                  // #3
  {0},                  // #4
  0xFFFFFFFF,           // #5
  (s16)0x8000,          // #6
  (s16)0x8000,          // #7
  0xFFFF,               // #8
  0xFFFF,               // #9
  {0},                  // #10
  {0},                  // #11
  {0},                  // #12
  0x80000000,           // #13
  0x80000000,           // #14
  0xFFFFFFFF,           // #15
  0xFFFFFFFF,           // #16
  0xFFFFFFFF,           // #17
  0xFFFFFFFFFFFFFFFFull,// #18    NOTE: 'ull' to indicate unsigned long long
  0x80000000,           // #19 
  0xFFFFFFFFFFFFFFFFull,// #20
  0xFFFFFFFFFFFFFFFFull,// #21
  0xFFFFFFFF,           // #22
  0x80000000,           // #23
  0x80000000,           // #24
  0xFFFFFFFF,           // #25
  (s16)0x8000,          // #26
  (s16)0x8000,          // #27
  (s16)0x8000,          // #28
  (s16)0x8000,          // #29
  (s16)0x8000,          // #30
  (s16)0x8000,          // #31
  (s16)0x8000,          // #32
  (s16)0x8000,          // #33
  0xFF,                 // #34
  0xFFFFFFFF,           // #35
  0xFFFFFFFF,           // #36
  0xFFFFFFFF,           // #37
  0xFFFFFFFF,           // #38
  0x80000000,           // #39
  0x80000000,           // #40
  0x80000000,           // #41
  0xFFFFFFFF,           // #42
  0xFFFF,               // #43
  0xFFFF,               // #44
  0xFFFFFFFF,           // #45
  0xFFFFFFFF,           // #46
  0xFFFF,               // #47
  {
    0,                  // #48:1
    0,                  // #48:2
    {0},                // #48:3
    {0},                // #48:4
    {0},                // #48:5
    {0},                // #48:6
    0,                  // #48:12
    {0},                // #48:13
    0xFFFF              // #48:22
  },
  0xFFFFFFFF,           // #49
  (s16)0x8000,          // #50
  (s16)0x8000,          // #51
  (s16)0x8000,          // #52
  0xFFFFFFFF,           // #53
  0xFFFFFFFF,           // #54
  0xFFFF,               // #55
  0xFFFF,               // #56
  0xFFFFFFFFFFFFFFFFull,// #57   // NOTE: 'ull' to indicate unsigned long long.
  0xFFFF,               // #58
  {0},                  // #59
  0xFFFF,               // #60
  0xFF,                 // #61
  0xFFFF,               // #62
  0xFF,                 // #63
  0xFFFFFFFF,           // #64
  0xFFFF,               // #65
                        // #66
  0x80000000,           // #67
  0x80000000,           // #68
  0xFFFFFFFF,           // #69
  {0},                  // #70
  0xFFFFFFFF,           // #71
  0,                    // #72
  {0},                  // #73
  {0},                  // #74
  0xFFFFFFFF,           // #75
  0xFFFFFFFF,           // #76
  0xFF,                 // #77
  0xFFFFFFFF,           // #78
  (s16)0x8000,          // #79
  (s16)0x8000,          // #80
  {0},                  // #81
  0x80000000,           // #82
  0x80000000,           // #83
  0x80000000,           // #84
  0x80000000,           // #85
  0x80000000,           // #86
  0x80000000,           // #87
  0x80000000,           // #88
  0x80000000,           // #89
  0x80000000,           // #90
  0x80000000,           // #91
  0x80000000,           // #92
  0x80000000,           // #93
  {0},                  // #94
  {0},                  // #95
  {0}                   // #100 // do we need to fill in 96..99?
};

#ifdef __cplusplus
}
#endif

