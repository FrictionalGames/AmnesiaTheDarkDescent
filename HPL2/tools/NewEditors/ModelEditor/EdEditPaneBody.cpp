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

#include "../ModelEditor/EdEditPaneBody.h"
#include "../Common/Editor.h"

#include "../ModelEditor/EdObjBody.h"

#include <algorithm>

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEdEditPaneBody::cEdEditPaneBody(iEdObject* apObject) : iEdScnObjEditPane(apObject)
{
}

//----------------------------------------------------------------------------

cEdEditPaneBody::~cEdEditPaneBody()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneBody::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	mpTabBody = mpTabs->AddTab(_W("Body"));
	mpTabAttachments = mpTabs->AddTab(_W("Attachments"));

	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);
	AddPropertyScale(mpTabGeneral);

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y;
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y;
	mpInpScale->SetPosition(vPos);
	vPos.y += mpInpScale->GetSize().y;

    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddPropertySetBody(mpTabBody);

	////////////////////////////////////////
	// Attachment stuff
	AddPropertySetAttachments(mpTabAttachments);
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEdEditPaneBody::AddPropertySetBody(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,30,0.1f);
	cVector2f vSize = cVector2f(120, 25);
	
	float fSep = 5;
	float fLongSep = 10;

	mpInpMaterial = CreateInputEnum(vPos, _W("Body Material"), apParentTab, 120);
	cSurfaceDataIterator it = mpEditor->GetEngine()->GetPhysics()->GetSurfaceDataIterator();
	while(it.HasNext())
	{
        cSurfaceData* pSData = static_cast<cSurfaceData*>(it.Next());
		tString sMaterialName = pSData->GetName();
		mpInpMaterial->AddValue(cString::To16Char(sMaterialName));
	}

	vPos.y += mpInpMaterial->GetSize().y + fSep;

	mpInpMass = CreateInputNumber(vPos, _W("Mass"), apParentTab, 50, 0.5f);
	mpInpMass->SetLowerBound(true, 0);
	vPos.y += mpInpMass->GetSize().y + fSep;
	
	mpInpAngularDamping = CreateInputNumber(vPos, _W("Angular Damping"), apParentTab, 50, 0.5f);
	vPos.y += mpInpAngularDamping->GetSize().y + fSep;

	mpInpLinearDamping = CreateInputNumber(vPos, _W("Linear Damping"), apParentTab, 50, 0.5f);
	vPos.y += mpInpLinearDamping->GetSize().y + fSep;

	mpInpMaxAngularSpeed = CreateInputNumber(vPos, _W("Max Angular Speed"), apParentTab, 50, 0.5f);
	vPos.y += mpInpMaxAngularSpeed->GetSize().y + fSep;

	mpInpMaxLinearSpeed = CreateInputNumber(vPos, _W("Max Linear Speed"), apParentTab, 50, 0.5f);
	vPos.y += mpInpMaxLinearSpeed->GetSize().y + fLongSep;

	mpInpBuoyancyDensityMul = CreateInputNumber(vPos, _W("Buoyancy density mul"), apParentTab, 50, 0.1f);
	vPos.y += mpInpBuoyancyDensityMul->GetSize().y + fLongSep;

	mpInpBlocksSound = CreateInputBool(vPos, _W("Blocks sound"), apParentTab);
	vPos.y += mpInpBlocksSound->GetSize().y + fSep;

	mpInpContinuousCollision = CreateInputBool(vPos, _W("Continuous collision"), apParentTab);
	vPos.y += mpInpContinuousCollision->GetSize().y + fSep;

	mpInpCanAttachCharacter = CreateInputBool(vPos, _W("Can attach character"), apParentTab);
	vPos.y += mpInpCanAttachCharacter->GetSize().y + fSep;

	mpInpPushedByCharacterGravity = CreateInputBool(vPos, _W("Pushed by character gravity"), apParentTab);
	vPos.y += mpInpPushedByCharacterGravity->GetSize().y + fSep;

	mpInpCollideCharacter = CreateInputBool(vPos, _W("Collide character"), apParentTab);
	vPos.y += mpInpCollideCharacter->GetSize().y + fSep;

	mpInpCollideNonCharacter = CreateInputBool(vPos, _W("Collide non character"), apParentTab);
	vPos.y += mpInpCollideNonCharacter->GetSize().y + fSep;

	mpInpVolatile = CreateInputBool(vPos, _W("Volatile"), apParentTab);
	vPos.y += mpInpVolatile->GetSize().y + fSep;

	mpInpUseSurfaceEffects = CreateInputBool(vPos, _W("Use surface effects"), apParentTab);
	vPos.y += mpInpUseSurfaceEffects->GetSize().y + fSep;

	mpInpHasGravity = CreateInputBool(vPos, _W("Has gravity"), apParentTab);
}

//----------------------------------------------------------------------------

