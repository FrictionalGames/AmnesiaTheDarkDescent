
//! File: <VariableSurface.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_VARIABLESURFACE_H_
#define _HAPTX_VARIABLESURFACE_H_

#include "FrictionalSurface.h"

namespace HaptX
{
	const mgFloat default_min_energy_drop = 0.0;

	// VariableSurface is a FrictionalSurface where the force in the direction of the normal
	// may vary over the surface according to some variable value. VariableSurface itself 
	// behaves as a FrictionalSurface. Sub-classes should override VariableSurface::Contact::Sample 
	// to create variable behaviour.
	class VariableSurface : public FrictionalSurface
	{
	protected:
		// See Surface::Contact
		class Contact : public FrictionalSurface::Contact
		{	
		public:
			inline			Contact(VariableSurface* s);	// Constructor
			inline			Contact();						// The void constructor is required in all sub-classes.
	  
			// Produces the rendering force and updates the proxy position. See Surface::Contact::Evaluate
			virtual void Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement);

			// Produces the rendering force and updates the proxy position. See Surface::Contact::Evaluate
			virtual void Evaluate3D(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& dr, const Vec3f& str_origin, Vec3f& force, Vec2f& proxy_movement);

			// Sample the surface at a given texture coordinate and penetration depth (in metres). The 
			// corresponding normal force magnitude and penetration energy must be returned. The penetration 
			// energy is the definite integral of the normal force magnitude from 0 to the given depth. 
			// See also Sample Sample3D
			inline virtual std::pair<mgFloat, mgFloat> Sample2D(const mgFloat& d, const Vec2f& p) const;

			// Sample the surface at a given texture coordinate and penetration depth (in metres). The 
			// corresponding normal force magnitude and penetration energy must be returned. The penetration 
			// energy is the definite integral of the normal force magnitude from 0 to the given depth. 
			// See also Sample Sample2D
			inline virtual std::pair<mgFloat, mgFloat> Sample3D(const mgFloat& d, const Vec3f& p) const;

			// Sample the surface at a given texture coordinate and penetration depth (in metres). The 
			// corresponding normal force magnitude and penetration energy must be returned. The penetration 
			// energy is the definite integral of the normal force magnitude from 0 to the given depth. 
			// See also Sample2D Sample3D
			inline virtual std::pair<mgFloat, mgFloat> Sample(const mgFloat& d, const Vec2f& p) const;

		protected:
			
			template<class TC>
			std::pair<mgFloat, mgFloat> SampleTmpl(const mgFloat& d, const TC& p) const;	// Determine which sample function to call depending on the dimension of the p argument

			template<class TC>
			void EvaluateTmpl(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec3f& str_origin, const TC& tc_origin, Vec3f& force, Vec2f& proxy_movement); // Helper function for the Evaluate functions.

			class Samp
			{
			public:
				Samp();
				Samp(const Vec2f& _p);
				
			public:
				std::pair<mgFloat,mgFloat>	sample;
				Vec2f						p;					// The uv coordinate.
				mgFloat						l2;
			};

			// Returns true if movement to the given position represents a drop in energy.
			// m_minEnergyDrop is the minimum allowed drop in energy (in joules).
			inline bool Jump(const mgFloat& d, const Samp& from, const Samp& to, const mgFloat& min_energy_drop) const; 

			template<class TC>
			void InitSamp(Samp& s, const mgFloat& depth) const;

			template<class TC>
			Vec2f Amoeba(const mgFloat& depth) const;			// The non-gradient function minimizer.
	  
			template<class TC>
			inline TC ToTexCoord(const Vec2f& p) const;
	  
			template<class TC>
			inline TC ToTexCoordVec(const Vec2f& v) const;

			inline mgFloat	ToS		(const Vec2f& p) const;
			inline mgFloat	ToSVec	(const Vec2f& p) const;
			inline Vec2f	ToST	(const Vec2f& p) const;		// ToST converts a point in UV coordinates to texture coordinates. Valid only during the call to Evaluate.
			inline Vec2f	ToSTVec	(const Vec2f& p) const;		// ToSTVec converts a vector in UV coordinates to texture coordinates. Valid only during the call to Evaluate.
			inline Vec3f	ToSTR	(const Vec2f& p) const;
			inline Vec3f	ToSTRVec(const Vec2f& p) const;
			inline Vec2f	ToUV	(const Vec2f& p) const;		// ToUV converts a point in texture coordinates to UV coordinates. Valid only during the call to Evaluate.
			inline Vec2f	ToUV	(const Vec3f& p) const;		// ToUV converts a point in texture coordinates to UV coordinates. Valid only during the call to Evaluate.
			inline Vec2f	ToUVVec	(const Vec2f& v) const;		// ToUVVec converts a vector in texture coordinates to UV coordinates. Valid only during the call to Evaluate.
			inline Vec2f	ToUVVec	(const Vec3f& v) const;		// ToUVVec converts a vector in texture coordinates to UV coordinates. Valid only during the call to Evaluate.

