#pragma once
#include <cmath>
/*
*	Common functions and structures for graphics
*/

namespace intersector
{

	/********
	* Vec3f *
	*********/

	struct Vec3f
	{
		float x;
		float y;
		float z;
	};

	// constructors for Vec3f
	inline Vec3f makeVec3f(const float x, const float y, const float z) { Vec3f v; v.x = x; v.y = y; v.z = z; return v; }
	inline Vec3f makeVec3f(const float a) { return makeVec3f(a, a, a); }

	// unary operations on Vec3f

	inline Vec3f operator-(const Vec3f a) { return makeVec3f(-a.x, -a.y, -a.z); }
	inline Vec3f neg(const Vec3f a) { return -a; }
	inline Vec3f abs(const Vec3f a)	{ return makeVec3f(std::abs(a.x), std::abs(a.y), std::abs(a.z)); }
	inline Vec3f exp(const Vec3f a) { return makeVec3f(std::exp(a.x), std::exp(a.y), std::exp(a.z)); }

	// binary operators on Vec3f

	inline Vec3f operator+(const Vec3f a, const Vec3f b) { return makeVec3f(a.x + b.x, a.y + b.y, a.z + b.z); }
	inline Vec3f operator-(const Vec3f a, const Vec3f b) { return makeVec3f(a.x - b.x, a.y - b.y, a.z - b.z); }
	inline Vec3f operator*(const Vec3f a, const Vec3f b) { return makeVec3f(a.x * b.x, a.y * b.y, a.z * b.z); }
	inline Vec3f operator*(const float a, const Vec3f b) { return makeVec3f(a * b.x, a * b.y, a * b.z); }
	inline Vec3f operator*(const Vec3f a, const float b) { return b * a; }
	inline Vec3f operator/(const Vec3f a, const Vec3f b) { return makeVec3f(a.x / b.x, a.y / b.y, a.z / b.z); }
	inline Vec3f operator/(const float a, const Vec3f b) { return makeVec3f(a / b.x, a / b.y, a / b.z); }
	inline Vec3f operator/(const Vec3f a, const float b) { return makeVec3f(a.x / b, a.y / b, a.z / b); }
	inline Vec3f min(const Vec3f a, const Vec3f b) { return makeVec3f(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z)); }
	inline Vec3f max(const Vec3f a, const Vec3f b) { return makeVec3f(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z)); }
	inline Vec3f pow(const Vec3f a, const float b) { return makeVec3f(powf(a.x, b), powf(a.y, b), powf(a.z, b)); }

	// comparison operators

	inline bool operator==(const Vec3f a, const Vec3f b) { return (a.x == b.x) && (a.y == b.y) && (a.z == b.z); }
	inline bool operator!=(const Vec3f a, const Vec3f b) { return (a.x != b.x) || (a.y != b.y) || (a.z != b.z); }
	inline bool eq(const Vec3f a, const Vec3f b) { return a == b; }
	inline bool neq(const Vec3f a, const Vec3f b) { return a != b; }

	// Euclidian space operators

	inline float dot(const Vec3f a, const Vec3f b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
	inline float length(const Vec3f a) { return sqrtf(dot(a, a)); }
	inline float distance(const Vec3f a, const Vec3f b) { return length(a - b); }
	inline Vec3f normalize(const Vec3f a) { return a / length(a); }
	inline Vec3f cross(const Vec3f a, const Vec3f b) { return makeVec3f(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
	inline Vec3f reflect(const Vec3f a, const Vec3f N) { return 2.0f * dot(a, N) * N - a; }

	/*********
	* Vec3fa *
	**********/

	struct Vec3fa
	{
		float x;
		float y;
		float z;
		float w;
	};

	// constructors

	inline Vec3fa makeVec3fa(const float x, const float y, const float z) { Vec3fa v; v.x = x; v.y = y; v.z = z; v.w = 1.0f; return v; }
	inline Vec3fa makeVec3fa(const float x, const float y, const float z, const float w) { Vec3fa v; v.x = x; v.y = y; v.z = z; v.w = w; return v; }
	inline Vec3fa makeVec3fa(const float a) { return makeVec3fa(a, a, a); }
	inline Vec3fa makeVec3fa(const Vec3f a) { return makeVec3fa(a.x, a.y, a.z); }

	// unary operations on Vec3fa

	inline Vec3fa operator-(const Vec3fa a) { return makeVec3fa(-a.x, -a.y, -a.z, -a.w); }
	inline Vec3fa neg(const Vec3fa a) { return -a; }
	inline Vec3fa abs(const Vec3fa a) { return makeVec3fa(std::abs(a.x), std::abs(a.y), std::abs(a.z), std::abs(a.w)); }
	inline Vec3fa exp(const Vec3fa a) { return makeVec3fa(std::exp(a.x), std::exp(a.y), std::exp(a.z), std::exp(a.w)); }

	// binary operators on Vec3fa

	inline Vec3fa operator+(const Vec3fa a, const Vec3fa b) { return makeVec3fa(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
	inline Vec3fa operator-(const Vec3fa a, const Vec3fa b) { return makeVec3fa(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
	inline Vec3fa operator*(const Vec3fa a, const Vec3fa b) { return makeVec3fa(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w); }
	inline Vec3fa operator*(const float a, const Vec3fa b)  { return makeVec3fa(a*b.x, a*b.y, a*b.z, a*b.w); }
	inline Vec3fa operator*(const Vec3fa a, const float b) 	{ return b * a; }
	inline Vec3fa operator/(const Vec3fa a, const Vec3fa b) { return makeVec3fa(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }
	inline Vec3fa operator/(const float a, const Vec3fa b) 	{ return makeVec3fa(a / b.x, a / b.y, a / b.z, a / b.w); }
	inline Vec3fa operator/(const Vec3fa a, const float b) 	{ return makeVec3fa(a.x / b, a.y / b, a.z / b, a.w / b); }
	inline Vec3fa min(const Vec3fa a, const Vec3fa b) { return makeVec3fa(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z), fminf(a.w, b.w)); }
	inline Vec3fa max(const Vec3fa a, const Vec3fa b) { return makeVec3fa(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z), fmaxf(a.w, b.w)); }
	inline Vec3fa pow(const Vec3fa a, const float b)  { return makeVec3fa(powf(a.x, b), powf(a.y, b), powf(a.z, b), powf(a.w, b)); }

	// comparison operators

	inline bool operator==(const Vec3fa a, const Vec3fa b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
	inline bool operator!=(const Vec3fa a, const Vec3fa b) { return a.x != b.x || a.y != b.y || a.z != b.z; }
	inline bool eq(const Vec3fa a, const Vec3fa b) { return a == b; }
	inline bool neq(const Vec3fa a, const Vec3fa b) { return a != b; }

	// Euclidian space operators

	inline float dot(const Vec3fa a, const Vec3fa b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
	inline float length(const Vec3fa a) { return sqrtf(dot(a, a)); }
	inline Vec3fa normalize(const Vec3fa a) { return a / length(a); }
	inline float distance(const Vec3fa a, const Vec3fa b) { return length(a - b); }
	inline Vec3fa cross(const Vec3fa a, const Vec3fa b) { makeVec3fa(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x); }

	/**************
	* LinearSpace *
	***************/

	struct LinearSpace3f
	{
		Vec3f vx;
		Vec3f vy;
		Vec3f vz;
	};

	// constructors

	inline LinearSpace3f makeLinearSpace3f(const Vec3f x, Vec3f y, Vec3f z) { LinearSpace3f l; l.vx = x; l.vy = y; l.vz = z; return l; }
	inline LinearSpace3f makeLinearSpace3f_identity() { return makeLinearSpace3f(makeVec3f(1.0f, 0.0f, 0.0f), makeVec3f(0.0f, 1.0f, 0.0f), makeVec3f(0.0f, 0.0f, 1.0f)); }

	// unary operators

	inline LinearSpace3f neg(const LinearSpace3f l) { return makeLinearSpace3f(-l.vx, -l.vy, -l.vz); }
	inline float det(const LinearSpace3f l) { return dot(l.vx, cross(l.vy, l.vz)); }
	inline LinearSpace3f transposed(const LinearSpace3f l) { return makeLinearSpace3f(makeVec3f(l.vx.x, l.vy.x, l.vz.x), makeVec3f(l.vx.y, l.vy.y, l.vz.y), makeVec3f(l.vx.z, l.vy.z, l.vz.z)); }
	inline LinearSpace3f adjoint(const LinearSpace3f l) { return transposed(makeLinearSpace3f(cross(l.vy, l.vz), cross(l.vz, l.vx), cross(l.vx, l.vy))); }

	// binary operators

	inline LinearSpace3f operator+(const LinearSpace3f a, const LinearSpace3f b) { return makeLinearSpace3f(a.vx + b.vx, a.vy + b.vy, a.vz + b.vz); }
	inline LinearSpace3f operator-(const LinearSpace3f a, const LinearSpace3f b) { return makeLinearSpace3f(a.vx - b.vx, a.vy - b.vy, a.vz - b.vz); }
	inline Vec3f operator*(const LinearSpace3f l, Vec3f v) { return v.x*l.vx + v.y*l.vy + v.z*l.vz; }
	inline LinearSpace3f operator*(const LinearSpace3f a, const LinearSpace3f b) { return makeLinearSpace3f(a * b.vx, a*b.vy, a*b.vz); }
	inline LinearSpace3f operator*(const float a, const LinearSpace3f b) { return makeLinearSpace3f(a*b.vx, a*b.vy, a*b.vz); }
	inline LinearSpace3f operator*(const LinearSpace3f a, const float b) { return b * a; }
	inline Vec3f xfmVector(const LinearSpace3f l, Vec3f v) { return l * v; }

	// comparison operators

	inline bool operator==(const LinearSpace3f a, const LinearSpace3f b) { return a.vx == b.vx && a.vy == b.vy && a.vz == b.vz; }
	inline bool operator!=(const LinearSpace3f a, const LinearSpace3f b) { return a.vx != b.vx || a.vy != b.vy || a.vz != b.vz; }
	inline bool eq(const LinearSpace3f a, const LinearSpace3f b) { return a == b; }
	inline bool neq(const LinearSpace3f a, const LinearSpace3f b) { return a != b; }

	/**************
	* AffineSpace *
	***************/

	struct AffineSpace3f
	{
		LinearSpace3f l;
		Vec3f p;
	};

	// constructors

	inline AffineSpace3f makeAffineSpace3f(const LinearSpace3f l, const Vec3f p) { AffineSpace3f a; a.l = l; a.p = p; return a; }
	inline AffineSpace3f makeAffineSpace3f(const Vec3f x, const Vec3f y, const Vec3f z, const Vec3f p) { AffineSpace3f a; a.l.vx = x; a.l.vy = y; a.l.vz = z; a.p = p; return a; }

	// unary operators

	inline AffineSpace3f neg(const AffineSpace3f a) { return makeAffineSpace3f(neg(a.l), neg(a.p)); }

	// binary operators

	inline AffineSpace3f operator+(const AffineSpace3f a, const AffineSpace3f b) { return makeAffineSpace3f(a.l + b.l, a.p + b.p); }
	inline AffineSpace3f operator-(const AffineSpace3f a, const AffineSpace3f b) { return makeAffineSpace3f(a.l - b.l, a.p - b.p); }
	inline AffineSpace3f operator*(const float a, const AffineSpace3f b) { return makeAffineSpace3f(a * b.l, a * b.p); }
	inline AffineSpace3f operator*(const AffineSpace3f a, const float b) { return b * a; }
	inline AffineSpace3f operator*(const AffineSpace3f a, const AffineSpace3f b) { return makeAffineSpace3f(a.l*b.l, a.l*b.p + a.p); }
	inline Vec3f xfmVector(const AffineSpace3f a, const Vec3f v) { return xfmVector(a.l, v); }
	inline Vec3fa xfmVector(const AffineSpace3f a, const Vec3fa v) { Vec3f va = makeVec3f(v.x, v.y, v.z); Vec3f vv = xfmVector(a.l, va); return makeVec3fa(vv.x, vv.y, vv.z, v.w); }
	inline Vec3f xfmPoint(const AffineSpace3f a, const Vec3f v) { return xfmVector(a.l, v) + a.p; }
	inline Vec3fa xfmPoint(const AffineSpace3f a, const Vec3fa v) { Vec3f va = makeVec3f(v.x, v.y, v.z); Vec3f vv = xfmVector(a.l, va) + a.p; return makeVec3fa(vv.x, vv.y, vv.z, v.w); }

	// comparison operators

	inline bool operator==(const AffineSpace3f a, const AffineSpace3f b) { return a.l == b.l; }
	inline bool operator!=(const AffineSpace3f a, const AffineSpace3f b) { return a.l != b.l; }
	inline bool eq(const AffineSpace3f a, const AffineSpace3f b) { return a == b; }
	inline bool neq(const AffineSpace3f a, const AffineSpace3f b) { return a != b; }

}