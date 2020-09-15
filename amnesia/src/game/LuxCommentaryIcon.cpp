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

#include "LuxCommentaryIcon.h"

#include "LuxMap.h"
#include "LuxEffectHandler.h"


//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxCommentaryIconLoader::cLuxCommentaryIconLoader(const tString& asName) : cEntityLoader_Object(asName)
{
	mfDefaultMaxFocusDistance = gpBase->mpGameCfg->GetFloat("Player_Interaction", "Commentary_MaxFocusDist",0);
}

//-----------------------------------------------------------------------

void cLuxCommentaryIconLoader::BeforeLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars)
{

}

//-----------------------------------------------------------------------

void cLuxCommentaryIconLoader::AfterLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars)
{
	cLuxMap *pMap = gpBase->mpCurrentMapLoading;
	if(pMap==NULL)	return;

	cLuxCommentaryIcon *pCommentaryIcon = hplNew(cLuxCommentaryIcon, (mpEntity->GetName(), mlID,pMap));

	//////////////////////////////
	// Set data
	pCommentaryIcon->msFileName = msFileName;
	pCommentaryIcon->m_mtxOnLoadTransform = a_mtxTransform;
	pCommentaryIcon->mvOnLoadScale = mvScale;

	pCommentaryIcon->mvBodies = mvBodies;
	pCommentaryIcon->mpMeshEntity = mpEntity;

	pCommentaryIcon->mvLights = mvLights;
	pCommentaryIcon->mvParticleSystems = mvParticleSystems;
	pCommentaryIcon->mvBillboards = mvBillboards;

	pCommentaryIcon->mfMaxFocusDistance = mfDefaultMaxFocusDistance;

	//Set body userdata
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		iPhysicsBody *pBody = mvBodies[i];
		iLuxEntity *pEnt = pCommentaryIcon;
		pBody->SetUserData(pEnt);
	}

	//////////////////////////////
	// User vars
	pCommentaryIcon->msRotateSubMesh = GetVarString("RotateSubMesh","");

	//////////////////////////////
	// Load Instance Variables
	if(apInstanceVars)
	{
		tString sFile = apInstanceVars->GetVarString("File", "");
		pCommentaryIcon->LoadCommentaryFile(sFile);
	}


	pCommentaryIcon->SetupAfterLoad(apWorld);

	//Add to the current map
	pMap->AddEntity(pCommentaryIcon);

	//Always disable by default
	pCommentaryIcon->SetActive(false);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxCommentaryIcon::cLuxCommentaryIcon(const tString &asName, int alID, cLuxMap *apMap) : iLuxEntity(asName,alID,apMap, eLuxEntityType_CommentaryIcon)
{
	mbPlayingSound = false;
	mvRotAngle =0;
}

//-----------------------------------------------------------------------

