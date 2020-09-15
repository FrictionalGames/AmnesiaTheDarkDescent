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

#include "LuxArea.h"

#include "LuxMap.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxAreaLoader::Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld)
{
	cLuxMap *pMap = gpBase->mpCurrentMapLoading;
	if(pMap==NULL) return;

	iLuxArea *pArea = CreateArea(asName, alID,pMap);

	//////////////////////////////
	// Create and set body
	iPhysicsWorld *pPhysicsWorld = apWorld->GetPhysicsWorld();
	iCollideShape* pShape = pPhysicsWorld->CreateBoxShape(avSize, NULL);
	iPhysicsBody* pBody = pPhysicsWorld->CreateBody(asName,pShape);

	pBody->SetCollide(false);
	pBody->SetCollideCharacter(false);
	pBody->SetMatrix(a_mtxTransform);
	pBody->SetUserData(pArea);
	
	pArea->mpBody = pBody;

	//////////////////////////////
	// Load base properties
	pArea->mvSize = avSize;
	pArea->m_mtxTransform = a_mtxTransform;

	//////////////////
	//Load variables
	LoadVariables(pArea, apWorld);

	//////////////////////////////
	// Load type specific properties
	SetupArea(pArea, apWorld);

	pMap->AddEntity(pArea);

	pArea->SetActive(abActive);

	pArea->SetupAfterLoad(apWorld);
}

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxArea::iLuxArea(const tString &asName, int alID, cLuxMap *apMap, eLuxAreaType aAreaType)  : iLuxEntity(asName,alID,apMap, eLuxEntityType_Area)
{
	mAreaType = aAreaType;

	mpBody = NULL;
}

//-----------------------------------------------------------------------

iLuxArea::~iLuxArea()
{
	if(mpBody)
	{
		mpMap->GetPhysicsWorld()->DestroyBody(mpBody);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxArea::OnRenderSolid(cRendererCallbackFunctions* apFunctions)
{
    /*if(mpBody==NULL) return;

	cBoundingVolume* pBV = mpBody->GetBoundingVolume();
	
	apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(), pBV->GetMax(),cColor(1,1,1,1));*/
}

//-----------------------------------------------------------------------

bool iLuxArea::CanInteract(iPhysicsBody *apBody)
{
	return false;
}

//-----------------------------------------------------------------------

bool iLuxArea::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	return false;
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair iLuxArea::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
	return eLuxFocusCrosshair_LastEnum;
}

//-----------------------------------------------------------------------

iEntity3D* iLuxArea::GetAttachEntity()
{
	return mpBody;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

void iLuxArea::OnSetActive(bool abX)
{	
	///////////////
	//Bodies
	if(mpBody)
		mpBody->SetActive(abX);
}

//-----------------------------------------------------------------------

void iLuxArea::OnUpdate(float afTimeStep)
{
	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeVirtual(iLuxArea_SaveData, iLuxEntity_SaveData)
kSerializeVar(mvSize,eSerializeType_Vector3f)
kSerializeVar(m_mtxTransform,eSerializeType_Matrixf)
kEndSerialize()


//-----------------------------------------------------------------------

iLuxEntity* iLuxArea_SaveData::CreateEntity(cLuxMap *apMap)
{
	iLuxArea *pArea = CreateArea(apMap);
	apMap->AddEntity(pArea);

	return pArea;
}

//-----------------------------------------------------------------------

void iLuxArea::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::SaveToSaveData(apSaveData);
	iLuxArea_SaveData *pData = static_cast<iLuxArea_SaveData*>(apSaveData);

    kCopyToVar(pData, mvSize);
	kCopyToVar(pData, m_mtxTransform);
}

//-----------------------------------------------------------------------

void iLuxArea::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::LoadFromSaveData(apSaveData);
	iLuxArea_SaveData *pData = static_cast<iLuxArea_SaveData*>(apSaveData);

	kCopyFromVar(pData, mvSize);
	kCopyFromVar(pData, m_mtxTransform);

	//////////////////////////////
	// Create and set body
	iPhysicsWorld *pPhysicsWorld = mpMap->GetPhysicsWorld();
	iCollideShape* pShape = pPhysicsWorld->CreateBoxShape(mvSize, NULL);
	iPhysicsBody* pBody = pPhysicsWorld->CreateBody(msName,pShape);

	pBody->SetCollide(false);
	pBody->SetCollideCharacter(false);
	pBody->SetMatrix(m_mtxTransform);
	pBody->SetUserData(this);
	pBody->SetActive(mbActive);

	mpBody = pBody;
	
	///////////////////
	//Do setup
	SetupAfterLoad(mpMap->GetWorld());
}

//-----------------------------------------------------------------------

void iLuxArea::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
	iLuxArea_SaveData *pData = static_cast<iLuxArea_SaveData*>(apSaveData);
}

//-----------------------------------------------------------------------

