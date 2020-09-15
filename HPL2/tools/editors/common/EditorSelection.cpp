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

#include "EditorSelection.h"

#include "EditorWorld.h"
#include "EntityWrapper.h"

#include <algorithm>

//----------------------------------------------------------------------

float cEditorSelection::mfScaleSnap = 0.25f;
float cEditorSelection::mfRotateSnap = kPi2f/6;

//----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// SELECTION CLASS
//////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------

cEditorSelection::cEditorSelection(iEditorBase* apEditor)
{
	mpEditor = apEditor;

	ClearEntities();
}

//----------------------------------------------------------------------

void cEditorSelection::AddEntity(iEntityWrapper* apEntity)
{
	///////////////////////////////////
	// If NULL entity, do nothing
	if(apEntity ==NULL) return;

	/////////////////////////////////////////////////////
	// Check if the entity isn't in selection already
	if(HasEntity(apEntity)==false)
	{
        mlstEntities.push_back(apEntity);
		mlstEntityIDs.push_back(apEntity->GetID());

		mlstReferenceTranslations.push_back(apEntity->GetPosition());
		mlstReferenceRotations.push_back(apEntity->GetRotation());
		mlstReferenceScales.push_back(apEntity->GetScale());

		UpdateCenter();

		if(mlstEntities.size()==1) 
			mvCenterRotation = apEntity->GetRotation();
		else
			mvCenterRotation = 0;
		mvCenterOldRotation = mvCenterRotation;

		apEntity->SetSelected(true);

		mbSelectionUpdated = true;
		mbPropertiesUpdated = true;

		mpEditor->SetSelectionChanged();
	}

}

void cEditorSelection::RemoveEntity(iEntityWrapper* apEntity)
{
	if(apEntity ==NULL) return;

	//////////////////////////////////////////////
	// Check if the entity is actually selected
	tEntityWrapperListIt it;
	if(HasEntity(apEntity, &it))
	{
        mlstEntities.erase(it);
		mlstEntityIDs.remove(apEntity->GetID());

		UpdateCenter();

		if(mlstEntities.size()==1) 
			mvCenterRotation = mlstEntities.front()->GetRotation();
		
		apEntity->SetSelected(false);

		mbSelectionUpdated = true;
		mbPropertiesUpdated = true;

		mpEditor->SetSelectionChanged();
	}
}

//----------------------------------------------------------------------

void cEditorSelection::ClearEntities()
{
	tEntityWrapperListIt it = mlstEntities.begin();
	for(;it!=mlstEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		if(mpEditor->GetEditorWorld()->HasEntity(pEnt))
			pEnt->SetSelected(false);
	}
	mlstEntities.clear();
	mlstEntityIDs.clear();

	mlstReferenceTranslations.clear();
	mlstReferenceRotations.clear();
	mlstReferenceScales.clear();

	mvMouseOffset = 0;
	mvSelectionMax = -99999999.0f;
	mvSelectionMin =  99999999.0f;
	mvCenterTranslation = 0;
	mvCenterRotation = 0;
	

	mvCenterOldRotation = 0;

	mbTransformed = true;

	mbSelectionUpdated = true;
	mbPropertiesUpdated = true;
}

//----------------------------------------------------------------------

bool cEditorSelection::HasEntity(int alEntityID, tIntListIt* apIt)
{
	tIntListIt it= find(mlstEntityIDs.begin(), mlstEntityIDs.end(), alEntityID);

	if(apIt)
		*apIt = it;

    return (it!=mlstEntityIDs.end());
}

bool cEditorSelection::HasEntity(iEntityWrapper* apEntity, tEntityWrapperListIt* apIt)
{
	if(apEntity==NULL) return false;

	tEntityWrapperListIt it= find(mlstEntities.begin(), mlstEntities.end(), apEntity);

	if(apIt)
		*apIt = it;

	return (it!=mlstEntities.end());
}

//----------------------------------------------------------------------

void cEditorSelection::AddEntitiesByID(const tIntList& alstIDs)
{
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();

	ClearEntities();

	tIntList::const_iterator it = alstIDs.begin();
	for(;it!=alstIDs.end();++it)
	{
		iEntityWrapper* pEnt = pWorld->GetEntity(*it);
		AddEntity(pEnt);
	}
}

void cEditorSelection::RemoveEntitiesByID(const tIntList& alstIDs)
{
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();

	tIntList::const_iterator it = alstIDs.begin();
	for(;it!=alstIDs.end();++it)
	{
		iEntityWrapper* pEnt = pWorld->GetEntity(*it);
		RemoveEntity(pEnt);
	}
}

