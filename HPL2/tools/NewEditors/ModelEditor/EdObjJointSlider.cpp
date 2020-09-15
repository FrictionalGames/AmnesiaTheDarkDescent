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

#include "../ModelEditor/EdObjJointSlider.h"


//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// TYPE JOINT SLIDER - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cTypeJointSlider::cTypeJointSlider() : iEdScnObjType(_W("Slider"), "JointSlider")
{
	AddFloat(eJointSliderFloat_MinDistance, "MinDistance");
	AddFloat(eJointSliderFloat_MaxDistance, "MaxDistance");

	mbScalable = false;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// TYPE JOINT SLIDER - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEdObjectData* cTypeJointSlider::CreateTypeSpecificData()
{
	return hplNew(cEdObjJointSliderData,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// JOINT SLIDER DATA - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjJointSliderData::cEdObjJointSliderData(iEdObjectType* apType) : iEdObjJointData(apType)
{
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// JOINT SLIDER DATA - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEdObject* cEdObjJointSliderData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjJointSlider,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// JOINT SLIDER - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjJointSlider::cEdObjJointSlider(iEdObjectData* apData) : iEdObjJoint(apData)
{
	mfMinDistance = 0;
	mfMaxDistance = 0;
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// JOINT SLIDER - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEdObjJointSlider::SetProperty(int alPropID, const float afX)
{
	switch(alPropID)
	{
	case eJointSliderFloat_MinDistance:
		SetMinDistance(afX);	break;
	case eJointSliderFloat_MaxDistance:
		SetMaxDistance(afX);	break;
	default:
		return iEdObjJoint::SetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjJointSlider::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eJointSliderFloat_MinDistance:
		afX = GetMinDistance();	break;
	case eJointSliderFloat_MaxDistance:
		afX = GetMaxDistance();	break;
	default:
		return iEdObjJoint::GetProperty(alPropID, afX);
	}

	return true;
}

//-----------------------------------------------------------------------
