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

#ifndef LUX_PROP_LEVER_H
#define LUX_PROP_LEVER_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_Lever_SaveData : public iLuxProp_SaveData
{
	kSerializableClassInit(cLuxProp_Lever_SaveData)
public:
	int mlCurrentState;
	int mlStuckState;

	float mfMiddleAngleAmount;
	bool mbAutoMoveToAngle;
	int mlAutoMoveGoal;

	bool mbInteractionDisablesStuck;
};

//----------------------------------------------


class cLuxProp_Lever : public iLuxProp
{
typedef iLuxProp super_class;
friend class cLuxPropLoader_Lever;
public:	
	cLuxProp_Lever(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxProp_Lever();

	//////////////////////
	//General
	bool CanInteract(iPhysicsBody *apBody);
	bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);
	
	void OnSetupAfterLoad(cWorld *apWorld);

	void OnResetProperties();

	void UpdatePropSpecific(float afTimeStep);
	
	void BeforePropDestruction();

	eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

	//////////////////////
	//Properties
	iLuxInteractData_RotateBase* GetMoveBaseData(){ return &mLeverData;}
	
	int GetLeverState(){ return mlCurrentState; }

	void SetStuckState(int alState, bool abEffects);
	int  GetStuckState(){ return mlStuckState; }

	void SetInteractionDisablesStuck(bool abX){ mbInteractionDisablesStuck = abX;}
	bool GetInteractionDisablesStuck(bool abX){ return mbInteractionDisablesStuck;}

	//////////////////////
	//Connection callbacks
	void OnConnectionStateChange(iLuxEntity *apEntity, int alState);
	
	//////////////////////
	//Save data stuff
	iLuxEntity_SaveData* CreateSaveData();
	void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
	void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
	void SetupSaveData(iLuxEntity_SaveData *apSaveData);

private:
	void CalculateMiddleAngle();

	void UpdateCheckStuckSound(float afTimeStep);
	void UpdateCheckLimit(float afAngle, float afTimeStep);
	void UpdateAutoMove(float afAngle, float afTimeStep);

	void ChangeState(int alState, bool abEffects);

	cLuxInteractData_Lever mLeverData;

	float mfStuckSoundTimer;

	float mfDefaultMinAngle;
	float mfDefaultMaxAngle;
	iPhysicsJointHinge *mpHingeJoint;
	iPhysicsBody *mpLeverBody;
	
	bool mbCanInteractWithStaticBody;
	float mfMinLimitRange;
	float mfMaxLimitRange;
	bool mbMinLimitStuck;
	bool mbMaxLimitStuck;

	float mfMiddleAngle;
	float mfMiddleAngleAmount;
	bool mbAutoMoveToAngle;
	int mlAutoMoveGoal;
	float mfAutoMoveSpeedFactor;
	float mfAutoMoveMaxSpeed;

	tString msMinLimitSound;
	tString msMaxLimitSound;

	cPidController<cVector3f> mRotatePid;

	int mlCurrentState;
	int mlStuckState;

	tString msStuckSound;

	bool mbInteractionDisablesStuck;
};

//----------------------------------------------

class cLuxPropLoader_Lever : public iLuxPropLoader
{
public:
	cLuxPropLoader_Lever(const tString& asName);
	virtual ~cLuxPropLoader_Lever(){}

	iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
	void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
	void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_PROP_LEVER_H
