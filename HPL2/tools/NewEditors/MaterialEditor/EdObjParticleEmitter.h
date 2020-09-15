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

#ifndef ED_OBJ_PARTICLE_EMITTER_H
#define ED_OBJ_PARTICLE_EMITTER_H

#include "../Common/EdWorld.h"

//----------------------------------------------------------------------

class iEdObject;

class cEdObjParticleEmitter;

typedef std::vector<cEdObjParticleEmitter*> tEdPartEmitterVec;

//----------------------------------------------------------------------

enum eEditorParticleEmitterType
{
	 eEditorParticleEmitterType_Point,
	 eEditorParticleEmitterType_Line,
	 eEditorParticleEmitterType_Axis
};

enum ePEInt
{
	ePEInt_MaxParticleNum = eObjInt_LastEnum,
	ePEInt_WarmUpStepsPerSec,
	ePEInt_CollisionUpdateRate,

	ePEInt_LowFreqPoints,
	ePEInt_HighFreqPoints,
	
	ePEInt_NumFiles,
	
	ePEInt_LastEnum,
};

enum ePEFloat
{
	ePEFloat_ParticlesPerSecond = eObjFloat_LastEnum,
	ePEFloat_StartTimeOffset,
	ePEFloat_WarmUpTime,

	ePEFloat_MinPauseLength,
	ePEFloat_MaxPauseLength,
	ePEFloat_MinPauseInterval,
	ePEFloat_MaxPauseInterval,

	ePEFloat_MinStartRadius,
	ePEFloat_MaxStartRadius,

	ePEFloat_MinStartVelSpeed,
	ePEFloat_MaxStartVelSpeed,

	ePEFloat_MinSpeedMultiply,
	ePEFloat_MaxSpeedMultiply,

	ePEFloat_MinVelMaximum,
	ePEFloat_MaxVelMaximum,

	ePEFloat_MinSpinRange,
	ePEFloat_MaxSpinRange,

	ePEFloat_MinLifeSpan,
	ePEFloat_MaxLifeSpan,

	ePEFloat_StartRelSize,
	ePEFloat_MiddleRelSize,
	ePEFloat_EndRelSize,

	ePEFloat_MiddleRelSizeTime,
	ePEFloat_MiddleRelSizeLength,
	ePEFloat_MiddleRelColorTime,
	ePEFloat_MiddleRelColorLength,
	ePEFloat_MinCollisionMax,
	ePEFloat_MaxCollisionMax,

	ePEFloat_MinBounceAmount,
	ePEFloat_MaxBounceAmount,

	ePEFloat_AnimLength,

	ePEFloat_LastEnum
};

enum ePEBool
{
	ePEBool_Respawn = eObjBool_LastEnum,
	ePEBool_UseDirection,
	ePEBool_UsePartSpin,
	ePEBool_UseRevolution,
	ePEBool_MultiplyRGBWithAlpha,
	ePEBool_Collides,
	ePEBool_UseBeamNoise,
	ePEBool_ApplyNoiseToStart,
	ePEBool_ApplyNoiseToEnd,

	ePEBool_LastEnum,
};

enum ePEStr
{
	ePEStr_PEType = eObjStr_LastEnum,
	ePEStr_StartPosType,
	ePEStr_StartVelType,
	ePEStr_CoordSystem,
	ePEStr_GravityType,

	ePEStr_PartSpinType,

	ePEStr_DrawType,
	ePEStr_DeathType,

	ePEStr_SubDivType,

	ePEStr_DeathPS,
	ePEStr_Material,

	ePEStr_LastEnum,
};

enum ePEVec2f
{
	ePEVec2f_MinStartAngles = eObjVec2f_LastEnum,
	ePEVec2f_MaxStartAngles,
	ePEVec2f_MinStartVelAngles,
	ePEVec2f_MaxStartVelAngles,
	ePEVec2f_MinStartSize,
	ePEVec2f_MaxStartSize,
	ePEVec2f_SubDivAmount,

