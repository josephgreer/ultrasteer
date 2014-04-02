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


  //Structure representing a connected square of pixels that are of the color being tracked.
  //Distance from a vector to a square is defined as the minimum distance over the coordinates
  //of all the pixels in the square and the vector.  
  template < class T >
  struct Square
  {
  public:
    Vec2 < T > ul, lr;
    int weight;

    Square() : ul(Vec2 < T >(0, 0)), lr(Vec2 < T >(0, 0)), weight(-1) {}
    Square(const Vec2 < T > & xy, int weight = 0) : ul(xy), lr(xy), weight(weight) {}
    Square(const Vec2 < T > & ul, const Vec2 < T > & lr, int weight = 0)
    {
      this->ul.x = std::min(ul.x, lr.x);
      this->ul.y = std::min(ul.y, lr.y);
      this->lr.x = std::max(ul.x, lr.x);
      this->lr.y = std::max(ul.y, lr.y);
      this->weight = weight;
    }

    int Area() const { return weight != 0 ? weight : (ul.x - lr.x + 1) * (ul.y - lr.y + 1); }

    int DistX(int x) const
    {
      if(x < ul.x)
        return ul.x - x;
      if(ul.x <= x && x <= lr.x)
        return 0;
      else
        return x - lr.x;
    }

    int DistY(int y) const
    {
      if(y < ul.y)
        return ul.y - y;
      if(ul.y <= y && y <= lr.y)
        return 0;
      else
        return y - lr.y;
    }

    static bool less(const Square & l, const Square & r)
    {
      return l.Area() < r.Area();
    }

    int DistX(const Square & s) const
    {
      int d1 = std::min(DistX(s.ul.x), DistX(s.lr.x));
      int d2 = std::min(s.DistX(ul.x), s.DistX(lr.x));
      return std::min(d1, d2);
    }

    int DistY(const Square & s) const
    {
      int d1 = std::min(DistY(s.ul.y), DistY(s.lr.y));
      int d2 = std::min(s.DistY(ul.y), s.DistY(lr.y));
      return std::min(d1, d2);
    }

    template < class T >
    float Dist(const T & a) const
    {
      float dx = (float)DistX(a);
      float dy = (float)DistY(a);

      return std::sqrt(dx * dx + dy * dy);
    }

    Square & operator += (const Vec2 < T > & r)
    {
      ul.x = std::min(ul.x, r.x);
      ul.y = std::min(ul.y, r.y);
      lr.x = std::max(lr.x, r.x);
      lr.y = std::max(lr.y, r.y);
      weight += 1;
      return *this;
    }

    Square & operator += (const Square & r)
    {
      ul.x = std::min(ul.x, r.ul.x);
      ul.y = std::min(ul.y, r.ul.y);
      lr.x = std::max(lr.x, r.lr.x);
      lr.y = std::max(lr.y, r.lr.y);
      weight += 1;
      return *this;
    }

    bool Overlaps(const Square & r)
    {
      return (DistX(r) == 0) && (DistY(r) == 0);
    }

    operator Vec2f() const
    {
      return Vec2f(0.5f * (ul.x + lr.x), 0.5f * (ul.y + lr.y));
    }

    operator Vec2i() const
    {
      return Vec2 < T >((ul.x + lr.x) / 2, (ul.y + lr.y) / 2);
    }

    operator Vec3f() const
    {
      return Vec3f(0.5f * (ul.x + lr.x), 0.5f * (ul.y + lr.y), 0.0f);
    }

    template < class U > 
    bool Inside(const Vec2 < U > v, float error = 0.0f) const
    {
      if(ul.x - error <= v.x && v.x <= lr.x + error && ul.y - error <= v.y && v.y <= lr.y + error)
        return true;
      return false;
    }
  };

  typedef Square < int > Squarei;
  typedef Square < float > Squaref;
}