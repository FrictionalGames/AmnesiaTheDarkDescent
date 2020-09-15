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

#include "ParticleEditorWindowEmitterParams.h"

#include "EntityWrapperParticleEmitter.h"

#include "../common/EditorInput.h"
#include "ParticleEditorWorld.h"


//-------------------------------------------------------------

//////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////

//-------------------------------------------------------------

cParticleEditorWindowEmitterParams::cParticleEditorWindowEmitterParams(iEditorBase* apEditor) : iEditorWindow(apEditor, "Emitter Parameters window")
{
	mpEmitter = NULL;
}

//-------------------------------------------------------------

cParticleEditorWindowEmitterParams::~cParticleEditorWindowEmitterParams()
{
}

//-------------------------------------------------------------

//////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////

//-------------------------------------------------------------

void cParticleEditorWindowEmitterParams::SetCurrentEmitter(cEntityWrapperParticleEmitter* apEmitter)
{
	if(mpEmitter==apEmitter)
		return;

	mpEmitter = apEmitter;
}

//-------------------------------------------------------------

//-------------------------------------------------------------

//////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////

//-------------------------------------------------------------

void cParticleEditorWindowEmitterParams::OnInitLayout()
{
	tWStringList lstLabelMinMax;
	lstLabelMinMax.push_back(_W("Min:"));
	lstLabelMinMax.push_back(_W("Max:"));

	tWStringList lstLabelXYZ;
	lstLabelXYZ.push_back(_W("X:"));
	lstLabelXYZ.push_back(_W("Y:"));
	lstLabelXYZ.push_back(_W("Z:"));

	SetSize(cVector2f(512,620));
	SetPosition(cVector3f(0,120,0.1f));

	mpTFCategories = mpSet->CreateWidgetTabFrame(cVector3f(5,5,0.1f), GetSize()-cVector2f(10,4),_W(""), mpBGFrame);

	cWidgetTab* pTab = NULL;
	cWidgetGroup* pGroup = NULL;
	iEditorInput* pInp = NULL;
	cVector3f vPos;

	//////////////////////////////////////////////////////
	// Tab General
	pTab = mpTFCategories->AddTab(_W("General"));
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(200,300), _W("General"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpName = CreateInputString(vPos, _W("Name"), "InputName", pGroup);
		pInp = mpInpName;
		vPos.y += pInp->GetSize().y;
		mpInpMaxParticles = CreateInputNumber(vPos, _W("Max particles"), "InputMaxParticles", pGroup);
		pInp = mpInpMaxParticles;
		vPos.y += pInp->GetSize().y;
		mpInpParticlesPerSec = CreateInputNumber(vPos, _W("Particles per sec"), "InputParticlesPerSec", pGroup);
		pInp = mpInpParticlesPerSec;
		vPos.y += pInp->GetSize().y;
		mpInpStartTimeOffset = CreateInputNumber(vPos, _W("Start time offset(s)"), "InputStartTimeOffset", pGroup);
		pInp = mpInpStartTimeOffset;
		vPos.y += pInp->GetSize().y;
		mpInpRespawn = CreateInputBool(vPos, _W("Respawn dead particles"), "InputRespawnDeadParticles", pGroup);
		vPos.y += mpInpRespawn->GetSize().y;
		mpInpWarmUpTime = CreateInputNumber(vPos, _W("Warmup time(s)"), "InputWarmupTime", pGroup);
		vPos.y += mpInpWarmUpTime->GetSize().y;
		mpInpWarmUpStepsPerSec = CreateInputNumber(vPos, _W("Warmup Steps/Sec"), "InputWarmupSteps", pGroup);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(220, 35,0.1f), cVector2f(200,175), _W("Pause"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpLength = CreateInputVec2(vPos, _W("Length(s)"), "InputLength", pGroup, 50, lstLabelMinMax);
		vPos.y += mpInpLength->GetSize().y;
		mpInpInterval = CreateInputVec2(vPos, _W("Interval(s)"), "InputInterval", pGroup, 50, lstLabelMinMax);
		
		pGroup = mpSet->CreateWidgetGroup(cVector3f(220,240,0.1f), cVector2f(200,175), _W("Offset"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpOffsetPos = CreateInputVec3(vPos, _W("Position(m)"), "InputPosition", pGroup, 50, lstLabelXYZ);
		pInp = mpInpOffsetPos;
		vPos.y += pInp->GetSize().y;
		mpInpOffsetAng = CreateInputVec3(vPos, _W("Angles(deg)"), "InputAngles", pGroup, 50, lstLabelXYZ);
	}
	//////////////////////////////////////////////////////
	// Tab Start
	pTab = mpTFCategories->AddTab(_W("Start"));
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(480,100), _W("General"), pTab);
		vPos = cVector3f(10,10,0.1f);

		tWStringList lstValues;
		lstValues.push_back(_W("Box"));
		lstValues.push_back(_W("Sphere"));
		mpInpStartType = CreateInputEnum(vPos, _W("Start Type"), "InputStartType", lstValues, pGroup);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,150,0.1f), cVector2f(480,100), _W("Box Start"), pTab);
		vPos = cVector3f(10,10,0.1f);

		mpInpBoxStartMinPos = CreateInputVec3(vPos, _W("Min Position(m)"), "InputMinPosition", pGroup, 50, lstLabelXYZ);
		vPos.x += mpInpBoxStartMinPos->GetSize().x;
		mpInpBoxStartMaxPos = CreateInputVec3(vPos, _W("Max Position(m)"), "InputMaxPosition", pGroup, 50, lstLabelXYZ);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,260,0.1f), cVector2f(480,80), _W("Sphere Start"), pTab);
		vPos = cVector3f(10,20,0.1f);
		mpInpSphStartMinAng = CreateInputVec2(vPos, _W("Min Angles(deg)"), "InputMinAngles", pGroup, 50, lstLabelXYZ);
		vPos.x += mpInpSphStartMinAng->GetSize().x + 20;
		mpInpSphStartMaxAng = CreateInputVec2(vPos, _W("Max Angles(deg)"), "InputMaxAngles", pGroup, 50, lstLabelXYZ);
		vPos.x += mpInpSphStartMaxAng->GetSize().x + 20;
		mpInpSphStartRadius = CreateInputVec2(vPos, _W("Radius(m)"), "InputRadius", pGroup, 50, lstLabelMinMax);
	}
    
	//////////////////////////////////////////////////////
	// Tab Movement
	pTab = mpTFCategories->AddTab(_W("Movement"));
	{
		tWStringList lstValues;
		lstValues.push_back(_W("Box"));
		lstValues.push_back(_W("Sphere"));

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(225,100), _W("General"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpVelType = CreateInputEnum(vPos, _W("Velocity Type"), "", lstValues, pGroup);
		lstValues.clear();
		lstValues.push_back(_W("World"));
		lstValues.push_back(_W("Local"));
		mpInpCoordSys = CreateInputEnum(vPos + cVector3f(mpInpVelType->GetSize().x, 0, 0), _W("Coord System"), "", lstValues, pGroup);
		vPos.y += mpInpVelType->GetSize().y;
		mpInpUseDir = CreateInputBool(vPos, _W("Uses Direction"), "InputUseDir", pGroup);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(245,35,0.1f), cVector2f(225,100), _W("Gravity"), pTab);
		vPos = cVector3f(10,10,0.1f);

		lstValues.clear();
		lstValues.push_back(_W("None"));
		lstValues.push_back(_W("Vector"));
		lstValues.push_back(_W("Center"));
		mpInpGravType = CreateInputEnum(vPos, _W("Gravity Type"), "", lstValues, pGroup);
		vPos.y += mpInpGravType->GetSize().y;
		mpInpGravAcc = CreateInputVec3(vPos, _W("Acceleration(m/s^2)"), "InputAccel", pGroup,50,lstLabelXYZ);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10, 145,0.1f), cVector2f(480,100), _W("Box velocity"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpMinVel = CreateInputVec3(vPos, _W("Min velocity(m/s)"), "InputMinVel", pGroup, 50,lstLabelXYZ);
		vPos.x += mpInpMinVel->GetSize().x + 20;
		mpInpMaxVel = CreateInputVec3(vPos, _W("Max velocity(m/s)"), "InputMaxVel", pGroup, 50,lstLabelXYZ);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,260,0.1f), cVector2f(480,80), _W("Sphere Velocity"), pTab);
		vPos = cVector3f(10,20,0.1f);
		mpInpSphVelMinAng = CreateInputVec2(vPos, _W("Min Angles(deg)"), "InputSphVelMinAngles", pGroup, 50,lstLabelXYZ);
		vPos.x += mpInpSphVelMinAng->GetSize().x + 20;
		mpInpSphVelMaxAng = CreateInputVec2(vPos, _W("Max Angles(deg)"), "InputSphVelMaxAngles", pGroup, 50,lstLabelXYZ);
		vPos.x += mpInpSphVelMaxAng->GetSize().x + 20;
		mpInpSphVelSpeed = CreateInputVec2(vPos, _W("Speed(m/s)"), "InputSphVelSpeed", pGroup, 50,lstLabelMinMax);
		
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,360,0.1f), cVector2f(480,150), _W("Acceleration"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpMinAcc = CreateInputVec3(vPos, _W("Min acceleration(m/s^2)"), "InputMinAccel", pGroup, 50,lstLabelXYZ);
		mpInpSpeedMul = CreateInputVec2(vPos + cVector3f(0,mpInpMinAcc->GetSize().y,0), _W("Speed multiplier"), "InputSpeedMul", pGroup, 50,lstLabelMinMax);
		vPos = cVector3f(240,10,0.1f);
		mpInpMaxAcc = CreateInputVec3(vPos, _W("Max acceleration(m/s^2)"), "InputMaxAccel", pGroup, 50,lstLabelXYZ);
		mpInpMaxSpeed = CreateInputVec2(vPos + cVector3f(0,mpInpMaxAcc->GetSize().y,0), _W("Maximum speed"), "InputMaxSpeed", pGroup, 50,lstLabelMinMax);
	}

	//////////////////////////////////////////////////////
	// Tab Rendering
	pTab = mpTFCategories->AddTab(_W("Rendering"));
	{
		tWStringList lstValues;
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(400,100), _W("General"), pTab);
		vPos = cVector3f(10,10,0.1f);
		lstValues.push_back(_W("Point"));
		lstValues.push_back(_W("Line"));
		lstValues.push_back(_W("Axis"));
		mpInpDrawType = CreateInputEnum(vPos, _W("Draw Type"), "InputDrawType", lstValues, pGroup);
		pInp = mpInpDrawType;
		vPos.x += pInp->GetSize().x + 20;
		mpInpMulRGBWithAlpha = CreateInputBool(vPos, _W("Multiply RGB with alpha"), "InputMulRGB", pGroup);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,235,0.1f), cVector2f(400,100), _W("Size"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpMinSize = CreateInputVec2(vPos, _W("Min Size(m)"), "InputMinSize", pGroup, 50,lstLabelXYZ);
		pInp = mpInpMinSize;
		mpInpMaxSize = CreateInputVec2(vPos + cVector3f(pInp->GetSize().x + 20,0,0), _W("Max Size(m)"), "InputMaxSize", pGroup, 50,lstLabelXYZ);
		pInp = mpInpMaxSize;
		vPos.y += pInp->GetSize().y;

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,350,0.1f), cVector2f(400,150), _W("Size Fading"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpStartRelSize = CreateInputNumber(vPos, _W("Start Relative Size:"), "InputStartRelSize", pGroup);
		pInp = mpInpStartRelSize;
		mpInpMiddleRelSizeStart = CreateInputNumber(vPos+cVector3f(pInp->GetSize().x,0,0), _W("Middle Relative start:"), "InputMidRelSize", pGroup);
		pInp = mpInpMiddleRelSizeStart;
		vPos.y += pInp->GetSize().y;
		mpInpMiddleRelSize = CreateInputNumber(vPos, _W("Middle Relative Size:"), "InputMidRelSize", pGroup);
		pInp = mpInpMiddleRelSize;
		mpInpMiddleRelSizeLength = CreateInputNumber(vPos+cVector3f(pInp->GetSize().x,0,0), _W("Middle Relative length:"), "InputMidRelLen", pGroup);
		pInp = mpInpMiddleRelSizeLength;
		vPos.y += pInp->GetSize().y;
		mpInpEndRelSize = CreateInputNumber(vPos, _W("End Relative Size:"), "InputEndRelSize", pGroup);
	}


	//////////////////////////////////////////////////////
	// Tab Color
	pTab = mpTFCategories->AddTab(_W("Color"));
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(480,100), _W("General"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpMinCol = CreateInputColorFrame(vPos, _W("Min Color"), "InputMinCol", pGroup);
		vPos.x += mpInpMinCol->GetSize().x + 10;
		mpInpMaxCol = CreateInputColorFrame(vPos, _W("Max Color"), "InputMaxCol", pGroup);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10, 145, 0.1f), cVector2f(480,120), _W("Fading"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpStartRelCol = CreateInputColorFrame(vPos, _W("Start Relative Color"), "InputStartRelCol", pGroup);
		mpInpMiddleRelColStart = CreateInputNumber(vPos + cVector3f(mpInpStartRelCol->GetSize().x + 50,0,0), _W("Middle relative start"), "InputMidRelStart", pGroup);
		mpInpMiddleRelColStart->SetLayoutStyle(eEditorInputLayoutStyle_RowLabelOnLeft);
		mpInpMiddleRelColStart->UpdateLayout();
        vPos.y += mpInpMiddleRelColStart->GetSize().y + 10;
		mpInpMiddleRelCol = CreateInputColorFrame(vPos, _W("Middle Relative Color"), "InputStartRelCol", pGroup);
		mpInpMiddleRelColLength = CreateInputNumber(vPos + cVector3f(mpInpMiddleRelCol->GetSize().x + 41,0,0), _W("Middle relative length"), "InputMidRelLen", pGroup);
		mpInpMiddleRelColLength->SetLayoutStyle(eEditorInputLayoutStyle_RowLabelOnLeft);
		mpInpMiddleRelColLength->UpdateLayout();
		vPos.y += mpInpMiddleRelColLength->GetSize().y + 10;
		mpInpEndRelCol = CreateInputColorFrame(vPos, _W("End Relative Color"), "InputEndRelCol", pGroup);
	}

	//////////////////////////////////////////////////////
	// Tab Life
	pTab = mpTFCategories->AddTab(_W("Life"));
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(200,100), _W("Life"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpLifeSpan = CreateInputVec2(vPos, _W("Life Span(s)"), "InputLifeSpan", pGroup, 50,lstLabelMinMax, eEditorInputLayoutStyle_ColumnLabelOnTop);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(220,35,0.1f), cVector2f(250,100), _W("Death"), pTab);
		vPos = cVector3f(10,10,0.1f);
		tWStringList lstValues;
		lstValues.push_back(_W("Age"));
		mpInpDeathType = CreateInputEnum(vPos, _W("Death Type"), "", lstValues, pGroup);
		vPos.x += mpInpDeathType->GetSize().x;
		mpInpDeathPS = CreateInputFile(vPos, _W("PS created at death"), "InputDeathPS", pGroup);
		mpInpDeathPS->SetBrowserType(eEditorResourceType_ParticleSystem);
	}

	//////////////////////////////////////////////////////
	// Tab Material
	pTab = mpTFCategories->AddTab(_W("Material"));
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(300,100), _W("Material"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpMaterial = CreateInputFile(vPos, _W("Material file"), "InputMatFile", pGroup);
		mpInpMaterial->SetBrowserType(eEditorResourceType_Material);
		mpInpMaterial->SetInitialPath(cString::AddSlashAtEndW(mpEditor->GetMainLookUpDir(eDir_Particles)) + _W("materials"));
		vPos.y += mpInpMaterial->GetSize().y;
		mpInpNumFiles = CreateInputNumber(vPos, _W("Number of files"), "InputNumFiles", pGroup);
		vPos.x += mpInpNumFiles->GetSize().x;
		mpInpAnimLen = CreateInputNumber(vPos, _W("Animation length(s)"), "InputAnimLen", pGroup);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(310,35,0.1f), cVector2f(180,100), _W("Sub Division"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpSubDAmount = CreateInputVec2(vPos, _W("Amount"), "InpSDAmount", pGroup, 50,lstLabelXYZ);
		vPos.y += mpInpSubDAmount->GetSize().y;
		tWStringList lstValues;
		lstValues.push_back(_W("Random"));
		lstValues.push_back(_W("Animation"));
		mpInpSubDType = CreateInputEnum(vPos, _W("Type"), "", lstValues, pGroup);
	}

	//////////////////////////////////////////////////////
	// Tab Rotation
	pTab = mpTFCategories->AddTab(_W("Rotation"));
	{
		tWStringList lstValues;
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(480,150), _W("General"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpUsePartSpin = CreateInputBool(vPos, _W("Use Particle Spinning"), "InputUsePartSpin", pGroup);
		pInp = mpInpUsePartSpin;
		vPos.y+=pInp->GetSize().y;
		lstValues.push_back(_W("Constant"));
		lstValues.push_back(_W("Movement"));
		mpInpSpinType = CreateInputEnum(vPos, _W("Spin Type"), "InputSpinType", lstValues, pGroup);
		vPos.y+=50;
		mpInpSpinVelRange = CreateInputVec2(vPos,_W("Particle Spin Velocity Range(rad/s)"), "InputSpinVelRange", pGroup, 50,lstLabelMinMax);
		vPos = cVector3f(200,10,0.1f);
		mpInpUseRev = CreateInputBool(vPos, _W("Use Revolution"), "InputUseRev", pGroup);
		pInp = mpInpUseRev;
		vPos.y += pInp->GetSize().y;
		mpInpMinRevVel = CreateInputVec3(vPos,_W("Min"), "InputMinRevVel", pGroup, 50,lstLabelXYZ);
		pInp = mpInpMinRevVel;
		vPos.y += pInp->GetSize().y;
		mpInpMaxRevVel = CreateInputVec3(vPos,_W("Max"), "InputMaxRevVel", pGroup, 50,lstLabelXYZ);
	}
	
	//////////////////////////////////////////////////////
	// Tab Collision
	pTab = mpTFCategories->AddTab(_W("Collision"));
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(480,100), _W("General"), pTab);
		vPos = cVector3f(10,10,0.1f);
		mpInpUseCollision = CreateInputBool(vPos, _W("Use Collisions"), "InputUseCollisions", pGroup);
		mpInpUpdateRate = CreateInputNumber( vPos + cVector3f(150,0,0), _W("Update Rate"), "InputUpdRate", pGroup);
		vPos.y += 50;
        mpInpMaxCollisions = CreateInputVec2(vPos, _W("Max Collisions"), "InputCol", pGroup, 50,lstLabelMinMax);
		pInp = mpInpMaxCollisions;
		vPos.x += pInp->GetSize().x;
		mpInpBounceAmount = CreateInputVec2(vPos, _W("Bounce Amount"), "InputBounce", pGroup, 50, lstLabelMinMax);	
	}

	//////////////////////////////////////////////////////
	// Tab Beam Specific
	pTab = mpTFCategories->AddTab(_W("Beam Specific"));
	{
	}
}

