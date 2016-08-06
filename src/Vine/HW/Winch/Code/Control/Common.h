#ifndef COMMON_H
#define COMMON_H

#include <math.h>

#include "string.h"

#ifdef __AVR_ATmega2560__
#include "Arduino.h"
#endif

typedef double f64;
typedef char s8;
typedef unsigned char u8;
typedef unsigned long u32;
typedef int s32;
typedef short s16;

#define N_TURN_ACT 3 

#define PI 3.14159265359

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define ABS(x) ((x) > 0 ? (x) : -(x))
#define SIGN(x) ((x) > 0 ? 1 : -1)
#define EPS (1e-7)
#define CLAMP(x,a,b) (MIN(MAX(x,a),b)) 

#define TIME_STEP 0.0333333  //time step of visual control loop in seconds

template < unsigned char N >
class Vecf64
{
public:
  f64 m_data[N];
  Vecf64(f64 *data)
  {
    memcpy(m_data, data, sizeof(f64)*N);
  }

  Vecf64()
  {
    memset(m_data, 0, sizeof(f64)*N);
  }

  operator f64*() { return &m_data[0]; }

  f64 &operator()(s32 idx) { return m_data[idx]; }
  f64 operator()(s32 idx) const { return m_data[idx]; }

#ifdef __AVR_ATmega2560__
  void Print(String name) const
  {
    String outputStr = name + "= [";
    for (s32 idx = 0; idx < N; idx++) {
      outputStr += String(m_data[idx], 6) + " ";
    }
    outputStr += "];";
    Serial.println(outputStr);
  }

  operator String() const
  {
    String outputStr = "[";
    for (s32 idx = 0; idx < N; idx++) {
      outputStr += String(m_data[idx], 6) + " ";
    }
    outputStr += "];";
    return outputStr;
  }
#endif

  Vecf64<N> operator=(const Vecf64 <N> &rhs)
  {
    for (s32 ii = 0; ii < N; ii++)
      m_data[ii] = rhs.m_data[ii];
    return *this;  // Return a reference to myself.
  }

  Vecf64<N> operator=(const f64 &rhs)
  {
    for (s32 ii = 0; ii < N; ii++)
      m_data[ii] = rhs;
    return *this;
  }

  Vecf64<N> operator+(const Vecf64<N> &b) const
  {
    f64 data[N];
    for (s32 ii = 0; ii < N; ii++)
      data[ii] = this->m_data[ii] + b.m_data[ii];
    return Vecf64<N>(data);
  }

  Vecf64<N> operator+=(const Vecf64<N> &b)
  {
    for (s32 ii = 0; ii < N; ii++)
      this->m_data[ii] += b.m_data[ii];
    return *this;
  }

  Vecf64<N> operator-(const Vecf64<N> &b) const
  {
    f64 data[N];
    for (s32 ii = 0; ii < N; ii++)
      data[ii] = this->m_data[ii] - b.m_data[ii];
    return Vecf64<N>(data);
  }

#ifdef __AVR_ATmega2560__
  String ToString()
  {
    String res = "[";
    for (s32 ii = 0; ii < N; ii++) {
      res += String(m_data[ii], 10) + ", ";
    }
    res += "]";
    return res;
  }
#endif

  Vecf64<N> operator-(const f64 &b) const
  {
    f64 data[N];
    for (s32 ii = 0; ii < N; ii++)
      data[ii] = this->m_data[ii] - b;
    return Vecf64<N>(data);
  }

  Vecf64<N> operator+(const f64 &b) const
  {
    f64 data[N];
    for (s32 ii = 0; ii < N; ii++)
      data[ii] = this->m_data[ii] + b;
    return Vecf64<N>(data);
  }

  f64 dot(const Vecf64<N> &b) const
  {
    f64 res = 0;
    for (s32 ii = 0; ii < N; ii++)
      res += this->m_data[ii] * b.m_data[ii];
    return res;
  }

  Vecf64<N> operator*(const f64 &b) const
  {
    f64 data[N] = { 0 };
    for (s32 ii = 0; ii < N; ii++)
      data[ii] = this->m_data[ii] * b;
    return Vecf64<N>(data);
  }

  Vecf64 operator/(const f64 &b) const
  {
    f64 data[N] = { 0 };
    for (s32 ii = 0; ii < N; ii++)
      data[ii] = this->m_data[ii] / b;
    return Vecf64<N>(data);
  }

  f64 magnitudeSquared() const
  {
    return this->dot(*this);
  }

  f64 magnitude() const
  {
    return sqrt(magnitudeSquared());
  }

