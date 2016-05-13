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