//-------------------------------------------------------------

void cParticleEditorWindowEmitterParams::OnUpdate(float afTimeStep)
{
	if(mpEmitter==NULL)
		return;

	///////////////////////////////////////////////////////////////////////
	// Tab General - Group General
	mpInpName->SetValue(cString::To16Char(mpEmitter->GetName()),false);
	mpInpMaxParticles->SetValue((float)mpEmitter->GetMaxParticleNum(),false);
	mpInpParticlesPerSec->SetValue(mpEmitter->GetParticlesPerSecond(), false);
	mpInpStartTimeOffset->SetValue(mpEmitter->GetStartTimeOffset(), false);
	mpInpRespawn->SetValue(mpEmitter->GetRespawn(), false);
	mpInpWarmUpTime->SetValue(mpEmitter->GetWarmUpTime(),false);
	mpInpWarmUpStepsPerSec->SetValue((float)mpEmitter->GetWarmUpStepsPerSec(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab General - Group Pause	
	mpInpLength->SetValue(cVector2f(mpEmitter->GetMinPauseLength(), mpEmitter->GetMaxPauseLength()), false);
	mpInpInterval->SetValue(cVector2f(mpEmitter->GetMinPauseInterval(), mpEmitter->GetMaxPauseInterval()), false);

	///////////////////////////////////////////////////////////////////////
	// Tab General - Group Offset
	mpInpOffsetPos->SetValue(mpEmitter->GetPosOffset(),false);
	mpInpOffsetAng->SetValue(mpEmitter->GetAngleOffset(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Start - Group General
	mpInpStartType->SetValue(mpEmitter->GetStartPosType(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Start - Group Box Start
	mpInpBoxStartMinPos->SetValue(mpEmitter->GetMinStartPos(), false);
	mpInpBoxStartMaxPos->SetValue(mpEmitter->GetMaxStartPos(), false);
	
	///////////////////////////////////////////////////////////////////////
	// Tab Start - Group Sphere Start
	mpInpSphStartMinAng->SetValue(mpEmitter->GetMinStartAngles(), false);
	mpInpSphStartMaxAng->SetValue(mpEmitter->GetMaxStartAngles(), false);
	mpInpSphStartRadius->SetValue(cVector2f(mpEmitter->GetMinStartRadius(),mpEmitter->GetMaxStartRadius()), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Movement - Group General
	mpInpVelType->SetValue(mpEmitter->GetStartVelType(), false);
	mpInpCoordSys->SetValue(mpEmitter->GetCoordSystem(), false);
	mpInpUseDir->SetValue(mpEmitter->GetUsesDirection(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Movement - Group Gravity
	mpInpGravType->SetValue(mpEmitter->GetGravityType(), false);
	mpInpGravAcc->SetValue(mpEmitter->GetGravityAcc(), false);
	
	///////////////////////////////////////////////////////////////////////
	// Tab Movement - Group Box Vel
	mpInpMinVel->SetValue(mpEmitter->GetMinStartVel(), false);
	mpInpMaxVel->SetValue(mpEmitter->GetMaxStartVel(), false);
	
	///////////////////////////////////////////////////////////////////////
	// Tab Movement - Group Sphere Vel
	mpInpSphVelMinAng->SetValue(mpEmitter->GetMinStartVelAngles(), false);
	mpInpSphVelMaxAng->SetValue(mpEmitter->GetMaxStartVelAngles(), false);
	mpInpSphVelSpeed->SetValue(cVector2f(mpEmitter->GetMinStartVelSpeed(), mpEmitter->GetMaxStartVelSpeed()), false);
    
	///////////////////////////////////////////////////////////////////////
	// Tab Movement - Group Acceleration
	mpInpMinAcc->SetValue(mpEmitter->GetMinStartAcc(), false);
	mpInpMaxAcc->SetValue(mpEmitter->GetMaxStartAcc(), false);
	mpInpSpeedMul->SetValue(cVector2f(mpEmitter->GetMinSpeedMultiply(), mpEmitter->GetMaxSpeedMultiply()), false);
	mpInpMaxSpeed->SetValue(cVector2f(mpEmitter->GetMinVelMaximum(), mpEmitter->GetMaxVelMaximum()),false);


	///////////////////////////////////////////////////////////////////////
	// Tab Rendering - Group General
	mpInpDrawType->SetValue(mpEmitter->GetDrawType(),false);
	mpInpMulRGBWithAlpha->SetValue(mpEmitter->GetMultiplyRGBWithAlpha(),false);

	///////////////////////////////////////////////////////////////////////
	// Tab Rendering - Group Size
	mpInpMinSize->SetValue(mpEmitter->GetMinStartSize(), false);
	mpInpMaxSize->SetValue(mpEmitter->GetMaxStartSize(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Rendering - Group Size Fading
	mpInpStartRelSize->SetValue(mpEmitter->GetStartRelSize(), false);
	mpInpMiddleRelSizeStart->SetValue(mpEmitter->GetMiddleRelSizeTime(), false);
	mpInpMiddleRelSize->SetValue(mpEmitter->GetMiddleRelSize(), false);
	mpInpMiddleRelSizeLength->SetValue(mpEmitter->GetMiddleRelSizeLength(), false);
	mpInpEndRelSize->SetValue(mpEmitter->GetEndRelSize(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Color - Group General
	mpInpMinCol->SetValue(mpEmitter->GetMinStartColor(), false);
	mpInpMaxCol->SetValue(mpEmitter->GetMaxStartColor(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Color - Group Fading
	mpInpStartRelCol->SetValue(mpEmitter->GetStartRelColor(), false);
	mpInpMiddleRelColStart->SetValue(mpEmitter->GetMiddleRelColorTime(), false);
	mpInpMiddleRelCol->SetValue(mpEmitter->GetMiddleRelColor(), false);
	mpInpMiddleRelColLength->SetValue(mpEmitter->GetMiddleRelColorLength(),false);
	mpInpEndRelCol->SetValue(mpEmitter->GetEndRelColor(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Life - Group Life
	mpInpLifeSpan->SetValue(cVector2f(mpEmitter->GetMinLifeSpan(), mpEmitter->GetMaxLifeSpan()), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Life - Group Death
	mpInpDeathType->SetValue(mpEmitter->GetDeathType(), false);
	mpInpDeathPS->SetValue(cString::To16Char(mpEmitter->GetDeathPS()), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Life - Group Life
	mpInpLifeSpan->SetValue(cVector2f(mpEmitter->GetMinLifeSpan(), mpEmitter->GetMaxLifeSpan()), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Life - Group Death
	mpInpDeathType->SetValue(mpEmitter->GetDeathType(), false);
	mpInpDeathPS->SetValue(cString::To16Char(mpEmitter->GetDeathPS()), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Material - Group Material
	mpInpMaterial->SetValue(cString::To16Char(mpEmitter->GetMaterial()), false);
	mpInpNumFiles->SetValue((float)mpEmitter->GetNumFiles(), false);
	mpInpAnimLen->SetValue(mpEmitter->GetAnimLength(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Material - Group Sub Division
	mpInpSubDAmount->SetValue(mpEmitter->GetSubDivAmount(), false);
	mpInpSubDType->SetValue(mpEmitter->GetSubDivisionType(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Rotation - Group General
	mpInpUsePartSpin->SetValue(mpEmitter->GetUsePartSpin(), false);
	mpInpSpinType->SetValue(mpEmitter->GetPartSpinType(), false);
	mpInpSpinVelRange->SetValue(cVector2f(mpEmitter->GetMinSpinRange(), mpEmitter->GetMaxSpinRange()), false);
	mpInpUseRev->SetValue(mpEmitter->GetUseRevolution(), false);
	mpInpMinRevVel->SetValue(mpEmitter->GetMinRevVel(), false);
	mpInpMaxRevVel->SetValue(mpEmitter->GetMaxRevVel(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Collision - Group General
	mpInpUseCollision->SetValue(mpEmitter->GetCollides(), false);
	mpInpUpdateRate->SetValue((float)mpEmitter->GetCollisionUpdateRate(), false);
	mpInpMaxCollisions->SetValue(cVector2f(mpEmitter->GetMinCollisionMax(), mpEmitter->GetMaxCollisionMax()), false);
	mpInpBounceAmount->SetValue(cVector2f(mpEmitter->GetMinBounceAmount(), mpEmitter->GetMaxBounceAmount()), false);
	
}

//-------------------------------------------------------------


bool cParticleEditorWindowEmitterParams::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(mpEmitter==NULL) return false;

	///////////////////////////////////////////////////////////////////////
	// Tab General - Group General
	if(apInput==mpInpName)
	{
		mpEmitter->SetName(cString::To8Char(apInput->GetValue()));
		((cParticleEditorWorld*)mpEditor->GetEditorWorld())->SetEmittersUpdated(true);
	}

	else if(apInput==mpInpMaxParticles)
		mpEmitter->SetMaxParticleNum((int)mpInpMaxParticles->GetValue());

	else if(apInput==mpInpParticlesPerSec)
		mpEmitter->SetParticlesPerSecond(mpInpParticlesPerSec->GetValue());

	else if(apInput==mpInpStartTimeOffset)
		mpEmitter->SetStartTimeOffset(mpInpStartTimeOffset->GetValue());

	else if(apInput==mpInpRespawn)
		mpEmitter->SetRespawn(mpInpRespawn->GetValue());

	else if(apInput==mpInpWarmUpTime)
		mpEmitter->SetWarmUpTime(mpInpWarmUpTime->GetValue());

	else if(apInput==mpInpWarmUpStepsPerSec)
		mpEmitter->SetWarmUpStepsPerSec((int)mpInpWarmUpStepsPerSec->GetValue());

	else if(apInput==mpInpLength)
	{
		mpEmitter->SetMinPauseLength(mpInpLength->GetValue().x);
		mpEmitter->SetMaxPauseLength(mpInpLength->GetValue().y);
	}

	else if(apInput==mpInpInterval)
	{
		mpEmitter->SetMinPauseInterval(mpInpInterval->GetValue().x);
		mpEmitter->SetMaxPauseInterval(mpInpInterval->GetValue().y);
	}

	else if(apInput==this->mpInpOffsetPos)
	{
		mpEmitter->SetPosOffset(mpInpOffsetPos->GetValue());
	}

	else if(apInput==mpInpOffsetAng)
		mpEmitter->SetAngleOffset(mpInpOffsetAng->GetValue());


	///////////////////////////////////////////////////////////////////////
	// Tab Rendering
	// General 
	else if(apInput==mpInpDrawType)
		mpEmitter->SetDrawType((eEditorParticleEmitterType)mpInpDrawType->GetValue());

	else if(apInput==mpInpMulRGBWithAlpha)
		mpEmitter->SetMultiplyRGBWithAlpha(mpInpMulRGBWithAlpha->GetValue());

	// Size
	else if(apInput==mpInpMinSize)
		mpEmitter->SetMinStartSize(mpInpMinSize->GetValue());
	else if(apInput==mpInpMaxSize)
		mpEmitter->SetMaxStartSize(mpInpMaxSize->GetValue());

	// Size fading
	else if(apInput==mpInpStartRelSize)
		mpEmitter->SetStartRelSize(mpInpStartRelSize->GetValue());

	else if(apInput==mpInpMiddleRelSizeStart)
		mpEmitter->SetMiddleRelSizeTime(mpInpMiddleRelSizeStart->GetValue());

	else if(apInput==mpInpMiddleRelSize)
		mpEmitter->SetMiddleRelSize(mpInpMiddleRelSize->GetValue());

	else if(apInput==this->mpInpMiddleRelSizeLength)
		mpEmitter->SetMiddleRelSizeLength(mpInpMiddleRelSizeLength->GetValue());

	else if(apInput==mpInpEndRelSize)
		mpEmitter->SetEndRelSize(mpInpEndRelSize->GetValue());

	///////////////////////////////////////////////////////////////////////
	// Tab Collision - Group Pause
	else if(apInput==mpInpUseCollision)
		mpEmitter->SetCollides(mpInpUseCollision->GetValue());

	else if(apInput==mpInpUpdateRate)
		mpEmitter->SetCollisionUpdateRate((int)mpInpUpdateRate->GetValue());

	else if(apInput==mpInpMaxCollisions)
	{
		cVector2f vTemp = mpInpMaxCollisions->GetValue();
		mpEmitter->SetMinCollisionMax(vTemp.x);
		mpEmitter->SetMaxCollisionMax(vTemp.y);
	}

	else if(apInput==mpInpBounceAmount)
	{
		cVector2f vTemp = mpInpBounceAmount->GetValue();
		mpEmitter->SetMinBounceAmount(vTemp.x);
		mpEmitter->SetMaxBounceAmount(vTemp.y);
	}

	///////////////////////////////////////////////////////////////////////
	// Tab Rotation
	else if(apInput==mpInpUsePartSpin)
		mpEmitter->SetUsePartSpin(mpInpUsePartSpin->GetValue());

	else if(apInput==mpInpSpinType)
		mpEmitter->SetPartSpinType((ePEPartSpinType)mpInpSpinType->GetValue());

	else if(apInput==mpInpSpinVelRange)
	{
		cVector2f vTemp = mpInpSpinVelRange->GetValue();
		mpEmitter->SetMinSpinRange(vTemp.x);
		mpEmitter->SetMaxSpinRange(vTemp.y);
	}
	else if(apInput==mpInpUseRev)
		mpEmitter->SetUseRevolution(mpInpUseRev->GetValue());

	else if(apInput==mpInpMinRevVel)
		mpEmitter->SetMinRevVel(mpInpMinRevVel->GetValue());

	else if(apInput==mpInpMaxRevVel)
		mpEmitter->SetMaxRevVel(mpInpMaxRevVel->GetValue());
	

	///////////////////////////////////////////////////////////////////////
	// Tab Start
	else if(apInput==mpInpStartType)
		mpEmitter->SetStartPosType((ePEStartPosType)mpInpStartType->GetValue());

	else if(apInput==this->mpInpBoxStartMinPos)
		mpEmitter->SetMinStartPos(mpInpBoxStartMinPos->GetValue());

	else if(apInput==mpInpBoxStartMaxPos)
		mpEmitter->SetMaxStartPos(mpInpBoxStartMaxPos->GetValue());

	else if(apInput==mpInpSphStartMinAng)
		mpEmitter->SetMinStartAngles(mpInpSphStartMinAng->GetValue());

	else if(apInput==mpInpSphStartMaxAng)
		mpEmitter->SetMaxStartAngles(mpInpSphStartMaxAng->GetValue());

	else if(apInput==this->mpInpSphStartRadius)
	{
		cVector2f vTemp = mpInpSphStartRadius->GetValue();
		mpEmitter->SetMinStartRadius(vTemp.x);
		mpEmitter->SetMaxStartRadius(vTemp.y);
	}

	///////////////////////////////////////////////////////////////////////
	// Tab Color
	else if(apInput==this->mpInpMinCol)
		mpEmitter->SetMinStartColor(mpInpMinCol->GetValue());

	else if(apInput==this->mpInpMaxCol)
		mpEmitter->SetMaxStartColor(mpInpMaxCol->GetValue());

	else if(apInput==this->mpInpStartRelCol)
		mpEmitter->SetStartRelColor(mpInpStartRelCol->GetValue());

	else if(apInput==mpInpMiddleRelCol)
		mpEmitter->SetMiddleRelColor(mpInpMiddleRelCol->GetValue());

	else if(apInput==mpInpEndRelCol)
		mpEmitter->SetEndRelColor(mpInpEndRelCol->GetValue());

	else if(apInput==mpInpMiddleRelColStart)
		mpEmitter->SetMiddleRelColorTime(mpInpMiddleRelColStart->GetValue());

	else if(apInput==mpInpMiddleRelColLength)
		mpEmitter->SetMiddleRelColorLength(mpInpMiddleRelColLength->GetValue());

	///////////////////////////////////////////////////////////////////////
	// Tab Movement
	else if(apInput==mpInpVelType)
		mpEmitter->SetStartVelType((ePEStartPosType)mpInpVelType->GetValue());

	else if(apInput==mpInpCoordSys)
		mpEmitter->SetCoordSystem((eParticleEmitterCoordSystem)mpInpCoordSys->GetValue());

	else if(apInput==mpInpUseDir)
		mpEmitter->SetUsesDirection(mpInpUseDir->GetValue());

	else if(apInput==mpInpGravType)
		mpEmitter->SetGravityType((ePEGravityType)mpInpGravType->GetValue());

	else if(apInput==mpInpGravAcc)
		mpEmitter->SetGravityAcc(mpInpGravAcc->GetValue());

	else if(apInput==mpInpMinVel)
		mpEmitter->SetMinStartVel(mpInpMinVel->GetValue());

	else if(apInput==mpInpMaxVel)
		mpEmitter->SetMaxStartVel(mpInpMaxVel->GetValue());

	else if(apInput==mpInpSphVelMinAng)
		mpEmitter->SetMinStartVelAngles(mpInpSphVelMinAng->GetValue());

	else if(apInput==mpInpSphVelMaxAng)
		mpEmitter->SetMaxStartVelAngles(mpInpSphVelMaxAng->GetValue());

	else if(apInput==mpInpSphVelSpeed)
	{
		cVector2f vTemp = mpInpSphVelSpeed->GetValue();
		mpEmitter->SetMinStartVelSpeed(vTemp.x);
		mpEmitter->SetMaxStartVelSpeed(vTemp.y);
	}

	else if(apInput==mpInpMinAcc)
		mpEmitter->SetMinStartAcc(mpInpMinAcc->GetValue());

	else if(apInput==mpInpMaxAcc)
		mpEmitter->SetMaxStartAcc(mpInpMaxAcc->GetValue());

	else if(apInput==mpInpSpeedMul)
	{
		cVector2f vTemp = mpInpSpeedMul->GetValue();
		mpEmitter->SetMinSpeedMultiply(vTemp.x);
		mpEmitter->SetMaxSpeedMultiply(vTemp.y);
	}

	else if(apInput==mpInpMaxSpeed)
	{
		cVector2f vTemp = mpInpMaxSpeed->GetValue();
		mpEmitter->SetMinVelMaximum(vTemp.x);
		mpEmitter->SetMaxVelMaximum(vTemp.y);
	}

    ///////////////////////////////////////////////////////////////////////
	// Tab Life
	else if(apInput==mpInpLifeSpan)
	{
		cVector2f vTemp = mpInpLifeSpan->GetValue();
		mpEmitter->SetMinLifeSpan(vTemp.x);
		mpEmitter->SetMaxLifeSpan(vTemp.y);
	}

	else if(apInput==mpInpDeathType)
		mpEmitter->SetDeathType((ePEDeathType)mpInpDeathType->GetValue());

	else if(apInput==mpInpDeathPS)
		mpEmitter->SetDeathPS(cString::To8Char(mpInpDeathPS->GetValue()));

	///////////////////////////////////////////////////////////////////////
	// Tab Material
	else if(apInput==mpInpMaterial)
		mpEmitter->SetMaterial(cString::To8Char(mpInpMaterial->GetValue()));

	else if(apInput==mpInpNumFiles)
		mpEmitter->SetNumFiles((int)mpInpNumFiles->GetValue());

	else if(apInput==mpInpAnimLen)
		mpEmitter->SetAnimLength(mpInpAnimLen->GetValue());

	else if(apInput==mpInpSubDAmount)
		mpEmitter->SetSubDivAmount(mpInpSubDAmount->GetValue());

	else if(apInput==mpInpSubDType)
		mpEmitter->SetSubDivisionType((ePESubDivType)mpInpSubDType->GetValue());



	mpEditor->SetLayoutNeedsUpdate(true);
	///////////////////////////////////////////////////////
	// Update particle system after setting values
	((cParticleEditorWorld*)mpEditor->GetEditorWorld())->UpdateParticleSystem();

	return true;
}

//-------------------------------------------------------------