	ePEVec2f_LastEnum,
};

enum ePEVec3f
{
	ePEVec3f_PosOffset = eObjVec3f_LastEnum,
	ePEVec3f_AngleOffset,

	ePEVec3f_MinStartPos,
	ePEVec3f_MaxStartPos,

	ePEVec3f_MinStartVel,
	ePEVec3f_MaxStartVel,
	ePEVec3f_MinStartAcc,
	ePEVec3f_MaxStartAcc,
	ePEVec3f_GravityAcc,
	ePEVec3f_MinRevVel,
	ePEVec3f_MaxRevVel,
	ePEVec3f_MinLFNoise,
	ePEVec3f_MaxLFNoise,
	ePEVec3f_MinHFNoise,
	ePEVec3f_MaxHFNoise,

	ePEVec3f_LastEnum,
};

enum ePECol
{
	ePECol_MinStartColor = eObjCol_LastEnum,
	ePECol_MaxStartColor,
	ePECol_StartRelColor,
	ePECol_MiddleRelColor,
	ePECol_EndRelColor,

	ePECol_LastEnum,
};

//----------------------------------------------------------------------

class cTypeParticleEmitter : public iEdObjectType
{
public:
	cTypeParticleEmitter();

protected:
	iEdObjectData* CreateTypeSpecificData();
};

//----------------------------------------------------------------------

class cEdObjParticleEmitterData : public iEdObjectData
{
public:
	cEdObjParticleEmitterData(iEdObjectType*);
    
protected:
	iEdObject* CreateTypeSpecificObject();

	bool LoadSpecificData(cXmlElement*) { return true; }
	bool SaveSpecificData(cXmlElement*) { return true; }
};

//----------------------------------------------------------------------

class cEdObjParticleEmitter : public iEdObject
{
public:
	cEdObjParticleEmitter(iEdObjectData*);

	bool SetProperty(int, const int);
	bool SetProperty(int, const float);
	bool SetProperty(int, const bool);
	bool SetProperty(int, const tString&);
	bool SetProperty(int, const cVector2f&);
	bool SetProperty(int, const cVector3f&);
	bool SetProperty(int, const cColor&);

	bool GetProperty(int, int&);
	bool GetProperty(int, float&);
	bool GetProperty(int, bool&);
	bool GetProperty(int, tString&);
	bool GetProperty(int, cVector2f&);
	bool GetProperty(int, cVector3f&);
	bool GetProperty(int, cColor&);

	iEdEditPane* CreateEditPane();

	void SetRespawn(bool abX) { mbRespawn = abX; }
	void SetPEType(ePEType aType) { mPEType = aType; }
	void SetMaxParticleNum(int alX) { mlMaxParticleNum = alX; }
	void SetParticlesPerSecond(float alX) { mfParticlesPerSecond = alX; }

	void SetStartTimeOffset(float afX) { mfStartTimeOffset = afX; }
	void SetWarmUpTime(float afX) { mfWarmUpTime = afX; }
	void SetWarmUpStepsPerSec(int alX) { mlWarmUpStepsPerSec = alX; }

	void SetMinPauseLength(float afX) { mfMinPauseLength = afX; }
	void SetMaxPauseLength(float afX) { mfMaxPauseLength = afX; }
	void SetMinPauseInterval(float afX) { mfMinPauseInterval = afX; }
	void SetMaxPauseInterval(float afX) { mfMaxPauseInterval = afX; } 

	void SetPosOffset(const cVector3f& avX) { mvPosOffset = avX; }
	void SetAngleOffset(const cVector3f& avX) { mvAngleOffset = avX; }

	void SetStartPosType(ePEStartPosType aType) { mStartPosType = aType; }
	void SetMinStartPos(const cVector3f& avX) { mvMinStartPos = avX; }
	void SetMaxStartPos(const cVector3f& avX) { mvMaxStartPos = avX; }
	void SetMinStartRadius(float afX) { mfMinStartRadius = afX; }
	void SetMaxStartRadius(float afX) { mfMaxStartRadius = afX; }
	void SetMinStartAngles(const cVector2f& avX) { mvMinStartAngles = avX; }
	void SetMaxStartAngles(const cVector2f& avX) { mvMaxStartAngles = avX; }

