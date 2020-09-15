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

#ifndef LUX_ENTITY_H
#define LUX_ENTITY_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class iLuxEntity;

//----------------------------------------------

class cLuxEntityConnection;

class cLuxEntityConnection_SaveData : public iSerializable
{
	kSerializableClassInit(cLuxEntityConnection_SaveData)
public:
	void FromConnection(cLuxEntityConnection *apConnection);
	void ToConnection(cLuxEntityConnection *apConnection, cLuxMap *apMap);

	tString msName;
	int mlEntityId;
	bool mbInvertStateSent;
	int mlStatesUsed;
	tString msCallbackFunc;
};

//----------------------------------------------

class iLuxEntity_SaveData : public iSerializable
{
	kSerializableClassInit(iLuxEntity_SaveData)
public:
	virtual ~iLuxEntity_SaveData(){}

	tString msName;
	bool mbActive;
	bool mbFullGameSave;
	int mlEntityType;
	int mlID;

	float mfMaxFocusDistance;

	bool mbInteractionDisabled;

	tString msCallbackFunc;
	tString msConnectionStateChangeCallback;

	tString msInteractCallback;
	bool mbInteractCallbackRemove;

	tString msLookAtCallback;
	bool mbLookAtCallbackRemove;
	bool mbIsLookedAt;

	cContainerVec<cLuxEntityConnection_SaveData> mvConnections;
	cContainerList<cLuxCollideCallback_SaveData> mlstCollideCallbacks;

	virtual iLuxEntity* CreateEntity(cLuxMap *apMap)=0;
};

//----------------------------------------------

class cLuxEntityConnection
{
friend class cLuxEntityConnection_SaveData;
public:
	cLuxEntityConnection(){}
	cLuxEntityConnection(const tString& asName, iLuxEntity *apEntity, bool abInvertStateSent, int alStatesUsed, const tString& asCallbackFunc) 
		: msName(asName), mpEntity(apEntity), mbInvertStateSent(abInvertStateSent), mlStatesUsed(alStatesUsed), msCallbackFunc(asCallbackFunc) {}

	const tString& GetName(){ return msName; }

	iLuxEntity* GetEntity(){ return mpEntity; }

	bool GetInvertStateSent(){ return mbInvertStateSent;}
	int GetStateUsed(){ return mlStatesUsed;}

	const tString& GetCallbackFunc(){ return msCallbackFunc; }

private:
	tString msName;
	iLuxEntity *mpEntity;
	bool mbInvertStateSent;
	int mlStatesUsed;
	tString msCallbackFunc;
};

//----------------------------------------------

class cLuxMap;

class iLuxEntity : public iLuxCollideCallbackContainer
{
friend class cLuxMap;
friend class cLuxSavedGameEntity;
friend class cLuxSavedGameMap;
public:	
	iLuxEntity(const tString &asName, int alID, cLuxMap *apMap,eLuxEntityType aEntityType);
	virtual ~iLuxEntity();

	//////////////////
	// General
	void UpdateLogic(float afTimeStep);

	virtual void OnRenderSolid(cRendererCallbackFunctions* apFunctions){}

