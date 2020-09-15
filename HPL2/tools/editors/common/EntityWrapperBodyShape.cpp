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

#include "EntityWrapperBodyShape.h"

#include "EngineEntity.h"
#include "EditorBaseClasses.h"
#include "EditorWorld.h"

#include "EditorWindowEntityEditBoxBodyShape.h"
#include "EditorWindowEntityEditBoxGroupShapes.h"

#include "EntityWrapperBody.h"

#include "EditorActionsBodies.h"

//---------------------------------------------------------------------------

cEntityWrapperTypeBodyShape::cEntityWrapperTypeBodyShape() : iEntityWrapperType(eEditorEntityType_BodyShape, _W("Shape"), "BodyShape")
{
	AddString(eShapeStr_ShapeType, "ShapeType", "Box", ePropCopyStep_PreEnt);
	AddInt(eShapeInt_ParentBodyID, "ParentBodyID", -1, ePropCopyStep_PostDeployAll, false);
}

cEntityWrapperTypeBodyShape::~cEntityWrapperTypeBodyShape()
{
	/*
	cMaterialManager* pMgr = mpWorld->GetEditor()->GetEngine()->GetResources()->GetMaterialManager();
	for(int i=0;i<2;++i)
		for(int j=0;j<2;++j)
			pMgr->Destroy(mvMaterials[i][j]);
			*/
}

cMaterial* cEntityWrapperTypeBodyShape::GetShapeMaterial(int alX, int alY)
{
	return mvMaterials[alX][alY];
}

const tString& cEntityWrapperTypeBodyShape::GetMaterialFile(int alX, int alY)
{
	return mvMaterialFiles[alX][alY];
}

void cEntityWrapperTypeBodyShape::OnSetWorld(iEditorWorld* apWorld)
{
	cMaterialManager* pMgr = apWorld->GetEditor()->GetEngine()->GetResources()->GetMaterialManager();
	/*mvMaterials[0][0] = pMgr->CreateMaterial("editor_bodyshape.mat");
	mvMaterials[0][1] = pMgr->CreateMaterial("editor_bodyshape_selected.mat");
	mvMaterials[1][0] = pMgr->CreateMaterial("editor_body.mat");
	mvMaterials[1][1] = pMgr->CreateMaterial("editor_body_selected.mat");*/

	mvMaterialFiles[0][0] = "editor_bodyshape.mat";
	mvMaterialFiles[0][1] = "editor_bodyshape.mat";
	mvMaterialFiles[1][0] = "editor_body.mat";
	mvMaterialFiles[1][1] = "editor_body.mat";
}

iEntityWrapperData* cEntityWrapperTypeBodyShape::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataBodyShape,(this));
}

cEntityWrapperDataBodyShape::cEntityWrapperDataBodyShape(iEntityWrapperType* apType) : iEntityWrapperData(apType)
{
	SetName("Shape");
}

//---------------------------------------------------------------------------

void cEntityWrapperDataBodyShape::CopyFromEntity(iEntityWrapper* apEntity)
{
	iEntityWrapperData::CopyFromEntity(apEntity);
	mpShape = (cEntityWrapperBodyShape*)apEntity;
}

//---------------------------------------------------------------------------

bool cEntityWrapperDataBodyShape::SaveSpecific(cXmlElement* apElement)
{
	if(iEntityWrapperData::SaveSpecific(apElement)==false)
		return false;

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

	return true;
}

//---------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataBodyShape::CreateSpecificEntity()
{
	return hplNew(cEntityWrapperBodyShape,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEntityWrapperBodyShape::cEntityWrapperBodyShape(iEntityWrapperData* apData) : iEntityWrapper(apData)
												
{
	mpParentBody = NULL;
	mpVBShape = NULL;

	mpRelativeMatrix = NULL;

	//mShapeType = aShapeType;

	//SetUpShape();

	//////////////////////////
	// Load textures (perhaps store elsewhere)?
	/*
	cTextureManager *pTexManager = GetEditorWorld()->GetEditor()->GetEngine()->GetResources()->GetTextureManager();
	mvTextures[0][0] = pTexManager->Create2D("editor_body_shape_default.tga", true);
	mvTextures[0][1] = pTexManager->Create2D("editor_body_shape_selected.tga", true);
	mvTextures[1][0] = pTexManager->Create2D("editor_body_shape_body.tga", true);
	mvTextures[1][1] = pTexManager->Create2D("editor_body_shape_body_selected.tga", true);
	*/
}

cEntityWrapperBodyShape::~cEntityWrapperBodyShape()
{
	if(mpParentBody)
		mpParentBody->RemoveComponent(this);
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

bool cEntityWrapperBodyShape::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eShapeStr_ShapeType:
		mShapeType = StringToEditorBodyShape(asX); break;
	default:
		return iEntityWrapper::SetProperty(alPropID, asX);
	}

	return true;
}

bool cEntityWrapperBodyShape::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eShapeStr_ShapeType:
		asX = EditorBodyShapeToString(GetShapeType()); break;
	default:
		return iEntityWrapper::GetProperty(alPropID, asX);
	}

	return true;
}


