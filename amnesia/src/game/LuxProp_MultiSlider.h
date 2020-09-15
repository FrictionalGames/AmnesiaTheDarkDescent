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

#ifndef LUX_PROP_MULTI_SLIDER_H
#define LUX_PROP_MULTI_SLIDER_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_MultiSlider_SaveData : public iLuxProp_SaveData
{
	kSerializableClassInit(cLuxProp_MultiSlider_SaveData)
public:
	int mlCurrentState;
	int mlStuckState;

	bool mbInteractionDisablesStuck;

	tString msChangeStateCallback;
};

//----------------------------------------------

class cLuxProp_MultiSlider_State
{
public:
    float mfPos;
};

//----------------------------------------------


class cLuxProp_MultiSlider : public iLuxProp
{
typedef iLuxProp super_class;
friend class cLuxPropLoader_MultiSlider;
public:	
	cLuxProp_MultiSlider(const tString &asName, int alID, cLuxMap *apMap);
	virtual ~cLuxProp_MultiSlider();

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
	cLuxInteractData_Slide* GetSlideData(){ return &mSlideData;}
	
	int GetMultiSliderState(){ return mlCurrentState; }

	void SetStuckState(int alState, bool abEffects);
	int  GetStuckState(){ return mlStuckState; }

	void SetInteractionDisablesStuck(bool abX){ mbInteractionDisablesStuck = abX;}
	bool GetInteractionDisablesStuck(bool abX){ return mbInteractionDisablesStuck;}

	void SetChangeStateCallback(const tString &asCallback){ msChangeStateCallback=asCallback;}

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
	void UpdateCheckNewState(float afPos, float afTimeStep);
	void UpdateAutoMove(float afPos, float afTimeStep);

	void ChangeState(int alState, bool abEffects);

	cLuxInteractData_Slide mSlideData;

	iPhysicsJointSlider *mpSliderJoint;
	iPhysicsBody *mpSliderBody;
	std::vector<cLuxProp_MultiSlider_State> mvStates;

	int mlNumOfStates;
	float mfStickToStateMaxDist;
	
	bool mbCanInteractWithStaticBody;
	
	bool mbAutoMoveToCurrentState;
	float mfAutoMoveSpeedFactor;
	float mfAutoMoveMaxSpeed;
	
	tString msChangeStateSound;
	tString msStuckSound;

	float mfDefaultMinDist;
	float mfDefaultMaxDist;

	int mlCurrentState;
	int mlStuckState;
	float mfStuckSoundTimer;

	bool mbInteractionDisablesStuck;

	cPidController<float> mAutoMovePid;

	tString msChangeStateCallback;
};

//----------------------------------------------

class cLuxPropLoader_MultiSlider : public iLuxPropLoader
{
public:
	cLuxPropLoader_MultiSlider(const tString& asName);
	virtual ~cLuxPropLoader_MultiSlider(){}

	iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
	void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
	void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_PROP_MULTI_SLIDER_H