  Vecf64<N> normalized()
  {
    f64 mag = this->magnitude();
    f64 data[N];

    for (s32 ii = 0; ii < N; ii++)
      data[ii] = this->m_data[ii] / mag;

    return Vecf64<N>(data);
  }

  f64 distanceSquared(const Vecf64<N> &b) const
  {
    Vecf64<N> delta = *this - b;
    return delta.magnitudeSquared();
  }

  bool isZero() const
  {
    return this->magnitude() < 1e-3;
  }
};

template <>
class Vecf64 <2>
{
public:
  union {
    struct {
      f64 x;
      f64 y;
    };
    f64 m_data[2];
  } ;

  Vecf64(f64 x, f64 y)
    : x(x)
    , y(y)
  {}

  Vecf64()
    : x(0)
    , y(0)
  {}

  Vecf64(f64 *data)
  {
    x = data[0];
    y = data[1];
  }

  f64 &operator()(s32 idx) { return idx == 0 ? x : y; }
  f64 operator()(s32 idx) const { return idx == 0 ? x : y; }

  Vecf64<2> operator=(const Vecf64<2> &rhs)
  {
    this->x = rhs.x;
    this->y = rhs.y;
    return *this;  // Return a reference to myself.
  }

  Vecf64<2> operator=(const f64 &rhs)
  {
    this->x = rhs;
    this->y = rhs;
    return *this;
  }

  Vecf64<2> operator+(const Vecf64<2> &b) const
  {
    return Vecf64<2>(this->x + b.x, this->y + b.y);
  }

  Vecf64<2> operator-(const Vecf64<2> &b) const
  {
    return Vecf64<2>(this->x - b.x, this->y - b.y);
  }

  Vecf64<2> operator*(const f64 &b) const
  {
    return Vecf64<2>(this->x*b, this->y*b);
  }

  Vecf64<2> operator+=(const Vecf64<2> &b)
  {
    this->x += b.x;
    this->y += b.y;
    return *this;
  }

#ifdef __AVR_ATmega2560__
  String ToString()
  {
    return "[" + String(this->x, 10) + ", " + String(this->y, 10) + "]";
  }
#endif

  f64 dot(Vecf64<2> b) const
  {
    return this->x*b.x + this->y*b.y;
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

#ifdef __AVR_ATmega2560__
  operator String() const
  {
    String outputStr = "[";
    for (s32 idx = 0; idx < 2; idx++) {
      outputStr += String(m_data[idx], 6) + " ";
    }
    outputStr += "];";
    return outputStr;
  }
#endif
};

template < unsigned char M, unsigned char N >
class Matrixf64
{
public:
  f64 m_data[M][N];

  Matrixf64()
  {
    memset(m_data, 0, sizeof(f64) * M * N);
  }

  Matrixf64(const f64 *mat)
  {
    memcpy(m_data, mat, sizeof(f64) * M * N);
  }

  Matrixf64(const f64 mat[M][N])
  {
    memcpy(m_data, &mat[0][0], sizeof(f64) * M * N);
  }

  static Matrixf64<M,N> Diagonal(f64 *vec)
  {
    f64 data[M][N];
    memset(data, 0, sizeof(f64) * M * N);
    for (s32 ii = 0; ii < MIN(M, N); ii++) {
      data[ii][ii] = vec[ii];
    }
    return Matrixf64<M,N>(data);
  }

  static Matrixf64<M, N> Identity()
  {
    f64 data[M][N];
    memset(data, 0, sizeof(f64) * M * N);
    for (s32 ii = 0; ii < MIN(M, N); ii++) {
      data[ii][ii] = 1;
    }
    return Matrixf64<M, N>(data);
  }

  static Matrixf64<M, N> FromCols(Vecf64<M> *cols)
  {
    f64 data[M][N];
    for (s32 jj = 0; jj < N; jj++) {
      for (s32 ii = 0; ii < M; ii++) {
        data[ii][jj] = cols[jj](ii);
      }
    }
    return Matrixf64<M, N>(data);
  }

  static Matrixf64<M,N> FromRows(Vecf64<N> *rows)
  {
    f64 data[M][N];
    for (s32 ii = 0; ii < M; ii++) {
      for (s32 jj = 0; jj < N; jj++) {
        data[ii][jj] = rows[ii](jj);
      }
    }
    return Matrixf64<M, N>(data);
  }

  f64 &operator()(s32 r, s32 c) { return m_data[r][c]; }

  Matrixf64<M,N> operator=(const Matrixf64<M,N> &rhs)
  {
    memcpy(&this->m_data[0][0], &rhs.m_data[0][0], sizeof(f64) * M * N);
    return *this;  // Return a reference to myself.
  }

