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

#ifndef LUX_HAND_OBJECT_H
#define LUX_HAND_OBJECT_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxPlayerHands;

//----------------------------------------------

class iLuxHandObject;

class cLuxHandObjectLoader : public cEntityLoader_Object
{
public:
	cLuxHandObjectLoader(const tString& asName);

	void SetHandObject(iLuxHandObject *apHandObject){ mpHandObject = apHandObject; }

	void BeforeLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars);
	void AfterLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars);

private:
	iLuxHandObject *mpHandObject;
};

//----------------------------------------------

class iLuxHandObject
{
friend class cLuxHandObjectLoader;
public:	
	iLuxHandObject(const tString& asName, cLuxPlayerHands *apHands);
	~iLuxHandObject();

	////////////////////////
	//General
	virtual void Update(float afTimeStep)=0;

	bool LoadMainData(cXmlElement *apMainElem);
	void LoadSettings(cXmlElement *apVarsElem);

	void CreateEntity(cLuxMap *apMap);
	void DestroyEntity(cLuxMap *apMap);

	virtual void RenderSolid(cRendererCallbackFunctions* apFunctions){}
	virtual void RenderTrans(cRendererCallbackFunctions* apFunctions){}

	void Reset();
	void ResetEntityContainers();

	void SetSetEntitiesVisible(bool abVisible);

	////////////////////////
	//Callbacks

	//Returning true, means go to hand mode
	virtual bool DoAction(eLuxPlayerAction aAction, bool abPressed)=0;
	
	//Returning true, means go to idle mode
	virtual bool AnimationIsOver()=0;

	////////////////////////
	//Properties
	const tString& GetName(){ return msName;}
	
	cMeshEntity *GetMeshEntity(){ return mpMeshEntity; }
	
	const tString& GetAttachBoneName(){ return msAttachBoneName; }

	const tString& GetAnimIdle(){ return msHandsAnim_Idle; }
	const tString& GetAnimDraw(){ return msHandsAnim_Draw; }
	const tString& GetAnimHolster(){ return msHandsAnim_Holster;}
	
protected:
	virtual void LoadImplementedVars(cXmlElement *apVarsElem)=0;

	virtual void ImplementedCreateEntity(cLuxMap *apMap)=0;
	virtual void ImplementedDestroyEntity(cLuxMap *apMap)=0;

	virtual void ImplementedReset()=0;

	/////////////////////////
	// Data
	cLuxPlayerHands *mpHands;
    tString msName;
	
	tString msModelFile;
	cMatrixf m_mtxOffset;

	tString msAttachBoneName;

	/////////////////////////
	// Animations
	tString msHandsAnim_Idle;
	tString msHandsAnim_Draw;
	tString msHandsAnim_Holster;

	/////////////////////////
	// Variables
	cMesh *mpMesh;
	cMeshEntity* mpMeshEntity;
	std::vector<cBillboard*> mvBillboards;
	std::vector<cParticleSystem*> mvParticleSystems;
	std::vector<iLight*> mvLights;
};

//----------------------------------------------



#endif // LUX_HAND_OBJECT_H
