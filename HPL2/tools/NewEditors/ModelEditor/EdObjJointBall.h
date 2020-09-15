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

#ifndef ED_OBJ_JOINT_BALL_H
#define ED_OBJ_JOINT_BALL_H

#include "../Common/StdAfx.h"

using namespace hpl;

#include "../ModelEditor/EdObjJoint.h"

//-----------------------------------------------------------------

enum eJointBallFloat
{
	eJointBallFloat_MaxConeAngle = eJointFloat_LastEnum, 
	eJointBallFloat_MaxTwistAngle,

	eJointBallFloat_LastEnum,
};

//-----------------------------------------------------------------

class cTypeJointBall : public iEdScnObjType
{
public:
	cTypeJointBall();

protected:
	iEdObjectData* CreateTypeSpecificData();
};

//-----------------------------------------------------------------

class cEdObjJointBallData : public iEdObjJointData
{
public:
	cEdObjJointBallData(iEdObjectType*);
	
protected:
	iEdObject* CreateTypeSpecificObject();

};

//-----------------------------------------------------------------

class cEdObjJointBall : public iEdObjJoint
{
public:
	cEdObjJointBall(iEdObjectData*);

	bool SetProperty(int, const float);
	bool GetProperty(int, float&);

	void SetMaxConeAngle(float afX) { mfMaxConeAngle = afX; }
	void SetMaxTwistAngle(float afX) { mfMaxTwistAngle = afX; }
	
	float GetMaxConeAngle() { return mfMaxConeAngle; }
	float GetMaxTwistAngle() { return mfMaxTwistAngle; }
	
	//void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEdEditMode* apEditMode, bool abIsSelected);

	//void SaveToElement(cXmlElement* apElement);
	
protected:
	float mfMaxConeAngle;
	float mfMaxTwistAngle;
};

//-----------------------------------------------------------------

#endif // ED_OBJ_JOINT_BALL_H
