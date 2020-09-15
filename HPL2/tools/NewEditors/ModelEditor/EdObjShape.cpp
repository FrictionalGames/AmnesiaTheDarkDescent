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

#include "../ModelEditor/EdObjShape.h"

#include "../ModelEditor/EdObjShapeBox.h"
#include "../ModelEditor/EdObjShapeCapsule.h"
#include "../ModelEditor/EdObjShapeCylinder.h"
#include "../ModelEditor/EdObjShapeSphere.h"

#include "../Common/EdAction.h"
#include "../Common/EdEngineObject.h"
#include "../Common/Editor.h"

#include "../Common/EdResource.h"

#include "../ModelEditor/EdEditPaneShape.h"
#include "../ModelEditor/EdEditPaneShapes.h"

#include "../ModelEditor/EdObjBody.h"

//#include "EditorActionsBodies.h"

#include "../Common/EdEditModeSelectToolScale.h"

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// TYPE SHAPE - CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cTypeShape::cTypeShape() : iEdScnObjType(_W("Shape"), "BodyShape")
{
	mbScalable = false;
	//AddString(eShapeStr_ShapeType, "ShapeType", "Box", ePropStep_PreCreate);
	AddInt(eShapeInt_ParentBodyID, "ParentBodyID", -1, ePropStep_PostDeployAll, false);

	//AddVec3f(eShapeVec3f_Size, "ShapeSize", cVector3f(1,1,1), ePropStep_PreCreate, false);

	for(int i=0;i<2;++i)
	{
		for(int j=0;j<2;++j)
		{
			mvMaterialFiles[i][j].clear();
			mvMaterials[i][j] = NULL;
		}
	}
}

cTypeShape::~cTypeShape()
{
	cMaterialManager* pMgr = mpWorld->GetEditor()->GetEngine()->GetResources()->GetMaterialManager();
	for(int i=0;i<2;++i)
	{
		for(int j=0;j<2;++j)
		{
			cMaterial* pMat = mvMaterials[i][j];
			if(pMat) pMgr->Destroy(pMat);
		}
	}
}

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// TYPE SHAPE - PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cTypeShape::Init()
{
	if(iEdScnObjType::Init()==false)
		return false;

	mvMaterialFiles[0][0] = "editor_bodyshape.mat";
	mvMaterialFiles[0][1] = "editor_bodyshape_selected.mat";
	mvMaterialFiles[1][0] = "editor_body.mat";
	mvMaterialFiles[1][1] = "editor_body_selected.mat";

	bool bSuccessful = true;
	//////////////////////////
	// Load textures (perhaps store elsewhere)?
	for(int i=0;i<2;++i)
		for(int j=0;j<2;++j)
            bSuccessful = bSuccessful && cEdLoader::LoadMaterial(mvMaterialFiles[i][j], &mvMaterials[i][j]);

	bSuccessful = bSuccessful && AddChildType(hplNew(cTypeShapeBox,()));
	bSuccessful = bSuccessful && AddChildType(hplNew(cTypeShapeCapsule,()));
	bSuccessful = bSuccessful && AddChildType(hplNew(cTypeShapeCylinder,()));
	bSuccessful = bSuccessful && AddChildType(hplNew(cTypeShapeSphere,()));

	return bSuccessful;
}

//---------------------------------------------------------------------------

iEdObjectType* cTypeShape::GetTypeByXmlElement(cXmlElement* apElement)
{
	if(iEdObjectType::GetTypeByXmlElement(apElement)==NULL)
		return NULL;

	for(int i=0; i<GetNumChildren(); ++i)
	{
		iEdObjectType* pSubType = GetChild(i);
		iEdObjectType* pType = pSubType->GetTypeByXmlElement(apElement);

		if(pType) return pType;
	}

	return NULL;
}

//---------------------------------------------------------------------------

cMaterial* cTypeShape::GetShapeMaterial(int alX, int alY)
{
	return mvMaterials[alX][alY];
}

const tString& cTypeShape::GetMaterialFile(int alX, int alY)
{
	return mvMaterialFiles[alX][alY];
}

//---------------------------------------------------------------------------

iEdEditPane* cTypeShape::CreateGroupEditPane(iEdScnObject* apObj)
{
	return hplNew(cEdEditPaneShapes,(apObj));
}

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// TYPE SHAPE - PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SHAPE SUBTYPE BASE - CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iShapeSubType::iShapeSubType(const tWString& asName) : iEdScnObjType(asName, "Shape")
{
}

iShapeSubType::~iShapeSubType()
{
}

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SHAPE SUBTYPE BASE - PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEdObjectType* iShapeSubType::GetTypeByXmlElement(cXmlElement* apElement)
{
	tWString sShapeType = cString::To16Char(apElement->GetAttributeString("ShapeType"));
	//tWString sShapeType = apElement->GetAttributeStringW("ShapeType");
	if(sShapeType==GetName())
		return this;

	return NULL;
}

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SHAPE DATA - CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEdObjShapeData::iEdObjShapeData(iEdObjectType* apType) : iEdScnObjData(apType)
{
	SetName(_W("Shape"));
}

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SHAPE DATA - PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void iEdObjShapeData::CopyFromObject(iEdObject* apObj)
{
	iEdScnObjData::CopyFromObject(apObj);
	mpShape = static_cast<iEdObjShape*>(apObj);
}