		protected:
			mgFloat			to_st[3][3];
			mgFloat			to_uv[2][4];
			Vec2f			m_fingerUV;
			mgFloat			m_minEnergyDrop;
	  
			Vec3f			m_stickPointSTR;					// The texture coordinate of the proxy during static contact.
			Vec3f			m_strProxyMovement;					// The st coordinates of our last reported proxy movement when in dynamic contact.
			Vec3f			m_oldFingerSTR;
		};

		inline virtual Surface::Contact* NewContact();			// New contact. 
		inline virtual Surface::Contact* CopyContact();			// Copy contact. 

	public:
		VariableSurface();										// Constructor
		VariableSurface(FrictionalSurfaceInfo* si);				// Destructor
	};


	//! Inline member functions

	VariableSurface::Contact::Contact(VariableSurface* s) : FrictionalSurface::Contact(s), m_minEnergyDrop(default_min_energy_drop), m_oldFingerSTR(0.0,0.0,0.0)
	{
	}

	inline bool VariableSurface::Contact::Jump(const mgFloat& d, const Samp& from, const Samp& to, const mgFloat& min_energy_drop) const
	{
		//! the change in spring energy.
		//! spring energy is half the stiffness_t times the length squared
		//! (the integral of Hooke's law over distance).
		//! The normal-direction spring energy change is explicitly given
		//! by the sample() function.
		
		mgFloat Es = 0.5 * m_deviceMaxStiffness * m_stiffnessT * (to.l2 - from.l2) + to.sample.second - from.sample.second;
		if (Es > -min_energy_drop) return false;

		Es += min_energy_drop;
		Es *= Es;

		//! the friction energy squared.
		//! the energy lost to friction is the average friction force
		//! (here simply the average of the 'from' force and the 'to' force) 
		//! times the distance covered in metres (hence in UV space).
		Vec2f j = to.p - from.p;
		mgFloat Ef = m_dynamicFriction * 0.5 * (from.sample.first + to.sample.first);
		Ef = (j * j) * Ef * Ef;
		
		// jump only if we lose more energy in the springs than we
		// burn in friction.
		return Ef < Es;
	}

	inline VariableSurface::VariableSurface() : FrictionalSurface(new FrictionalSurfaceInfo())
	{
		m_supportsTex3D = true;
		m_supportsTex2D = true;
		m_supportsTex1D = false;
		m_supportsTex0D = true;
	}

	inline VariableSurface::VariableSurface(FrictionalSurfaceInfo* si) : FrictionalSurface(si)
	{
		m_supportsTex3D = true;
		m_supportsTex2D = true;
		m_supportsTex1D = false;
		m_supportsTex0D = true;
	}

	inline VariableSurface::Contact::Contact()
	{
	}

	inline std::pair<mgFloat, mgFloat> VariableSurface::Contact::Sample2D(const mgFloat& d, const Vec2f& p) const
	{
		return (Sample(d,p));
	}

	inline std::pair<mgFloat, mgFloat> VariableSurface::Contact::Sample3D(const mgFloat& d, const Vec3f& p) const
	{
		mgFloat f = m_stiffness * m_deviceMaxStiffness *  d;
		return std::make_pair(f,f * d * 0.5);
	}

	inline std::pair<mgFloat, mgFloat> VariableSurface::Contact::Sample(const mgFloat& d, const Vec2f& p) const
	{
		mgFloat f = m_stiffness * m_deviceMaxStiffness*  d;
		return std::make_pair(f,f * d * 0.5);
	}      

	inline mgFloat VariableSurface::Contact::ToS(const Vec2f& p) const
	{
		return (to_st[0][0] * p.u + to_st[0][1] * p.v + to_st[0][2]);
	}

	inline mgFloat VariableSurface::Contact::ToSVec(const Vec2f& v) const
	{
		return (to_st[0][0] * v.u + to_st[0][1] * v.v);
	}		

	//! ToST converts a point in UV coordinates to texture coordinates.
	//! Valid only during the call to Evaluate.
	inline Vec2f VariableSurface::Contact::ToST(const Vec2f& p) const
	{
		return Vec2f(to_st[0][0] * p.u + to_st[0][1] * p.v + to_st[0][2],
					 to_st[1][0] * p.u + to_st[1][1] * p.v + to_st[1][2]);
	}
	  
	//! ToSTVec converts a vector in UV coordinates to texture coordinates.
	//! Valid only during the call to Evaluate.
	inline Vec2f VariableSurface::Contact::ToSTVec(const Vec2f& v) const
	{
		return Vec2f(to_st[0][0] * v.u + to_st[0][1] * v.v,
					 to_st[1][0] * v.u + to_st[1][1] * v.v);
	}

	inline Vec3f VariableSurface::Contact::ToSTR(const Vec2f& p) const
	{
		return Vec3f(to_st[0][0] * p.u + to_st[0][1] * p.v + to_st[0][2],
					 to_st[1][0] * p.u + to_st[1][1] * p.v + to_st[1][2],
					 to_st[2][0] * p.u + to_st[2][1] * p.v + to_st[2][2]);
	}

	inline Vec3f VariableSurface::Contact::ToSTRVec(const Vec2f& v) const
	{
		return Vec3f(to_st[0][0] * v.u + to_st[0][1] * v.v,
					 to_st[1][0] * v.u + to_st[1][1] * v.v,
					 to_st[2][0] * v.u + to_st[2][1] * v.v);
	}

	//! ToUV converts a point in texture coordinates to UV coordinates.
	//! Valid only during the call to Evaluate.
	inline Vec2f VariableSurface::Contact::ToUV(const Vec2f& p) const
	{
		return Vec2f(to_uv[0][0] * p.s + to_uv[0][1] * p.t + to_uv[0][3],
					 to_uv[1][0] * p.s + to_uv[1][1] * p.t + to_uv[1][3]);
	}

	//! ToUV converts a point in texture coordinates to UV coordinates.
	//! Valid only during the call to Evaluate.
	inline Vec2f VariableSurface::Contact::ToUV(const Vec3f& p) const
	{
		return Vec2f(to_uv[0][0] * p.s + to_uv[0][1] * p.t + to_uv[0][2] * p.r + to_uv[0][3],
					 to_uv[1][0] * p.s + to_uv[1][1] * p.t + to_uv[1][2] * p.r + to_uv[1][3]);
	}
	  
	//! ToUVVec converts a vector in texture coordinates to UV coordinates.
	//! Valid only during the call to Evaluate.
	inline Vec2f VariableSurface::Contact::ToUVVec(const Vec2f& v) const
	{
		return Vec2f(to_uv[0][0] * v.s + to_uv[0][1] * v.t,
					 to_uv[1][0] * v.s + to_uv[1][1] * v.t);
	}
	  
	//! ToUVVec converts a vector in texture coordinates to UV coordinates.
	//! Valid only during the call to Evaluate.
	inline Vec2f VariableSurface::Contact::ToUVVec(const Vec3f& v) const
	{
		return Vec2f(to_uv[0][0] * v.s + to_uv[0][1] * v.t + to_uv[0][2] * v.r,
					 to_uv[1][0] * v.s + to_uv[1][1] * v.t + to_uv[1][2] * v.r);
	}

	inline VariableSurface::Contact::Samp::Samp()
	{
	}

	inline VariableSurface::Contact::Samp::Samp(const Vec2f& _p) : p(_p)
	{
	}

	template< class TC >
	inline TC VariableSurface::Contact::ToTexCoord(const Vec2f& p) const
	{
		return TC(to_st[0][0] * p.u + to_st[0][1] * p.v + to_st[0][2],
				  to_st[1][0] * p.u + to_st[1][1] * p.v + to_st[1][2]);
	}

	template< class TC >
	inline TC VariableSurface::Contact::ToTexCoordVec(const Vec2f& v) const
	{
		return TC(to_st[0][0] * v.u + to_st[0][1] * v.v,
				  to_st[1][0] * v.u + to_st[1][1] * v.v);
	}

	inline Surface::Contact* VariableSurface::NewContact()
	{
		return new Contact(this);
	}

	inline Surface::Contact* VariableSurface::CopyContact()
	{
		return CopyContactT(new Contact);
	}
}

#endif
