////////////////////////////////////////////////////////////////////////////////
///@file utx_opt.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

// *** NOTE: The enumerations here are somewhat flexible
//           You can always add another one in a group, it doesn't have to be
//           in sequence of the other ones in a group (e.g. see optBImage)
//           However, GLOBALLY, they do have to be in one contiguous block.
//           e.g. if the previous last enumerated value in _any_ group is "49",
//           you cannot arbitrarily use some other number, it must be "50"
//           However, when you change the variable boundaries using the "Setup"
//           program, you make it increment continously, it is NOT the value you
//           set there.
//           e.g. If you enumerate 0, 1, 2, 3, 4, 50 the variable boundary
//           should be 0/5 (1), not 0/4 (1) * 50/50 (1)

////////////////////////////////////////////////////////////////////////////////
/// System Optimizations Definition.
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// B Image Optimization
/// \li Adjusts: transmit frequency, receive filters, transmit waveforms
////////////////////////////////////////////////////////////////////////////////
#define optTypeBImage               "b image"
////////////////////////////////////////////////////////////////////////////////
/// Color Frame Rate Optimization
/// \li Adjusts: color line density, B line density, ensemble length
////////////////////////////////////////////////////////////////////////////////
#define optTypeColorFR              "color frame rate"
////////////////////////////////////////////////////////////////////////////////
/// M Spectrum Map Optimization
/// \li Adjusts: spectrum brightness, spectrum contrast, spectrum gamma
////////////////////////////////////////////////////////////////////////////////
#define optTypeMMap                 "m map"
////////////////////////////////////////////////////////////////////////////////
/// Color Mode Optimization
/// \li Adjusts: data output (velocity/sigma vs. power), color map, color persistence
////////////////////////////////////////////////////////////////////////////////
#define optTypeColorMode            "color mode"
////////////////////////////////////////////////////////////////////////////////
/// B Frame Rate Optimization
/// \li Adjusts: B line density
////////////////////////////////////////////////////////////////////////////////
#define optTypeBFR                  "b frame rate"
////////////////////////////////////////////////////////////////////////////////
/// Spectral Doppler Map Optimization
/// \li Adjusts: spectrum brightness, spectrum contrast, spectrum gamma
////////////////////////////////////////////////////////////////////////////////
#define optTypeDMap                 "doppler map"
////////////////////////////////////////////////////////////////////////////////
/// Elastography Optimization
/// \li Adjusts: display thresholds (stiffness levels, soft -> hard)
////////////////////////////////////////////////////////////////////////////////
#define optTypeElasto               "elastography"
////////////////////////////////////////////////////////////////////////////////
/// MRU Optimization
/// \li Adjusts: filtering type, filtering levels
////////////////////////////////////////////////////////////////////////////////
#define optTypeMRU                  "mru filtering"
////////////////////////////////////////////////////////////////////////////////
/// B Map Optimization
/// \li Adjusts: brightness, contrast, gamma
////////////////////////////////////////////////////////////////////////////////
#define optTypeBMap                 "b map"
////////////////////////////////////////////////////////////////////////////////
/// Hardware Optimization
/// \li Adjusts: receive aperture parameters, vca settings, etc.
////////////////////////////////////////////////////////////////////////////////
#define optTypeHardware             "hw-platform"

////////////////////////////////////////////////////////////////////////////////
/// B Image Optimization
///@sa optTypeBImage
////////////////////////////////////////////////////////////////////////////////
/// Penetration imaging (lower frequency)
#define optPenetration            "Penetration"
/// General imaging
#define optGeneral                "General"
/// Resolution imaging (higher frequency)
#define optResolution             "Resolution"
/// Harmonics Resolution
#define optHarmonicsRes           "Har-Res"
/// Harmonics Penetration
#define optHarmonicsPen           "Har-Pen"
/// EPI imaging
#define optEPI                    "EPI"

////////////////////////////////////////////////////////////////////////////////
/// B Frame Rate Optimization
///@sa optTypeBFR
////////////////////////////////////////////////////////////////////////////////
/// Medium Frame Rate
#define optBFRMed                  "FrRate Med"
/// High Frame Rate
#define optBFRHigh                 "FrRate High"
/// Maximum Frame Rate
#define optBFRMax                  "FrRate Max"

