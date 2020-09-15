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

#include "EntityWrapperParticleEmitter.h"

#include "ParticleEditorWorld.h"
#include "ParticleEditor.h"

#include "../common/EditorHelper.h"

//----------------------------------------------------------------------

static ePEType ToPEType(const char *apString)
{
	if(apString==NULL) return ePEType_Normal;
	tString sType = cString::ToLowerCase(apString);

	if(sType == "beam") return ePEType_Beam;
	else if(sType == "normal") return ePEType_Normal;

	return ePEType_Normal;
}

static tString PETypeToString(ePEType aType)
{
	switch(aType)
	{
	case ePEType_Normal:
		return "Normal";
	case ePEType_Beam:
		return "Beam";
	}

	return "Normal";
}

	// ---

	/////////////////////////

static ePEStartPosType ToStartPosType(const char *apString)
{
	if(apString==NULL) return ePEStartPosType_Box;

	tString sType = cString::ToLowerCase(apString);
	
    if(sType == "box") return ePEStartPosType_Box;
	else if(sType == "sphere") return ePEStartPosType_Sphere;

	return ePEStartPosType_Box;
}

static tString StartPosTypeToString(ePEStartPosType aType)
{
	switch(aType)
	{
	case ePEStartPosType_Box:
		return "Box";
	case ePEStartPosType_Sphere:
		return "Sphere";
	}

	return "Box";
}

	/////////////////////////
	
static eParticleEmitterCoordSystem ToCoordSystem(const char *apString)
{
	if(apString==NULL) return eParticleEmitterCoordSystem_World;

	tString sType = cString::ToLowerCase(apString);

	if(sType=="world") return eParticleEmitterCoordSystem_World;
	else if(sType=="local") return eParticleEmitterCoordSystem_Local;
	
	return eParticleEmitterCoordSystem_World;
}

static tString CoordSystemToString(eParticleEmitterCoordSystem aType)
{
	switch(aType)
	{
	case eParticleEmitterCoordSystem_World:
		return "World";
	case eParticleEmitterCoordSystem_Local:
		return "Local";
	}

	return "World";
}


	/////////////////////////

static ePEDeathType ToDeathType(const char *apString)
{
	if(apString==NULL) return ePEDeathType_Age;

	tString sType = cString::ToLowerCase(apString);

	if(sType=="age") return ePEDeathType_Age;

	return ePEDeathType_Age;
}

static tString DeathTypeToString(ePEDeathType aType)
{
	switch(aType)
	{
	case ePEDeathType_Age:
		return "Age";
	}

	return "Age";
}

	/////////////////////////

static eEditorParticleEmitterType ToDrawType(const char *apString)
{
	if(apString==NULL) return eEditorParticleEmitterType_Point;

	tString sType = cString::ToLowerCase(apString);

	if(sType == "point") return eEditorParticleEmitterType_Point;
	else if(sType == "line") return eEditorParticleEmitterType_Line;
	else if(sType == "axis") return eEditorParticleEmitterType_Axis;

	return eEditorParticleEmitterType_Point;
}

static tString DrawTypeToString(eEditorParticleEmitterType aType)
{
	switch(aType)
	{
	case eEditorParticleEmitterType_Point:
		return "Point";
	case eEditorParticleEmitterType_Line:
		return "Line";
	case eEditorParticleEmitterType_Axis:
		return "Axis";
	}

	return "Point";
}
	
	/////////////////////////

static ePEGravityType ToGravityType(const char *apString)
{
	if(apString==NULL) return ePEGravityType_None;

	tString sType = cString::ToLowerCase(apString);

	if(sType == "none") return ePEGravityType_None;
	else if(sType == "vector") return ePEGravityType_Vector;
	else if(sType == "center") return ePEGravityType_Center;

	return ePEGravityType_None;
}

static tString GravityTypeToString(ePEGravityType aType)
{
	switch(aType)
	{
	case ePEGravityType_None:
		return "None";
	case ePEGravityType_Vector:
		return "Vector";
	case ePEGravityType_Center:
		return "Center";
	}

	return "None";
}

	/////////////////////////

static ePESubDivType ToSubDivType(const char *apString)
{
	if(apString==NULL) return ePESubDivType_Random;

	tString sType = cString::ToLowerCase(apString);

	if(sType == "random") return ePESubDivType_Random;
	else if(sType == "animation") return ePESubDivType_Animation;
	
	return ePESubDivType_Random;
}

static tString SubDivTypeToString(ePESubDivType aType)
{
	switch(aType)
	{
	case ePESubDivType_Random:
		return "Random";
	case ePESubDivType_Animation:
		return "Animation";
	}

	return "Random";
}

	////////////////////////
	/// NEW

static ePEPartSpinType ToPartSpinType(const char *apString)
{
	if (apString==NULL) return ePEPartSpinType_Constant;

	tString sType = cString::ToLowerCase(apString);

	if (sType == "constant") return ePEPartSpinType_Constant;
	else if (sType == "movement") return ePEPartSpinType_Movement;

	return ePEPartSpinType_Constant;
}

static tString PartSpinTypeToString(ePEPartSpinType aType)
{
	switch(aType)
	{
	case ePEPartSpinType_Constant:
		return "Constant";
	case ePEPartSpinType_Movement:
		return "Movement";
	}

	return "Constant";
}

