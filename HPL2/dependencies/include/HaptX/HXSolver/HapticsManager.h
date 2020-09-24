
//! File: <HapticsManager.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

// The HapticsManager handles the realtime rendering of the
// various RealtimeGeometry instances created each scene-graph loop.
//
// The Scene object creates a HapticsManager object for each collision
// traversal and passes it to the realtime process for simulation (see
// Realtime.h).

//! NOT FOR RELEASE

#ifndef _HAPTX_HAPTICSMANAGER_H_
#define _HAPTX_HAPTICSMANAGER_H_

#include "Surface.h"
#include "RealtimeGeometry.h"
#include "ValueError.h"

namespace HaptX
{
	// errors associated with memory allocation.
	HAPTX_VALUE_ERROR(const int, BadAllocationSize);
	HAPTX_VALUE_ERROR(const int, BadDeallocationSize);

	class HAPTX_SOLVER_SPEC HapticsManager
	{
	public:
		typedef std::list<ExclusionPlane> ExclusionPlanes;
		
		inline			HapticsManager(const int& _index);				// Constructor.
		inline			~HapticsManager();								// Destructor.

		inline void		SetDeviceId(const int& _index);					// Set the haptics device id.
		inline int		GetDeviceId() const;							// Get the haptics device id.

		inline void		Initialize();									// Initialize resets the haptics manager.
		
		inline void		SetProxy(const Vec3f& _proxy);					// Set the proxy position.
		inline Vec3f	GetProxy() const;								// Returns the proxy position.
		
		inline void		SetStart(const Vec3f& _start);					// Set the start position.

		inline void		SetGeometries(GeometryVec& gv);					// Moves the RealtimeGeometry instances from the given vector to our rt_geometries vector.

		// Collide attempts to move along the given line segment stopping where it comes into contact with one or more of the
		// exclusion_planes. The point of collision (or the endpoint if there wasn't one) is returned in the 'to' argument and the range
		// [exclusion_planes.begin(), end_picked] indicates the restraining surfaces.
		void			Collide(const Vec3f& from, Vec3f& to, const mgFloat& eps);
		
		void			CollideNoPick(const Vec3f& from, Vec3f& to);		// The same as Collide except we dont pick the resulting colliders.
		void			CollideUpdate(const Vec3f& from, Vec3f& to);

		Vec3f			Constrain(const Vec3f& p);							// Finds the exclusion_planes (no more than 3) that are constraining the proxy from reaching the given point.
		
		void			UpdateExclusionPlanes(const Vec3f& p);				// Calls ExcludeGlobal in all of the rt_geometries. Upon return the exclusion_planes vector will contain the generated planes.

		inline void		SpliceEPs(ExclusionPlanes::iterator i, ExclusionPlanes::iterator j);										// shorthand to move one or more elements of the exclusion_planes list.
		inline void		SpliceEPs(ExclusionPlanes::iterator i, ExclusionPlanes::iterator j, ExclusionPlanes::iterator k);			// shorthand to move one or more elements of the exclusion_planes list.
		inline void		MoveEPs(ExclusionPlanes::iterator& i, ExclusionPlanes::iterator& j);										// shorthand to move one or more elements of the exclusion_planes list.
		inline void		MoveEPs(ExclusionPlanes::iterator& first, ExclusionPlanes::iterator last, ExclusionPlanes::iterator& k);	// shorthand to move one or more elements of the exclusion_planes list.
			
		void			DumpEPs();											// Debug function

		static void		ClearAndFreePool();
		
		// The function call operator generates a force for the given haptic device position (as measured relative to its initial
		// position, which is the local origin). Weighting is the in the range [0,1] indicating the factor by
		// which the resulting force will be scaled before being sent to the haptic device. Forces are weighted in order to interpolate
		// between successive HapticsManagers. The surface operator <em>must not</em> scale its resulting force according to this
		// weighting. The weighting should be used if the operator is going to impart an effect on the graphics loop (such as an impulse).
		Vec3f			operator () (const Vec3f& p, const mgFloat& weighting, const mgFloat& delta_t);

