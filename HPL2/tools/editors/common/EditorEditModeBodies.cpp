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

#include "EditorEditModeBodies.h"
#include "EditorEditModeSelect.h"

#include "EditorBaseClasses.h"
#include "EditorActionHandler.h"

#include "EditorWindowFactory.h"

#include "EditorWindowBodies.h"

#include "EditorActionsBodies.h"

#include "EntityWrapperBodyShape.h"
#include "EntityWrapperBody.h"

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

cEditorEditModeBodies::cEditorEditModeBodies(iEditorBase* apEditor,
											 iEditorWorld* apEditorWorld) : iEditorEditModeObjectCreator(apEditor, "Shapes", apEditorWorld)
{
}

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

void cEditorEditModeBodies::DrawObjectPreview(cEditorWindowViewport* apViewport, cRendererCallbackFunctions *apFunctions, const cVector3f& avPos, bool abPreCreationActive)
{
	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	apFunctions->GetLowLevelGfx()->DrawSphere(mpEditor->GetPosOnGridFromMousePos(),0.1f,cColor(1,0,0,1));

	eEditorBodyShape shapeType = ((cEditorWindowBodies*)mpWindow)->GetBodyShapeType();
	cBoundingVolume shapeBV;

	switch(shapeType)
	{
	case eEditorBodyShape_Box:		shapeBV.SetSize(cVector3f(1)); 
									break;
	case eEditorBodyShape_Sphere:	shapeBV.SetSize(cVector3f(2)); //diameter = radius*2 = 2
									break;
	case eEditorBodyShape_Cylinder:	shapeBV.SetSize(cVector3f(2,1,2));  //diameter = radius*2 = 2
									break;
	case eEditorBodyShape_Capsule:	shapeBV.SetSize(cVector3f(2,1,2));  //diameter = radius*2 = 2
									break;
	default:						shapeBV.SetSize(cVector3f(1)); 
									break;
	}

	const cVector3f& vBVMin = shapeBV.GetMin();
	const cVector3f& vBVMax = shapeBV.GetMax();
	
	cMatrixf mtxObject = cMath::MatrixTranslate(avPos);
	
	apFunctions->SetMatrix(&mtxObject);
		

	if(abPreCreationActive)
	{
		apFunctions->SetDepthTestFunc(eDepthTestFunc_Greater);
		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vBVMin,vBVMax, cColor(1,0,0,0.6f));
		apFunctions->SetDepthTestFunc(eDepthTestFunc_Less);
		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vBVMin,vBVMax, cColor(0,1,0,0.6f));
	
		/////////////////////////////////////////
		// Draw Textured Mesh
		//iVertexBuffer* pShapeVtxBuffer = mpEditor->GetBodyShapeVertexBuffer(shapeType);
		//if(pShapeVtxBuffer)
		//{
		//	apFunctions->SetVertexBuffer(pShapeVtxBuffer);
		//	apFunctions->DrawCurrent();
		//}
	}
	else
	{
		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vBVMin,vBVMax, cColor(1,0.5f));
	}

	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetMatrix(NULL);	

}

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

iEditorWindow* cEditorEditModeBodies::CreateSpecificWindow()
{
	return hplNew(cEditorWindowBodies,(this));
}

//-----------------------------------------------------------------

bool cEditorEditModeBodies::SetUpCreationData(iEntityWrapperData* apData)
{
	cEditorWindowBodies* pWin = (cEditorWindowBodies*)mpWindow;

	iEditorEditModeObjectCreator::SetUpCreationData(apData);

	cEntityWrapperDataBodyShape* pData = (cEntityWrapperDataBodyShape*)apData;
	pData->SetString(eShapeStr_ShapeType, cEntityWrapperBodyShape::EditorBodyShapeToString(pWin->GetBodyShapeType()));

	return true;
}

//-----------------------------------------------------------------

void cEditorEditModeBodies::CreateTypes()
{
	mvTypes.push_back(hplNew(cEntityWrapperTypeBodyShape,()));
	mvTypes.push_back(hplNew(cEntityWrapperTypeBody,()));
}

//-----------------------------------------------------------------


