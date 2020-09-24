
//! File: <LinearAlgebra.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

//! Linear algebra types. All types herein are public. 

#ifndef _HAPTX_LINEARALGEBRA_H_
#define _HAPTX_LINEARALGEBRA_H_

#include <iostream>

#include "Util.h"

//! A function is declared using exception specification, which Visual C++ accepts but does not implement
//! See Warning C4290 for more details.
#pragma warning(disable : 4290)

namespace HaptX 
{
	// The Vectors namespace is used to encapsulate geometric vector
	// types. 
	//
	// Note that a vector dot product can be performed either with the dot member
	// function or the * operator. A cross product can be performed either with the cross
	// member function or the % operator. 
	namespace Vectors 
	{
		// Vec2f is a 2-dimensional vector with mgFloats. 
		class HAPTX_SOLVER_SPEC Vec2f 
		{
		public:
			// Constructor
			inline Vec2f()													{}
			// Constructor
			inline Vec2f(const mgFloat _x, const mgFloat _y) : x(_x), y(_y) {}

			// Calculate the dot product between two vectors.
			inline mgFloat			Dot(const Vec2f& r) const;
			// Calculate the length of this vector
			inline mgFloat			Length() const;

			inline mgFloat			LengthSqr() const;

			inline mgFloat&			operator [] (const int& i);
			inline const mgFloat&	operator [] (const int& i) const;

			// Normalize makes the vector a unit vector. If the vector already is very close to 
			// unit length, nothing is done. This version does not check for zero length, in which 
			// case a floating-point error will occur. see normalizeSafe. 
			inline void Normalize();

			// NormalizeSafe makes the vector a unit vector. NormalizeSafe checks for zero length, 
			// in which case nothing is done. 
			inline void NormalizeSafe();

			// SetLength sets a vector to the given length. 
			inline void SetLength(const mgFloat& length);

			// SetLength sets the length of the vector it operates on to be 
			// of the same length as the provided "example" vector. 
			inline void SetLength(const Vec2f& example);

		public:
			
			union { mgFloat x; mgFloat u; mgFloat s; };
			union { mgFloat y; mgFloat v; mgFloat t; };
		};
    
    
		// Vec3f is a 3-dimensional vector with mgFloats. Note that a cross product 
		// can be peformed either with the cross member function or the % and %= operators.
		class HAPTX_SOLVER_SPEC Vec3f 
		{
		public:
			// Constructor
			inline Vec3f(){}
			// Constructor
			inline Vec3f(const mgFloat& _x, const mgFloat& _y, const mgFloat& _z) : x(_x), y(_y), z(_z) {}

			// Calculate the dot product between two vectors
			inline mgFloat			Dot(const Vec3f& r) const;
			// Calculate the cross product between two vectors
			inline Vec3f			Cross(const Vec3f& r) const;
			// Calculate the length of the vector
			inline mgFloat			Length() const;
			inline mgFloat			LengthSqr() const;
			inline mgFloat&			operator [] (const int& i);
			inline const mgFloat&	operator [] (const int& i) const;

			// Normalize makes the vector a unit vector. If the vector already is very close to 
			// unit length, nothing is done. This version does not check for zero length, in which 
			// case a floating-point error will occur. see normalizeSafe. 
			inline void Normalize();

			// NormalizeSafe makes the vector a unit vector. NormalizeSafe checks for zero length, 
			// in which case nothing is done. 
			inline void NormalizeSafe();

			// SetLength sets a vector to the given length. 
			inline void SetLength(const mgFloat& length);

			// SetLength sets the length of the vector it operates on to be 
			// of the same length as the provided "example" vector. 
			inline void SetLength(const Vec3f& example);

			// Converts a vector from spherical-polar coordinates to cartesian coordinates. Spherical-polar 
			// coordinates are represented as ( x == theta, y == phi, z == radius ), where the phi==0 pole 
			// is to be the positive y axis, and the theta==0 plane is the z<0,x==0 half-plane. 
			inline Vec3f Sph2Cart() const;

			// Converts a vector from cartesian coordinates to spherical-polar coordinates. Spherical-polar 
			// coordinates are represented as ( x == theta, y == phi, z == radius ), where the phi==0 pole 
			// is to be the positive y axis, and the theta==0 plane is the z<0,x==0 half-plane. 
			inline Vec3f Cart2Sph() const;

		public:
			
			union { mgFloat x; mgFloat u; mgFloat s; };
			union { mgFloat y; mgFloat v; mgFloat t; };
			union { mgFloat z; mgFloat w; mgFloat r; };
		};


		// Vec4f is a 4-dimensional vector with mgFloats. Note: the order of 
		// the elements of a Vec4f is x, y, z, w. 
		class HAPTX_SOLVER_SPEC Vec4f 
		{
		public:
			// Constructor
			inline Vec4f(){}
			// Constructor
			inline Vec4f(const mgFloat& _x, const mgFloat& _y, const mgFloat& _z, const mgFloat& _w) : x(_x), y(_y), z(_z), w(_w)	{}

			inline mgFloat&			operator [] (const int& i);
			inline const mgFloat&	operator [] (const int& i) const;

		public:
			
			union { mgFloat x; mgFloat s; };
			union { mgFloat y; mgFloat t; };
			union { mgFloat z; mgFloat r; };
			union { mgFloat w; mgFloat q; };
		};

		class Rotation;
		class Quaternion;

	
		// Matrix3f is a 3x3 matrix of mgFloats. 
		class HAPTX_SOLVER_SPEC Matrix3f 
		{
		public:
			// Constructor
			inline Matrix3f() {}
			// Constructor
			inline Matrix3f(const mgFloat& m00,const mgFloat& m01,const mgFloat& m02,
						    const mgFloat& m10,const mgFloat& m11,const mgFloat& m12,
						    const mgFloat& m20,const mgFloat& m21,const mgFloat& m22);

