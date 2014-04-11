#pragma once
#pragma warning(push,0)

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "windows.h"
#include <cv.h>

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

#define PI 3.14159265359f

namespace Nf
{

  static void NTrace(const char *fmt, ...)
  {
    char str[1024] = {0};
    va_list Argp;
    va_start (Argp, fmt);
    vsprintf(str, fmt, Argp);
    va_end(Argp);
    OutputDebugString(str);
  }

#if 0
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

    Vec2<T> operator*(const T &b)
    {
      return Vec2<T>(this->x*b, this->y*b);
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

    T magnitudeSquared() const
    {
      return this->dot(*this);
    }

    T magnitude() const
    {
      return sqrt(magnitudeSquared());
    }

    Vec2<T> normalized()
    {
      T mag = this->magnitude();

      return Vec2<T>(this->x/mag, this->y/mag);
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

    Vec3<T> operator*(const T &b)
    {
      return Vec3<T>(this->x*b, this->y*b, this->z*b);
    }

    Vec3<T> cross(const Vec3<T> &b) const
    {
      return Vec3<T>(this->y*b.z-this->z*b.y, this->z*b.z-this->x*b.z, this->x*b.y-this->y*b.x);
    }

    T magnitudeSquared() const
    {
      return this->dot(*this);
    }

    T magnitude() const
    {
      return sqrt(magnitudeSquared);
    }

    Vec2<T> normalized()
    {
      T mag = this->magnitude();

      return Vec2<T>(this->x/mag, this->y/mag);
    }

    T distanceSquared(const Vec3<T> &b) const
    {
      Vec3<T> delta = *this-b;
      return delta.magnitudeSquared();
    }
  };

  typedef Vec2<f32> Vec2f;
  typedef Vec3<f32> Vec3f;
  typedef Vec2<s32> Vec2i;
  typedef Vec3<s32> Vec3i;
  typedef Vec2<f64> Vec2d;
  typedef Vec3<f64> Vec3d;
#endif

  //Structure representing a connected square of pixels that are of the color being tracked.
  //Distance from a vector to a square is defined as the minimum distance over the coordinates
  //of all the pixels in the square and the vector.  
  template < class T >
  struct Square
  {
  public:
    cv::Vec< T, 2 >ul, lr;
    int weight;

    Square() : ul(cv::Vec < T, 2 >(0, 0)), lr(cv::Vec < T, 2 >(0, 0)), weight(-1) {}
    Square(const cv::Vec< T, 2 >& xy, int weight = 0) : ul(xy), lr(xy), weight(weight) {}
    Square(const cv::Vec< T, 2 >& ul, const cv::Vec< T, 2 >& lr, int weight = 0)
    {
      this->ul[0] = min(ul[0], lr[0]);
      this->ul[1] = min(ul[1], lr[1]);
      this->lr[0] = max(ul[0], lr[0]);
      this->lr[1] = max(ul[1], lr[1]);
      this->weight = weight;
    }

    int Area() const { return weight != 0 ? weight : (ul[0] - lr[0] + 1) * (ul[1] - lr[1] + 1); }

    int DistX(int x) const
    {
      if(x < ul[0])
        return ul[0] - x;
      if(ul[0] <= x && x <= lr[0])
        return 0;
      else
        return x - lr[0];
    }

    int DistY(int y) const
    {
      if(y < ul[1])
        return ul[1] - y;
      if(ul[1] <= y && y <= lr[1])
        return 0;
      else
        return y - lr[1];
    }

    static bool less(const Square & l, const Square & r)
    {
      return l.Area() < r.Area();
    }

    int DistX(const Square & s) const
    {
      int d1 = std::min(DistX(s.ul[0]), DistX(s.lr[0]));
      int d2 = std::min(s.DistX(ul[0]), s.DistX(lr[0]));
      return std::min(d1, d2);
    }

    int DistY(const Square & s) const
    {
      int d1 = std::min(DistY(s.ul[1]), DistY(s.lr[1]));
      int d2 = std::min(s.DistY(ul[1]), s.DistY(lr[1]));
      return std::min(d1, d2);
    }

    template < class T >
    float Dist(const T & a) const
    {
      float dx = (float)DistX(a);
      float dy = (float)DistY(a);

      return std::sqrt(dx * dx + dy * dy);
    }

    Square & operator += (const cv::Vec< T, 2 >& r)
    {
      ul[0] = std::min(ul[0], r[0]);
      ul[1] = std::min(ul[1], r[1]);
      lr[0] = std::max(lr[0], r[0]);
      lr[1] = std::max(lr[1], r[1]);
      weight += 1;
      return *this;
    }

    Square & operator += (const Square & r)
    {
      ul[0] = std::min(ul[0], r.ul[0]);
      ul[1] = std::min(ul[1], r.ul[1]);
      lr[0] = std::max(lr[0], r.lr[0]);
      lr[1] = std::max(lr[1], r.lr[1]);
      weight += 1;
      return *this;
    }

    bool Overlaps(const Square & r)
    {
      return (DistX(r) == 0) && (DistY(r) == 0);
    }

    operator cv::Vec2f() const
    {
      return cv::Vec2f(0.5f * (ul[0] + lr[0]), 0.5f * (ul[1] + lr[1]));
    }

    operator cv::Vec2i() const
    {
      return cv::Vec2 < T >((ul[0] + lr[0]) / 2, (ul[1] + lr[1]) / 2);
    }

    operator cv::Vec3f() const
    {
      return Vec3f(0.5f * (ul[0] + lr[0]), 0.5f * (ul[1] + lr[1]), 0.0f);
    }

    template < class U > 
    bool Inside(const cv::Vec < U, 2 > v, float error = 0.0f) const
    {
      if(ul[0] - error <= v[0] && v[0] <= lr[0] + error && ul[1] - error <= v[1] && v[1] <= lr[1] + error)
        return true;
      return false;
    }
  };

  typedef Square < int > Squarei;
  typedef Square < float > Squaref;
}