//----------------------------------------------------------------------

	/*

		tStringVec vPETypes;
	vPETypes.push_back("Normal");
	vPETypes.push_back("Beam");

	tStringVec vStartPosTypes;
	vStartPosTypes.push_back("Box");
	vStartPosTypes.push_back("Sphere");

	tStringVec vCoordSystems;
	vCoordSystems.push_back("World");
	vCoordSystems.push_back("Local");

	tStringVec vDrawTypes;
	vDrawTypes.push_back("Point");
	vDrawTypes.push_back("Line");
	vDrawTypes.push_back("Axis");

	tStringVec vGravTypes;
	vGravTypes.push_back("None");
	vGravTypes.push_back("Vector");
	vGravTypes.push_back("Center");

	tStringVec vSpinTypes;
	vSpinTypes.push_back("Constant");
	vSpinTypes.push_back("Movement");

	tStringVec vDeathTypes;
	vDeathTypes.push_back("Age");

	tStringVec vSubDivTypes;
	vSubDivTypes.push_back("Random");
	vSubDivTypes.push_back("Animation");*/

	cEntityWrapperTypeParticleEmitter::cEntityWrapperTypeParticleEmitter() : iEntityWrapperType(eEditorEntityType_ParticleEmitter, _W("ParticleEmitter"), "ParticleEmitter")
{
	GetPropInt(eObjInt_ID)->SetSaved(false);
	GetPropBool(eObjBool_Active)->SetSaved(false);
	GetPropString(eObjStr_Tag)->SetSaved(false);
	GetPropVec3f(eObjVec3f_Position)->SetSaved(false);
	GetPropVec3f(eObjVec3f_Rotation)->SetSaved(false);
	GetPropVec3f(eObjVec3f_Scale)->SetSaved(false);

	AddString(ePEStr_PEType, "PEType", PETypeToString(ePEType_Normal));
	//AddInt(ePEInt_PEType, "DrawType", 0, ePropCopyStep_PreEnt);
	AddInt(ePEInt_MaxParticleNum, "MaxParticleNum", 10, ePropCopyStep_PreEnt);
	AddFloat(ePEFloat_ParticlesPerSecond, "ParticlesPerSecond", 1);

	AddFloat(ePEFloat_StartTimeOffset, "StartTimeOffset", 0);
	AddBool(ePEBool_Respawn, "Respawn");
	
	AddFloat(ePEFloat_WarmUpTime, "WarmUpTime");
	AddInt(ePEInt_WarmUpStepsPerSec, "WarmUpStepsPerSec", 60);
	
	AddFloat(ePEFloat_MinPauseLength, "MinPauseLength");
	AddFloat(ePEFloat_MaxPauseLength, "MaxPauseLength");
	AddFloat(ePEFloat_MinPauseInterval, "MinPauseInterval");
	AddFloat(ePEFloat_MaxPauseInterval, "MaxPauseInterval");

	AddVec3f(ePEVec3f_PosOffset, "PosOffset");
	AddVec3f(ePEVec3f_AngleOffset, "AngleOffset");

	AddString(ePEStr_StartPosType, "StartPosType", StartPosTypeToString(ePEStartPosType_Box));
	AddVec3f(ePEVec3f_MinStartPos, "MinStartPos");
	AddVec3f(ePEVec3f_MaxStartPos, "MaxStartPos");

	AddFloat(ePEFloat_MinStartRadius, "MinStartRadius");
	AddFloat(ePEFloat_MaxStartRadius, "MaxStartRadius", 1);

	AddVec2f(ePEVec2f_MinStartAngles, "MinStartAngles");
	AddVec2f(ePEVec2f_MaxStartAngles, "MaxStartAngles");


	AddString(ePEStr_StartVelType, "StartVelType", StartPosTypeToString(ePEStartPosType_Box));
	AddString(ePEStr_CoordSystem, "CoordSystem", CoordSystemToString(eParticleEmitterCoordSystem_World));
	AddVec3f(ePEVec3f_MinStartVel, "MinStartVel");
	AddVec3f(ePEVec3f_MaxStartVel, "MaxStartVel");

	AddVec2f(ePEVec2f_MinStartVelAngles, "MinStartVelAngles", -180);
	AddVec2f(ePEVec2f_MaxStartVelAngles, "MaxStartVelAngles", 180);

    AddFloat(ePEFloat_MinStartVelSpeed, "MinStartVelSpeed");
	AddFloat(ePEFloat_MaxStartVelSpeed, "MaxStartVelSpeed", 1);

	AddFloat(ePEFloat_MinSpeedMultiply, "MinSpeedMultiply", 1);
	AddFloat(ePEFloat_MaxSpeedMultiply, "MaxSpeedMultiply", 1);

	AddVec3f(ePEVec3f_MinStartAcc, "MinStartAcc");
	AddVec3f(ePEVec3f_MaxStartAcc, "MaxStartAcc");

	AddFloat(ePEFloat_MinVelMaximum, "MinVelMaximum");
	AddFloat(ePEFloat_MaxVelMaximum, "MaxVelMaximum");

	AddBool(ePEBool_UseDirection, "UsesDirection", false);

	// Add gravity type
	AddString(ePEStr_GravityType, "GravityType", GravityTypeToString(ePEGravityType_None));
	AddVec3f(ePEVec3f_GravityAcc, "GravityAcc");

	AddBool(ePEBool_UsePartSpin, "UsePartSpin", false);
	AddString(ePEStr_PartSpinType, "PartSpinType", PartSpinTypeToString(ePEPartSpinType_Constant));
	AddFloat(ePEFloat_MinSpinRange, "MinSpinRange");
	AddFloat(ePEFloat_MaxSpinRange, "MaxSpinRange");

	AddBool(ePEBool_UseRevolution, "UseRevolution", false);
	AddVec3f(ePEVec3f_MinRevVel, "MinRevVel");
	AddVec3f(ePEVec3f_MaxRevVel, "MaxRevVel");

	AddFloat(ePEFloat_MinLifeSpan, "MinLifeSpan", 10);
	AddFloat(ePEFloat_MaxLifeSpan, "MaxLifeSpan", 10);
	AddString(ePEStr_DeathType,"DeathType", DeathTypeToString(ePEDeathType_Age));
	AddString(ePEStr_DeathPS, "DeathPS");

	AddString(ePEStr_DrawType, "DrawType", DrawTypeToString(eEditorParticleEmitterType_Point));
	AddBool(ePEBool_MultiplyRGBWithAlpha, "MultiplyRGBWithAlpha", false);
	AddVec2f(ePEVec2f_MinStartSize, "MinStartSize", 1);
	AddVec2f(ePEVec2f_MaxStartSize, "MaxStartSize", 1);

	AddFloat(ePEFloat_StartRelSize, "StartRelSize", 1);
	AddFloat(ePEFloat_MiddleRelSize, "MiddleRelSize", 1);
	AddFloat(ePEFloat_EndRelSize, "EndRelSize", 1);
	AddFloat(ePEFloat_MiddleRelSizeTime, "MiddleRelSizeTime");
	AddFloat(ePEFloat_MiddleRelSizeLength, "MiddleRelSizeLength");

	AddColor(ePECol_MinStartColor, "MinStartColor");
	AddColor(ePECol_MaxStartColor, "MaxStartColor");
	AddColor(ePECol_StartRelColor, "StartRelColor");
	AddColor(ePECol_MiddleRelColor, "MiddleRelColor");
	AddColor(ePECol_EndRelColor, "EndRelColor");
	AddFloat(ePEFloat_MiddleRelColorTime, "MiddleRelColorTime");
	AddFloat(ePEFloat_MiddleRelColorLength, "MiddleRelColorLength");

	AddBool(ePEBool_Collides, "Collides", false);
	AddFloat(ePEFloat_MinCollisionMax, "MinCollisionMax", 10);
	AddFloat(ePEFloat_MaxCollisionMax, "MaxCollisionMax", 10);
	AddFloat(ePEFloat_MinBounceAmount, "MinBounceAmount", 0.6f);
	AddFloat(ePEFloat_MaxBounceAmount, "MaxBounceAmount", 0.6f);
	AddInt(ePEInt_CollisionUpdateRate, "CollisionUpdateRate", 10);

	AddBool(ePEBool_UseBeamNoise, "UseBeamNoise", false);
	AddInt(ePEInt_LowFreqPoints, "LowFreqPoints", 2);
	AddVec3f(ePEVec3f_MinLFNoise, "MinLFNoise");
	AddVec3f(ePEVec3f_MaxLFNoise, "MaxLFNoise");
	AddInt(ePEInt_HighFreqPoints, "HighFreqPoints", 2);
	AddVec3f(ePEVec3f_MinHFNoise, "MinHFNoise");
	AddVec3f(ePEVec3f_MaxHFNoise, "MaxHFNoise");
	AddBool(ePEBool_ApplyNoiseToStart, "ApplyNoiseToStart", false);
	AddBool(ePEBool_ApplyNoiseToEnd, "ApplyNoiseToEnd", false);

	AddString(ePEStr_Material, "Material");
	AddInt(ePEInt_NumFiles, "MaterialNum", 1);
	AddFloat(ePEFloat_AnimLength, "AnimationLength", 1);
	AddVec2f(ePEVec2f_SubDivAmount, "SubDiv");
	AddString(ePEStr_SubDivType, "SubDivType", "Random");


		/*

	//////////////////////////////////
	// Beam
	apElement->SetAttributeBool("UseBeamNoise", mbUseBeamNoise);
	apElement->SetAttributeInt("LowFreqPoints", mlLowFreqPoints);
	apElement->SetAttributeVector3f("MinLFNoise", mvMinLFNoise);
	apElement->SetAttributeVector3f("MaxLFNoise", mvMaxLFNoise);
	apElement->SetAttributeInt("HighFreqPoints", mlHighFreqPoints);
	apElement->SetAttributeVector3f("MinHFNoise", mvMinHFNoise);
	apElement->SetAttributeVector3f("MaxHFNoise", mvMaxHFNoise);
	apElement->SetAttributeBool("ApplyNoiseToStart", mbApplyNoiseToStart);
	apElement->SetAttributeBool("ApplyNoiseToEnd", mbApplyNoiseToEnd);

	//////////////////////////////////
	// Material
	apElement->SetAttributeString("Material", msMaterial);
	apElement->SetAttributeInt("MaterialNum", mlNumFiles);
	apElement->SetAttributeFloat("AnimationLength", mfAnimLength);

	apElement->SetAttributeVector2f("SubDiv", mvSubDivAmount);
	apElement->SetAttributeString("SubDivType", vSubDivTypes[mSubDivType]);

	*/
}

