#ifndef COMMON_H
#define COMMON_H

#include <math.h>

typedef double f64;
typedef char s8;
typedef unsigned char u8;
typedef unsigned long u32;
typedef int s32;
typedef short s16;

class Vec2f64
{
public:
	f64 x,y;
	Vec2f64(f64 x, f64 y)
		: x(x)
		, y(y)
	{}

	Vec2f64()
		: x(0)
		, y(0)
	{}

	Vec2f64 operator=(const Vec2f64 &rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		return *this;  // Return a reference to myself.
	}

	Vec2f64 operator+(const Vec2f64 &b) const
	{
		return Vec2f64(this->x+b.x, this->y+b.y);
	}

	Vec2f64 operator-(const Vec2f64 &b) const
	{
		return Vec2f64(this->x-b.x, this->y-b.y);
	}

	Vec2f64 operator*(const f64 &b)
	{
		return Vec2f64(this->x*b, this->y*b);
	}

	Vec2f64 operator+=(const Vec2f64 &b)
	{
		this->x += b.x;
		this->y += b.y;
		return *this;
	}
	
	f64 dot(Vec2f64 b) const
	{
		return this->x*b.x+this->y*b.y;
	}

	f64 magnitudeSquared() const
	{
		return this->dot(*this);
	}

	f64 magnitude() const
	{
		return sqrt(magnitudeSquared());
	}

	f64 angle() const 
	{
		return atan2(this->y, this->x);
	}
};  

class Matrix22f64
{
public:
  f64 m_data[2][2];

  Matrix22f64()
  {
    memset(m_data, 0, sizeof(f64) * 4);
  }

  Matrix22f64(const f64 mat[2][2])
  {
    memcpy(m_data, mat, sizeof(f64) * 4);
  }

  Matrix22f64(f64 a11, f64 a12, f64 a21, f64 a22)
  {
    m_data[0][0] = a11; m_data[0][1] = a12; 
    m_data[1][0] = a21; m_data[1][1] = a22; 
  }

  static Matrix22f64 I()
  {
    f64 data[2][2];
    data[0][0] = data[1][1] = 1;
    data[0][1] = data[1][0] = 0;
    return Matrix22f64(data);
  }

  static Matrix22f64 Diagonal(f64 a, f64 b)
  {
    f64 data[2][2];
    memset(data, 0, sizeof(f64) * 4);
    data[0][0] = a;
    data[1][1] = b;
    return Matrix22f64(data);
  }

  static Matrix22f64 Diagonal(Vec2f64 values)
  {
    f64 data[2][2];
    memset(data, 0, sizeof(f64) * 4);
    data[0][0] = values.x;
    data[1][1] = values.y;
    return Matrix22f64(data);
  }

  f64 Trace() const
  {
    f64 res = this->m_data[0][0] + this->m_data[1][1];
    return res;
  }

  static Matrix22f64 FromCols(Vec2f64 a, Vec2f64 b)
  {
    f64 data[2][2];
    data[0][0] = a.x;   data[0][1] = b.x;  
    data[1][0] = a.y;   data[1][1] = b.y;  
    return Matrix22f64(data);
  }

  static Matrix22f64 FromRows(Vec2f64 a, Vec2f64 b)
  {
    f64 data[2][2];
    data[0][0] = a.x;  data[0][1] = a.y; 
    data[1][0] = b.x;  data[1][1] = b.y;  
    return Matrix22f64(data);
  }

  Matrix22f64 operator=(const Matrix22f64 &rhs)
  {
    memcpy(&this->m_data[0][0], &rhs.m_data[0][0], sizeof(f64) * 4);
    return *this;  // Return a reference to myself.
  }

  Matrix22f64 operator+(const Matrix22f64 &b) const
  {
    f64 res[2][2];
    res[0][0] = this->m_data[0][0] + b.m_data[0][0];
    res[0][1] = this->m_data[0][1] + b.m_data[0][1];
    res[1][0] = this->m_data[1][0] + b.m_data[1][0];
    res[1][1] = this->m_data[1][1] + b.m_data[1][1];

    return Matrix22f64(res);
  }

