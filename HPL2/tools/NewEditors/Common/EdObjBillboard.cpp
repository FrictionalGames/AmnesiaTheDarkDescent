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

#include "../Common/EdObjBillboard.h"

#include "../Common/EdWorld.h"
#include "../Common/Editor.h"

#include "../Common/EdWindowViewport.h"

#include "../Common/EdResource.h"

//#include "EditorWindowEntityEditBoxBillboard.h"

#include "../Common/EdObjLight.h"

//------------------------------------------------------------------------------

cIconEntityBB::cIconEntityBB(iEdScnObject* apParent) : iIconObject(apParent, "BB")
{
	mbSubs = false;
}

cIconEntityBB::~cIconEntityBB()
{
	DestroyBB();
}

void cIconEntityBB::Update()
{
	cEdObjBillboard* pParent = (cEdObjBillboard*)mpParent;
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
	if(mpObject)
	{
		cBillboard* pBB = (cBillboard*)mpObject;
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
			cMaterial* pMat = NULL;
			cEdLoader::LoadMaterial(pParent->GetMaterialFile(), &pMat);
			
			pBB->SetMaterial(pMat);
			pParent->mbMatUpdated=false;
		}

		pParent->mbBBUpdated=false;
	}
}

void cIconEntityBB::Draw(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions, bool abIsSelected, bool abIsActive)
{
	iIconObject::Draw(apViewport, apFunctions, abIsSelected, abIsActive);
	if(abIsSelected==false)
		return;

	apFunctions->SetMatrix(NULL);
	cBoundingVolume* pBV = GetRenderBV();
	if(pBV)
		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(), pBV->GetMax(), cColor(1));
}

bool cIconEntityBB::ReCreateBB()
{
	DestroyBB();
	cEdObjBillboard* pBB = (cEdObjBillboard*)mpParent;
	cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();
	cBillboard* pEngBB = pWorld->CreateBillboard(cString::To8Char(pBB->GetName()), 1, pBB->GetBillboardTypeFromString(pBB->GetBillboardType()));
	pEngBB->SetRenderableUserData(mpParent);
    mpObject = pEngBB;

	return true;
}

void cIconEntityBB::DestroyBB()
{
	if(mpObject)
	{
		cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();
		pWorld->DestroyBillboard((cBillboard*)mpObject);
		mpObject = NULL;
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

cTypeBillboard::cTypeBillboard() : iEdScnObjType(_W("Billboard"), "Billboard")
{
	//GetPropVec3f(eScnVec3f_Scale)->SetSaved(false);

	AddString(eBillboardStr_Type, "BillboardType", gsBillboardTypeString[0], ePropStep_PreCreate);
	
	AddString(eBillboardStr_Material, "MaterialFile", "", ePropStep_PreCreate);
	AddBool(eBillboardBool_IsHalo, "IsHalo", false, ePropStep_PreCreate);
	AddVec2f(eBillboardVec2f_Size, "BillboardSize", 1, ePropStep_PreCreate);
	AddVec3f(eBillboardVec3f_HaloSourceSize, "HaloSourceSize", 0.3f, ePropStep_PreCreate);
	AddFloat(eBillboardFloat_Offset, "BillboardOffset", 0, ePropStep_PreCreate);
	AddColor(eBillboardCol_Color, "BillboardColor", cColor(1,1), ePropStep_PreCreate);

	AddString(eBillboardStr_ConnectedLight, "ConnectLight", "", ePropStep_PostDeployAll);

	mbScalable = false;
}

iEdObjectData* cTypeBillboard::CreateTypeSpecificData()
{
	return hplNew(cEdObjBillboardData,(this));
}

//------------------------------------------------------------------------------

cEdObjBillboardData::cEdObjBillboardData(iEdObjectType* apType) : iEdScnObjData(apType)
{
}

//------------------------------------------------------------------------------

iEdObject* cEdObjBillboardData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjBillboard,(this));
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

cEdObjBillboard::cEdObjBillboard(iEdObjectData* apData) : iEdScnObject(apData)
{
	mbBBUpdated = true;
	mbMatUpdated = true;
	mbTypeUpdated = true;

	mpConnectedLight = NULL;
}

//------------------------------------------------------------------------------

cEdObjBillboard::~cEdObjBillboard()
{
	if(GetWorld()->IsClearing()==false)
		SetConnectedLightName("");
}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

bool cEdObjBillboard::GetProperty(int alPropID, cColor& aX)
{
	switch(alPropID)
	{
	case eBillboardCol_Color:
		aX = GetBillboardColor();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, aX);
	}

	return true;
}