void cEditorSelection::ToggleEntitySelectionByID(const tIntList& alstIDs)
{
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();

	tIntList::const_iterator it = alstIDs.begin();
	for(;it!=alstIDs.end();++it)
	{
		iEntityWrapper* pEnt = pWorld->GetEntity(*it);
		if(HasEntity(pEnt))
			RemoveEntity(pEnt);
		else
            AddEntity(pEnt);
	}
}
//----------------------------------------------------------------------

void cEditorSelection::UpdateCenter()
{
	mvSelectionMax = -99999999.0f;
	mvSelectionMin =  99999999.0f;

	cVector3f vSelectionMax = mvSelectionMax;
	cVector3f vSelectionMin = mvSelectionMin;
	
	tEntityWrapperListIt it = mlstEntities.begin();
	for(;it!=mlstEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;

		cBoundingVolume* pBV = pEnt->GetPickBV(NULL);
		if(pBV)
			cMath::ExpandAABB(mvSelectionMin,mvSelectionMax, pBV->GetMin(),pBV->GetMax());

		cMath::ExpandAABB(vSelectionMin,vSelectionMax, pEnt->GetPosition(),pEnt->GetPosition());		
	}
	mvCenterTranslation = (vSelectionMin + vSelectionMax)*0.5f;
}

//----------------------------------------------------------------------

void cEditorSelection::SetRelativeTranslation(const cVector3f& avPosition, bool abUseSnap)
{
	tEntityWrapperListIt it = mlstEntities.begin();
	tVector3fListIt itTranslation = mlstReferenceTranslations.begin();

	for(;it!=mlstEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		cVector3f vReferenceTranslation = *itTranslation;
		
		pEnt->SetAbsPosition(vReferenceTranslation + avPosition);

		if(abUseSnap)
			pEnt->SnapToGrid();
		
		pEnt->UpdateEntity();

		++itTranslation;
	}

	UpdateCenter();
	mbTransformed = true;
}

//----------------------------------------------------------------------

void cEditorSelection::SetRelativeRotation(const cVector3f& avRotation, bool abUseSnap)
{
	cVector3f vAddRotation = avRotation;

	if(abUseSnap)
		SnapRotate(cVector3f(0), vAddRotation);

	cMatrixf mtxAddRotation = cMath::MatrixRotate(vAddRotation,eEulerRotationOrder_XYZ);
	cMatrixf mtxNewRotation = cMath::MatrixMul(cMath::MatrixRotate(mvCenterOldRotation,eEulerRotationOrder_XYZ),mtxAddRotation);

    cVector3f vNewRotation = cMath::MatrixToEulerAngles(mtxNewRotation, eEulerRotationOrder_XYZ);

	
	vAddRotation = vNewRotation - mvCenterOldRotation;

	tEntityWrapperListIt it = mlstEntities.begin();
	tVector3fListIt itRotation = mlstReferenceRotations.begin();

	for(;it!=mlstEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		cVector3f vReferenceRotation = *itRotation;

				
		pEnt->SetAbsRotation(vReferenceRotation + vAddRotation);
		pEnt->UpdateEntity();

		++itRotation;
	}

	mvCenterRotation = vNewRotation;

	UpdateCenter();
	mbTransformed = true;
}

//----------------------------------------------------------------------

void cEditorSelection::SetRelativeScale(const cVector3f& avScale, bool abUseSnap)
{
	tEntityWrapperListIt it = mlstEntities.begin();
	tVector3fListIt itScale = mlstReferenceScales.begin();

	int lAxis = -1;
	for(int i=0;i<3;++i)
	{
		if(avScale.v[i]!=0)
			lAxis=i;
	}

	int i=0;
	for(;it!=mlstEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		cVector3f vReferenceScale = *itScale;
		cVector3f vNewScale = vReferenceScale + avScale;
		
		if(abUseSnap)
			SnapScale(vNewScale);

		pEnt->SetAbsScale(vNewScale, lAxis);
		
		pEnt->UpdateEntity();

		++itScale;
	}

	UpdateCenter();
	mbTransformed = true;
}

//----------------------------------------------------------------------

void cEditorSelection::UpdateReferenceTransforms()
{
	tEntityWrapperListIt itEntities = mlstEntities.begin();
	tVector3fListIt		 itTranslations = mlstReferenceTranslations.begin();
	tVector3fListIt		 itRotations = mlstReferenceRotations.begin();
	tVector3fListIt		 itScales = mlstReferenceScales.begin();

	while(itEntities!= mlstEntities.end())
	{
		iEntityWrapper* pEnt = *itEntities;
		
		*itTranslations = pEnt->GetPosition();
		*itRotations = pEnt->GetRotation();
		*itScales = pEnt->GetScale();
		
		++itEntities;
		++itTranslations;
		++itRotations;
		++itScales;
	}

	mvCenterOldTranslation = mvCenterTranslation;
	mvCenterOldRotation = mvCenterRotation;
}

//----------------------------------------------------------------------

