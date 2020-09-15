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

#include "EntityWrapperBillboard.h"

#include "EditorWorld.h"
#include "EditorBaseClasses.h"

#include "EditorWindowViewport.h"

#include "EditorWindowEntityEditBoxBillboard.h"

#include "EntityWrapperLight.h"

//------------------------------------------------------------------------------

cIconEntityBB::cIconEntityBB(iEntityWrapper* apParent) : iIconEntity(apParent, "BB")
{
}

cIconEntityBB::~cIconEntityBB()
{
	DestroyBB();
}

void cIconEntityBB::Update()
{
	cEntityWrapperBillboard* pParent = (cEntityWrapperBillboard*)mpParent;
	if(pParent->mbBBUpdated==false)
		return;

	if(pParent->mbTypeUpdated)
	{
		if(ReCreateBB())
		{
			pParent->mbTypeUpdated=false;
			pParent->mbMatUpdated = true;
		}
	}
	if(mpEntity)
	{
		cBillboard* pBB = (cBillboard*)mpEntity;
		pBB->SetSize(pParent->GetBillboardSize());
		pBB->SetForwardOffset(pParent->GetBillboardOffset());
		pBB->SetIsHalo(pParent->GetIsHalo());
		pBB->SetHaloSourceSize(pParent->GetHaloSourceSize());

		if(pParent->mbColorUpdated)
		{
			pParent->mbColorUpdated=false;
			pBB->SetColor(pParent->GetBillboardColor());
		}
		
		if(pParent->mbMatUpdated)
		{
			cMaterialManager* pManager = pParent->GetEditorWorld()->GetEditor()->GetEngine()->GetResources()->GetMaterialManager();
			cMaterial* pMat = pManager->CreateMaterial(pParent->GetMaterialFile());

			pBB->SetMaterial(pMat);
			pParent->mbMatUpdated=false;
		}

		pParent->mbBBUpdated=false;
	}
}

void cIconEntityBB::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, bool abIsSelected, bool abIsActive)
{
	iIconEntity::Draw(apViewport, apFunctions, abIsSelected, abIsActive);
	if(abIsSelected==false)
		return;

	//apFunctions->SetMatrix(NULL);
	//cBoundingVolume* pBV = GetRenderBV();
	//if(pBV)
	//	apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(), pBV->GetMax(), cColor(1));
}

bool cIconEntityBB::ReCreateBB()
{
	DestroyBB();
	cEntityWrapperBillboard* pBB = (cEntityWrapperBillboard*)mpParent;
	cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();
	mpEntity  = pWorld->CreateBillboard(pBB->GetName(), 1, pBB->GetBillboardTypeFromString(pBB->GetBillboardType()));

	return true;
}

void cIconEntityBB::DestroyBB()
{
	if(mpEntity)
	{
		cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();
		pWorld->DestroyBillboard((cBillboard*)mpEntity);
		mpEntity = NULL;
	}
}

//------------------------------------------------------------------------------

static tString gsBillboardTypeString[] = 
{
	"Point",
	"Axis",
	"FixedAxis",
};

//------------------------------------------------------------------------------

cEntityWrapperTypeBillboard::cEntityWrapperTypeBillboard() : iEntityWrapperType(eEditorEntityType_Billboard, _W("Billboard"), "Billboard")
{
	GetPropVec3f(eObjVec3f_Scale)->SetSaved(false);

	AddString(eBillboardStr_Type, "BillboardType", gsBillboardTypeString[0], ePropCopyStep_PreEnt);
	
	AddString(eBillboardStr_Material, "MaterialFile", "", ePropCopyStep_PreEnt);
	AddBool(eBillboardBool_IsHalo, "IsHalo", false, ePropCopyStep_PreEnt);
	AddVec2f(eBillboardVec2f_Size, "BillboardSize", 1, ePropCopyStep_PreEnt);
	AddVec3f(eBillboardVec3f_HaloSourceSize, "HaloSourceSize", 0.3f, ePropCopyStep_PreEnt);
	AddFloat(eBillboardFloat_Offset, "BillboardOffset", 0, ePropCopyStep_PreEnt);
	AddColor(eBillboardCol_Color, "BillboardColor", cColor(1,1), ePropCopyStep_PreEnt);

	AddString(eBillboardStr_ConnectedLight, "ConnectLight", "", ePropCopyStep_PostDeployAll);
}