bool cEdObjBillboard::GetProperty(int alPropID, bool& abX)
{
	switch(alPropID)
	{
	case eBillboardBool_IsHalo:
		abX = GetIsHalo();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, abX);
	}

	return true;
}

bool cEdObjBillboard::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eBillboardFloat_Offset:
		afX = GetBillboardOffset();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjBillboard::GetProperty(int alPropID, tString& asX)
{
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
		return iEdScnObject::GetProperty(alPropID, asX);
	}

	return true;
}

bool cEdObjBillboard::GetProperty(int alPropID, cVector2f& avX)
{
	switch(alPropID)
	{
	case eBillboardVec2f_Size:
		avX = GetBillboardSize();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, avX);
	}

	return true;
}

bool cEdObjBillboard::GetProperty(int alPropID, cVector3f& avX)
{
	switch(alPropID)
	{
	case eBillboardVec3f_HaloSourceSize:
		avX = GetHaloSourceSize();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, avX);
	}

	return true;
}



bool cEdObjBillboard::SetProperty(int alPropID, const cColor& aX)
{
	switch(alPropID)
	{
	case eBillboardCol_Color:
		SetBillboardColor(aX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, aX);
	}

	return true;
}

bool cEdObjBillboard::SetProperty(int alPropID, const bool abX)
{
	switch(alPropID)
	{
	case eBillboardBool_IsHalo:
		SetIsHalo(abX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, abX);
	}

	return true;
}

bool cEdObjBillboard::SetProperty(int alPropID, const float afX)
{
	switch(alPropID)
	{
	case eBillboardFloat_Offset:
		SetBillboardOffset(afX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, afX);
	}

	return true;
}

bool cEdObjBillboard::SetProperty(int alPropID, const tString& asX)
{
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
		return iEdScnObject::SetProperty(alPropID, asX);
	}

	return true;
}

bool cEdObjBillboard::SetProperty(int alPropID, const cVector2f& avX)
{
	switch(alPropID)
	{
	case eBillboardVec2f_Size:
		SetBillboardSize(avX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, avX);
	}

	return true;
}

bool cEdObjBillboard::SetProperty(int alPropID, const cVector3f& avX)
{
	switch(alPropID)
	{
	case eBillboardVec3f_HaloSourceSize:
		SetHaloSourceSize(avX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, avX);
	}

	return true;
}

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

void cEdObjBillboard::OnDraw(const cModuleDrawData& aData)
{
	iEdScnObject::OnDraw(aData);

    if(mbSelected==false)
		return;

	aData.mpFunctions->SetDepthTest(true);
	aData.mpFunctions->SetDepthWrite(false);

	aData.mpFunctions->SetMatrix(NULL);
	aData.mpFunctions->SetProgram(NULL);
	
	cBoundingVolume* pBV = mpEngObject->GetRenderBV();
	if(pBV)
		aData.mpFunctions->GetLowLevelGfx()->DrawBoxMinMax(pBV->GetMin(), pBV->GetMax(), cColor(1,1));

	if(msType=="Axis" || msType=="FixedAxis")
		aData.mpViewport->DrawArrow(aData.mpFunctions, mmtxTransform, 1, true, cVector2f(0.05f, 0.4f), cColor(1,1));

	if(mbIsHalo)
	{
		cMatrixf mtxTransform = mmtxTransform.GetRotation();
		mtxTransform.SetTranslation(GetTranslation());
		aData.mpFunctions->SetMatrix(&mtxTransform);
		cVector3f vHalfHaloSourceSize = mvHaloSourceSize*0.5f;
		aData.mpFunctions->GetLowLevelGfx()->DrawBoxMinMax(vHalfHaloSourceSize*-1, vHalfHaloSourceSize, cColor(0,1,0,1));
	}
}