//---------------------------------------------------------------------------

void cEntityWrapperBodyShape::Draw(	cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions,
									iEditorEditMode* apEditMode,bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol )
{
	apFunctions->SetProgram(NULL);
	apFunctions->SetTextureRange(NULL,0);

	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	cEngineEntityGeneratedMesh* pMesh = (cEngineEntityGeneratedMesh*)mpEngineEntity;
	apFunctions->SetMatrix(pMesh->GetMeshEntity()->GetSubMeshEntity(0)->GetModelMatrix(NULL));

	cColor col = abIsSelected?cColor(1) : cColor(1,1,1,0.1f);
	apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
	apFunctions->DrawWireFrame(pMesh->GetVertexBuffer(), col);

	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_None);

//	apFunctions->SetMatrix(&mmtxTransform);

//	apFunctions->SetTexture(0, mvTextures[(mpParentBody!=NULL)][abIsSelected]);
//	apFunctions->GetLowLevelGfx()->SetColor(cColor(0,1,0,0.1f));
	
//	apFunctions->SetVertexBuffer(mpShapeVtxBuffer);
//	apFunctions->DrawCurrent();
	
//	apFunctions->GetLowLevelGfx()->SetBlendActive(false);
//	apFunctions->DrawCurrent(eVertexBufferDrawType_LineStrip);

//	apFunctions->SetTexture(0,NULL);

//	apFunctions->SetMatrix(NULL);
}

//---------------------------------------------------------------------------

/*
void cEntityWrapperBodyShape::SaveToElement(cXmlElement* apElement)
{
	apElement->SetValue("BodyShape");

	iEntityWrapper::SaveToElement(apElement);

	apElement->SetAttributeInt("ShapeTypeID", mShapeType);
	tString sShapeType;
	switch(mShapeType)
	{
	case eEditorBodyShape_Box:
		sShapeType = "Box"; 
		break;
	case eEditorBodyShape_Sphere:	
		sShapeType = "Sphere"; 
		break;
	case eEditorBodyShape_Cylinder:	
		sShapeType = "Cylinder"; 
		break;
	case eEditorBodyShape_Capsule:	
		sShapeType = "Capsule"; 
		break;
	default:						
		break;
	}
	apElement->SetAttributeString("ShapeType", sShapeType);

	apElement->SetAttributeVector3f("RelativeTranslation", mvTranslationRelativeToBody);
	apElement->SetAttributeVector3f("RelativeRotation", mvRotationRelativeToBody);
	apElement->SetAttributeVector3f("RelativeScale", mvScaleRelativeToBody);
}*/

//---------------------------------------------------------------------------

cEditorWindowEntityEditBox* cEntityWrapperBodyShape::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	cEditorWindowEntityEditBox* pEditBox = (cEditorWindowEntityEditBox*)hplNew(cEditorWindowEntityEditBoxBodyShape, (apEditMode, this));

	return pEditBox;
}

//---------------------------------------------------------------------------

void cEntityWrapperBodyShape::SetParentBody(cEntityWrapperBody* apParentBody)
{
	if(mpParentBody==apParentBody)
		return;

	mpParentBody = apParentBody;

	if(mpEngineEntity)
	{
		((cEngineEntityGeneratedMesh*)mpEngineEntity)->ReCreate(CreateShape());
		UpdateEntity();
	}
}

//---------------------------------------------------------------------------

