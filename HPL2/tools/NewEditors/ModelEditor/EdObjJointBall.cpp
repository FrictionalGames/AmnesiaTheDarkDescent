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

#include "../ModelEditor/EdObjJointBall.h"

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// TYPE JOINT BALL - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cTypeJointBall::cTypeJointBall() : iEdScnObjType(_W("Ball"), "JointBall")
{
	AddFloat(eJointBallFloat_MaxConeAngle, "MaxConeAngle");
	AddFloat(eJointBallFloat_MaxTwistAngle, "MaxTwistAngle");

	mbScalable = false;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// TYPE JOINT BALL - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEdObjectData* cTypeJointBall::CreateTypeSpecificData()
{
	return hplNew(cEdObjJointBallData,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// JOINT BALL DATA - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjJointBallData::cEdObjJointBallData(iEdObjectType* apType) : iEdObjJointData(apType)
{	
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// JOINT BALL DATA - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEdObject* cEdObjJointBallData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjJointBall,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// JOINT BALL - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjJointBall::cEdObjJointBall(iEdObjectData* apData) : iEdObjJoint(apData)
{
	//mfMaxConeAngle = 0;
	//mfMaxTwistAngle = 0;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// JOINT BALL - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEdObjJointBall::SetProperty(int alPropID, const float afX)
{
	switch(alPropID)
	{
	case eJointBallFloat_MaxConeAngle:
		SetMaxConeAngle(afX);	break;
	case eJointBallFloat_MaxTwistAngle:
		SetMaxTwistAngle(afX);	break;
	default:
		return iEdObjJoint::SetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjJointBall::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eJointBallFloat_MaxConeAngle:
		afX = GetMaxConeAngle();	break;
	case eJointBallFloat_MaxTwistAngle:
		afX = GetMaxTwistAngle();	break;
	default:
		return iEdObjJoint::GetProperty(alPropID, afX);
	}

	return true;
}

/*
void cEdObjJointBall::SaveToElement(cXmlElement* apElement)
{
	cEntityWrapperJoint::SaveToElement(apElement);

	apElement->SetValue("JointBall");

	apElement->SetAttributeFloat("MaxConeAngle", mfMaxConeAngle);
	apElement->SetAttributeFloat("MaxTwistAngle", mfMaxTwistAngle);
}
*/
//-----------------------------------------------------------------------

/*
void cEdObjJointBall::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEdEditMode* apEditMode, bool abIsSelected)
{
	cEntityWrapperJoint::Draw(apViewport,apFunctions, apEditMode, abIsSelected);
}
*/
//-----------------------------------------------------------------------
