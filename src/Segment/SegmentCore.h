#pragma once
#pragma warning(push,0)

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <vtkSmartPointer.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
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
#define ABS(x) ((x)>0?(x):(-(x)))

#define PI 3.14159265359f

#ifdef _DEBUG
#define DEBUG_ASSERT(x) if(!(x)) { int a = 0;  int b = 1/a; } 
#else
#define DEBUG_ASSERT(x)
#endif

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

#ifdef VNL_INCLUDE
    operator vnl_vector < T > ()
    {
      vnl_vector < T > res(2);
      res[0] = this->x;
      res[1] = this->y;
      return res;
    }
#endif
  };

  template <class T>
  class Vec4
  {
  public:
    T x,y,z,w;
    Vec4(T x, T y, T z, T w)
      : x(x)
      , y(y)
      , z(z)
      , w(w)
    {}

    Vec4<T>()
      : x(0)
      , y(0)
      , z(0)
      , w(0)
    {}

    Vec4<T> operator=(const Vec4<T> &rhs)
    {
      this->x = rhs.x;
      this->y = rhs.y;
      this->z = rhs.z;
      this->w = rhs.w;
      return *this;  // Return a reference to myself.
    }

    Vec4<T> operator+(const Vec4<T> &b) const
    {
      return Vec4<T>(this->x+b.x, this->y+b.y, this->z+b.z, this->w+b.w);
    }

    Vec4<T> operator-(const Vec4<T> &b) const
    {
      return Vec4<T>(this->x-b.x, this->y-b.y,this->z-b.z,this->w-b.w);
    }

    Vec4<T> operator*(const T &b)
    {
      return Vec4<T>(this->x*b, this->y*b, this->z*b, this->w*b);
    }

    Vec4<T> operator+=(const Vec4<T> &b)
    {
      this->x += b.x;
      this->y += b.y;
      this->z += b.z;
      this->w += b.w;
      return *this;
    }

    T dot(Vec4<T> b) const
    {
      return this->x*b.x+this->y*b.y+this->z*b.z+this->w*b.w;
    }

    T magnitudeSquared() const
    {
      return this->dot(*this);
    }

    T magnitude() const
    {
      return sqrt(magnitudeSquared());
    }

    Vec4<T> normalized()
    {
      T mag = this->magnitude();

      return Vec4<T>(this->x/mag, this->y/mag, this->z/mag, this->w/mag);
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

    Vec3<T> operator=(const Vec4<T> &rhs)
    {
      this->x = rhs.x;
      this->y = rhs.y;
      this->z = rhs.z;
      return *this;
    }

    Vec3<T> operator+(const Vec3<T> &b) const
    {
      return Vec3<T>(this->x+b.x, this->y+b.y, this->z+b.z);
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
      T va[3] = {this->x, this->y, this->z};
      T vb[3] = {b.x, b.y, b.z};
      T vc[3] = {0};
      vtkMath::Cross(va, vb, vc);
      return Vec3<T>(vc[0], vc[1], vc[2]);
    }

    T magnitudeSquared() const
    {
      return this->dot(*this);
    }

    T magnitude() const
    {
      return sqrt(magnitudeSquared());
    }

    Vec3<T> normalized()
    {
      T mag = this->magnitude();

      return Vec3<T>(this->x/mag, this->y/mag, this->z/mag);
    }

    T distanceSquared(const Vec3<T> &b) const
    {
      Vec3<T> delta = *this-b;
      return delta.magnitudeSquared();
    }

#ifdef VNL_INCLUDE
    operator vnl_vector < T > ()
    {
      vnl_vector < T > res(3);
      res[0] = this->x;
      res[1] = this->y;
      res[2] = this->z;
      return res;
    }
#endif
  };

  typedef Vec2<f32> Vec2f;
  typedef Vec3<f32> Vec3f;
  typedef Vec4<f32> Vec4f;
  typedef Vec2<s32> Vec2i;
  typedef Vec3<s32> Vec3i;
  typedef Vec4<s32> Vec4i;
  typedef Vec2<f64> Vec2d;
  typedef Vec3<f64> Vec3d;
  typedef Vec4<f64> Vec4d;


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
      this->ul.x = min(ul.x, lr.x);
      this->ul.y = min(ul.y, lr.y);
      this->lr.x = max(ul.x, lr.x);
      this->lr.y = max(ul.y, lr.y);
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

  //should have used vtkMatrix3x3 but apparently they don't advertise it very well, so I made
  //this before I found out about it
  template < class T >
  class Matrix33
  {
  public:
    T m_data[3][3];

    Matrix33()
    {
      memset(m_data, 0, sizeof(T)*9);
    }

    Matrix33(const T mat[3][3])
    {
      memcpy(m_data, mat, sizeof(T)*9);
    }

    Matrix33(f64 a11, f64 a12, f64 a13, f64 a21, f64 a22, f64 a23, f64 a31, f64 a32, f64 a33)
    {
      m_data[0][0] = a11; m_data[0][1] = a12; m_data[0][2] = a13; 
      m_data[1][0] = a21; m_data[1][1] = a22; m_data[1][2] = a23; 
      m_data[2][0] = a31; m_data[2][1] = a32; m_data[2][2] = a33; 
    }
    static Matrix33<T> I()
    {
      T data[3][3];
      vtkMath::Identity3x3(data);
      return Matrix33<T>(data);
    }

    static Matrix33<T> Diaganol(T a, T b, T c)
    {
      T data[3][3];
      vtkMath::Identity3x3(data);
      data[0][0] = a;
      data[1][1] = b;
      data[2][2] = c;
      return Matrix33<T>(data);
    }

    static Matrix33<T> Diaganol(Vec3 < T > values)
    {
      T data[3][3];
      vtkMath::Identity3x3(data);
      data[0][0] = values.x;
      data[1][1] = values.y;
      data[2][2] = values.z;
      return Matrix33<T>(data);
    }

    static Matrix33<T> FromCols(Vec3d a, Vec3d b, Vec3d c)
    {
      T data[3][3];
      data[0][0] = a.x;   data[0][1] = b.x;  data[0][2] = c.x;
      data[1][0] = a.y;   data[1][1] = b.y;  data[1][2] = c.y;
      data[2][0] = a.z;   data[2][1] = b.z;  data[2][2] = c.z;
      return Matrix33<T>(data);
    }

    static Matrix33<T> FromRows(Vec3d a, Vec3d b, Vec3d c)
    {
      T data[3][3];
      data[0][0] = a.x;  data[0][1] = a.y;  data[0][2] = a.z;
      data[1][0] = b.x;  data[1][1] = b.y;  data[1][2] = b.z;
      data[2][0] = c.x;  data[2][1] = c.y;  data[2][2] = c.z;
    }

    Matrix33<T> operator=(const Matrix33<T> &rhs)
    {
      memcpy(&this->m_data[0][0], &rhs.m_data[0][0], sizeof(T)*9);
      return *this;  // Return a reference to myself.
    }

    Matrix33<T> operator=(const cv::Mat &rhs)
    {
      if(rhs.cols != 3 && rhs.rows != 3) {
        assert(0);
      }

      T data[3][3];

      for(s32 r=0; r<3; r++) {
        for(s32 c=0; c<3; c++) {
          data[r][c] = rhs.at<T>(r,c);
        }
      }

      return Matrix33<T>(data);
    }

    Matrix33<T> operator+(const Matrix33<T> &b) const
    {
      T res[3][3];
      res[0][0] = this->m_data[0][0] + b.m_data[0][0];
      res[0][1] = this->m_data[0][1] + b.m_data[0][1];
      res[0][2] = this->m_data[0][2] + b.m_data[0][2];
      res[1][0] = this->m_data[1][0] + b.m_data[1][0];
      res[1][1] = this->m_data[1][1] + b.m_data[1][1];
      res[1][2] = this->m_data[1][2] + b.m_data[1][2];
      res[2][0] = this->m_data[2][0] + b.m_data[2][0];
      res[2][1] = this->m_data[2][1] + b.m_data[2][1];
      res[2][2] = this->m_data[2][2] + b.m_data[2][2];

      return Matrix33<T>(res);
    }

    Matrix33<T> operator+=(const Matrix33<T> &b)
    {
      this->m_data[0][0] += b.m_data[0][0];
      this->m_data[0][1] += b.m_data[0][1];
      this->m_data[0][2] += b.m_data[0][2];
      this->m_data[1][0] += b.m_data[1][0];
      this->m_data[1][1] += b.m_data[1][1];
      this->m_data[1][2] += b.m_data[1][2];
      this->m_data[2][0] += b.m_data[2][0];
      this->m_data[2][1] += b.m_data[2][1];
      this->m_data[2][2] += b.m_data[2][2];

      return *this;
    }

    Matrix33<T> operator-(const Matrix33<T> &b) const
    {
      T res[3][3];
      res[0][0] = this->m_data[0][0] - b.m_data[0][0];
      res[0][1] = this->m_data[0][1] - b.m_data[0][1];
      res[0][2] = this->m_data[0][2] - b.m_data[0][2];
      res[1][0] = this->m_data[1][0] - b.m_data[1][0];
      res[1][1] = this->m_data[1][1] - b.m_data[1][1];
      res[1][2] = this->m_data[1][2] - b.m_data[1][2];
      res[2][0] = this->m_data[2][0] - b.m_data[2][0];
      res[2][1] = this->m_data[2][1] - b.m_data[2][1];
      res[2][2] = this->m_data[2][2] - b.m_data[2][2];

      return Matrix33<T>(res);
    }

    Matrix33<T> operator*(const T &b) const
    {
      T res[3][3];
      res[0][0] = this->m_data[0][0]*b;
      res[0][1] = this->m_data[0][1]*b;
      res[0][2] = this->m_data[0][2]*b;
      res[1][0] = this->m_data[1][0]*b;
      res[1][1] = this->m_data[1][1]*b;
      res[1][2] = this->m_data[1][2]*b;
      res[2][0] = this->m_data[2][0]*b;
      res[2][1] = this->m_data[2][1]*b;
      res[2][2] = this->m_data[2][2]*b;
      return Matrix33<T>(res);
    }

    void Print() const
    {
      NTrace("[%f %f %f; %f %f %f; %f %f %f]\n", m_data[0][0], m_data[0][1], m_data[0][2],
        m_data[1][0], m_data[1][1], m_data[1][2],
        m_data[2][0], m_data[2][1], m_data[2][2]);
    }

    Vec3<T> Col(s32 col) const
    {
      return Vec3<T>(m_data[0][col], m_data[1][col], m_data[2][col]);
    }

    Vec3<T> Row(s32 row) const
    {
      return Vec3<T>(m_data[row][0], m_data[row][1], m_data[row][2]);
    }

    Matrix33<T> operator*(const Matrix33<T> &b) const
    {
      T res[3][3];
      vtkMath::Multiply3x3(this->m_data, b.m_data, res);
      return Matrix33<T>(res);
    }

    Vec3<T> operator*(const Vec3<T> &b) const
    {
      return this->Col(0)*b.x + this->Col(1)*b.y + this->Col(2)*b.z;
    }

    Matrix33<T> Transpose() const
    {
      T res[3][3];
      vtkMath::Transpose3x3(this->m_data,res);
      return res;
    }

    Matrix33<T> Inverse() const
    {
      T res[3][3];
      vtkMath::Invert3x3(this->m_data,res);
      return res;
    }

    T Determinant() const
    {
      return vtkMath::Determinant3x3(&m_data[0][0], &m_data[1][0], &m_data[2][0]);
    }
  };

  typedef Matrix33 < f32 > Matrix33f;
  typedef Matrix33 < f64 > Matrix33d;

  template < class T >
  class Cube
  {
  public:
    Vec3<T> m_ul;
    Matrix33<T> m_axes;
    Cube()
      : m_ul(0,0,0)
    {
      m_axes = Matrix33<T>::I();
    }

    Cube(Vec3<T> ul, Matrix33<T> axes)
      : m_ul(ul)
      , m_axes(axes)
    {
    }
  };

  typedef Cube < s32 > Cubei;
  typedef Cube < f32 > Cubef;
  typedef Cube < f64 > Cubed;

  class Matrix44d 
  {
  public:
    f64 m_data[4][4];

    Matrix44d()
    {
      memset(m_data, 0, sizeof(f64)*16);
    }

    Matrix44d(const f64 mat[4][4])
    {
      memcpy(m_data, mat, sizeof(f64)*16);
    }

    Matrix44d(f64 a11, f64 a12, f64 a13, f64 a14, f64 a21, f64 a22, f64 a23, f64 a24, f64 a31, f64 a32, f64 a33, f64 a34, f64 a41, f64 a42, f64 a43, f64 a44)
    {
      m_data[0][0] = a11; m_data[0][1] = a12; m_data[0][2] = a13; m_data[0][3] = a14;
      m_data[1][0] = a21; m_data[1][1] = a22; m_data[1][2] = a23; m_data[1][3] = a24;
      m_data[2][0] = a31; m_data[2][1] = a32; m_data[2][2] = a33; m_data[2][3] = a34;
      m_data[3][0] = a41; m_data[3][1] = a42; m_data[3][2] = a43; m_data[3][3] = a44;
    }

    static Matrix44d I()
    {
      f64 data[4][4] = {0};
      vtkMatrix4x4::Identity(&data[0][0]);
      return Matrix44d(data);
    }

    static Matrix44d Diaganol(f64 a, f64 b, f64 c, f64 d)
    {
      f64 data[4][4] = {0};
      vtkMatrix4x4::Identity(&data[0][0]);
      data[0][0] = a;
      data[1][1] = b;
      data[2][2] = c;
      data[3][3] = d;
      return Matrix44d(data);
    }

    static Matrix44d Diaganol(Vec4d values)
    {
      f64 data[4][4];
      vtkMatrix4x4::Identity(&data[0][0]);
      data[0][0] = values.x;
      data[1][1] = values.y;
      data[2][2] = values.z;
      data[3][3] = values.w;
      return Matrix44d(data);
    }

    static Matrix44d FromOrientationAndTranslation(const Matrix33d &orientation, const Vec3d translation)
    {
      Matrix44d res = Matrix44d::I();
      memcpy(&res.m_data[0][0], &orientation.m_data[0][0], sizeof(f64)*3);
      memcpy(&res.m_data[1][0], &orientation.m_data[1][0], sizeof(f64)*3);
      memcpy(&res.m_data[2][0], &orientation.m_data[2][0], sizeof(f64)*3);

      res.m_data[0][3] = translation.x;
      res.m_data[1][3] = translation.y;
      res.m_data[2][3] = translation.z;

      return res;
    }

    Matrix33d GetOrientation()
    {
      Matrix33d res;
      memcpy(&res.m_data[0][0], &this->m_data[0][0], sizeof(f64)*3);
      memcpy(&res.m_data[1][0], &this->m_data[1][0], sizeof(f64)*3);
      memcpy(&res.m_data[2][0], &this->m_data[2][0], sizeof(f64)*3);
      return res;
    }

    Matrix44d operator=(const Matrix44d &rhs)
    {
      memcpy(&this->m_data[0][0], &rhs.m_data[0][0], sizeof(f64)*16);
      return *this;  // Return a reference to myself.
    }

    static Matrix44d FromCvMat(const cv::Mat &rhs)
    {
      if(rhs.cols != 4 && rhs.rows != 4) {
        assert(0);
      }

      f64 data[4][4];

      for(s32 r=0; r<4; r++) {
        for(s32 c=0; c<4; c++) {
          data[r][c] = rhs.at<f64>(r,c);
        }
      }

      return Matrix44d(data);
    }

    static Matrix44d FromVtkMatrix4x4(const vtkMatrix4x4 *rhs)
    {
      f64 data[4][4];

      for(s32 r=0; r<4; r++) {
        for(s32 c=0; c<4; c++) {
          data[r][c] = rhs->Element[r][c];
        }
      }

      return Matrix44d(data);
    }

    Matrix44d operator+(const Matrix44d &b) const
    {
      f64 res[4][4];
      res[0][0] = this->m_data[0][0] + b.m_data[0][0];
      res[0][1] = this->m_data[0][1] + b.m_data[0][1];
      res[0][2] = this->m_data[0][2] + b.m_data[0][2];
      res[0][3] = this->m_data[0][3] + b.m_data[0][3];

      res[1][0] = this->m_data[1][0] + b.m_data[1][0];
      res[1][1] = this->m_data[1][1] + b.m_data[1][1];
      res[1][2] = this->m_data[1][2] + b.m_data[1][2];
      res[1][3] = this->m_data[1][3] + b.m_data[1][3];

      res[2][0] = this->m_data[2][0] + b.m_data[2][0];
      res[2][1] = this->m_data[2][1] + b.m_data[2][1];
      res[2][2] = this->m_data[2][2] + b.m_data[2][2];
      res[2][3] = this->m_data[2][3] + b.m_data[2][3];

      res[3][0] = this->m_data[3][0] + b.m_data[3][0];
      res[3][1] = this->m_data[3][1] + b.m_data[3][1];
      res[3][2] = this->m_data[3][2] + b.m_data[3][2];
      res[3][3] = this->m_data[3][3] + b.m_data[3][3];

      return Matrix44d(res);
    }

    Matrix44d operator+=(const Matrix44d &b)
    {
      this->m_data[0][0] += b.m_data[0][0];
      this->m_data[0][1] += b.m_data[0][1];
      this->m_data[0][2] += b.m_data[0][2];
      this->m_data[0][3] += b.m_data[0][3];

      this->m_data[1][0] += b.m_data[1][0];
      this->m_data[1][1] += b.m_data[1][1];
      this->m_data[1][2] += b.m_data[1][2];
      this->m_data[1][3] += b.m_data[1][3];

      this->m_data[2][0] += b.m_data[2][0];
      this->m_data[2][1] += b.m_data[2][1];
      this->m_data[2][2] += b.m_data[2][2];
      this->m_data[2][3] += b.m_data[2][3];

      this->m_data[3][0] += b.m_data[3][0];
      this->m_data[3][1] += b.m_data[3][1];
      this->m_data[3][2] += b.m_data[3][2];
      this->m_data[3][3] += b.m_data[3][3];

      return *this;
    }

    Matrix44d operator-(const Matrix44d &b) const
    {
      f64 res[4][4];
      res[0][0] = this->m_data[0][0] - b.m_data[0][0];
      res[0][1] = this->m_data[0][1] - b.m_data[0][1];
      res[0][2] = this->m_data[0][2] - b.m_data[0][2];
      res[0][3] = this->m_data[0][3] - b.m_data[0][3];

      res[1][0] = this->m_data[1][0] - b.m_data[1][0];
      res[1][1] = this->m_data[1][1] - b.m_data[1][1];
      res[1][2] = this->m_data[1][2] - b.m_data[1][2];
      res[1][3] = this->m_data[1][3] - b.m_data[1][3];

      res[2][0] = this->m_data[2][0] - b.m_data[2][0];
      res[2][1] = this->m_data[2][1] - b.m_data[2][1];
      res[2][2] = this->m_data[2][2] - b.m_data[2][2];
      res[2][3] = this->m_data[2][3] - b.m_data[2][3];

      res[3][0] = this->m_data[3][0] - b.m_data[3][0];
      res[3][1] = this->m_data[3][1] - b.m_data[3][1];
      res[3][2] = this->m_data[3][2] - b.m_data[3][2];
      res[3][3] = this->m_data[3][3] - b.m_data[3][3];

      return Matrix44d(res);
    }

    Matrix44d operator*(const f64 &b) const
    {
      f64 res[4][4];
      res[0][0] = this->m_data[0][0] * b;
      res[0][1] = this->m_data[0][1] * b;
      res[0][2] = this->m_data[0][2] * b;
      res[0][3] = this->m_data[0][3] * b;

      res[1][0] = this->m_data[1][0] * b;
      res[1][1] = this->m_data[1][1] * b;
      res[1][2] = this->m_data[1][2] * b;
      res[1][3] = this->m_data[1][3] * b;

      res[2][0] = this->m_data[2][0] * b;
      res[2][1] = this->m_data[2][1] * b;
      res[2][2] = this->m_data[2][2] * b;
      res[2][3] = this->m_data[2][3] * b;

      res[3][0] = this->m_data[3][0] * b;
      res[3][1] = this->m_data[3][1] * b;
      res[3][2] = this->m_data[3][2] * b;
      res[3][3] = this->m_data[3][3] * b;

      return Matrix44d(res);
    }

    Matrix44d operator*(const Matrix44d &b) const
    {
      f64 res[4][4] = {0};
      vtkMatrix4x4::Multiply4x4(&this->m_data[0][0], &b.m_data[0][0], &res[0][0]);
      return Matrix44d(res);
    }

    void Print() const
    {
      NTrace("[%f %f %f %f; %f %f %f %f; %f %f %f %f; %f %f %f %f]\n", m_data[0][0], m_data[0][1], m_data[0][2], m_data[0][3],
        m_data[1][0], m_data[1][1], m_data[1][2], m_data[1][3],
        m_data[2][0], m_data[2][1], m_data[2][2], m_data[2][3],
        m_data[3][0], m_data[3][1], m_data[3][2], m_data[3][3]);
    }

    Vec4d Col(s32 col) const
    {
      return Vec4d(m_data[0][col], m_data[1][col], m_data[2][col], m_data[3][col]);
    }

    Vec4d Row(s32 row) const
    {
      return Vec4d(m_data[row][0], m_data[row][1], m_data[row][2], m_data[row][3]);
    }

    Vec4d operator*(const Vec4d &b) const
    {
      return this->Col(0)*b.x + this->Col(1)*b.y + this->Col(2)*b.z + this->Col(3)*b.w;
    }

    Matrix44d Transpose() const
    {
      f64 res[4][4] = {0};
      vtkMatrix4x4::Transpose(&this->m_data[0][0],&res[0][0]);
      return Matrix44d(res);
    }

    Matrix44d Inverse() const
    {
      f64 res[4][4] = {0};
      vtkMatrix4x4::Invert(&this->m_data[0][0], &res[0][0]);
      return Matrix44d(res);
    }

    f64 Determinant() const
    {
      return vtkMatrix4x4::Determinant(&this->m_data[0][0]);
    }

    vtkSmartPointer<vtkMatrix4x4> GetVTKMatrix() const
    {
      vtkSmartPointer<vtkMatrix4x4> res = vtkSmartPointer<vtkMatrix4x4>::New();
      for(s32 i=0; i<4; i++) {
        for(s32 j=0; j<4; j++) {
          res->SetElement(i,j,this->m_data[i][j]);
        }
      }
      return res;
    }
  };

  template < class T >
  static void printMatrix33(const Matrix33<T> & matrix)
  {
    matrix.Print();
  }

  static void printMatrix44(const Matrix44d & matrix)
  {
    matrix.Print();
  }
}