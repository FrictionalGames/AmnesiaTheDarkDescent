
//! File: <ShapeManager.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef	_HAPTX_SHAPEMANAGER_H_
#define _HAPTX_SHAPEMANAGER_H_

#include <map>

#include "HXEngine.h"
#include "HXSolver/LinearAlgebra.h"
#include "HXSolver/HaptXDefines.h"

namespace HaptX
{
	// forward declaration
	class ShapeManager;
	class Surface;
	class RealtimeGeometry;
	class ImpulseBase;

	// A deviceContactInfo_t is created for each contact between a HapticShape and a HapticsDevice. The 
	// deviceContactInfo_t holds information about the force and torque that is applied by the HapticsDevice
	// as well as information about which triangles that are involved in the contact.
	struct deviceContactInfo_t
	{
		Vec3f	m_force;
		Vec3f	m_torque;
		int		m_numContactPoints;
		int*	m_triangleIDs;
		int		m_numTriangles;
	};
	
	// A HapticShape represents a HaptX enabled shape. 
	// Each HapticShape has a pointer to a RealtimeGeometry which contains the triangles that are used for collision detection.
	// A HapticShape also has a surface id. The Surface with that surface id generates forces and constrains the proxy when collision
	// occurs. The ImpulseBase calculates the impulse forces applied by the haptics device on the HapticShape. 
	// The HapticShape also has a map of deviceContactInfo_t. One deviceContactInfo_t is created for each contact with
	// the RealtimeGeometry and holds information about that contact.
	class HapticShape
	{
	public:
		~HapticShape();													// destructor

		void*				m_customData;								// Custom user data.

		iSurfaceID			m_surface;									// The id of the surface that is connected to the HapticShape.
		RealtimeGeometry*	m_realtimeGeometry;							// A pointer to the geometry of the HapticShape.
		ImpulseBase*		m_impulse;									// A pointer to the impulse object connected to this HapticShape.
		
		Matrix4f			m_localToGlobal;							// The transform of the HapticShape.
		Matrix4f			m_accumFrw;									// The accumulated forward matrix for this HapticShape
		Matrix4f			m_accumInv;									// The accumulated inverse matrix for this HapticShape
		
		int					m_numDeviceContacts;						// The number of devices that are currently in contact with the HapticShape.
		std::map<iHapticDeviceID,deviceContactInfo_t*> m_contactInfos;	// A map which holds a deviceContactInfo_t for each contact HapticsDevice that are in contact with the HapticShape.

	};

	
	ShapeManager*	GetShapeManager();									// Global function to get a pointer to the ShapeManager
	void			CreateShapeManager();								// Global function to create a ShapeManager
	void			ReleaseShapeManager();								// Global function to release the ShapeManager

	// The ShapeManager handles all registerd HapticShapes. When a HapticShape gets registered in the ShapeManager
	// an ID is assigned to the HapticShape. The ShapeManager is also taking care of the deletion of shapes and makes
	// sure that no references are held from the HaptX thread before deleting a HapticShape.
	class ShapeManager
	{
	public:
		ShapeManager();													// Constructor.
		~ShapeManager();												// Destructor.
		
		iHapticShapeID		RegisterShape(HapticShape* shape);			// Puts the HapticShape in the m_mapHapticShapes and assigns an ID to it. The ID is returned.
		HapticShape*		GetHapticShape(iHapticShapeID shapeID);		// Returns the HapticShape instance with the given ID. Null if the id doesn't exist.
		bool				ShapeExists(iHapticShapeID shapeID);		// Returns true if the shapeID exists.
		bool				DestroyShape(iHapticShapeID shapeID);		// Places the HapticShape in the m_listDeadShapes and removes it from all render lists.
		bool				CleanUpDeadShapes();						// Tries to delete the HapticShapes in m_listDeadShapes. If the haptics thread still holds references to a HapticShape it is not deleted.
		
	protected:
		int										m_nextID;				// The ID that the next HapticShape will get.
		std::list<HapticShape*>					m_listDeadShapes;		// A list of all HapticShapes that should be destoyed as soon as the haptic thread has no more references to it.
		std::map<iHapticShapeID,HapticShape*>	m_mapHapticShapes;		// A map which contains all HapticShape instances and their ID's.
	};
}

#endif