		void*			operator new (size_t);					// we provide our own allocation/deallocation of haptics manager objects, for efficiency.
		void			operator delete (void*, size_t);		// we provide our own allocation/deallocation of haptics manager objects, for efficiency.

	public:
		GeometryVec		m_rtGeometries;							// Holds pointers to the geometries that are currently being rendered.
		ExclusionPlanes m_exclusionPlanes;						// The planes excluding us from the rt_geometries.
	
		Vec3f			m_saveP;
		int				m_saveNc;

		Vec3f			m_proxy;								// The position of the proxy in the local space of the operator (initially equal to the intersection point).
		Vec3f			m_start;								// The position of the haptics device upon creation of the HapticsManager. Subsequent calls to operator() will provide haptic device position relative to start.
		
		ExclusionPlanes::iterator m_endPicked;					// The surfaces picked as actually constraining will be [exclusion_planes.begin(), end_picked]

		Vec3f			m_lastFinger;
		int				m_deviceId;

	private:
		int				m_rc;
	};

    //! Inline member methods

    //! Constructor
    inline HapticsManager::HapticsManager(const int& _index) : m_deviceId(_index)
    {
    }

    //! Destructor
    inline HapticsManager::~HapticsManager() 
	{
		GeometryVec::iterator rtg;
		for (rtg = m_rtGeometries.begin(); rtg != m_rtGeometries.end(); rtg++)
		{
			(*rtg)->DecreaseHapticRef();
			(*rtg)->m_surface->DecreaseHapticRef();
		}
	}

    //! Initialize
    inline void HapticsManager::Initialize()
    {
	    m_proxy = m_start = m_lastFinger = Vec3f(0.0,0.0,0.0);
	    m_rtGeometries.clear();
	    m_exclusionPlanes.clear();
    }

    //! setDeviceId
    inline void HapticsManager::SetDeviceId(const int& _index)
    {
	    m_deviceId = _index;
    }

    //! GetDeviceId
    inline int HapticsManager::GetDeviceId() const
    {
	    return m_deviceId;
    }

    //! Set the proxy position.
    inline void HapticsManager::SetProxy(const Vec3f& _proxy)
    {
	    m_proxy = _proxy;
    }

    //! Set the start position.
    inline void HapticsManager::SetStart(const Vec3f& _start)
    {
	    m_start = m_lastFinger = _start;
    }

    //! Moves the RealtimeGeometry instances from the given vector to our rt_geometries vector.
    inline void HapticsManager::SetGeometries(GeometryVec& gv)
    {
	    m_rtGeometries = gv;
    }

    //! Returns the proxy position.
    inline Vec3f HapticsManager::GetProxy() const
    {
	    return m_proxy;
    }

    //! Shorthand to swap two elements of the exclusion_planes list.
    inline void HapticsManager::SpliceEPs(ExclusionPlanes::iterator i, ExclusionPlanes::iterator j)
    {
	    m_exclusionPlanes.splice(i,m_exclusionPlanes,j);
    }

    //! Shorthand to swap two elements of the exclusion_planes list.
    inline void HapticsManager::SpliceEPs(ExclusionPlanes::iterator i, ExclusionPlanes::iterator j, ExclusionPlanes::iterator k)
    {
	    m_exclusionPlanes.splice(i,m_exclusionPlanes,j,k);
    }

    //! MoveEPs - the following move functions move elements from the beginning of
    //! one range to the beginning of another, updating the iterators to
    //! the beginning of the ranges.
    inline void HapticsManager::MoveEPs(ExclusionPlanes::iterator& i, ExclusionPlanes::iterator& j)
    {
	    ExclusionPlanes::iterator t = i;
	    ++t;
	    m_exclusionPlanes.splice(j,m_exclusionPlanes,i);
	    j = i;
	    i = t;
    }

    //! MoveEPs
    inline void HapticsManager::MoveEPs(ExclusionPlanes::iterator& first, ExclusionPlanes::iterator last, ExclusionPlanes::iterator& k)
    {
	    m_exclusionPlanes.splice(k,m_exclusionPlanes,first,last);
	    k = first;
	    first = last;
    }
}

#endif