  Matrix22f64 operator+=(const Matrix22f64 &b)
  {
    this->m_data[0][0] += b.m_data[0][0];
    this->m_data[0][1] += b.m_data[0][1];
    this->m_data[1][0] += b.m_data[1][0];
    this->m_data[1][1] += b.m_data[1][1];

    return *this;
  }

  Matrix22f64 operator-(const Matrix22f64 &b) const
  {
    f64 res[2][2];
    res[0][0] = this->m_data[0][0] - b.m_data[0][0];
    res[0][1] = this->m_data[0][1] - b.m_data[0][1];
    res[1][0] = this->m_data[1][0] - b.m_data[1][0];
    res[1][1] = this->m_data[1][1] - b.m_data[1][1];

    return Matrix22f64(res);
  }

  Matrix22f64 operator*(const f64 &b) const
  {
    f64 res[2][2];
    res[0][0] = this->m_data[0][0] * b;
    res[0][1] = this->m_data[0][1] * b;
    res[1][0] = this->m_data[1][0] * b;
    res[1][1] = this->m_data[1][1] * b;
    return Matrix22f64(res);
  }

  void Print() const
  {
    Serial.println("[" + String(m_data[0][0]) + " " + String(m_data[0][1]) + ";\n" +
      String(m_data[1][0]) + " " + String(m_data[1][1]) + "]");
  }

  Vec2f64 Col(s32 col) const
  {
    return Vec2f64(m_data[0][col], m_data[1][col]);
  }

  Vec2f64 Row(s32 row) const
  {
    return Vec2f64(m_data[row][0], m_data[row][1]);
  }

  Matrix22f64 operator*(const Matrix22f64 &b) const
  {
    f64 res[2][2];
    memset(&res[0][0], 0, sizeof(f64) * 4);

    res[0][0] = this->Row(0).dot(b.Col(0));
    res[1][0] = this->Row(1).dot(b.Col(0));
    res[0][1] = this->Row(0).dot(b.Col(1));
    res[1][1] = this->Row(1).dot(b.Col(1));

    return Matrix22f64(res);
  }

  Vec2f64 operator*(const Vec2f64 &b) const
  {
    return this->Col(0)*b.x + this->Col(1)*b.y;
  }

  Matrix22f64 Transpose() const
  {
    f64 res[2][2];
    res[0][0] = this->m_data[0][0];
    res[0][1] = this->m_data[1][0];
    res[1][0] = this->m_data[0][1];
    res[1][1] = this->m_data[1][1];
    
    return Matrix22f64(res);
  }

  f64 Determinant() const
  {
    return m_data[0][0] * m_data[1][1] - m_data[0][1] * m_data[1][0];
  }

  Matrix22f64 Inverse() const
  {
    f64 res[2][2];
    res[0][0] = m_data[1][1]; res[0][1] = -m_data[0][1];
    res[1][0] = -m_data[1][0]; res[1][1] = m_data[0][0];

    Matrix22f64 rv(res);
    return rv * (1.0 / Determinant());
  }
};

#define TIME_LOOP(x,y) \
	static u32 g_n##x = 0; static u32 g_begTime##x; \
	if(g_n##x == y) { g_n##x = 0; u32 currTime = millis(); \
										Serial.println("Loop " + String(#x) + " Time = " + String((f64)(currTime-g_begTime##x)/(f64)y) + " ms g_nx " + String(g_n##x)); g_begTime##x = currTime;}\
										g_n##x++;
#endif

#define PI 3.14159265359

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define ABS(x) ((x) > 0 ? (x) : -(x))
#define EPS (1e-8)
#define CLAMP(x,a,b) (MIN(MAX(x,a),b))