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

#include "EditorWindowJoints.h"
#include "EditorEditModeJoints.h"

#include "EditorGrid.h"
#include "EntityWrapper.h"

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEditorWindowJoints::cEditorWindowJoints(cEditorEditModeJoints* apEditMode) : iEditModeObjectCreatorWindow(apEditMode)
{
}

cEditorWindowJoints::~cEditorWindowJoints()
{
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

//-------------------------------------------------------------------

void cEditorWindowJoints::Reset()
{
	ResetInputs();
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void cEditorWindowJoints::OnInitLayout()
{
	//////////////////////
	// Set up layout
    mpBGFrame->SetSize(cVector2f(200,600));

	cVector3f vPos = cVector3f(5,5,1);
	cVector2f vSize;

	///////////////////////////////////////////////////
	// Joint Selection
	/*
	mpJointSelection = mpSet->CreateWidgetComboBox(vPos,cVector2f(190,13),_W(""), mpBGFrame);
	for(int i=0; i< eEditorBodyShape_LastEnum; ++i)
	{
		mpJointSelection->AddItem(gsEditorJointName[i]);
	}
	mpJointSelection->SetSelectedItem(0);
	*/

	cEditorEditModeJoints* pEditMode = (cEditorEditModeJoints*)mpEditMode;
	mpInpJointType = CreateInputEnum(vPos, _W("Joint Type"), "", tWStringList(), mpBGFrame);
	for(int i=0;i<pEditMode->GetTypeNum();++i)
	{
		iEntityWrapperType* pType = pEditMode->GetType(i);
		mpInpJointType->AddValue(cString::To16Char(pType->GetXmlElementName()));
	}
	mpInpJointType->SetValue(0);
}

//-------------------------------------------------------------------

void cEditorWindowJoints::OnSetActive(bool abX)
{
	Reset();
}

//-------------------------------------------------------------------

void cEditorWindowJoints::ResetInputs()
{
	
}

//-------------------------------------------------------------------

bool cEditorWindowJoints::WindowSpecificInputCallback(iEditorInput* apInput)
{
	cEditorEditModeJoints* pEditMode = (cEditorEditModeJoints*)mpEditMode;
	if(apInput==mpInpJointType)
		pEditMode->SetSubType(mpInpJointType->GetValue());

	return true;
}

