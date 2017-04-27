/*
 * Copyright (C)2007-2016 SightLine Applications Inc
 * SightLine Applications Library of signal, vision, and speech processing
 * http://www.sightlineapplications.com
 *------------------------------------------------------------------------*/

#pragma once

/*!@defgroup DataTypeGroup SLTypes
 *  Data Types.
 */
//@{

#ifndef s8
  typedef long long      s64; //!< 64 bit signed integer
#ifdef  _TMS320C6600
  typedef __int40_t          s40;
  typedef unsigned __int40_t u40;
  typedef unsigned long long u64;
#elif defined (_TMS320C6X)
  typedef unsigned long long u64; //!< 64 bit unsigned integer
#if defined(__TI_EABI__) // Used by DM814x with ELF format (as opposed to COFF).
  typedef __int40_t           s40; //!< 40 bit (or more) signed integer
  typedef unsigned __int40_t  u40; //!< 40 bit (or more) unsigned integer
#else
  typedef long                s40; //!< 40 bit (or more) signed integer
  typedef unsigned long       u40; //!< 40 bit (or more) unsigned integer
#endif
  typedef volatile unsigned int        vu32;
  typedef volatile unsigned short      vu16;
  typedef volatile unsigned char       vu8;
  typedef volatile int                 vs32;
  typedef volatile short               vs16;
  typedef volatile char                vs8;

#elif defined (linux) || defined (LINUX)

//#include <asm/types.h>
//#include <sys/types.h>//#include <linux/types.h>
#include <stdint.h>
  typedef uint64_t u64; //!< 64 bit unsigned integer

  typedef int64_t  s40; //!< 40 bit (or more) signed integer
  typedef uint64_t u40; //!< 40 bit (or more) unsigned integer

  typedef int64_t	__int64;


  typedef unsigned int        vu32;
  typedef unsigned short      vu16;
  typedef unsigned char       vu8;
  typedef int                 vs32;
  typedef short               vs16;
  typedef char                vs8;

#else   //_WIN32
  typedef unsigned long long  u64; //!< 64 bit unsigned integer
  typedef long long           s40; //!< 40 bit (or more) signed integer
  typedef unsigned long long  u40; //!< 40 bit (or more) unsigned integer
  typedef unsigned int        vu32;
  typedef unsigned short      vu16;
  typedef unsigned char       vu8;
  typedef int                 vs32;
  typedef short               vs16;
  typedef char                vs8;
#endif
  typedef float          f32; //!< 32 bit floating point value
  typedef double         f64; //!< 64 bit floating point value
#endif
//@} End of DataTypeGroup


#if defined (_TMS320C6x) || defined (linux)
#define SLINLINE inline   //!< Indicates to compiler that function should be inlines if possible
#else
#define SLINLINE __inline //!< Indicates to compiler that function should be inlines if possible
#endif


/// Common Return Types
typedef enum {
  SLA_ERROR     = -1,
  SLA_SUCCESS   = 0,
  SLA_FAIL      = 1,
  SLA_TIMEOUT   = 2,
  SLA_LICENSE   = 3,
  SLA_TERMINATE = 4,
  SLA_NOP       = 5,
  SLA_NOT_SENT  = 6,
  SLA_ALLOC     = 7
} SLStatus;

#define ITO0        1   //!< 2 to the 0th
#define ITO1        2   //!< 2 to the 1th
#define ITO2        4   //!< 2 to the 2th
#define ITO3        8   //!< 2 to the 3th
#define ITO4       16   //!< 2 to the 4th
#define ITO5       32   //!< 2 to the 5th
#define ITO6       64   //!< 2 to the 6th
#define ITO7      128   //!< 2 to the 7th
#define ITO8      256   //!< 2 to the 8th
#define ITO9      512   //!< 2 to the 9th
#define ITO10    1024   //!< 2 to the 10th
#define ITO11    2048   //!< 2 to the 11th
#define ITO12    4096   //!< 2 to the 12th
#define ITO13    8192   //!< 2 to the 13th
#define ITO14   16384   //!< 2 to the 14th
#define ITO15   32768   //!< 2 to the 15th
#define ITO16   65536   //!< 2 to the 16th
#define ITO17  131072   //!< 2 to the 17th
#define ITO18  262144   //!< 2 to the 18th
#define ITO19  524288   //!< 2 to the 19th
#define ITO20 1048576   //!< 2 to the 20th
#define ITO21 2097152   //!< 2 to the 21st
#define ITO22 4194304   //!< 2 to the 22nd
#define ITO23 8388608   //!< 2 to the 23rd

// Angle represented as a 16 bit integer:
//   [0..65536) maps to [0..360) degrees
typedef u16 SLAngle;

// Double constant definitions
#define SLPID 3.1415926535897932384626433832795
#define SLPI180D 0.0174532925199432957692369076849  // PI / 180
#define SL180PID 57.295779513082320876798154814114  // 180 / PI