	virtual bool CanInteract(iPhysicsBody *apBody)=0;
	virtual bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)=0;
	
	virtual void AfterWorldLoad(){}
	virtual void OnMapEnter(){}

	virtual void InFocusDraw(cGuiSet *apGuiSet,float afFrameTime){}

	//////////////////
	// Action
	cSoundEntity* PlaySound(const tString& asName, const tString& asFile, bool abRemoveWhenDone, bool abAttach);

	void RunCallbackFunc(const tString& asType);
	void RunInteractCallbackFunc();

	virtual void GiveDamage(float afAmount, int alStrength)=0;

	//////////////////
	// Properties
	eLuxEntityType GetEntityType()const{ return mEntityType;}
	
	const tString& GetName()const{ return msName;}
	int GetID(){ return mlID;}

	void SetActive(bool abX);
	bool IsActive(){ return mbActive;}

	cLuxMap *GetMap(){ return mpMap;}

	void SetFullGameSave(bool abX){ mbFullGameSave=abX;}
	bool GetFullGameSave(){ return mbFullGameSave;}

	void SetIsSaved(bool abX){ mbIsSaved = abX;}
	bool IsSaved(){ return mbIsSaved;}
	bool GetDestroyMe(){ return mbDestroyMe;}

	void SetInteractionDisabled(bool abX){ mbInteractionDisabled = abX; }
	bool GetInteractionDisabled(){ return mbInteractionDisabled; }

	float GetMaxFocusDistance(){ return mfMaxFocusDistance;}
	virtual eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)=0;

	void SetCustomFocusCrossHair(eLuxFocusCrosshair aX){ mCustomFocusCrossHair = aX;}

	virtual tWString GetFocusText(){ return _W("");}

	virtual iEntity3D* GetAttachEntity()=0;

	virtual cMeshEntity* GetMeshEntity(){ return NULL; }

	void SetCallbackFunc(const tString& asFunc){ msCallbackFunc = asFunc;}
	void SetConnectionStateChangeCallback(const tString& asFunc){ msConnectionStateChangeCallback = asFunc;}

	void SetPlayerInteractCallback(const tString &asCallbackFunc, bool abRemoveWhenInteracted);
	void SetPlayerLookAtCallback(const tString &asCallbackFunc, bool abRemoveWhenLookedAt);
	bool IsLookedAtByPlayer(){ return mbIsLookedAt; }

	void AddCollideCallbackParent(iLuxCollideCallbackContainer* apCallback);
	void RemoveCollideCallbackParent(iLuxCollideCallbackContainer* apCallback);
	
	//////////////////
	// Connection
	int GetConnectionNum(){ return (int)mvConnections.size();}
	cLuxEntityConnection *GetConnection(int lIdx){ return mvConnections[lIdx];}
	void AddConnection(const tString& asName, iLuxEntity *apEntity, bool abInvertStateSent, int alStatesUsed, const tString &asCallbackFunc);

	////////////////
	// Debug
	virtual float DrawDebug(cGuiSet *apSet,iFontData *apFont,float afStartY){ return afStartY;}

		
	//////////////////////
	//Save data stuff
	void SetSaveData(iLuxEntity_SaveData *apData){ mpSaveData = apData;}
	iLuxEntity_SaveData *GetSaveData(){ return mpSaveData;}

	virtual iLuxEntity_SaveData* CreateSaveData()=0;
	virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);

protected:
	void UpdateCheckCollideCallback(float afTimeStep);
	void UpdatePlayerLookAt(float afTimeStep);
	void ConnectionStateChange(int alState);

	/////////////////
	//Virtual methods
	virtual void OnConnectionStateChange(iLuxEntity *apEntity, int alState)=0;
	virtual void OnUpdate(float afTimeStep)=0;
	virtual void BeforeEntityDestruction(){}
	virtual void OnSetActive(bool abX){}

	/////////////////
	//Helper methods
	void PreloadEntityModel(const tString &asFile);
	bool CollidesWithPlayer();

	tString msName;
	bool mbFullGameSave;
	bool mbActive;
	bool mbIsSaved;
	int mlID;

	bool mbInteractionDisabled;

	tLuxCollideCallbackContainerList mlstCollideCallbackParents;

	tString msCallbackFunc;
	tString msConnectionStateChangeCallback;

	tString msInteractCallback;
	bool mbInteractCallbackRemove;

	tString msLookAtCallback;
	bool mbLookAtCallbackRemove;
	float mfLookAtCount;
	bool mbIsLookedAt;

	float mfMaxFocusDistance;

	eLuxFocusCrosshair mCustomFocusCrossHair;

	cLuxMap *mpMap;

	bool mbDestroyMe;
	
	iLuxEntity_SaveData *mpSaveData;

	std::vector<cMesh*> mvPreloadedMeshes;
	std::vector<cLuxEntityConnection*> mvConnections;

private:
	eLuxEntityType mEntityType;

	void DestroyMe(){ mbDestroyMe = true;}
};

//----------------------------------------------


#endif // LUX_ENTITY_H