void cEditorSelection::FallToReferenceTransforms()
{
	tEntityWrapperListIt itEntities = mlstEntities.begin();
	tVector3fListIt		 itTranslations = mlstReferenceTranslations.begin();
	tVector3fListIt		 itRotations = mlstReferenceRotations.begin();
	tVector3fListIt		 itScales = mlstReferenceScales.begin();

	while(itEntities!= mlstEntities.end())
	{
		iEntityWrapper* pEnt = *itEntities;
		
		pEnt->SetAbsPosition(*itTranslations);
		pEnt->SetAbsRotation(*itRotations);
		pEnt->SetAbsScale(*itScales);
		
		++itEntities;
		++itTranslations;
		++itRotations;
		++itScales;
	}

	mvCenterTranslation = mvCenterOldTranslation;
	mvCenterRotation = mvCenterOldRotation;
}

//----------------------------------------------------------------------

void cEditorSelection::SnapScale(cVector3f &avVector)
{
	SnapValue(avVector,mfScaleSnap);
}

//----------------------------------------------------------------------

void cEditorSelection::SnapValue(cVector3f& avVector, float afSnapAmount)
{
	if(afSnapAmount != 0)
	{
		afSnapAmount = cMath::Abs(afSnapAmount);
		avVector.x = cMath::RoundToInt(avVector.x/afSnapAmount)*afSnapAmount;
		avVector.y = cMath::RoundToInt(avVector.y/afSnapAmount)*afSnapAmount;
		avVector.z = cMath::RoundToInt(avVector.z/afSnapAmount)*afSnapAmount;
	}
}

//----------------------------------------------------------------------

void cEditorSelection::SnapRotate(const cVector3f &avBaseRotation, cVector3f &avAddedRotation)
{
	SnapValue(avAddedRotation, mfRotateSnap);
}


//----------------------------------------------------------------------

void cEditorSelection::SetCenterTranslation(const cVector3f& avTranslation)
{
	mvCenterOldTranslation = mvCenterTranslation;

	mvCenterTranslation = avTranslation;
}

//----------------------------------------------------------------------

void cEditorSelection::SetCenterRotation(const cVector3f& avRotation)
{
	mvCenterOldRotation = mvCenterRotation;

	mvCenterRotation = avRotation;
}

//----------------------------------------------------------------------

bool cEditorSelection::CanTranslate()
{
	UpdateProperties();

	return mbCanTranslate;
}

//----------------------------------------------------------------------

bool cEditorSelection::CanRotate()
{
	UpdateProperties();

	return mbCanRotate;
}

//----------------------------------------------------------------------

bool cEditorSelection::CanScale()
{
	UpdateProperties();

	return mScaleType!=eScaleType_None;
}


//----------------------------------------------------------------------

bool cEditorSelection::IsCloneable()
{
	UpdateProperties();

	return mbCloneable;
}

//----------------------------------------------------------------------

bool cEditorSelection::IsDeletable()
{
	UpdateProperties();

	return mbDeletable;
}

//----------------------------------------------------------------------

bool cEditorSelection::IsHomogeneousSelection()
{
	UpdateProperties();

	return mbHomogeneous;
}

//----------------------------------------------------------------------

void cEditorSelection::UpdateProperties()
{
	if(mbPropertiesUpdated)
	{
		mbPropertiesUpdated=false;

		if(IsEmpty())
		{
			mbCanTranslate=false;
			mbCanRotate=false;
			mScaleType=eScaleType_None;
			mbCloneable=false;
			mbDeletable=false;

			return;
		}

		mbCanTranslate=true;
		mbCanRotate=true;
		mScaleType=eScaleType_Normal;
		mbCloneable=true;
		mbDeletable=true;

		mbHomogeneous=true;

		int type = mlstEntities.front()->GetTypeID();
		bool bSingleEntity = mlstEntities.size()==1;

		tEntityWrapperListIt it = mlstEntities.begin();
		for(;it!=mlstEntities.end();++it)
		{
			iEntityWrapper* pEnt = *it;

			if(type!=pEnt->GetTypeID())
				mbHomogeneous=false;
			mbCanTranslate = mbCanTranslate && pEnt->CanTranslate();
			mbCanRotate = mbCanRotate && pEnt->CanRotate();
			if(mScaleType!=eScaleType_None)
			{
				if(mScaleType!=eScaleType_UniformOnly)
				{
					//if(bSingleEntity==false && pEnt->IsScalableGrouped()==false)
					//	mScaleType = eScaleType_None;
					//else
					mScaleType = pEnt->GetScaleType();
				}
				else
				{
					if(pEnt->GetScaleType()==eScaleType_None)
						mScaleType = eScaleType_None;
				}
			}
			mbCloneable = mbCloneable && pEnt->IsCloneable();
			mbDeletable = mbDeletable && pEnt->IsDeletable();
		}
	}
}

//----------------------------------------------------------------------