			// Set to the identity matrix.
			inline void SetToIdentity();

			// Calculate the inverse of the matrix. 
			Matrix3f Inverse() const throw(...);

			// Calculate the single-value-decomposition inverse of the matrix. It deals with 
			// singular matrices by returning the least-squared best inverse for the matrix.
			Matrix3f SvdInverse() const;

			// Calculate the scaling components of the matrix. The elements are returned unordered. 
			Vec3f ScalingComponents() const;

			// Calculate the squares of the scaling components of the matrix.
			// The elements are returned unordered. 
			Vec3f ScalingComponentsSqr() const;

			// Calculate the eigenvectors and eigenvalues for this matrix using the method of 
			// Jacobi Transformations. Note: the matrix must be symmetric. The normalized 
			// eigenvectors are placed in the columns of the returned "eigenvectors" matrix. 
			// The eigenvalues are returned in eigenvalues.
			void Jacobi(Matrix3f& eigenvectors, Vec3f& eigenvalues) throw(...);

			// Returns the vector x such that *this * x == y. Gauss-Jordan elimination with full 
			// pivoting is used. If the method succeeds the matrix will be inverted, otherwise the 
			// contents are undefined. 
			Vec3f Solve(const Vec3f& y) throw(...);

			// Casts to a rotation, assuming the source matrix is orthogonal (a rotation matrix). 
			operator Rotation() const;

			// Casts to a quaternion, assuming the source matrix is orthogonal (a rotation matrix).
			operator Quaternion() const;
			
			inline void		SetRow(const int i, const Vec3f& v);		// Set a row in the matrix.
			inline Vec3f	GetRow(const int i) const;					// Get a row in the matrix.
			inline void		SetColumn(const int i, const Vec3f& v);		// Set a column in the matrix.
			inline Vec3f	GetColumn(const int i) const;				// Get a column in the matrix. 

			inline mgFloat*			operator [] (const int i)			{ return m[i]; }
			inline const mgFloat*	operator [] (const int i) const		{ return m[i]; }
		
		public:
			mgFloat m[3][3];
		};


		// Matrix4f is a 4x4 matrix of mgFloats. 	
		class HAPTX_SOLVER_SPEC Matrix4f 
		{
		public:
			// Constructor
			inline Matrix4f() { SetToIdentity(); }
			// Constructor
			inline Matrix4f(const mgFloat& m00, const mgFloat& m01, const mgFloat& m02, const mgFloat& m03,
							const mgFloat& m10, const mgFloat& m11, const mgFloat& m12, const mgFloat& m13,
						    const mgFloat& m20, const mgFloat& m21, const mgFloat& m22, const mgFloat& m23,
						    const mgFloat& m30, const mgFloat& m31, const mgFloat& m32, const mgFloat& m33);

			// Set to the identity matrix. 
			inline void SetToIdentity();

			// Returns the inverse of the matrix. 
			Matrix4f Inverse() const throw(...);

			// Returns the inverse of the matrix, assuming the matrix is of the form:
			// [m00 m01 m02 m03]
			// [m10 m11 m12 m13]
			// [m20 m21 m22 m23]
			// [ 0   0   0   1 ] 
			Matrix4f TransformInverse() const throw(...);

			// Calculate the scaling components of the matrix. The elements are returned unordered. 
			Vec3f ScalingComponents() const;

			// Calculate the squares of the scaling components of the matrix. 
			// The elements are returned unordered. 
			Vec3f ScalingComponentsSqr() const;

			// Get the scaling and rotation components of the matrix (the upper left 3x3 block). 
			inline Matrix3f GetScaleRotation() const;

			inline operator Matrix3f() const;

			// Casts to a rotation assuming the source matrix is orthogonal (a rotation matrix).
			// Ignores the translational component. 
			operator Rotation() const;

			// Casts to a quaternion assuming the source matrix is orthogonal (a rotation matrix). 
			// Ignores the translational component. 
			operator Quaternion() const;

			// ScaleRotate is similar to a post-multiplication with a Vec3f, except the fourth
			// component of the vector is assumed to be 0 (which means a vector transformation rather
			// than point transformation). 
			inline Vec3f ScaleRotate(const Vec3f& v) const;

			// Rotates the given vector without changing its length.
			inline Vec3f Rotate(const Vec3f& v) const;

			// Returns the vector x such that *this * x == y. Gauss-Jordan elimination with full 
			// pivoting is used. If the method succeeds the matrix will be inverted, otherwise the 
			// contents are undefined. 
			Vec4f Solve(const Vec4f& y) throw(...);

			inline mgFloat*			operator [] (const int i)			{ return m[i]; }
			inline const mgFloat*	operator [] (const int i) const		{ return m[i]; }

		public:
			mgFloat m[4][4];
		};


		// Rotation describes an arbitrary rotation about an axis vector. x, y, z define the axis 
		// vector and a is the angle in radians in the right-handed direction (with your right hand, 
		// use your thumb as the axis - your fingers point in the direction of positive rotation). 
		// The axis vector must be of unit length.
		class HAPTX_SOLVER_SPEC Rotation 
		{
		public:
			// Constructor
			inline Rotation() {}
			// Constructor
			inline Rotation(const mgFloat& _x, const mgFloat& _y, const mgFloat& _z, const mgFloat& _angle) : axis(_x,_y,_z), angle(_angle) { axis.NormalizeSafe(); }
			// Constructor
			inline Rotation(const Vec3f& _axis, const mgFloat& _angle) : axis(_axis), angle(_angle)											{ axis.NormalizeSafe(); }

			// Calculate the rotation from n1 to n2, where n1 and n2 are unit vectors. If n1 is 
			// close to -n2, the rotation is around an arbitrary axis perpendicular to n1. 
			inline Rotation(const Vec3f& n1, const Vec3f& n2);