	void SetStartVelType(ePEStartPosType aType) { mStartVelType = aType; }
	void SetMinStartVel(const cVector3f& avX) { mvMinStartVel = avX; }
	void SetMaxStartVel(const cVector3f& avX) { mvMaxStartVel = avX; }

	void SetCoordSystem(eParticleEmitterCoordSystem aSystem) { mCoordSystem = aSystem; }

	void SetMinStartVelAngles(const cVector2f& avX) { mvMinStartVelAngles = avX; }
	void SetMaxStartVelAngles(const cVector2f& avX) { mvMaxStartVelAngles = avX; }

	void SetMinStartVelSpeed(float afX) { mfMinStartVelSpeed = afX; }
	void SetMaxStartVelSpeed(float afX) { mfMaxStartVelSpeed = afX; }

	void SetMinSpeedMultiply(float afX) { mfMinSpeedMultiply = afX; }
	void SetMaxSpeedMultiply(float afX) { mfMaxSpeedMultiply = afX; }

	void SetMinStartAcc(const cVector3f& avX) { mvMinStartAcc = avX; }
	void SetMaxStartAcc(const cVector3f& avX) { mvMaxStartAcc = avX; }

	void SetMinVelMaximum(float afX) { mfMinVelMaximum = afX; }
	void SetMaxVelMaximum(float afX) { mfMaxVelMaximum = afX; }

	void SetUsesDirection(bool abX) { mbUsesDirection = abX; }

	void SetGravityType(ePEGravityType aType) { mGravityType = aType; }
	void SetGravityAcc(const cVector3f& avX) { mvGravityAcc = avX; }

	void SetUsePartSpin(bool abX) { mbUsePartSpin = abX; }
	void SetPartSpinType(ePEPartSpinType aType) { mPartSpinType = aType; }
	void SetMinSpinRange(float afX) { mfMinSpinRange = afX; }
	void SetMaxSpinRange(float afX) { mfMaxSpinRange = afX; }

	void SetUseRevolution(bool abX) { mbUseRevolution = abX; }
	void SetMinRevVel(const cVector3f& avX) { mvMinRevVel = avX; }
	void SetMaxRevVel(const cVector3f& avX) { mvMaxRevVel = avX; }
	bool GetUseRevolution() { return mbUseRevolution; }
	const cVector3f& GetMinRevVel() { return mvMinRevVel; }
	const cVector3f& GetMaxRevVel() { return mvMaxRevVel; }

	void SetMinLifeSpan(float afX) { mfMinLifeSpan = afX; }
	void SetMaxLifeSpan(float afX) { mfMaxLifeSpan = afX; }

	void SetDeathType(ePEDeathType aType) { mDeathType = aType; }
	void SetDeathPS(const tString& asX) { msDeathPS = asX; }

	void SetDrawType(eEditorParticleEmitterType aType) { mDrawType = aType; }
	void SetMultiplyRGBWithAlpha(bool abX) { mbMultiplyRGBWithAlpha = abX; }
	void SetMinStartSize(const cVector2f& avX) { mvMinStartSize = avX; }
	void SetMaxStartSize(const cVector2f& avX) { mvMaxStartSize = avX; }
	void SetStartRelSize(float afX) { mfStartRelSize = afX; }
	void SetMiddleRelSize(float afX) { mfMiddleRelSize = afX; }
	void SetEndRelSize(float afX) { mfEndRelSize = afX; }
	void SetMiddleRelSizeTime(float afX) { mfMiddleRelSizeTime = afX; }
	void SetMiddleRelSizeLength(float afX) { mfMiddleRelSizeLength = afX; }
	
