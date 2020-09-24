
//! File: <ForceModelLibrary.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_FORCEMODELLIBRARY_H_
#define _HAPTX_FORCEMODELLIBRARY_H_

#include "ForceModelInfo.h"
#include "HXSolver/ForceModel.h"
#include "HXSolver/mgTime.h"

namespace HaptX
{
	// The base class from which all animated ForceModels are inherited. It contains parameters 
	// for applying time control to animate ForceModels, see also ForceModelInfo
	class ForceModelAnimated : public ForceModel3DOF
	{
	public:
						ForceModelAnimated(ForceModelInfo* fmI, int DeviceID);	// constructor
		virtual			~ForceModelAnimated();									// destructor

		virtual Vec3f	Evaluate(const Vec3f& pos, const mgFloat& weight, const mgFloat& delta_t); // The force rendering function
		virtual void	UpdateMembers();										// Updates the members of the ForceModel

		ForceModelInfo*	GetForceModelInfo();									// Returns the ForceModelInfo which holds the parameters for the ForceModel
		virtual void	SetDeviceID(int DeviceID){ForceModel::SetDeviceID(DeviceID); m_forceModelInfo->SetDeviceID(DeviceID); }
		virtual void	ValidateParameters(mgFloat WorldScale){}				//Provides a place to put any validation of input parameters just before the ForceModel is started.
	public:
		ForceModelInfo* m_forceModelInfo;										// a pointer to the ForceModelInfo.
		bool			m_justStarted;
	};

	// Creates an infinite long force in a given direction that can be animated to impulses 
	// by applying time control. By setting oneLoopDuration and loopIdleTime to a given
	// number of seconds in forceModelSetTimeControl the ForceModel will give impulses.
	// See also ForceModelImpulseInfo
	class ForceModelImpulse : public ForceModelAnimated
	{
	public:
								ForceModelImpulse(Vec3f f, int DeviceID);							  // Constructor
								ForceModelImpulse(ForceModelImpulseInfo* fmi, Vec3f f, int DeviceID); // Constructor

		virtual Vec3f			Evaluate(const Vec3f& pos, const mgFloat& weight, const mgFloat& delta_t);
		virtual void			UpdateMembers();
		ForceModelImpulseInfo*	GetForceModelInfo();
	};

 	// An infinite long sinus wave force in a given direction that can be animated to impulses 
	// by applying time control. See also ForceModelSineWaveInfo 
	class ForceModelSineWave : public ForceModelAnimated
	{
	public:
								ForceModelSineWave(Vec3f d, float a, float f, int DeviceID);								// Constructor
								ForceModelSineWave(ForceModelSineWaveInfo* fmi, Vec3f d, float a, float f, int DeviceID);	// Constructor

		virtual Vec3f			Evaluate(const Vec3f& pos, const mgFloat& weight, const mgFloat& delta_t);
		virtual void			UpdateMembers();
		ForceModelSineWaveInfo* GetForceModelInfo();
	};

	// A SawWave ForceModel, see also ForceModelSineWaveInfo
	class ForceModelSawWave : public ForceModelSineWave
	{
	public:
								ForceModelSawWave(const Vec3f& d, float& a, float& f, int DeviceID); // Constructor
								ForceModelSawWave(ForceModelSawWaveInfo* fmi, const Vec3f& d, float& a, float& f, int DeviceID); // Constructor

		virtual Vec3f			Evaluate(const Vec3f& pos, const mgFloat& weight, const mgFloat& delta_t);
		virtual void			UpdateMembers();
		ForceModelSawWaveInfo*	GetForceModelInfo();
	};

	// A SawWave ForceModel, , see also ForceModelSineWaveInfo
	class ForceModelSquareWave : public ForceModelSineWave
	{
	public:
									ForceModelSquareWave(const Vec3f& d, float& a, float& f, int DeviceID); // Constructor
									ForceModelSquareWave(ForceModelSquareWaveInfo* fmi, const Vec3f& d, float& a, float& f, int DeviceID); // Constructor
		
		virtual Vec3f				Evaluate(const Vec3f& pos, const mgFloat& weight, const mgFloat& delta_t);
		virtual void				UpdateMembers();
		ForceModelSquareWaveInfo*	GetForceModelInfo();
	};

	// Explosion is a global ForceModel and is a combination of a sinus vibration and a repelling magnet.
	// It can be visualized as a sphere with the maximum force and vibration output at the origin with 
	// linearly decreasing force and vibration towards the effectDistance. See also ForceModelExplosionInfo
	class ForceModelExplosion : public ForceModelSineWave
	{
	public:
									ForceModelExplosion( float& a, float& f, const Vec3f& o, float& e, int DeviceID); // Constructor
									ForceModelExplosion(ForceModelExplosionInfo* fmi, float& a, float& f, const Vec3f& o, float& e, int DeviceID); // Constructor

		virtual Vec3f				Evaluate(const Vec3f& pos, const mgFloat& weight, const mgFloat& delta_t);
		virtual void				UpdateMembers();
		ForceModelExplosionInfo*	GetForceModelInfo();