			// Normalize ensures the axis of rotation is a unit vector. 
			inline void Normalize();

			// Calculate the spherical linear interpolation between *this and the Rotation (r), 
			// where (f = 0) corresponds to *this and (f = 1) corresponds to r.
			Rotation Slerp(const Rotation& r, const mgFloat& f) const;

			// Cast to the Matrix3f rotation matrix equivalent to this Rotation.
			inline operator Matrix3f() const;

			// Cast to a quaternion. 
			inline operator Quaternion() const;

			// Cast to the Matrix4f rotation matrix equivalent to this Rotation. 
			inline operator Matrix4f() const;

			// Compose sets the first argument to static_cast< Matrix4f >( *this ) 
			// and the second to static_cast< Matrix4f >( *this * -1 ).
			// That is, it produces transform matrices for both the forward and inverse 
			// rotation, taking advantage of redundant computation. 
			inline void Compose(Matrix4f& forward, Matrix4f& inverse ) const;

		public:
			Vec3f	axis;
			mgFloat angle;
		};

		class HAPTX_SOLVER_SPEC Quaternion 
		{
		public:
			// Constructor
			inline Quaternion() {}
			// Constructor
			inline Quaternion(const Vec3f& _v, const mgFloat& _w) : v(_v), w(_w) {}
			// Constructor
			inline Quaternion(const mgFloat& _x, const mgFloat& _y, const mgFloat& _z, const mgFloat& _w) : v(_x,_y,_z), w(_w) {}
			
			inline void			Normalize();
			inline Quaternion	Cnj() const;
			inline Quaternion	Inv() const;
			inline mgFloat		Norm() const;
			inline mgFloat		Length() const;


			// Sqr returns *this^2 
			inline Quaternion Sqr() const;

			// Cast to Matrix3f, assuming the quaternion is normalized. 
			inline operator Matrix3f() const;

			// Cast to Matrix4f, assuming the quaternion is normalized. 
			inline operator Matrix4f() const;

			inline operator	Rotation() const;

			// Calculate the spherical linear interpolation between *this and the Quaternion (q), 
			// where (f = 0) corresponds to *this and (f = 1) corresponds to q. 
			Quaternion Slerp(const Quaternion& q, const mgFloat& f) const;

		public:
			mgFloat w;
			Vec3f	v;
		};


		// Slerper is a functional object for optimizing spherical linear interpolation 
		// between Rotations. The constructor requires two Rotations (p,q). The function 
		// call operator returns the spherical linear interpolation from p to q, 
		// where (f = 0) corresponds to p and (f = 1) corresponds to q. 
		class HAPTX_SOLVER_SPEC Slerper 
		{
		public:
			// Constructor
			inline Slerper() {}
			// Constructor
			Slerper(const Rotation& _p, const Rotation& _q);
			Rotation operator () (const mgFloat& f) const;

		protected:
			enum type_e
			{
				NORMAL		= 0, 
				OPPOSITE	= 1, 
				CLOSE		= 2
			};

			type_e		type;
			mgFloat		sinom;
			mgFloat		omega;
			Rotation	p;		// actually quaternions.
			Rotation	q;		// actually quaternions.
		};


		// Plane represents a 3D plane defined by the point p for which 
		// p * n = d where n is the normal of the plane and d is the offset scalar. 
		class HAPTX_SOLVER_SPEC Plane
		{
		public:
			// Constructor
			inline Plane() {}
			// Constructor
			inline Plane(const Vec3f& _n, const mgFloat& _d) : n(_n), d(_d) {}

			// Transform the plane according to the given coordinate space transform.
			void XformFrw(const Matrix4f& frw) throw(...);

			// Transform the plane according to the _inverse_ of the given coordinate space transform. 
			void XformInv(const Matrix4f& inv) throw();
			
			// Transform the plane according to the _inverse_ of the given coordinate space transform. 
			void XformInv(const double* inv) throw();

		public:
			Vec3f	n;
			mgFloat d;
		};


		// An SVDDecomposition is a Single-Value-Decomposition of a Matrix3f. 
		//! (see "Numerical Recipes in C", section 2.6). 
		struct HAPTX_SOLVER_SPEC SVDDecomposition 
		{
		public:
			Matrix3f	u;
			Vec3f		w;
			Matrix3f	v;
		};


		//! Vec2f operators  

		inline std::wostream& operator << (std::wostream& os, const Vec2f& v) 
		{
			os << v.x << " " << v.y;
			return os;
		} 

		inline bool operator == (const Vec2f& l, const Vec2f& r)
		{
			return l.x == r.x && l.y == r.y;
		}

		inline Vec2f operator + (const Vec2f& l, const Vec2f& r)
		{
			return Vec2f(l.x + r.x, l.y + r.y);
		}

		inline Vec2f operator * (const Vec2f& l, const mgFloat& r ) 
		{
			return Vec2f(l.x * r, l.y * r);
		}

		inline mgFloat operator * (const Vec2f& l, const Vec2f& r) 
		{
			return l.x * r.x + l.y * r.y;
		}

    
		//! Vec3f operators    

		inline std::wostream& operator << (std::wostream& os, const Vec3f& v)
		{
			os << v.x << " " << v.y << " " << v.z;
			return os;
		} 

		inline bool operator == (const Vec3f& l, const Vec3f& r)
		{
			return l.x == r.x && l.y == r.y && l.z == r.z;
		}

		inline Vec3f operator + (const Vec3f& l, const Vec3f& r)
		{
			return Vec3f(l.x + r.x, l.y + r.y, l.z + r.z);
		}

		inline Vec3f operator * (const Vec3f& l, const mgFloat& r)
		{
			return Vec3f(l.x * r, l.y * r, l.z * r);
		}

		inline mgFloat operator * (const Vec3f& l, const Vec3f& r)
		{
			return l.x*r.x + l.y*r.y + l.z*r.z;
		}