	void SetMinStartColor(const cColor& aX) { mMinStartColor = aX; }
	void SetMaxStartColor(const cColor& aX) { mMaxStartColor = aX; }

	void SetStartRelColor(const cColor& aX) { mStartRelColor = aX; }
	void SetMiddleRelColor(const cColor& aX) { mMiddleRelColor = aX; }
	void SetEndRelColor(const cColor& aX) { mEndRelColor = aX; }
	void SetMiddleRelColorTime(float afX) { mfMiddleRelColorTime = afX; }
	void SetMiddleRelColorLength(float afX) { mfMiddleRelColorLength = afX; }
	
	void SetCollides(bool abX) { mbCollides = abX; }

	void SetMinCollisionMax(float afX) { mfMinCollisionMax = afX; }
	void SetMaxCollisionMax(float afX) { mfMaxCollisionMax = afX; }

	void SetMinBounceAmount(float afX) { mfMinBounceAmount = afX; }
	void SetMaxBounceAmount(float afX) { mfMaxBounceAmount = afX; }
	void SetCollisionUpdateRate(int alX) { mlCollisionUpdateRate = alX; }
	
	void SetUseBeamNoise(bool abX) { mbUseBeamNoise = abX; }
	void SetLowFreqPoints(int alX) { mlLowFreqPoints = alX; }
	void SetMinLFNoise(const cVector3f& avX) { mvMinLFNoise = avX; }
	void SetMaxLFNoise(const cVector3f& avX) { mvMaxLFNoise = avX; }
	void SetHighFreqPoints(int alX) { mlHighFreqPoints = alX; }
	void SetMinHFNoise(const cVector3f& avX) { mvMinHFNoise = avX; }
	void SetMaxHFNoise(const cVector3f& avX) { mvMaxHFNoise = avX; }
	void SetApplyNoiseToStart(bool abX) { mbApplyNoiseToStart = abX; }
	void SetApplyNoiseToEnd(bool abX) { mbApplyNoiseToEnd = abX; }

	void SetMaterial(const tString& asX);
	void SetNumFiles(int alX) { mlNumFiles = alX; }
	void SetAnimLength(float afX) { mfAnimLength = afX; }

	void SetSubDivAmount(const cVector2f& avX) { mvSubDivAmount=avX; }
	void SetSubDivisionType(ePESubDivType aX) { mSubDivType=aX; }


	
	bool GetRespawn() { return mbRespawn; }
	ePEType GetPEType() { return mPEType; }
	int GetMaxParticleNum() { return mlMaxParticleNum; }
	float GetParticlesPerSecond() { return mfParticlesPerSecond; }

	float GetStartTimeOffset() { return mfStartTimeOffset; }
	float GetWarmUpTime() { return mfWarmUpTime; }
	int GetWarmUpStepsPerSec() { return mlWarmUpStepsPerSec; }

	float GetMinPauseLength() { return mfMinPauseLength; }
	float GetMaxPauseLength() { return mfMaxPauseLength; }
	float GetMinPauseInterval() { return mfMinPauseInterval; }
	float GetMaxPauseInterval() { return mfMaxPauseInterval; } 

	const cVector3f& GetPosOffset() { return mvPosOffset; }
	const cVector3f& GetAngleOffset() { return mvAngleOffset; }

	ePEStartPosType GetStartPosType() { return mStartPosType; }
	const cVector3f& GetMinStartPos() { return mvMinStartPos; }
	const cVector3f& GetMaxStartPos() { return mvMaxStartPos; }
	float GetMinStartRadius() { return mfMinStartRadius; }
	float GetMaxStartRadius() { return mfMaxStartRadius; }
	const cVector2f& GetMinStartAngles() { return mvMinStartAngles; }
	const cVector2f& GetMaxStartAngles() { return mvMaxStartAngles; }

	ePEStartPosType GetStartVelType() { return mStartVelType; }
	const cVector3f& GetMinStartVel() { return mvMinStartVel; }
	const cVector3f& GetMaxStartVel() { return mvMaxStartVel; }

