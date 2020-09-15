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

#ifndef LUX_AREA_H
#define LUX_AREA_H

//----------------------------------------------

#include "LuxEntity.h"

//----------------------------------------------

class iLuxArea;

class iLuxArea_SaveData : public iLuxEntity_SaveData
{
	kSerializableClassInit(iLuxArea_SaveData)
public:
	////////////////
	//Properties
	cVector3f mvSize;
	cMatrixf m_mtxTransform;

	////////////////
	//Methods
	iLuxEntity* CreateEntity(cLuxMap *apMap);
	
	virtual iLuxArea* CreateArea(cLuxMap *apMap)=0;
};

//----------------------------------------------

class iLuxArea : public iLuxEntity
{
typedef iLuxEntity super_class;
friend class iLuxAreaLoader;
friend class iLuxArea_SaveData;
public:	
	iLuxArea(const tString &asName, int alID, cLuxMap *apMap, eLuxAreaType aAreaType);
	virtual ~iLuxArea();

	//////////////////////
	//General
	virtual void OnRenderSolid(cRendererCallbackFunctions* apFunctions);
	
	virtual bool CanInteract(iPhysicsBody *apBody);
	virtual bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);

	//////////////////////
	//Actions
	void GiveDamage(float afAmount, int alStrength){}

	//////////////////////
	//Properties
	eLuxAreaType GetAreaType(){ return mAreaType;}

	iPhysicsBody* GetBody() { return mpBody;}
	cVector3f GetPosition() { return mpBody->GetLocalPosition(); }

	int GetBodyNum(){ return 1; }
	iPhysicsBody* GetBody(int alIdx){ return mpBody;}

	eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

	virtual iEntity3D* GetAttachEntity();

	//////////////////////
	//Save data stuff
	virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);
protected:
	virtual void SetupAfterLoad(cWorld *apWorld){}
	void OnSetActive(bool abX);
	void OnUpdate(float afTimeStep);

	eLuxAreaType mAreaType;
	
	iPhysicsBody *mpBody;
	cVector3f mvSize;
	cMatrixf m_mtxTransform;
};

//----------------------------------------------

class iLuxAreaLoader : public iAreaLoader
{
public:
	iLuxAreaLoader(const tString& asName) : iAreaLoader(asName){}
	virtual ~iLuxAreaLoader(){}

	void Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld);

	virtual iLuxArea *CreateArea(const tString& asName, int alID, cLuxMap *apMap)=0;
	virtual void LoadVariables(iLuxArea *apArea, cWorld *apWorld)=0;
	virtual void SetupArea(iLuxArea *apAre, cWorld *apWorld)=0;
};

//----------------------------------------------


#endif // LUX_AREA_H
