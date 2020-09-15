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

#include "scene/ParticleSystem.h"

#include "impl/tinyXML/tinyxml.h"

#include "system/String.h"

#include "scene/ParticleEmitter_UserData.h"

#include "resources/Resources.h"
#include "resources/LowLevelResources.h"
#include "resources/XmlDocument.h"
#include "resources/ParticleManager.h"

#include "engine/Engine.h"

#include "scene/Scene.h"
#include "scene/World.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CREATOR
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cParticleSystemData::cParticleSystemData(const tString &asName,
												cResources* apResources,cGraphics *apGraphics)
												: iResourceBase(asName, _W(""),0)
	{
		mpResources = apResources;
		mpGraphics = apGraphics;
	}
	
	//-----------------------------------------------------------------------

	cParticleSystemData::~cParticleSystemData()
	{
		STLDeleteAll(mvEmitterData);
	}

	//-----------------------------------------------------------------------

	void cParticleSystemData::AddEmitterData(iParticleEmitterData *apData)
	{
		mvEmitterData.push_back(apData);
	}

	//-----------------------------------------------------------------------

	cParticleSystem* cParticleSystemData::Create(tString asName, cVector3f avSize)
	{
		if(mvEmitterData.empty())
		{
			Warning("Particle system '%s' has no emitters.\n",msName.c_str());
			return NULL;
		}

		cParticleSystem *pPS = hplNew( cParticleSystem, (asName,this,mpResources,mpGraphics) );
		
		for(size_t i=0; i<mvEmitterData.size(); ++i)
		{
			///////////////////////////
			// Create and add
			iParticleEmitter *pPE = static_cast<iParticleEmitter*>(mvEmitterData[i]->Create(asName, avSize));
			pPS->AddEmitter(pPE);
			pPE->SetSystem(pPS);
		}

		return pPS;
	}

	//-----------------------------------------------------------------------

	bool cParticleSystemData::LoadFromFile(const tWString &asFile)
	{
		SetFullPath(asFile);

		iXmlDocument* pXmlDoc = mpResources->GetLowLevel()->CreateXmlDocument();
		if(pXmlDoc->CreateFromFile(asFile)==false)
		{
			hplDelete(pXmlDoc);
			return false;
		}

		bool bRet = LoadFromElement(pXmlDoc);

		hplDelete(pXmlDoc);
		return bRet;
		/*
		
		FILE *pFile = _wfopen(asFile.c_str(),_W("rb"));
		if(pFile==NULL) return false;

		TiXmlDocument* pXmlDoc = hplNew( TiXmlDocument,() );
		
		if(pXmlDoc->LoadFile(pFile)==false)
		{
			Warning("Couldn't open XML file %s\n",cString::To8Char(asFile).c_str());
			fclose(pFile);
			hplDelete(pXmlDoc);
			return false;
		}
		fclose(pFile);
		
		TiXmlElement *pRootElem = pXmlDoc->RootElement();

		bool bRet = LoadFromElement(pRootElem);

        hplDelete(pXmlDoc);
		return bRet;*/
	}

	//-----------------------------------------------------------------------

	bool cParticleSystemData::LoadFromElement(cXmlElement* apElement)
	{
		if(apElement->GetValue()!="ParticleSystem")
			return false;

		cXmlNodeListIterator it = apElement->GetChildIterator();
		while(it.HasNext())
		{
			cXmlElement* pElem = it.Next()->ToElement();
			if(pElem->GetValue()!="ParticleEmitter")
				continue;

			cParticleEmitterData_UserData *pPE = hplNew( cParticleEmitterData_UserData,("",	mpResources,mpGraphics) );
			pPE->LoadFromElement(pElem);

			mvEmitterData.push_back(pPE);
		}

		/*cXmlElement *pEmitterElem = apElement->GetFirstElement("ParticleEmitter");
		for(; pEmitterElem != NULL; pEmitterElem = pEmitterElem->NextSiblingElement("ParticleEmitter"))
		{
			cParticleEmitterData_UserData *pPE = hplNew( cParticleEmitterData_UserData,("",
																	mpResources,mpGraphics) );

            pPE->LoadFromElement(pEmitterElem);
			
			mvEmitterData.push_back(pPE);
		}*/

		return (mvEmitterData.empty()==false);
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cParticleSystem::cParticleSystem(const tString asName, 
										cParticleSystemData *apData,
										cResources *apResources, cGraphics *apGraphics)
		: iEntity3D(asName)
	{
		mpResources = apResources;
		mpGraphics = apGraphics;
		mpParticleManager = NULL;
		mpData = apData;

		mbRemoveWhenDead = true;

		mbIsVisible = true;

		mbFirstUpdate = true;

		mColor = cColor(1,1);
		mbFadeAtDistance = false;
		mfMinFadeDistanceStart = 2;
		mfMinFadeDistanceEnd = 1;
		mfMaxFadeDistanceStart = 100;
		mfMaxFadeDistanceEnd = 110;
	}

	//-----------------------------------------------------------------------

	cParticleSystem::~cParticleSystem()
	{
		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			hplDelete(mvEmitters[i]);
		}
		if(mpParticleManager) mpParticleManager->Destroy(mpData);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void cParticleSystem::SetVisible(bool abVisible)
	{
		if(mbIsVisible == abVisible) return;

		mbIsVisible = abVisible;
		
		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			mvEmitters[i]->SetVisible(mbIsVisible);
		}
		
	}
	
	//-----------------------------------------------------------------------

	bool cParticleSystem::IsDead()
	{
		size_t lCount =0;

		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			iParticleEmitter *pPE = mvEmitters[i];

			if(pPE->IsDead()) lCount++;
		}

		if(lCount == mvEmitters.size()) return true;

		return false;
	}

	bool cParticleSystem::IsDying()
	{
		size_t lCount =0;

		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			iParticleEmitter *pPE = mvEmitters[i];

			if(pPE->IsDying()) lCount++;
		}

		if(lCount == mvEmitters.size()) return true;

		return false;
	}

	//-----------------------------------------------------------------------

	void cParticleSystem::Kill()
	{
		//SetIsSaved(false);
		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			iParticleEmitter *pPE = mvEmitters[i];

			pPE->Kill();
		}
	}
	
	//-----------------------------------------------------------------------

	void cParticleSystem::KillInstantly()
	{
		//SetIsSaved(false);
		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			iParticleEmitter *pPE = mvEmitters[i];

			pPE->KillInstantly();
		}
	}

	//-----------------------------------------------------------------------
	
	void cParticleSystem::UpdateLogic(float afTimeStep)
	{
		if(IsActive()==false) return;

		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			iParticleEmitter *pPE = mvEmitters[i];

			//////////////////////////
			//Warm Up
			if(mbFirstUpdate)
			{
				iParticleEmitterData *pData =  mpData->GetEmitterData((int)i);
				
				if(pData->GetWarmUpTime() >0)
				{
					float fTime = pData->GetWarmUpTime();
					float fStepSize = 1.0f /pData->GetWarmUpStepsPerSec();

					while(fTime >0)
					{
						pPE->UpdateLogic(fStepSize);
						fTime -= fStepSize;
					}
				}
			}
			
			//////////////////////////
			//Update
			pPE->UpdateLogic(afTimeStep);
		}

		//No loner first time!
		mbFirstUpdate = false;
	}

	//-----------------------------------------------------------------------

	void cParticleSystem::AddEmitter(iParticleEmitter* apEmitter)
	{
		mvEmitters.push_back(apEmitter);

		AddChild(apEmitter);
	}
	
	//-----------------------------------------------------------------------

	iParticleEmitter* cParticleSystem::GetEmitter(int alIdx)
	{
		return mvEmitters[alIdx];
	}
	
	//-----------------------------------------------------------------------

	int cParticleSystem::GetEmitterNum()
	{
		return (int)mvEmitters.size();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	//-----------------------------------------------------------------------
}
