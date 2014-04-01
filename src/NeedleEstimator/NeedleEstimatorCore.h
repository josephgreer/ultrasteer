#pragma warning(push,0)

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "windows.h"

typedef unsigned char u8;
typedef char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;
typedef double f64;

#define ASSERT(x) if(!(x)) { int a = 0;  int b = 1/a; } 
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define MAX(x,y) ((x)>(y) ? (x) : (y))

static void NTrace(const char *fmt, ...)
{
  char str[1024] = {0};
  va_list Argp;
  va_start (Argp, fmt);
  vsprintf(str, fmt, Argp);
  va_end(Argp);
  OutputDebugString(str);
}

template <class T>
class Vec2
{
public:
  T x,y;
  Vec2(T x, T y)
    : x(x)
    , y(y)
  {}

  Vec2<T>()
    : x(0)
    , y(0)
  {}

  Vec2<T> operator=(const Vec2<T> &rhs)
  {
    this->x = rhs.x;
    this->y = rhs.y;
    return *this;  // Return a reference to myself.
  }

  Vec2<T> operator+(const Vec2<T> &b) const
  {
    return Vec2<T>(this->x+b.x, this->y+b.y);
  }

  Vec2<T> operator-(const Vec2<T> &b) const
  {
    return Vec2<T>(this->x-b.x, this->y-b.y);
  }

  Vec2<T> operator+=(const Vec2<T> &b)
  {
    this->x += b.x;
    this->y += b.y;
    return *this;
  }

  T dot(Vec2<T> b) const
  {
    return this->x*b.x+this->y*b.y;
  }
};

template <class T>
class Vec3
{
public:
  T x,y,z;
  Vec3(T x, T y, T z)
    : x(x)
    , y(y)
    , z(z)
  {}

  Vec3<T>()
    : x(0)
    , y(0)
    , z(0)
  {}

  Vec3<T> operator=(const Vec3<T> &rhs)
  {
    this->x = rhs.x;
    this->y = rhs.y;
    this->z = rhs.z;
    return *this;  // Return a reference to myself.
  }

  Vec3<T> operator+(const Vec3<T> &b) const
  {
    return Vec3<T>(a.x+b.x, a.y+b.y, a.z+b.z);
  }

  Vec3<T> operator+=(const Vec3<T> &b)
  {
    this->x += b.x;
    this->y += b.y;
    this->z += b.z;
    return *this;
  }

  Vec3<T> operator-(const Vec3<T> &b) const
  {
    return Vec3<T>(this->x-b.x, this->y-b.y, this->z-b.z);
  }

  T dot(const Vec3<T> &b) const
  {
    return (T)(this->x*b.x+this->y*b.y+this->z*b.z);
  }

  Vec3<T> cross(const Vec3<T> &b) const
  {
    return Vec3<T>(this->y*b.z-this->z*b.y, this->z*b.z-this->x*b.z, this->x*b.y-this->y*b.x);
  }

  T magnitudeSquraed() const
  {
    return this->dot(*this);
  }

  T distanceSquared(const Vec3<T> &b) const
  {
    Vec3<T> delta = *this-b;
    return delta.magnitudeSquraed();
  }
};

typedef Vec2<f32> Vec2f;
typedef Vec3<f32> Vec3f;