iEntityWrapperData* cEntityWrapperTypeParticleEmitter::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataParticleEmitter,(this));
}

//----------------------------------------------------------------------

cEntityWrapperDataParticleEmitter::cEntityWrapperDataParticleEmitter(iEntityWrapperType* apType) : iEntityWrapperData(apType)
{
	/*
	mPEType = ePEType_Normal;
	mlMaxParticleNum = 10;
	mfParticlesPerSecond = 1;

	mfStartTimeOffset = 0;

	mbRespawn = true;

	mfWarmUpTime = 0;
	mlWarmUpStepsPerSec = 60;

	mfMinPauseLength = 0;
	mfMaxPauseLength = 0;
	mfMinPauseInterval = 0;
	mfMaxPauseInterval = 0;

	mvPosOffset = 0;
	mvAngleOffset = 0;


	mStartPosType = ePEStartPosType_Box;

	mvMinStartPos = 0;
	mvMaxStartPos = 0;

	mfMinStartRadius = 0;
	mfMaxStartRadius = 1;

	mvMinStartAngles = cVector2f(-180,-180);
	mvMaxStartAngles = cVector2f(180,180);


	mStartVelType = ePEStartPosType_Box;
	mCoordSystem = eParticleEmitterCoordSystem_World;

	mvMinStartVel = 0;
	mvMaxStartVel = 0;

	mvMinStartVelAngles = -180;
	mvMaxStartVelAngles = 180;

	mfMinStartVelSpeed = 0;
	mfMaxStartVelSpeed = 1;

	mfMinSpeedMultiply = 1;
	mfMaxSpeedMultiply = 1;

	mvMinStartAcc = 0;
	mvMaxStartAcc = 0;

	mfMinVelMaximum = 0;
	mfMaxVelMaximum = 0;
	
	mbUsesDirection = false;

	mGravityType = ePEGravityType_None;
	mvGravityAcc = 0;
	
	mbUsePartSpin = false;
	mPartSpinType = ePEPartSpinType_Constant;
	mfMinSpinRange = 0;
	mfMaxSpinRange = 0;

	mbUseRevolution = false;
	mvMinRevVel = 0;
	mvMaxRevVel = 0;

	mfMinLifeSpan = 10;
	mfMaxLifeSpan = 10;
	mDeathType = ePEDeathType_Age;
	msDeathPS = "";

	mDrawType = eEditorParticleEmitterType_Point;
	mbMultiplyRGBWithAlpha = false;
	mvMinStartSize = 1;
	mvMaxStartSize = 1;

	mfStartRelSize = 1;
	mfMiddleRelSize = 1;
	mfEndRelSize = 1;
	mfMiddleRelSizeTime = 0;
	mfMiddleRelSizeLength = 0;
	
	mMinStartColor = 1;
	mMaxStartColor = 1;
	mStartRelColor = 1;
	mMiddleRelColor = 1;
	mEndRelColor = 1;
	mfMiddleRelColorTime = 0;
	mfMiddleRelColorLength = 0;

	mbCollides = false;
	mfMinCollisionMax = 10;
	mfMaxCollisionMax = 10;
	mfMinBounceAmount = 0.6f;
	mfMaxBounceAmount = 0.6f;
	mlCollisionUpdateRate = 10;

	mbUseBeamNoise = false;
	mlLowFreqPoints = 2;
	mvMinLFNoise = 0;
	mvMaxLFNoise = 0;
	mlHighFreqPoints = 2;
	mvMinHFNoise = 0;
	mvMaxHFNoise = 0;
	mbApplyNoiseToStart = false;
	mbApplyNoiseToEnd = false;

	msMaterial = "";
	mlNumFiles = 1;
	mfAnimLength = 1;

	mvSubDivAmount = 0;
	mSubDivType = ePESubDivType_Random;*/
}

//----------------------------------------------------------------------