//---------------------------------------------------------------------------

bool iEdObjShapeData::LoadSpecificData(cXmlElement* apElement)
{
	if(iEdScnObjData::LoadSpecificData(apElement)==false)
		return false;

	SetVec3f(eShapeVec3f_Size, GetVec3f(eScnVec3f_Scale));
	SetVec3f(eScnVec3f_Scale, 1);

	return true;
}

//---------------------------------------------------------------------------

bool iEdObjShapeData::SaveSpecificData(cXmlElement* apElement)
{
	if(iEdScnObjData::SaveSpecificData(apElement)==false)
		return false;

	// Hack so we save shape size as scale
	apElement->SetAttributeVector3f("Scale", mpShape->GetSize());

	cMatrixf mtxRelativeTransform = cMatrixf::Identity;
	cMatrixf* pRelativeMtx = mpShape->GetRelativeMatrix();
	if(pRelativeMtx)
		mtxRelativeTransform = *pRelativeMtx;

	cMatrixf mtxInvMatrix = cMath::MatrixInverse(mtxRelativeTransform);
	cVector3f vScale = cVector3f(1.0f/mtxInvMatrix.GetRight().Length(),
									1.0f/mtxInvMatrix.GetUp().Length(),
									1.0f/mtxInvMatrix.GetForward().Length());
	cMatrixf mtxScale = cMath::MatrixScale(vScale);
	cMatrixf mtxRotate = cMath::MatrixMul(mtxRelativeTransform.GetRotation(), cMath::MatrixInverse(mtxScale));

	apElement->SetAttributeVector3f("RelativeTranslation", mtxRelativeTransform.GetTranslation());
	apElement->SetAttributeVector3f("RelativeRotation", cMath::MatrixToEulerAngles(mtxRotate, eEulerRotationOrder_XYZ));
	apElement->SetAttributeVector3f("RelativeScale", vScale);

	apElement->SetAttributeString("ShapeType", cString::To8Char(mpShape->GetTypeName()));

	return true;
}

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SHAPE DATA - PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SHAPE - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool iEdObjShape::mbHideConnectedShapes = false;
int	 iEdObjShape::mlMeshResolution = 18;

//---------------------------------------------------------------------------

iEdObjShape::iEdObjShape(iEdObjectData* apData) : iEdScnObject(apData)
												
{
	mpParentBody = NULL;
	mpVBShape = NULL;

	mpRelativeMatrix = NULL;
}

iEdObjShape::~iEdObjShape()
{
	if(mpParentBody)
		mpParentBody->RemoveComponent(this);

	if(mpEngObject)
	{
		static_cast<cEngineGeneratedMesh*>(mpEngObject)->SetCustomMaterial(NULL, false);
	}
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SHAPE - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cMaterial* iEdObjShape::GetShapeMaterial(int alX, int alY)
{
	cTypeShape* pType = static_cast<cTypeShape*>(GetMainType());

	return pType->GetShapeMaterial(alX, alY);
}

const tString& iEdObjShape::GetShapeMaterialFile(int alX)
{
	cTypeShape* pType = static_cast<cTypeShape*>(GetMainType());

	return pType->GetMaterialFile(alX, 0);
}

//---------------------------------------------------------------------------

bool iEdObjShape::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eShapeStr_ShapeType:
		mShapeType = StringToEditorBodyShape(asX); break;
	default:
		return iEdScnObject::SetProperty(alPropID, asX);
	}

	return true;
}

bool iEdObjShape::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eShapeStr_ShapeType:
		asX = EditorBodyShapeToString(GetShapeType()); break;
	default:
		return iEdScnObject::GetProperty(alPropID, asX);
	}

	return true;
}

//---------------------------------------------------------------------------

bool iEdObjShape::SetProperty(int alPropID, const cVector3f& avX)
{
	switch(alPropID)
	{
	case eShapeVec3f_Size:
		SetSize(avX); break;
	default:
		return iEdScnObject::SetProperty(alPropID, avX);
	}

	return true;
}

bool iEdObjShape::GetProperty(int alPropID, cVector3f& avX)
{
	switch(alPropID)
	{
	case eShapeVec3f_Size:
		avX = GetSize(); break;
	default:
		return iEdScnObject::GetProperty(alPropID, avX);
	}

	return true;
}

//---------------------------------------------------------------------------

bool iEdObjShape::CheckPicked(cUIPickFilter* apFilter, cUIPickMethod* apMethod, iEdScnObject** apPickedObject)
{
	bool bPicked = iEdScnObject::CheckPicked(apFilter, apMethod, apPickedObject);
	bool bCheckBody = HasParentBody() && apFilter->GetTypeFilter(mpParentBody->GetMainType());

	if(bCheckBody)
	{
		if(bPicked==false)
			bPicked = apMethod->Picks(this);
		
		if(bPicked && apPickedObject)
			*apPickedObject = mpParentBody;
	}

	return bPicked;
}

