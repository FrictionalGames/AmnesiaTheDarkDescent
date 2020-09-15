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

#include "EntityWrapperJointSlider.h"


//-----------------------------------------------------------------------

cEntityWrapperTypeJointSlider::cEntityWrapperTypeJointSlider() : iEntityWrapperTypeJoint(eEditorEntityJointType_Slider, "JointSlider")
{
	AddFloat(eJointSliderFloat_MinDistance, "MinDistance");
	AddFloat(eJointSliderFloat_MaxDistance, "MaxDistance");
}

iEntityWrapperData* cEntityWrapperTypeJointSlider::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataJointSlider,(this));
}

//-----------------------------------------------------------------------

cEntityWrapperDataJointSlider::cEntityWrapperDataJointSlider(iEntityWrapperType* apType) : iEntityWrapperDataJoint(apType)
{
	//msName = "SliderJoint";

	//mfMinDistance = 0;
	//mfMaxDistance = 0;
}

//-----------------------------------------------------------------------
/*
void cEntityWrapperDataJointSlider::CopyFromEntity(iEntityWrapper* apEntity)
{
	cEntityWrapperDataJoint::CopyFromEntity(apEntity);
	cEntityWrapperJointSlider* pEnt = (cEntityWrapperJointSlider*)apEntity;
	
	mfMinDistance = pEnt->GetMinDistance();
	mfMaxDistance = pEnt->GetMaxDistance();
}

//-----------------------------------------------------------------------

void cEntityWrapperDataJointSlider::CopyToEntity(iEntityWrapper* apEntity)
{
	cEntityWrapperDataJoint::CopyToEntity(apEntity);
	cEntityWrapperJointSlider* pEnt = (cEntityWrapperJointSlider*)apEntity;
	
	pEnt->SetMinDistance(mfMinDistance);
	pEnt->SetMaxDistance(mfMaxDistance);
}

//-----------------------------------------------------------------------

void cEntityWrapperDataJointSlider::Load(cXmlElement* apElement)
{
	cEntityWrapperDataJoint::Load(apElement);

	mfMinDistance = apElement->GetAttributeFloat("MinDistance");
	mfMaxDistance = apElement->GetAttributeFloat("MaxDistance");
}
*/
//-----------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataJointSlider::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperJointSlider,(this));
}

//-----------------------------------------------------------------------

////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cEntityWrapperJointSlider::cEntityWrapperJointSlider(iEntityWrapperData* apData) : iEntityWrapperJoint(apData)
{
	mfMinDistance = 0;
	mfMaxDistance = 0;
}

//-----------------------------------------------------------------------

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cEntityWrapperJointSlider::SetProperty(int alPropID, const float& afX)
{
	switch(alPropID)
	{
	case eJointSliderFloat_MinDistance:
		SetMinDistance(afX);	break;
	case eJointSliderFloat_MaxDistance:
		SetMaxDistance(afX);	break;
	default:
		return iEntityWrapperJoint::SetProperty(alPropID, afX);
	}

	return true;
}

bool cEntityWrapperJointSlider::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eJointSliderFloat_MinDistance:
		afX = GetMinDistance();	break;
	case eJointSliderFloat_MaxDistance:
		afX = GetMaxDistance();	break;
	default:
		return iEntityWrapperJoint::GetProperty(alPropID, afX);
	}

	return true;
}

//-----------------------------------------------------------------------
