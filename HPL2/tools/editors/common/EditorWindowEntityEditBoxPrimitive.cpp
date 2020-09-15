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

#include "EditorWindowEntityEditBoxPrimitive.h"

#include "EntityWrapperPrimitive.h"
#include "EntityWrapperPrimitivePlane.h"

#include "EditorHelper.h"

#include "EditorEditModeSelect.h"

//#include "EditorActionsPrimitive.h"

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxPrimitive::cEditorWindowEntityEditBoxPrimitive(cEditorEditModeSelect* apEditMode, iEntityWrapperPrimitive* apObject) : cEditorWindowEntityEditBox(apEditMode,apObject)
{
	mpEntity = apObject;

	mpInpAlignToWorld = NULL;
}

//----------------------------------------------------------------------------

cEditorWindowEntityEditBoxPrimitive::~cEditorWindowEntityEditBoxPrimitive()
{
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxPrimitive::Create()
{
	mpTabGeneral = mpTabs->AddTab(_W("General"));
	////////////////////////////////////////
	// Properties in Tab 'General'
	AddPropertyName(mpTabGeneral);
	AddPropertyPosition(mpTabGeneral);
	AddPropertyRotation(mpTabGeneral);
	AddPropertyScale(mpTabGeneral);

	cVector3f vPos = cVector3f(10,10,0.1f);
	mpInpName->SetPosition(vPos);
	vPos.y += mpInpName->GetSize().y+5;
	mpInpPosition->SetPosition(vPos);
	vPos.y += mpInpPosition->GetSize().y+5;
	mpInpRotation->SetPosition(vPos);
	vPos.y += mpInpRotation->GetSize().y+5;
	mpInpScale->SetPosition(vPos);
	vPos.y += mpInpScale->GetSize().y+5;

	mpInpScale->SetLowerBound(true, 0);

	mpTabPrimitive = mpTabs->AddTab(_W("Primitive"));
    ////////////////////////////////////////
	// Properties in Tab 'Specific'
	AddPrimitivePropertySet(mpTabPrimitive);
	
	
	switch(mpEntity->GetPrimitiveType())
	{
	case eEditorPrimitiveType_Plane:
		mpTabPrimitiveType = mpTabs->AddTab(_W("Plane"));
		AddPrimitivePropertySetPlane(mpTabPrimitiveType);
		break;
	}
}

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxPrimitive::AddPrimitivePropertySet(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,30,0.1f);

	mpInpMaterial = CreateInputFile(vPos, _W("Material"), "", apParentTab);
	mpInpMaterial->SetBrowserType(eEditorResourceType_Material);
	//mpInpMaterial->SetInitialPath(mpEditor->GetWorkingDir());

	vPos.y += mpInpMaterial->GetSize().y+15;

	mpInpCollides = CreateInputBool(vPos, _W("Collides"), "", apParentTab);

	vPos.y += mpInpCollides->GetSize().y+10;

	mpInpCastShadows = CreateInputBool(vPos, _W("Cast Shadows"), "", apParentTab);
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxPrimitive::AddPrimitivePropertySetPlane(cWidgetTab* apParentTab)
{
	cVector3f vPos = cVector3f(10,30,0.1f);

	mpInpTileAmount = CreateInputVec3(vPos, _W("Tile amount"), "", apParentTab, 50, tWStringList(), eEditorInputLayoutStyle_RowLabelOnTop, 0.1f);
	vPos.y+= mpInpTileAmount->GetSize().y+10;
	mpInpTileOffset = CreateInputVec3(vPos, _W("Tile offset"), "", apParentTab, 50, tWStringList(), eEditorInputLayoutStyle_RowLabelOnTop, 0.1f);
	vPos.y+= mpInpTileOffset->GetSize().y+10;
	mpInpTextureAngle = CreateInputNumber(vPos, _W("Texture angle"), "", apParentTab, 50, 15);
	vPos.y+= mpInpTextureAngle->GetSize().y+10;

	mpInpAlignToWorld = CreateInputBool(vPos, _W("Align To World Coords"), "", apParentTab);

	/*

	mpLTileAmount = mpSet->CreateWidgetLabel(vPos, 0, _W("Tile amount"), apParentTab);
	AddWidget(mpLTileAmount);

	mpLOffset = mpSet->CreateWidgetLabel(vPos+cVector3f(0,50,0), 0, _W("Tile offset"), apParentTab);
	AddWidget(mpLOffset);

	for(int i=0;i<3;++i)
	{
		mpTBTileAmount[i] = mpSet->CreateWidgetTextBox(vPos+cVector3f(i*60.0f,20,0), cVector2f(55,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric, 0.1f);
		mpTBTileAmount[i]->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

		mpTBOffset[i] = mpSet->CreateWidgetTextBox(vPos+cVector3f(i*60.0f,70,0), cVector2f(55,25), _W(""), apParentTab, eWidgetTextBoxInputType_Numeric, 0.1f);
		mpTBOffset[i]->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(InputCallback));

		AddWidget(mpTBTileAmount[i]);
		AddWidget(mpTBOffset[i]);
	}

	vPos.y+=100;

	mpInpAlignToWorld = CreateInputBool(vPos, _W("Align To World Coords"), "", apParentTab);

	//mpCBAlignToWorld = mpSet->CreateWidgetCheckBox(vPos, 0, _W("Align to World Coords"), apParentTab);
	//AddWidget(mpCBAlignToWorld);
	//mpCBAlignToWorld->AddCallback(eGuiMessage_CheckChange, this, kGuiCallback(InputCallback));

	vPos.y += mpInpAlignToWorld->GetSize().y + 10;
	*/
}

//----------------------------------------------------------------------------

void cEditorWindowEntityEditBoxPrimitive::OnUpdate(float afTimeStep)
{
	cEditorWindowEntityEditBox::OnUpdate(afTimeStep);

	mpInpMaterial->SetValue(cString::To16Char(mpEntity->GetMaterial()),false);
	mpInpCollides->SetValue(mpEntity->GetCollides(), false);
	mpInpCastShadows->SetValue(mpEntity->GetCastShadows(), false);

	switch(mpEntity->GetPrimitiveType())
	{
	case eEditorPrimitiveType_Plane:
		{
			cEntityWrapperPrimitivePlane* pPlane = (cEntityWrapperPrimitivePlane*)mpEntity;
			mpInpTileAmount->SetValue(pPlane->GetTileAmount(), false);
			mpInpTileOffset->SetValue(pPlane->GetTileOffset(), false);
			mpInpTextureAngle->SetValue(cMath::ToDeg(pPlane->GetTextureAngle()), false);
			mpInpAlignToWorld->SetValue(pPlane->GetAlignToWorld(), false);

			break;
		}
	}
}

//----------------------------------------------------------------------------
/*
bool cEditorWindowEntityEditBoxPrimitive::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	iEditorAction* pAction = NULL;
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();
	int lID = mpEntity->GetID();
	tString sFilename = cString::To8Char(apWidget->GetText());
	bool bEmptyString = sFilename=="";

	if(apWidget==mpTBMaterial)
	{
		cMaterial* pMat;
		if(bEmptyString || 
			cEditorHelper::LoadResourceFile(eEditorResourceType_Material, sFilename, (void**)&pMat))
			pAction = mpEntity->CreateSetPropertyActionString(ePrimitiveStr_Material, sFilename);
			//pAction = hplNew(cEditorActionPrimitiveSetStringProperty,(pWorld, lID, ePrimitiveStringProperty_Material,sFilename));
		else
			apWidget->SetText(_W(""));
	}
	else if(apWidget==mpBMaterialBrowse)
	{
		mpEditor->ShowLoadFilePicker(mvLoadedFiles, msLastPath,this,kGuiCallback(FileMaterialCallback), _W("Materials"), tWStringList(1,_W("*.mat")));
	}
	else if(apWidget==mpTBTileAmount[0] || apWidget==mpTBTileAmount[1] || apWidget==mpTBTileAmount[2])
	{
		cVector3f vTemp = cVector3f(mpTBTileAmount[0]->GetNumericValue(), mpTBTileAmount[1]->GetNumericValue(), mpTBTileAmount[2]->GetNumericValue());
		pAction = hplNew(cEditorActionPrimitivePlaneSetVec3Property,(pWorld,lID, ePrimitivePlaneVec3Property_TileAmount,vTemp));
	}
	else if(apWidget==mpTBOffset[0] || apWidget==mpTBOffset[1] || apWidget==mpTBOffset[2])
	{
		cVector3f vTemp = cVector3f(mpTBOffset[0]->GetNumericValue(), mpTBOffset[1]->GetNumericValue(), mpTBOffset[2]->GetNumericValue());
		pAction = hplNew(cEditorActionPrimitivePlaneSetVec3Property,(pWorld,lID, ePrimitivePlaneVec3Property_TileOffset,vTemp));
	}
	else if(apWidget==mpCBAlignToWorld)
	{
		pAction = hplNew(cEditorActionPrimitivePlaneSetBoolProperty,(pWorld,lID, ePrimitivePlaneBoolProperty_AlignToWorld, mpCBAlignToWorld->IsChecked()));
	}
	

	mpEditor->AddAction(pAction);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxPrimitive,InputCallback);

//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxPrimitive::FileMaterialCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal==1)
	{
		msLastPath = cString::GetFilePathW(mvLoadedFiles.front());
		tWString sFilename = cString::GetFileNameW(mvLoadedFiles.front());

		mpTBMaterial->SetText(sFilename);
		InputCallback(mpTBMaterial, cGuiMessageData());
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowEntityEditBoxPrimitive, FileMaterialCallback);
*/
//----------------------------------------------------------------------------

bool cEditorWindowEntityEditBoxPrimitive::WindowSpecificInputCallback(iEditorInput* apInput)
{
	if(cEditorWindowEntityEditBox::WindowSpecificInputCallback(apInput))
		return true;

	iEditorAction* pAction = NULL;
	iEditorWorld* pWorld = mpEditor->GetEditorWorld();
	int lID = mpEntity->GetID();

	
	if(apInput==mpInpAlignToWorld)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(ePrimitivePlaneBool_AlignToWorld, mpInpAlignToWorld->GetValue());
		//pAction = hplNew(cEditorActionPrimitivePlaneSetBoolProperty,(pWorld,lID, ePrimitivePlaneBoolProperty_AlignToWorld, mpInpAlignToWorld->GetValue()));
	}
	else if(apInput==mpInpCastShadows)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(ePrimitiveBool_CastShadows, mpInpCastShadows->GetValue());
		//pAction = hplNew(cEditorActionPrimitivePlaneSetBoolProperty,(pWorld, lID, ePrimitivePlaneBoolProperty_CastShadows, mpInpCastShadows->GetValue()));
	}
	else if(apInput==mpInpCollides)
	{
		pAction = mpEntity->CreateSetPropertyActionBool(ePrimitiveBool_Collides, mpInpCollides->GetValue());
		//pAction = hplNew(cEditorActionPrimitivePlaneSetBoolProperty,(pWorld, lID, ePrimitivePlaneBoolProperty_Collides, mpInpCollides->GetValue()));
	}
	else if(apInput==mpInpMaterial)
	{
		tString sFile = cString::To8Char(mpInpMaterial->GetValue());
		if(mpInpMaterial->GetValue()==_W("") || 
			cEditorHelper::LoadResourceFile(eEditorResourceType_Material, sFile, NULL))
			pAction = mpEntity->CreateSetPropertyActionString(ePrimitiveStr_Material, sFile);
		//pAction = hplNew(cEditorActionPrimitiveSetStringProperty,(pWorld, lID, ePrimitiveStringProperty_Material, cString::To8Char(mpInpMaterial->GetValue())));
	}
	else if(apInput==mpInpTileAmount)
	{
		pAction = mpEntity->CreateSetPropertyActionVector3f(ePrimitivePlaneVec3f_TileAmount, mpInpTileAmount->GetValue());
	}
	else if(apInput==mpInpTileOffset)
	{
		pAction = mpEntity->CreateSetPropertyActionVector3f(ePrimitivePlaneVec3f_TileOffset, mpInpTileOffset->GetValue());
	}
	else if(apInput==mpInpTextureAngle)
	{
		pAction = mpEntity->CreateSetPropertyActionFloat(ePrimitivePlaneFloat_TextureAngle, cMath::ToRad(mpInpTextureAngle->GetValue()));
	}

	
	mpEditor->AddAction(pAction);

	return true;
}

//----------------------------------------------------------------------------