  Matrixf64<M,N> operator+(const Matrixf64<M,N> &b) const
  {
    f64 res[M][N];
    for (s32 ii = 0; ii < M; ii++) {
      for (s32 jj = 0; jj < N; jj++) {
        res[ii][jj] = this->m_data[ii][jj] + b.m_data[ii][jj];
      }
    }

    return Matrixf64<M,N>(res);
  }

  Matrixf64<M,N> operator+=(const Matrixf64<M,N> &b)
  {
    for (s32 ii = 0; ii < M; ii++) {
      for (s32 jj = 0; jj < N; jj++) {
        this->m_data[ii][jj] += b.m_data[ii][jj];
      }
    }

    return *this;
  }

  Matrixf64<M,N> operator-(const Matrixf64<M,N> &b) const
  {
    f64 res[M][N];
    for (s32 ii = 0; ii < M; ii++) {
      for (s32 jj = 0; jj < N; jj++) {
        res[ii][jj] = this->m_data[ii][jj] - b.m_data[ii][jj];
      }
    }

    return Matrixf64<M,N>(res);
  }

  Matrixf64<M,N> operator*(const f64 &b) const
  {
    f64 res[M][N] = { 0 };
    for (s32 ii = 0; ii < M; ii++) {
      for (s32 jj = 0; jj < N; jj++) {
        res[ii][jj] = this->m_data[ii][jj] * b;
      }
    }
    return Matrixf64<M,N>(res);
  }

  template <unsigned char O>
  Matrixf64<M, O> operator*(const Matrixf64<N, O> &b)
  {
    f64 data[M][O] = { 0 };
    for (s32 ii = 0; ii < M; ii++) {
      for (s32 jj = 0; jj < O; jj++) {
        for (s32 kk = 0; kk < N; kk++) {
          data[ii][jj] += this->m_data[ii][kk] * b.m_data[kk][jj];
        }
      }
    }
    return Matrixf64<M, O>(data);
  }

#ifdef __AVR_ATmega2560__
  void Print(String name) const
  {
    String outputStr = name + "= [";
    for (s32 rr = 0; rr < M; rr++) {
      for (s32 cc = 0; cc < N; cc++) {
        outputStr += String(m_data[rr][cc], 6) + " ";
      }
      outputStr += ";\n";
    }
    outputStr += "];";
    Serial.println(outputStr);
  }

  operator String() const
  {
    String outputStr = "[\n";
    for (s32 rr = 0; rr < M; rr++) {
      for (s32 cc = 0; cc < N; cc++) {
        outputStr += String(m_data[rr][cc], 6) + " ";
      }
      outputStr += ";\n";
    }
    outputStr += "];";
    return outputStr;
  }
#endif

  Vecf64<M> Col(s32 col) const
  {
    f64 data[M] = { 0 };
    for (s32 ii = 0; ii < M; ii++)
      data[ii] = m_data[ii][col];
    return Vecf64<M>(data);
  }

  void SetCol(s32 idx, const Vecf64<M> &col)
  {
    for (s32 ii = 0; ii < M; ii++)
      m_data[ii][idx] = col.m_data[ii];
  }

  void SetRow(s32 idx, const Vecf64<N> &row)
  {
    for (s32 ii = 0; ii < N; ii++)
      m_data[idx][ii] = row.m_data[ii];
  }

  Vecf64<N> Row(s32 row) const
  {
    f64 data[N] = { 0 };
    for (s32 ii = 0; ii < N; ii++)
      data[ii] = m_data[row][ii];
    return Vecf64<N>(data);
  }

  template < unsigned char O >
  Matrixf64<M,O> operator*(const Matrixf64<N,O> &b) const
  {
    f64 res[M][O];
    memset(&res[0][0], 0, sizeof(f64) * M * O);

    for (s32 ii = 0; ii < M; ii++) {
      for (s32 jj = 0; jj < O; jj++) {
        res[ii][jj] = this->Row(ii).dot(b.Col(jj));
      }
    }

    return Matrixf64<M,O>(res);
  }

  Vecf64<M> operator*(const Vecf64<N> &b) const
  {
    Vecf64<M> res;
    for (s32 ii = 0; ii < N; ii++) {
      res = res + this->Col(ii)*b.m_data[ii];
    }
    return res;
  }

  Matrixf64<N, M> Transpose() const
  {
    f64 res[N][M] = { 0 };
    for (s32 ii = 0; ii < N; ii++) {
      for (s32 jj = 0; jj < M; jj++) {
        res[ii][jj] = this->m_data[jj][ii];
      }
    }

    return Matrixf64<N,M>(res);
  }

