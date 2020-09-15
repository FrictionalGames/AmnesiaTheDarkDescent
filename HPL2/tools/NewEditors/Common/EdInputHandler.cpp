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

#include "../Common/EdInputHandler.h"

#include "../Common/Editor.h"

//----------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//----------------------------------------------------------------

cEdInputHandler::cEdInputHandler(iEditor* apEditor) : iEdModule(apEditor, _W("InputHandler"))
{
}

//----------------------------------------------------------------


//----------------------------------------------------------------


//----------------------------------------------------------------


//----------------------------------------------------------------


//----------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////

//----------------------------------------------------------------

void cEdInputHandler::UpdateGuiInput()
{
	cGui* pGui = mpEditor->GetEngine()->GetGui();
	iKeyboard* pKB = mpInput->GetKeyboard();

	int lKeyModifier = pKB->GetModifier();

	if(mbMouseMoved)
		pGui->SendMousePos(mvMouseAbsPos, mvMouseRelPos);

	// Left button
	if(mpInput->BecameTriggerd("LftClick"))
		pGui->SendMouseClickDown(eGuiMouseButton_Left, lKeyModifier);
	if(mpInput->WasTriggerd("LftClick"))
		pGui->SendMouseClickUp(eGuiMouseButton_Left, lKeyModifier);
	if(mpInput->DoubleTriggerd("LftClick", 0.2f))
		pGui->SendMouseDoubleClick(eGuiMouseButton_Left, lKeyModifier);

	// Middle button
	if(mpInput->BecameTriggerd("MidClick"))
		pGui->SendMouseClickDown(eGuiMouseButton_Middle, lKeyModifier);
	if(mpInput->WasTriggerd("MidClick"))
		pGui->SendMouseClickUp(eGuiMouseButton_Middle, lKeyModifier);
	if(mpInput->DoubleTriggerd("MidClick", 0.2f))
		pGui->SendMouseDoubleClick(eGuiMouseButton_Middle, lKeyModifier);

	// Right button
	if(mpInput->BecameTriggerd("RgtClick"))
		pGui->SendMouseClickDown(eGuiMouseButton_Right, lKeyModifier);
	if(mpInput->WasTriggerd("RgtClick"))
		pGui->SendMouseClickUp(eGuiMouseButton_Right, lKeyModifier);
	if(mpInput->DoubleTriggerd("RgtClick", 0.2f))
		pGui->SendMouseDoubleClick(eGuiMouseButton_Right, lKeyModifier);
	
	// Wheel
	if(mpInput->BecameTriggerd("WheelUp"))
		pGui->SendMouseClickDown(eGuiMouseButton_WheelUp);
	if(mpInput->WasTriggerd("WheelUp"))
		pGui->SendMouseClickUp(eGuiMouseButton_WheelUp);

	if(mpInput->BecameTriggerd("WheelDn"))
		pGui->SendMouseClickDown(eGuiMouseButton_WheelDown);
	if(mpInput->WasTriggerd("WheelDn"))
		pGui->SendMouseClickUp(eGuiMouseButton_WheelDown);

	///////////////////////////////
	// GUI Key presses && releases
	bool bKeyProcessed = false;
	while(pKB->KeyIsPressed())
		bKeyProcessed = pGui->SendKeyPress(pKB->GetKey());
	while(pKB->KeyIsReleased())
		bKeyProcessed = pGui->SendKeyRelease(pKB->GetReleasedKey());
}

//----------------------------------------------------------------

void cEdInputHandler::OnInit()
{
	//////////////////
	// Enable input
	mpInput = mpEditor->GetEngine()->GetInput();

	mpInput->GetLowLevel()->LockInput(false);

	// Mouse buttons
	mpInput->CreateAction("LftClick")->AddMouseButton(eMouseButton_Left);
	mpInput->CreateAction("MidClick")->AddMouseButton(eMouseButton_Middle);
	mpInput->CreateAction("RgtClick")->AddMouseButton(eMouseButton_Right);
	mpInput->CreateAction("WheelUp")->AddMouseButton(eMouseButton_WheelUp);
	mpInput->CreateAction("WheelDn")->AddMouseButton(eMouseButton_WheelDown);

	// Special keys
	mpInput->CreateAction("LftAlt")->AddKey(eKey_LeftAlt);
	mpInput->CreateAction("LftShift")->AddKey(eKey_LeftShift);
	mpInput->CreateAction("LftControl")->AddKey(eKey_LeftCtrl);
}

//----------------------------------------------------------------

void cEdInputHandler::OnUpdate()
{
	//////////////////////////////////////////////////////////////
	// Update Input
	iKeyboard* pKB = mpInput->GetKeyboard();
	iMouse* pMouse = mpInput->GetMouse();

	///////////////////////////////
	// Update Mouse
	mvMouseAbsPos = pMouse->GetAbsPosition();
	mvMouseRelPos = pMouse->GetRelPosition();

	mbMouseMoved = (mvMouseRelPos!=0);

	UpdateGuiInput();
}

//----------------------------------------------------------------

void cEdInputHandler::OnLoadGlobalConfig(cConfigFile* apCfg)
{
}

//----------------------------------------------------------------

void cEdInputHandler::OnLoadLocalConfig(cConfigFile* apCfg)
{
}

//----------------------------------------------------------------
