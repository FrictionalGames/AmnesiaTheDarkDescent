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

#include "../ModelEditor/EdObjJointHinge.h"

//-----------------------------------------------------------------------

cTypeJointHinge::cTypeJointHinge() : iEdScnObjType(_W("Hinge"), "JointHinge")
{
	AddFloat(eJointHingeFloat_MinAngle, "MinAngle");
	AddFloat(eJointHingeFloat_MaxAngle, "MaxAngle");

	mbScalable = false;
}

iEdObjectData* cTypeJointHinge::CreateTypeSpecificData()
{
	return hplNew(cEdObjJointHingeData,(this));
}

//-----------------------------------------------------------------------

cEdObjJointHingeData::cEdObjJointHingeData(iEdObjectType* apType) : iEdObjJointData(apType)
{
}


//-----------------------------------------------------------------------

iEdObject* cEdObjJointHingeData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjJointHinge,(this));
}

//-----------------------------------------------------------------------

////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cEdObjJointHinge::cEdObjJointHinge(iEdObjectData* apData) : iEdObjJoint(apData)
{
}

//-----------------------------------------------------------------------

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cEdObjJointHinge::SetProperty(int alPropID, const float afX)
{
	switch(alPropID)
	{
	case eJointHingeFloat_MinAngle:
		SetMinAngle(afX); break;
	case eJointHingeFloat_MaxAngle:
		SetMaxAngle(afX); break;
	default:
		return iEdObjJoint::SetProperty(alPropID, afX);
	}
	
	return true;
}

bool cEdObjJointHinge::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eJointHingeFloat_MinAngle:
		afX = GetMinAngle();	break;
	case eJointHingeFloat_MaxAngle:
		afX = GetMaxAngle();	break;
	default:
		return iEdObjJoint::GetProperty(alPropID, afX);
	}
	
	return true;
}

//-----------------------------------------------------------------------