/*
void cEntityWrapperDataParticleEmitter::CopyFromEntity(iEntityWrapper* apEntity)
{
	iEntityWrapperData::CopyFromEntity(apEntity);
	cEntityWrapperParticleEmitter* pEnt = (cEntityWrapperParticleEmitter*)apEntity;

	////////////////////////////////////////////////////
	// General
	mPEType = pEnt->GetPEType();
	mlMaxParticleNum = pEnt->GetMaxParticleNum();
	mfParticlesPerSecond = pEnt->GetParticlesPerSecond();

	mfStartTimeOffset = pEnt->GetStartTimeOffset();

	mbRespawn = pEnt->GetRespawn();

	mfWarmUpTime = pEnt->GetWarmUpTime();
	mlWarmUpStepsPerSec = pEnt->GetWarmUpStepsPerSec();

	mfMinPauseLength = pEnt->GetMinPauseLength();
	mfMaxPauseLength = pEnt->GetMaxPauseLength();

	mfMinPauseInterval = pEnt->GetMinPauseInterval();
	mfMaxPauseInterval = pEnt->GetMaxPauseInterval();

	mvPosOffset = pEnt->GetPosOffset();
	mvAngleOffset = pEnt->GetAngleOffset();

	////////////////////////////////////////////////////
	// Start
	mStartPosType = pEnt->GetStartPosType();

	mvMinStartPos = pEnt->GetMinStartPos();
	mvMaxStartPos = pEnt->GetMaxStartPos();

	mfMinStartRadius = pEnt->GetMinStartRadius();
	mfMaxStartRadius = pEnt->GetMaxStartRadius();

	mvMinStartAngles = pEnt->GetMinStartAngles();
	mvMaxStartAngles = pEnt->GetMaxStartAngles();

	////////////////////////////////////////////////////
	// Movement
	mStartVelType = pEnt->GetStartVelType();
	mCoordSystem = pEnt->GetCoordSystem();

	mbUsesDirection = pEnt->GetUsesDirection();

	mGravityType = pEnt->GetGravityType();
	mvGravityAcc = pEnt->GetGravityAcc();

	mvMinStartVel = pEnt->GetMinStartVel();
	mvMaxStartVel = pEnt->GetMaxStartVel();

	mvMinStartVelAngles = pEnt->GetMinStartVelAngles();
	mvMaxStartVelAngles = pEnt->GetMaxStartVelAngles();

	mfMinStartVelSpeed = pEnt->GetMinStartVelSpeed();
	mfMaxStartVelSpeed = pEnt->GetMaxStartVelSpeed();

	mfMinSpeedMultiply = pEnt->GetMinSpeedMultiply();
	mfMaxSpeedMultiply = pEnt->GetMaxSpeedMultiply();

	mvMinStartAcc = pEnt->GetMinStartAcc();
	mvMaxStartAcc = pEnt->GetMaxStartAcc();

	mfMinVelMaximum = pEnt->GetMinVelMaximum();
	mfMaxVelMaximum = pEnt->GetMaxVelMaximum();

	
	////////////////////////////////////////////////////
	// Rotation
	mbUsePartSpin = pEnt->GetUsePartSpin();
	mPartSpinType = pEnt->GetPartSpinType();
	mfMinSpinRange = pEnt->GetMinSpinRange();
	mfMaxSpinRange = pEnt->GetMaxSpinRange();

	mbUseRevolution = pEnt->GetUseRevolution();
	mvMinRevVel = pEnt->GetMinRevVel();
	mvMaxRevVel = pEnt->GetMaxRevVel();

	////////////////////////////////////////////////////
	// Life
	mfMinLifeSpan = pEnt->GetMinLifeSpan();
	mfMaxLifeSpan = pEnt->GetMaxLifeSpan();
	mDeathType = pEnt->GetDeathType();
	msDeathPS = pEnt->GetDeathPS();

	////////////////////////////////////////////////////
	// Rendering
	mDrawType = pEnt->GetDrawType();
	mbMultiplyRGBWithAlpha = pEnt->GetMultiplyRGBWithAlpha();
	mvMinStartSize = pEnt->GetMinStartSize();
	mvMaxStartSize = pEnt->GetMaxStartSize();

	mfStartRelSize = pEnt->GetStartRelSize();
	mfMiddleRelSize = pEnt->GetMiddleRelSize();
	mfEndRelSize = pEnt->GetEndRelSize();
	mfMiddleRelSizeTime = pEnt->GetMiddleRelSizeTime();
	mfMiddleRelSizeLength = pEnt->GetMiddleRelSizeLength();
	
	////////////////////////////////////////////////////
	// Color
	mMinStartColor = pEnt->GetMinStartColor();
	mMaxStartColor = pEnt->GetMaxStartColor();
	mStartRelColor = pEnt->GetStartRelColor();
	mMiddleRelColor = pEnt->GetMiddleRelColor();
	mEndRelColor = pEnt->GetEndRelColor();
	mfMiddleRelColorTime = pEnt->GetMiddleRelColorTime();
	mfMiddleRelColorLength = pEnt->GetMiddleRelColorLength();

	////////////////////////////////////////////////////
	// Collision
	mbCollides = pEnt->GetCollides();
	mfMinCollisionMax = pEnt->GetMinCollisionMax();
	mfMaxCollisionMax = pEnt->GetMaxCollisionMax();
	mfMinBounceAmount = pEnt->GetMinBounceAmount();
	mfMaxBounceAmount = pEnt->GetMaxBounceAmount();
	mlCollisionUpdateRate = pEnt->GetCollisionUpdateRate();

	////////////////////////////////////////////////////
	// Beam
	mbUseBeamNoise = pEnt->GetUseBeamNoise();
	mlLowFreqPoints = pEnt->GetLowFreqPoints();
	mvMinLFNoise = pEnt->GetMinLFNoise();
	mvMaxLFNoise = pEnt->GetMaxLFNoise();
	mlHighFreqPoints = pEnt->GetHighFreqPoints();
	mvMinHFNoise = pEnt->GetMinHFNoise();
	mvMaxHFNoise = pEnt->GetMaxHFNoise();
	mbApplyNoiseToStart = pEnt->GetApplyNoiseToStart();
	mbApplyNoiseToEnd = pEnt->GetApplyNoiseToEnd();

	////////////////////////////////////////////////////
	// Material
	msMaterial = pEnt->GetMaterial();
	mlNumFiles = pEnt->GetNumFiles();
	mfAnimLength = pEnt->GetAnimLength();

	mvSubDivAmount = pEnt->GetSubDivAmount();
	mSubDivType = pEnt->GetSubDivisionType();
}
^*/
//----------------------------------------------------------------------
/*
void cEntityWrapperDataParticleEmitter::CopyToEntity(iEntityWrapper* apEntity)
{
	iEntityWrapperData::CopyToEntity(apEntity);
	cEntityWrapperParticleEmitter* pEnt = (cEntityWrapperParticleEmitter*)apEntity;

	pEnt->SetPEType(mPEType);
	pEnt->SetMaxParticleNum(mlMaxParticleNum);
	pEnt->SetParticlesPerSecond(mfParticlesPerSecond);

	pEnt->SetStartTimeOffset(mfStartTimeOffset);

	pEnt->SetRespawn(mbRespawn);

	pEnt->SetWarmUpTime(mfWarmUpTime);
	pEnt->SetWarmUpStepsPerSec(mlWarmUpStepsPerSec);

	pEnt->SetMinPauseLength(mfMinPauseLength);
	pEnt->SetMaxPauseLength(mfMaxPauseLength);
	pEnt->SetMinPauseInterval(mfMinPauseInterval);
	pEnt->SetMaxPauseInterval(mfMaxPauseInterval);

	pEnt->SetPosOffset(mvPosOffset);
	pEnt->SetAngleOffset(mvAngleOffset);

	pEnt->SetStartPosType(mStartPosType);
	pEnt->SetMinStartPos(mvMinStartPos);
	pEnt->SetMaxStartPos(mvMaxStartPos);
	pEnt->SetMinStartRadius(mfMinStartRadius);
	pEnt->SetMaxStartRadius(mfMaxStartRadius);
	pEnt->SetMinStartAngles(mvMinStartAngles);
	pEnt->SetMaxStartAngles(mvMaxStartAngles);

	pEnt->SetStartVelType(mStartVelType);
	pEnt->SetCoordSystem(mCoordSystem);
	pEnt->SetMinStartVel(mvMinStartVel);
	pEnt->SetMaxStartVel(mvMaxStartVel);
	pEnt->SetMinStartVelAngles(mvMinStartVelAngles);
	pEnt->SetMaxStartVelAngles(mvMaxStartVelAngles);
	pEnt->SetMinStartVelSpeed(mfMinStartVelSpeed);
	pEnt->SetMaxStartVelSpeed(mfMaxStartVelSpeed);
	pEnt->SetMinSpeedMultiply(mfMinSpeedMultiply);
	pEnt->SetMaxSpeedMultiply(mfMaxSpeedMultiply);
	pEnt->SetMinStartAcc(mvMinStartAcc);
	pEnt->SetMaxStartAcc(mvMaxStartAcc);
	pEnt->SetMinVelMaximum(mfMinVelMaximum);
	pEnt->SetMaxVelMaximum(mfMaxVelMaximum);
	
	pEnt->SetUsesDirection(mbUsesDirection);

	pEnt->SetGravityType(mGravityType);
	pEnt->SetGravityAcc(mvGravityAcc);
	
	pEnt->SetUsePartSpin(mbUsePartSpin);
	pEnt->SetPartSpinType(mPartSpinType);
	pEnt->SetMinSpinRange(mfMinSpinRange);
	pEnt->SetMaxSpinRange(mfMaxSpinRange);

	pEnt->SetUseRevolution(mbUseRevolution);
	pEnt->SetMinRevVel(mvMinRevVel);
	pEnt->SetMaxRevVel(mvMaxRevVel);

	pEnt->SetMinLifeSpan(mfMinLifeSpan);
	pEnt->SetMaxLifeSpan(mfMaxLifeSpan);
	pEnt->SetDeathType(mDeathType);
	pEnt->SetDeathPS(msDeathPS);

	pEnt->SetDrawType(mDrawType);
	pEnt->SetMultiplyRGBWithAlpha(mbMultiplyRGBWithAlpha);
	pEnt->SetMinStartSize(mvMinStartSize);
	pEnt->SetMaxStartSize(mvMaxStartSize);

	pEnt->SetStartRelSize(mfStartRelSize);
	pEnt->SetMiddleRelSize(mfMiddleRelSize);
	pEnt->SetEndRelSize(mfEndRelSize);
	pEnt->SetMiddleRelSizeTime(mfMiddleRelSizeTime);
	pEnt->SetMiddleRelSizeLength(mfMiddleRelSizeLength);
	
	pEnt->SetMinStartColor(mMinStartColor);
	pEnt->SetMaxStartColor(mMaxStartColor);
	pEnt->SetStartRelColor(mStartRelColor);
	pEnt->SetMiddleRelColor(mMiddleRelColor);
	pEnt->SetEndRelColor(mEndRelColor);
	pEnt->SetMiddleRelColorTime(mfMiddleRelColorTime);
	pEnt->SetMiddleRelColorLength(mfMiddleRelColorLength);

	pEnt->SetCollides(mbCollides);
	pEnt->SetMinCollisionMax(mfMinCollisionMax);
	pEnt->SetMaxCollisionMax(mfMaxCollisionMax);
	pEnt->SetMinBounceAmount(mfMinBounceAmount);
	pEnt->SetMaxBounceAmount(mfMaxBounceAmount);
	pEnt->SetCollisionUpdateRate(mlCollisionUpdateRate);

	pEnt->SetUseBeamNoise(mbUseBeamNoise);
	pEnt->SetLowFreqPoints(mlLowFreqPoints);
	pEnt->SetMinLFNoise(mvMinLFNoise);
	pEnt->SetMaxLFNoise(mvMaxLFNoise);
	pEnt->SetHighFreqPoints(mlHighFreqPoints);
	pEnt->SetMinHFNoise(mvMinHFNoise);
	pEnt->SetMaxHFNoise(mvMaxHFNoise);
	pEnt->SetApplyNoiseToStart(mbApplyNoiseToStart);
	pEnt->SetApplyNoiseToEnd(mbApplyNoiseToEnd);

	pEnt->SetMaterial(msMaterial);
	pEnt->SetNumFiles(mlNumFiles);
	pEnt->SetAnimLength(mfAnimLength);

	pEnt->SetSubDivAmount(mvSubDivAmount);
	pEnt->SetSubDivisionType(mSubDivType);
}
*/
//----------------------------------------------------------------------
/*
void cEntityWrapperDataParticleEmitter::Load(cXmlElement* apElement)
{
	iEntityWrapperData::Load(apElement);

	//////////////////////////////////
	// General
	mPEType =  GetPEType(apElement->GetAttributeString("PEType").c_str());
	mlMaxParticleNum = apElement->GetAttributeInt("MaxParticleNum");
	mfParticlesPerSecond = apElement->GetAttributeFloat("ParticlesPerSecond");

	mfStartTimeOffset = apElement->GetAttributeFloat("StartTimeOffset");

	mbRespawn = apElement->GetAttributeBool("Respawn");

	mfWarmUpTime = apElement->GetAttributeFloat("WarmUpTime");
	mlWarmUpStepsPerSec = apElement->GetAttributeInt("WarmUpStepsPerSec");

	mfMinPauseLength = apElement->GetAttributeFloat("MinPauseLength");
	mfMaxPauseLength = apElement->GetAttributeFloat("MaxPauseLength");
	mfMinPauseInterval = apElement->GetAttributeFloat("MinPauseInterval");
	mfMaxPauseInterval = apElement->GetAttributeFloat("MaxPauseInterval");

	mvPosOffset = apElement->GetAttributeVector3f("PosOffset");
	mvAngleOffset = apElement->GetAttributeVector3f("AngleOffset");

	//////////////////////////////////
	// Start
	mStartPosType = GetStartPosType(apElement->GetAttributeString("StartPosType").c_str());
	mvMinStartPos = apElement->GetAttributeVector3f("MinStartPos");
	mvMaxStartPos = apElement->GetAttributeVector3f("MaxStartPos");
	mfMinStartRadius = apElement->GetAttributeFloat("MinStartRadius");
	mfMaxStartRadius = apElement->GetAttributeFloat("MaxStartRadius");
	mvMinStartAngles = apElement->GetAttributeVector2f("MinStartAngles");
	mvMaxStartAngles = apElement->GetAttributeVector2f("MaxStartAngles");

	//////////////////////////////////
	// Movement
	mStartVelType = GetStartPosType(apElement->GetAttributeString("StartVelType").c_str());
	mCoordSystem = GetCoordSystem(apElement->GetAttributeString("CoordSystem").c_str());

	mvMinStartVel = apElement->GetAttributeVector3f("MinStartVel");
	mvMaxStartVel = apElement->GetAttributeVector3f("MaxStartVel");

	mvMinStartVelAngles = apElement->GetAttributeVector2f("MinStartVelAngles");
	mvMaxStartVelAngles = apElement->GetAttributeVector2f("MaxStartVelAngles");

	mfMinStartVelSpeed = apElement->GetAttributeFloat("MinStartVelSpeed");
	mfMaxStartVelSpeed = apElement->GetAttributeFloat("MaxStartVelSpeed");

	mfMinSpeedMultiply = apElement->GetAttributeFloat("MinSpeedMultiply");
	mfMaxSpeedMultiply = apElement->GetAttributeFloat("MaxSpeedMultiply");

	mvMinStartAcc = apElement->GetAttributeVector3f("MinStartAcc");
	mvMaxStartAcc = apElement->GetAttributeVector3f("MaxStartAcc");

	mfMinVelMaximum = apElement->GetAttributeFloat("MinVelMaximum");
	mfMaxVelMaximum = apElement->GetAttributeFloat("MaxVelMaximum");

	mbUsesDirection = apElement->GetAttributeBool("UsesDirection");

	mGravityType = GetGravityType(apElement->GetAttributeString("GravityType").c_str());
	mvGravityAcc = apElement->GetAttributeVector3f("GravityAcc");
	
	/////////////////////////////////
	// Rotation
	mbUsePartSpin = apElement->GetAttributeBool("UsePartSpin");
	mPartSpinType = GetPartSpinType(apElement->GetAttributeString("PartSpinType").c_str());

	mfMinSpinRange = apElement->GetAttributeFloat("MinSpinRange");
	mfMaxSpinRange = apElement->GetAttributeFloat("MaxSpinRange");

	mbUseRevolution = apElement->GetAttributeBool("UseRevolution");

	mvMinRevVel = apElement->GetAttributeVector3f("MinRevVel");
	mvMaxRevVel = apElement->GetAttributeVector3f("MaxRevVel");

	/////////////////////////////////
	// Life
	mfMinLifeSpan = apElement->GetAttributeFloat("MinLifeSpan");
	mfMaxLifeSpan = apElement->GetAttributeFloat("MaxLifeSpan");
	mDeathType = GetDeathType(apElement->GetAttributeString("DeathType").c_str());
	msDeathPS = apElement->GetAttributeString("DeathPS");

	/////////////////////////////////
	// Rendering
	mDrawType = GetDrawType(apElement->GetAttributeString("DrawType").c_str());
	mbMultiplyRGBWithAlpha = apElement->GetAttributeBool("MultiplyRGBWithAlpha");
	mvMinStartSize = apElement->GetAttributeVector2f("MinStartSize");
	mvMaxStartSize = apElement->GetAttributeVector2f("MaxStartSize");

	mfStartRelSize = apElement->GetAttributeFloat("StartRelSize");
	mfMiddleRelSize = apElement->GetAttributeFloat("MiddleRelSize");
	mfEndRelSize = apElement->GetAttributeFloat("EndRelSize");
	mfMiddleRelSizeTime = apElement->GetAttributeFloat("MiddleRelSizeTime");
	mfMiddleRelSizeLength = apElement->GetAttributeFloat("MiddleRelSizeLength");
	
	/////////////////////////////////
	// Color
	mMinStartColor = apElement->GetAttributeColor("MinStartColor");
	mMaxStartColor = apElement->GetAttributeColor("MaxStartColor");
	
	mStartRelColor = apElement->GetAttributeColor("StartRelColor");
	mMiddleRelColor = apElement->GetAttributeColor("MiddleRelColor");
	mEndRelColor = apElement->GetAttributeColor("EndRelColor");
	mfMiddleRelColorTime = apElement->GetAttributeFloat("MiddleRelColorTime");
	mfMiddleRelColorLength = apElement->GetAttributeFloat("MiddleRelColorLength");

	/////////////////////////////////
	// Collision
	mbCollides = apElement->GetAttributeBool("Collides");
	mfMinCollisionMax = apElement->GetAttributeFloat("MinCollisionMax");
	mfMaxCollisionMax = apElement->GetAttributeFloat("MaxCollisionMax");
	mfMinBounceAmount = apElement->GetAttributeFloat("MinBounceAmount");
	mfMaxBounceAmount = apElement->GetAttributeFloat("MaxBounceAmount");
	mlCollisionUpdateRate = apElement->GetAttributeInt("CollisionUpdateRate");

	/////////////////////////////////
	// Beam
	mbUseBeamNoise = apElement->GetAttributeBool("UseBeamNoise");
	mlLowFreqPoints = apElement->GetAttributeInt("LowFreqPoints");
	mvMinLFNoise = apElement->GetAttributeVector3f("MinLFNoise");
	mvMaxLFNoise = apElement->GetAttributeVector3f("MaxLFNoise");
	mlHighFreqPoints = apElement->GetAttributeInt("HighFreqPoints");
	mvMinHFNoise = apElement->GetAttributeVector3f("MinHFNoise");
	mvMaxHFNoise = apElement->GetAttributeVector3f("MaxHFNoise");
	mbApplyNoiseToStart = apElement->GetAttributeBool("ApplyNoiseToStart");
	mbApplyNoiseToEnd = apElement->GetAttributeBool("ApplyNoiseToEnd");

	/////////////////////////////////
	// Material
	msMaterial = apElement->GetAttributeString("Material");
	mlNumFiles = apElement->GetAttributeInt("MaterialNum");
	mfAnimLength = apElement->GetAttributeFloat("AnimationLength");

	mvSubDivAmount = apElement->GetAttributeVector2f("SubDiv");
	mSubDivType = GetSubDivType(apElement->GetAttribute("SubDivType")); 
}
*/
//----------------------------------------------------------------------


