
//! File: <ForceModel.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_FORCEMODEL_H_
#define _HAPTX_FORCEMODEL_H_

#include "LinearAlgebra.h"
#include "RefCounter.h"
#include "HaptXDefines.h"
#include "Extrapolator.h"

#pragma warning(disable: 4251)	//! Disable warning bescause STL templates are exported implicitly.

namespace HaptX
{
	//! Using namespace Vectors to get the inductively defined 
	namespace Vectors
	{
		class ForceTorque
		{
		public:
			inline	ForceTorque()								: force(0.0,0.0,0.0), torque(0.0,0.0,0.0) {}
			inline	ForceTorque(const Vec3f& f, const Vec3f& t) : force(f), torque(t) {}

		public:
			Vec3f	force;
			Vec3f	torque;
		};

		inline bool operator == (const ForceTorque& l, const ForceTorque& r)
		{
			return l.force == r.force && l.torque == r.torque;
		}

		inline ForceTorque operator + (const ForceTorque& l, const ForceTorque& r)
		{
			return ForceTorque(l.force + r.force, l.torque + r.torque);
		}

		inline ForceTorque operator * (const ForceTorque& l, const mgFloat& r)
		{
			return ForceTorque(l.force * r, l.torque * r);
		}
	}

	using Vectors::ForceTorque;

	// The base class for all ForceModels.
	// HaptX rendering can be viewed as the implementation of a function mapping 
	// from the Touch Controller's position to a rendered force vector.
	// A ForceModel is a generalization of such a mapping. The resulting force 
	// vectors from successive HaptX loops are interpolated and combined to achieve
	// smooth transitions from one loop to the next.
	// 
	// Two arguments are supplied to the Evaluate() function in 3DOF ForceModels. 
	// The first is the Vec3f representing the unconstrained device position (with or 
	// without camera transformation). The second argument is a weighting in 
	// the range [0,1] indicating the factor by which the force vector will be 
	// scaled before being sent to the Touch Controller. Forces are weighted when 
	// interpolating between successive ForceModels. The force model should NOT scale 
	// its resulting force according to this weighting. 
	// 
	// Three arguments are supplied to the Evaluate6DOF() function in 6DOF ForceModels. 
	// The first and last arguments are the same as in Evaluate(), and the middle one 
	// is the rotation of the device.
	//
	// When creating new ForceModels either ForceModel3DOF or ForceModel6DOF should be
	// inherited.
	class HAPTX_SOLVER_SPEC ForceModel : public RefCounter
	{
	public:
		inline ForceModel(const bool& _six_dof, int DeviceID);	// Constructor

		inline virtual Vec3f		Evaluate(const Vec3f& p, const mgFloat& w, const mgFloat& delta_t) = 0;  // Evaluate function is called from the Haptx loop and used by 3DOF ForceModels.
		inline virtual ForceTorque	Evaluate6DOF(const Vec3f& p, const Quaternion& r, const mgFloat& w, const mgFloat& delta_t) = 0; // Evaluate6DOF function is called from the Haptx loop and used by 6DOF ForceModels.
		inline virtual void			UpdateMembers() {}														// UpdateMembers is called once per render loop (from the main thread). Sub-classes may update changes done in the main thread into the variables of the ForceModel instance.
		inline virtual void			SetDeviceID(int DeviceID){m_deviceID = DeviceID;}
		inline		   int			GetDeviceID(){return m_deviceID;}
	public:
		bool	m_sixDof;				// False for 3DOF ForceModels, true for 6DOF ForceModels.
		int		m_coordinateSystem;		// The coordinate system type, See also CollisionState::AddAbsoluteGlobalFM and CollisionState::AddAbsoluteLocalFM.
		mgFloat	m_deviceMaxStiffness;	// The maximum stiffness of the device. 
		mgFloat	m_deviceMaxForce;		// The maximum force of the device. 
		mgFloat	m_forceModelScale;		// Scale value for the ForceModel;
		
		bool	m_local;
		LinearExtrapolator<Matrix4f> m_invExtrapolator;
		LinearExtrapolator<Matrix4f> m_frwExtrapolator;
		Matrix4f m_inv;
		Matrix4f m_frw;
	
	protected:
		int		m_deviceID;				// The id of the device that this ForceModel is created for.
		


	};

	// The base class for 3DOF ForceModels.
	class HAPTX_SOLVER_SPEC ForceModel3DOF : public ForceModel
	{
	public:
		inline ForceModel3DOF(int DeviceID);
		inline virtual ForceTorque Evaluate6DOF(const Vec3f& p, const Quaternion& r, const mgFloat& w, const mgFloat& delta_t);
	};
  