	eParticleEmitterCoordSystem GetCoordSystem() { return mCoordSystem; } 

	const cVector2f& GetMinStartVelAngles() { return mvMinStartVelAngles; }
	const cVector2f& GetMaxStartVelAngles() { return mvMaxStartVelAngles; }

	float GetMinStartVelSpeed() { return mfMinStartVelSpeed; }
	float GetMaxStartVelSpeed() { return mfMaxStartVelSpeed; }

	float GetMinSpeedMultiply() { return mfMinSpeedMultiply; }
	float GetMaxSpeedMultiply() { return mfMaxSpeedMultiply; }

	const cVector3f& GetMinStartAcc() { return mvMinStartAcc; }
	const cVector3f& GetMaxStartAcc() { return mvMaxStartAcc; }

	float GetMinVelMaximum() { return mfMinVelMaximum; }
	float GetMaxVelMaximum() { return mfMaxVelMaximum; }

	bool GetUsesDirection() { return mbUsesDirection; }

	ePEGravityType GetGravityType() { return mGravityType; }
	const cVector3f& GetGravityAcc() { return mvGravityAcc; }

	bool GetUsePartSpin() { return mbUsePartSpin; }
	ePEPartSpinType GetPartSpinType() { return mPartSpinType; }
	float GetMinSpinRange() { return mfMinSpinRange; }
	float GetMaxSpinRange() { return mfMaxSpinRange; }

	float GetMinLifeSpan() { return mfMinLifeSpan; }
	float GetMaxLifeSpan() { return mfMaxLifeSpan; }
	ePEDeathType GetDeathType() { return mDeathType; }
	const tString& GetDeathPS() { return msDeathPS; }

	eEditorParticleEmitterType GetDrawType() { return mDrawType; }
	bool GetMultiplyRGBWithAlpha() { return mbMultiplyRGBWithAlpha; }
	const cVector2f& GetMinStartSize() { return mvMinStartSize; }
	const cVector2f& GetMaxStartSize() { return mvMaxStartSize; }
	float GetStartRelSize() { return mfStartRelSize; }
	float GetMiddleRelSize() { return mfMiddleRelSize; }
	float GetEndRelSize() { return mfEndRelSize; }
	float GetMiddleRelSizeTime() { return mfMiddleRelSizeTime; }
	float GetMiddleRelSizeLength() { return mfMiddleRelSizeLength; }
	
	const cColor& GetMinStartColor() { return mMinStartColor; }
	const cColor& GetMaxStartColor() { return mMaxStartColor; }
	const cColor& GetStartRelColor() { return mStartRelColor; }
	const cColor& GetMiddleRelColor() { return mMiddleRelColor; }
	const cColor& GetEndRelColor() { return mEndRelColor; }
	float GetMiddleRelColorTime() { return mfMiddleRelColorTime; }
	float GetMiddleRelColorLength() { return mfMiddleRelColorLength; }
	
	bool GetCollides() { return mbCollides; }
	float GetMinCollisionMax() { return mfMinCollisionMax; }
	float GetMaxCollisionMax() { return mfMaxCollisionMax; }
	float GetMinBounceAmount() { return mfMinBounceAmount; }
	float GetMaxBounceAmount() { return mfMaxBounceAmount; }
	int GetCollisionUpdateRate() { return mlCollisionUpdateRate; }
	
	bool GetUseBeamNoise() { return mbUseBeamNoise; }
	int GetLowFreqPoints() { return mlLowFreqPoints; }
	const cVector3f& GetMinLFNoise() { return mvMinLFNoise; }
	const cVector3f& GetMaxLFNoise() { return mvMaxLFNoise; }
	int GetHighFreqPoints() { return mlHighFreqPoints; }
	const cVector3f& GetMinHFNoise() { return mvMinHFNoise; }
	const cVector3f& GetMaxHFNoise() { return mvMaxHFNoise; }
	bool GetApplyNoiseToStart() { return mbApplyNoiseToStart; }
	bool GetApplyNoiseToEnd() { return mbApplyNoiseToEnd; }

