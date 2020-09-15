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

#include "../ParticleEditor/EdEditPaneParticleEmitter.h"

#include "../ParticleEditor/EdObjParticleEmitter.h"
#include "../Common/Editor.h"

//-------------------------------------------------------------

//////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////

//-------------------------------------------------------------

int cEdEditPaneParticleEmitter::mlLastEditedTab = -1;

//-------------------------------------------------------------

cEdEditPaneParticleEmitter::cEdEditPaneParticleEmitter(iEdObject* apObj) : iEdEditPane(apObj)
{
	mpTFCategories = NULL;
}

//-------------------------------------------------------------

cEdEditPaneParticleEmitter::~cEdEditPaneParticleEmitter()
{
	//if(mpTFCategories) 
	//	mlLastEditedTab = mpTFCategories->GetTabOnTopIndex();
}

//-------------------------------------------------------------

//////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////

//-------------------------------------------------------------

void cEdEditPaneParticleEmitter::SetCurrentEmitter(cEdObjParticleEmitter* apEmitter)
{
	//if(mpObject==apEmitter)
		return;

	//mpObject = apEmitter;
	SetUpdated();
}

//-------------------------------------------------------------

//-------------------------------------------------------------

//////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////

//-------------------------------------------------------------

void cEdEditPaneParticleEmitter::OnCreateLayout()
{
	iEdEditPane::OnCreateLayout();

	tWStringList lstLabelMinMax;
	lstLabelMinMax.push_back(_W("Min:"));
	lstLabelMinMax.push_back(_W("Max:"));

	tWStringList lstLabelXYZ;
	lstLabelXYZ.push_back(_W("X:"));
	lstLabelXYZ.push_back(_W("Y:"));
	lstLabelXYZ.push_back(_W("Z:"));

	const cVector2f& vSize = GetBG()->GetSize();
	mpTFCategories = mpSet->CreateWidgetTabFrame(cVector3f(5,5,0.1f), vSize-cVector2f(10,4),_W(""), GetBG());


	cWidgetTab* pTab = NULL;
	cWidgetGroup* pGroup = NULL;
	iEdInput* pInp = NULL;
	cVector3f vPos;

	//////////////////////////////////////////////////////
	// Tab General
	pTab = mpTFCategories->AddTab(_W("General"));
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(200,300), _W("General"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpName = CreateInputString(vPos, _W("Name"), pGroup);
		pInp = mpInpName;
		vPos.y += pInp->GetSize().y;
		mpInpMaxParticles = CreateInputNumber(vPos, _W("Max particles"), pGroup);
		pInp = mpInpMaxParticles;
		vPos.y += pInp->GetSize().y;
		mpInpParticlesPerSec = CreateInputNumber(vPos, _W("Particles per sec"), pGroup);
		pInp = mpInpParticlesPerSec;
		vPos.y += pInp->GetSize().y;
		mpInpStartTimeOffset = CreateInputNumber(vPos, _W("Start time offset(s)"), pGroup);
		pInp = mpInpStartTimeOffset;
		vPos.y += pInp->GetSize().y;
		mpInpRespawn = CreateInputBool(vPos, _W("Respawn dead particles"), pGroup);
		vPos.y += mpInpRespawn->GetSize().y;
		mpInpWarmUpTime = CreateInputNumber(vPos, _W("Warmup time(s)"), pGroup);
		vPos.y += mpInpWarmUpTime->GetSize().y;
		mpInpWarmUpStepsPerSec = CreateInputNumber(vPos, _W("Warmup Steps/Sec"), pGroup);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(220, 35,0.1f), cVector2f(200,175), _W("Pause"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpLength = CreateInputVec2(vPos, _W("Length(s)"), pGroup, 50, lstLabelMinMax);
		vPos.y += mpInpLength->GetSize().y;
		mpInpInterval = CreateInputVec2(vPos, _W("Interval(s)"), pGroup, 50, lstLabelMinMax);
		
		pGroup = mpSet->CreateWidgetGroup(cVector3f(220,240,0.1f), cVector2f(200,175), _W("Offset"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpOffsetPos = CreateInputVec3(vPos, _W("Position(m)"), pGroup, 50, lstLabelXYZ);
		pInp = mpInpOffsetPos;
		vPos.y += pInp->GetSize().y;
		mpInpOffsetAng = CreateInputVec3(vPos, _W("Angles(deg)"), pGroup, 50, lstLabelXYZ);
	}
	//////////////////////////////////////////////////////
	// Tab Start
	pTab = mpTFCategories->AddTab(_W("Start"));
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(480,100), _W("General"), pTab);
		vPos = cVector3f(10,10,0.1f);

		mpInpStartType = CreateInputEnum(vPos, _W("Start Type"), pGroup);
		mpInpStartType->AddValue(_W("Box"));
		mpInpStartType->AddValue(_W("Sphere"));

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,150,0.1f), cVector2f(480,100), _W("Box Start"), pTab);

		vPos = cVector3f(10,10,0.1f);

		mpInpBoxStartMinPos = CreateInputVec3(vPos, _W("Min Position(m)"), pGroup, 50, lstLabelXYZ);
		vPos.x += mpInpBoxStartMinPos->GetSize().x;
		mpInpBoxStartMaxPos = CreateInputVec3(vPos, _W("Max Position(m)"), pGroup, 50, lstLabelXYZ);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,260,0.1f), cVector2f(480,80), _W("Sphere Start"), pTab);

		vPos = cVector3f(10,20,0.1f);
		mpInpSphStartMinAng = CreateInputVec2(vPos, _W("Min Angles(deg)"), pGroup, 50, lstLabelXYZ);
		vPos.x += mpInpSphStartMinAng->GetSize().x + 20;
		mpInpSphStartMaxAng = CreateInputVec2(vPos, _W("Max Angles(deg)"), pGroup, 50, lstLabelXYZ);
		vPos.x += mpInpSphStartMaxAng->GetSize().x + 20;
		mpInpSphStartRadius = CreateInputVec2(vPos, _W("Radius(m)"), pGroup, 50, lstLabelMinMax);
	}
    
	//////////////////////////////////////////////////////
	// Tab Movement
	pTab = mpTFCategories->AddTab(_W("Movement"));
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(225,100), _W("General"), pTab);

		vPos = cVector3f(10,10,0.1f);
		
		mpInpVelType = CreateInputEnum(vPos, _W("Velocity Type"), pGroup);
		mpInpVelType->AddValue(_W("Box"));
		mpInpVelType->AddValue(_W("Sphere"));
		
		mpInpCoordSys = CreateInputEnum(vPos + cVector3f(mpInpVelType->GetSize().x, 0, 0), _W("Coord System"), pGroup);
		mpInpCoordSys->AddValue(_W("World"));
		mpInpCoordSys->AddValue(_W("Local"));

		vPos.y += mpInpVelType->GetSize().y;
		mpInpUseDir = CreateInputBool(vPos, _W("Uses Direction"), pGroup);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(245,35,0.1f), cVector2f(225,100), _W("Gravity"), pTab);
		vPos = cVector3f(10,10,0.1f);

		mpInpGravType = CreateInputEnum(vPos, _W("Gravity Type"), pGroup);
		mpInpGravType->AddValue(_W("None"));
		mpInpGravType->AddValue(_W("Vector"));
		mpInpGravType->AddValue(_W("Center"));

		vPos.y += mpInpGravType->GetSize().y;
		mpInpGravAcc = CreateInputVec3(vPos, _W("Acceleration(m/s^2)"), pGroup,50,lstLabelXYZ);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10, 145,0.1f), cVector2f(480,100), _W("Box velocity"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpMinVel = CreateInputVec3(vPos, _W("Min velocity(m/s)"), pGroup, 50,lstLabelXYZ);
		vPos.x += mpInpMinVel->GetSize().x + 20;
		mpInpMaxVel = CreateInputVec3(vPos, _W("Max velocity(m/s)"), pGroup, 50,lstLabelXYZ);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,260,0.1f), cVector2f(480,80), _W("Sphere Velocity"), pTab);

		vPos = cVector3f(10,20,0.1f);
		mpInpSphVelMinAng = CreateInputVec2(vPos, _W("Min Angles(deg)"), pGroup, 50,lstLabelXYZ);
		vPos.x += mpInpSphVelMinAng->GetSize().x + 20;
		mpInpSphVelMaxAng = CreateInputVec2(vPos, _W("Max Angles(deg)"), pGroup, 50,lstLabelXYZ);
		vPos.x += mpInpSphVelMaxAng->GetSize().x + 20;
		mpInpSphVelSpeed = CreateInputVec2(vPos, _W("Speed(m/s)"), pGroup, 50,lstLabelMinMax);
		
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,360,0.1f), cVector2f(480,150), _W("Acceleration"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpMinAcc = CreateInputVec3(vPos, _W("Min acceleration(m/s^2)"), pGroup, 50,lstLabelXYZ);
		mpInpSpeedMul = CreateInputVec2(vPos + cVector3f(0,mpInpMinAcc->GetSize().y,0), _W("Speed multiplier"), pGroup, 50,lstLabelMinMax);
		vPos = cVector3f(240,10,0.1f);
		mpInpMaxAcc = CreateInputVec3(vPos, _W("Max acceleration(m/s^2)"), pGroup, 50,lstLabelXYZ);
		mpInpMaxSpeed = CreateInputVec2(vPos + cVector3f(0,mpInpMaxAcc->GetSize().y,0), _W("Maximum speed"), pGroup, 50,lstLabelMinMax);
	}

	//////////////////////////////////////////////////////
	// Tab Rendering
	pTab = mpTFCategories->AddTab(_W("Rendering"));
	{
		tWStringList lstValues;
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(400,100), _W("General"), pTab);

		vPos = cVector3f(10,10,0.1f);
		
		mpInpDrawType = CreateInputEnum(vPos, _W("Draw Type"), pGroup);
		mpInpDrawType->AddValue(_W("Point"));
		mpInpDrawType->AddValue(_W("Line"));
		mpInpDrawType->AddValue(_W("Axis"));

		pInp = mpInpDrawType;
		vPos.x += pInp->GetSize().x + 20;
		mpInpMulRGBWithAlpha = CreateInputBool(vPos, _W("Multiply RGB with alpha"), pGroup);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,235,0.1f), cVector2f(400,100), _W("Size"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpMinSize = CreateInputVec2(vPos, _W("Min Size(m)"), pGroup, 50,lstLabelXYZ);
		pInp = mpInpMinSize;
		mpInpMaxSize = CreateInputVec2(vPos + cVector3f(pInp->GetSize().x + 20,0,0), _W("Max Size(m)"), pGroup, 50,lstLabelXYZ);
		pInp = mpInpMaxSize;
		vPos.y += pInp->GetSize().y;

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,350,0.1f), cVector2f(400,150), _W("Size Fading"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpStartRelSize = CreateInputNumber(vPos, _W("Start Relative Size:"), pGroup);
		pInp = mpInpStartRelSize;
		mpInpMiddleRelSizeStart = CreateInputNumber(vPos+cVector3f(pInp->GetSize().x,0,0), _W("Middle Relative start:"), pGroup);
		pInp = mpInpMiddleRelSizeStart;
		vPos.y += pInp->GetSize().y;
		mpInpMiddleRelSize = CreateInputNumber(vPos, _W("Middle Relative Size:"), pGroup);
		pInp = mpInpMiddleRelSize;
		mpInpMiddleRelSizeLength = CreateInputNumber(vPos+cVector3f(pInp->GetSize().x,0,0), _W("Middle Relative length:"), pGroup);
		pInp = mpInpMiddleRelSizeLength;
		vPos.y += pInp->GetSize().y;
		mpInpEndRelSize = CreateInputNumber(vPos, _W("End Relative Size:"), pGroup);
	}


	//////////////////////////////////////////////////////
	// Tab Color
	pTab = mpTFCategories->AddTab(_W("Color"));
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(480,100), _W("General"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpMinCol = CreateInputColor(vPos, _W("Min Color"), pGroup);
		vPos.x += mpInpMinCol->GetSize().x + 10;
		mpInpMaxCol = CreateInputColor(vPos, _W("Max Color"), pGroup);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(10, 145, 0.1f), cVector2f(480,120), _W("Fading"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpStartRelCol = CreateInputColor(vPos, _W("Start Relative Color"), pGroup);
		mpInpMiddleRelColStart = CreateInputNumber(vPos + cVector3f(mpInpStartRelCol->GetSize().x + 50,0,0), _W("Middle relative start"), pGroup);
		mpInpMiddleRelColStart->SetStyle(eEdInpStyle_RowLabelOnLeft);
		mpInpMiddleRelColStart->UpdateLayout();
        vPos.y += mpInpMiddleRelColStart->GetSize().y + 10;
		mpInpMiddleRelCol = CreateInputColor(vPos, _W("Middle Relative Color"), pGroup);
		mpInpMiddleRelColLength = CreateInputNumber(vPos + cVector3f(mpInpMiddleRelCol->GetSize().x + 41,0,0), _W("Middle relative length"), pGroup);
		mpInpMiddleRelColLength->SetStyle(eEdInpStyle_RowLabelOnLeft);
		mpInpMiddleRelColLength->UpdateLayout();
		vPos.y += mpInpMiddleRelColLength->GetSize().y + 10;
		mpInpEndRelCol = CreateInputColor(vPos, _W("End Relative Color"), pGroup);
	}

	//////////////////////////////////////////////////////
	// Tab Life
	pTab = mpTFCategories->AddTab(_W("Life"));
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(200,100), _W("Life"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpLifeSpan = CreateInputVec2(vPos, _W("Life Span(s)"), pGroup, 50,lstLabelMinMax, eEdInpStyle_ColumnLabelOnTop);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(220,35,0.1f), cVector2f(250,100), _W("Death"), pTab);

		vPos = cVector3f(10,10,0.1f);

		mpInpDeathType = CreateInputEnum(vPos, _W("Death Type"), pGroup);
		mpInpDeathType->AddValue(_W("Age"));

		vPos.x += mpInpDeathType->GetSize().x;

		mpInpDeathPS = CreateInputParticleSystem(vPos, _W("PS created at death"), pGroup);
	}

	//////////////////////////////////////////////////////
	// Tab Material
	pTab = mpTFCategories->AddTab(_W("Material"));
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(300,100), _W("Material"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpMaterial = CreateInputMaterial(vPos, _W("Material file"), pGroup);
		mpInpMaterial->SetInitialPath(cString::AddSlashAtEndW(mpEditor->GetMainLookUpDir(eDir_Particles)) + _W("materials"));

		vPos.y += mpInpMaterial->GetSize().y;
		mpInpNumFiles = CreateInputNumber(vPos, _W("Number of files"), pGroup);
		vPos.x += mpInpNumFiles->GetSize().x + 10;
		mpInpAnimLen = CreateInputNumber(vPos, _W("Animation length(s)"), pGroup);

		pGroup = mpSet->CreateWidgetGroup(cVector3f(310,35,0.1f), cVector2f(180,100), _W("Sub Division"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpSubDAmount = CreateInputVec2(vPos, _W("Amount"), pGroup, 50,lstLabelXYZ);
		vPos.y += mpInpSubDAmount->GetSize().y;
		
		mpInpSubDType = CreateInputEnum(vPos, _W("Type"), pGroup);
		mpInpSubDType->AddValue(_W("Random"));
		mpInpSubDType->AddValue(_W("Animation"));
	}

	//////////////////////////////////////////////////////
	// Tab Rotation
	pTab = mpTFCategories->AddTab(_W("Rotation"));
	{
		tWStringList lstValues;
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(480,150), _W("General"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpUsePartSpin = CreateInputBool(vPos, _W("Use Particle Spinning"), pGroup);
		pInp = mpInpUsePartSpin;
		vPos.y+=pInp->GetSize().y;
		
		mpInpSpinType = CreateInputEnum(vPos, _W("Spin Type"), pGroup);
		mpInpSpinType->AddValue(_W("Constant"));
		mpInpSpinType->AddValue(_W("Movement"));

		vPos.y+=50;
		mpInpSpinVelRange = CreateInputVec2(vPos,_W("Particle Spin Velocity Range(rad/s)"), pGroup, 50,lstLabelMinMax);
		vPos = cVector3f(200,10,0.1f);
		mpInpUseRev = CreateInputBool(vPos, _W("Use Revolution"), pGroup);
		pInp = mpInpUseRev;
		vPos.y += pInp->GetSize().y;
		mpInpMinRevVel = CreateInputVec3(vPos,_W("Min"), pGroup, 50,lstLabelXYZ);
		pInp = mpInpMinRevVel;
		vPos.y += pInp->GetSize().y;
		mpInpMaxRevVel = CreateInputVec3(vPos,_W("Max"), pGroup, 50,lstLabelXYZ);
	}
	
	//////////////////////////////////////////////////////
	// Tab Collision
	pTab = mpTFCategories->AddTab(_W("Collision"));
	{
		pGroup = mpSet->CreateWidgetGroup(cVector3f(10,35,0.1f), cVector2f(480,100), _W("General"), pTab);

		vPos = cVector3f(10,10,0.1f);
		mpInpUseCollision = CreateInputBool(vPos, _W("Use Collisions"), pGroup);
		mpInpUpdateRate = CreateInputNumber( vPos + cVector3f(150,0,0), _W("Update Rate"), pGroup);
		vPos.y += 50;
        mpInpMaxCollisions = CreateInputVec2(vPos, _W("Max Collisions"), pGroup, 50,lstLabelMinMax);
		pInp = mpInpMaxCollisions;
		vPos.x += pInp->GetSize().x;
		mpInpBounceAmount = CreateInputVec2(vPos, _W("Bounce Amount"), pGroup, 50, lstLabelMinMax);	
	}

	//////////////////////////////////////////////////////
	// Tab Beam Specific
	pTab = mpTFCategories->AddTab(_W("Beam Specific"));
	{
	}

	Log("Setting tab %d\n", mlLastEditedTab);
	mpTFCategories->SetTabOnTopByIndex(mlLastEditedTab);
}

//-------------------------------------------------------------

void cEdEditPaneParticleEmitter::OnUpdate()
{
	if(mpObject==NULL)
		return;

	iEdEditPane::OnUpdate();

	cEdObjParticleEmitter* pEmitter = static_cast<cEdObjParticleEmitter*>(mpObject);

	///////////////////////////////////////////////////////////////////////
	// Tab General - Group General
	mpInpName->SetValue(pEmitter->GetName(),false);
	mpInpMaxParticles->SetValue((float)pEmitter->GetMaxParticleNum(),false);
	mpInpParticlesPerSec->SetValue(pEmitter->GetParticlesPerSecond(), false);
	mpInpStartTimeOffset->SetValue(pEmitter->GetStartTimeOffset(), false);
	mpInpRespawn->SetValue(pEmitter->GetRespawn(), false);
	mpInpWarmUpTime->SetValue(pEmitter->GetWarmUpTime(),false);
	mpInpWarmUpStepsPerSec->SetValue((float)pEmitter->GetWarmUpStepsPerSec(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab General - Group Pause	
	mpInpLength->SetValue(cVector2f(pEmitter->GetMinPauseLength(), pEmitter->GetMaxPauseLength()), false);
	mpInpInterval->SetValue(cVector2f(pEmitter->GetMinPauseInterval(), pEmitter->GetMaxPauseInterval()), false);

	///////////////////////////////////////////////////////////////////////
	// Tab General - Group Offset
	mpInpOffsetPos->SetValue(pEmitter->GetPosOffset(),false);
	mpInpOffsetAng->SetValue(pEmitter->GetAngleOffset(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Start - Group General
	mpInpStartType->SetValue(pEmitter->GetStartPosType(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Start - Group Box Start
	mpInpBoxStartMinPos->SetValue(pEmitter->GetMinStartPos(), false);
	mpInpBoxStartMaxPos->SetValue(pEmitter->GetMaxStartPos(), false);
	
	///////////////////////////////////////////////////////////////////////
	// Tab Start - Group Sphere Start
	mpInpSphStartMinAng->SetValue(pEmitter->GetMinStartAngles(), false);
	mpInpSphStartMaxAng->SetValue(pEmitter->GetMaxStartAngles(), false);
	mpInpSphStartRadius->SetValue(cVector2f(pEmitter->GetMinStartRadius(),pEmitter->GetMaxStartRadius()), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Movement - Group General
	mpInpVelType->SetValue(pEmitter->GetStartVelType(), false);
	mpInpCoordSys->SetValue(pEmitter->GetCoordSystem(), false);
	mpInpUseDir->SetValue(pEmitter->GetUsesDirection(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Movement - Group Gravity
	mpInpGravType->SetValue(pEmitter->GetGravityType(), false);
	mpInpGravAcc->SetValue(pEmitter->GetGravityAcc(), false);
	
	///////////////////////////////////////////////////////////////////////
	// Tab Movement - Group Box Vel
	mpInpMinVel->SetValue(pEmitter->GetMinStartVel(), false);
	mpInpMaxVel->SetValue(pEmitter->GetMaxStartVel(), false);
	
	///////////////////////////////////////////////////////////////////////
	// Tab Movement - Group Sphere Vel
	mpInpSphVelMinAng->SetValue(pEmitter->GetMinStartVelAngles(), false);
	mpInpSphVelMaxAng->SetValue(pEmitter->GetMaxStartVelAngles(), false);
	mpInpSphVelSpeed->SetValue(cVector2f(pEmitter->GetMinStartVelSpeed(), pEmitter->GetMaxStartVelSpeed()), false);
    
	///////////////////////////////////////////////////////////////////////
	// Tab Movement - Group Acceleration
	mpInpMinAcc->SetValue(pEmitter->GetMinStartAcc(), false);
	mpInpMaxAcc->SetValue(pEmitter->GetMaxStartAcc(), false);
	mpInpSpeedMul->SetValue(cVector2f(pEmitter->GetMinSpeedMultiply(), pEmitter->GetMaxSpeedMultiply()), false);
	mpInpMaxSpeed->SetValue(cVector2f(pEmitter->GetMinVelMaximum(), pEmitter->GetMaxVelMaximum()),false);


	///////////////////////////////////////////////////////////////////////
	// Tab Rendering - Group General
	mpInpDrawType->SetValue(pEmitter->GetDrawType(),false);
	mpInpMulRGBWithAlpha->SetValue(pEmitter->GetMultiplyRGBWithAlpha(),false);

	///////////////////////////////////////////////////////////////////////
	// Tab Rendering - Group Size
	mpInpMinSize->SetValue(pEmitter->GetMinStartSize(), false);
	mpInpMaxSize->SetValue(pEmitter->GetMaxStartSize(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Rendering - Group Size Fading
	mpInpStartRelSize->SetValue(pEmitter->GetStartRelSize(), false);
	mpInpMiddleRelSizeStart->SetValue(pEmitter->GetMiddleRelSizeTime(), false);
	mpInpMiddleRelSize->SetValue(pEmitter->GetMiddleRelSize(), false);
	mpInpMiddleRelSizeLength->SetValue(pEmitter->GetMiddleRelSizeLength(), false);
	mpInpEndRelSize->SetValue(pEmitter->GetEndRelSize(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Color - Group General
	mpInpMinCol->SetValue(pEmitter->GetMinStartColor(), false);
	mpInpMaxCol->SetValue(pEmitter->GetMaxStartColor(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Color - Group Fading
	mpInpStartRelCol->SetValue(pEmitter->GetStartRelColor(), false);
	mpInpMiddleRelColStart->SetValue(pEmitter->GetMiddleRelColorTime(), false);
	mpInpMiddleRelCol->SetValue(pEmitter->GetMiddleRelColor(), false);
	mpInpMiddleRelColLength->SetValue(pEmitter->GetMiddleRelColorLength(),false);
	mpInpEndRelCol->SetValue(pEmitter->GetEndRelColor(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Life - Group Life
	mpInpLifeSpan->SetValue(cVector2f(pEmitter->GetMinLifeSpan(), pEmitter->GetMaxLifeSpan()), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Life - Group Death
	mpInpDeathType->SetValue(pEmitter->GetDeathType(), false);
	mpInpDeathPS->SetValue(cString::To16Char(pEmitter->GetDeathPS()), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Life - Group Life
	mpInpLifeSpan->SetValue(cVector2f(pEmitter->GetMinLifeSpan(), pEmitter->GetMaxLifeSpan()), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Life - Group Death
	mpInpDeathType->SetValue(pEmitter->GetDeathType(), false);
	mpInpDeathPS->SetValue(cString::To16Char(pEmitter->GetDeathPS()), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Material - Group Material
	mpInpMaterial->SetValue(cString::To16Char(pEmitter->GetMaterial()), false);
	mpInpNumFiles->SetValue((float)pEmitter->GetNumFiles(), false);
	mpInpAnimLen->SetValue(pEmitter->GetAnimLength(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Material - Group Sub Division
	mpInpSubDAmount->SetValue(pEmitter->GetSubDivAmount(), false);
	mpInpSubDType->SetValue(pEmitter->GetSubDivisionType(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Rotation - Group General
	mpInpUsePartSpin->SetValue(pEmitter->GetUsePartSpin(), false);
	mpInpSpinType->SetValue(pEmitter->GetPartSpinType(), false);
	mpInpSpinVelRange->SetValue(cVector2f(pEmitter->GetMinSpinRange(), pEmitter->GetMaxSpinRange()), false);
	mpInpUseRev->SetValue(pEmitter->GetUseRevolution(), false);
	mpInpMinRevVel->SetValue(pEmitter->GetMinRevVel(), false);
	mpInpMaxRevVel->SetValue(pEmitter->GetMaxRevVel(), false);

	///////////////////////////////////////////////////////////////////////
	// Tab Collision - Group General
	mpInpUseCollision->SetValue(pEmitter->GetCollides(), false);
	mpInpUpdateRate->SetValue((float)pEmitter->GetCollisionUpdateRate(), false);
	mpInpMaxCollisions->SetValue(cVector2f(pEmitter->GetMinCollisionMax(), pEmitter->GetMaxCollisionMax()), false);
	mpInpBounceAmount->SetValue(cVector2f(pEmitter->GetMinBounceAmount(), pEmitter->GetMaxBounceAmount()), false);

}

//-------------------------------------------------------------


bool cEdEditPaneParticleEmitter::WindowSpecificInputCallback(iEdInput* apInput)
{
	if(mpObject==NULL) return false;
	cEdObjParticleEmitter* pEmitter = static_cast<cEdObjParticleEmitter*>(mpObject);

	///////////////////////////////////////////////////////////////////////
	// Tab General - Group General
	if(apInput==mpInpName)
		pEmitter->SetName(apInput->GetValue());

	else if(apInput==mpInpMaxParticles)
		pEmitter->SetMaxParticleNum((int)mpInpMaxParticles->GetValue());

	else if(apInput==mpInpParticlesPerSec)
		pEmitter->SetParticlesPerSecond(mpInpParticlesPerSec->GetValue());

	else if(apInput==mpInpStartTimeOffset)
		pEmitter->SetStartTimeOffset(mpInpStartTimeOffset->GetValue());

	else if(apInput==mpInpRespawn)
		pEmitter->SetRespawn(mpInpRespawn->GetValue());

	else if(apInput==mpInpWarmUpTime)
		pEmitter->SetWarmUpTime(mpInpWarmUpTime->GetValue());

	else if(apInput==mpInpWarmUpStepsPerSec)
		pEmitter->SetWarmUpStepsPerSec((int)mpInpWarmUpStepsPerSec->GetValue());

	else if(apInput==mpInpLength)
	{
		pEmitter->SetMinPauseLength(mpInpLength->GetValue().x);
		pEmitter->SetMaxPauseLength(mpInpLength->GetValue().y);
	}

	else if(apInput==mpInpInterval)
	{
		pEmitter->SetMinPauseInterval(mpInpInterval->GetValue().x);
		pEmitter->SetMaxPauseInterval(mpInpInterval->GetValue().y);
	}

	else if(apInput==mpInpOffsetPos)
	{
		pEmitter->SetPosOffset(mpInpOffsetPos->GetValue());
	}

	else if(apInput==mpInpOffsetAng)
		pEmitter->SetAngleOffset(mpInpOffsetAng->GetValue());


	///////////////////////////////////////////////////////////////////////
	// Tab Rendering
	// General 
	else if(apInput==mpInpDrawType)
		pEmitter->SetDrawType((eEditorParticleEmitterType)mpInpDrawType->GetValue());

	else if(apInput==mpInpMulRGBWithAlpha)
		pEmitter->SetMultiplyRGBWithAlpha(mpInpMulRGBWithAlpha->GetValue());

	// Size
	else if(apInput==mpInpMinSize)
		pEmitter->SetMinStartSize(mpInpMinSize->GetValue());
	else if(apInput==mpInpMaxSize)
		pEmitter->SetMaxStartSize(mpInpMaxSize->GetValue());

	// Size fading
	else if(apInput==mpInpStartRelSize)
		pEmitter->SetStartRelSize(mpInpStartRelSize->GetValue());

	else if(apInput==mpInpMiddleRelSizeStart)
		pEmitter->SetMiddleRelSizeTime(mpInpMiddleRelSizeStart->GetValue());

	else if(apInput==mpInpMiddleRelSize)
		pEmitter->SetMiddleRelSize(mpInpMiddleRelSize->GetValue());

	else if(apInput==mpInpMiddleRelSizeLength)
		pEmitter->SetMiddleRelSizeLength(mpInpMiddleRelSizeLength->GetValue());

	else if(apInput==mpInpEndRelSize)
		pEmitter->SetEndRelSize(mpInpEndRelSize->GetValue());

	///////////////////////////////////////////////////////////////////////
	// Tab Collision - Group Pause
	else if(apInput==mpInpUseCollision)
		pEmitter->SetCollides(mpInpUseCollision->GetValue());

	else if(apInput==mpInpUpdateRate)
		pEmitter->SetCollisionUpdateRate((int)mpInpUpdateRate->GetValue());

	else if(apInput==mpInpMaxCollisions)
	{
		cVector2f vTemp = mpInpMaxCollisions->GetValue();
		pEmitter->SetMinCollisionMax(vTemp.x);
		pEmitter->SetMaxCollisionMax(vTemp.y);
	}

	else if(apInput==mpInpBounceAmount)
	{
		cVector2f vTemp = mpInpBounceAmount->GetValue();
		pEmitter->SetMinBounceAmount(vTemp.x);
		pEmitter->SetMaxBounceAmount(vTemp.y);
	}

	///////////////////////////////////////////////////////////////////////
	// Tab Rotation
	else if(apInput==mpInpUsePartSpin)
		pEmitter->SetUsePartSpin(mpInpUsePartSpin->GetValue());

	else if(apInput==mpInpSpinType)
		pEmitter->SetPartSpinType((ePEPartSpinType)mpInpSpinType->GetValue());

	else if(apInput==mpInpSpinVelRange)
	{
		cVector2f vTemp = mpInpSpinVelRange->GetValue();
		pEmitter->SetMinSpinRange(vTemp.x);
		pEmitter->SetMaxSpinRange(vTemp.y);
	}
	else if(apInput==mpInpUseRev)
		pEmitter->SetUseRevolution(mpInpUseRev->GetValue());

	else if(apInput==mpInpMinRevVel)
		pEmitter->SetMinRevVel(mpInpMinRevVel->GetValue());

	else if(apInput==mpInpMaxRevVel)
		pEmitter->SetMaxRevVel(mpInpMaxRevVel->GetValue());
	

	///////////////////////////////////////////////////////////////////////
	// Tab Start
	else if(apInput==mpInpStartType)
		pEmitter->SetStartPosType((ePEStartPosType)mpInpStartType->GetValue());

	else if(apInput==this->mpInpBoxStartMinPos)
		pEmitter->SetMinStartPos(mpInpBoxStartMinPos->GetValue());

	else if(apInput==mpInpBoxStartMaxPos)
		pEmitter->SetMaxStartPos(mpInpBoxStartMaxPos->GetValue());

	else if(apInput==mpInpSphStartMinAng)
		pEmitter->SetMinStartAngles(mpInpSphStartMinAng->GetValue());

	else if(apInput==mpInpSphStartMaxAng)
		pEmitter->SetMaxStartAngles(mpInpSphStartMaxAng->GetValue());

	else if(apInput==mpInpSphStartRadius)
	{
		cVector2f vTemp = mpInpSphStartRadius->GetValue();
		pEmitter->SetMinStartRadius(vTemp.x);
		pEmitter->SetMaxStartRadius(vTemp.y);
	}

	///////////////////////////////////////////////////////////////////////
	// Tab Color
	else if(apInput==mpInpMinCol)
		pEmitter->SetMinStartColor(mpInpMinCol->GetValue());

	else if(apInput==mpInpMaxCol)
		pEmitter->SetMaxStartColor(mpInpMaxCol->GetValue());

	else if(apInput==mpInpStartRelCol)
		pEmitter->SetStartRelColor(mpInpStartRelCol->GetValue());

	else if(apInput==mpInpMiddleRelCol)
		pEmitter->SetMiddleRelColor(mpInpMiddleRelCol->GetValue());

	else if(apInput==mpInpEndRelCol)
		pEmitter->SetEndRelColor(mpInpEndRelCol->GetValue());

	else if(apInput==mpInpMiddleRelColStart)
		pEmitter->SetMiddleRelColorTime(mpInpMiddleRelColStart->GetValue());

	else if(apInput==mpInpMiddleRelColLength)
		pEmitter->SetMiddleRelColorLength(mpInpMiddleRelColLength->GetValue());

	///////////////////////////////////////////////////////////////////////
	// Tab Movement
	else if(apInput==mpInpVelType)
		pEmitter->SetStartVelType((ePEStartPosType)mpInpVelType->GetValue());

	else if(apInput==mpInpCoordSys)
		pEmitter->SetCoordSystem((eParticleEmitterCoordSystem)mpInpCoordSys->GetValue());

	else if(apInput==mpInpUseDir)
		pEmitter->SetUsesDirection(mpInpUseDir->GetValue());

	else if(apInput==mpInpGravType)
		pEmitter->SetGravityType((ePEGravityType)mpInpGravType->GetValue());

	else if(apInput==mpInpGravAcc)
		pEmitter->SetGravityAcc(mpInpGravAcc->GetValue());

	else if(apInput==mpInpMinVel)
		pEmitter->SetMinStartVel(mpInpMinVel->GetValue());

	else if(apInput==mpInpMaxVel)
		pEmitter->SetMaxStartVel(mpInpMaxVel->GetValue());

	else if(apInput==mpInpSphVelMinAng)
		pEmitter->SetMinStartVelAngles(mpInpSphVelMinAng->GetValue());

	else if(apInput==mpInpSphVelMaxAng)
		pEmitter->SetMaxStartVelAngles(mpInpSphVelMaxAng->GetValue());

	else if(apInput==mpInpSphVelSpeed)
	{
		cVector2f vTemp = mpInpSphVelSpeed->GetValue();
		pEmitter->SetMinStartVelSpeed(vTemp.x);
		pEmitter->SetMaxStartVelSpeed(vTemp.y);
	}

	else if(apInput==mpInpMinAcc)
		pEmitter->SetMinStartAcc(mpInpMinAcc->GetValue());

	else if(apInput==mpInpMaxAcc)
		pEmitter->SetMaxStartAcc(mpInpMaxAcc->GetValue());

	else if(apInput==mpInpSpeedMul)
	{
		cVector2f vTemp = mpInpSpeedMul->GetValue();
		pEmitter->SetMinSpeedMultiply(vTemp.x);
		pEmitter->SetMaxSpeedMultiply(vTemp.y);
	}

	else if(apInput==mpInpMaxSpeed)
	{
		cVector2f vTemp = mpInpMaxSpeed->GetValue();
		pEmitter->SetMinVelMaximum(vTemp.x);
		pEmitter->SetMaxVelMaximum(vTemp.y);
	}

    ///////////////////////////////////////////////////////////////////////
	// Tab Life
	else if(apInput==mpInpLifeSpan)
	{
		cVector2f vTemp = mpInpLifeSpan->GetValue();
		pEmitter->SetMinLifeSpan(vTemp.x);
		pEmitter->SetMaxLifeSpan(vTemp.y);
	}

	else if(apInput==mpInpDeathType)
		pEmitter->SetDeathType((ePEDeathType)mpInpDeathType->GetValue());

	else if(apInput==mpInpDeathPS)
		pEmitter->SetDeathPS(cString::To8Char(mpInpDeathPS->GetValue()));

	///////////////////////////////////////////////////////////////////////
	// Tab Material
	else if(apInput==mpInpMaterial)
		pEmitter->SetMaterial(cString::To8Char(mpInpMaterial->GetValue()));

	else if(apInput==mpInpNumFiles)
		pEmitter->SetNumFiles((int)mpInpNumFiles->GetValue());

	else if(apInput==mpInpAnimLen)
		pEmitter->SetAnimLength(mpInpAnimLen->GetValue());

	else if(apInput==mpInpSubDAmount)
		pEmitter->SetSubDivAmount(mpInpSubDAmount->GetValue());

	else if(apInput==mpInpSubDType)
		pEmitter->SetSubDivisionType((ePESubDivType)mpInpSubDType->GetValue());

	SetUpdated();
	///////////////////////////////////////////////////////
	// Update particle system after setting values
	mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldObjModify, pEmitter);

	return true;
}

//-------------------------------------------------------------

void cEdEditPaneParticleEmitter::OnSetActive(bool abX)
{
	if(abX==false && mpTFCategories)
		mlLastEditedTab = mpTFCategories->GetTabOnTopIndex();
}

//-------------------------------------------------------------