// Float constant definitions
#define SLPI ((f32)SLPID)
#define SLPI180 ((f32)SLPI180D)  // PI / 180
#define SL180PI ((f32)SL180PID)  // 180 / PI

#define SLDEG2RAD(x) (SLPI180*(x))
#define SLRAD2DEG(x) ((x)*SL180PI)

/// Integer rectangle
typedef struct {
  s16 row;    //!< Vertical offset to the upper left corner of the rectangle
  s16 col;    //!< Horizontal offset to the upper left corner of the rectangle
  s16 high;   //!< Height of the rectangle in pixels
  s16 wide;   //!< Width of the rectangle in pixels
} SLIRect;

/// Integer circle
typedef struct {
  s16 row;    //!< Center row of the circle
  s16 col;    //!< Center column of the circle
  s16 rad;    //!< Radius of the circle
} SLICircle;

/// Floating point circle
typedef struct {
  f32 row;    //!< Center row of the circle
  f32 col;    //!< Center column of the circle
  f32 rad;    //!< Radius of the circle
} SLFCircle;

/// Floating point ellipse
typedef struct {
  f32 row;    //!< Center row of the circle
  f32 col;    //!< Center column of the circle
  f32 rmaj;   //!< Radius of the major axis
  f32 rmin;   //!< Radius of the minor axis
  f32 angle;  //!< Angle of the major axis
} SLFEllipse;

/// Integer point
typedef struct {
  s16 row;    //!< Vertical offset to the point
  s16 col;    //!< Horizontal offset to the point
} SLIPoint;

/// Integer point
typedef struct {
  s32 row;    //!< Vertical offset to the point
  s32 col;    //!< Horizontal offset to the point
} SLI32Point;

/// Floating point point
typedef struct {
  f32 row;    //!< Vertical offset to the point
  f32 col;    //!< Horizontal offset to the point
} SLFPoint;

/// Floating point point
typedef struct {
  f64 row;    //!< Vertical offset to the point
  f64 col;    //!< Horizontal offset to the point
} SLF64Point;

/// Integer value point
typedef struct {
  s16 row;    //!< Vertical offset to the point
  s16 col;    //!< Horizontal offset to the point
  s16 val;    //!< Value
} SLIValPt;

/// Integer value point
typedef struct {
  s32 row;    //!< Vertical offset to the point
  s32 col;    //!< Horizontal offset to the point
  u32 val;    //!< Value
} SLI32ValPt;

/// Float value point
typedef struct {
  f32 row;    //!< Vertical offset to the point
  f32 col;    //!< Horizontal offset to the point
  f32 val;    //!< Value
} SLFValPt;

// Fixed point transformation
struct SLI32XForm {
  s32 row8;
  s32 col8;
  s32 angle8;
  s32 scale16;
};

/// Floating point transformation
typedef struct {
  f32 row;    //!< Vertical offset
  f32 col;    //!< Horizontal offset
  f32 angle;  //!< Rotation offset in degress
  f32 scale;  //!< Scale offset (1.0 = unit scale)
} SLFXForm;

/// Floating point transformation
typedef struct {
  f64 row;    //!< Vertical offset
  f64 col;    //!< Horizontal offset
  f64 angle;  //!< Rotation offset in degress
  f64 scale;  //!< Scale offset (1.0 = unit scale)
} SLF64XForm;

// General memory buffer
typedef struct {
  u8 *data;       //!< pointer to data buffer
  u32 actualLen;  //!< number of bytes filled in data buffer
  u32 maxLen;     //!< maximum number of bytes allocated in buffer
} SLMemoryBuffer;

#define SLMAX(x,y) (((x)>(y))?(x):(y))
#define SLMIN(x,y) (((x)<(y))?(x):(y))
#define SLLIMIT(x,min,max) (((x)<(min))?(min):(x)>(max)?(max):(x))
#define SLCLIPU8(x) (u8)((x)>255?255:(x)<0?0:(x))
#define SLCLIPS8(x) (s8)((x)>127?127:(x)<-128?-128:(x))
#define SLCLIPU16(x) (u16)((x)>65535?65535:(x)<0?0:(x))
#define SLCLIPS16(x) (s16)((x)>32767?32767:(x)<-32768?-32768:(x))
#define SLCLIPS32(x) (s32)((x)>(s32)0x7fffffff?(s32)0x7fffffff:(x)<(s32)0x80000000?(s32)0x80000000:(x))
#define SLCLIPU32(x) (s32)((x)>(u32)0xffffffff?(u32)0xffffffff:(x)<0?0:(x))
#define SLROUNDZERO(x) ((x)>0?((x)+0.5f):((x)-0.5f))
#define SLROUNDFIXED(a, shft) (((a)+(1<<(shft-1)))>>shft)

