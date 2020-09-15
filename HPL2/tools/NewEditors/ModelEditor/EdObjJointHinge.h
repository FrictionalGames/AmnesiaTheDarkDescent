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

#ifndef ED_OBJ_JOINT_HINGE_H
#define ED_OBJ_JOINT_HINGE_H

#include "../Common/StdAfx.h"

using namespace hpl;

#include "../ModelEditor/EdObjJoint.h"

//-----------------------------------------------------------------

enum eJointHingeFloat
{
	eJointHingeFloat_MinAngle = eJointFloat_LastEnum,
	eJointHingeFloat_MaxAngle,

	eJointHingeFloat_LastEnum,
};

//-----------------------------------------------------------------

class cTypeJointHinge : public iEdScnObjType
{
public:
	cTypeJointHinge();

protected:
	iEdObjectData* CreateTypeSpecificData();
};

//-----------------------------------------------------------------

class cEdObjJointHingeData : public iEdObjJointData
{
public:
	cEdObjJointHingeData(iEdObjectType*);

protected:
	iEdObject* CreateTypeSpecificObject();

};

//-----------------------------------------------------------------

class cEdObjJointHinge : public iEdObjJoint
{
public:
	cEdObjJointHinge(iEdObjectData*);

	bool SetProperty(int, const float);
	bool GetProperty(int, float&);

	void SetMinAngle(float afX) { mfMinAngle = afX; }
	void SetMaxAngle(float afX) { mfMaxAngle = afX; }

	float GetMinAngle() { return mfMinAngle; }
	float GetMaxAngle() { return mfMaxAngle; }

protected:
	float mfMinAngle;
	float mfMaxAngle;
};

//-----------------------------------------------------------------

#endif // ED_OBJ_JOINT_HINGE_H