bool cEntityWrapperBodyShape::EntitySpecificFilterCheck(bool abPassAll, bool abPassType)
{
	if(HasParentBody())
		return abPassType && abPassAll==false;
	
	return iEntityWrapper::EntitySpecificFilterCheck(abPassAll, abPassType);
}

//---------------------------------------------------------------------------

cEditorWindowEntityEditBoxGroup* cEntityWrapperBodyShape::CreateGroupAllSameTypeEditBox(cEditorEditModeSelect* apEditMode, tEntityWrapperList& alstEntities)
{
    return hplNew(cEditorWindowEntityEditBoxGroupShapes,(apEditMode, alstEntities));
}

//---------------------------------------------------------------------------

void cEntityWrapperBodyShape::SetAbsScale(const cVector3f& avScale, int alAxis)
{
	if(mvScale==avScale)
		return;

	//Log("%s %s input scale = %s\n", EditorBodyShapeToString(mShapeType).c_str(), msName.c_str(), avScale.ToFileString().c_str());

	cVector3f vScale = avScale;
	switch(mShapeType)
	{
	case eEditorBodyShape_Cylinder:
	case eEditorBodyShape_Capsule:
		{
			if(alAxis==0 || alAxis==-1)
				vScale.z = vScale.x;
			else if(alAxis==2)
				vScale.x = vScale.z;
		}
		break;
	case eEditorBodyShape_Sphere:
		{
			if(alAxis==-1)
				alAxis=mlLastTransformedAxis;
			if(alAxis==-1)
				alAxis=0;

			for(int i=0;i<3;++i)
				vScale.v[i] = vScale.v[alAxis];
		}
	}

	//Log("Shape processed input scale = %s\n", vScale.ToFileString().c_str());
	
	iEntityWrapper::SetAbsScale(vScale, alAxis);
	if(mpEngineEntity)
		((cEngineEntityGeneratedMesh*)mpEngineEntity)->ReCreate(CreateShape());
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cMesh* cEntityWrapperBodyShape::CreateShape()
{
	cEntityWrapperTypeBodyShape* pType = (cEntityWrapperTypeBodyShape*)mpType;
	tString sMat = pType->GetMaterialFile(HasParentBody(), mbSelected);

	int lMeshResolution = 18;
	cMesh* pMesh = NULL;
	cMeshCreator* pCreator = GetEditorWorld()->GetEditor()->GetEngine()->GetGraphics()->GetMeshCreator();
	switch(mShapeType)
	{
	case eEditorBodyShape_Box:		pMesh = pCreator->CreateBox("", mvScale, sMat);
									break;
	case eEditorBodyShape_Sphere:	pMesh = pCreator->CreateSphere("", mvScale.x, lMeshResolution, lMeshResolution, sMat); //mCustomBV.SetSize(cVector3f(2)); //diameter = radius*2 = 2
									break;
	case eEditorBodyShape_Cylinder:	pMesh = pCreator->CreateCylinder("", cVector2f(mvScale.x, mvScale.y), lMeshResolution, sMat); //mCustomBV.SetSize(cVector3f(2,1,2));  //diameter = radius*2 = 2
									break;
	case eEditorBodyShape_Capsule:	pMesh = pCreator->CreateCapsule("", cVector2f(mvScale.x, mvScale.y), lMeshResolution, 5, sMat); //mCustomBV.SetSize(cVector3f(2,1,2));  //diameter = radius*2 = 2//mCustomBV.SetSize(cVector3f(2,1,2));  //diameter = radius*2 = 2
									break;
	}

	return pMesh;
}

//---------------------------------------------------------------------------

iEngineEntity* cEntityWrapperBodyShape::CreateSpecificEngineEntity()
{
	cMesh* pShapeMesh = CreateShape();
	if(pShapeMesh==NULL)
		return NULL;

	cEntityWrapperTypeBodyShape* pType = (cEntityWrapperTypeBodyShape*)mpType;
	cEngineEntityGeneratedMesh* pEnt = hplNew( cEngineEntityGeneratedMesh,(this, pShapeMesh));

	//pEnt->SetCustomMaterial(pType->GetShapeMaterial(0,0));

	return pEnt;
}

//---------------------------------------------------------------------------


eEditorBodyShape cEntityWrapperBodyShape::StringToEditorBodyShape(const tString& asName)
{
	tString sLowName = cString::ToLowerCase(asName);

	if(sLowName == "box")		return eEditorBodyShape_Box;
	if(sLowName == "cylinder")	return eEditorBodyShape_Cylinder;
	if(sLowName == "sphere")	return eEditorBodyShape_Sphere;
	if(sLowName == "capsule")	return eEditorBodyShape_Capsule;

	return eEditorBodyShape_Box;
}

//----------------------------------------------------------------------------

tString cEntityWrapperBodyShape::EditorBodyShapeToString(eEditorBodyShape aX)
{
	switch(aX)
	{
	case eEditorBodyShape_Sphere:	return "Sphere";
									break;
	case eEditorBodyShape_Cylinder:	return "Cylinder";
									break;
	case eEditorBodyShape_Capsule:	return "Capsule";
									break;
	}

	return "Box";
}

//---------------------------------------------------------------------------

/*
void cEntityWrapperBodyShape::UpdateMatrix()
{
	if(mbEntityUpdated)
	{
		if(mbTranslationUpdated)
		{
			mbTranslationUpdated=false;
			mmtxTranslate = cMath::MatrixTranslate(mvPosition);
		}
		if(mbRotationUpdated)
		{
			mbRotationUpdated=false;
			mmtxRotate = cMath::MatrixRotate(mvRotation,eEulerRotationOrder_XYZ);
		}

		mmtxTransform = cMath::MatrixMul(mmtxTranslate, mmtxRotate);

		mbEntityUpdated = false;
	}
}
*/


void cEntityWrapperBodyShape::UpdateEntity()
{
	UpdateMatrix();

	CheckCulledByClipPlanes(GetEditorWorld()->GetClipPlanes());

	if(mpEngineEntity)
	{
		mpEngineEntity->Update();
		mpEngineEntity->SetMatrix(cMath::MatrixMul(mmtxTranslate, mmtxRotate));
		mpEngineEntity->UpdateVisibility();
	}
	//iEntityWrapper::UpdateEntity();

	//cEntityWrapperTypeBodyShape* pType = (cEntityWrapperTypeBodyShape*)mpType;
	//cEngineEntityGeneratedMesh* pEntity = (cEngineEntityGeneratedMesh*)mpEngineEntity;
	//pEntity->SetCustomMaterial(pType->GetShapeMaterial(GetParentBody()!=NULL, mbSelected), false);
}

//---------------------------------------------------------------------------

iEditorAction* cEntityWrapperBodyShape::CreateActionCreateBody()
{
	return CreateActionCreateBodyFromShapes(GetEditorWorld(), tIntList(1, mlID));
}

iEditorAction* cEntityWrapperBodyShape::CreateActionCreateBodyFromShapes(iEditorWorld* apWorld, const tIntList& alstShapeIDs)
{
	cEditorEditModeSelect* pEditMode = (cEditorEditModeSelect*)apWorld->GetEditor()->GetEditMode("Select");
	cEditorActionCompoundAction* pCompoundAction = hplNew(cEditorActionCompoundAction,("Create Body"));

	iEntityWrapperType* pType = apWorld->GetEntityTypeByID(eEditorEntityType_Body);
	iEntityWrapperData* pData = pType->CreateData();
	int lBodyID = apWorld->GetFreeID();

	pData->SetID(lBodyID);
	pData->SetName(apWorld->GenerateName(pData->GetName()));

	pCompoundAction->AddAction(hplNew(cEditorActionObjectCreate,(apWorld, pData)));
	pCompoundAction->AddAction(hplNew(cEditorActionBodyAddShapes,(apWorld, lBodyID, alstShapeIDs)));
	pCompoundAction->AddAction(pEditMode->CreateSelectEntityAction(tIntList(1,lBodyID), eSelectActionType_Select));

	return pCompoundAction;
}

//---------------------------------------------------------------------------

void cEntityWrapperBodyShape::OnSetSelected(bool abX)
{
	cEntityWrapperTypeBodyShape* pType = (cEntityWrapperTypeBodyShape*)mpType;

	//((cEngineEntityGeneratedMesh*)mpEngineEntity)->SetCustomMaterial(pType->GetShapeMaterial(GetParentBody()!=NULL, abX), false);
}

//---------------------------------------------------------------------------