		inline Vec3f operator % (const Vec3f& l, const Vec3f& r)
		{
			return Vec3f(l.y*r.z - l.z*r.y, l.z*r.x - l.x*r.z, l.x*r.y - l.y*r.x);
		}

    
		//! Vec4f operators 

		inline std::wostream& operator << ( std::wostream& os, const Vec4f& v )
		{
			os << v.x << " " << v.y << " " << v.z << " " << v.w;
			return os;
		} 
		inline bool operator == (const Vec4f& l, const Vec4f& r)
		{
			return l.x == r.x && l.y == r.y && l.z == r.z && l.w == r.w;
		}

		inline Vec4f operator + (const Vec4f& l, const Vec4f& r)
		{
			return Vec4f(l.x + r.x, l.y + r.y, l.z + r.z, l.w + r.w);
		}

		inline Vec4f operator * (const Vec4f& l, const mgFloat& r)
		{
			return Vec4f(l.x * r, l.y * r, l.z * r, l.w * r);
		}

		inline mgFloat operator * (const Vec4f& l, const Vec4f& r)
		{
			return l.x*r.x + l.y*r.y + l.z*r.z + l.w*r.w;
		}

    
		//! Rotation operators 

		inline std::wostream& operator << (std::wostream& os, const Rotation& r) 
		{
			os << r.axis << " " << r.angle;
			return os;
		}

		inline bool operator == (const Rotation& l, const Rotation& r)
		{
			return l.axis == r.axis && l.angle == r.angle;
		}

		//! Scalar multiplication of a rotation scales the angle. 
		inline Rotation operator * (const Rotation& l, const mgFloat& r)
		{
			return Rotation(l.axis,l.angle * r);
		}

		// Multiplication is composition of rotations, (v * r1) * r2 == v * (r1 * r2)
		Rotation HAPTX_SOLVER_SPEC operator * (const Rotation& l, const Rotation& r);

		// The division a/b calculates the Rotation from b to a. That is, v * a == v * b * (a/b), 
		// (where v is a Vec3f and a,b are Rotations). 
		Rotation HAPTX_SOLVER_SPEC operator / (const Rotation& l, const Rotation& r);

		// Rotation of a vector.
		Vec3f HAPTX_SOLVER_SPEC operator * (const Vec3f& l ,const Rotation& r);

	
		//! Quaternion operators

		inline Quaternion operator + (const Quaternion& l, const Quaternion& r)
		{
			return Quaternion(l.v + r.v, l.w + r.w);
		}

		inline Quaternion operator * (const Quaternion& l, const Quaternion& r)
		{
			return Quaternion(l.v % r.v + r.v * l.w + l.v * r.w, l.w * r.w - l.v * r.v);
		}

		inline Quaternion operator * (const Quaternion& l, const mgFloat& r)
		{
			return Quaternion(l.v * r, l.w * r);
		}

		inline Quaternion operator / (const Quaternion& l, const mgFloat& r)
		{
			return Quaternion(l.v * (1.0 / r), l.w / r);
		}

		//! Rotation of a vector. 
		inline Vec3f operator * (const Vec3f& l, const Quaternion& r)
		{
			mgFloat t1 = r.w*r.w;
			mgFloat t3 = r.w*r.v.z;
			mgFloat t6 = r.w*r.v.y;
			mgFloat t9 = r.v.x*r.v.x;
			mgFloat t11 = r.v.x*r.v.y;
			mgFloat t14 = r.v.x*r.v.z;
			mgFloat t17 = r.v.y*r.v.y;
			mgFloat t19 = r.v.z*r.v.z;
			mgFloat t25 = r.w*r.v.x;
			mgFloat t32 = r.v.y*r.v.z;
			return Vec3f(l.x*t1-2*t3*l.y+2*t6*l.z+t9*l.x+2*t11*l.y+2*t14*l.z-t17*l.x-t19*l.x,
						 2*t3*l.x+l.y*t1-2*t25*l.z+2*t11*l.x-t9*l.y+t17*l.y+2*t32*l.z-t19*l.y,
						-2*t6*l.x+2*t25*l.y+l.z*t1+2*t14*l.x-t9*l.z+2*t32*l.y-t17*l.z+t19*l.z);
		}

	
		//! Matrix3f operators 

		inline Matrix3f operator * (const Matrix3f& l, const Matrix3f& r)
		{
			return Matrix3f( 
				l[0][0]*r[0][0] + l[0][1]*r[1][0] + l[0][2]*r[2][0],
				l[0][0]*r[0][1] + l[0][1]*r[1][1] + l[0][2]*r[2][1],
				l[0][0]*r[0][2] + l[0][1]*r[1][2] + l[0][2]*r[2][2],

				l[1][0]*r[0][0] + l[1][1]*r[1][0] + l[1][2]*r[2][0],
				l[1][0]*r[0][1] + l[1][1]*r[1][1] + l[1][2]*r[2][1],
				l[1][0]*r[0][2] + l[1][1]*r[1][2] + l[1][2]*r[2][2],

				l[2][0]*r[0][0] + l[2][1]*r[1][0] + l[2][2]*r[2][0],
				l[2][0]*r[0][1] + l[2][1]*r[1][1] + l[2][2]*r[2][1],
				l[2][0]*r[0][2] + l[2][1]*r[1][2] + l[2][2]*r[2][2]);
		}

		inline Vec3f operator * (const Matrix3f& l, const Vec3f& r)
		{
			return Vec3f(l[0][0]*r.x + l[0][1]*r.y + l[0][2]*r.z,
						 l[1][0]*r.x + l[1][1]*r.y + l[1][2]*r.z,
						 l[2][0]*r.x + l[2][1]*r.y + l[2][2]*r.z);
		}

