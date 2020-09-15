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

#include "EditorEditModePrimitive.h"

#include "EditorBaseClasses.h"
#include "EditorWorld.h"
#include "EditorWindowPrimitives.h"

#include "EditorActionEntity.h"
#include "EntityWrapperPrimitivePlane.h"



//---------------------------------------------------------------------------

iPrimitiveMeshCreator::iPrimitiveMeshCreator(cEditorEditModePrimitives* apEditMode)
{
	mpEditMode = apEditMode;
}

//---------------------------------------------------------------------------

iEntityWrapperData* iPrimitiveMeshCreator::CreateData()
{
	return mpType->CreateData();
}

//---------------------------------------------------------------------------

void iPrimitiveMeshCreator::SetUpData(iEntityWrapperData* apData)
{
	cEditorWindowPrimitives* pWindow = (cEditorWindowPrimitives*)mpEditMode->GetEditorWindow();
	
	apData->SetString(ePrimitiveStr_Material, pWindow->GetMaterial());
	apData->SetBool(ePrimitiveBool_CastShadows, pWindow->GetCastShadows());
	apData->SetBool(ePrimitiveBool_Collides, pWindow->GetCollides());
}

//---------------------------------------------------------------------------

cPrimitiveMeshCreatorPlane::cPrimitiveMeshCreatorPlane(cEditorEditModePrimitives* apEditMode) : iPrimitiveMeshCreator(apEditMode)
{
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::OnViewportMouseDown(int alButtons)
{
	mvCorners.clear();

	StoreMousePosition();
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::OnViewportMouseUp(int alButtons)
{
	StoreMousePosition();
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::StoreMousePosition()
{
	cVector3f &vMousePos = mpEditMode->GetEditor()->GetPosOnGridFromMousePos();
	mvCorners.push_back(vMousePos);
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cVector3f& avPos)
{
	if(mvCorners.size()==1)
	{
		apFunctions->SetProgram(NULL);
		apFunctions->SetTextureRange(NULL,0);
	
		apFunctions->SetMatrix(NULL);
		
		cVector3f vFirstCorner = mvCorners[0];
		cVector3f vMousePos = mpEditMode->GetEditor()->GetPosOnGridFromMousePos();

		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vFirstCorner, vMousePos,cColor(1,1));

		apFunctions->GetLowLevelGfx()->DrawSphere((vFirstCorner + vMousePos)*0.5f, 0.1f, cColor(1,0,0,1));
	}
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::SetUpData(iEntityWrapperData* apData)
{
	iPrimitiveMeshCreator::SetUpData(apData);
	cEditorWindowPrimitives* pWindow = (cEditorWindowPrimitives*)mpEditMode->GetEditorWindow();

	apData->SetVec3f(ePrimitivePlaneVec3f_TileAmount, pWindow->GetTileAmount());
	apData->SetVec3f(ePrimitivePlaneVec3f_TileOffset, pWindow->GetTileOffset());
	apData->SetFloat(ePrimitivePlaneFloat_TextureAngle, cMath::ToRad(pWindow->GetTextureAngle()));
	apData->SetBool(ePrimitivePlaneBool_AlignToWorld, pWindow->GetAlignToWorld());

	SortCorners();

	cVector3f vEndCorner = mvCorners[1]-mvCorners[0];
	apData->SetVec3f(ePrimitivePlaneVec3f_StartCorner, 0);
	apData->SetVec3f(ePrimitivePlaneVec3f_EndCorner, vEndCorner);
	apData->SetVec3f(eObjVec3f_Position, mvCorners[0]);

	for(int i=0;i<3;++i)
		if(vEndCorner.v[i]==0) vEndCorner.v[i] = 1;
	apData->SetVec3f(eObjVec3f_Scale, vEndCorner);
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::SortCorners()
{
	////////////////////////////////////////////////
	// Really strange algo to determine the corner 
	// which is "most negative" (closest to (-inf,-inf) in the plane of creation)
	cVector3f vCorner1 = mvCorners[0];
	cVector3f vCorner2 = mvCorners[1];
	cVector3f vDiff = vCorner2 - vCorner1;
	int lFirstNegativeIndex = -1;
	int lFirstPositiveIndex = -1;
	int lNumNegativeCoords = 0;
	for(int i=0;i<3;++i)
	{
		if(vDiff.v[i]<0) 
		{
			lNumNegativeCoords++;
			if(lFirstNegativeIndex==-1) 
				lFirstNegativeIndex = i;
		}
		else if(vDiff.v[i]>0)
		{
			if(lFirstPositiveIndex==-1) 
				lFirstPositiveIndex = i;
		}
	}

	if(lNumNegativeCoords==0)
	{
	}
	else if(lNumNegativeCoords==2)
	{
		//////////////////////////////////////
		// Store the corners backwards
		mvCorners.clear();
		mvCorners.push_back(vCorner2);
		mvCorners.push_back(vCorner1);
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////////////////////////
		// Bake the corners to store, since we are not that interested in the ones given by the user
		float fWidth = cMath::Abs(vCorner2.v[lFirstNegativeIndex]-vCorner1.v[lFirstNegativeIndex]);
		float fHeight = cMath::Abs(vCorner2.v[lFirstPositiveIndex]-vCorner1.v[lFirstPositiveIndex]);
        
		cVector3f vAdd1 = cVector3f(0);
		cVector3f vAdd2 = cVector3f(0);
		vAdd1.v[lFirstNegativeIndex] = fWidth;
		vAdd2.v[lFirstPositiveIndex] = fHeight;
		
		mvCorners.clear();
		mvCorners.push_back(vCorner1-vAdd1);
		mvCorners.push_back(vCorner1+vAdd2);
	}
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::Reset()
{
	mvCorners.clear();
}

//---------------------------------------------------------------------------

bool cPrimitiveMeshCreatorPlane::IsDoneCreating()
{
	if(mvCorners.size()!=2)
		return false;

    int lNumEqualCoords=0;
	for(int i=0;i<3;++i)
	{
		if(mvCorners[0].v[i]==mvCorners[1].v[i]) lNumEqualCoords++;
	}

	return lNumEqualCoords<2;
}

//---------------------------------------------------------------------------

cEditorEditModePrimitives::cEditorEditModePrimitives(iEditorBase* apEditor, iEditorWorld* apEditorWorld) : iEditorEditModeObjectCreator(apEditor, "Primitives", apEditorWorld) 
{
}

//---------------------------------------------------------------------------

cEditorEditModePrimitives::~cEditorEditModePrimitives()
{
	STLDeleteAll(mvMeshCreators);
}

//---------------------------------------------------------------------------

iEntityWrapperData* cEditorEditModePrimitives::CreateObjectData()
{
	iEntityWrapperData* pData = NULL;
	if(mpCurrentCreator)
	{
		pData = mpCurrentCreator->CreateData();

		//////////////////////////////
		// General data setup (name)
		iEditorEditModeObjectCreator::SetUpCreationData(pData);
		mpCurrentCreator->SetUpData(pData);

		if(pData->GetString(ePrimitiveStr_Material)=="" || pData->PostCreateSetUp()==false)
		{
			hplDelete(pData);
			pData = NULL;
		}
	}

	return pData;
}

//---------------------------------------------------------------------------

void cEditorEditModePrimitives::SetPrimitiveType(int alX)
{
	SetSubType(alX);
	mpCurrentCreator = mvMeshCreators[alX];
}

//---------------------------------------------------------------------------

void cEditorEditModePrimitives::OnViewportMouseDown(int alButtons)
{
	if((alButtons&eGuiMouseButton_Left) == 0) return;

	mpCurrentCreator->OnViewportMouseDown(alButtons);
}

//---------------------------------------------------------------------------

void cEditorEditModePrimitives::OnViewportMouseUp(int alButtons)
{
	if(alButtons&eGuiMouseButton_Right)
	{
		mpCurrentCreator->Reset();
		return;
	}
	else if((alButtons&eGuiMouseButton_Left) == 0) return;

	mpCurrentCreator->OnViewportMouseUp(alButtons);

	if(mpCurrentCreator->IsDoneCreating())
	{
		iEditorAction* pAction = CreateObject(CreateObjectData());
		mpEditor->AddAction(pAction);
	}
}

//---------------------------------------------------------------------------

void cEditorEditModePrimitives::DrawPostGrid(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cVector3f& avPos)
{
	iEditorEditMode::DrawPostGrid(apViewport, apFunctions, avPos);

	mpCurrentCreator->Draw(apViewport, apFunctions, avPos);
}

//---------------------------------------------------------------------------

iEditorWindow* cEditorEditModePrimitives::CreateSpecificWindow()
{
	return hplNew(cEditorWindowPrimitives,(this));
}

//---------------------------------------------------------------------------

void cEditorEditModePrimitives::CreateTypes()
{
	cPrimitiveMeshCreatorPlane* pCreator = hplNew( cPrimitiveMeshCreatorPlane,(this));
	mvMeshCreators.push_back(pCreator);

	iEntityWrapperType* pType = hplNew(cEntityWrapperTypePrimitivePlane,());
	mvTypes.push_back(pType);

	pCreator->SetPrimitiveType(pType);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
