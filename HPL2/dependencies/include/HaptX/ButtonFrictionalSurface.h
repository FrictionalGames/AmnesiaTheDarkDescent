
//! File: <ButtonFrictionalSurface.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_BUTTONSURFACE_H_
#define _HAPTX_BUTTONSURFACE_H_

#include <map>

#include "FrictionalSurface.h"
#include "HXSolver/mgTime.h"

namespace HaptX 
{
	// ButtonFrictionalSurface modifies the FrictionalSurface to behave like a button.
	// The surface becomes "armed" when pressed. See also ButtonFrictionalSurfaceInfo.
	class ButtonFrictionalSurface : public FrictionalSurface 
	{
	protected:
		// See Surface::Contact
		class Contact : public FrictionalSurface::Contact 
		{
		public:
			inline							Contact(ButtonFrictionalSurface* s);	// Constructor
			inline							Contact() {}							// Constructor
		  
			inline virtual void				Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin,	Vec3f& force, Vec2f& proxy_movement);// The force rendering function. See Surface::Contact::Evaluate
			inline virtual void				Evaluate0D(const Vec3f& finger, Vec3f& force, Vec2f& proxy_movement);// The force rendering function. See Surface::Contact::Evaluate

		protected:
			bool*							r_armed;								// Holds a reference to the SurfaceInfo reference. The value of the reference is set by the Contact object.
			mgFloat							r_click_force;							// Holds a copy of the SurfaceInfo value.
		};
		friend class Contact;

	public:
											ButtonFrictionalSurface();				// Constructor

		ButtonFrictionalSurfaceInfo*		GetSurfaceInfo();						// Returns the SurfaceInfo which holds the parameters for the surface.
			
		inline virtual Surface::Contact*	NewContact();							// New contact. 
		inline virtual Surface::Contact*	CopyContact();							// Copy contact. 

	protected:
		virtual void						UpdateFields();							// UpdateFields is called once per main loop. See also Surface::UpdateFields()

	public:
		Time								m_activateTime;							// The time when the button was released.
		bool								m_armed;								// Indicates if the button surface is pressed.

	protected:
		bool								rt_armed;								// The HaptX loop state of the surface armed member.
	};


	inline ButtonFrictionalSurface::Contact::Contact(ButtonFrictionalSurface* s) : FrictionalSurface::Contact(s), r_armed(&(s->rt_armed)), r_click_force(s->GetSurfaceInfo()->GetClickForce()) 
	{
	}

	//! The force rendering function.
	inline void ButtonFrictionalSurface::Contact::Evaluate0D(const Vec3f& finger, Vec3f& force, Vec2f& proxy_movement) 
	{
		FrictionalSurface::Contact::Evaluate0D(finger,force,proxy_movement);

		mgFloat clickForce =  r_click_force * m_deviceMaxForce;

		if (*r_armed) 
		{
			if (force.z < clickForce * 0.6666) *r_armed = false;
		}
		else if (force.z > clickForce)
		{
			*r_armed = true;
		}

		if (*r_armed) force.z -= clickForce * 0.1666;
	}

	//! The force rendering function.
	inline void ButtonFrictionalSurface::Contact::Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement) 
	{
		FrictionalSurface::Contact::Evaluate(finger,ds,dt,st_origin,force,proxy_movement);

		mgFloat clickForce = r_click_force * m_deviceMaxForce;

		if (*r_armed) 
		{
			if (force.z < clickForce * 0.6666) *r_armed = false;
		}
		else if (force.z > clickForce)
		{
			*r_armed = true;
		}

		if (*r_armed) force.z -= clickForce * 0.1666;
	}

	inline Surface::Contact* ButtonFrictionalSurface::NewContact()
	{
		return new Contact(this);
	}

	inline Surface::Contact* ButtonFrictionalSurface::CopyContact() 
	{
		return CopyContactT(new Contact);
	}
}

#endif