iEntityWrapperData* cEntityWrapperTypeBillboard::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataBillboard,(this));
}

//------------------------------------------------------------------------------

cEntityWrapperDataBillboard::cEntityWrapperDataBillboard(iEntityWrapperType* apType) : iEntityWrapperData(apType)
{
}

//------------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataBillboard::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperBillboard,(this));
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEntityWrapperBillboard::cEntityWrapperBillboard(iEntityWrapperData* apData) : iEntityWrapper(apData)
{
	mbBBUpdated = true;
	mbMatUpdated = true;
	mbTypeUpdated = true;

	mpConnectedLight = NULL;
}

//------------------------------------------------------------------------------

cEntityWrapperBillboard::~cEntityWrapperBillboard()
{
	if(GetEditorWorld()->IsClearingEntities()==false)
		SetConnectedLightName("");
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

bool cEntityWrapperBillboard::GetProperty(int alPropID, cColor& aX)
{
	if(iEntityWrapper::GetProperty(alPropID, aX)==true)
		return true;

	switch(alPropID)
	{
	case eBillboardCol_Color:
		aX = GetBillboardColor();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperBillboard::GetProperty(int alPropID, bool& abX)
{
	if(iEntityWrapper::GetProperty(alPropID, abX)==true)
		return true;

	switch(alPropID)
	{
	case eBillboardBool_IsHalo:
		abX = GetIsHalo();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperBillboard::GetProperty(int alPropID, float& afX)
{
	if(iEntityWrapper::GetProperty(alPropID, afX)==true)
		return true;

	switch(alPropID)
	{
	case eBillboardFloat_Offset:
		afX = GetBillboardOffset();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperBillboard::GetProperty(int alPropID, tString& asX)
{
	if(iEntityWrapper::GetProperty(alPropID, asX)==true)
		return true;

	switch(alPropID)
	{
	case eBillboardStr_Type:
		asX = GetBillboardType();
		break;
	case eBillboardStr_Material:
		asX = GetMaterialFile();
		break;
	case eBillboardStr_ConnectedLight:
		asX = GetConnectedLightName();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperBillboard::GetProperty(int alPropID, cVector2f& avX)
{
	if(iEntityWrapper::GetProperty(alPropID, avX)==true)
		return true;

	switch(alPropID)
	{
	case eBillboardVec2f_Size:
		avX = GetBillboardSize();
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperBillboard::GetProperty(int alPropID, cVector3f& avX)
{
	if(iEntityWrapper::GetProperty(alPropID, avX)==true)
		return true;

	switch(alPropID)
	{
	case eBillboardVec3f_HaloSourceSize:
		avX = GetHaloSourceSize();
		break;
	default:
		return false;
	}

	return true;
}



bool cEntityWrapperBillboard::SetProperty(int alPropID, const cColor& aX)
{
	if(iEntityWrapper::SetProperty(alPropID, aX)==true)
		return true;

	switch(alPropID)
	{
	case eBillboardCol_Color:
		SetBillboardColor(aX);
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperBillboard::SetProperty(int alPropID, const bool& abX)
{
	if(iEntityWrapper::SetProperty(alPropID, abX)==true)
		return true;

	switch(alPropID)
	{
	case eBillboardBool_IsHalo:
		SetIsHalo(abX);
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperBillboard::SetProperty(int alPropID, const float& afX)
{
	if(iEntityWrapper::SetProperty(alPropID, afX)==true)
		return true;

	switch(alPropID)
	{
	case eBillboardFloat_Offset:
		SetBillboardOffset(afX);
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperBillboard::SetProperty(int alPropID, const tString& asX)
{
	if(iEntityWrapper::SetProperty(alPropID, asX)==true)
		return true;

	switch(alPropID)
	{
	case eBillboardStr_Type:
		SetBillboardType(asX);
		break;
	case eBillboardStr_Material:
		SetMaterialFile(asX);
		break;
	case eBillboardStr_ConnectedLight:
		SetConnectedLightName(asX);
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperBillboard::SetProperty(int alPropID, const cVector2f& avX)
{
	if(iEntityWrapper::SetProperty(alPropID, avX)==true)
		return true;

	switch(alPropID)
	{
	case eBillboardVec2f_Size:
		SetBillboardSize(avX);
		break;
	default:
		return false;
	}

	return true;
}

bool cEntityWrapperBillboard::SetProperty(int alPropID, const cVector3f& avX)
{
	if(iEntityWrapper::SetProperty(alPropID, avX)==true)
		return true;

	switch(alPropID)
	{
	case eBillboardVec3f_HaloSourceSize:
		SetHaloSourceSize(avX);
		break;
	default:
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

void cEntityWrapperBillboard::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol)
{
	iEntityWrapper::Draw(apViewport, apFunctions, apEditMode, abIsSelected, aHighlightCol);

    if(abIsSelected==false)
		return;

	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	apFunctions->SetMatrix(NULL);
	apFunctions->SetProgram(NULL);
	
	cBoundingVolume* pBV = mpEngineEntity->GetRenderBV();
	apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(), pBV->GetMax(), cColor(1,1));

	if(msType=="Axis" || msType=="FixedAxis")
		DrawArrow(apViewport, apFunctions, mmtxTransform, 1, true, cVector2f(0.05f, 0.4f), cColor(1,1));

	if(mbIsHalo)
	{
		cMatrixf mtxTransform = mmtxTransform.GetRotation();
		mtxTransform.SetTranslation(mvPosition);
		apFunctions->SetMatrix(&mtxTransform);
		cVector3f vHalfHaloSourceSize = mvHaloSourceSize*0.5f;
		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vHalfHaloSourceSize*-1, vHalfHaloSourceSize, cColor(0,1,0,1));
	}
}

//------------------------------------------------------------------------------

void cEntityWrapperBillboard::OnSetCulled(bool abX)
{
	iEntityWrapper::OnSetCulled(abX);

	//((cBillboard*)mpEngineEntity)->SetVisible(abX==false);
}

//------------------------------------------------------------------------------
/*
void cEntityWrapperBillboard::SaveToElement(cXmlElement* apElement)
{
	iEntityWrapper::SaveToElement(apElement);
	apElement->SetValue("Billboard");

	apElement->SetAttributeVector3f("Axis", cMath::MatrixMul(cMath::MatrixRotate(mvRotation, eEulerRotationOrder_XYZ), cVector3f(0,1,0)));
	apElement->SetAttributeVector2f("BillboardSize", mvBillboardSize);
	apElement->SetAttributeFloat("BillboardOffset", mfBillboardOffset);
	apElement->SetAttributeString("MaterialFile", mpEditorWorld->GetEditor()->GetFilePathRelativeToWorkingDir(msMatFile));
	apElement->SetAttributeString("BillboardType", msType);
	apElement->SetAttributeColor("BillboardColor", mCol);
	apElement->SetAttributeString("ConnectLight", msConnectedLightName);
	apElement->SetAttributeBool("IsHalo", mbIsHalo);
	apElement->SetAttributeVector3f("HaloSourceSize", mvHaloSourceSize);
}
*/
//------------------------------------------------------------------------------

cEditorWindowEntityEditBox* cEntityWrapperBillboard::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntityEditBox* pEditBox = hplNew(cEditorWindowEntityEditBoxBillboard,(apEditMode,this));

	return pEditBox;
}

//------------------------------------------------------------------------------

void cEntityWrapperBillboard::SetBillboardOffset(float afX)
{
	if(mfBillboardOffset==afX)
		return;

	mfBillboardOffset = afX;
	mbBBUpdated=true;
}

//------------------------------------------------------------------------------

void cEntityWrapperBillboard::SetMaterialFile(const tString& asMatFile)
{
	if(msMatFile==asMatFile)
		return;

	msMatFile = asMatFile;
	mbBBUpdated=true;
	mbMatUpdated=true;
}

//------------------------------------------------------------------------------

void cEntityWrapperBillboard::SetBillboardSize(const cVector2f& avSize)
{
	if(mvBillboardSize==avSize)
		return;

	mvBillboardSize = avSize;
	//mvScale = cVector3f(avSize.x, avSize.y, avSize.x);

	mbBBUpdated=true;
}


//------------------------------------------------------------------------------

void cEntityWrapperBillboard::SetBillboardType(const tString& asType)
{
	if(msType==asType)
		return;

	msType = asType;

	mbBBUpdated=true;
	mbTypeUpdated=true;
}

//------------------------------------------------------------------------------

void cEntityWrapperBillboard::SetBillboardColor(const cColor& aCol)
{
	if(mCol==aCol)
		return;

	mCol = aCol;
	mbBBUpdated=true;
	mbColorUpdated=true;
}

void cEntityWrapperBillboard::SetConnectedLightName(const tString& asLightName)
{
	if(msConnectedLightName==asLightName)
		return;

	iEntityWrapperLight* pLight = (iEntityWrapperLight*)GetEditorWorld()->GetEntityByName(msConnectedLightName);
	if(pLight)
		pLight->RemoveConnectedBillboard(this);
	else
		SetConnectedLight(NULL);

	msConnectedLightName = asLightName;
	pLight = (iEntityWrapperLight*)GetEditorWorld()->GetEntityByName(asLightName);
	if(pLight)
		pLight->AddConnectedBillboard(this);
	else
		mbColorUpdated=true;

	mbBBUpdated=true;
	mbConnectLightUpdated=true;
}

void cEntityWrapperBillboard::SetConnectedLight(iEntityWrapperLight* apLight)
{
	if(mpConnectedLight==apLight)
		return;
    
	mpConnectedLight = apLight;
	msConnectedLightName = apLight?apLight->GetName():"";
	mbBBUpdated=true;
	mbConnectLightUpdated=true;
}

//------------------------------------------------------------------------------

void cEntityWrapperBillboard::SetIsHalo(bool abX)
{
	if(mbIsHalo==abX)
		return;

	mbIsHalo = abX;

	mbBBUpdated=true;
}

//------------------------------------------------------------------------------

void cEntityWrapperBillboard::SetHaloSourceSize(const cVector3f& avX)
{
	if(mvHaloSourceSize==avX)
		return;

	mvHaloSourceSize = avX;

	mbBBUpdated=true;
}

//------------------------------------------------------------------------------

void cEntityWrapperBillboard::SetAbsScale(const cVector3f& avScale, int alAxis)
{
	//iEntityWrapper::SetAbsScale(avScale, alAxis);
	/*
	cVector3f vScale = avScale;
	if(alAxis==2)
		vScale.x = vScale.z;
	else if(alAxis==0)
		vScale.z = vScale.x;

	iEntityWrapper::SetAbsScale(vScale, alAxis);
*/
	
	if(alAxis==2)
		SetBillboardSize(cVector2f(avScale.z, mvBillboardSize.y));
	else if(alAxis==1)
		SetBillboardSize(cVector2f(mvBillboardSize.x, avScale.y));
	else if(alAxis==0)
		SetBillboardSize(cVector2f(avScale.x, mvBillboardSize.y));
	else
	{
		if(mlLastTransformedAxis==-1)
			SetBillboardSize(cVector2f(avScale.x, avScale.y));
		else
		{
			if(alAxis==2)
				SetBillboardSize(cVector2f(avScale.z, mvBillboardSize.y));
			else if(alAxis==1)
				SetBillboardSize(cVector2f(mvBillboardSize.x, avScale.y));
			else if(alAxis==0)
				SetBillboardSize(cVector2f(avScale.x, mvBillboardSize.y));
		}
	}
	

	mlLastTransformedAxis = alAxis;
}

//------------------------------------------------------------------------------

const cVector3f& cEntityWrapperBillboard::GetScale()
{
	mvBBScale =  cVector3f(mvBillboardSize.x, mvBillboardSize.y, mvBillboardSize.x);

	return mvBBScale;
}

void cEntityWrapperBillboard::SetWorldMatrix(const cMatrixf& amtxX)
{
	cMatrixf mtxInvMatrix = cMath::MatrixInverse(amtxX);
	cVector3f vScale = cVector3f(1.0f/mtxInvMatrix.GetRight().Length(),
									1.0f/mtxInvMatrix.GetUp().Length(),
									1.0f/mtxInvMatrix.GetForward().Length());

	cMatrixf mtxScale = cMath::MatrixScale(vScale);
	cMatrixf mtxRotate = cMath::MatrixMul(amtxX.GetRotation(), cMath::MatrixInverse(mtxScale));

	//Log("SetWorldMatrixCalled for %s\n", msName.c_str());

	SetAbsPosition(amtxX.GetTranslation());
	SetAbsRotation(cMath::MatrixToEulerAngles(mtxRotate, eEulerRotationOrder_XYZ));
	//SetAbsScale(vScale);
}

//------------------------------------------------------------------------------

void cEntityWrapperBillboard::UpdateEntity()
{
	iEntityWrapper::UpdateEntity();
	if(mpEngineEntity)
	{
		mpEngineEntity->Update();
		mpEngineEntity->SetMatrix(cMath::MatrixMul(mmtxTranslate, mmtxRotate));
		mpEngineEntity->UpdateVisibility();
	}
}

//void cEntityWrapperBillboard::UpdateEntity()
//{
	/*
	if(mbBBUpdated)
	{
		mbBBUpdated = false;

		if(mbTypeUpdated)
		{
		/*	if(mpEngineEntity)
			{
				if(mpConnectedLight)
					mpConnectedLight->RemoveConnectedBillboard(this);
				GetEditorWorld()->GetWorld()->DestroyBillboard((cBillboard*)mpEngineEntity);
			}
			//mpEngineEntity = CreateEngineEntity(GetEditorWorld(), mlID, msType);
			//if(mpEngineEntity)
			//{
				mbTypeUpdated = false;

				mbSizeUpdated=true;
				mbMatUpdated=true;
				mbColorUpdated=true;
				mbHaloUpdated=true;
				mbEntityUpdated=true;
				mbConnectLightUpdated=true;
			//}
		}
		if(mpEngineEntity)
		{
			cBillboard* pBB = (cBillboard*)mpEngineEntity;
			if(mbMatUpdated)
			{
				cMaterialManager* pManager = GetEditorWorld()->GetEditor()->GetEngine()->GetResources()->GetMaterialManager();
				cMaterial* pMat = pManager->CreateMaterial(msMatFile);

				if(pMat)
				{
					//pBB->SetMaterial(pMat);
					mbMatUpdated=false;
				}
			}
			if(mbOffsetUpdated)
			{
				//pBB->SetForwardOffset(mfBillboardOffset);
				mbOffsetUpdated=false;
			}
			if(mbColorUpdated)
			{
				//pBB->SetColor(mCol);
				mbColorUpdated=false;
			}
			if(mbSizeUpdated)
			{
				pBB->SetSize(mvBillboardSize);
				mbSizeUpdated=false;
			}
			if(mbHaloUpdated)
			{
				pBB->SetIsHalo(mbIsHalo);
				pBB->SetHaloSourceSize(mvHaloSourceSize);
				mbHaloUpdated=false;
			}
			if(mbConnectLightUpdated)
			{
				//cEntityWrapperLight* pLight = (cEntityWrapperLight*)GetEditorWorld()->GetEntityByName(msConnectedLightName);
				//if(pLight)
				//	pLight->AddConnectedBillboard(this);
				mbConnectLightUpdated=false;
			}
		}
	}
*/
//	iEntityWrapper::UpdateEntity();
//}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

eBillboardType cEntityWrapperBillboard::GetBillboardTypeFromString(const tString& asType)
{
	for(int i=0;i<3;++i)
	{
		if(asType==gsBillboardTypeString[i])
			return (eBillboardType)i;
	}

	return eBillboardType_Point;
}

//------------------------------------------------------------------------------

iEngineEntity* cEntityWrapperBillboard::CreateSpecificEngineEntity()
{
	return hplNew(cIconEntityBB,(this));
}

//------------------------------------------------------------------------------
