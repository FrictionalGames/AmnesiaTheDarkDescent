
//! File: <ForceModelCallback.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_FORCEMODELCALLBACK_H_
#define _HAPTX_FORCEMODELCALLBACK_H_

#include "ForceModelManager.h"
#include "HXSolver/CollisionState.h"

namespace HaptX
{

// ForceModelSetUp is used by HaptX to set up ForceModels for rendering. This function is registered as a 
// callback function when a HapticDevice is created. It is called from whithin HapticsDevice::render.
// ForceModelSetUp pushes the camera matrix onto the CollisionState stack and then loops though the render 
// list and for each ForceModel in the render list the following is done:
//			UpdateMember is called.
//			The ForceModel is added to the CollistionState either globally or locally.
// In this case, since only the camera matrix is pushed on the stack all locally added ForceModels
// will be relative to the camera.
// Parameters: cs - A pointer to the CollisionSate object associated with the HapticsDevice.
// cameraMatrix - The current camera matrix for the device.
// forceModelRenderList - a list of ForceModel ID's that are currently active for rendering on the device.
int ForceModelSetUp(CollisionState* cs, const Matrix4f& cameraMatrix, std::list<int> forceModelRenderList)
{
	if (GetForceModelManager()->ForceModelsIsEnabled())
	{
		Matrix4f cam_mat = cameraMatrix;
		cs->Push(cam_mat,cam_mat.Inverse(),cam_mat,cam_mat.Inverse());

		for (std::list<int>::iterator j = forceModelRenderList.begin(); j != forceModelRenderList.end(); j++) 
		{
			ForceModel* fm = GetForceModelManager()->GetForceModel((*j));

			if (fm == NULL) continue;

			fm->UpdateMembers();

			if (fm->m_coordinateSystem == HX_ABSOLUTE_GLOBAL)
			{
				cs->AddAbsoluteGlobalFM(fm);
			}
			else if (fm->m_coordinateSystem == HX_ABSOLUTE_LOCAL)
			{
				cs->AddAbsoluteLocalFM(fm);
			}
		}

		cs->Pop();
	}

	return 0;
}
}

#endif