  f64 Determinant() const
  {
    if (M == 1 && N == 1) {
      return m_data[0][0];
    }
    else if (M == 3 && N == 3) {
      return m_data[0][0] * (m_data[1][1] * m_data[2][2] - m_data[1][2] * m_data[2][1]) -
        m_data[0][1] * (m_data[1][0] * m_data[2][2] - m_data[1][2] * m_data[2][0]) +
        m_data[0][2] * (m_data[1][0] * m_data[2][1] - m_data[1][1] * m_data[2][0]);
    }
    return NAN;
  }

  Matrixf64<N, M> Inverse() const
  {
    f64 data[N][M] = { 0 };
    if (M == 1 && N == 1) {
      data[0][0] = 1 / m_data[0][0];
    }
    else if (M == 3 && N == 3) {
      f64 invdet = 1.0 / Determinant();
      data[0][0] = (m_data[1][1] * m_data[2][2] - m_data[2][1] * m_data[1][2]) * invdet;
      data[0][1] = (m_data[0][2] * m_data[2][1] - m_data[0][1] * m_data[2][2]) * invdet;
      data[0][2] = (m_data[0][1] * m_data[1][2] - m_data[0][2] * m_data[1][1]) * invdet;
      data[1][0] = (m_data[1][2] * m_data[2][0] - m_data[1][0] * m_data[2][2]) * invdet;
      data[1][1] = (m_data[0][0] * m_data[2][2] - m_data[0][2] * m_data[2][0]) * invdet;
      data[1][2] = (m_data[1][0] * m_data[0][2] - m_data[0][0] * m_data[1][2]) * invdet;
      data[2][0] = (m_data[1][0] * m_data[2][1] - m_data[2][0] * m_data[1][1]) * invdet;
      data[2][1] = (m_data[2][0] * m_data[0][1] - m_data[0][0] * m_data[2][1]) * invdet;
      data[2][2] = (m_data[0][0] * m_data[1][1] - m_data[1][0] * m_data[0][1]) * invdet;
    }
    else {
      for (s32 ii = 0; ii < N; ii++) {
        for (s32 jj = 0; jj < M; jj++) {
          data[N][M] = NAN;
        }
      }
    }
    return Matrixf64<N, M>(data);
  }
};


template <>
class Matrixf64<2,2>
{
public:
  f64 m_data[2][2];

  Matrixf64()
  {
    memset(m_data, 0, sizeof(f64) * 4);
  }

  Matrixf64(const f64 mat[2][2])
  {
    memcpy(m_data, mat, sizeof(f64) * 4);
  }

  Matrixf64(f64 a11, f64 a12, f64 a21, f64 a22)
  {
    m_data[0][0] = a11; m_data[0][1] = a12; 
    m_data[1][0] = a21; m_data[1][1] = a22; 
  }

  static Matrixf64<2,2> I()
  {
    f64 data[2][2];
    data[0][0] = data[1][1] = 1;
    data[0][1] = data[1][0] = 0;
    return Matrixf64<2,2>(data);
  }

  static Matrixf64<2,2> Diagonal(f64 a, f64 b)
  {
    f64 data[2][2];
    memset(data, 0, sizeof(f64) * 4);
    data[0][0] = a;
    data[1][1] = b;
    return Matrixf64<2,2>(data);
  }

  static Matrixf64<2,2> Diagonal(Vecf64<2> values)
  {
    f64 data[2][2];
    memset(data, 0, sizeof(f64) * 4);
    data[0][0] = values.x;
    data[1][1] = values.y;
    return Matrixf64<2,2>(data);
  }

  f64 Trace() const
  {
    f64 res = this->m_data[0][0] + this->m_data[1][1];
    return res;
  }

  static Matrixf64<2,2> FromCols(Vecf64<2> a, Vecf64<2> b)
  {
    f64 data[2][2];
    data[0][0] = a.x;   data[0][1] = b.x;  
    data[1][0] = a.y;   data[1][1] = b.y;  
    return Matrixf64<2,2>(data);
  }

  static Matrixf64<2,2> FromRows(Vecf64<2> a, Vecf64<2> b)
  {
    f64 data[2][2];
    data[0][0] = a.x;  data[0][1] = a.y; 
    data[1][0] = b.x;  data[1][1] = b.y;  
    return Matrixf64<2,2>(data);
  }

  void SetCol(s32 idx, const Vecf64<2> &col)
  {
    for (s32 ii = 0; ii < 2; ii++)
      m_data[ii][idx] = col.m_data[ii];
  }

  void SetRow(s32 idx, const Vecf64<2> &row)
  {
    for (s32 ii = 0; ii < 2; ii++)
      m_data[idx][ii] = row.m_data[ii];
  }

