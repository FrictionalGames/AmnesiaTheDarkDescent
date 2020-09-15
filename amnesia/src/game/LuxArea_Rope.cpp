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

#include "LuxArea_Rope.h"

#include "LuxMap.h"

#include "LuxProp.h"

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// ROPE START LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxAreaRopeLoader::Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld)
{
	cLuxMap *pMap = gpBase->mpCurrentMapLoading;
	if(pMap==NULL) return;
	
	cLuxRope *pRope = hplNew(cLuxRope, (asName, alID,pMap));


	//////////////////////////
	//Set Properties
	pRope->mvStartPos = a_mtxTransform.GetTranslation();

	pRope->msEndPosNode = GetVarString("EndPosNode",""); 
	pRope->msStartBody = GetVarString("StartBody",""); 
	pRope->msEndBody = GetVarString("EndBody","");

	pRope->mbAutoMove = GetVarBool("AutoMove",false); 
	pRope->mfAutoMoveAcc = GetVarFloat("AutoMoveAcc",0); 
	pRope->mfAutoMoveMaxSpeed = GetVarFloat("AutoMoveMaxSpeed",0); 

	pRope->mfMinTotalLength = GetVarFloat("MinTotalLength",0); 
	pRope->mfMaxTotalLength = GetVarFloat("MaxTotalLength",0); 

	//Load settings file
	tString sFile = GetVarString("RopeFile", "");
	iXmlDocument *pDoc = NULL;
    if(sFile != "")
	{
		pDoc = gpBase->mpEngine->GetResources()->LoadXmlDocument(sFile);
		if(pDoc==NULL)
			Error("Could not load '%s' for rope '%s'. Using set variables instead!\n", sFile.c_str(), asName.c_str());
	}

	// From XML 
	if(pDoc)
	{
		pRope->mfSegmentLength = pDoc->GetAttributeFloat("SegmentLength",0); 
		pRope->mfDamping = pDoc->GetAttributeFloat("Damping",0); 
		pRope->mfStrength = pDoc->GetAttributeFloat("Strength",0); 
		pRope->mfStiffness = pDoc->GetAttributeFloat("Stiffness",0); 
		pRope->msMaterial = pDoc->GetAttributeString("Material",""); 
		pRope->mfRadius = pDoc->GetAttributeFloat("Radius",0); 
		pRope->mfLengthTileAmount = pDoc->GetAttributeFloat("LengthTileAmount",0); 
		pRope->mfLengthTileSize = pDoc->GetAttributeFloat("LengthTileSize",0); 
		pRope->msSound = pDoc->GetAttributeString("Sound",""); 
		pRope->mfSoundStartSpeed = pDoc->GetAttributeFloat("SoundStartSpeed",0); 
		pRope->mfSoundStopSpeed = pDoc->GetAttributeFloat("SoundStopSpeed",0); 

		gpBase->mpEngine->GetResources()->DestroyXmlDocument(pDoc);
	}
	//From Area variables
	else
	{
		pRope->mfSegmentLength = GetVarFloat("SegmentLength",0); 
		pRope->mfDamping = GetVarFloat("Damping",0); 
		pRope->mfStrength = GetVarFloat("Strength",0); 
		pRope->mfStiffness = GetVarFloat("Stiffness",0); 
		pRope->msMaterial = GetVarString("Material",""); 
		pRope->mfRadius = GetVarFloat("Radius",0); 
		pRope->mfLengthTileAmount = GetVarFloat("LengthTileAmount",0); 
		pRope->mfLengthTileSize = GetVarFloat("LengthTileSize",0); 
		pRope->msSound = GetVarString("Sound",""); 
		pRope->mfSoundStartSpeed = GetVarFloat("SoundStartSpeed",0); 
		pRope->mfSoundStopSpeed = GetVarFloat("SoundStopSpeed",0); 
	}
		

	//////////////////////////
	//Add to map
	pMap->AddEntity(pRope);
	pRope->SetActive(abActive);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxRope::cLuxRope(const tString& asName, int alID, cLuxMap *apMap)  : iLuxEntity(asName,alID,apMap, eLuxEntityType_Rope)
{
	mbRopeCreated = false;

	mpRope = NULL;
	mpRopeGfx= NULL;
}

//-----------------------------------------------------------------------

cLuxRope::~cLuxRope()
{
	if(mpRopeGfx)
	{
		mpMap->GetWorld()->DestroyRopeEntity(mpRopeGfx);
	}
	if(mpRope)
	{
		mpMap->GetPhysicsWorld()->DestroyRope(mpRope);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxRope::AfterWorldLoad()
{
	//The rope entity is only created once!
	if(mbRopeCreated)
	{
		if(mpRopeGfx==NULL && mpRope==NULL)
		{
			mpRope = mpMap->GetPhysicsWorld()->GetRopeFromUniqueID(mlID);
			if(mpRope==NULL) Error("Cannot find rope physics entity with ID %d for lux rope '%s'\n", mlID, msName.c_str());

			mpRopeGfx = mpMap->GetWorld()->GetRopeEntityFromUniqueID(mlID);
			if(mpRopeGfx==NULL) Error("Cannot find rope gfx entity with ID %d for lux rope '%s'\n", mlID, msName.c_str());
		}

		return; 
	}

	mbRopeCreated = true;

	/////////////////////////
	// Get End Position
	cLuxNode_Pos *pPosNode = mpMap->GetPosNode(msEndPosNode);
	if(pPosNode == NULL)
	{
		Error("Could not find end pos '%s' for rope '%s'\n", msEndPosNode.c_str(), msName.c_str());
		return;
	}
	
	mvEndPos = pPosNode->GetPosition();

	/////////////////////////
	// Get Bodies
	iPhysicsBody *pStartBody = NULL;
	if(msStartBody != "") pStartBody = GetBody(msStartBody);
	
	iPhysicsBody *pEndBody = NULL;
	if(msEndBody != "") pEndBody = GetBody(msEndBody);

	/////////////////////////
	// Create Physics Rope
	mpRope = mpMap->GetPhysicsWorld()->CreateRope(msName, mvStartPos, mvEndPos);

	mpRope->SetMinTotalLength(mfMinTotalLength);
	mpRope->SetMaxTotalLength(mfMaxTotalLength);
	mpRope->SetSegmentLength(mfSegmentLength);
	mpRope->SetDamping(mfDamping);
	mpRope->SetStrength(mfStrength);
	mpRope->SetStiffness(mfStiffness);
	mpRope->SetMotorSound(msSound);
	mpRope->SetMotorSoundStartSpeed(mfSoundStartSpeed);
	mpRope->SetMotorSoundStopSpeed(mfSoundStopSpeed);
	mpRope->SetAutoMoveActive(mbAutoMove);
	mpRope->SetAutoMoveAcc(mfAutoMoveAcc);
	mpRope->SetAutoMoveMaxSpeed(mfAutoMoveMaxSpeed);

	mpRope->SetUniqueID(mlID);

	//Start body
	if(pStartBody!=NULL)
	{
		mpRope->SetAttachedStartBody(pStartBody);
		if(pStartBody->GetMass() != 0)
			mpRope->GetStartParticle()->SetInvMass(1.0f);
	}
	
	//End body
	if(pEndBody!=NULL)
	{
		mpRope->SetAttachedEndBody(pEndBody);
	}
	
	/////////////////////////
	// Create Graphical Rope
	cResources *pResources = gpBase->mpEngine->GetResources();

	int lMaxSegments = (int)(mfMaxTotalLength / mfSegmentLength)+4;
	mpRopeGfx = mpMap->GetWorld()->CreateRopeEntity(msName, mpRope, lMaxSegments);

	mpRopeGfx->SetMaterial(pResources->GetMaterialManager()->CreateMaterial(msMaterial));
	mpRopeGfx->SetRadius(mfRadius);
	mpRopeGfx->SetLengthTileAmount(mfLengthTileAmount);
	mpRopeGfx->SetLengthTileSize(mfLengthTileSize);

	mpRopeGfx->SetUniqueID(mlID);
}

//-----------------------------------------------------------------------

void cLuxRope::OnSetActive(bool abX)
{

}

//-----------------------------------------------------------------------

void cLuxRope::OnUpdate(float afTimeStep)
{

}

//-----------------------------------------------------------------------

void cLuxRope::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iPhysicsBody* cLuxRope::GetBody(const tString& asName)
{
	iPhysicsBody *pBody = mpMap->GetPhysicsWorld()->GetBody(asName);
	if(pBody==NULL)
	{
		iLuxEntity *pEntity = mpMap->GetEntityByName(asName, eLuxEntityType_Prop);
        if(pEntity==NULL)
		{
			Error("Could not find body or entity with the name '%s'\n", asName.c_str());
			return NULL;
		}
		iLuxProp *pProp = static_cast<iLuxProp*>(pEntity);
		pBody = pProp->GetMainBody();
	}
	return pBody;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxRope_SaveData, iLuxEntity_SaveData)
//mvSize
kSerializeVar(mbRopeCreated,eSerializeType_Bool)

kSerializeVar(mvStartPos,eSerializeType_Vector3f)
kSerializeVar(mvEndPos,eSerializeType_Vector3f)

kSerializeVar(msEndPosNode,eSerializeType_String)
kSerializeVar(msStartBody,eSerializeType_String)
kSerializeVar(msEndBody,eSerializeType_String)

kSerializeVar(mfMinTotalLength,eSerializeType_Float32)
kSerializeVar(mfMaxTotalLength,eSerializeType_Float32)
kSerializeVar(mfSegmentLength,eSerializeType_Float32)
kSerializeVar(mfDamping,eSerializeType_Float32)
kSerializeVar(mfStrength,eSerializeType_Float32)
kSerializeVar(mfStiffness,eSerializeType_Float32)
kSerializeVar(msMaterial,eSerializeType_String)
kSerializeVar(mfRadius,eSerializeType_Float32)
kSerializeVar(mfLengthTileAmount,eSerializeType_Float32)
kSerializeVar(mfLengthTileSize,eSerializeType_Float32)
kSerializeVar(msSound,eSerializeType_String)
kSerializeVar(mfSoundStartSpeed,eSerializeType_Float32)
kSerializeVar(mfSoundStopSpeed,eSerializeType_Float32)
kSerializeVar(mbAutoMove,eSerializeType_Bool)
kSerializeVar(mfAutoMoveAcc,eSerializeType_Float32)
kSerializeVar(mfAutoMoveMaxSpeed,eSerializeType_Float32)
kEndSerialize()


//-----------------------------------------------------------------------

iLuxEntity* cLuxRope_SaveData::CreateEntity(cLuxMap *apMap)
{
	cLuxRope *pRope = hplNew(cLuxRope, (msName, mlID,apMap));
	apMap->AddEntity(pRope);

	return pRope;
}

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxRope::CreateSaveData()
{
	return hplNew(cLuxRope_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxRope::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::SaveToSaveData(apSaveData);
	cLuxRope_SaveData *pData = static_cast<cLuxRope_SaveData*>(apSaveData);

	////////////////
	//Variables
	kCopyToVar(pData, mbRopeCreated);

	////////////////
	//Properties
	kCopyToVar(pData, mvStartPos);
	kCopyToVar(pData, mvEndPos);

	kCopyToVar(pData, msEndPosNode); 
	kCopyToVar(pData, msStartBody);
	kCopyToVar(pData, msEndBody);

	kCopyToVar(pData, mfMinTotalLength);
	kCopyToVar(pData,  mfMaxTotalLength);
	kCopyToVar(pData,  mfSegmentLength);
	kCopyToVar(pData,  mfDamping);
	kCopyToVar(pData,  mfStrength);
	kCopyToVar(pData,  mfStiffness);
	kCopyToVar(pData,  msMaterial);
	kCopyToVar(pData,  mfRadius);
	kCopyToVar(pData,  mfLengthTileAmount);
	kCopyToVar(pData,  mfLengthTileSize);
	kCopyToVar(pData,  msSound);
	kCopyToVar(pData,  mfSoundStartSpeed);
	kCopyToVar(pData,  mfSoundStopSpeed);
	kCopyToVar(pData,  mbAutoMove);
	kCopyToVar(pData,  mfAutoMoveAcc);
	kCopyToVar(pData,  mfAutoMoveMaxSpeed);
}

//-----------------------------------------------------------------------

void cLuxRope::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	super_class::LoadFromSaveData(apSaveData);
	cLuxRope_SaveData *pData = static_cast<cLuxRope_SaveData*>(apSaveData);

	//(pData, mvSize);
	////////////////
	//Variables
	kCopyFromVar(pData, mbRopeCreated);

	////////////////
	//Properties
	kCopyFromVar(pData, mvStartPos);
	kCopyFromVar(pData, mvEndPos);

	kCopyFromVar(pData, msEndPosNode); 
	kCopyFromVar(pData, msStartBody);
	kCopyFromVar(pData, msEndBody);

	kCopyFromVar(pData, mfMinTotalLength);
	kCopyFromVar(pData,  mfMaxTotalLength);
	kCopyFromVar(pData,  mfSegmentLength);
	kCopyFromVar(pData,  mfDamping);
	kCopyFromVar(pData,  mfStrength);
	kCopyFromVar(pData,  mfStiffness);
	kCopyFromVar(pData,  msMaterial);
	kCopyFromVar(pData,  mfRadius);
	kCopyFromVar(pData,  mfLengthTileAmount);
	kCopyFromVar(pData,  mfLengthTileSize);
	kCopyFromVar(pData,  msSound);
	kCopyFromVar(pData,  mfSoundStartSpeed);
	kCopyFromVar(pData,  mfSoundStopSpeed);
	kCopyFromVar(pData,  mbAutoMove);
	kCopyFromVar(pData,  mfAutoMoveAcc);
	kCopyFromVar(pData,  mfAutoMoveMaxSpeed);
}

//-----------------------------------------------------------------------

void cLuxRope::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
	cLuxRope_SaveData *pData = static_cast<cLuxRope_SaveData*>(apSaveData);
}

//-----------------------------------------------------------------------

