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

#include "../ParticleEditor/EdWindowEmitters.h"

#include "../ParticleEditor/EdEditModeParticleEmitters.h"
#include "../ParticleEditor/EdObjParticleEmitter.h"

#include "../Common/Editor.h"
#include "../Common/EdWorld.h"

//-------------------------------------------------------------

//////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////

//-------------------------------------------------------------

cEdWindowEmitters::cEdWindowEmitters(iEdEditMode* apMode) : iEdEditModePane(apMode)
{
	mbEmittersUpdated = false;
}

//-------------------------------------------------------------

cEdWindowEmitters::~cEdWindowEmitters()
{
}

//-------------------------------------------------------------

//////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////

//-------------------------------------------------------------

void cEdWindowEmitters::SelectEntry(int alIdx)
{
	mpInpEmitters->SetValue(alIdx);
}

//-------------------------------------------------------------

void cEdWindowEmitters::OnReset()
{
	iEdPane::OnReset();
}

//-------------------------------------------------------------

void cEdWindowEmitters::OnWorldLoad()
{
	SetUpdated();
}

//-------------------------------------------------------------

void cEdWindowEmitters::OnWorldObjModify()
{
	for(int i=0; i<mpInpEmitters->GetNumValues(); ++i)
	{
		const tWString& sName = mpInpEmitters->GetEnumValue(i);
		iEdObject* pEmitter = static_cast<iEdObject*>(mpInpEmitters->GetUserData(i));

		if(pEmitter->GetName()!=sName)
		{
			SetUpdated();
			break;
		}
	}
}

//-------------------------------------------------------------

void cEdWindowEmitters::OnWorldAddRemObject()
{
	OnWorldLoad();
}

//-------------------------------------------------------------

//////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////

//-------------------------------------------------------------

void cEdWindowEmitters::OnCreateLayout()
{
	iEdPane::OnCreateLayout();

	SetSize(cVector2f(512,100));
	SetPosition(cVector3f(0,0,0.1f));

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpEmitters = CreateInputEnum(vPos, _W("Emitters"), GetBG(), 230);

	vPos.y += mpInpEmitters->GetSize().y + 10;

	mpBNewEmitter = mpSet->CreateWidgetButton(vPos, cVector2f(70,25), _W("Add"), GetBG());
	mpBNewEmitter->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(New_OnPressed));

	mpBCopyEmitter = mpSet->CreateWidgetButton(vPos + cVector3f(80,0,0), cVector2f(70,25), _W("Copy"), GetBG());
	mpBCopyEmitter->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Copy_OnPressed));

	mpBDeleteEmitter = mpSet->CreateWidgetButton(vPos + cVector3f(160,0,0), cVector2f(70,25), _W("Delete"), GetBG());
	mpBDeleteEmitter->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Delete_OnPressed));
}

//-------------------------------------------------------------

void cEdWindowEmitters::OnUpdate()
{
	iEdEditModePane::OnUpdate();

	iEdWorld* pWorld = mpEditor->GetWorld();
	cEdEditModeParticleEmitters* pMode = static_cast<cEdEditModeParticleEmitters*>(mpEditMode);

	mpInpEmitters->ClearValues();
	const tEdObjectMap& mapEmitters = pWorld->GetObjects();
	tEdObjectMap::const_iterator it = mapEmitters.begin();
	iEdObject* pSelectedEmitter = pMode->GetSelectedEmitter();
	for(; it!=mapEmitters.end(); ++it)
	{
		iEdObject* pEmitter = it->second;
		int lIndex = mpInpEmitters->AddValue(pEmitter->GetName(), pEmitter);

		if(pEmitter==pSelectedEmitter)
			mpInpEmitters->SetValue(lIndex, false);
	}

	mpBDeleteEmitter->SetEnabled(mapEmitters.size()!=1);
}

//-------------------------------------------------------------

bool cEdWindowEmitters::WindowSpecificInputCallback(iEdInput* apInput)
{
	if(apInput==mpInpEmitters)
	{
		cEdEditModeParticleEmitters* pMode = static_cast<cEdEditModeParticleEmitters*>(mpEditMode);
		iEdObject* pEmitter = static_cast<iEdObject*>(mpInpEmitters->GetSelectedUserData());

		pMode->SetSelectedEmitter(pEmitter);
	}
	return true;
}

//-------------------------------------------------------------

bool cEdWindowEmitters::New_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEdWorld* pWorld = mpEditor->GetWorld();
	cEdEditModeParticleEmitters* pMode = static_cast<cEdEditModeParticleEmitters*>(mpEditMode);

	iEdObjectData* pData = pMode->CreateObjectData();
	iEdAction* pAction = pWorld->CreateActionCreateObject(pData);

	mpEditor->AddAction(pAction);
	SetUpdated();

	iEdObject* pNewEmitter = pWorld->GetLastAddedObject();
	pMode->SetSelectedEmitter(pNewEmitter);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowEmitters,New_OnPressed);

//-------------------------------------------------------------

bool cEdWindowEmitters::Copy_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEdWorld* pWorld = mpEditor->GetWorld();
	cEdEditModeParticleEmitters* pMode = static_cast<cEdEditModeParticleEmitters*>(mpEditMode);

	cEdObjParticleEmitter* pEmitter = static_cast<cEdObjParticleEmitter*>(mpInpEmitters->GetSelectedUserData());

	iEdAction* pAction = pWorld->CreateActionCloneObject(pEmitter);
	
	mpEditor->AddAction(pAction);
	SetUpdated();

	iEdObject* pNewEmitter = pWorld->GetLastAddedObject();
	pMode->SetSelectedEmitter(pNewEmitter);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowEmitters,Copy_OnPressed);

//-------------------------------------------------------------

bool cEdWindowEmitters::Delete_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEdWorld* pWorld = mpEditor->GetWorld();
	cEdEditModeParticleEmitters* pMode = static_cast<cEdEditModeParticleEmitters*>(mpEditMode);

	int lIdx = mpInpEmitters->GetValue();
	if(lIdx>0) lIdx--;
	else lIdx++;

	cEdObjParticleEmitter* pEmitter = static_cast<cEdObjParticleEmitter*>(mpInpEmitters->GetSelectedUserData());
	iEdAction* pAction = pWorld->CreateActionDeleteObject(pEmitter);
	
	mpEditor->AddAction(pAction);
	SetUpdated();

	cEdObjParticleEmitter* pPrevEmitter = static_cast<cEdObjParticleEmitter*>(mpInpEmitters->GetUserData(lIdx));
	pMode->SetSelectedEmitter(pPrevEmitter);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowEmitters,Delete_OnPressed);
