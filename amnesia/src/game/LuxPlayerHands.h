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

#ifndef LUX_PLAYER_HANDS_H
#define LUX_PLAYER_HANDS_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxPlayer;
class iLuxHandObject;
class cLuxHandObjectLoader;

//----------------------------------------------

class cLuxPlayerHands;

class cLuxPlayerHandsLoader : public cEntityLoader_Object
{
public:
	cLuxPlayerHandsLoader(const tString& asName, cLuxPlayerHands *apPlayerHands);

	void BeforeLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars);
	void AfterLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars);

private:
	cLuxPlayerHands *mpPlayerHands;
};

//----------------------------------------------

enum eLuxHandsState
{
	eLuxHandsState_Disabled, 
	eLuxHandsState_Holster,
	eLuxHandsState_Draw,
	eLuxHandsState_Idle,
	eLuxHandsState_HandObject,
};

//----------------------------------------------

class cLuxPlayerHands : public iLuxPlayerHelper
{
friend class cLuxPlayerHands_SaveData;
friend class cLuxPlayerHandsLoader;
public:	
	cLuxPlayerHands(cLuxPlayer *apPlayer);
	~cLuxPlayerHands();
	
	///////////////////////////////
	// General
	void OnStart();
	void Update(float afTimeStep);
	void PostUpdate(float afTimeStep);
	void Reset();

	void OnMapEnter(cLuxMap *apMap);
	void OnMapLeave(cLuxMap *apMap);

	void CreateWorldEntities(cLuxMap *apMap);
	void DestroyWorldEntities(cLuxMap *apMap);

	void RenderSolid(cRendererCallbackFunctions* apFunctions);
	void RenderTrans(cRendererCallbackFunctions* apFunctions);

	///////////////////////////////
	// Actions
	void PlayAnim(const tString& asAnim, bool abLoop);
	bool AnimOver();
	bool CheckAnimationEvent(float afRelTime);

	void DoAction(eLuxPlayerAction aAction, bool abPressed);

	///////////////////////////////
	// Properties
	void SetActiveHandObject(const tString& asName);
	iLuxHandObject* GetHandObject(const tString& asName);

	void SetCurrentHandObject(iLuxHandObject *apObject);
	iLuxHandObject* GetCurrentHandObject(){ return mpCurrentHandObject;}

	void SetState(eLuxHandsState aState);
	eLuxHandsState GetState(){ return mHandState; }

	const tString& GetCurrentAnimation(){ return msCurrentAnim; }
	cMeshEntity *GetHandsEntity(){ return mpHandsEntity;}

    
public://Only used by hand object
	float mfHandObjectChargeCount;
	int mlHandObjectState;
	bool mbHandObjectAttackDown;
	bool mbHandObjectInteractDown;
	float mfHandObjectAlpha;

private:
	void ResetHandObjectVars();

	void CreateHandEntity(cLuxMap *apMap);
	void DestroyHandEntity(cLuxMap *apMap);
	void CreateAndAttachHandObject(cLuxMap *apMap, iLuxHandObject *apHandObject);
	void HideAllHandObjects();
	
	void UpdatePlayerHandsPos(float afTimeStep);

	iLuxHandObject* LoadHandObject(const tString& asName);
	iLuxHandObject* CreateObjectFromType(const tString& asName, eLuxHandObjectType aType);
	eLuxHandObjectType ToHandObjectType(const tString& asType);

	cMeshEntity *mpHandsEntity;
	cMesh *mpHandsMesh;
	std::vector<cAnimation*> mvHandAnimations;

	iLuxHandObject *mpCurrentHandObject;
	std::vector<iLuxHandObject*> mvHandObjects;

	cLuxHandObjectLoader *mpHandObjectLoader;

	float mfPosAddMul;

	int mlMaxCamRotations;
	tVector3fList mlstCamRotations;

	eLuxHandsState mHandState;
	tString msCurrentAnim;
};

//----------------------------------------------



#endif // LUX_PLAYER_HANDS_H
