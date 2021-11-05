// ========================================================================= //
// Author: Daniel Ströter, Matthias Bein                                     //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Creation Date: 04.11.2009                                                 //
// Edited: 29.04.2020                                                        //
// ========================================================================= //

#pragma once
#include <math.h>

#define M_RadToDeg 0.0174532925f
#define EPS 0.00001f

template<typename T>
struct Vec3
{
private:
	T data[3];

public:
	Vec3() { zero(); };
	Vec3(const T x, const T y, const T z) : data{ x,y,z } {}
	Vec3(const T v) : data{ v,v,v } {}


	T x() const { return data[0]; }
	T y() const { return data[1]; }
	T z() const { return data[2]; }

	T& x() { return data[0]; }
	T& y() { return data[1]; }
	T& z() { return data[2]; }
	T operator[] (const unsigned int i) const { return data[i]; }
	T&      operator[] (const unsigned int i) { return data[i]; }


	Vec3& operator+= (const Vec3 &v)
	{
		data[0] += v[0];
		data[1] += v[1];
		data[2] += v[2];
		return *this;
	}
	Vec3& operator-= (const Vec3 &v)
	{
		data[0] -= v[0];
		data[1] -= v[1];
		data[2] -= v[2];
		return *this;
	}
	Vec3& operator*= (const T f)
	{
		data[0] *= f;
		data[1] *= f;
		data[2] *= f;
		return *this;
	}
	Vec3& operator/= (const T f)
	{
		data[0] /= f;
		data[1] /= f;
		data[2] /= f;
		return *this;
	}

	friend Vec3 operator+ (Vec3 v_0, const Vec3 &v_1)
	{
		v_0 += v_1;
		return v_0;
	}

	friend Vec3 operator- (Vec3 v_0, const Vec3 &v_1)
	{
		v_0 -= v_1;
		return v_0;
	}

	T operator* (const Vec3 &v) const
	{
		return x()*v.x() + y()*v.y() + z()*v.z();
	}

	friend Vec3 operator* (Vec3 v, const T f)
	{
		v *= f;
		return v;
	}

	friend Vec3 operator* (const T f, Vec3 v)
	{
		return v * f;
	}

	friend Vec3 operator/ (Vec3 v, const T f)
	{
		v /= f;
		return v;
	}

	T length()                      const { return sqrt(x()*x() + y()*y() + z()*z()); }
	T sqlength()                    const { return x()*x() + y()*y() + z()*z(); }
	T distance(const Vec3 &v)       const { return (v - *this).length(); }

	// normalizes this vector (division by length). returns false when length is < EPS
	bool normalize()
	{
		T l = this->length();
		if (std::fabs(l) < EPS)
			return false;
		*this /= l;
		return true;
	}
	Vec3 normalized() const
	{
		T l = this->length();
		if (std::fabs(l) < EPS)
			return *this;
		return *this / l;
	}
	void zero()
	{
		data[0] = 0;
		data[1] = 0;
		data[2] = 0;
	}
	// rotates the vector around x (angle in degree)
	void rotX(float angle)
	{
		float y_new = cos(angle*M_RadToDeg)*y() - sin(angle*M_RadToDeg)*z();
		float z_new = sin(angle*M_RadToDeg)*y() + cos(angle*M_RadToDeg)*z();
		y() = y_new;
		z() = z_new;
	}
	// rotates the vector around y (angle in degree)
	void rotY(float angle)
	{
		float x_new = cos(angle*M_RadToDeg)*x() + sin(angle*M_RadToDeg)*z();
		float z_new = -sin(angle*M_RadToDeg)*x() + cos(angle*M_RadToDeg)*z();
		x() = x_new;
		z() = z_new;
	}
	// rotates the vector around z (angle in degree)
	void rotZ(float angle)
	{
		float x_new = cos(angle*M_RadToDeg)*x() - sin(angle*M_RadToDeg)*y();
		float y_new = sin(angle*M_RadToDeg)*x() + cos(angle*M_RadToDeg)*y();
		x() = x_new;
		y() = y_new;
	}
};

template <class T>
Vec3<T> cross(const Vec3<T> &v_0, const Vec3<T> &v_1)
{
	Vec3<T> result;
	result.x() = v_0.y()*v_1.z() - v_0.z()*v_1.y();
	result.y() = v_0.z()*v_1.x() - v_0.x()*v_1.z();
	result.z() = v_0.x()*v_1.y() - v_0.y()*v_1.x();
	return result;
}

typedef Vec3<float>         Vec3f;
typedef Vec3<int>           Vec3i;
typedef Vec3<unsigned int>  Vec3ui;