//----------------------------------------------------------------------

//----------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataParticleEmitter::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperParticleEmitter,(this));
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

cEntityWrapperParticleEmitter::cEntityWrapperParticleEmitter(iEntityWrapperData* apData) : iEntityWrapper(apData)
{
}
//----------------------------------------------------------------------

bool cEntityWrapperParticleEmitter::GetProperty(int alPropID, int& alX)
{
	switch(alPropID)
	{
	case ePEInt_MaxParticleNum:
		alX = GetMaxParticleNum(); break;
	case ePEInt_WarmUpStepsPerSec:
		alX = GetWarmUpStepsPerSec(); break;
	case ePEInt_CollisionUpdateRate:
		alX = GetCollisionUpdateRate(); break;
	case ePEInt_LowFreqPoints:
		alX = GetLowFreqPoints(); break;
	case ePEInt_HighFreqPoints:
		alX = GetHighFreqPoints(); break;
	case ePEInt_NumFiles:
		alX = GetNumFiles(); break;
	default:
		return iEntityWrapper::GetProperty(alPropID, alX);
	}
	return true;
}

bool cEntityWrapperParticleEmitter::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case ePEFloat_ParticlesPerSecond:
		afX = GetParticlesPerSecond(); break;
	case ePEFloat_StartTimeOffset:
		afX = GetStartTimeOffset(); break;
	case ePEFloat_WarmUpTime:
		afX = GetWarmUpTime(); break;
	case ePEFloat_MinPauseLength:
		afX = GetMinPauseLength(); break;
	case ePEFloat_MaxPauseLength:
		afX = GetMaxPauseLength(); break;
	case ePEFloat_MinPauseInterval:
		afX = GetMinPauseInterval(); break;
	case ePEFloat_MaxPauseInterval:
		afX = GetMaxPauseInterval(); break;

	case ePEFloat_MinStartRadius:
		afX = GetMinStartRadius(); break;
	case ePEFloat_MaxStartRadius:
		afX = GetMaxStartRadius(); break;

	case ePEFloat_MinStartVelSpeed:
		afX = GetMinStartVelSpeed(); break;
	case ePEFloat_MaxStartVelSpeed:
		afX = GetMaxStartVelSpeed(); break;

	case ePEFloat_MinSpeedMultiply:
		afX = GetMinSpeedMultiply(); break;
	case ePEFloat_MaxSpeedMultiply:
		afX = GetMaxSpeedMultiply(); break;

	case ePEFloat_MinVelMaximum:
		afX = GetMinVelMaximum(); break;
	case ePEFloat_MaxVelMaximum:
		afX = GetMaxVelMaximum(); break;

	case ePEFloat_MinSpinRange:
		afX = GetMinSpinRange(); break;
	case ePEFloat_MaxSpinRange:
		afX = GetMaxSpinRange(); break;

	case ePEFloat_MinLifeSpan:
		afX = GetMinLifeSpan(); break;
	case ePEFloat_MaxLifeSpan:
		afX = GetMaxLifeSpan(); break;

	case ePEFloat_StartRelSize:
		afX = GetStartRelSize(); break;
	case ePEFloat_MiddleRelSize:
		afX = GetMiddleRelSize(); break;
	case ePEFloat_EndRelSize:
		afX = GetEndRelSize(); break;

	case ePEFloat_MiddleRelSizeTime:
		afX = GetMiddleRelSizeTime(); break;
	case ePEFloat_MiddleRelSizeLength:
		afX = GetMiddleRelSizeLength(); break;
	case ePEFloat_MiddleRelColorTime:
		afX = GetMiddleRelColorTime(); break;
	case ePEFloat_MiddleRelColorLength:
		afX = GetMiddleRelColorLength(); break;
	case ePEFloat_MinCollisionMax:
		afX = GetMinCollisionMax(); break;
	case ePEFloat_MaxCollisionMax:
		afX = GetMaxCollisionMax(); break;

	case ePEFloat_MinBounceAmount:
		afX = GetMinBounceAmount(); break;
	case ePEFloat_MaxBounceAmount:
		afX = GetMaxBounceAmount(); break;

	case ePEFloat_AnimLength:
		afX = GetAnimLength(); break;

	default:
		return iEntityWrapper::GetProperty(alPropID, afX);

	}

	return true;
}