//---------------------------------------------------------------------------

void iEdObjShape::OnDraw(const cModuleDrawData& aData)
{
	if(aData.mStep==eEdDrawStep_PostTrans) return;

	bool bHasBody = mpParentBody!=NULL;
	bool bBodySelected = bHasBody&&mpParentBody->IsSelected();

	if(mpEngObject) 
		mpEngObject->Draw(aData.mpViewport, aData.mpFunctions, mbSelected || bBodySelected, true);

	if(mbSelected && bHasBody)
	{
		aData.mpFunctions->GetLowLevelGfx()->DrawLine(GetTranslation(), mpParentBody->GetTranslation(), cColor(1,0,0,1));
	}
}

//---------------------------------------------------------------------------

iEdEditPane* iEdObjShape::CreateEditPane()
{
	return hplNew(cEdEditPaneShape,(this));
}

//---------------------------------------------------------------------------

void iEdObjShape::SetSize(const cVector3f& avX)
{
	if(mvShapeSize==avX)
		return;

	mvShapeSize = avX;

	if(mpEngObject)
	{
		static_cast<cEngineGeneratedMesh*>(mpEngObject)->ReCreate(CreateShape());
		OnUpdate(false);
	}

	SetUpdated();
}

//---------------------------------------------------------------------------

void iEdObjShape::SetParentBody(cEdObjBody* apParentBody)
{
	if(mpParentBody==apParentBody)
		return;

	mpParentBody = apParentBody;

	if(mpEngObject)
	{
		static_cast<cEngineGeneratedMesh*>(mpEngObject)->ReCreate(CreateShape());
		OnUpdate(false);
	}
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SHAPE - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool iEdObjShape::OnUpdate(bool abForce)
{
	if(mpEngObject)
	{
		cMatrixf mtxWorld = cMath::MatrixMul(GetTranslationMatrix(abForce), GetRotationMatrix(abForce));;
		//Log("Object %ls matrix : %s\n", GetName().c_str(), mtxWorld.ToFileString().c_str());
		mpEngObject->Update();
		mpEngObject->UpdateVisibility();
		mpEngObject->SetMatrix(mtxWorld);
		return true;
	}
	
	return static_cast<iEdScnObjType*>(mpType)->AllowNullEngineObject();
}

//---------------------------------------------------------------------------

iEngineObject* iEdObjShape::CreateEngineObject()
{
	cMesh* pShapeMesh = CreateShape();
	if(pShapeMesh==NULL)
		return NULL;

	cTypeShape* pType = static_cast<cTypeShape*>(mpType);
	cEngineGeneratedMesh* pEnt = hplNew(cEngineGeneratedMesh,(this, pShapeMesh));

	return pEnt;
}

//---------------------------------------------------------------------------


eEdShape iEdObjShape::StringToEditorBodyShape(const tString& asName)
{
	tString sLowName = cString::ToLowerCase(asName);

	if(sLowName == "box")		return eEdShape_Box;
	if(sLowName == "cylinder")	return eEdShape_Cylinder;
	if(sLowName == "sphere")	return eEdShape_Sphere;
	if(sLowName == "capsule")	return eEdShape_Capsule;

	return eEdShape_Box;
}

//----------------------------------------------------------------------------

tString iEdObjShape::EditorBodyShapeToString(eEdShape aX)
{
	switch(aX)
	{
	case eEdShape_Sphere:	return "Sphere";
									break;
	case eEdShape_Cylinder:	return "Cylinder";
									break;
	case eEdShape_Capsule:	return "Capsule";
									break;
	}

	return "Box";
}

//---------------------------------------------------------------------------

iEdAction* iEdObjShape::CreateActionCreateBody()
{
	return CreateActionCreateBodyFromShapes(GetWorld(), tScnObjList(1, this));
}

iEdAction* iEdObjShape::CreateActionCreateBodyFromShapes(iEdWorld* apWorld, const tScnObjList& alstShapes)
{
	cEdActionCompound* pAction = hplNew(cEdActionCompound,(_W("Create Body")));

	iEdObjectType* pType = apWorld->GetTypeByName(_W("Body"));
	iEdObjectData* pData = pType->CreateData();
	pData->SetName(apWorld->GenerateValidName(pData->GetName(), true));

	pAction->AddAction(apWorld->CreateActionCreateObject(pData));
	pAction->StepForward();
	cEdObjBody* pBody = static_cast<cEdObjBody*>(apWorld->GetObject(pData->GetID()));
	pAction->AddAction(pBody->CreateAddComponentsAction(alstShapes));
	
	return pAction;
}

//---------------------------------------------------------------------------

bool iEdObjShape::SkipAddingToOutline(int alLevel, const tScnObjList& alstAddedChildren)
{
	bool bHasParentBody = GetParentBody()!=NULL;
	if(alLevel==0 && bHasParentBody)
		return true;

	if(bHasParentBody && mbHideConnectedShapes)
		return true;

	return iEdScnObject::SkipAddingToOutline(alLevel, alstAddedChildren);
}

//---------------------------------------------------------------------------
