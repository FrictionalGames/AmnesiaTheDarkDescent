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

#include "LuxTypes.h"

#include "LuxEntity.h"
#include "LuxArea.h"
#include "LuxProp.h"
#include "LuxMap.h"

//////////////////////////////////////////////////////////////////////////
// TYPE CONVERSIONS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool LuxIsCorrectType(iLuxEntity *apEntity, eLuxEntityType aType, int alSubType)
{
	if(aType == eLuxEntityType_LastEnum) return true;

	eLuxEntityType entityType = apEntity->GetEntityType();
	if(entityType != aType) return false;

	if(alSubType<0) return true;

	if(entityType == eLuxEntityType_Area)
	{
		iLuxArea *pArea = static_cast<iLuxArea*>(apEntity);
		return pArea->GetAreaType() == (eLuxAreaType)alSubType;
	}
	else
	{
		iLuxProp *pProp = static_cast<iLuxProp*>(apEntity);
		return pProp->GetPropType() == (eLuxPropType)alSubType;
	}
}
//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// ID PAIR
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxIdPair)
kSerializeVar(mlParentId, eSerializeType_Int32)
kSerializeVar(mlChildId, eSerializeType_Int32)
kEndSerialize()

//-----------------------------------------------------------------------

cLuxIdPair LuxGetIdPairFromBody(iPhysicsBody *apBody)
{
	cLuxIdPair idPair;
	if(apBody==NULL)
	{
		idPair.mlParentId = -1;
		idPair.mlChildId = -1;

		return idPair;
	}
	
	idPair.mlChildId = apBody->GetUniqueID();
	if(apBody->GetUserData())
	{
		iLuxEntity *pEntity = (iLuxEntity*)apBody->GetUserData();
		idPair.mlParentId = pEntity->GetID();
	}
	else
	{
		idPair.mlParentId = -1;
	}

	return idPair;
}

//////////////////////////////////////////////////////////////////////////
// UPDATEABLE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxUpdateable::ClearFonts()
{
	cFontManager* pFontManager = gpBase->mpEngine->GetResources()->GetFontManager();
	for(int i=0;i<(int)mvFonts.size();++i)
		pFontManager->Destroy(mvFonts[i]);

	mvFonts.clear();

	OnClearFonts();
}

//-----------------------------------------------------------------------