bool cEntityWrapperParticleEmitter::GetProperty(int alPropID, bool& abX)
{
	switch(alPropID)
	{
	case ePEBool_Respawn:
		abX = GetRespawn(); break;
	case ePEBool_UseDirection:
		abX = GetUsesDirection(); break;
	case ePEBool_UsePartSpin:
		abX = GetUsePartSpin(); break;
	case ePEBool_UseRevolution:
		abX = GetUseRevolution(); break;
	case ePEBool_MultiplyRGBWithAlpha:
		abX = GetMultiplyRGBWithAlpha(); break;
	case ePEBool_Collides:
		abX = GetCollides(); break;
	case ePEBool_UseBeamNoise:
		abX = GetUseBeamNoise(); break;
	case ePEBool_ApplyNoiseToStart:
		abX = GetApplyNoiseToStart(); break;
	case ePEBool_ApplyNoiseToEnd:
		abX = GetApplyNoiseToEnd(); break;

	default:
		iEntityWrapper::GetProperty(alPropID, abX);
	}

	return true;
}

bool cEntityWrapperParticleEmitter::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case ePEStr_PEType:
		asX = PETypeToString(GetPEType()); break;
	case ePEStr_StartPosType:
		asX = StartPosTypeToString(GetStartPosType()); break;
	case ePEStr_StartVelType:
		asX = StartPosTypeToString(GetStartVelType()); break;
	case ePEStr_CoordSystem:
		asX = CoordSystemToString(GetCoordSystem()); break;
	case ePEStr_GravityType:
		asX = GravityTypeToString(GetGravityType()); break;

	case ePEStr_PartSpinType:
		asX = PartSpinTypeToString(GetPartSpinType()); break;

	case ePEStr_DrawType:
		asX = DrawTypeToString(GetDrawType()); break;
	case ePEStr_DeathType:
		asX = DeathTypeToString(GetDeathType()); break;

	case ePEStr_SubDivType:
		asX = SubDivTypeToString(GetSubDivisionType()); break;


	case ePEStr_DeathPS:
		asX = GetDeathPS(); break;
	case ePEStr_Material:
		asX = GetMaterial(); break;
	default:
		iEntityWrapper::GetProperty(alPropID, asX);
	}

	return true;
}

