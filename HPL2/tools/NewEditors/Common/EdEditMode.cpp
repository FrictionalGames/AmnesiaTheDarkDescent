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

#include "../Common/EdEditMode.h"

#include "../Common/Editor.h"
#include "../Common/EdWindowViewport.h"
#include "../Common/EdScnObject.h"

#include "../Common/EdObjectActions.h"

//--------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// EDIT MODE BASE - CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

iEdEditMode::iEdEditMode(iEditor* apEditor, const tWString& asName) : iEdModule(apEditor, _W("EditMode: ") + asName)
{
	mpWindow = NULL;
	mpIcon = NULL;
}

iEdEditMode::~iEdEditMode()
{
	if(mpIcon)
		mpEditor->GetGuiSet()->GetGui()->DestroyGfx(mpIcon);
}

//--------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// EDIT MODE BASE - PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

iEdWorld* iEdEditMode::GetWorld()
{
	return mpEditor->GetWorld();
}

//--------------------------------------------------------------------------------

void iEdEditMode::SetActive(bool abX)
{
	if(mpWindow) mpWindow->SetActive(abX);

	OnSetActive(abX);
}

bool iEdEditMode::IsActive()
{
	return mpEditor->GetActiveEditMode()==this;
}

//--------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// EDIT MODE BASE - PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

cGuiGfxElement* iEdEditMode::CreateIcon()
{
	tString sFilename = cString::To8Char(GetName());
	sFilename = cString::ReplaceCharTo(sFilename, ":", "_");
	sFilename = cString::ReplaceCharTo(sFilename, " ", "");

	return mpEditor->GetGuiSet()->GetGui()->CreateGfxImage(sFilename + ".tga", eGuiMaterial_Alpha);
}

//--------------------------------------------------------------------------------

void iEdEditMode::OnCreateLayout()
{
	mpIcon = CreateIcon();

	mpWindow = CreateWindow();
	if(mpWindow)
		mpEditor->GetWindowHandler()->AddWindow(mpWindow, eLayoutBlock_EditMode, false);
}

//--------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// OBJECT CREATOR BASE - CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

iEdEditModeObjectCreator::iEdEditModeObjectCreator(iEditor* apEditor, 
												   const tWString& asName) : iEdEditMode(apEditor, asName)
{
	mbPreCreationActive = false;
	mpType = NULL;

	mlSubType = -1;
}

iEdEditModeObjectCreator::~iEdEditModeObjectCreator()
{
}

//--------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// OBJECT CREATOR BASE - PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------

iEdObjectType* iEdEditModeObjectCreator::GetMainType()
{
	return mpType;
}

void iEdEditModeObjectCreator::SetSelectedType(int alIdx)
{
	if(mlSubType==alIdx)
		return;

	mlSubType = alIdx;

	if(mlSubType<-1) mlSubType = -1;
	if(mlSubType>=mpType->GetNumChildren()) mlSubType=mpType->GetNumChildren();

	OnSetSubType(alIdx);
}

iEdObjectType* iEdEditModeObjectCreator::GetSelectedType()
{
	if(mlSubType==-1 || mpType->GetNumChildren()==0)
		return mpType;

	return mpType->GetChild(mlSubType);
}

//--------------------------------------------------------------------------------

void iEdEditModeObjectCreator::SetBaseName(const tWString& asX)
{
	msBaseName = asX;
}

//--------------------------------------------------------------------------------

bool iEdEditModeObjectCreator::IsBusy()
{
	return mbPreCreationActive;
}


//--------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// OBJECT CREATOR BASE - PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------


//---------------------------------------------------------------------

void iEdEditModeObjectCreator::OnViewportMouseDown(const cViewportClick& aData)
{
	if(aData.mbLeft==false)
		return;

	//mpEditor->SetFlags(eEdFlag_Placing, true);
	mbPreCreationActive = true;
}

//-----------------------------------------------------------------

void iEdEditModeObjectCreator::OnViewportMouseUp(const cViewportClick& aData)
{
	//if(mpEditor->GetFlags(eEdFlag_Placing)==false ||
	if(aData.mbLeft==false)
		return;

	iEdObjectData* pCreationData = CreateObjectData();

	iEdAction* pAction = mpEditor->GetWorld()->CreateActionCreateObject(pCreationData);
	mpEditor->AddAction(pAction);

	//mpEditor->SetFlags(eEdFlag_Placing, false);

	mbPreCreationActive = false;
}

//-----------------------------------------------------------------

iEdObjectData* iEdEditModeObjectCreator::CreateObjectData()
{
	iEdObjectType* pType = GetSelectedType();
	if(pType==NULL)	return NULL;
	
	iEdWorld* pWorld = pType->GetWorld();

	iEdObjectData* pCreationData = pType->CreateData();
	if(pCreationData && SetUpCreationData(pCreationData))
	{
		tWString sName = msBaseName;
		if(sName.empty())
			sName = pCreationData->GetName();

		sName = pWorld->GenerateValidName(sName, true);
		pCreationData->SetName(sName);
	}
	else
	{
		if(pCreationData)
			hplDelete(pCreationData);

		pCreationData = NULL;
	}

	return pCreationData;
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------

/*
void iEdEditModeObjectCreator::OnWorldPostTransDraw(const cModuleDrawData& aData)
{
	cRendererCallbackFunctions* apFunctions = aData.mpFunctions;
	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetProgram(NULL);
	
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	cMatrixf mtxTransform = cMath::MatrixMul(cMath::MatrixTranslate(GetCreatorPosition()),
			cMath::MatrixMul( cMath::MatrixRotate(GetCreatorRotation(), eEulerRotationOrder_XYZ),
			cMath::MatrixScale(GetCreatorScale())));

	cColor col = mbPreCreationActive?cColor(0,1,0,1):cColor(1,0,0,1);
	apFunctions->GetLowLevelGfx()->DrawSphere(mtxTransform.GetTranslation(),0.1f,col);

	DrawObjectPreview(aData.mpViewport, aData.mpFunctions, mtxTransform, mbPreCreationActive);

	apFunctions->SetBlendMode(eMaterialBlendMode_None);
}
*/

//-----------------------------------------------------------------

void iEdEditModeObjectCreator::OnInit()
{
	mpType = CreateType();
	if(mpEditor->GetWorld()->AddType(mpType)==false)
	{
		FatalError("%ls with NULL type!!", GetName().c_str());
	}
}

//-----------------------------------------------------------------

void iEdEditModeObjectCreator::OnSetActive(bool abX)
{
}

//-----------------------------------------------------------------


//-----------------------------------------------------------------

/*
bool iEdEditModeObjectCreator::SetUpCreationData(iEdObjectData* apData)
{
	//apData->SetVec3f(eScnVec3f_Translation, GetCreatorPosition());
	//apData->SetVec3f(eScnVec3f_Rotation, mvCreatorWorldRotation);
	//apData->SetVec3f(eScnVec3f_Scale, mvCreatorScale);

	return GetSelectedType()->SetUpCreationData(apData);
}
*/


//-----------------------------------------------------------------
