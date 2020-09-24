
//! File: <Motion.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_MOTION_H_
#define _HAPTX_MOTION_H_

#include "LinearAlgebra.h"

namespace HaptX 
{
	// A Motion describes the difference between two coordinate spaces and is used to 
	// interpolate between them. The Matrix4f provided to the constructor defines the 
	// transformation from one coordinate space to another. A function f is provided 
	// that maps from a scalar in the range [0,1] to an interpolated matrix M', 
	// where f(0) = I and f(1) = M.
	//
	// Usually a Motion instance represents motion over one second. It is therefore possible 
	// to transform fractionally, to derive the transform for one millisecond (for example). 
	class HAPTX_SOLVER_SPEC Motion
	{
	public:
		Motion() : m_character(NONE) {}					// Constructor 
		Motion(const Matrix4f& _m);						// Constructor 

		inline Vec3f	Transform(const Vec3f& p, const mgFloat& f) const;		// Transforms the given point according to the interpolation factor f in the range [0,1]. [f=0] - p is returned unchanged. [f=1] - p is transformed according to the matrix given to the constructor. 
		inline Vec3f	TransformVec(const Vec3f& v, const mgFloat& f) const;	// Transforms vectors (i.e. it neglects to translate v).
		inline bool		IsNull() const;											// Returns true if there is no motion.
	
	public:
		Vec3f		m_translation;											// Matrix translation component.
		Rotation	m_rotation;												// Matrix rotation component.
		mgFloat		m_scale;												// Matrix scaling component.
		int			m_character;

		// A characterization of the motion.
		enum motionType_e
		{
		  NONE			= 0x0, 
		  ROTATION		= 0x1, 
		  TRANSLATION	= 0x2, 
		  SCALE			= 0x4
		};
	};

  
	//! inline member functions.

	//! Transforms the given point according to the interpolation factor f in the range [0,1].
	//! for f=0, p is returned unchanged.
	//! for f=1, p is transformed according to the matrix given to the constructor. 
	inline Vec3f Motion::Transform(const Vec3f& p, const mgFloat& f) const 
	{
		return (p * (m_rotation * f)) * (1.0 + f * (m_scale - 1.0)) + f * m_translation;
	}


	//! Transforms vectors (i.e. it neglects to translate v).
	inline Vec3f Motion::TransformVec(const Vec3f& v, const mgFloat& f) const
	{
		return (v * (m_rotation * f)) * (1.0 + f*(m_scale - 1.0));
	}

	//! Returns true if there is no motion. 
	inline bool Motion::IsNull() const
	{
		return m_translation*m_translation < Util::epsilon && mgAbs(m_rotation.angle) < Util::epsilon && mgAbs(m_scale-1) < Util::epsilon;
	}

	//! the scalar multiplication operator scales the components.   
	inline void operator *= (Motion& l, const mgFloat& s)
	{
		l.m_rotation *= s;
		l.m_translation *= s;
		l.m_scale = 1 + (1-l.m_scale) * s;
	}

	//! the scalar division operator divides the components.
	inline void operator /= (Motion& l, const mgFloat& s)
	{
		l *= 1.0 / s;
	}
}

#endif
