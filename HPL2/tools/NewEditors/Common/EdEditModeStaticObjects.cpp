/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "../Common/EdEditModeStaticObjects.h"
#include "../Common/EdWindowStaticObjects.h"
#include "../Common/EdObjStaticObject.h"

//#include "../Common/EdWorld.h"
#include "../LevelEditor/LevelEditor.h"


//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

cEdEditModeStaticObjects::cEdEditModeStaticObjects(iEditor* apEditor) : iEdEditModeScnObjCreator(apEditor, _W("Static Objects") )
{
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

void cEdEditModeStaticObjects::DrawObjectPreview(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cMatrixf& amtxTransform, bool abPreCreationActive)
{
	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	/*cVector3f vPosition = amtxTransform.GetTranslation();

	apFunctions->GetLowLevelGfx()->DrawSphere(vPosition,0.1f,cColor(1,0,0,1));
	*/

	
	cEdWindowStaticObjects* pWin = static_cast<cEdWindowStaticObjects*>(mpWindow);
	iEdObjectIndexEntryMeshObject* pObj = pWin->GetSelectedObject();

	if(pObj)
	{
		const cVector3f& vBVMin = pObj->GetBVMin();
		const cVector3f& vBVMax = pObj->GetBVMax();

		apFunctions->SetMatrix((cMatrixf*)&amtxTransform);
		cMeshEntity* pEnt = pWin->GetPreviewEntity();
		if(pEnt)
			pEnt->SetVisible(abPreCreationActive);

		if(abPreCreationActive)
		{
			apFunctions->SetDepthTestFunc(eDepthTestFunc_Greater);
			apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vBVMin,vBVMax, cColor(1,0,0,0.6f));
			apFunctions->SetDepthTestFunc(eDepthTestFunc_Less);
			apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vBVMin,vBVMax, cColor(0,1,0,0.6f));
			
			/////////////////////////////////////////
			// Draw Textured Mesh
			if(pEnt)
			{
				pEnt->SetMatrix(amtxTransform);
			}
		}
		else
		{
			apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vBVMin,vBVMax, cColor(1,0.5f));
		}
	}

	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetMatrix(NULL);	
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

iEdWindow* cEdEditModeStaticObjects::CreateWindow()
{
	return hplNew(cEdWindowStaticObjects,(this, mpEditor->GetLookUpDirs(eDir_StaticObjects)));
}

//-----------------------------------------------------------------

iEdObjectType* cEdEditModeStaticObjects::CreateType()
{
	return hplNew(cTypeStaticObject,());
}
//-----------------------------------------------------------------

