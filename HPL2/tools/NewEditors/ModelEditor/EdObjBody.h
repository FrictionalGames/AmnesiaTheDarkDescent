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

#ifndef ED_OBJ_BODY_H
#define ED_OBJ_BODY_H

#include "../Common/StdAfx.h"

using namespace hpl;

#include "../Common/EdScnObject.h"

#include "../ModelEditor/EdObjJoint.h"

class iEdObjJoint;

//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------

class cEdEditPaneBody;


enum eBodyStr
{
	eBodyStr_Material = eObjStr_LastEnum,

	eBodyStr_LastEnum
};

enum eBodyFloat
{
	eBodyFloat_Mass = eObjFloat_LastEnum,
	eBodyFloat_AngularDamping,
	eBodyFloat_LinearDamping,
	eBodyFloat_MaxAngularSpeed,
	eBodyFloat_MaxLinearSpeed,
	eBodyFloat_BuoyancyDensityMul,

	eBodyFloat_LastEnum
};

enum eBodyBool
{
	eBodyBool_BlockSound = eObjBool_LastEnum,
	eBodyBool_ContinuousCollision,
	eBodyBool_CanAttachCharacter,
	eBodyBool_PushedByCharacterGravity,
	eBodyBool_CollideCharacter,
	eBodyBool_CollideNonCharacter,
	eBodyBool_Volatile,
	eBodyBool_UseSurfaceEffects,
	eBodyBool_HasGravity,

	eBodyBool_LastEnum
};

class cTypeBody : public iBaseAggregateType
{
public:
	cTypeBody();

	bool AllowNullEngineObject() { return true; }

protected:
	iEdObjectData* CreateTypeSpecificData();
};

class cEdObjBodyData : public iBaseAggregateObjData
{
public:
	cEdObjBodyData(iEdObjectType*);

	void CopyFromObject(iEdObject*);
	void CopyToObject(iEdObject*, ePropStep);

protected:
	iEdObject* CreateTypeSpecificObject();

	tIntVec mvParentJointIDs;
	tIntVec mvChildJointIDs;
};

//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------

//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------

class cEdObjBody : public iBaseAggregateObject
{
public:
	cEdObjBody(iEdObjectData*);
	~cEdObjBody();

	bool SetProperty(int, const float);
	bool SetProperty(int, const bool);
	bool SetProperty(int, const tString&);
	bool GetProperty(int, float&);
	bool GetProperty(int, bool&);
	bool GetProperty(int, tString&);

	void OnDraw(const cModuleDrawData&);
	//void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions,iEdEditMode* apEditMode, 
	//			bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol);

	iEdEditPane* CreateEditPane();

	void SetMaterial(const tString& asMaterial) { msMaterial = asMaterial; }

	void SetMass(float afX) { mfMass = afX; }
	void SetAngularDamping(float afX) { mfAngularDamping = afX; }
	void SetLinearDamping(float afX) { mfLinearDamping = afX; }
	void SetMaxAngularSpeed(float afX) { mfMaxAngularSpeed = afX; }
	void SetMaxLinearSpeed(float afX) { mfMaxLinearSpeed = afX; }
	void SetBuoyancyDensityMul(float afX) { mfBuoyancyDensityMul = afX; }

	tString& GetMaterial() { return msMaterial; }
	float GetMass() { return mfMass; }
	float GetAngularDamping() { return mfAngularDamping; }
	float GetLinearDamping() { return mfLinearDamping; }
	float GetMaxAngularSpeed() { return mfMaxAngularSpeed; }
	float GetMaxLinearSpeed() { return mfMaxLinearSpeed; }
	float GetBuoyancyDensityMul() { return mfBuoyancyDensityMul; }
    
	void SetBlocksSound(bool abX) { mbBlocksSound = abX; }
	void SetContinuousCollision(bool abX) { mbContinuousCollision = abX; }
	void SetCanAttachCharacter(bool abX) { mbCanAttachCharacter = abX; }
	void SetPushedByCharacterGravity(bool abX) { mbPushedByCharacterGravity = abX; }
	void SetCollideCharacter(bool abX) { mbCollideCharacter = abX; }
	void SetCollideNonCharacter(bool abX) { mbCollideNonCharacter = abX; }
	void SetVolatile(bool abX) { mbVolatile = abX; }
	void SetUseSurfaceEffects(bool abX) { mbUseSurfaceEffects = abX; }
	void SetHasGravity(bool abX) { mbHasGravity = abX; }


	bool BlocksSound() { return mbBlocksSound; }
	bool HasContinuousCollision() { return mbContinuousCollision; }
	bool CanAttachCharacter() { return mbCanAttachCharacter; }
	bool IsPushedByCharacterGravity() { return mbPushedByCharacterGravity; }
	bool CollidesCharacter() { return mbCollideCharacter; }
	bool CollidesNonCharacter() { return mbCollideNonCharacter; }
	bool IsVolatile() { return mbVolatile; }
	bool UsesSurfaceEffects() { return mbUseSurfaceEffects; }
	bool HasGravity() { return mbHasGravity; }


	void AddParentJoint(iEdObjJoint*);
	void AddChildJoint(iEdObjJoint*);
	
	void RemoveJoint(iEdObjJoint*, bool abModifyJoint);
	void RemoveJointHelper(iEdObjJoint*, bool);

	void ClearJoints();

	const tEdJointVec& GetParentJoints() { return mvParentJoints; }
	const tEdJointVec& GetChildJoints() { return mvChildJoints; }

	iEdAction* CreateAddComponentsAction(const tScnObjList&);
	iEdAction* CreateRemoveComponentsAction(const tScnObjList&);

	void AddToOutlineSpecific(cWidgetListBox* apList, int alLevel, tScnObjList& alstAddedChildren);

	void DrawObject(const cModuleDrawData& aData, bool abHighlight, const cColor& aHighlightCol);
protected:
	void OnAddComponent(iEdScnObject*);
	void OnRemoveComponent(iEdScnObject*);
	void OnUpdateRelativeTransform(iEdScnObject*, const cMatrixf&);
	void UpdateComponents();
	///////////////////////////
	// Data
	tString msMaterial;

	float mfMass;
	float mfAngularDamping;
	float mfLinearDamping;
	float mfMaxAngularSpeed;
	float mfMaxLinearSpeed;
	float mfBuoyancyDensityMul;

	bool mbBlocksSound;
	bool mbContinuousCollision;
	bool mbCanAttachCharacter;
	bool mbPushedByCharacterGravity;
	bool mbCollideCharacter;
	bool mbCollideNonCharacter;
	bool mbVolatile;
	bool mbUseSurfaceEffects;
	bool mbHasGravity;

	tEdJointVec mvParentJoints;
	tEdJointVec mvChildJoints;
};

//---------------------------------------------------------------

//---------------------------------------------------------------

#endif // ED_OBJ_BODY_H
