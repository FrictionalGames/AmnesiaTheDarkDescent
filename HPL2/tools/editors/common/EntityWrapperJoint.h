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

#ifndef HPLEDITOR_ENTITY_WRAPPER_BODY_JOINT_H
#define HPLEDITOR_ENTITY_WRAPPER_BODY_JOINT_H

#include "StdAfx.h"

using namespace hpl;

#include "EntityWrapper.h"
#include "EditorTypes.h"

//-------------------------------------------------------------------

class iEditorWorld;
class cEntityWrapperBody;
class cEditorWindowEntityEditBox;

//-------------------------------------------------------------------

enum eJointInt
{
	eJointInt_LimitAutoSleepNumSteps = eObjInt_LastEnum,
	eJointInt_ParentID,
	eJointInt_ChildID,

	eJointInt_LastEnum,
};

enum eJointFloat
{
	eJointFloat_Stiffness = eObjFloat_LastEnum, 
	eJointFloat_MinMoveSpeed,
	eJointFloat_MinMoveFreq,
	eJointFloat_MinMoveFreqSpeed,
	eJointFloat_MinMoveVolume,
	eJointFloat_MaxMoveSpeed,
	eJointFloat_MaxMoveFreq,
	eJointFloat_MaxMoveFreqSpeed,
	eJointFloat_MaxMoveVolume,
	eJointFloat_MiddleMoveSpeed,
	eJointFloat_MiddleMoveVolume,
	eJointFloat_MinLimitMinSpeed,
	eJointFloat_MinLimitMaxSpeed,
	eJointFloat_MaxLimitMinSpeed,
	eJointFloat_MaxLimitMaxSpeed,
	eJointFloat_BreakForce,

	eJointFloat_LastEnum,
};

enum eJointBool
{
	eJointBool_CollideBodies = eObjBool_LastEnum,
	eJointBool_StickyMinLimit,
	eJointBool_StickyMaxLimit,
	eJointBool_Breakable,

	eJointBool_LastEnum,
};

enum eJointStr
{
	eJointStr_MoveSound = eObjStr_LastEnum,
	eJointStr_MoveType,
	eJointStr_MinLimitSound,
	eJointStr_MaxLimitSound,
	eJointStr_BreakSound,

	eJointStr_LastEnum,
};

//-------------------------------------------------------------------

class iEntityWrapperTypeJoint : public iEntityWrapperType
{
public:
	iEntityWrapperTypeJoint(eEditorEntityJointType aSubType, const tString&);
	int GetJointType() { return mJointType; }
protected:
	eEditorEntityJointType mJointType;
};

//-------------------------------------------------------------------

class iEntityWrapperDataJoint : public iEntityWrapperData
{
public:
	iEntityWrapperDataJoint(iEntityWrapperType*);

	bool SaveSpecific(cXmlElement*);

	//void CopyFromEntity(iEntityWrapper* apEntity);
	//void CopyToEntity(iEntityWrapper* apEntity);
	//bool Load(cXmlElement*);
	//bool SaveSpecific(cXmlElement*);

protected:
};

//-------------------------------------------------------------------

class iEntityWrapperJoint : public iEntityWrapper
{
public:
	iEntityWrapperJoint(iEntityWrapperData*);
	virtual ~iEntityWrapperJoint();

	int GetJointType();

	bool SetProperty(int, const int&);
	bool SetProperty(int, const float&);
	bool SetProperty(int, const bool&);
	bool SetProperty(int, const tString&);

	bool GetProperty(int, int&);
	bool GetProperty(int, float&);
	bool GetProperty(int, bool&);
	bool GetProperty(int, tString&);

	void SetStiffness(float afX) { mfStiffness = afX; }
	float GetStiffness() { return mfStiffness; }

	void SetStickyMinLimit(bool abX) { mbStickyMinLimit = abX; }
	void SetStickyMaxLimit(bool abX) { mbStickyMaxLimit = abX; }
	bool GetStickyMinLimit() { return mbStickyMinLimit; }
	bool GetStickyMaxLimit() { return mbStickyMaxLimit; }

	void SetCollideBodies(bool abX) { mbCollideBodies = abX; }
	bool GetCollideBodies() { return mbCollideBodies; }

	///////////////////////////////////////////////////////////
	// Sound properties
	tString& GetMoveSound() { return msMoveSound; }
	tString& GetMoveType() { return msMoveType; }

	tString& GetMinLimitSound() { return msMinLimitSound; }
	float GetMinLimitMinSpeed() { return mfMinLimitMinSpeed; }
	float GetMinLimitMaxSpeed() { return mfMinLimitMaxSpeed; }

	tString& GetMaxLimitSound() { return msMaxLimitSound; }
	float GetMaxLimitMinSpeed() { return mfMaxLimitMinSpeed; }
	float GetMaxLimitMaxSpeed() { return mfMaxLimitMaxSpeed; }

	float GetMinMoveSpeed() { return mfMinMoveSpeed; }
	float GetMinMoveFreq() { return mfMinMoveFreq; }
	float GetMinMoveFreqSpeed() { return mfMinMoveFreqSpeed; }
	float GetMinMoveVolume() { return mfMinMoveVolume; }
	
