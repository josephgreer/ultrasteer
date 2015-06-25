#pragma once
#pragma warning(push,0)

#include <stdio.h>
#include <stdarg.h>
#include "windows.h"

typedef unsigned char u8;
typedef char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;
typedef double f64;

using ::s8;
using ::u16;
using ::s16;
using ::u32;
using ::s32;
using ::f32;
using ::f64;

#define ASSERT(x) if(!(x)) { int a = 0;  int b = 1/a; } 
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define ABS(x) ((x)>0?(x):(-(x)))

#define PI 3.14159265359f

#ifdef _DEBUG
#define DEBUG_ASSERT(x) if(!(x)) { int a = 0;  int b = 1/a; } 
#else
#define DEBUG_ASSERT(x)
#endif

namespace Nf
{
  void NTrace(const char *fmt, ...);

#define BEGIN_TIMING(x,y) \
  u32 g_beginTick##x = GetTickCount(); static u32 g_n##x = 0; static u32 g_tot##x = 0;
#define END_TIMING(x,y) \
  u32 g_endTick##x = GetTickCount(); g_tot##x += g_endTick##x-g_beginTick##x; \
  if(g_n##x++ >= y) { \
    NTrace("Timing: %s time %f ms\n", #x, (f32)g_tot##x/g_n##x); \
    g_tot##x = 0; \
    g_n##x = 0; \
  }
}