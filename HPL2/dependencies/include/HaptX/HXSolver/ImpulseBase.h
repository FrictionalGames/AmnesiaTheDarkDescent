
//! File: <ImpulseBase.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_IMPULSEBASE_H_
#define _HAPTX_IMPULSEBASE_H_

#include "HXSolver.h"
#include "LinearAlgebra.h"
#include "MgTime.h"

namespace HaptX
{	
	// ImpulseBase handles impulse forces applied onto geometries. The function AddImpulse is 
	// called from the HaptX loop if the Touch Controller is in contact with the geometry that 
	// is associated with the current ImpulseBase object.
	class HAPTX_SOLVER_SPEC ImpulseBase
	{	
	public:
		ImpulseBase();
		virtual void Reset();

		// AddImpulse is called by the HaptX loop to add the given impulse at the given point in 
		// global coordinates. The impulse is given as the force f applied for the given duration t.
		// Observe that it should only be called from the HaptX loop. 
		inline virtual void AddImpulse(const Vec3f& f, const Time& t, const Vec3f& p)
		{	
			Vec3f ft = f * t;
			m_impulse += ft;

			Vec3f arm = p - m_fulcrum;
			m_angularImpulse += arm % ft;
		}

	public:
		Vec3f m_fulcrum;
		Vec3f m_impulse;
		Vec3f m_angularImpulse;
		Vec3f m_position;
	};
}

#endif
