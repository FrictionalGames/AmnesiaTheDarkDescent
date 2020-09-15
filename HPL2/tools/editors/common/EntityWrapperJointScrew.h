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

#ifndef HPLEDITOR_ENTITY_WRAPPER_JOINT_SCREW_H
#define HPLEDITOR_ENTITY_WRAPPER_JOINT_SCREW_H

#include "StdAfx.h"

using namespace hpl;

#include "EntityWrapperJoint.h"

//-----------------------------------------------------------------

enum eJointScrewFloat
{
	eJointScrewFloat_MinDistance = eJointFloat_LastEnum,
	eJointScrewFloat_MaxDistance,

	eJointScrewFloat_LastEnum,
};

//-----------------------------------------------------------------

class cEntityWrapperTypeJointScrew : public iEntityWrapperTypeJoint
{
public:
	cEntityWrapperTypeJointScrew();

protected:
	iEntityWrapperData* CreateSpecificData();
};

//-----------------------------------------------------------------

class cEntityWrapperDataJointScrew : public iEntityWrapperDataJoint
{
public:
	cEntityWrapperDataJointScrew(iEntityWrapperType*);
	
protected:
	iEntityWrapper* CreateSpecificEntity();
};

//-----------------------------------------------------------------

class cEntityWrapperJointScrew : public iEntityWrapperJoint
{
public:
	cEntityWrapperJointScrew(iEntityWrapperData*);

	bool SetProperty(int, const float&);
	bool GetProperty(int, float&);

	void SetMinDistance(float afX) { mfMinDistance = afX; }
	void SetMaxDistance(float afX) { mfMaxDistance = afX; }

	float GetMinDistance() { return mfMinDistance; }
	float GetMaxDistance() { return mfMaxDistance; }

protected:
	float mfMinDistance;
	float mfMaxDistance;
};

//-----------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_JOINT_SCREW_H
