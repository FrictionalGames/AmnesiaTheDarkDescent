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

#include "EditorWindowDecals.h"
#include "EditorEditModeDecals.h"

#include "EditorTypes.h"
#include "EditorInput.h"

//-------------------------------------------------------------------


//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEditorWindowDecals::cEditorWindowDecals(cEditorEditModeDecals* apEditMode) : iEditModeObjectCreatorWindow(apEditMode)
{
}

cEditorWindowDecals::~cEditorWindowDecals()
{
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

//-------------------------------------------------------------------

//-------------------------------------------------------------------

void cEditorWindowDecals::Reset()
{
	ResetInputs();
}

//-------------------------------------------------------------------

const tString& cEditorWindowDecals::GetMaterial()
{
	msMaterial = cString::To8Char(mpInpMaterial->GetValue());
	return msMaterial;
}

//-------------------------------------------------------------------

float cEditorWindowDecals::GetAngle()
{
	if(mpInpRandomizeAngle->GetValue())
		return cMath::ToRad(cMath::RandRectf(mpInpMinAngle->GetValue(), mpInpMaxAngle->GetValue()));
	else
		return cMath::ToRad(mpInpMinAngle->GetValue());
}

//-------------------------------------------------------------------

cVector3f cEditorWindowDecals::GetSize()
{
	cVector2f vSize2D;
	cVector2f vMinSize = mpInpMinSize->GetValue();
	cVector2f vMaxSize = mpInpMaxSize->GetValue();
	if(mpInpRandomizeSize->GetValue())
	{
		vSize2D = cMath::RandRectVector2f(vMinSize, vMaxSize);
		if(mpInpUniformSize->GetValue())
		{
			float fRatio = vMinSize.x/vMinSize.y;
			vSize2D.y = vSize2D.x*fRatio; 
		}
	}
	else
		vSize2D = vMinSize;

	float fDecalDepth = mpInpCustomDepthActive->GetValue()?mpInpCustomDepth->GetValue() : cMath::Max(vSize2D.x, vSize2D.y);

	return cVector3f(vSize2D.x, fDecalDepth, vSize2D.y);
}

//-------------------------------------------------------------------

cColor cEditorWindowDecals::GetColor()
{
	return mpInpColor->GetValue();
}

//-------------------------------------------------------------------

cVector2l cEditorWindowDecals::GetSubDivisions()
{
	cVector2l vSubDivs;
	int lValue = mpInpSubDivisions->GetValue();
	return cVector2l(lValue+1);
}

//-------------------------------------------------------------------

int cEditorWindowDecals::GetCurrentSubDiv()
{
	int lCurrentSubDiv = mpInpCurrentSubDiv->GetValue();
	int lSubDivs = mpInpSubDivisions->GetValue()+1;
	lSubDivs *= lSubDivs;
	if(lCurrentSubDiv==0)
	{
		lCurrentSubDiv = cMath::RandRectl(0, lSubDivs-1);
	}
	else
		--lCurrentSubDiv;

	return lCurrentSubDiv;
}

//-------------------------------------------------------------------

void cEditorWindowDecals::UpdateRotate()
{
	if(mbRotateUpdated==false && mpInpRandomizeAngle->GetValue()==false)
		return;

	mbRotateUpdated=false;

	float fDegRotate;
	if(mpInpRandomizeAngle->GetValue())
        fDegRotate = cMath::RandRectf(mpInpMinAngle->GetValue(), mpInpMaxAngle->GetValue());
	else
		fDegRotate = mpInpMinAngle->GetValue();

	mvRotate.x = 0;
	mvRotate.z = 0;
	mvRotate.y = cMath::ToRad(fDegRotate);
}

void cEditorWindowDecals::UpdateScale()
{
	if(mbScaleUpdated==false && mpInpRandomizeSize->GetValue()==false)
		return;

	cVector2f vSize2D;
	cVector2f vMinSize = mpInpMinSize->GetValue();
	cVector2f vMaxSize = mpInpMaxSize->GetValue();
	if(mpInpRandomizeSize->GetValue())
	{
		vSize2D = cMath::RandRectVector2f(vMinSize, vMaxSize);
		if(mpInpUniformSize->GetValue())
		{
			float fRatio = vMinSize.x/vMinSize.y;
			vSize2D.y = vSize2D.x*fRatio; 
		}
	}
	else
		vSize2D = vMinSize;

	float fDecalDepth = mpInpCustomDepthActive->GetValue()?mpInpCustomDepth->GetValue() : cMath::Max(vSize2D.x, vSize2D.y);

	mvScale = cVector3f(vSize2D.x, fDecalDepth, vSize2D.y);
}

//-------------------------------------------------------------------

eDecalAffectedType cEditorWindowDecals::TranslateTypeToAffectedType(eEditorEntityType aType)
{
	switch(aType)
	{
	case eEditorEntityType_StaticObject:
		return eDecalAffectedType_StaticObject;
	case eEditorEntityType_Entity:
		return eDecalAffectedType_Entity;
	case eEditorEntityType_Primitive:
		return eDecalAffectedType_Primitive;
	}

	return eDecalAffectedType_LastEnum;
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

//-------------------------------------------------------------------

void cEditorWindowDecals::OnInitLayout()
{
	//////////////////////
	// Set up layout
    mpBGFrame->SetSize(cVector2f(200,500));

	cVector3f vPos =cVector3f(5,5,0.1f);

	AddCreateOnSurfaceControls(true);
	mpGSurface->SetPosition(vPos);
	mpInpCreateOnSurface->SetValue(true, false);
	mpInpCreateOnSurface->GetHandle()->SetEnabled(false);

	vPos.y += mpGSurface->GetSize().y+10;

	mpInpMaterial = CreateInputFile(vPos, _W("Material"), "", mpBGFrame);
	mpInpMaterial->SetBrowserType(eEditorResourceType_Material);
	mpInpMaterial->SetInitialPath(mpEditor->GetMainLookUpDir(eDir_Decals));
	vPos.y += mpInpMaterial->GetSize().y+10;
	//mpInpMaxTriangles = CreateInputNumber(vPos, _W("Max triangles"), "", mpBGFrame, 50, 10);
	//mpInpMaxTriangles->SetDecimals(0);
	//vPos.y += mpInpMaxTriangles->GetSize().y+10;
	mpInpRandomizeAngle = CreateInputBool(vPos, _W("Randomize angle"), "", mpBGFrame);
	vPos.y += mpInpRandomizeAngle->GetSize().y+5;
	mpInpMinAngle = CreateInputNumber(vPos, _W("Min Angle"), "", mpBGFrame, 50, 15);
	mpInpMaxAngle = CreateInputNumber(vPos+cVector3f(mpInpMinAngle->GetSize().x+5,0,0), _W("Max Angle"), "", mpBGFrame, 50, 15);
	vPos.y += mpInpMinAngle->GetSize().y+10;

	mpInpRandomizeSize = CreateInputBool(vPos, _W("Randomize size"), "", mpBGFrame);
	vPos.x += mpInpRandomizeSize->GetSize().x+5;
	mpInpUniformSize = CreateInputBool(vPos, _W("Uniform"), "", mpBGFrame);
	vPos.x -= mpInpRandomizeSize->GetSize().x+5;
	vPos.y += mpInpRandomizeSize->GetSize().y+5;

	mpInpMinSize = CreateInputVec2(vPos, _W("Min Size"), "", mpBGFrame,40,tWStringList(), eEditorInputLayoutStyle_RowLabelOnTop, 0.5f);
	mpInpMinSize->SetLowerBound(true, 0);
	mpInpMaxSize = CreateInputVec2(vPos+cVector3f(mpInpMinSize->GetSize().x+5,0,0), _W("Max Size"), "", mpBGFrame,40,tWStringList(), eEditorInputLayoutStyle_RowLabelOnTop, 0.5f);
	mpInpMaxSize->SetLowerBound(true, 0);

	vPos.y += mpInpMinSize->GetSize().y+8;
	mpInpCustomDepthActive = CreateInputBool(vPos, _W("Use custom depth"), "", mpBGFrame);
	mpInpCustomDepth = CreateInputNumber(vPos+cVector3f(mpInpCustomDepthActive->GetSize().x+5,-3,0), _W(""), "", mpBGFrame, 50, 0.5f);
	mpInpCustomDepth->SetLayoutStyle(eEditorInputLayoutStyle_RowLabelOnLeft);
	mpInpCustomDepth->UpdateLayout();
	mpInpCustomDepth->SetLowerBound(true, 0);

	vPos.y += mpInpCustomDepth->GetSize().y + 10;

	mpInpColor = CreateInputColorFrame(vPos, _W("Color"), "", mpBGFrame);
	vPos.y += mpInpColor->GetSize().y + 10;
	mpInpSubDivisions = CreateInputEnum(vPos, _W("UV SubDivisions"), "", tWStringList(), mpBGFrame, 50);
	mpInpSubDivisions->AddValue(_W("1x1"));
	mpInpSubDivisions->AddValue(_W("2x2"));
	mpInpSubDivisions->AddValue(_W("3x3"));
	mpInpSubDivisions->AddValue(_W("4x4"));

	vPos.x += mpInpSubDivisions->GetSize().x + 10;

	mpInpCurrentSubDiv = CreateInputEnum(vPos, _W("Use SubDiv"), "", tWStringList(), mpBGFrame, 80);
	mpInpCurrentSubDiv->SetMaxShownItems(4);

	mpInpMaterial->SetValue(_W(""),false);
	//mpInpMaxTriangles->SetValue(300, false);
	mpInpMinAngle->SetValue(0,false);
	mpInpMaxAngle->SetValue(0,false);
	mpInpMinSize->SetValue(1,false);
	mpInpMaxSize->SetValue(1,false);
	mpInpCustomDepthActive->SetValue(false);
	mpInpCustomDepth->SetValue(1);
	mpInpRandomizeAngle->SetValue(false);
	mpInpRandomizeSize->SetValue(false);
	mpInpUniformSize->SetValue(true);

	mpInpColor->SetValue(1, false);
	mpInpSubDivisions->SetValue(0,true);
}

void cEditorWindowDecals::OnUpdate(float afTimeStep)
{
}

void cEditorWindowDecals::OnSetActive(bool abX)
{
	if(abX)
	{
		Reset();
	}
}

//-------------------------------------------------------------------

void cEditorWindowDecals::ResetInputs()
{
}


//-------------------------------------------------------------------

bool cEditorWindowDecals::InputCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowDecals, InputCallback);



//-------------------------------------------------------------------

bool cEditorWindowDecals::WindowSpecificInputCallback(iEditorInput* apInput)
{
	cEditorEditModeDecals* pEditMode = (cEditorEditModeDecals*)mpEditMode;

	if(apInput==mpInpRandomizeAngle)
	{
		bool bRandomizeEnabled = mpInpRandomizeAngle->GetValue();

		mpInpMaxAngle->GetHandle()->SetEnabled(bRandomizeEnabled);

		pEditMode->UpdateLocalRotation();
	}
	else if(apInput==mpInpRandomizeSize)
	{
		bool bRandomizeEnabled = mpInpRandomizeSize->GetValue();

		mpInpUniformSize->GetHandle()->SetEnabled(bRandomizeEnabled);
		mpInpMaxSize->GetHandle()->SetEnabled(bRandomizeEnabled);

		pEditMode->UpdateCreatorScale();
	}
	else if(apInput==mpInpCustomDepthActive)
	{
		mbScaleUpdated = true;
		mpInpCustomDepth->GetHandle()->SetEnabled(mpInpCustomDepthActive->GetValue());

		pEditMode->UpdateCreatorScale();
	}
	else if(apInput==mpInpMinSize)
	{
		mbScaleUpdated = true;
		cVector2f vMinSize = mpInpMinSize->GetValue();
		cVector2f vMaxSize = mpInpMaxSize->GetValue();
		if(vMinSize.x>vMaxSize.x)
		{
			if(vMinSize.y>vMaxSize.y)
				mpInpMaxSize->SetValue(vMinSize, false);
			else
				mpInpMaxSize->SetValue(cVector2f(vMinSize.x, vMaxSize.y), false);
		}
		else
		{
			if(vMinSize.y>vMaxSize.y)
				mpInpMaxSize->SetValue(cVector2f(vMaxSize.x, vMinSize.y), false);
		}

		pEditMode->UpdateCreatorScale();
	}
	else if(apInput==mpInpMaxSize)
	{
		mbScaleUpdated = true;

		cVector2f vMinSize = mpInpMinSize->GetValue();
		cVector2f vMaxSize = mpInpMaxSize->GetValue();
		if(vMinSize.x>vMaxSize.x)
		{
			if(vMinSize.y>vMaxSize.y)
				mpInpMinSize->SetValue(vMaxSize, false);
			else
				mpInpMinSize->SetValue(cVector2f(vMaxSize.x, vMinSize.y), false);
		}
		else
		{
			if(vMinSize.y>vMaxSize.y)
				mpInpMinSize->SetValue(cVector2f(vMinSize.x, vMaxSize.y), false);
		}

		pEditMode->UpdateCreatorScale();
	}
	else if(apInput==mpInpMinAngle)
	{
		mbRotateUpdated = true;
		float fMinAngle = mpInpMinAngle->GetValue();
		float fMaxAngle = mpInpMaxAngle->GetValue();
		if(fMinAngle>fMaxAngle)
			mpInpMaxAngle->SetValue(fMinAngle, false);

		pEditMode->UpdateLocalRotation();
	}
	else if(apInput==mpInpMaxAngle)
	{
		mbRotateUpdated = true;
		float fMinAngle = mpInpMinAngle->GetValue();
		float fMaxAngle = mpInpMaxAngle->GetValue();
		if(fMinAngle>fMaxAngle)
			mpInpMinAngle->SetValue(fMaxAngle, false);

		pEditMode->UpdateLocalRotation();
	}
	else if(apInput==mpInpSubDivisions)
	{
		int lValue = mpInpSubDivisions->GetValue()+1;
		int lSubDivAmount = lValue*lValue;
		mpInpCurrentSubDiv->ClearValues();
		for(int i=0;i<lSubDivAmount+1;++i)
		{
			tWString sValue;
			if(i==0)
				sValue = _W("Random");
			else
				sValue = cString::ToStringW(i-1);

			mpInpCurrentSubDiv->AddValue(sValue);
		}

		mpInpCurrentSubDiv->SetValue(0);
	}
	else if(apInput==mpInpCurrentSubDiv)
	{
		((cEditorEditModeDecals*)mpEditMode)->SetCurrentSubDiv(GetCurrentSubDiv());
	}

	mpEditMode->Reset();

	return true;
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------