iFontData* iLuxUpdateable::LoadFont(const tString& asFile)
{
	iFontData* pFont = NULL;
	pFont = gpBase->mpEngine->GetResources()->GetFontManager()->CreateFontData(asFile);

	if(pFont)
		mvFonts.push_back(pFont);

	return pFont;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// COLLISION CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxCollideCallbackContainer::iLuxCollideCallbackContainer()
{
	mbUpdatingCollideCallbacks = false;
}

void iLuxCollideCallbackContainer::DestroyCollideCallbacks()
{
	for(tLuxCollideCallbackListIt it = mlstCollideCallbacks.begin(); it != mlstCollideCallbacks.end(); ++it)
	{
		cLuxCollideCallback *pCallback = *it;
		iLuxEntity *pEntity = pCallback->mpCollideEntity;

		pEntity->RemoveCollideCallbackParent(this);
		hplDelete(pCallback);
	}
	mlstCollideCallbacks.clear();
	mlstDeleteCallbacks.clear();//Should not be needed, but what the heck...
}

//-----------------------------------------------------------------------

void iLuxCollideCallbackContainer::CheckCollisionCallback(const tString& asName, cLuxMap *apMap)
{
	mbUpdatingCollideCallbacks = true;
    
	/////////////////////
	//Iterate the collide callbacks
	for(tLuxCollideCallbackListIt it = mlstCollideCallbacks.begin(); it != mlstCollideCallbacks.end(); ++it)
	{
		cLuxCollideCallback *pCallback = *it;
		iLuxEntity *pEntity = pCallback->mpCollideEntity;
		bool bCollide=false;

		if(pEntity==NULL) continue;
		if(pEntity->IsActive()==false) continue;

		bCollide = CheckEntityCollision(pEntity, apMap);

		/////////////////////
		//Handle collision
		if( (bCollide && pCallback->mbColliding==false) || (bCollide==false && pCallback->mbColliding) )
		{
			int lState = bCollide ? 1 : -1;
            pCallback->mbColliding = bCollide;
			if(lState == pCallback->mlStates || pCallback->mlStates==0)
			{
				tString sCommand = pCallback->msCallbackFunc+"(\"" + asName + "\", \""+ pEntity->GetName()+"\", "+cString::ToString(lState)+")" ;
				apMap->RunScript(sCommand);
			
				///////////////////////
				// Auto remove
				if(pCallback->mbDeleteWhenColliding)
				{
					// Check so it is not already in delete list
					bool bCallbackExistInDeleteList = false;
					for(tLuxCollideCallbackListIt it = mlstDeleteCallbacks.begin(); it != mlstDeleteCallbacks.end(); ++it)
					{
						cLuxCollideCallback *pTestCallback = *it;
						if(pTestCallback == pCallback) 
						{
							bCallbackExistInDeleteList = true;
							break;
						}
					}
					
					// Add to delete list (if not already there)
					if(bCallbackExistInDeleteList==false)
					{
						mlstDeleteCallbacks.push_back(pCallback);
					}
				}
			}
		}
	}

	mbUpdatingCollideCallbacks = false;

	/////////////////////
	//Delete callbacks
	for(tLuxCollideCallbackListIt it = mlstDeleteCallbacks.begin(); it != mlstDeleteCallbacks.end(); ++it)
	{
		cLuxCollideCallback *pCallback = *it;
		
		pCallback->mpCollideEntity->RemoveCollideCallbackParent(this);
		
		STLFindAndDelete(mlstCollideCallbacks, pCallback);
	}
	mlstDeleteCallbacks.clear();
}

//-----------------------------------------------------------------------

bool iLuxCollideCallbackContainer::CheckEntityCollision(iLuxEntity*apEntity, cLuxMap *apMap)
{
	iPhysicsWorld *pPhysicsWorld =apMap->GetPhysicsWorld();

	cCollideData collideData;
	collideData.SetMaxSize(1);

    /////////////////////
	//Iterate bodies and check for collision
	for(int i=0; i<GetBodyNum(); ++i)
	{
		for(int j=0; j<apEntity->GetBodyNum(); ++j)
		{
			iPhysicsBody *pBodyA = GetBody(i);
			iPhysicsBody *pBodyB = apEntity->GetBody(i);

			if(cMath::CheckBVIntersection(*pBodyA->GetBoundingVolume(), *pBodyB->GetBoundingVolume()))
			{
				bool bCollide = pPhysicsWorld->CheckShapeCollision(	pBodyA->GetShape(), pBodyA->GetLocalMatrix(), 
																	pBodyB->GetShape(), pBodyB->GetLocalMatrix(),
																	collideData,1,false);
				if(bCollide) return true;
			}
		}
	}

	return false;
}

//-----------------------------------------------------------------------

void iLuxCollideCallbackContainer::AddCollideCallback(iLuxEntity *apEntity, const tString& asCallbackFunc, bool abRemoveAtCollide, int alStates)
{
	////////////////////////////////
	//Check if exists!
	for(tLuxCollideCallbackListIt it = mlstCollideCallbacks.begin(); it != mlstCollideCallbacks.end(); ++it)
	{
		cLuxCollideCallback *pCallback = *it;
		if(pCallback->mpCollideEntity == apEntity)
		{
			Warning("A callback with entity '%s' already exists!\n", apEntity->GetName().c_str());	
			return;
		}
	}

	////////////////////////////////
	// Create and add callback
	cLuxCollideCallback *pCallback = hplNew(cLuxCollideCallback, ());
	
	pCallback->mpCollideEntity = apEntity;
	pCallback->msCallbackFunc = asCallbackFunc;
	pCallback->mbDeleteWhenColliding = abRemoveAtCollide;
	pCallback->mbColliding = false;
	pCallback->mlStates = alStates;

	apEntity->AddCollideCallbackParent(this);

	mlstCollideCallbacks.push_back(pCallback);
}

//-----------------------------------------------------------------------

void iLuxCollideCallbackContainer::RemoveCollideCallback(cLuxCollideCallback *apCallback)
{
	if(mbUpdatingCollideCallbacks)
	{
		mlstDeleteCallbacks.push_back(apCallback);	
	}
	else
	{
		apCallback->mpCollideEntity->RemoveCollideCallbackParent(this);
		
		STLFindAndDelete(mlstCollideCallbacks, apCallback);
	}
}

//-----------------------------------------------------------------------

void iLuxCollideCallbackContainer::RemoveCollideCallback(const tString& asEntityName)
{
	for(tLuxCollideCallbackListIt it = mlstCollideCallbacks.begin(); it != mlstCollideCallbacks.end(); ++it)
	{
		cLuxCollideCallback *pCallback = *it;
		if(pCallback->mpCollideEntity->GetName() == asEntityName)
		{
			RemoveCollideCallback(pCallback);	
			break;
		}
	}
}

//-----------------------------------------------------------------------

void iLuxCollideCallbackContainer::RemoveCollideCallbackInstantly(iLuxEntity *apEntity)
{
	for(tLuxCollideCallbackListIt it = mlstCollideCallbacks.begin(); it != mlstCollideCallbacks.end(); )
	{
		cLuxCollideCallback *pCallback = *it;
		if(pCallback->mpCollideEntity == apEntity)
		{
			hplDelete(pCallback);
			it = mlstCollideCallbacks.erase(it);
		}
		else
		{
			++it;
		}
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// ALPHA FADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxAlphaFader::Reset()
{
	mbActive = false;
	mfAlpha =0;
}

//-----------------------------------------------------------------------

void cLuxAlphaFader::Update(float afTimeStep)
{
	if(mbActive)
	{
		mfAlpha += mfFadeInSpeed*afTimeStep;
		if(mfAlpha > 1.0f) mfAlpha = 1.0f;
	}
	else
	{
		mfAlpha -= mfFadeOutSpeed*afTimeStep;
		if(mfAlpha < 0) mfAlpha = 0;
	}
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// HEAD POS ADD
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxHeadPosAdd)
kSerializeVar(mvAdd, eSerializeType_Vector3f)
kSerializeVar(mbMoving, eSerializeType_Bool)
kSerializeVar(mvMoveGoal, eSerializeType_Vector3f)
kSerializeVar(mfMoveSpeed, eSerializeType_Float32)
kSerializeVar(mfMoveSlowDownDist, eSerializeType_Float32)
kEndSerialize()

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// EVENT TIMER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxEventTimer)
kSerializeVar(msName, eSerializeType_String)
kSerializeVar(msFunction, eSerializeType_String)
kSerializeVar(mfCount, eSerializeType_Float32)
kSerializeVar(mbDestroyMe, eSerializeType_Bool)
kEndSerialize()

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// COMBINE ITEMS CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxCombineItemsCallback)
kSerializeVar(msName, eSerializeType_String)
kSerializeVar(msItemA, eSerializeType_String)
kSerializeVar(msItemB, eSerializeType_String)
kSerializeVar(msFunction, eSerializeType_String)
kSerializeVar(mbAutoDestroy, eSerializeType_Bool)
kEndSerialize()

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// USE ITEM CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxUseItemCallback)
kSerializeVar(msName, eSerializeType_String)
kSerializeVar(msItem, eSerializeType_String)
kSerializeVar(msEntity, eSerializeType_String)
kSerializeVar(msFunction, eSerializeType_String)
kSerializeVar(mbAutoDestroy, eSerializeType_Bool)
kEndSerialize()

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// NOTE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxNote)
kSerializeVar(msNameEntry, eSerializeType_String)
kSerializeVar(msTextEntry, eSerializeType_String)

kSerializeVar(msIconFile, eSerializeType_String)
kSerializeVar(msImageFile, eSerializeType_String)
kEndSerialize()
//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// DIARY
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxDiary)
kSerializeVar(msNameEntry, eSerializeType_String)
kSerializeVar(msTextEntry, eSerializeType_String)

kSerializeVar(msIconFile, eSerializeType_String)
kSerializeVar(msImageFile, eSerializeType_String)
kEndSerialize()

//////////////////////////////////////////////////////////////////////////
// QUEST NOTE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxQuestNote)
kSerializeVar(mbActive, eSerializeType_Bool)
kSerializeVar(msName, eSerializeType_String)
kSerializeVar(msNameEntry, eSerializeType_String)
kSerializeVar(msTextEntry, eSerializeType_String)
kEndSerialize()

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SCRIPT VAR
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
kBeginSerializeBase(cLuxScriptVar)
kSerializeVar(msName, eSerializeType_String)
kSerializeVar(msVal, eSerializeType_String)
kEndSerialize()


//////////////////////////////////////////////////////////////////////////
// VOICE DATA
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
kBeginSerializeBase(cLuxVoiceData)

kSerializeVar(msText, eSerializeType_WString)
kSerializeVar(msVoiceFile, eSerializeType_String)
kSerializeVar(msEffectFile, eSerializeType_String)
kSerializeVar(mbUsePosition, eSerializeType_Bool)
kSerializeVar(mvPosition, eSerializeType_Vector3f)
kSerializeVar(mfMinDistance, eSerializeType_Float32)
kSerializeVar(mfMaxDistance, eSerializeType_Float32)

kEndSerialize()

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// COLLIDE CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxCollideCallback_SaveData::FromCallback(cLuxCollideCallback *apCallback)
{
	if(apCallback->mpCollideEntity)	mlCollideEntity = apCallback->mpCollideEntity->GetID();
	else							mlCollideEntity = -1;

	msCallbackFunc = apCallback->msCallbackFunc;
	mbDeleteWhenColliding = apCallback->mbDeleteWhenColliding;
	mlStates = apCallback->mlStates;
	mbColliding = apCallback->mbColliding;
}

//-----------------------------------------------------------------------

void cLuxCollideCallback_SaveData::ToCallback(cLuxMap *apMap, iLuxCollideCallbackContainer* apCallbackContainer, cLuxCollideCallback *apCallback)
{
	apCallback->msCallbackFunc = msCallbackFunc;
	apCallback->mbDeleteWhenColliding = mbDeleteWhenColliding;
	apCallback->mlStates = mlStates;
	apCallback->mbColliding = mbColliding;

	apCallback->mpCollideEntity = apMap->GetEntityByID(mlCollideEntity);
	if(apCallback->mpCollideEntity==NULL)
	{
		Error("Could not find entity with ID %d when loading saved collide callback.\n", mlCollideEntity);
	}
	else
	{
		apCallback->mpCollideEntity->AddCollideCallbackParent(apCallbackContainer);
	}
}

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxCollideCallback_SaveData)
kSerializeVar(mlCollideEntity, eSerializeType_Int32)
kSerializeVar(msCallbackFunc, eSerializeType_String)
kSerializeVar(mbDeleteWhenColliding, eSerializeType_Bool)
kSerializeVar(mlStates, eSerializeType_Int32)
kSerializeVar(mbColliding, eSerializeType_Bool)
kEndSerialize()

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// ATTACHED PROP
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeBase(cLuxProp_AttachedProp)
kSerializeVar(msFileName, eSerializeType_String)
kSerializeVar(msName, eSerializeType_String)
kSerializeVar(m_mtxOffset, eSerializeType_Matrixf)
kEndSerialize()

//-----------------------------------------------------------------------