	const tString& GetMaterial() { return msMaterial; }
	int GetNumFiles() { return mlNumFiles; }
	float GetAnimLength() { return mfAnimLength; }

	const cVector2f& GetSubDivAmount() { return mvSubDivAmount; }
	ePESubDivType GetSubDivisionType() { return mSubDivType; }

protected:
	bool OnCreate() { return true; }
	void OnPostDeployAll(bool){}

	void OnUpdate(bool) {}

	bool mbRespawn;
	ePEType mPEType;
	int mlMaxParticleNum;
	float mfParticlesPerSecond;
	float mfStartTimeOffset;
	float mfWarmUpTime;
	int mlWarmUpStepsPerSec;

	float mfMinPauseLength;
	float mfMaxPauseLength;
	float mfMinPauseInterval;
	float mfMaxPauseInterval;

	cVector3f mvPosOffset;
	cVector3f mvAngleOffset;

	ePEStartPosType mStartPosType;
	cVector3f mvMinStartPos;
	cVector3f mvMaxStartPos;

	float mfMinStartRadius;
	float mfMaxStartRadius;

	cVector2f mvMinStartAngles;
	cVector2f mvMaxStartAngles;

	ePEStartPosType mStartVelType;

	cVector3f mvMinStartVel;
	cVector3f mvMaxStartVel;

	cVector2f mvMinStartVelAngles;
	cVector2f mvMaxStartVelAngles;

	float mfMinStartVelSpeed;
	float mfMaxStartVelSpeed;

	float mfMinSpeedMultiply;
	float mfMaxSpeedMultiply;

	cVector3f mvMinStartAcc;
	cVector3f mvMaxStartAcc;

	float mfMinVelMaximum;
	float mfMaxVelMaximum;

	bool mbUsesDirection;
	ePEGravityType mGravityType;

	cVector3f mvGravityAcc;

	eParticleEmitterCoordSystem mCoordSystem;

	bool mbUsePartSpin;
	ePEPartSpinType mPartSpinType;
	float mfMinSpinRange;
	float mfMaxSpinRange;

	bool mbUseRevolution;
	cVector3f mvMinRevVel;
	cVector3f mvMaxRevVel;

	float mfMinLifeSpan;
	float mfMaxLifeSpan;

	ePEDeathType mDeathType;

	tString msDeathPS;

	eEditorParticleEmitterType mDrawType;

	bool mbMultiplyRGBWithAlpha;

	cVector2f mvMinStartSize;
	cVector2f mvMaxStartSize;

	float mfStartRelSize;
	float mfMiddleRelSize;
	float mfEndRelSize;
	float mfMiddleRelSizeTime;
	float mfMiddleRelSizeLength;

	cColor mMinStartColor;
	cColor mMaxStartColor;

	cColor mStartRelColor;
	cColor mMiddleRelColor;
	cColor mEndRelColor;
	float mfMiddleRelColorTime;
	float mfMiddleRelColorLength;

	bool mbCollides;

	float mfMinCollisionMax;
	float mfMaxCollisionMax;

	float mfMinBounceAmount;
	float mfMaxBounceAmount;

	int mlCollisionUpdateRate;

	bool mbUseBeamNoise;

	int mlLowFreqPoints;
	cVector3f mvMinLFNoise;
	cVector3f mvMaxLFNoise;

	int mlHighFreqPoints;
	cVector3f mvMinHFNoise;
	cVector3f mvMaxHFNoise;

	bool mbApplyNoiseToStart;
	bool mbApplyNoiseToEnd;

	tString msMaterial;
	int mlNumFiles;
	float mfAnimLength;

	cVector2f mvSubDivAmount;
	ePESubDivType mSubDivType; 
};

#endif // ED_OBJ_PARTICLE_EMITTER_H