bool cEntityWrapperParticleEmitter::GetProperty(int alPropID, cVector2f& avX)
{
	switch(alPropID)
	{
	case ePEVec2f_MinStartAngles:
		avX = GetMinStartAngles(); break;
	case ePEVec2f_MaxStartAngles:
		avX = GetMaxStartAngles(); break;
	case ePEVec2f_MinStartVelAngles:
		avX = GetMinStartVelAngles(); break;
	case ePEVec2f_MaxStartVelAngles:
		avX = GetMaxStartVelAngles(); break;
	case ePEVec2f_MinStartSize:
		avX = GetMinStartSize(); break;
	case ePEVec2f_MaxStartSize:
		avX = GetMaxStartSize(); break;
	case ePEVec2f_SubDivAmount:
		avX = GetSubDivAmount(); break;

	default:
		iEntityWrapper::GetProperty(alPropID, avX);
	}

	return true;
}

bool cEntityWrapperParticleEmitter::GetProperty(int alPropID, cVector3f& avX)
{
	switch(alPropID)
	{
	case ePEVec3f_PosOffset:
		avX = GetPosOffset(); break;
	case ePEVec3f_AngleOffset:
		avX = GetAngleOffset(); break;

	case ePEVec3f_MinStartPos:
		avX = GetMinStartPos(); break;
	case ePEVec3f_MaxStartPos:
		avX = GetMaxStartPos(); break;

	case ePEVec3f_MinStartVel:
		avX = GetMinStartVel(); break;
	case ePEVec3f_MaxStartVel:
		avX = GetMaxStartVel(); break;
	
	case ePEVec3f_MinStartAcc:
		avX = GetMinStartAcc(); break;
	case ePEVec3f_MaxStartAcc:
		avX = GetMaxStartAcc(); break;

	case ePEVec3f_GravityAcc:
		avX = GetGravityAcc(); break;
	
	case ePEVec3f_MinRevVel:
		avX = GetMinRevVel(); break;
	case ePEVec3f_MaxRevVel:
		avX = GetMaxRevVel(); break;

	case ePEVec3f_MinLFNoise:
		avX = GetMinLFNoise(); break;
	case ePEVec3f_MaxLFNoise:
		avX = GetMaxLFNoise(); break;
	case ePEVec3f_MinHFNoise:
		avX = GetMinHFNoise(); break;
	case ePEVec3f_MaxHFNoise:
		avX = GetMaxHFNoise(); break;

	default:
		iEntityWrapper::GetProperty(alPropID, avX);
	}

	return true;
}

bool cEntityWrapperParticleEmitter::GetProperty(int alPropID, cColor& aX)
{
	switch(alPropID)
	{
	case ePECol_MinStartColor:
		aX = GetMinStartColor(); break;
	case ePECol_MaxStartColor:
		aX = GetMaxStartColor(); break;
	case ePECol_StartRelColor:
		aX = GetStartRelColor(); break;
	case ePECol_MiddleRelColor:
		aX = GetMiddleRelColor(); break;
	case ePECol_EndRelColor:
		aX = GetEndRelColor(); break;

	default:
		iEntityWrapper::GetProperty(alPropID, aX);
	}

	return true;
}




bool cEntityWrapperParticleEmitter::SetProperty(int alPropID, const int& alX)
{
	switch(alPropID)
	{
	case ePEInt_MaxParticleNum:
		SetMaxParticleNum(alX); break;
	case ePEInt_WarmUpStepsPerSec:
		SetWarmUpStepsPerSec(alX); break;
	case ePEInt_CollisionUpdateRate:
		SetCollisionUpdateRate(alX); break;
	case ePEInt_LowFreqPoints:
		SetLowFreqPoints(alX); break;
	case ePEInt_HighFreqPoints:
		SetHighFreqPoints(alX); break;
	case ePEInt_NumFiles:
		SetNumFiles(alX); break;
	default:
		return iEntityWrapper::SetProperty(alPropID, alX);
	}
	return true;
}

bool cEntityWrapperParticleEmitter::SetProperty(int alPropID, const float& afX)
{
	switch(alPropID)
	{
	case ePEFloat_ParticlesPerSecond:
		SetParticlesPerSecond(afX); break;
	case ePEFloat_StartTimeOffset:
		SetStartTimeOffset(afX); break;
	case ePEFloat_WarmUpTime:
		SetWarmUpTime(afX); break;
	case ePEFloat_MinPauseLength:
		SetMinPauseLength(afX); break;
	case ePEFloat_MaxPauseLength:
		SetMaxPauseLength(afX); break;
	case ePEFloat_MinPauseInterval:
		SetMinPauseInterval(afX); break;
	case ePEFloat_MaxPauseInterval:
		SetMaxPauseInterval(afX); break;

	case ePEFloat_MinStartRadius:
		SetMinStartRadius(afX); break;
	case ePEFloat_MaxStartRadius:
		SetMaxStartRadius(afX); break;

	case ePEFloat_MinStartVelSpeed:
		SetMinStartVelSpeed(afX); break;
	case ePEFloat_MaxStartVelSpeed:
		SetMaxStartVelSpeed(afX); break;

	case ePEFloat_MinSpeedMultiply:
		SetMinSpeedMultiply(afX); break;
	case ePEFloat_MaxSpeedMultiply:
		SetMaxSpeedMultiply(afX); break;

	case ePEFloat_MinVelMaximum:
		SetMinVelMaximum(afX); break;
	case ePEFloat_MaxVelMaximum:
		SetMaxVelMaximum(afX); break;

	case ePEFloat_MinSpinRange:
		SetMinSpinRange(afX); break;
	case ePEFloat_MaxSpinRange:
		SetMaxSpinRange(afX); break;

	case ePEFloat_MinLifeSpan:
		SetMinLifeSpan(afX); break;
	case ePEFloat_MaxLifeSpan:
		SetMaxLifeSpan(afX); break;

	case ePEFloat_StartRelSize:
		SetStartRelSize(afX); break;
	case ePEFloat_MiddleRelSize:
		SetMiddleRelSize(afX); break;
	case ePEFloat_EndRelSize:
		SetEndRelSize(afX); break;

	case ePEFloat_MiddleRelSizeTime:
		SetMiddleRelSizeTime(afX); break;
	case ePEFloat_MiddleRelSizeLength:
		SetMiddleRelSizeLength(afX); break;
	case ePEFloat_MiddleRelColorTime:
		SetMiddleRelColorTime(afX); break;
	case ePEFloat_MiddleRelColorLength:
		SetMiddleRelColorLength(afX); break;
	case ePEFloat_MinCollisionMax:
		SetMinCollisionMax(afX); break;
	case ePEFloat_MaxCollisionMax:
		SetMaxCollisionMax(afX); break;

	case ePEFloat_MinBounceAmount:
		SetMinBounceAmount(afX); break;
	case ePEFloat_MaxBounceAmount:
		SetMaxBounceAmount(afX); break;

	case ePEFloat_AnimLength:
		SetAnimLength(afX); break;

	default:
		return iEntityWrapper::SetProperty(alPropID, afX);

	}

	return true;
}