		inline Matrix3f operator + (const Matrix3f& l, const Matrix3f& r)
		{
			return Matrix3f(l[0][0]+r[0][0], l[0][1]+r[0][1], l[0][2]+r[0][2],
							l[1][0]+r[1][0], l[1][1]+r[1][1], l[1][2]+r[1][2],
							l[2][0]+r[2][0], l[2][1]+r[2][1], l[2][2]+r[2][2]);
		}

		inline Matrix3f operator * (const Matrix3f& l, const mgFloat& r)
		{
			return Matrix3f(l[0][0]*r, l[0][1]*r, l[0][2]*r,
							l[1][0]*r, l[1][1]*r, l[1][2]*r,
							l[2][0]*r, l[2][1]*r, l[2][2]*r);
		}

		inline Vec3f operator * (const Vec3f& l, const Matrix3f& r)
		{
			return Vec3f(l.x*r[0][0] + l.y*r[1][0] + l.z*r[2][0],
						 l.x*r[0][1] + l.y*r[1][1] + l.z*r[2][1],
						 l.x*r[0][2] + l.y*r[1][2] + l.z*r[2][2]);
		}

	
		//! Matrix4f operators 

		inline Matrix4f operator * (const Matrix4f& l, const Matrix4f& r)
		{
			return Matrix4f( 
				l[0][0]*r[0][0] + l[0][1]*r[1][0] + l[0][2]*r[2][0] + l[0][3]*r[3][0],
				l[0][0]*r[0][1] + l[0][1]*r[1][1] + l[0][2]*r[2][1] + l[0][3]*r[3][1],
				l[0][0]*r[0][2] + l[0][1]*r[1][2] + l[0][2]*r[2][2] + l[0][3]*r[3][2],
				l[0][0]*r[0][3] + l[0][1]*r[1][3] + l[0][2]*r[2][3] + l[0][3]*r[3][3],

				l[1][0]*r[0][0] + l[1][1]*r[1][0] + l[1][2]*r[2][0] + l[1][3]*r[3][0],
				l[1][0]*r[0][1] + l[1][1]*r[1][1] + l[1][2]*r[2][1] + l[1][3]*r[3][1],
				l[1][0]*r[0][2] + l[1][1]*r[1][2] + l[1][2]*r[2][2] + l[1][3]*r[3][2],
				l[1][0]*r[0][3] + l[1][1]*r[1][3] + l[1][2]*r[2][3] + l[1][3]*r[3][3],

				l[2][0]*r[0][0] + l[2][1]*r[1][0] + l[2][2]*r[2][0] + l[2][3]*r[3][0],
				l[2][0]*r[0][1] + l[2][1]*r[1][1] + l[2][2]*r[2][1] + l[2][3]*r[3][1],
				l[2][0]*r[0][2] + l[2][1]*r[1][2] + l[2][2]*r[2][2] + l[2][3]*r[3][2],
				l[2][0]*r[0][3] + l[2][1]*r[1][3] + l[2][2]*r[2][3] + l[2][3]*r[3][3],

				l[3][0]*r[0][0] + l[3][1]*r[1][0] + l[3][2]*r[2][0] + l[3][3]*r[3][0],
				l[3][0]*r[0][1] + l[3][1]*r[1][1] + l[3][2]*r[2][1] + l[3][3]*r[3][1],
				l[3][0]*r[0][2] + l[3][1]*r[1][2] + l[3][2]*r[2][2] + l[3][3]*r[3][2],
				l[3][0]*r[0][3] + l[3][1]*r[1][3] + l[3][2]*r[2][3] + l[3][3]*r[3][3]);
		}

		inline Vec4f operator * (const Matrix4f& l, const Vec4f& r)
		{
			return Vec4f(l[0][0]*r.x + l[0][1]*r.y + l[0][2]*r.z + l[0][3]*r.w,
						 l[1][0]*r.x + l[1][1]*r.y + l[1][2]*r.z + l[1][3]*r.w,
						 l[2][0]*r.x + l[2][1]*r.y + l[2][2]*r.z + l[2][3]*r.w,
						 l[3][0]*r.x + l[3][1]*r.y + l[3][2]*r.z + l[3][3]*r.w);
		}

		inline Matrix4f operator + (const Matrix4f& l, const Matrix4f& r)
		{
			return Matrix4f( 
				l[0][0]+r[0][0], l[0][1]+r[0][1], l[0][2]+r[0][2], l[0][3]+r[0][3],
				l[1][0]+r[1][0], l[1][1]+r[1][1], l[1][2]+r[1][2], l[1][3]+r[1][3],
				l[2][0]+r[2][0], l[2][1]+r[2][1], l[2][2]+r[2][2], l[2][3]+r[2][3],
				l[3][0]+r[3][0], l[3][1]+r[3][1], l[3][2]+r[3][2], l[3][3]+r[3][3]);
		}

		inline Matrix4f operator * (const Matrix4f& l, const mgFloat& r)
		{
			return Matrix4f(l[0][0]*r, l[0][1]*r, l[0][2]*r, l[0][3]*r,
						    l[1][0]*r, l[1][1]*r, l[1][2]*r, l[1][3]*r,
						    l[2][0]*r, l[2][1]*r, l[2][2]*r, l[2][3]*r,
						    l[3][0]*r, l[3][1]*r, l[3][2]*r, l[3][3]*r);
		}

		inline Vec4f operator * (const Vec4f& l, const Matrix4f& r)
		{
			return Vec4f(l.x*r[0][0] + l.y*r[1][0] + l.z*r[2][0] + l.w*r[3][0],
						 l.x*r[0][1] + l.y*r[1][1] + l.z*r[2][1] + l.w*r[3][1],
						 l.x*r[0][2] + l.y*r[1][2] + l.z*r[2][2] + l.w*r[3][2],
						 l.x*r[0][3] + l.y*r[1][3] + l.z*r[2][3] + l.w*r[3][3]);
		}

