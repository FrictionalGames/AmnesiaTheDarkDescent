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

#include "EditorEditModeLights.h"

#include "EditorBaseClasses.h"
#include "EditorActionHandler.h"
#include "EditorWorld.h"

#include "EntityWrapperLightBox.h"
#include "EntityWrapperLightSpot.h"
#include "EntityWrapperLightPoint.h"

#include "EditorWindowViewport.h"
#include "EditorWindowLights.h"

#include "BoxCreator.h"
#include "SphereCreator.h"

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

cEditorEditModeLights::cEditorEditModeLights(iEditorBase* apEditor,
											 iEditorWorld* apEditorWorld) : iEditorEditModeObjectCreator(apEditor, "Lights", apEditorWorld)
{
	mpCurrentCreator = NULL;
}

//-----------------------------------------------------------------

cEditorEditModeLights::~cEditorEditModeLights()
{
	for(int i=0;i<(int)mvShapeCreators.size();++i)
	{
		if(mvShapeCreators[i])
			hplDelete(mvShapeCreators[i]);
	}
	mvShapeCreators.clear();
}

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

void cEditorEditModeLights::DrawObjectPreview(cEditorWindowViewport* apViewport, cRendererCallbackFunctions *apFunctions, const cMatrixf& amtxTransform, bool abPreCreationActive)
{
	iEditorEditModeObjectCreator::DrawObjectPreview(apViewport, apFunctions, amtxTransform,abPreCreationActive);
	if(mpCurrentCreator)
		mpCurrentCreator->Draw(apViewport, apFunctions);
}

void cEditorEditModeLights::OnViewportMouseDown(int alButtons)
{
	iEditorEditModeObjectCreator::OnViewportMouseDown(alButtons);
	if(mpCurrentCreator)
		mpCurrentCreator->OnViewportMouseDown(alButtons);
}

void cEditorEditModeLights::OnViewportMouseUp(int alButtons)
{
	if(mpCurrentCreator)
	{
		mpCurrentCreator->OnViewportMouseUp(alButtons);
		if(mpCurrentCreator->IsDoneCreating())
		{
			iEditorEditModeObjectCreator::OnViewportMouseUp(alButtons);
			mpCurrentCreator->Reset();
		}
	}
	else
		iEditorEditModeObjectCreator::OnViewportMouseUp(alButtons);
}

void cEditorEditModeLights::OnEditorUpdate(float afTimeStep)
{
	iEditorEditModeObjectCreator::OnEditorUpdate(afTimeStep);
	if(mpCurrentCreator)
		mpCurrentCreator->OnEditorUpdate();
}
//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

bool cEditorEditModeLights::SetUpCreationData(iEntityWrapperData* apData)
{
	if(iEditorEditModeObjectCreator::SetUpCreationData(apData)==false)
		return false;

	switch(mlSubType)
	{
	case eEditorEntityLightType_Box:
		{
			cBoxCreator* pCreator = (cBoxCreator*)mpCurrentCreator;
			apData->SetVec3f(eObjVec3f_Position, pCreator->GetBoxCenter());
			apData->SetVec3f(eObjVec3f_Scale, pCreator->GetBoxSize());
			apData->SetVec3f(eLightBoxVec3f_Size, pCreator->GetBoxSize());

			break;
		}
	case eEditorEntityLightType_Point:
		{
			cSphereCreator* pCreator = (cSphereCreator*)mpCurrentCreator;
			apData->SetVec3f(eObjVec3f_Position, pCreator->GetSphereCenter());
			apData->SetFloat(eLightFloat_Radius, pCreator->GetRadius());

			break;
		}
	}

	return true;
}

//-----------------------------------------------------------------

void cEditorEditModeLights::OnSetSubType(int alX)
{
	mpCurrentCreator = mvShapeCreators[alX];
}

//-----------------------------------------------------------------

iEditorWindow* cEditorEditModeLights::CreateSpecificWindow()
{
	return hplNew(cEditorWindowLights,(this));
}

//-----------------------------------------------------------------

void cEditorEditModeLights::CreateTypes()
{
	mvTypes.push_back(hplNew(cEntityWrapperTypeLightBox,()));
	mvShapeCreators.push_back(hplNew(cBoxCreator,(this)));


	mvTypes.push_back(hplNew(cEntityWrapperTypeLightPoint,()));
	mvShapeCreators.push_back(hplNew(cSphereCreator,(this)));

	mvTypes.push_back(hplNew(cEntityWrapperTypeLightSpot,()));
	mvShapeCreators.push_back(NULL);
}