void cEdEditPaneBody::OnUpdate()
{
	iEdScnObjEditPane::OnUpdate();

	cEdObjBody* pBody = static_cast<cEdObjBody*>(mpObject);

	mpInpMaterial->SetValue(cString::To16Char(pBody->GetMaterial()), false);
	mpInpMass->SetValue(pBody->GetMass(), false);
	mpInpAngularDamping->SetValue(pBody->GetAngularDamping(),false);
	mpInpLinearDamping->SetValue(pBody->GetLinearDamping(),false);
	mpInpMaxAngularSpeed->SetValue(pBody->GetMaxAngularSpeed(),false);
	mpInpMaxLinearSpeed->SetValue(pBody->GetMaxLinearSpeed(),false);
	mpInpBuoyancyDensityMul->SetValue(pBody->GetBuoyancyDensityMul(),false);

	mpInpBlocksSound->SetValue(pBody->BlocksSound(), false);
	mpInpContinuousCollision->SetValue(pBody->HasContinuousCollision(), false);
	mpInpCanAttachCharacter->SetValue(pBody->CanAttachCharacter(), false);
	mpInpPushedByCharacterGravity->SetValue(pBody->IsPushedByCharacterGravity(), false);
	mpInpCollideCharacter->SetValue(pBody->CollidesCharacter(), false);
	mpInpCollideNonCharacter->SetValue(pBody->CollidesNonCharacter(), false);
	mpInpVolatile->SetValue(pBody->IsVolatile(), false);
	mpInpUseSurfaceEffects->SetValue(pBody->UsesSurfaceEffects(), false);
	mpInpHasGravity->SetValue(pBody->HasGravity(), false);
}

//----------------------------------------------------------------------------

bool cEdEditPaneBody::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdEditPaneBody,InputCallback);


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

bool cEdEditPaneBody::WindowSpecificInputCallback(iEdInput* apInput)
{
	iEdAction* pAction = NULL;
	iEdWorld* pWorld = mpEditor->GetWorld();

	///////////////////////////////////////////
	// Material
	if(apInput == mpInpMaterial)
	{
		pAction = mpObject->CreateSetStringAction(eBodyStr_Material, mpInpMaterial->GetValue8Char());
	}
	///////////////////////////////////////////
	// Mass
	else if(apInput == mpInpMass)
	{
		pAction = mpObject->CreateSetFloatAction(eBodyFloat_Mass, mpInpMass->GetValue());
	}
	///////////////////////////////////////////
	// Angular Damping
	else if(apInput == mpInpAngularDamping)
	{
		pAction = mpObject->CreateSetFloatAction(eBodyFloat_AngularDamping, mpInpAngularDamping->GetValue());
	}
	///////////////////////////////////////////
	// Linear Damping
	else if(apInput == mpInpLinearDamping)
	{
		pAction = mpObject->CreateSetFloatAction(eBodyFloat_LinearDamping, mpInpLinearDamping->GetValue());
	}
	///////////////////////////////////////////
	// Max Angular Speed
	else if(apInput == mpInpMaxAngularSpeed)
	{
		pAction = mpObject->CreateSetFloatAction(eBodyFloat_MaxAngularSpeed, mpInpMaxAngularSpeed->GetValue());
	}
	///////////////////////////////////////////
	// Max Linear Speed
	else if(apInput == mpInpMaxLinearSpeed) 
	{
		pAction = mpObject->CreateSetFloatAction(eBodyFloat_MaxLinearSpeed, mpInpMaxLinearSpeed->GetValue());
	}
	///////////////////////////////////////////
	// Block sound
	else if(apInput == mpInpBlocksSound)
	{
		pAction = mpObject->CreateSetBoolAction(eBodyBool_BlockSound, mpInpBlocksSound->GetValue());
	}
	///////////////////////////////////////////
	// Continuous collision
	else if(apInput == mpInpContinuousCollision)
	{
		pAction = mpObject->CreateSetBoolAction(eBodyBool_ContinuousCollision, mpInpContinuousCollision->GetValue());
	}
	///////////////////////////////////////////
	// Can attach character
	else if(apInput == mpInpCanAttachCharacter)
	{
		pAction = mpObject->CreateSetBoolAction(eBodyBool_CanAttachCharacter, mpInpCanAttachCharacter->GetValue());
	}
	///////////////////////////////////////////
	// Pushed by character gravity
	else if(apInput == mpInpPushedByCharacterGravity)
	{
		pAction = mpObject->CreateSetBoolAction(eBodyBool_PushedByCharacterGravity, mpInpPushedByCharacterGravity->GetValue());
	}
	///////////////////////////////////////////
	// Collide character
	else if(apInput == mpInpCollideCharacter)
	{
		pAction = mpObject->CreateSetBoolAction(eBodyBool_CollideCharacter, mpInpCollideCharacter->GetValue());
	}
	///////////////////////////////////////////
	// Collide non character
	else if(apInput == mpInpCollideNonCharacter)
	{
		pAction = mpObject->CreateSetBoolAction(eBodyBool_CollideNonCharacter, mpInpCollideNonCharacter->GetValue());
	}
	///////////////////////////////////////////
	// Volatile
	else if(apInput == mpInpVolatile)
	{
		pAction = mpObject->CreateSetBoolAction(eBodyBool_Volatile, mpInpVolatile->GetValue());
	}

	else if(apInput==mpInpBuoyancyDensityMul)
	{
		pAction = mpObject->CreateSetFloatAction(eBodyFloat_BuoyancyDensityMul, mpInpBuoyancyDensityMul->GetValue());
	}
	else if(apInput==mpInpUseSurfaceEffects)
	{
		pAction = mpObject->CreateSetBoolAction(eBodyBool_UseSurfaceEffects, mpInpUseSurfaceEffects->GetValue());
	}
	else if(apInput==mpInpHasGravity)
	{
		pAction = mpObject->CreateSetBoolAction(eBodyBool_HasGravity, mpInpHasGravity->GetValue());
	}
	else
		return iEdScnObjEditPane::WindowSpecificInputCallback(apInput);


	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------