		//! Multiplication with Vec3fs assumes that the fourth element
		//! of the vector is 1 (a point).
		inline Vec3f operator * (const Matrix4f& l, const Vec3f& r) 
		{
			return Vec3f(l[0][0]*r.x + l[0][1]*r.y + l[0][2]*r.z + l[0][3],
						 l[1][0]*r.x + l[1][1]*r.y + l[1][2]*r.z + l[1][3],
						 l[2][0]*r.x + l[2][1]*r.y + l[2][2]*r.z + l[2][3]) * (1.0 / (l[3][0]*r.x + l[3][1]*r.y + l[3][2]*r.z + l[3][3]));
		}

    
		//! Inductively defined operators over all Vector types.

		template<class T, class U>
		inline bool operator != (const T& l, const U& r)
		{
			return ! (l == r);
		}

		template<class T>
		inline T operator - (const T& r)
		{
			return r * -1;
		}

#ifndef _MSC_VER
		template< class T >
		inline T operator - (const T& l, const T& r)
		{
			return l + (-r);
		}
#else
		//! Visual C++ has a "bug" in it causing the generic operator- 
		//! defined above to interfere with an operator in the STL.
		inline Vec2f operator - (const Vec2f& l, const Vec2f& r)
		{
			return l + (-r);
		}

		inline Vec3f operator - (const Vec3f& l, const Vec3f& r)
		{
			return l + (-r);
		}

		inline Vec4f operator - (const Vec4f& l, const Vec4f& r)
		{
			return l + (-r);
		}

		inline Matrix3f operator - (const Matrix3f& l, const Matrix3f& r)
		{
			return l + (-r);
		}

		inline Matrix4f operator - (const Matrix4f& l, const Matrix4f& r)
		{
			return l + (-r);
		}

		inline Quaternion operator - (const Quaternion& l, const Quaternion& r)
		{
			return l + (-r);
		}
#endif
		template<class T>
		inline T operator * (const mgFloat& l, const T& r)
		{
			return r * l;
		}

		template<class T>
		inline T operator / (const T& l, const mgFloat& r)
		{
			return l * (1/r);
		}

		template<class T, class U>
		inline void operator += (T& l, const U& r)
		{
			l = l + r;
		}

		template<class T, class U>
		inline void operator -= (T& l, const U& r)
		{
			l = l - r;
		}

		template<class T, class U>
		inline void operator *= (T& l, const U& r)
		{
			l = l * r;
		}

		template<class T, class U>
		inline void operator /= (T& l, const U& r)
		{
			l = l / r;
		}

		template<class T, class U>
		inline void operator %= (T& l, const U& r)
		{
			l = l % r;
		}

    
		//! Vec2f inline member functions. 

		inline mgFloat Vec2f::Dot(const Vec2f& r) const
		{
			return (*this) * r;
		}

		inline mgFloat Vec2f::Length() const
		{
			return mgSqrt(LengthSqr());
		}

		inline mgFloat Vec2f::LengthSqr() const
		{
			return x*x + y*y;
		}

		inline mgFloat &Vec2f::operator [] (const int& i)
		{
			if (i == 0) return x;
			return y;
		}

		inline const mgFloat &Vec2f::operator [] (const int& i) const
		{
			if (i == 0) return x;
			return y;
		}

		inline void Vec2f::Normalize()
		{
			mgFloat l = x*x+y*y;
			if (mgAbs(l-1) > Util::epsilon)
			{
				l = mgSqrt(l);
				x /= l; y /= l;
			}
		}

		inline void Vec2f::NormalizeSafe()
		{
			mgFloat l = x*x+y*y;
			//! Util::epsilon is too large, so we use 1e-14 instead
			if (mgAbs(l-1) > 1e-14 && mgAbs(l) > 1e-14)
			{
				l = mgSqrt(l);
				x /= l; y /= l;
			}
		}

		inline void Vec2f::SetLength(const mgFloat& length)
		{
			mgFloat s = length / mgSqrt(x*x + y*y);
			x *= s;
			y *= s;
		}

		inline void Vec2f::SetLength(const Vec2f& v)
		{
			mgFloat s = mgSqrt((v.x*v.x + v.y*v.y)/(x*x + y*y) );
			x *= s;
			y *= s;
		}


		//! Vec3f inline member functions. 

		inline mgFloat Vec3f::Dot(const Vec3f& r) const
		{
			return (*this) * r;
		}

		inline Vec3f Vec3f::Cross(const Vec3f& r) const 
		{
			return (*this) % r;
		}

		inline mgFloat Vec3f::Length() const 
		{
			return mgSqrt(LengthSqr());
		}

		inline mgFloat Vec3f::LengthSqr() const 
		{
			return x*x + y*y + z*z;
		}

		inline mgFloat& Vec3f::operator [] (const int& i)
		{
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}

		inline const mgFloat& Vec3f::operator [] (const int& i) const 
		{
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}

		inline void Vec3f::Normalize()
		{
			mgFloat l = x*x + y*y + z*z;
			if (mgAbs(l - 1.0) > Util::epsilon)
			{
				l = mgSqrt(l);
				x /= l; y /= l; z /= l;
			}
		}

		inline void Vec3f::NormalizeSafe()
		{
			mgFloat l = x*x + y*y + z*z;
			if (mgAbs(l - 1.0) > 1e-14 && mgAbs(l) > 1e-14)
			{
				l = mgSqrt(l);
				x /= l; y /= l; z /= l;
			}
		}

		inline void Vec3f::SetLength(const mgFloat& length)
		{
			mgFloat s = length / mgSqrt(x*x + y*y + z*z);
			x *= s;
			y *= s;
			z *= s;
		}