  f64 operator()(s32 r, s32 c) const { return m_data[r][c]; }

  Matrixf64<2,2> operator=(const Matrixf64<2,2> &rhs)
  {
    memcpy(&this->m_data[0][0], &rhs.m_data[0][0], sizeof(f64) * 4);
    return *this;  // Return a reference to myself.
  }

  Matrixf64<2,2> operator+(const Matrixf64<2,2> &b) const
  {
    f64 res[2][2];
    res[0][0] = this->m_data[0][0] + b.m_data[0][0];
    res[0][1] = this->m_data[0][1] + b.m_data[0][1];
    res[1][0] = this->m_data[1][0] + b.m_data[1][0];
    res[1][1] = this->m_data[1][1] + b.m_data[1][1];

    return Matrixf64<2,2>(res);
  }

  Matrixf64<2,2> operator+=(const Matrixf64<2,2> &b)
  {
    this->m_data[0][0] += b.m_data[0][0];
    this->m_data[0][1] += b.m_data[0][1];
    this->m_data[1][0] += b.m_data[1][0];
    this->m_data[1][1] += b.m_data[1][1];

    return *this;
  }

  Matrixf64<2,2> operator-(const Matrixf64<2,2> &b) const
  {
    f64 res[2][2];
    res[0][0] = this->m_data[0][0] - b.m_data[0][0];
    res[0][1] = this->m_data[0][1] - b.m_data[0][1];
    res[1][0] = this->m_data[1][0] - b.m_data[1][0];
    res[1][1] = this->m_data[1][1] - b.m_data[1][1];

    return Matrixf64<2,2>(res);
  }

  Matrixf64<2,2> operator*(const f64 &b) const
  {
    f64 res[2][2];
    res[0][0] = this->m_data[0][0] * b;
    res[0][1] = this->m_data[0][1] * b;
    res[1][0] = this->m_data[1][0] * b;
    res[1][1] = this->m_data[1][1] * b;
    return Matrixf64<2,2>(res);
  }

#ifdef __AVR_ATmega2560__
  void Print() const
  {
    Serial.println("[" + String(m_data[0][0]) + " " + String(m_data[0][1]) + ";\n" +
      String(m_data[1][0]) + " " + String(m_data[1][1]) + "]");
  }
#endif

  Vecf64<2> Col(s32 col) const
  {
    return Vecf64<2>(m_data[0][col], m_data[1][col]);
  }

  Vecf64<2> Row(s32 row) const
  {
    return Vecf64<2>(m_data[row][0], m_data[row][1]);
  }

  Matrixf64<2,2> operator*(const Matrixf64<2,2> &b) const
  {
    f64 res[2][2];
    memset(&res[0][0], 0, sizeof(f64) * 4);

    res[0][0] = this->Row(0).dot(b.Col(0));
    res[1][0] = this->Row(1).dot(b.Col(0));
    res[0][1] = this->Row(0).dot(b.Col(1));
    res[1][1] = this->Row(1).dot(b.Col(1));

    return Matrixf64<2,2>(res);
  }

  Vecf64<2> operator*(const Vecf64<2> &b) const
  {
    return this->Col(0)*b.x + this->Col(1)*b.y;
  }

  Matrixf64<2,2> Transpose() const
  {
    f64 res[2][2];
    res[0][0] = this->m_data[0][0];
    res[0][1] = this->m_data[1][0];
    res[1][0] = this->m_data[0][1];
    res[1][1] = this->m_data[1][1];
    
    return Matrixf64<2,2>(res);
  }

  f64 Determinant() const
  {
    return m_data[0][0] * m_data[1][1] - m_data[0][1] * m_data[1][0];
  }

  Matrixf64<2,2> Inverse() const
  {
    f64 res[2][2];
    res[0][0] = m_data[1][1]; res[0][1] = -m_data[0][1];
    res[1][0] = -m_data[1][0]; res[1][1] = m_data[0][0];

    Matrixf64<2,2> rv(res);
    return rv * (1.0 / Determinant());
  }
};

void sortArray(f64 *a, u8 *idx, s32 nel);

#define MAX_REGULATOR_PWM 155

#define TIME_LOOP(x,y) \
	static u32 g_n##x = 0; static u32 g_begTime##x; \
	if(g_n##x == y) { g_n##x = 0; u32 currTime = millis(); \
										Serial.println("Loop " + String(#x) + " Time = " + String((f64)(currTime-g_begTime##x)/(f64)y) + " ms g_nx " + String(g_n##x)); g_begTime##x = currTime;}\
										g_n##x++;
#endif