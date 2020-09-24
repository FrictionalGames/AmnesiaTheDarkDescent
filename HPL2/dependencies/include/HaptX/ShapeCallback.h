
//! File: <ShapeCallback.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_SHAPECALLBACK_H_
#define _HAPTX_SHAPECALLBACK_H_

#include "ShapeManager.h"
#include "HXSolver/CollisionState.h"

namespace HaptX
{

// ShapeSetUp is used by HaptX to set up Shapes for rendering. This function is registered as a callback 
// function when a HapticsDevice is created. It is called from whithin HapticsDevice::render.
// SetUpShapes loops though the render list and for each shape in the render list the following is done:
//			The shape's transform matrix is pushed onto the matrix stack.
//			The surface is set in the CollisionState
//			The ImpulseBase is set in the CollisionState.
//			The contact info from the previous loop is updated.
//			The shape is addded to the HaptX thread by calling AddCollision in the CollisionState object.
//			The shape's transform matrix is popped off the matrix stack.
//			The surface pointer in the CollisionState is reset.
// Parameters: cs - A pointer to the CollisionSate object associated with the HapticsDevice
// deviceID - The id of the device that is calling ShapeSetUp
// delta_t - The time step since the last call to ShapeSetUP
// shapeRenderList - a list of shape ID's that are currently active for rendering on the device
int ShapeSetUp(CollisionState* cs, const int& deviceID, const double& delta_t, std::list<int> shapeRenderList)
{
	if (GetSurfaceManager()->SurfacesIsEnabled() && !shapeRenderList.empty())
	{
		for (std::list<int>::const_iterator i = shapeRenderList.begin(); i != shapeRenderList.end(); i++)
		{
			// Get the haptic shape from the render list
			HapticShape* currentShape = GetShapeManager()->GetHapticShape((*i));

			if (!currentShape) continue;
			if (!GetSurfaceManager()->SurfaceExists(currentShape->m_surface)) continue;

			// Remove the contact info from the last render
			if (currentShape->m_numDeviceContacts > 0) currentShape->m_numDeviceContacts -=1;

			// Get the matrix info (local coordinates to global)
			Matrix4f mi = currentShape->m_localToGlobal;
			
			// Push the matrix onto the CollisionState stack 
			cs->Push(mi,mi.Inverse(),mi,mi.Inverse());	
			cs->SetSurface(GetSurfaceManager()->GetSurface(currentShape->m_surface));
			cs->SetTextureDimension(-1);

			// update the fields in the surface
			if (cs->GetSurface()) cs->GetSurface()->UpdateFields();

			if (currentShape->m_impulse)
			{
				currentShape->m_accumFrw = cs->GetAccumulatedForward();
				currentShape->m_impulse->m_fulcrum = Vec3f(currentShape->m_accumFrw.m[0][3],currentShape->m_accumFrw.m[1][3],currentShape->m_accumFrw.m[2][3]);
					
				cs->SetImpulseBase(currentShape->m_impulse);	
			}

			int numContacts = cs->GetSurface()->GetNumContacts(deviceID);

			if (numContacts > 0)
			{
				if (currentShape->m_contactInfos.find(deviceID) ==  currentShape->m_contactInfos.end())
				{
					currentShape->m_contactInfos[deviceID] = new deviceContactInfo_t();
					// since we can have maximum 3 planes constraining the proxy at a time we allocate space 
					// for three id's. The id's are -1 if there isn't a constraint
					currentShape->m_contactInfos[deviceID]->m_triangleIDs = new int[3];
				}
					
				currentShape->m_contactInfos[deviceID]->m_numContactPoints  = numContacts;
				
				int triId0, triId1, triId2;
				currentShape->m_realtimeGeometry->GetTriangleID(triId0,triId1,triId2);
				currentShape->m_contactInfos[deviceID]->m_triangleIDs[0] = triId0;
				currentShape->m_contactInfos[deviceID]->m_triangleIDs[1] = triId1;
				currentShape->m_contactInfos[deviceID]->m_triangleIDs[2] = triId2;

				ImpulseBase* imp = cs->GetImpulseBase();
				if (imp)
				{
					currentShape->m_contactInfos[deviceID]->m_force = imp->m_impulse / delta_t;
					currentShape->m_contactInfos[deviceID]->m_torque = imp->m_angularImpulse / delta_t;
				}
				else
				{
					currentShape->m_contactInfos[deviceID]->m_force = Vec3f(0.0,0.0,0.0);
					currentShape->m_contactInfos[deviceID]->m_torque = Vec3f(0.0,0.0,0.0);
				}

				currentShape->m_numDeviceContacts += 1;
			}
			else
			{
				if (currentShape->m_contactInfos.find(deviceID) !=  currentShape->m_contactInfos.end())
				{
					delete currentShape->m_contactInfos[deviceID]->m_triangleIDs;
					delete currentShape->m_contactInfos[deviceID];
					
					currentShape->m_contactInfos.erase(deviceID);
				}
			}
				
			cs->AddCollision(currentShape->m_realtimeGeometry);
			cs->Pop();
			cs->SetSurface(0);
		}
	}

	return 1;
}

}
#endif