cLuxCommentaryIcon::~cLuxCommentaryIcon()
{
	cWorld *pWorld = mpMap->GetWorld();
	iPhysicsWorld *pPhysicsWorld = pWorld->GetPhysicsWorld();
	
	////////////////////
	// Destroy physics
	{
		//Bodies
		for(size_t i=0; i<mvBodies.size(); ++i)
		{
			iPhysicsBody *pBody = mvBodies[i];
				
			pWorld->GetPhysicsWorld()->DestroyBody(pBody);
		}
	}

	////////////////////
	// Destroy graphics
	{
		//Mesh entity
		if(mpMeshEntity) pWorld->DestroyMeshEntity(mpMeshEntity);

		//Lights
		for(size_t i=0; i<mvLights.size(); ++i) pWorld->DestroyLight(mvLights[i]);
		
		//Particle systems
		for(size_t i=0; i<mvParticleSystems.size(); ++i) 
		{
			cParticleSystem *pPS = mvParticleSystems[i];
			if(pPS && pWorld->ParticleSystemExists(pPS)) pPS->Kill();
		}

		//Billboards
		for(size_t i=0; i<mvBillboards.size(); ++i)	pWorld->DestroyBillboard(mvBillboards[i]);
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxCommentaryIcon::SetupAfterLoad(cWorld *apWorld)
{
	mvSubMeshMatrix.resize(mpMeshEntity->GetSubMeshEntityNum());
	for(int i=0; i<mpMeshEntity->GetSubMeshEntityNum(); ++i)
	{
		cSubMeshEntity *pSubEnt = mpMeshEntity->GetSubMeshEntity(i);

		mvSubMeshMatrix[i] = pSubEnt->GetLocalMatrix();
	}
}

//-----------------------------------------------------------------------

void cLuxCommentaryIcon::OnUpdate(float afTimeStep)
{
	if(mbPlayingSound)
	{
		mvRotAngle.y += afTimeStep*2.5f;
	}
	else
	{
		if(mvRotAngle.y != 0)
		{
			mvRotAngle.y = cMath::Wrap(mvRotAngle.y, 0, kPif);
			mvRotAngle.y += afTimeStep*2.5f;
			if(mvRotAngle.y>kPif) mvRotAngle.y =0;
		}
	}

	for(int i=0; i<mpMeshEntity->GetSubMeshEntityNum(); ++i)
	{
		cSubMeshEntity *pSubEnt = mpMeshEntity->GetSubMeshEntity(i);
		cSubMesh *pSubMesh = pSubEnt->GetSubMesh();
		if(pSubMesh->GetName() != msRotateSubMesh) continue;
		
		cMatrixf mtxTrans = cMath::MatrixMul(mvSubMeshMatrix[i], cMath::MatrixRotate(mvRotAngle, eEulerRotationOrder_XYZ));
        pSubEnt->SetMatrix(mtxTrans);
	}
}

//-----------------------------------------------------------------------

bool cLuxCommentaryIcon::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
	if(mbPlayingSound)
	{
		gpBase->mpEffectHandler->GetPlayCommentary()->Stop();
	}
	else
	{
		gpBase->mpEffectHandler->GetPlayCommentary()->Start(msTalker, msTopic, msSoundFile, mlID);
	}
	SetPlayingSound(!mbPlayingSound);
	
	return true;
}

//-----------------------------------------------------------------------

tWString cLuxCommentaryIcon::GetFocusText()
{
	return cString::To16Char(msTalker + "\n\""+msTopic+"\"");
}

//-----------------------------------------------------------------------

void cLuxCommentaryIcon::SetPlayingSound(bool abX)
{
	if(mbPlayingSound ==abX) return;

	mbPlayingSound = abX;

    if(mbPlayingSound)
	{
		cParticleSystem *pPS = mpMap->GetWorld()->CreateParticleSystem("_commentary_icon_ps","ps_commentary_icon.ps",1);
		if(pPS)
		{
			pPS->SetPosition(mvBodies[0]->GetWorldPosition());
			pPS->SetIsSaved(false);
		}
	}
	else
	{
		cParticleSystem *pPS = mpMap->GetWorld()->GetParticleSystem("_commentary_icon_ps");
		if(pPS)
		{
			pPS->Kill();
		}
	}
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxCommentaryIcon::OnSetActive(bool abX)
{	
	///////////////
	//Bodies
	for(size_t i=0; i<mvBodies.size(); ++i)
	{
		mvBodies[i]->SetActive(abX);
	}

	///////////////////
	//Mesh entity
	if(mpMeshEntity)
	{
		mpMeshEntity->SetActive(abX);
		mpMeshEntity->SetVisible(abX);
	}

	///////////////////
	//Lights
	for(size_t i=0; i<mvLights.size(); ++i)
	{
		mvLights[i]->SetVisible(abX);
		mvLights[i]->SetActive(abX);
	}

	///////////////////
	//Particle systems
	for(size_t i=0; i<mvParticleSystems.size(); ++i) 
	{
		cParticleSystem *pPS = mvParticleSystems[i];
		if(pPS)
		{
			pPS->SetVisible(abX);
			pPS->SetActive(abX);
		}
	}

	///////////////////
	//Billboards
	for(size_t i=0; i<mvBillboards.size(); ++i)
	{
		mvBillboards[i]->SetActive(abX);
		mvBillboards[i]->SetVisible(abX);
	}
}


//-----------------------------------------------------------------------

void cLuxCommentaryIcon::LoadCommentaryFile(const tString& asFile)
{
	tString sFile = cString::SetFileExt(asFile,"comment");
	cResources *pResources = gpBase->mpEngine->GetResources();

	iXmlDocument *pXmlDoc = pResources->LoadXmlDocument(sFile);
	if(pXmlDoc==NULL)
	{
		Error("Could not load flashback file: '%s'\n", sFile.c_str());
		return;
	}

	msTalker = pXmlDoc->GetAttributeString("Talker","");
	msTopic = pXmlDoc->GetAttributeString("Topic","");
	msSoundFile = pXmlDoc->GetAttributeString("SoundFile","");
	
	pResources->DestroyXmlDocument(pXmlDoc);
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxCommentaryIcon::CreateSaveData()
{
	return hplNew(cLuxCommentaryIcon_SaveData, ());
}

//-----------------------------------------------------------------------

kBeginSerialize(cLuxCommentaryIcon_SaveData, iLuxEntity_SaveData)

kSerializeVar(msFileName, eSerializeType_String)
kSerializeVar(m_mtxOnLoadTransform, eSerializeType_Matrixf)
kSerializeVar(mvOnLoadScale, eSerializeType_Vector3f)

kSerializeVar(msTalker,eSerializeType_String)
kSerializeVar(msTopic,eSerializeType_String)
kSerializeVar(msSoundFile,eSerializeType_String)

kEndSerialize()

//-----------------------------------------------------------------------

cLuxCommentaryIcon_SaveData::~cLuxCommentaryIcon_SaveData()
{
}

//-----------------------------------------------------------------------

iLuxEntity* cLuxCommentaryIcon_SaveData::CreateEntity(cLuxMap *apMap)
{
	cWorld *pWorld = apMap->GetWorld();
	apMap->ResetLatestEntity();
	pWorld->CreateEntity(msName,m_mtxOnLoadTransform, msFileName, mlID, true, mvOnLoadScale);

	return apMap->GetLatestEntity();
}

//-----------------------------------------------------------------------

void cLuxCommentaryIcon::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::SaveToSaveData(apSaveData);
	cLuxCommentaryIcon_SaveData *pData = static_cast<cLuxCommentaryIcon_SaveData*>(apSaveData);

    ///////////////////////
	//Properties
	kCopyToVar(pData, msFileName);
	kCopyToVar(pData, m_mtxOnLoadTransform);
	kCopyToVar(pData, mvOnLoadScale);
	kCopyToVar(pData, msTalker);
	kCopyToVar(pData, msTopic);
	kCopyToVar(pData, msSoundFile);
	
}

//-----------------------------------------------------------------------

void cLuxCommentaryIcon::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
	///////////////////////
	// Init
	super_class::LoadFromSaveData(apSaveData);
	cLuxCommentaryIcon_SaveData *pData = static_cast<cLuxCommentaryIcon_SaveData*>(apSaveData);

		///////////////////////
	//CommentaryIconerties
	kCopyFromVar(pData, m_mtxOnLoadTransform);
	kCopyFromVar(pData, mvOnLoadScale);
	kCopyFromVar(pData, msTalker);
	kCopyFromVar(pData, msTopic);
	kCopyFromVar(pData, msSoundFile);
	
}

//-----------------------------------------------------------------------

void cLuxCommentaryIcon::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
	super_class::SetupSaveData(apSaveData);
	cLuxCommentaryIcon_SaveData *pData = static_cast<cLuxCommentaryIcon_SaveData*>(apSaveData);
}

//-----------------------------------------------------------------------