//------------------------------------------------------------------------------

/*
void cEdObjBillboard::OnSetCulled(bool abX)
{
	iEdScnObject::OnSetCulled(abX);

	//((cBillboard*)mpEngineEntity)->SetVisible(abX==false);
}
*/

//------------------------------------------------------------------------------
/*
void cEdObjBillboard::SaveToElement(cXmlElement* apElement)
{
	iEdObject::SaveToElement(apElement);
	apElement->SetValue("Billboard");

	apElement->SetAttributeVector3f("Axis", cMath::MatrixMul(cMath::MatrixRotate(mvRotation, eEulerRotationOrder_XYZ), cVector3f(0,1,0)));
	apElement->SetAttributeVector2f("BillboardSize", mvBillboardSize);
	apElement->SetAttributeFloat("BillboardOffset", mfBillboardOffset);
	apElement->SetAttributeString("MaterialFile", mpEdWorld->GetEditor()->GetFilePathRelativeToWorkingDir(msMatFile));
	apElement->SetAttributeString("BillboardType", msType);
	apElement->SetAttributeColor("BillboardColor", mCol);
	apElement->SetAttributeString("ConnectLight", msConnectedLightName);
	apElement->SetAttributeBool("IsHalo", mbIsHalo);
	apElement->SetAttributeVector3f("HaloSourceSize", mvHaloSourceSize);
}
*/
//------------------------------------------------------------------------------

iEdEditPane* cEdObjBillboard::CreateEditPane()
{
	return hplNew(cEdEditPaneBillboard,(this));
}

//------------------------------------------------------------------------------

void cEdObjBillboard::SetBillboardOffset(float afX)
{
	if(mfBillboardOffset==afX)
		return;

	mfBillboardOffset = afX;
	mbBBUpdated=true;
}

//------------------------------------------------------------------------------

void cEdObjBillboard::SetMaterialFile(const tString& asMatFile)
{
	if(msMatFile==asMatFile)
		return;

	msMatFile = asMatFile;
	mbBBUpdated=true;
	mbMatUpdated=true;
}

//------------------------------------------------------------------------------

void cEdObjBillboard::SetBillboardSize(const cVector2f& avSize)
{
	if(mvBillboardSize==avSize)
		return;

	mvBillboardSize = avSize;
	//mvScale = cVector3f(avSize.x, avSize.y, avSize.x);

	mbBBUpdated=true;
}


//------------------------------------------------------------------------------

void cEdObjBillboard::SetBillboardType(const tString& asType)
{
	if(msType==asType)
		return;

	msType = asType;

	mbBBUpdated=true;
	mbTypeUpdated=true;
}

//------------------------------------------------------------------------------

void cEdObjBillboard::SetBillboardColor(const cColor& aCol)
{
	if(mCol==aCol)
		return;

	mCol = aCol;
	mbBBUpdated=true;
	mbColorUpdated=true;
}

void cEdObjBillboard::SetConnectedLightName(const tString& asLightName)
{
	if(msConnectedLightName==asLightName)
		return;

	iEdObjLight* pLight = static_cast<iEdObjLight*>(GetWorld()->GetObject(cString::To16Char(msConnectedLightName)));
	if(pLight)
		pLight->RemoveConnectedBillboard(this);
	else
		SetConnectedLight(NULL);

	msConnectedLightName = asLightName;
	pLight = static_cast<iEdObjLight*>(GetWorld()->GetObject(cString::To16Char(asLightName)));
	if(pLight)
		pLight->AddConnectedBillboard(this);
	else
		mbColorUpdated=true;

	mbBBUpdated=true;
	mbConnectLightUpdated=true;
}