bool cEntityWrapperParticleEmitter::SetProperty(int alPropID, const bool& abX)
{
	switch(alPropID)
	{
	case ePEBool_Respawn:
		SetRespawn(abX); break;
	case ePEBool_UseDirection:
		SetUsesDirection(abX); break;
	case ePEBool_UsePartSpin:
		SetUsePartSpin(abX); break;
	case ePEBool_UseRevolution:
		SetUseRevolution(abX); break;
	case ePEBool_MultiplyRGBWithAlpha:
		SetMultiplyRGBWithAlpha(abX); break;
	case ePEBool_Collides:
		SetCollides(abX); break;
	case ePEBool_UseBeamNoise:
		SetUseBeamNoise(abX); break;
	case ePEBool_ApplyNoiseToStart:
		SetApplyNoiseToStart(abX); break;
	case ePEBool_ApplyNoiseToEnd:
		SetApplyNoiseToEnd(abX); break;

	default:
		iEntityWrapper::SetProperty(alPropID, abX);
	}

	return true;
}

bool cEntityWrapperParticleEmitter::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case ePEStr_PEType:
		SetPEType(ToPEType(asX.c_str())); break;

	case ePEStr_StartPosType:
		SetStartPosType(ToStartPosType(asX.c_str())); break;

	case ePEStr_CoordSystem:
		SetCoordSystem(ToCoordSystem(asX.c_str())); break;

	case ePEStr_StartVelType:
		SetStartVelType(ToStartPosType(asX.c_str())); break;

	case ePEStr_DrawType:
		SetDrawType(ToDrawType(asX.c_str())); break;

	case ePEStr_GravityType:
		SetGravityType(ToGravityType(asX.c_str())); break;

	case ePEStr_PartSpinType:
		SetPartSpinType(ToPartSpinType(asX.c_str())); break;


	case ePEStr_DeathType:
		SetDeathType(ToDeathType(asX.c_str())); break;

	case ePEStr_DeathPS:
		SetDeathPS(asX); break;

	case ePEStr_SubDivType:
		SetSubDivisionType(ToSubDivType(asX.c_str())); break;

	case ePEStr_Material:
		SetMaterial(asX); break;
	default:
		iEntityWrapper::SetProperty(alPropID, asX);
	}

	return true;
}

bool cEntityWrapperParticleEmitter::SetProperty(int alPropID, const cVector2f& avX)
{
	switch(alPropID)
	{
	case ePEVec2f_MinStartAngles:
		SetMinStartAngles(avX); break;

	case ePEVec2f_MaxStartAngles:
		SetMaxStartAngles(avX); break;

	case ePEVec2f_MinStartVelAngles:
		SetMinStartVelAngles(avX); break;

	case ePEVec2f_MaxStartVelAngles:
		SetMaxStartVelAngles(avX); break;

	case ePEVec2f_MinStartSize:
		SetMinStartSize(avX); break;

	case ePEVec2f_MaxStartSize:
		SetMaxStartSize(avX); break;

	case ePEVec2f_SubDivAmount:
		SetSubDivAmount(avX); break;

	default:
		iEntityWrapper::SetProperty(alPropID, avX);
	}

	return true;
}

bool cEntityWrapperParticleEmitter::SetProperty(int alPropID, const cVector3f& avX)
{
	switch(alPropID)
	{
	case ePEVec3f_PosOffset:
		SetPosOffset(avX); break;
	case ePEVec3f_AngleOffset:
		SetAngleOffset(avX); break;

	case ePEVec3f_MinStartPos:
		SetMinStartPos(avX); break;
	case ePEVec3f_MaxStartPos:
		SetMaxStartPos(avX); break;

	case ePEVec3f_MinStartVel:
		SetMinStartVel(avX); break;
	case ePEVec3f_MaxStartVel:
		SetMaxStartVel(avX); break;
	
	case ePEVec3f_MinStartAcc:
		SetMinStartAcc(avX); break;
	case ePEVec3f_MaxStartAcc:
		SetMaxStartAcc(avX); break;

	case ePEVec3f_GravityAcc:
		SetGravityAcc(avX); break;
	
	case ePEVec3f_MinRevVel:
		SetMinRevVel(avX); break;
	case ePEVec3f_MaxRevVel:
		SetMaxRevVel(avX); break;

	case ePEVec3f_MinLFNoise:
		SetMinLFNoise(avX); break;
	case ePEVec3f_MaxLFNoise:
		SetMaxLFNoise(avX); break;
	case ePEVec3f_MinHFNoise:
		SetMinHFNoise(avX); break;
	case ePEVec3f_MaxHFNoise:
		SetMaxHFNoise(avX); break;

	default:
		iEntityWrapper::SetProperty(alPropID, avX);
	}

	return true;
}

bool cEntityWrapperParticleEmitter::SetProperty(int alPropID, const cColor& aX)
{
	switch(alPropID)
	{
	case ePECol_MinStartColor:
		SetMinStartColor(aX); break;
	case ePECol_MaxStartColor:
		SetMaxStartColor(aX); break;
	case ePECol_StartRelColor:
		SetStartRelColor(aX); break;
	case ePECol_MiddleRelColor:
		SetMiddleRelColor(aX); break;
	case ePECol_EndRelColor:
		SetEndRelColor(aX); break;

	default:
		iEntityWrapper::SetProperty(alPropID, aX);
	}

	return true;
}

//----------------------------------------------------------------------

void cEntityWrapperParticleEmitter::SetMaterial(const tString& asX)
{
	iEditorBase* pEditor = GetEditorWorld()->GetEditor();
	cFileSearcher* pSearcher = pEditor->GetEngine()->GetResources()->GetFileSearcher();

	tString sSanitizedFile = asX;
	int lLength = sSanitizedFile.size()-1;
	if(lLength>=0 && sSanitizedFile[lLength]==' ') sSanitizedFile = cString::Sub(sSanitizedFile, 0, lLength);

	tWString sMatFile = pEditor->GetPathRelToWD(pSearcher->GetFilePath(sSanitizedFile));
	msMaterial = cString::To8Char(sMatFile);
}