		inline void Vec3f::SetLength(const Vec3f& v)
		{
			mgFloat s = mgSqrt(( v.x*v.x + v.y*v.y + v.z*v.z ) / ( x*x + y*y + z*z ));
			x *= s;
			y *= s;
			z *= s;
		}
		inline Vec3f Vec3f::Sph2Cart() const 
		{
			mgFloat cosphi = mgCos(y);
			mgFloat sintheta = mgSin(x);
			return Vec3f(-z * cosphi * sintheta, z * sintheta, -z * cosphi * mgCos(y));
		}

		inline Vec3f Vec3f::Cart2Sph() const 
		{
			mgFloat s2 = z*z + x*x;
			return Vec3f(mgAtan2(-x, -z), mgAtan2(y, mgSqrt( s2 )), mgSqrt(y*y + s2));
		}

    
		//! Vec4f inline member functions. 

		inline mgFloat& Vec4f::operator [] (const int& i)
		{
			if (i == 0) return x;
			if (i == 1) return y;
			if (i == 2) return z;
			return w;
		}

		inline const mgFloat& Vec4f::operator [] (const int& i) const
		{
			if (i == 0) return x;
			if (i == 1) return y;
			if (i == 2) return z;
			return w;
		}


		//! Rotation inline member functions.

		inline Rotation::Rotation(const Vec3f& n1, const Vec3f& n2)
		{
			bool use_perpendicular = false;
			mgFloat dot_product = n1 * n2;
			mgFloat s = 1.0;

			if (dot_product < -1.0 + Util::epsilon)
			{
				angle = Util::pi;
				use_perpendicular = true;
			} 
			else if (dot_product > 1.0 - Util::epsilon)
			{
				angle = 0;
				use_perpendicular = true;
			} 
			else 
			{
				angle = mgAcos(n1 * n2);
				s = mgSin( angle );
			}

			if (use_perpendicular || mgAbs(s) < Util::epsilon)
			{
				//! make an arbitrary axis perpendicular to n1.
					 if (mgAbs(n1.x) > 0.5) axis = Vec3f(-n1.z,0.0,n1.x);
				else if (mgAbs(n1.y) > 0.5) axis = Vec3f(n1.y,-n1.x,0.0);
				else						axis = Vec3f(0.0,n1.z,-n1.y);

				axis.Normalize();
			} 
			else
			{
				axis = (n1 % n2) / s;
			}
		}


		inline void Rotation::Normalize() 
		{
			axis.Normalize();
		}

		inline Rotation::operator Matrix3f() const
		{
			mgFloat t1 = axis.x * axis.x;
			mgFloat t2 = mgCos(angle);
			mgFloat t3 = 1.0-t2;
			mgFloat t7 = axis.x * axis.y * t3;
			mgFloat t8 = mgSin(angle);
			mgFloat t9 = axis.z * t8;
			mgFloat t12 = axis.x * axis.z * t3;
			mgFloat t13 = axis.y * t8;
			mgFloat t16 = axis.y * axis.y;
			mgFloat t20 = axis.y * axis.z * t3;
			mgFloat t21 = axis.x * t8;
			mgFloat t25 = axis.z * axis.z;
			return Matrix3f(t1*t3+t2, t7-t9,     t12+t13,
							t7+t9,    t16*t3+t2, t20-t21,
						    t12-t13,  t20+t21,   t25*t3+t2);
		}


		inline Rotation::operator Matrix4f() const 
		{
			mgFloat t1 = axis.x * axis.x;
			mgFloat t2 = mgCos(angle);
			mgFloat t3 = 1.0-t2;
			mgFloat t7 = axis.x * axis.y * t3;
			mgFloat t8 = mgSin(angle);
			mgFloat t9 = axis.z * t8;
			mgFloat t12 = axis.x * axis.z * t3;
			mgFloat t13 = axis.y * t8;
			mgFloat t16 = axis.y * axis.y;
			mgFloat t20 = axis.y * axis.z * t3;
			mgFloat t21 = axis.x * t8;
			mgFloat t25 = axis.z * axis.z;
			return Matrix4f(t1*t3+t2, t7-t9,     t12+t13,   0.0,
						    t7+t9,    t16*t3+t2, t20-t21,   0.0,
						    t12-t13,  t20+t21,   t25*t3+t2, 0.0,
						    0.0,      0.0,       0.0,       1.0);
		}


		inline void Rotation::Compose(Matrix4f& forward, Matrix4f& inverse) const 
		{
			mgFloat c = mgCos(angle);
			mgFloat s = mgSin(angle);
			mgFloat cc = 1.0 - c;
			Vec3f a = axis * s;

			forward = inverse = Matrix4f(axis.x*axis.x*cc+c,   axis.x*axis.y*cc-a.z, axis.x*axis.z*cc+a.y, 0.0,
										 axis.y*axis.x*cc+a.z, axis.y*axis.y*cc+c,   axis.y*axis.z*cc-a.x, 0.0,
										 axis.z*axis.x*cc-a.y, axis.z*axis.y*cc+a.x, axis.z*axis.z*cc+c,   0.0,
										 0.0,                  0.0,                  0.0,                  1.0);

			a *= 2.0;
			inverse.m[0][1] += a.z; inverse.m[0][2] -= a.y;
			inverse.m[1][0] -= a.z; inverse.m[1][2] += a.x;
			inverse.m[2][0] += a.y; inverse.m[2][1] -= a.x;
		}


		//! Cast to a quaternion. 
		inline Rotation::operator Quaternion() const
		{
			return Quaternion(axis * mgSin(angle / 2.0), mgCos(angle / 2.0));
		}


		//! Quaternion inline member functions.

		inline Quaternion Quaternion::Cnj() const
		{
			return Quaternion(-v,w);
		}

		inline Quaternion Quaternion::Inv() const
		{
			return Cnj() / Norm();
		}

		inline mgFloat Quaternion::Norm() const
		{
			return w * w + v * v;
		}

		inline mgFloat Quaternion::Length() const
		{
			return mgSqrt(Norm());
		}