void cEdObjBillboard::SetConnectedLight(iEdObjLight* apLight)
{
	if(mpConnectedLight==apLight)
		return;
    
	mpConnectedLight = apLight;
	tWString sLightName = apLight?apLight->GetName():_W("");
	msConnectedLightName = cString::To8Char(sLightName);
	mbBBUpdated=true;
	mbConnectLightUpdated=true;
}

//------------------------------------------------------------------------------

void cEdObjBillboard::SetIsHalo(bool abX)
{
	if(mbIsHalo==abX)
		return;

	mbIsHalo = abX;

	mbBBUpdated=true;
}

//------------------------------------------------------------------------------

void cEdObjBillboard::SetHaloSourceSize(const cVector3f& avX)
{
	if(mvHaloSourceSize==avX)
		return;

	mvHaloSourceSize = avX;

	mbBBUpdated=true;
}

//------------------------------------------------------------------------------

/*
void cEdObjBillboard::SetScale(const cVector3f& avScale)
{
	//iEdObject::SetAbsScale(avScale, alAxis);
	/*
	cVector3f vScale = avScale;
	if(alAxis==2)
		vScale.x = vScale.z;
	else if(alAxis==0)
		vScale.z = vScale.x;

	iEdObject::SetAbsScale(vScale, alAxis);
*/
	/*
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
	*/
	

	//mlLastTransformedAxis = alAxis;
//}


//------------------------------------------------------------------------------

/*
const cVector3f& cEdObjBillboard::GetScale()
{
	mvBBScale =  cVector3f(mvBillboardSize.x, mvBillboardSize.y, mvBillboardSize.x);

	return mvBBScale;
}
*/

void cEdObjBillboard::SetWorldMatrix(const cMatrixf& amtxX)
{
	cMatrixf mtxInvMatrix = cMath::MatrixInverse(amtxX);
	cVector3f vScale = cVector3f(1.0f/mtxInvMatrix.GetRight().Length(),
									1.0f/mtxInvMatrix.GetUp().Length(),
									1.0f/mtxInvMatrix.GetForward().Length());

	cMatrixf mtxScale = cMath::MatrixScale(vScale);
	cMatrixf mtxRotate = cMath::MatrixMul(amtxX.GetRotation(), cMath::MatrixInverse(mtxScale));

	//Log("SetWorldMatrixCalled for %s\n", msName.c_str());

	SetTranslation(amtxX.GetTranslation());
	SetRotation(cMath::MatrixToEulerAngles(mtxRotate, eEulerRotationOrder_XYZ));
	//SetAbsScale(vScale);
}

//------------------------------------------------------------------------------

void cEdObjBillboard::UpdateEntity()
{
	/*
	iEdObject::UpdateEntity();
	if(mpEngineEntity)
	{
		mpEngineEntity->Update();
		mpEngineEntity->SetMatrix(cMath::MatrixMul(mmtxTranslate, mmtxRotate));
		mpEngineEntity->UpdateVisibility();
	}
	*/
}

//void cEdObjBillboard::UpdateEntity()
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
				GetEdWorld()->GetWorld()->DestroyBillboard((cBillboard*)mpEngineEntity);
			}
			//mpEngineEntity = CreateEngineEntity(GetEdWorld(), mlID, msType);
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
				cMaterialManager* pManager = GetEdWorld()->GetEditor()->GetEngine()->GetResources()->GetMaterialManager();
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
				//cEdObjLight* pLight = (cEdObjLight*)GetEdWorld()->GetObjectByName(msConnectedLightName);
				//if(pLight)
				//	pLight->AddConnectedBillboard(this);
				mbConnectLightUpdated=false;
			}
		}
	}
*/
//	iEdObject::UpdateEntity();
//}

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

eBillboardType cEdObjBillboard::GetBillboardTypeFromString(const tString& asType)
{
	for(int i=0;i<3;++i)
	{
		if(asType==gsBillboardTypeString[i])
			return (eBillboardType)i;
	}

	return eBillboardType_Point;
}

//------------------------------------------------------------------------------

iEngineObject* cEdObjBillboard::CreateEngineObject()
{
	return hplNew(cIconEntityBB,(this));
}

//------------------------------------------------------------------------------
