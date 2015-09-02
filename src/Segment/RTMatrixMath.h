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

#include <vnl/vnl_rotation_matrix.h>
#include <vnl/algo/vnl_cholesky.h>
#include <vnl/algo/vnl_matrix_inverse.h>

#include <armadillo>

#include "RTUtil.h"

namespace Nf
{
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

    template < class U >
    operator Vec2<U>() const 
    {
      return Vec2<U>((U)this->x, (U)this->y);
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

    arma::vec2 ToArmaVec() const
    {
      arma::vec2 res; res << this->x << arma::endr << this->y;
      return res;
    }

    static Vec2<T> FromArmaVec(arma::vec rhs)
    {
      return Vec2(rhs(0), rhs(1));
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

    arma::vec4 ToArmaVec() const
    {
      arma::vec4 res;
      res << this->x << arma::endr << this->y << arma::endr << this->z << arma::endr << this->w;
      return res;
    }

    static Vec4<T> FromArmaVec(arma::vec rhs)
    {
      return Vec4(rhs(0), rhs(1), rhs(2), rhs(3));
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

    Vec3<T> operator/(const T &b)
    {
      return Vec3<T>(this->x/b, this->y/b, this->z/b);
    }

    template < class U >
    operator Vec3<U>() const 
    {
      return Vec3<U>((U)this->x, (U)this->y, (U)this->z);
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

    arma::vec3 ToArmaVec() const
    {
      arma::vec3 res;
      res << this->x << arma::endr << this->y << arma::endr << this->z;
      return res;
    }

    static Vec3<T> FromArmaVec(arma::vec rhs)
    {
      return Vec3(rhs(0), rhs(1), rhs(2));
    }

    bool isZero() const
    {
      return this->magnitude() < 1e-3;
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

  template <class T>
  class Vec6
  {
  public:
    T x,y,z,a,b,c;
    Vec6(T x, T y, T z, T a, T b, T c)
      : x(x)
      , y(y)
      , z(z)
      , a(a)
      , b(b)
      , c(c)
    {}

    Vec6<T>()
      : x(0)
      , y(0)
      , z(0)
      , a(0)
      , b(0)
      , c(0)
    {}

    Vec6<T> operator=(const Vec6<T> &rhs)
    {
      this->x = rhs.x;
      this->y = rhs.y;
      this->z = rhs.z;
      this->a = rhs.a;
      this->b = rhs.b;
      this->c = rhs.c;
      return *this;  // Return a reference to myself.
    }

    Vec6<T> operator+(const Vec6<T> &b) const
    {
      return Vec6<T>(this->x+b.x, this->y+b.y, this->z+b.z, this->a+b.a, this->b+b.b, this->c+b.c);
    }

    Vec6<T> operator-(const Vec6<T> &b) const
    {
      return Vec4<T>(this->x-b.x, this->y-b.y,this->z-b.z, this->a-b.a, this->b-b.b, this->c-b.c);
    }

    Vec6<T> operator*(const T &b)
    {
      return Vec6<T>(this->x*b, this->y*b, this->z*b, this->a*b, this->b*b, this->c*b);
    }

    Vec6<T> operator+=(const Vec6<T> &b)
    {
      this->x += b.x;
      this->y += b.y;
      this->z += b.z;
      this->a += b.a;
      this->b += b.b;
      this->c += b.c;
      return *this;
    }

    T dot(Vec6<T> b) const
    {
      return this->x*b.x+this->y*b.y+this->z*b.z+this->a*b.a+this->b*b.b+this->c*b.c;
    }

    static Vec6<T> From2x3s(Vec3<T> a, Vec3<T> b)
    {
      Vec6<T> res(a.x, a.y, a.z, b.x, b.y, b.z);
      return res;
    }

    T magnitudeSquared() const
    {
      return this->dot(*this);
    }

    T magnitude() const
    {
      return sqrt(magnitudeSquared());
    }

    Vec6<T> normalized()
    {
      T mag = this->magnitude();

      return Vec6<T>(this->x/mag, this->y/mag, this->z/mag, this->a/mag, this->b/mag, this->c/mag);
    }

    T at(int i)
    {
      switch(i)
      {
      case 0:
        return this->x;
      case 1:
        return this->y;
      case 2:
        return this->z;
      case 3:
        return this->a;
      case 4:
        return this->b;
      case 5:
        return this->c;
      default:
        return 0;
      }
    }
  };
  typedef Vec2<f32> Vec2f;
  typedef Vec3<f32> Vec3f;
  typedef Vec4<f32> Vec4f;
  typedef Vec6<f32> Vec6f;
  typedef Vec2<s32> Vec2i;
  typedef Vec3<s32> Vec3i;
  typedef Vec4<s32> Vec4i;
  typedef Vec6<s32> Vec6i;
  typedef Vec2<f64> Vec2d;
  typedef Vec3<f64> Vec3d;
  typedef Vec4<f64> Vec4d;
  typedef Vec6<f64> Vec6d;


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

    Square & operator = (const Square & r)
    {
      this->ul = r.ul;
      this->lr = r.lr;
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

    static Matrix33<T> Diagonal(T a, T b, T c)
    {
      T data[3][3];
      vtkMath::Identity3x3(data);
      data[0][0] = a;
      data[1][1] = b;
      data[2][2] = c;
      return Matrix33<T>(data);
    }

    static Matrix33<T> Diagonal(Vec3 < T > values)
    {
      T data[3][3];
      vtkMath::Identity3x3(data);
      data[0][0] = values.x;
      data[1][1] = values.y;
      data[2][2] = values.z;
      return Matrix33<T>(data);
    }

    
    static Matrix33<T> FromArmaMatrix3x3(const arma::mat rhs)
    {
      f64 data[3][3];

      for(s32 r=0; r<3; r++) {
        for(s32 c=0; c<3; c++) {
          data[r][c] = rhs(r,c);
        }
      }

      return Matrix33d(data);
    }

    T Trace() const
    {
      T res = this->m_data[0][0] + this->m_data[1][1] + this->m_data[2][2]; 
      return res;
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
      return Matrix33<T>(data);
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

    arma::mat33 ToArmaMat() const
    {
      arma::mat33 res;
      res << m_data[0][0] << m_data[0][1] << m_data[0][2] << arma::endr << 
             m_data[1][0] << m_data[1][1] << m_data[1][2] << arma::endr <<
             m_data[2][0] << m_data[2][1] << m_data[2][2] << arma::endr;
      return res;
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

    static Matrix44d Zero()
    {
      f64 data[4][4] = {0};
      return Matrix44d(data);
    }

    static Matrix44d Diagonal(f64 a, f64 b, f64 c, f64 d)
    {
      f64 data[4][4] = {0};
      vtkMatrix4x4::Identity(&data[0][0]);
      data[0][0] = a;
      data[1][1] = b;
      data[2][2] = c;
      data[3][3] = d;
      return Matrix44d(data);
    }

    static Matrix44d Diagonal(Vec4d values)
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

    bool isIdentity()
    {
      return arma::norm(arma::eye(4,4)-this->ToArmaMatrix4x4()) < 1e-3;
    }

    bool isZero()
    {
      return arma::norm(arma::zeros(4,4)-this->ToArmaMatrix4x4()) < 1e-3;
    }

    Vec3d GetPosition()
    {
      Vec3d res(this->m_data[0][3],this->m_data[1][3],this->m_data[2][3]);
      return res;
    }

    void SetPosition(const Vec3d &p)
    {
      this->m_data[0][3] = p.x;
      this->m_data[1][3] = p.y;
      this->m_data[2][3] = p.z;
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

    cv::Mat ToCvMat() const
    {
      cv::Mat res(4,4,CV_64F);
      for(s32 r=0; r<4; r++)  {
        for(s32 c=0; c<4; c++) {
          res.at<f64>(r,c) = this->m_data[r][c];
        }
      }
      return res;
    }

    arma::mat ToArmaMatrix4x4() const
    {
      using namespace arma;
      mat A(4, 4);

      for(int i = 0; i < 4; i++)
      {
        for(int j = 0; j < 4; j++){
          A(i,j) = this->m_data[i][j];}
      }
      return A;
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

    static Matrix44d FromArmaMatrix4x4(const arma::mat rhs)
    {
      f64 data[4][4];

      for(s32 r=0; r<4; r++) {
        for(s32 c=0; c<4; c++) {
          data[r][c] = rhs(r,c);
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

  class Matrix66d 
  {
  public:
    f64 m_data[6][6];

    Matrix66d()
    {
      memset(m_data, 0, sizeof(f64)*36);
    }

    Matrix66d(const f64 mat[6][6])
    {
      memcpy(m_data, mat, sizeof(f64)*36);
    }

    Matrix66d(const vnl_matrix<f64> A)
    {
      for( int i = 0; i < 6; i++ )
      {
        for( int j = 0 ; j < 6;  j++ )
        {
          this->m_data[i][j] = A(i,j);
        }
      }
    }

    static Matrix66d I()
    {
      f64 data[6][6] = {0};
      for(int i = 0; i < 6; i++)
      { 
        data[i][i] = 1.0; 
      }
      return Matrix66d(data);
    }

    static Matrix66d OuterProduct(Vec6d a, Vec6d b)
    {
      f64 data[6][6] = {0};

      for(int i = 0; i<6; i++)
      {
        for( int j = 0; j<6; j++)
        {
          data[i][j] = a.at(i)*b.at(j);
        }
      }
      return Matrix66d(data);
    }

    static Matrix66d Diagonal(f64 a, f64 b, f64 c, f64 d, f64 e, f64 f)
    {
      f64 data[6][6] = {0};
      data[0][0] = a;
      data[1][1] = b;
      data[2][2] = c;
      data[3][3] = d;
      data[4][4] = e;
      data[5][5] = f;
      return Matrix66d(data);
    }

    static Matrix66d Diagonal(Vec6d values)
    {
      f64 data[6][6] = {0};
      data[0][0] = values.x;
      data[1][1] = values.y;
      data[2][2] = values.z;
      data[3][3] = values.a;
      data[4][4] = values.b;
      data[5][5] = values.c;
      return Matrix66d(data);
    }

    Matrix66d operator=(const Matrix66d &rhs)
    {
      memcpy(&this->m_data[0][0], &rhs.m_data[0][0], sizeof(f64)*36);
      return *this;  // Return a reference to myself.
    }

    Matrix66d operator+(const Matrix66d &b) const
    {
      f64 res[6][6];
      for( int i = 0; i < 6; i++)
      {
        for( int j = 0; j < 6; j++)
        {
          res[i][j] = this->m_data[i][j] + b.m_data[i][j];
        }
      }
      return Matrix66d(res);
    }

    Matrix66d operator+=(const Matrix66d &b)
    {

      for( int i = 0; i < 6; i++)
      {
        for( int j = 0; j < 6; j++)
        {
          this->m_data[i][j] += b.m_data[i][j];
        }
      }
      return *this;
    }

    Matrix66d operator-(const Matrix66d &b) const
    {
      f64 res[6][6];
      for( int i = 0; i < 6; i++)
      {
        for( int j = 0; j < 6; j++)
        {
          res[i][j] = this->m_data[i][j] - b.m_data[i][j];
        }
      }
      return Matrix66d(res);
    }

    Matrix66d operator*(const f64 &b) const
    {
      f64 res[6][6];
      for( int i = 0; i < 6; i++)
      {
        for( int j = 0; j < 6; j++)
        {
          res[i][j] = this->m_data[i][j] * b;
        }
      }
      return Matrix66d(res);
    }

    Matrix66d operator/(const f64 &b) const
    {
      f64 res[6][6];
      for( int i = 0; i < 6; i++)
      {
        for( int j = 0; j < 6; j++)
        {
          res[i][j] = this->m_data[i][j] / b;
        }
      }
      return Matrix66d(res);
    }

    Matrix66d operator*(const Matrix66d &b) const
    {
      f64 res[6][6] = {0};

      for( int i = 0; i < 6; i++ )
      {
        for( int j = 0 ; j < 6;  j++ )
        {
          for( int k = 0; k<6; k++ )
          {
            res[i][j]=res[i][j]+this->m_data[i][k]*b.m_data[k][j];
          }
        }
      }
      return Matrix66d(res);
    }

    void Print() const
    {
      NTrace("[%f %f %f %f %f %f; %f %f %f %f %f %f; %f %f %f %f %f %f; %f %f %f %f %f %f; %f %f %f %f %f %f; %f %f %f %f %f %f; ]\n", 
        m_data[0][0], m_data[0][1], m_data[0][2], m_data[0][3], m_data[0][4], m_data[0][5], 
        m_data[1][0], m_data[1][1], m_data[1][2], m_data[1][3], m_data[1][4], m_data[1][5],
        m_data[2][0], m_data[2][1], m_data[2][2], m_data[2][3], m_data[2][4], m_data[2][5],
        m_data[3][0], m_data[3][1], m_data[3][2], m_data[3][3], m_data[3][4], m_data[3][5],
        m_data[4][0], m_data[4][1], m_data[4][2], m_data[4][3], m_data[4][4], m_data[4][5],
        m_data[5][0], m_data[5][1], m_data[5][2], m_data[5][3], m_data[5][4], m_data[5][5]);
    }

    Vec6d Col(s32 col) const
    {
      return Vec6d(m_data[0][col], m_data[1][col], m_data[2][col], m_data[3][col], m_data[4][col], m_data[5][col]);
    }

    Vec6d Row(s32 row) const
    {
      return Vec6d(m_data[row][0], m_data[row][1], m_data[row][2], m_data[row][3], m_data[row][4], m_data[row][5]);
    }

    Vec6d operator*(const Vec6d &b) const
    {
      return this->Col(0)*b.x + this->Col(1)*b.y + this->Col(2)*b.z + this->Col(3)*b.a + this->Col(4)*b.b + this->Col(5)*b.c;
    }

    Matrix66d Transpose() const
    {
      f64 res[6][6] = {0};
      for( int i = 0; i < 6; i++ )
      {
        for( int j = 0 ; j < 6;  j++ )
        {
          res[i][j] = this->m_data[j][i];
        }
      }
      return Matrix66d(res);
    }

    Matrix66d Inverse() const
    {
      f64 res[6][6] = {0};
      Matrix66d temp(this->m_data);
      Matrix66d *p_temp = &temp;

      f64 *startRows[6] = { p_temp->m_data[0], p_temp->m_data[1], p_temp->m_data[2], p_temp->m_data[3], p_temp->m_data[4], p_temp->m_data[5] };
      f64 *inverseRows[6] = { res[0], res[1], res[2], res[3], res[4], res[5]  };

      vtkMath::InvertMatrix( startRows, inverseRows, 6);
      return Matrix66d(res);
    }

    vnl_matrix<f64> getVnl() const
    {
      vnl_matrix<f64> A(6,6);
      for( int i = 0; i < 6; i++ )
      {
        for( int j = 0 ; j < 6;  j++ )
        {
          A(i,j) = this->m_data[i][j];
        }
      }
      return A;
    }

    Matrix66d cholesky() const
    {
      vnl_matrix<f64> M = this->getVnl();

      vnl_cholesky chol(M,vnl_cholesky::verbose);
      vnl_matrix<f64> S = chol.upper_triangle();

      return Matrix66d(S);
    }

    /*f64 Determinant() const
    {
    return vtkMatrix4x4::Determinant(&this->m_data[0][0]);
    }*/

    /*vtkSmartPointer<vtkMatrix4x4> GetVTKMatrix() const
    {
    vtkSmartPointer<vtkMatrix4x4> res = vtkSmartPointer<vtkMatrix4x4>::New();
    for(s32 i=0; i<4; i++) {
    for(s32 j=0; j<4; j++) {
    res->SetElement(i,j,this->m_data[i][j]);
    }
    }
    return res;
    }*/
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