	// The base class for 6DOF ForceModels.
	class HAPTX_SOLVER_SPEC ForceModel6DOF : public ForceModel
	{
	public:
		inline ForceModel6DOF(int DeviceID);
		inline virtual Vec3f Evaluate(const Vec3f& p, const mgFloat& w, const mgFloat& delta_t);
	};

	// <NO_DOC>
	// ForceModelSum is a collection of ForceModels and is itself a 6DOF ForceModel.
	// The ForceModels in the vector are summed.
	class HAPTX_SOLVER_SPEC ForceModelSum : public std::vector<ForceModel*>, public ForceModel6DOF
	{
	public:
		inline ForceModelSum(int DeviceID): ForceModel6DOF(DeviceID) {}
		
		inline void push_back(ForceModel* fm); 

		// Evaluate6DOF() sums the ForceModels to create the final effect.
		inline virtual ForceTorque Evaluate6DOF(const Vec3f& p, const Quaternion& r, const mgFloat& w, const mgFloat& delta_t);
	};
	// </NO_DOC>

    //! Inline member methods
    using Vectors::ForceTorque;

    //! constructor
    inline ForceModel::ForceModel(const bool& _six_dof, int DeviceID) : m_sixDof(_six_dof), m_coordinateSystem(HX_ABSOLUTE_GLOBAL), m_deviceMaxStiffness(0.0), m_deviceMaxForce(0.0), m_deviceID(-1), m_forceModelScale(1.0)
    {
		m_deviceID = DeviceID;
		m_local = false;
		m_invExtrapolator.Init(m_inv, m_inv);
		m_frwExtrapolator.Init(m_frw, m_frw);
    }

    //! Constructor
    inline ForceModel3DOF::ForceModel3DOF(int DeviceID) : ForceModel(false,DeviceID)
    {
    }

    //! ForceModel3DOF::Evaluate6DOF
    inline ForceTorque ForceModel3DOF::Evaluate6DOF(const Vec3f& p, const Quaternion& r, const mgFloat& w, const mgFloat& delta_t)
    {
	    return ForceTorque(Vec3f(0.0,0.0,0.0),Vec3f(0.0,0.0,0.0));
    }

    //! ForceModel6DOF::ForceModel6DOF
    inline ForceModel6DOF::ForceModel6DOF(int DeviceID) : ForceModel(true,DeviceID)
    {
    }

    //! ForceModel6DOF::Evaluate
    inline Vec3f ForceModel6DOF::Evaluate(const Vec3f& p, const mgFloat& w, const mgFloat& delta_t)
    {
	    return Vec3f(0.0,0.0,0.0);
    }

	// < NO_DOC>
    //! ForceModelSum::push_back
    inline void ForceModelSum::push_back(ForceModel* fm)
    {
		// The DeviceID of the parent Force Model is initiated to -1 in the constructor. 
		// Therefore we set it to the deviceID of the forcemodel that is added to the list.
		SetDeviceID(fm->GetDeviceID()); 
		std::vector<ForceModel*>::push_back(fm);
    }

    //! ForceModel6DOF::Evaluate
    inline ForceTorque ForceModelSum::Evaluate6DOF(const Vec3f& p, const Quaternion& r, const mgFloat& w, const mgFloat& delta_t)
    {
	    ForceTorque ft(Vec3f(0.0,0.0,0.0), Vec3f(0.0,0.0,0.0));

	    for (iterator i = begin(); i != end(); ++i)
	    {
		    if ((*i)->m_sixDof)
		    {
			    ForceTorque fti = (*i)->Evaluate6DOF(p, r, w, delta_t);
			    ft.force += fti.force;
			    ft.torque += fti.torque;
		    }
		    else
		    {
				if ((*i)->m_local)
				{
					(*i)->m_invExtrapolator.UpdateExtrapolation();
					(*i)->m_frwExtrapolator.UpdateExtrapolation();
					(*i)->m_inv +=  (*i)->m_invExtrapolator.GetExtrapolation();
					(*i)->m_frw +=  (*i)->m_frwExtrapolator.GetExtrapolation();
					
					Vec3f origin = Vec3f((*i)->m_inv[0][3], (*i)->m_inv[1][3], (*i)->m_inv[2][3]);
					Vec3f pos = (*i)->m_frw.Rotate( (*i)->m_inv.Rotate(p) + origin);
					ft.force += (*i)->m_frw.Rotate((*i)->Evaluate((*i)->m_inv.Rotate(p) + origin, w, delta_t));
				}
				else
					ft.force += (*i)->Evaluate(p, w, delta_t);
			
		    }
	    }

	    return ft;
    }
   	// </NO_DOC>
}

#endif