	float GetMaxMoveSpeed() { return mfMaxMoveSpeed; }
	float GetMaxMoveFreq() { return mfMaxMoveFreq; }
	float GetMaxMoveFreqSpeed() { return mfMaxMoveFreqSpeed; }
	float GetMaxMoveVolume() { return mfMaxMoveVolume; }
	
	float GetMiddleMoveSpeed() { return mfMiddleMoveSpeed; }
	float GetMiddleMoveVolume() { return mfMiddleMoveVolume; }

	
	void SetMoveSound(const tString& asX) { msMoveSound = asX; }
	void SetMoveType(const tString& asX) { msMoveType = asX; }

	void SetMinLimitSound(const tString& asX) { msMinLimitSound = asX; }
	void SetMinLimitMinSpeed(float afX) { mfMinLimitMinSpeed = afX; }
	void SetMinLimitMaxSpeed(float afX) { mfMinLimitMaxSpeed = afX; }

	void SetMaxLimitSound(const tString& asX) { msMaxLimitSound = asX; }
	void SetMaxLimitMinSpeed(float afX) { mfMaxLimitMinSpeed = afX; }
	void SetMaxLimitMaxSpeed(float afX) { mfMaxLimitMaxSpeed = afX; }

	void SetMinMoveSpeed(float afX) { mfMinMoveSpeed = afX; }
	void SetMinMoveFreq(float afX) { mfMinMoveFreq = afX; }
	void SetMinMoveFreqSpeed(float afX) { mfMinMoveFreqSpeed = afX; }
	void SetMinMoveVolume(float afX) { mfMinMoveVolume = afX; }
	
	void SetMaxMoveSpeed(float afX) { mfMaxMoveSpeed = afX; }
	void SetMaxMoveFreq(float afX) { mfMaxMoveFreq = afX; }
	void SetMaxMoveFreqSpeed(float afX) { mfMaxMoveFreqSpeed = afX; }
	void SetMaxMoveVolume(float afX) { mfMaxMoveVolume = afX; }
	
	void SetMiddleMoveSpeed(float afX) { mfMiddleMoveSpeed = afX; }
	void SetMiddleMoveVolume(float afX) { mfMiddleMoveVolume = afX; }

	int GetLimitStepCount() { return mlLimitAutoSleepNumSteps; }
	void SetLimitStepCount(int alX) { mlLimitAutoSleepNumSteps = alX; }

	void SetBreakable(bool abX) { mbBreakable = abX; }
	void SetBreakForce(float afX) { mfBreakForce = afX; }
	void SetBreakSound(const tString& asX) { msBreakSound = asX; }
	bool IsBreakable() { return mbBreakable; }
	float GetBreakForce() { return mfBreakForce; }
	tString& GetBreakSound() { return msBreakSound; }


	void SetParentBody(cEntityWrapperBody* apBody, bool abRemoveFromOld=true);
	void SetChildBody(cEntityWrapperBody* apBody, bool abRemoveFromOld=true);

	cEntityWrapperBody* GetParentBody() { return mpParentBody; }
	cEntityWrapperBody* GetChildBody() { return mpChildBody; }

	void SetParentBodyByID(int);
	void SetChildBodyByID(int);
	int GetParentBodyID();
	int GetChildBodyID();

	cEditorWindowEntityEditBox* CreateEditBox(cEditorEditModeSelect* apEditMode);

	//void SaveToElement(cXmlElement* apElement);
	
	void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol);
protected:
	cEntityIcon* CreateIcon();
	////////////////////////////////////////////
	// Data
	bool mbCollideBodies;

	float mfStiffness;

	bool mbStickyMinLimit;
	bool mbStickyMaxLimit;

	tString msMoveSound;
	tString msMoveType;
	float mfMinMoveSpeed;
	float mfMinMoveFreq;
	float mfMinMoveFreqSpeed;
	float mfMinMoveVolume;
	float mfMaxMoveSpeed;
	float mfMaxMoveFreq;
	float mfMaxMoveFreqSpeed;
	float mfMaxMoveVolume;
	float mfMiddleMoveSpeed;
	float mfMiddleMoveVolume;

	tString msMinLimitSound;
	tString msMaxLimitSound;

	float mfMinLimitMinSpeed;
	float mfMinLimitMaxSpeed;
	float mfMaxLimitMinSpeed;
	float mfMaxLimitMaxSpeed;

	bool mbLimitAutoSleep;
	float mfLimitAutoSleepDist;
	int mlLimitAutoSleepNumSteps;

	bool mbBreakable;
	float mfBreakForce;
	tString msBreakSound;

	cEntityWrapperBody* mpParentBody;
	cEntityWrapperBody* mpChildBody;
};

//-------------------------------------------------------------------

typedef std::vector<iEntityWrapperJoint*> tJointWrapperVec;
typedef tJointWrapperVec::iterator		  tJointWrapperVecIt;

//-------------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_BODY_JOINT_H
