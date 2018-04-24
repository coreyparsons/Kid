#ifndef MATHS_H
#define MATHS_H

#define ROOT2 1.4142135623730950488016887f
#define PI 3.14159265359f

#include <math.h>

struct Vec2
{
	float x;
	float y;
};

inline Vec2 operator+(Vec2 a, Vec2 b)
{
	Vec2 result = { a.x + b.x, a.y + b.y };
	return result;
}

inline Vec2 & operator+=(Vec2 &a, Vec2 b)
{
	a = (a + b);
	return a;
}


inline Vec2 operator-(Vec2 a, Vec2 b)
{
	Vec2 result = { a.x - b.x, a.y - b.y };
	return result;
}

inline Vec2 & operator-=(Vec2 &a, Vec2 b)
{
	a = (a - b);
	return a;
}


inline Vec2 operator/(Vec2 a, Vec2 b) //don't know if i want this
{
	Vec2 result = { a.x / b.x, a.y / b.y };
	return result;
}


inline Vec2 operator*(Vec2 a, Vec2 b) //don't know if i want this
{
	Vec2 result = { a.x * b.x, a.y * b.y };
	return result;
}


inline Vec2 operator*(Vec2 a, float b)
{
	Vec2 result = { a.x * b, a.y * b };
	return result;
}

inline Vec2 operator*(float b, Vec2 a)
{
	Vec2 result = { a.x * b, a.y * b };
	return result;
}

inline Vec2 & operator*=(Vec2 &a, float b)
{
	a = (a * b);
	return a;
}


inline Vec2 operator/(Vec2 a, float b)
{
	Vec2 result = { a.x / b, a.y / b };
	return result;
}

inline Vec2 operator/(float b, Vec2 a)
{
	Vec2 result = { a.x / b, a.y / b };
	return result;
}

inline Vec2 & operator/=(Vec2 &a, float b)
{
	a = (a / b);
	return a;
}

inline Vec2 operator+(Vec2 a, float b)
{
	Vec2 result = { a.x + b, a.y + b };
	return result;
}

inline Vec2 operator+(float b, Vec2 a)
{
	Vec2 result = { a.x + b, a.y + b };
	return result;
}

inline Vec2 operator-(Vec2 a, float b)
{
	Vec2 result = { a.x - b, a.y - b };
	return result;
}

inline Vec2 operator-(float b, Vec2 a)
{
	Vec2 result = { b - a.x, b - a.y };
	return result;
}

inline float dot(Vec2 a, Vec2 b)
{
	float result = (a.x*b.x + a.y*b.y);
	return result;
}

inline float absolute(float a)
{
	float result = (a < 0) ? -a : a;
	return result;
}

inline Vec2 absolute(Vec2 a)
{
	Vec2 result = { absolute(a.x), absolute(a.y) };
	return result;
}

inline float absdot(Vec2 a, Vec2 b)
{
	float result = absolute(a.x*b.x + a.y*b.y);
	return result;
}


inline int roundToI(float f)
{
	int result = (int)(f + 0.5f);
	return result;
}

inline Vec2 vectorise(float angle, float magnitude)
{
	Vec2 result = { (magnitude * (cos(angle))), (magnitude * (sin(angle))) };
	return result;
}

inline float min(float a, float b)
{
	float result = (a < b) ? a : b;
	return result;
}

inline float max(float a, float b)
{
	float result = (a > b) ? a : b;
	return result;
}

inline float clamp(float a, float minA, float maxA)
{
	float result = max( min(a, maxA ), minA);
	return result;
}

inline float square(float a)
{
	float result = a * a;
	return result;
}

inline float mag(Vec2 a)
{
	float result = sqrtf(square(a.x) + square(a.y));
	return result;
}

inline Vec2 unitLen(Vec2 a)
{
	float magnitude = mag(a);
	if (magnitude == 0.0f)
	{
		return Vec2{ 0, 0 };
	}
	Vec2 result = { a.x / magnitude, a.y / magnitude };
	return result;
}

inline Vec2 roundToI(Vec2 a)
{
	Vec2 result = { (float)roundToI(a.x), (float)roundToI(a.y) };
	return result;
}

inline Vec2 floorVec(Vec2 a)
{
	Vec2 result = { (float)((int)a.x), (float)((int)a.y) };
	return result;
}

inline float lerp(float start, float end, float t)
{
	float result = (start + ((end - start) * t));
	return result;
}

inline Vec2 lerp(Vec2 start, Vec2 end, float t)
{
	Vec2 result = { lerp(start.x, end.x, t), lerp(start.y, end.y, t) };
	return result;
}

inline float randInt(float a, float b)
{
	float result = (float)((int)a + (rand() % (int)(b - a)));
	return result;
}

#if 0 //didn't need to use this in the end
inline float normalDist(float x) //x must be between 0 and 1
{
	float y = (sin(2.0f * PI * (x - 1.0f / 4.0f)) + 1.0f) / 2.0f;
	return y;
}
#endif

#endif