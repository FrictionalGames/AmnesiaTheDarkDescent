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

#include "EntityWrapperJointHinge.h"


//-----------------------------------------------------------------------

cEntityWrapperTypeJointHinge::cEntityWrapperTypeJointHinge() : iEntityWrapperTypeJoint(eEditorEntityJointType_Hinge, "JointHinge")
{
	AddFloat(eJointHingeFloat_MinAngle, "MinAngle");
	AddFloat(eJointHingeFloat_MaxAngle, "MaxAngle");
}

iEntityWrapperData* cEntityWrapperTypeJointHinge::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataJointHinge,(this));
}

//-----------------------------------------------------------------------

cEntityWrapperDataJointHinge::cEntityWrapperDataJointHinge(iEntityWrapperType* apType) : iEntityWrapperDataJoint(apType)
{
}


//-----------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataJointHinge::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperJointHinge,(this));
}

////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cEntityWrapperJointHinge::cEntityWrapperJointHinge(iEntityWrapperData* apData) : iEntityWrapperJoint(apData)
{
//	mfMinAngle = 0;
//	mfMaxAngle = 0;
}

//-----------------------------------------------------------------------

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cEntityWrapperJointHinge::SetProperty(int alPropID, const float& afX)
{
	switch(alPropID)
	{
	case eJointHingeFloat_MinAngle:
		SetMinAngle(afX); break;
	case eJointHingeFloat_MaxAngle:
		SetMaxAngle(afX); break;
	default:
		return iEntityWrapperJoint::SetProperty(alPropID, afX);
	}
	
	return true;
}

bool cEntityWrapperJointHinge::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eJointHingeFloat_MinAngle:
		afX = GetMinAngle();	break;
	case eJointHingeFloat_MaxAngle:
		afX = GetMaxAngle();	break;
	default:
		return iEntityWrapperJoint::GetProperty(alPropID, afX);
	}
	
	return true;
}

//-----------------------------------------------------------------------
