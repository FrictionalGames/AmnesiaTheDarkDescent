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

#include "EditorWindowEntityEditBoxBody.h"

#include "EntityWrapperBody.h"

#include "EditorEditModeSelect.h"

#include "EditorActionsBodies.h"
#include "EditorActionEntity.h"
#include "EditorHelper.h"

#include "EditorInput.h"

#include <algorithm>

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxBody::cEditorWindowEntityEditBoxBody(cEditorEditModeSelect* apEditMode, cEntityWrapperBody* apObject) : cEditorWindowEntityEditBox(apEditMode,apObject)
{
	mpEntity = apObject;
}

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxBody::~cEditorWindowEntityEditBoxBody()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxBody::Create()
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

void cEditorWindowEntityEditBoxBody::AddPropertySetBody(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,30,0.1f);
	cVector2f vSize = cVector2f(120, 25);

	float fAddNear = 18;
	float fAddFar = 25;

	mpLabelMaterial = mpSet->CreateWidgetLabel(vPos, 0, _W("Body Material"), apParentTab);

	vPos.y += fAddNear;
	mpComboBoxMaterial = mpSet->CreateWidgetComboBox(vPos, vSize, _W(""), apParentTab);
	cSurfaceDataIterator it = mpEditor->GetEngine()->GetPhysics()->GetSurfaceDataIterator();
	while(it.HasNext())
	{
        cSurfaceData* pSData = (cSurfaceData*)it.Next();
		tString sMaterialName = pSData->GetName();
		mpComboBoxMaterial->AddItem(sMaterialName);
	}

	mpComboBoxMaterial->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(InputCallback));


	vPos.y += fAddFar;

	mpLabelMass = mpSet->CreateWidgetLabel(vPos, 0, _W("Mass"), apParentTab);

	vPos.y += fAddNear;
	mpInputMass = mpSet->CreateWidgetTextBox(vPos, cVector2f(120,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric, 0.5f);
	mpInputMass->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

	
	vPos.y += fAddFar;
    
	mpLabelAngularDamping = mpSet->CreateWidgetLabel(vPos, 0, _W("Angular Damping"), apParentTab);

	vPos.y +=fAddNear;
	mpInputAngularDamping = mpSet->CreateWidgetTextBox(vPos, vSize, _W(""), apParentTab, eWidgetTextBoxInputType_Numeric, 0.5f);
	mpInputAngularDamping->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

	
	vPos.y += fAddFar;

	mpLabelLinearDamping = mpSet->CreateWidgetLabel(vPos, 0, _W("Linear Damping"), apParentTab);

	vPos.y +=fAddNear;
	mpInputLinearDamping = mpSet->CreateWidgetTextBox(vPos, vSize, _W(""), apParentTab, eWidgetTextBoxInputType_Numeric, 0.5f);
	mpInputLinearDamping->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));


	vPos.y += fAddFar;

	mpLabelMaxAngularSpeed = mpSet->CreateWidgetLabel(vPos, 0, _W("Max Angular Speed"), apParentTab);

	vPos.y +=fAddNear;
	mpInputMaxAngularSpeed = mpSet->CreateWidgetTextBox(vPos, vSize, _W(""), apParentTab, eWidgetTextBoxInputType_Numeric, 0.5f);
	mpInputMaxAngularSpeed->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));


	vPos.y += fAddFar;

	mpLabelMaxLinearSpeed = mpSet->CreateWidgetLabel(vPos, 0, _W("Max Linear Speed"), apParentTab);

	vPos.y +=fAddNear;
	mpInputMaxLinearSpeed = mpSet->CreateWidgetTextBox(vPos, vSize, _W(""), apParentTab, eWidgetTextBoxInputType_Numeric, 0.5f);
	mpInputMaxLinearSpeed->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));


	vPos.y += fAddFar;

	mpInpBuoyancyDensityMul = CreateInputNumber(vPos, _W("Buoyancy density mul"), "", apParentTab, 50, 0.1f);

	vPos.y += mpInpBuoyancyDensityMul->GetSize().y+10;


	mpCheckBoxBlocksSound = mpSet->CreateWidgetCheckBox(vPos, 0, _W("Blocks sound"), apParentTab);
	mpCheckBoxBlocksSound->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(InputCallback));


	vPos.y += fAddNear;

	mpCheckBoxContinuousCollision = mpSet->CreateWidgetCheckBox(vPos, 0, _W("Continuous collision"), apParentTab);
	mpCheckBoxContinuousCollision->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(InputCallback));


	vPos.y += fAddNear;

	mpCheckBoxCanAttachCharacter = mpSet->CreateWidgetCheckBox(vPos, 0, _W("Can attach character"), apParentTab);
	mpCheckBoxCanAttachCharacter->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(InputCallback));


	vPos.y += fAddNear;

	mpCheckBoxPushedByCharacterGravity = mpSet->CreateWidgetCheckBox(vPos, 0, _W("Pushed by character gravity"), apParentTab);
	mpCheckBoxPushedByCharacterGravity->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(InputCallback));


	vPos.y += fAddNear;

	mpCheckBoxCollideCharacter = mpSet->CreateWidgetCheckBox(vPos, 0, _W("Collide character"), apParentTab);
	mpCheckBoxCollideCharacter->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(InputCallback));


	vPos.y += fAddNear;

	mpCheckBoxCollideNonCharacter = mpSet->CreateWidgetCheckBox(vPos, 0, _W("Collide non character"), apParentTab);
	mpCheckBoxCollideNonCharacter->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(InputCallback));


	vPos.y += fAddNear;

	mpCheckBoxVolatile = mpSet->CreateWidgetCheckBox(vPos, 0, _W("Volatile"), apParentTab);
	mpCheckBoxVolatile->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(InputCallback));


	vPos.y += fAddNear;
	mpInpUseSurfaceEffects = CreateInputBool(vPos, _W("Use surface effects"), "", apParentTab);
	vPos.y += mpInpUseSurfaceEffects->GetSize().y + 10;
	mpInpHasGravity = CreateInputBool(vPos, _W("Has gravity"), "", apParentTab);
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxBody::OnUpdate(float afTimeStep)
{
	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);

	if(mpComboBoxMaterial)
	{
		int lMaterialIdx = -1;
		tString sMaterial = mpEntity->GetMaterial();
		tString sDefaultMaterial = "Default";
		for(int i=0;i<(int)mpComboBoxMaterial->GetItemNum();++i)
		{
			tString sComboBoxMaterial = cString::To8Char(mpComboBoxMaterial->GetItemText(i));
			if(sComboBoxMaterial==sMaterial ||
			   lMaterialIdx<0 && sComboBoxMaterial==sDefaultMaterial)
			{
				lMaterialIdx = i;
			}
		}
		if(lMaterialIdx<0) lMaterialIdx = 0;
		if(sMaterial=="") mpEntity->SetMaterial(cString::To8Char(mpComboBoxMaterial->GetItemText(lMaterialIdx)));

		mpComboBoxMaterial->SetSelectedItem(lMaterialIdx, false, false);
	}
	if(mpInputMass) mpInputMass->SetNumericValue(mpEntity->GetMass());
	if(mpInputAngularDamping) mpInputAngularDamping->SetNumericValue(mpEntity->GetAngularDamping());
	if(mpInputLinearDamping) mpInputLinearDamping->SetNumericValue(mpEntity->GetLinearDamping());
	if(mpInputMaxAngularSpeed) mpInputMaxAngularSpeed->SetNumericValue(mpEntity->GetMaxAngularSpeed());
	if(mpInputMaxLinearSpeed) mpInputMaxLinearSpeed->SetNumericValue(mpEntity->GetMaxLinearSpeed());
	mpInpBuoyancyDensityMul->SetValue(mpEntity->GetBuoyancyDensityMul(),false);

	if(mpCheckBoxBlocksSound) mpCheckBoxBlocksSound->SetChecked(mpEntity->BlocksSound(), false);
	if(mpCheckBoxContinuousCollision) mpCheckBoxContinuousCollision->SetChecked(mpEntity->HasContinuousCollision(), false);
	if(mpCheckBoxCanAttachCharacter) mpCheckBoxCanAttachCharacter->SetChecked(mpEntity->CanAttachCharacter(), false);
	if(mpCheckBoxPushedByCharacterGravity) mpCheckBoxPushedByCharacterGravity->SetChecked(mpEntity->IsPushedByCharacterGravity(), false);
	if(mpCheckBoxCollideCharacter) mpCheckBoxCollideCharacter->SetChecked(mpEntity->CollidesCharacter(), false);
	if(mpCheckBoxCollideNonCharacter) mpCheckBoxCollideNonCharacter->SetChecked(mpEntity->CollidesNonCharacter(), false);
	if(mpCheckBoxVolatile) mpCheckBoxVolatile->SetChecked(mpEntity->IsVolatile(), false);
	if(mpInpUseSurfaceEffects) mpInpUseSurfaceEffects->SetValue(mpEntity->UsesSurfaceEffects(), false);
	mpInpHasGravity->SetValue(mpEntity->HasGravity(), false);
}

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxBody::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEditorAction* pAction = NULL;

	int lID = mpEntity->GetID();
	iEditorWorld* pLevel = mpEditor->GetEditorWorld();

    ///////////////////////////////////////////
	// Material
	if(apWidget == mpComboBoxMaterial)
	{
		pAction = mpEntity->CreateSetPropertyActionString(eBodyStr_Material, cString::To8Char(mpComboBoxMaterial->GetText()));
	}
	///////////////////////////////////////////
	// Mass
	else if(apWidget == mpInputMass)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eBodyFloat_Mass, mpInputMass->GetNumericValue());
		

	}
	///////////////////////////////////////////
	// Angular Damping
	else if(apWidget == mpInputAngularDamping)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eBodyFloat_AngularDamping, mpInputAngularDamping->GetNumericValue());
	}
	///////////////////////////////////////////
	// Linear Damping
	else if(apWidget == mpInputLinearDamping)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eBodyFloat_LinearDamping, mpInputLinearDamping->GetNumericValue());
	}
	///////////////////////////////////////////
	// Max Angular Speed
	else if(apWidget == mpInputMaxAngularSpeed)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eBodyFloat_MaxAngularSpeed, mpInputMaxAngularSpeed->GetNumericValue());
	}
	///////////////////////////////////////////
	// Max Linear Speed
	else if(apWidget == mpInputMaxLinearSpeed) 
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eBodyFloat_MaxLinearSpeed, mpInputMaxLinearSpeed->GetNumericValue());
	}
	///////////////////////////////////////////
	// Block sound
	else if(apWidget == mpCheckBoxBlocksSound)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eBodyBool_BlockSound, mpCheckBoxBlocksSound->IsChecked());
	}
	///////////////////////////////////////////
	// Continuous collision
	else if(apWidget == mpCheckBoxContinuousCollision)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eBodyBool_ContinuousCollision, mpCheckBoxContinuousCollision->IsChecked());
	}
	///////////////////////////////////////////
	// Can attach character
	else if(apWidget == mpCheckBoxCanAttachCharacter)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eBodyBool_CanAttachCharacter, mpCheckBoxCanAttachCharacter->IsChecked());
	}
	///////////////////////////////////////////
	// Pushed by character gravity
	else if(apWidget == mpCheckBoxPushedByCharacterGravity)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eBodyBool_PushedByCharacterGravity, mpCheckBoxPushedByCharacterGravity->IsChecked());
	}
	///////////////////////////////////////////
	// Collide character
	else if(apWidget == mpCheckBoxCollideCharacter)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eBodyBool_CollideCharacter, mpCheckBoxCollideCharacter->IsChecked());
	}
	///////////////////////////////////////////
	// Collide non character
	else if(apWidget == mpCheckBoxCollideNonCharacter)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eBodyBool_CollideNonCharacter, mpCheckBoxCollideNonCharacter->IsChecked());
	}
	///////////////////////////////////////////
	// Volatile
	else if(apWidget == mpCheckBoxVolatile)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eBodyBool_Volatile, mpCheckBoxVolatile->IsChecked());
	}

	///////////////////////////////////////////
	// Execute action
	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxBody,InputCallback);


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxBody::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(cEditorWindowEntityEditBox::WindowSpecificInputCallback(apInput)==true)
		return true;

	iEditorAction* pAction = NULL;
	int lID = mpEntity->GetID();
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();

	if(apInput==mpInpBuoyancyDensityMul)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(eBodyFloat_BuoyancyDensityMul, mpInpBuoyancyDensityMul->GetValue());
	}
	else if(apInput==mpInpUseSurfaceEffects)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eBodyBool_UseSurfaceEffects, mpInpUseSurfaceEffects->GetValue());
	}
	else if(apInput==mpInpHasGravity)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(eBodyBool_HasGravity, mpInpHasGravity->GetValue());
	}

	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------