		inline void Quaternion::Normalize()
		{
			mgFloat n = Norm();
			if (mgAbs(n - 1.0) > Util::epsilon)
			{
				mgFloat l = mgSqrt(n);
				w /= l;
				v /= l;
			}
		}


		inline Quaternion Quaternion::Sqr() const
		{
			return Quaternion(2.0 * w * v,w * w - v * v);
		}

		inline Quaternion::operator Rotation() const
		{
			mgFloat v2 = v * v;
			if (v2 < Util::epsilon) return Rotation(Vec3f(1.0,0.0,0.0),0.0);
		
			//! make sure that roundoff errors don't cause domain errors if e.g. w = 1.0000000001
			mgFloat angle;
				 if (w > 1.0)	angle = 0.0;
			else if (w < -1.0)	angle = Util::pi;
			else				angle = 2.0 * mgAcos(w);

			return Rotation(v / mgSqrt(v2),angle);
		}


		inline Quaternion::operator Matrix3f() const 
		{
			mgFloat t1 = w * w;
			mgFloat t2 = 2.0 * t1;
			mgFloat t3 = v.x * v.x;
			mgFloat t6 = v.x * v.y;
			mgFloat t7 = v.z * w;
			mgFloat t9 = v.x * v.z;
			mgFloat t10 = v.y * w;
			mgFloat t13 = v.y * v.y;
			mgFloat t16 = v.y * v.z;
			mgFloat t17 = v.x * w;
			mgFloat t21 = v.z * v.z;
			return Matrix3f(t2+2*t3-1,  2*t6-2*t7,   2*t9+2*t10,
						    2*t6+2*t7,  t2+2*t13-1,  2*t16-2*t17,
						    2*t9-2*t10, 2*t16+2*t17, t2+2*t21-1);
		}

		inline Quaternion::operator Matrix4f() const 
		{
			mgFloat t1 = w * w;
			mgFloat t2 = 2.0 * t1;
			mgFloat t3 = v.x * v.x;
			mgFloat t6 = v.x * v.y;
			mgFloat t7 = v.z * w;
			mgFloat t9 = v.x * v.z;
			mgFloat t10 = v.y * w;
			mgFloat t13 = v.y * v.y;
			mgFloat t16 = v.y * v.z;
			mgFloat t17 = v.x * w;
			mgFloat t21 = v.z * v.z;
			return Matrix4f(t2+2*t3-1,  2*t6-2*t7,   2*t9+2*t10,  0.0,
						    2*t6+2*t7,  t2+2*t13-1,  2*t16-2*t17, 0.0,
						    2*t9-2*t10, 2*t16+2*t17, t2+2*t21-1,  0.0,
						    0.0,        0.0,         0.0,         1.0);
		}


		//! Matrix3f inline member functions.

		inline Matrix3f::Matrix3f(const mgFloat& m00,const mgFloat& m01,const mgFloat& m02,
								  const mgFloat& m10,const mgFloat& m11,const mgFloat& m12,
								  const mgFloat& m20,const mgFloat& m21,const mgFloat& m22)
		{
			m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; 
			m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; 
			m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; 
		}

		
		inline void Matrix3f::SetToIdentity()
		{
			m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0;
			m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0;
			m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0;
		}

		inline void Matrix3f::SetRow(const int i, const Vec3f& v)
		{
			m[i][0] = v.x; m[i][1] = v.y; m[i][2] = v.z;
		}

		inline Vec3f Matrix3f::GetRow(const int i) const
		{
			return Vec3f(m[i][0], m[i][1], m[i][2]);
		}

		inline void Matrix3f::SetColumn(const int i, const Vec3f& v)
		{
			m[0][i] = v.x; m[1][i] = v.y; m[2][i] = v.z;
		}

		inline Vec3f Matrix3f::GetColumn(const int i) const
		{
			return Vec3f(m[0][i],m[1][i],m[2][i]);
		}


		//! Matrix4f inline member functions.

		inline Matrix4f::Matrix4f(const mgFloat& m00, const mgFloat& m01, const mgFloat& m02, const mgFloat& m03,
								  const mgFloat& m10, const mgFloat& m11, const mgFloat& m12, const mgFloat& m13,
								  const mgFloat& m20, const mgFloat& m21, const mgFloat& m22, const mgFloat& m23,
								  const mgFloat& m30, const mgFloat& m31, const mgFloat& m32, const mgFloat& m33 )
		{
			m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03; 
			m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13; 
			m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23; 
			m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33; 
		}
		
		inline void Matrix4f::SetToIdentity()
		{
			m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = 0.0;
			m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = 0.0;
			m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = 0.0;
			m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
		}


		inline Matrix3f Matrix4f::GetScaleRotation() const 
		{
			return Matrix3f(m[0][0],m[0][1],m[0][2],
						    m[1][0],m[1][1],m[1][2],
						    m[2][0],m[2][1],m[2][2]);
		}

		inline Matrix4f::operator Matrix3f() const
		{
			return GetScaleRotation();
		}


		inline Vec3f Matrix4f::ScaleRotate(const Vec3f& v) const
		{
			return Vec3f(m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
						 m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
						 m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z);
		}


		inline Vec3f Matrix4f::Rotate(const Vec3f& v) const 
		{
			mgFloat v2 = v * v;
			Vec3f w = ScaleRotate(v);
			mgFloat w2 = w * w;
			if (mgAbs(v2 - w2) > Util::epsilon) return w * mgSqrt(v2 / w2);

			return w;
		}
	}

	//! "using namespace Vectors" would include the operators, which is not desired.
	using Vectors::Vec2f;
	using Vectors::Vec3f;
	using Vectors::Vec4f;
	using Vectors::Rotation;
	using Vectors::Quaternion;
	using Vectors::Slerper;
	using Vectors::Matrix3f;
	using Vectors::Matrix4f;
	using Vectors::Plane;
	using Vectors::SVDDecomposition;
}

#endif