	private:
		Vec3f						m_distance;	// Holds a copy of the ForceModelExplosionInfo value.
	};

	// A ForceModel simulating a magnetic force. The force is weakest at the effectDistance and origin, and 
	// increases linearly to the origoSnapDistance. See also ForceModelMagnetInfo
	class ForceModelMagnet : public ForceModelAnimated
	{
	public:
								ForceModelMagnet(Vec3f& o, float& m, float& d, float& e, int DeviceID); // Constructor
								ForceModelMagnet(ForceModelMagnetInfo* fmi, Vec3f& o, float& m, float& d, float& e, int DeviceID); // Constructor

		virtual Vec3f			Evaluate(const Vec3f& pos, const mgFloat& weight, const mgFloat& delta_t);
		virtual void			UpdateMembers();
		ForceModelMagnetInfo*	GetForceModelInfo();
		virtual void			ValidateParameters(mgFloat WorldScale);
	};

	// A ForceModel simulating a radial spring which pulls the device with a linear force towards the 
	// origo, see also ForceModelSpringInfo
	class ForceModelSpring : public ForceModelAnimated
	{
	public:
								ForceModelSpring(Vec3f& o, float& s, float& d, int DeviceID); // Constructor
								ForceModelSpring(ForceModelSpringInfo* fmi, Vec3f& o, float& s, float& d, int DeviceID); // Constructor

		virtual Vec3f			Evaluate(const Vec3f& pos, const mgFloat& weight, const mgFloat& delta_t);
		virtual void			UpdateMembers();
		ForceModelSpringInfo*	GetForceModelInfo();

	protected:
		Vec3f					m_lastPos;		// Used internally.
	};

	// A ForceModel simulating a radial repelling magnetic force which pushes the device from the origo. 
	// The force output is zero at the effectDistance, see also ForceModelRepellingMagnetInfo
	class ForceModelRepellingMagnet : public ForceModelMagnet
	{
	public:
										ForceModelRepellingMagnet(Vec3f& o, float& s, float& d, float& e, int DeviceID); // Constructor
										ForceModelRepellingMagnet(ForceModelRepellingMagnetInfo* fmi, Vec3f& o, float& s, float& d, float& e, int DeviceID); // Constructor

		virtual Vec3f					Evaluate(const Vec3f& pos, const mgFloat& weight, const mgFloat& delta_t);
		virtual void					UpdateMembers();
		ForceModelRepellingMagnetInfo*	GetForceModelInfo();
	};

	// The ForceModel simulates that a mass is connected to the Touch Controller through a spring, 
	// see also ForceModelViscosityInfo
	class ForceModelViscosity : public ForceModelAnimated
	{
	public:
									ForceModelViscosity(Vec3f& v, float& m, float& s, float& d, int DeviceID); // Constructor
									ForceModelViscosity(ForceModelViscosityInfo* fmi, Vec3f& v, float& m, float& s, float& d, int DeviceID); // Constructor

		virtual Vec3f				Evaluate(const Vec3f& pos, const mgFloat& weight, const mgFloat& delta_t);
		virtual void				UpdateMembers();
		ForceModelViscosityInfo*	GetForceModelInfo();

	protected:
		Time						m_lastTime;			// Holds a copy of the ForceModelViscosityInfo value.
		Vec3f						m_springForce;		// Holds a copy of the ForceModelViscosityInfo value.
		Vec3f						m_dampingForce;		// Holds a copy of the ForceModelViscosityInfo value.
		Vec3f						m_momentumForce;	// Holds a copy of the ForceModelViscosityInfo value.
		Vec3f						m_acceleration;		// Holds a copy of the ForceModelViscosityInfo value.
		Vec3f						m_initPosition;		// The position of the device when the ForceModel is started. 
	};


	// The ForceModel is used in Rigid Body Interactions
	// See also ForceModelRBIInfo
	class ForceModelRBI : public ForceModelAnimated
	{
	public:
									ForceModelRBI(int max_contacts, int DeviceID);								// Constructor
									ForceModelRBI(ForceModelRBIInfo* fmi, int max_contacts, int DeviceID);		// Constructor

		virtual Vec3f				Evaluate(const Vec3f& pos, const mgFloat& weight, const mgFloat& delta_t);
		virtual void				UpdateMembers();
		ForceModelRBIInfo*			GetForceModelInfo();

	private:
		Vec3f						m_lastForce;
	};

	// The ForceModel is used NavigationModels and creates a force field around a defined interaction radius. 
	// See also ForceModelNavigationInfo
	class ForceModelNavigation : public ForceModelAnimated
	{
	public:
									ForceModelNavigation(Vec3f& o, float& r, float& s, float& d, float& f, int DeviceID);								// Constructor
									ForceModelNavigation(ForceModelNavigationInfo* fmi, Vec3f& o, float& r, float& s, float& d, float& f,  int DeviceID);		// Constructor

		virtual Vec3f				Evaluate(const Vec3f& pos, const mgFloat& weight, const mgFloat& delta_t);
		virtual void				UpdateMembers();
		ForceModelNavigationInfo*	GetForceModelInfo();

	};


}

#endif
