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

#include "EntityWrapperJointScrew.h"


//-----------------------------------------------------------------------

cEntityWrapperTypeJointScrew::cEntityWrapperTypeJointScrew() : iEntityWrapperTypeJoint(eEditorEntityJointType_Screw, "JointScrew")
{
	AddFloat(eJointScrewFloat_MinDistance, "MinDistance");
	AddFloat(eJointScrewFloat_MaxDistance, "MaxDistance");
}

iEntityWrapperData* cEntityWrapperTypeJointScrew::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataJointScrew,(this));
}

//-----------------------------------------------------------------------

cEntityWrapperDataJointScrew::cEntityWrapperDataJointScrew(iEntityWrapperType* apType) : iEntityWrapperDataJoint(apType)
{
}

//-----------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataJointScrew::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperJointScrew,(this));
}

//-----------------------------------------------------------------------

////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cEntityWrapperJointScrew::cEntityWrapperJointScrew(iEntityWrapperData* apData) : iEntityWrapperJoint(apData)
{
	//mfMinDistance = 0;
	//mfMaxDistance = 0;
	//mvPin = 0;
}

//-----------------------------------------------------------------------

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cEntityWrapperJointScrew::SetProperty(int alPropID, const float& afX)
{
	switch(alPropID)
	{
	case eJointScrewFloat_MinDistance:
		SetMinDistance(afX);	break;
	case eJointScrewFloat_MaxDistance:
		SetMaxDistance(afX);	break;
	default:
		return iEntityWrapperJoint::SetProperty(alPropID, afX);
	}

	return true;
}

bool cEntityWrapperJointScrew::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eJointScrewFloat_MinDistance:
		afX = GetMinDistance();	break;
	case eJointScrewFloat_MaxDistance:
		afX = GetMaxDistance();	break;
	default:
		return iEntityWrapperJoint::GetProperty(alPropID, afX);
	}

	return true;
}

/*
void cEntityWrapperJointScrew::SaveToElement(cXmlElement* apElement)
{
	cEntityWrapperJoint::SaveToElement(apElement);

	apElement->SetValue("JointScrew");

	mvPin.Normalize();

	apElement->SetAttributeFloat("MinDistance", mfMinDistance);
	apElement->SetAttributeFloat("MaxDistance", mfMaxDistance);
	apElement->SetAttributeVector3f("Pin", mvPin);
}
*/

//-----------------------------------------------------------------------
