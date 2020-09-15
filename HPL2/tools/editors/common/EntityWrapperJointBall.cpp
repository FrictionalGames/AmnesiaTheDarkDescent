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

#include "EntityWrapperJointBall.h"

//-----------------------------------------------------------------------

cEntityWrapperTypeJointBall::cEntityWrapperTypeJointBall() : iEntityWrapperTypeJoint(eEditorEntityJointType_Ball, "JointBall")
{
	AddFloat(eJointBallFloat_MaxConeAngle, "MaxConeAngle");
	AddFloat(eJointBallFloat_MaxTwistAngle, "MaxTwistAngle");
}

iEntityWrapperData* cEntityWrapperTypeJointBall::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataJointBall,(this));
}

cEntityWrapperDataJointBall::cEntityWrapperDataJointBall(iEntityWrapperType* apType) : iEntityWrapperDataJoint(apType)
{	
//	msName = "BallJoint";

//	mfMaxConeAngle = 0;
//	mfMaxTwistAngle = 0;
}

//-----------------------------------------------------------------------

/*
void cEntityWrapperDataJointBall::CopyFromEntity(iEntityWrapper* apEntity)
{
	cEntityWrapperDataJoint::CopyFromEntity(apEntity);
	cEntityWrapperJointBall* pEnt = (cEntityWrapperJointBall*)apEntity;
	
	mfMaxConeAngle = pEnt->GetMaxConeAngle();
	mfMaxTwistAngle = pEnt->GetMaxTwistAngle();
}

//-----------------------------------------------------------------------

void cEntityWrapperDataJointBall::CopyToEntity(iEntityWrapper* apEntity)
{
	cEntityWrapperDataJoint::CopyToEntity(apEntity);
	cEntityWrapperJointBall* pEnt = (cEntityWrapperJointBall*)apEntity;
	
	pEnt->SetMaxConeAngle(mfMaxConeAngle);
	pEnt->SetMaxTwistAngle(mfMaxTwistAngle);
}

//-----------------------------------------------------------------------

void cEntityWrapperDataJointBall::Load(cXmlElement* apElement)
{
	cEntityWrapperDataJoint::Load(apElement);

	mfMaxConeAngle = apElement->GetAttributeFloat("MaxConeAngle");
	mfMaxTwistAngle = apElement->GetAttributeFloat("MaxTwistAngle");
}
*/
//-----------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataJointBall::CreateSpecificEntity()
{
	return hplNew( cEntityWrapperJointBall,(this));
}


//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cEntityWrapperJointBall::cEntityWrapperJointBall(iEntityWrapperData* apData) : iEntityWrapperJoint(apData)
{
	mfMaxConeAngle = 0;
	mfMaxTwistAngle = 0;
}

//-----------------------------------------------------------------------

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cEntityWrapperJointBall::SetProperty(int alPropID, const float& afX)
{
	switch(alPropID)
	{
	case eJointBallFloat_MaxConeAngle:
		SetMaxConeAngle(afX);	break;
	case eJointBallFloat_MaxTwistAngle:
		SetMaxTwistAngle(afX);	break;
	default:
		return iEntityWrapperJoint::SetProperty(alPropID, afX);
	}

	return true;
}

bool cEntityWrapperJointBall::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eJointBallFloat_MaxConeAngle:
		afX = GetMaxConeAngle();	break;
	case eJointBallFloat_MaxTwistAngle:
		afX = GetMaxTwistAngle();	break;
	default:
		return iEntityWrapperJoint::GetProperty(alPropID, afX);
	}

	return true;
}

/*
void cEntityWrapperJointBall::SaveToElement(cXmlElement* apElement)
{
	cEntityWrapperJoint::SaveToElement(apElement);

	apElement->SetValue("JointBall");

	apElement->SetAttributeFloat("MaxConeAngle", mfMaxConeAngle);
	apElement->SetAttributeFloat("MaxTwistAngle", mfMaxTwistAngle);
}
*/
//-----------------------------------------------------------------------

/*
void cEntityWrapperJointBall::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected)
{
	cEntityWrapperJoint::Draw(apViewport,apFunctions, apEditMode, abIsSelected);
}
*/
//-----------------------------------------------------------------------
