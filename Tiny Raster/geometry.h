//these twolines and #endif at end of file are include guards
#ifndef __GEOMETRY_H__ //this line checks if __GEOMETRY_H__ is already defined, prevents double delaration of identifiers like types, enums, or static variables
#define __GEOMETRY_H__ //this line defines it if not      #pragma once serves same purpose as these two lines, but not standard or supported by all IDEs/compilers
//in larger projects you could intentionally or unintentionally include a header file many times which can result in warnings like variable already declared

#include <cmath>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t> struct Vec2 {  //Vec2 is a template for a structure //class in a template declaration means typename, not an actual Class
	union {
		struct {t u, v;}; //the variables here are objects of class T, class t could mean int or float variavles
		struct {t x, y;}; //i dont think these do anything
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u),v(_v) {}
	//the inline keyword makes the compiler replace a function call to the function defined as inline with the actual code of the function
	//these functions are operator overloads, their name is + or * , only applies when operator is used on an object of Vec2
	//const between parameter and function body guarantees and enforces that it will not modify the object's current state, only const functions can
	//call const objects, Vec2 V is const here so it's needed for first two functions
	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u+V.u, v+V.v); } //all these operators are for operator overloading, 
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u-V.u, v-V.v); } //it lets you redefine the behavior of operators for user-defined types like Vec2
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }     //that means (instance of Vec2) + or - or * (second instance of Vec2 or float)
	inline bool operator ==(const Vec2<t>& V) const { if (x == V.x && y == V.y) return true; else return false; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);     //all depends on what is written in these lines
};

template <class t> struct Vec3 {
	union {
		struct {t x, y, z;};
		struct {t u, v, w; };
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
	inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
	inline bool operator ==(const Vec3<t>& V) const { if (x == V.x && y == V.y && z == V.z) return true; else return false; }
	float norm () const { 
		return std::sqrt(x*x+y*y+z*z); 
	}
	Vec3<t> & normalize(t l=1) { 
		*this = (*this)*(l/norm());
		return *this; 
	}
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) { //template for how << operator works with output stream
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {  //also template for how << operator works with output stream buts takes Vec3 arg instead
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

#endif //__GEOMETRY_H__
