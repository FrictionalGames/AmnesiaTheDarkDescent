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

#include "ParticleEditorWindowEmitters.h"

#include "../common/EditorInput.h"
#include "../common/EntityWrapper.h"
#include "EntityWrapperParticleEmitter.h"
#include "ParticleEditorWorld.h"
#include "ParticleEditorWindowEmitterParams.h"


//-------------------------------------------------------------

//////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////

//-------------------------------------------------------------

cParticleEditorWindowEmitters::cParticleEditorWindowEmitters(iEditorBase* apEditor) : iEditorWindow(apEditor, "Emitters window")
{
	mbEmittersUpdated = false;
}

//-------------------------------------------------------------

cParticleEditorWindowEmitters::~cParticleEditorWindowEmitters()
{
}

//-------------------------------------------------------------

//////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////

//-------------------------------------------------------------

void cParticleEditorWindowEmitters::SelectEntry(int alIdx)
{
	mpEnumEmitters->SetValue(alIdx);
}

//-------------------------------------------------------------

void cParticleEditorWindowEmitters::Reset()
{
	mpEnumEmitters->SetValue(-1);
	mpWEmitterParams->SetCurrentEmitter(NULL);
}

//-------------------------------------------------------------

//////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////

//-------------------------------------------------------------

void cParticleEditorWindowEmitters::OnInitLayout()
{
	SetSize(cVector2f(512,100));
	SetPosition(cVector3f(0,25,0.1f));

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpEnumEmitters = CreateInputEnum(vPos, _W("Emitters"), "InpEmitters", tWStringList());

	vPos.y += mpEnumEmitters->GetSize().y;

	mpBNewEmitter = mpSet->CreateWidgetButton(vPos, cVector2f(70,25), _W("Add"), mpBGFrame);
	mpBNewEmitter->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_OnPressed));

	mpBCopyEmitter = mpSet->CreateWidgetButton(vPos + cVector3f(80,0,0), cVector2f(70,25), _W("Copy"), mpBGFrame);
	mpBCopyEmitter->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_OnPressed));

	mpBDeleteEmitter = mpSet->CreateWidgetButton(vPos + cVector3f(160,0,0), cVector2f(70,25), _W("Delete"), mpBGFrame);
	mpBDeleteEmitter->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_OnPressed));

	mpWEmitterParams = hplNew(cParticleEditorWindowEmitterParams,(mpEditor));
	mpWEmitterParams->Init();
	mpWEmitterParams->SetActive(true);
	mpEditor->AddWindow(mpWEmitterParams);
}

//-------------------------------------------------------------

void cParticleEditorWindowEmitters::OnUpdate(float afTimeStep)
{
	cParticleEditorWorld* pWorld = (cParticleEditorWorld*)mpEditor->GetEditorWorld();
	if(pWorld->GetEmittersUpdated()==false) return;

	pWorld->SetEmittersUpdated(false);

	int lSelectedItem = mpEnumEmitters->GetValue();

	mvEmitters.clear();
	mpEnumEmitters->ClearValues();
	tEntityWrapperMap& mapEmitters = mpEditor->GetEditorWorld()->GetEntities();

	tEntityWrapperMapIt itEmitters = mapEmitters.begin();
	for(;itEmitters!=mapEmitters.end();++itEmitters)
	{
		iEntityWrapper* pEmitter = itEmitters->second;
		mpEnumEmitters->AddValue(cString::To16Char(pEmitter->GetName()));
		mvEmitters.push_back((cEntityWrapperParticleEmitter*)pEmitter);
	}

	mpEnumEmitters->SetValue(lSelectedItem);
}

//-------------------------------------------------------------

bool cParticleEditorWindowEmitters::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(apInput==mpEnumEmitters)
	{
		int lIdx = mpEnumEmitters->GetValue();
		cEntityWrapperParticleEmitter* pEmitter = NULL;
		if(lIdx>=0)pEmitter = mvEmitters[lIdx];
		mpWEmitterParams->SetCurrentEmitter(pEmitter);
		mpEditor->SetLayoutNeedsUpdate(true);
	}
	return true;
}

//-------------------------------------------------------------

bool cParticleEditorWindowEmitters::Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	cParticleEditorWorld* pWorld = (cParticleEditorWorld*)mpEditor->GetEditorWorld();
	iEntityWrapper* pEmitter = NULL;
	int lEnumItem = mpEnumEmitters->GetValue();

	if(apWidget==mpBNewEmitter)
	{
		lEnumItem = mpEnumEmitters->GetNumValues();
		pEmitter = pWorld->AddEmitter();
	}
	else if(apWidget==mpBCopyEmitter)
	{
		if(lEnumItem>=0)
		{
			iEntityWrapperData* pCopyData = mvEmitters[lEnumItem]->CreateCopyData();;
			
			pWorld->AddEmitter(pCopyData);
			lEnumItem = mpEnumEmitters->GetNumValues();
		}

	}
	else if(apWidget==mpBDeleteEmitter)
	{
		if(mpEnumEmitters->GetNumValues()==1)
			mpEditor->ShowMessageBox(_W("Warning!"), _W("Particle System must have at least one emitter"), _W("OK"),_W(""), NULL, NULL);
		else
		{
			if(lEnumItem>=0 && lEnumItem<(int)mvEmitters.size())
			{
				iEntityWrapper* pEmitter = mvEmitters[lEnumItem];

				pWorld->RemoveEmitter(pEmitter);
				if(lEnumItem!=0)
					--lEnumItem;
			}
		}
	}

	Update(0);
	mpEnumEmitters->SetValue(lEnumItem);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cParticleEditorWindowEmitters,Button_OnPressed);