////////////////////////////////////////////////////////////////////////////////
/// B Map Optimization
///@sa optTypeBMap
////////////////////////////////////////////////////////////////////////////////
/// First B Map
#define optMap1                    "Map 1"
/// Second B Map
#define optMap2                    "Map 2"
/// Third B Map
#define optMap3                    "Map 3"
/// Fourth B Map
#define optMap4                    "Map 4"
/// Fifth B Map
#define optMap5                    "Map 5"
/// Sixth B Map
#define optMap6                    "Map 6"
/// Seventh B Map
#define optMap7                    "Map 7"
/// Eighth B Map
#define optMap8                    "Map 8"
/// Ninth B Map
#define optMap9                    "Map 9"
/// Tenth B Map
#define optMap10                   "Map 10"
/// Eleventh B Map
#define optMap11                   "Map 11"
/// Twelfth B Map
#define optMap12                   "Map 12"
/// Thirteenth B Map
#define optMap13                   "Map 13"
/// Fourteenth B Map
#define optMap14                   "Map 14"
/// Fifteenth B Map
#define optMap15                   "Map 15"
/// Sixteenth B Map
#define optMap16                   "Map 16"

////////////////////////////////////////////////////////////////////////////////
/// M Spectrum Map Optimization
///@sa optTypeMMap
////////////////////////////////////////////////////////////////////////////////
/// First M Map
#define optMMap1                   "MapM 1"
/// Second M Map
#define optMMap2                   "MapM 2"
/// Third M Map
#define optMMap3                   "MapM 3"
/// Fourth M Map
#define optMMap4                   "MapM 4"
/// Fifth M Map
#define optMMap5                   "MapM 5"
/// Sixth M Map
#define optMMap6                   "MapM 6"
/// Seventh M Map
#define optMMap7                   "MapM 7"
/// Eighth M Map
#define optMMap8                   "MapM 8"
/// Ninth B Map
#define optMMap9                   "MapM 9"
/// Tenth M Map
#define optMMap10                  "MapM 10"
/// Eleventh M Map
#define optMMap11                  "MapM 11"
/// Twelfth M Map
#define optMMap12                  "MapM 12"
/// Thirteenth M Map
#define optMMap13                  "MapM 13"
/// Fourteenth M Map
#define optMMap14                  "MapM 14"
/// Fifteenth M Map
#define optMMap15                  "MapM 15"
/// Sixteenth M Map
#define optMMap16                  "MapM 16"

////////////////////////////////////////////////////////////////////////////////
/// Spectral Doppler Map Optimization
///@sa optTypeDMap
////////////////////////////////////////////////////////////////////////////////
/// First Spectral Doppler Map
#define optDMap1                   "MapD 1"
/// Second Spectral Doppler Map
#define optDMap2                   "MapD 2"
/// Third Spectral Doppler Map
#define optDMap3                   "MapD 3"

////////////////////////////////////////////////////////////////////////////////
/// MRU Optimization
///@sa optTypeMRU
////////////////////////////////////////////////////////////////////////////////
//optMRU "mru filtering"
/// MRU filtering off
#define optMruOff                  "Clarity Off"
/// Low MRU filtering
#define optMruLow                  "Clarity Low"
/// Medium MRU filtering
#define optMruMed                  "Clarity Med"
/// High MRU filtering
#define optMruHigh                 "Clarity High"
/// Max MRU filtering
#define optMruMax                  "Clarity Max"

////////////////////////////////////////////////////////////////////////////////
/// Elastography Optimization
///@sa optTypeElasto
////////////////////////////////////////////////////////////////////////////////
/// Shows all stiffness levels
#define optElastoAll               "Rgn All"
/// Shows harder areas
#define optElastoHard              "Rgn Hard"
/// Shows areas of medium stiffness
#define optElastoMed               "Rgn Med"
/// Shows softer areas
#define optElastoSoft              "Rgn Soft"

////////////////////////////////////////////////////////////////////////////////
/// Color Mode Optimization
///@sa optTypeColorMode
////////////////////////////////////////////////////////////////////////////////
/// Velocity imaging
#define optColor                   "Method Color"
/// Power Doppler imaging
#define optPower                   "Method Power"
/// Tissue Doppler imaging
#define optTDI                     "Method TDI"
/// B-Flow imaging
#define optFlow                    "Method Flow"

////////////////////////////////////////////////////////////////////////////////
/// Color Frame Rate Optimization
///@sa optTypeColorFR
////////////////////////////////////////////////////////////////////////////////
/// Medium color frame rate
#define optColorFRMed              "FrRate Med"
/// High color frame rate
#define optColorFRHigh             "FrRate High"
/// Maximum color frame rate
#define optColorFRMax              "FrRate Max"

////////////////////////////////////////////////////////////////////////////////
/// Hardware Optimization
///@sa optTypeHardware
////////////////////////////////////////////////////////////////////////////////
/// Version 2 OP/SP
#define optHWV1                     "V2"
/// Version 3 Touch/MDP
#define optHWV2                     "V3"
/// Version 4 Touch/MDP
#define optHWV3                     "V4"
