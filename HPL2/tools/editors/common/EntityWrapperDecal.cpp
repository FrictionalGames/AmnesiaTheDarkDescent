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

#include "EntityWrapperDecal.h"

#include "EditorWorld.h"
#include "EditorWindowViewport.h"
#include "EditorWindowEntityEditBoxDecal.h"
#include "EditorHelper.h"
#include "EditorEditModeDecals.h"

#include "EntityIcon.h"

#include "EngineEntity.h"

#include <algorithm>

//-----------------------------------------------------------------------------------------

////////////////////////////////
// Static global decal variables
//  Min and max for decal offset. Actual offset value should pick a random value between these two.
float cEntityWrapperTypeDecal::mfDecalOffsetMin = 0.005f;
float cEntityWrapperTypeDecal::mfDecalOffsetMax = 0.01f;

//	Global max triangles per decal.
int cEntityWrapperTypeDecal::mlGlobalMaxTriangles = 300;
bool cEntityWrapperTypeDecal::mbForcingUpdate = false;

//------------------------------------------------------------------------------------------

cEntityWrapperTypeDecal::cEntityWrapperTypeDecal() : iEntityWrapperType(eEditorEntityType_Decal, _W("Decal"), "Decal")
{
	AddInt(eDecalInt_FileIndex, "MaterialIndex", ePropCopyStep_PreEnt);
	AddString(eDecalStr_Material, "Material", "", ePropCopyStep_PreEnt, false);
	AddInt(eDecalInt_CurrentSubDiv, "CurrentSubDiv", 0, ePropCopyStep_PreEnt);
	AddInt(eDecalInt_MaxTris, "MaxTriangles", -1, ePropCopyStep_PreEnt);

	AddFloat(eDecalFloat_Offset, "Offset", 0, ePropCopyStep_PreEnt);
	AddVec2f(eDecalVec2f_SubDivs, "SubDiv", 1, ePropCopyStep_PreEnt);
	AddColor(eDecalCol_Color, "Color", cColor(1), ePropCopyStep_PreEnt);

	AddBool(eDecalBool_AffectStatic, "OnStatic", true, ePropCopyStep_PreEnt);
	AddBool(eDecalBool_AffectPrimitive, "OnPrimitive", true, ePropCopyStep_PreEnt);
	AddBool(eDecalBool_AffectEntity, "OnEntity", true, ePropCopyStep_PreEnt);
}

//-----------------------------------------------------------------------------------------

iEntityWrapperData* cEntityWrapperTypeDecal::CreateSpecificData()
{
	return hplNew(cEntityWrapperDataDecal,(this));
}

cEntityWrapperDataDecal::cEntityWrapperDataDecal(iEntityWrapperType* apType) : iEntityWrapperData(apType)
{
	mpDecal = NULL;
	mpMesh = NULL;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDataDecal::CopyFromEntity(iEntityWrapper* apEntity)
{
	iEntityWrapperData::CopyFromEntity(apEntity);
	mpDecal = apEntity;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDataDecal::CopyToEntity(iEntityWrapper* apEntity, int alCopyFlags)
{
	iEntityWrapperData::CopyToEntity(apEntity, alCopyFlags);
	//cEntityWrapperDecal* pDecal = (cEntityWrapperDecal*)apEntity;
	//pDecal->SetAffectedEntityIDs(mvEntityIDs);
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDataDecal::Load(cXmlElement* apElement)
{
	if(iEntityWrapperData::Load(apElement)==false)
		return false;

	iEditorWorld* pWorld = mpType->GetWorld();

	////////////////////////////////////////////
	// Load File index
	int lFileIndex = GetInt(eDecalInt_FileIndex);
	if(lFileIndex==-1)
	{
		tString sMat = cString::To8Char(pWorld->GetEditor()->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(GetString(eDecalStr_Material)));
		lFileIndex = pWorld->AddFilenameToIndex("Decals", sMat);
		SetInt(eDecalInt_FileIndex, lFileIndex);
	}

	////////////////////////////////////////////
	// Link to file by index
	tString sMaterial = pWorld->GetFilenameFromIndex("Decals", lFileIndex);
	if(sMaterial=="" && lFileIndex==-1)
	{
		pWorld->SetLoadErrorMessage(_W("File index out of bounds!"));
		return false;
	}

	SetString(eDecalStr_Material, sMaterial);

	////////////////////////////////////////////
	// Fix decal offset inside boundaries
	float fOffset = GetFloat(eDecalFloat_Offset);
	float fOffsetMin = cEntityWrapperTypeDecal::GetDecalOffsetMin();
	float fOffsetMax = cEntityWrapperTypeDecal::GetDecalOffsetMax();
	if(fOffset < fOffsetMin || 
		fOffset > fOffsetMax)
	{
		fOffset = cMath::RandRectf(fOffsetMin, fOffsetMax);

		SetFloat(eDecalFloat_Offset, fOffset);
	}

	cEngine* pEng = mpType->GetWorld()->GetEditor()->GetEngine();
	mpMesh = cEngineFileLoading::LoadDecalMeshHelper(apElement->GetFirstElement("DecalMesh"), pEng->GetGraphics(), pEng->GetResources(), GetName(), sMaterial, GetColor(eDecalCol_Color)); 

	/////////////////////////////////////////////////
	// Load affected entity IDs
	//tString sAffectedEntityIDs = apElement->GetAttributeString("AffectedEnts", "");
	//mvEntityIDs.clear();
	//cString::GetIntVec(sAffectedEntityIDs, mvEntityIDs);

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDataDecal::SaveSpecific(cXmlElement* apElement)
{
	if(iEntityWrapperData::SaveSpecific(apElement)==false || mpDecal==NULL)
		return false;

	cEngineEntityGeneratedMesh* pDecalMesh = (cEngineEntityGeneratedMesh*)mpDecal->GetEngineEntity();
	iVertexBuffer* pVB = NULL;

	//////////////////////////////////////////////////
	// Fix for meshless decals
	int lVertexNum = 0;
	int lIndexNum = 0;

	if(pDecalMesh)
	{
        pVB = pDecalMesh->GetVertexBuffer();
		lVertexNum = pVB->GetVertexNum();
		lIndexNum = pVB->GetIndexNum();
	}

	cXmlElement* pMesh = apElement->CreateChildElement("DecalMesh");
	pMesh->SetAttributeInt("NumVerts", lVertexNum);
	pMesh->SetAttributeInt("NumInds", lIndexNum);

	if(pVB==NULL)
		return true;

	//////////////////////////////////////////////
	// Get data from vertex buffer
	float* pPositions = pVB->GetFloatArray(eVertexBufferElement_Position);
	float* pTexCoords = pVB->GetFloatArray(eVertexBufferElement_Texture0);
	float* pNormals = pVB->GetFloatArray(eVertexBufferElement_Normal);
	float* pTangents = pVB->GetFloatArray(eVertexBufferElement_Texture1Tangent);
	unsigned int* pIndices = pVB->GetIndices();

	int lPosStride = pVB->GetElementNum(eVertexBufferElement_Position);
	int lTexCoordStride = pVB->GetElementNum(eVertexBufferElement_Texture0);
	int lNormalStride = pVB->GetElementNum(eVertexBufferElement_Normal);
	int lTangentStride = pVB->GetElementNum(eVertexBufferElement_Texture1Tangent);

	//////////////////////////////////////
	// Create xml elements
	cXmlElement* pXmlPositions = pMesh->CreateChildElement("Positions");
	cXmlElement* pXmlNormals = pMesh->CreateChildElement("Normals");
	cXmlElement* pXmlTangents = pMesh->CreateChildElement("Tangents");
	cXmlElement* pXmlTexCoords = pMesh->CreateChildElement("TexCoords");
	cXmlElement* pXmlIndices = pMesh->CreateChildElement("Indices");

	tString sPositions;
	tString sNormals;
	tString sTangents;
	tString sTexCoords;
	tString sIndices;

	///////////////////////////////////////////////////
	// Iterate through all data and save to temporary strings
	for(int i=0;i<pVB->GetVertexNum();++i)
	{
		SaveGeometryDataToString(sPositions, lPosStride, pPositions);
		SaveGeometryDataToString(sTexCoords, lTexCoordStride, pTexCoords);
		SaveGeometryDataToString(sNormals, lNormalStride, pNormals);
		SaveGeometryDataToString(sTangents, lTangentStride, pTangents);

		pPositions += lPosStride;
		pTexCoords += lTexCoordStride;
		pNormals += lNormalStride;
		pTangents += lTangentStride;
	}

	sIndices = cString::ToString((int)pIndices[0]);
	for(int i=1;i<pVB->GetIndexNum();++i)
		sIndices += " " + cString::ToString((int)pIndices[i]);		

	/////////////////////////////////////////////////
	// Save strings to elements
	pXmlPositions->SetAttributeString("Array", sPositions);
	pXmlNormals->SetAttributeString("Array", sNormals);
	pXmlTangents->SetAttributeString("Array", sTangents);
	pXmlTexCoords->SetAttributeString("Array", sTexCoords);
	pXmlIndices->SetAttributeString("Array", sIndices);

	return true;
}

//-----------------------------------------------------------------------------------------

iEntityWrapper* cEntityWrapperDataDecal::CreateSpecificEntity()
{
	/////////////////////////////////////////////////////////////////////////////////////////
	// Check so the File Index is still valid ! (can become invalid after undoing a delete
	iEditorWorld* pWorld = mpType->GetWorld();

	int lFileIndex = GetInt(eDecalInt_FileIndex);
	const tString& sCurrentFile = GetString(eDecalStr_Material);
	tWString sPath = pWorld->GetEditor()->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(sCurrentFile);
	int lFileIndexForCurrentFile = pWorld->AddFilenameToIndex("Decals", cString::To8Char(sPath));

	// the If first index and the one we just retrieved are different, update first
	if(lFileIndex!=lFileIndexForCurrentFile)
		SetInt(eDecalInt_FileIndex, lFileIndexForCurrentFile);

	return hplNew(cEntityWrapperDecal,(this));
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDataDecal::SaveGeometryDataToString(tString& asOutput, int alNumElements, float* apData)
{
	for(int i=0;i<alNumElements;++i)
		asOutput += cString::ToString(apData[i], 3, true) + " ";
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

cEntityWrapperDecal::cEntityWrapperDecal(iEntityWrapperData* apData) : iEntityWrapper(apData)
{
	mbDecalUpdated=true;
	mbGeometryUpdated=true;
	mbMaterialUpdated=true;
	mColor = 1;
	mvSubDivisions = 1;
	
	mbAffectsStaticObjects = true;
	mbAffectsEntities = true;
	mbAffectsPrimitives = true;

	mbDeployed = false;
}

//-----------------------------------------------------------------------------------------

cEntityWrapperDecal::~cEntityWrapperDecal()
{
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDecal::SetProperty(int alPropID, const int& alX)
{
	switch(alPropID)
	{
	case eDecalInt_FileIndex:
		SetFileIndex(alX);
		break;
	case eDecalInt_CurrentSubDiv:
		SetCurrentSubDiv(alX);
		break;
	case eDecalInt_MaxTris:
		SetMaxTriangles(alX);
		break;
	default:
		return iEntityWrapper::SetProperty(alPropID, alX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDecal::SetProperty(int alPropID, const float& afX)
{
	switch(alPropID)
	{
	case eDecalFloat_Offset:
		SetOffset(afX);
		break;
	default:
		return iEntityWrapper::SetProperty(alPropID, afX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDecal::SetProperty(int alPropID, const bool& abX)
{
	switch(alPropID)
	{
	case eDecalBool_AffectStatic:
		SetAffectStatic(abX);
		break;
	case eDecalBool_AffectPrimitive:
		SetAffectPrimitive(abX);
		break;
	case eDecalBool_AffectEntity:
		SetAffectEntity(abX);
		break;
	default:
		return iEntityWrapper::SetProperty(alPropID, abX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDecal::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eDecalStr_Material:
		SetMaterial(asX);
		break;
	default:
		return iEntityWrapper::SetProperty(alPropID, asX);
	}
	
	return true;
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDecal::SetProperty(int alPropID, const cVector2f& avX)
{
	switch(alPropID)
	{
	case eDecalVec2f_SubDivs:
		{
			cVector2l vSubDivs = cVector2l((int)avX.x, (int)avX.y);	
			SetUVSubDivisions(vSubDivs);
		}
		break;
	default:
		return iEntityWrapper::SetProperty(alPropID, avX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDecal::SetProperty(int alPropID, const cColor& aX)
{
	switch(alPropID)
	{
	case eDecalCol_Color:
		SetColor(aX);
		break;
	default:
		return iEntityWrapper::SetProperty(alPropID, aX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDecal::GetProperty(int alPropID, int& alX)
{
	switch(alPropID)
	{
	case eDecalInt_FileIndex:
		alX = GetFileIndex();
		break;
	case eDecalInt_CurrentSubDiv:
		alX = GetCurrentSubDiv();
		break;
	case eDecalInt_MaxTris:
		alX = GetMaxTriangles();
		break;
	default:
		return iEntityWrapper::GetProperty(alPropID, alX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDecal::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eDecalFloat_Offset:
		afX = GetOffset();
		break;
	default:
		return iEntityWrapper::GetProperty(alPropID, afX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDecal::GetProperty(int alPropID, bool& abX)
{
	switch(alPropID)
	{
	case eDecalBool_AffectStatic:
		abX = mbAffectsStaticObjects;
		break;
	case eDecalBool_AffectPrimitive:
		abX = mbAffectsPrimitives;
		break;
	case eDecalBool_AffectEntity:
		abX = mbAffectsEntities;
		break;
	default:
		return iEntityWrapper::GetProperty(alPropID, abX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDecal::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eDecalStr_Material:
		asX = GetMaterial();
		break;
	default:
		return iEntityWrapper::GetProperty(alPropID, asX);
	}
	
	return true;
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDecal::GetProperty(int alPropID, cVector2f& avX)
{
	switch(alPropID)
	{
	case eDecalVec2f_SubDivs:
		{
			const cVector2l& vSubDivs = GetUVSubDivisions();
			avX = cVector2f((float)vSubDivs.x, (float)vSubDivs.y);
		}
		break;
	default:
		return iEntityWrapper::GetProperty(alPropID, avX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEntityWrapperDecal::GetProperty(int alPropID, cColor& aX)
{
	switch(alPropID)
	{
	case eDecalCol_Color:
		aX = GetColor();
		break;
	default:
		return iEntityWrapper::SetProperty(alPropID, aX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::OnPostDeployAll(bool abX)
{
	mbDeployed = true;

	UpdateEntity();

	iEntityWrapper::OnPostDeployAll(abX);
}


//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions,iEditorEditMode* apEditMode,
						bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol)
{
	iEntityWrapper::Draw(apViewport, apFunctions, apEditMode, abIsSelected);
	if(mbSelected)
	{
		//cDecalCreator* pCreator = GetEditorWorld()->GetEditor()->GetEngine()->GetGraphics()->GetDecalCreator();
		//pCreator->DrawDebug(apFunctions, true, true);
		//cMatrixf mtxOrientation = mmtxRotate.GetTranspose();
		//apFunctions->SetTextureRange(NULL,0);
		//apFunctions->SetDepthTest(true);
		//apFunctions->SetMatrix(&mmtxTranslate);
		//apFunctions->GetLowLevelGfx()->DrawLine(0, mtxOrientation.GetRight()*mvScale.x*0.5f, cColor(1,0,0,1));
		//apFunctions->GetLowLevelGfx()->DrawLine(0, mtxOrientation.GetUp()*mvScale.y*0.5f, cColor(0,1,0,1));
		//apFunctions->GetLowLevelGfx()->DrawLine(0, mtxOrientation.GetForward()*mvScale.z*0.5f, cColor(0,0,1,1));

		/*apFunctions->SetTextureRange(NULL,0);
		apFunctions->SetDepthTest(true);
		apFunctions->SetMatrix(&mmtxTransform);
		apFunctions->GetLowLevelGfx()->DrawLine(0, cVector3f(1,0,0), cColor(1,0,0,1));
		apFunctions->GetLowLevelGfx()->DrawLine(0, cVector3f(0,1,0), cColor(0,1,0,1));
		apFunctions->GetLowLevelGfx()->DrawLine(0, cVector3f(0,0,1), cColor(0,0,1,1));
		
		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(-0.5f, 0.5f, cColor(1));*/
	}
}

//-----------------------------------------------------------------------------------------

cEditorWindowEntityEditBox* cEntityWrapperDecal::CreateEditBox(cEditorEditModeSelect* apEditMode)
{
	return hplNew(cEditorWindowEntityEditBoxDecal,(apEditMode, this));
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::SetAffectStatic(bool abX)
{
	if(mbAffectsStaticObjects==abX)
		return;
	mbAffectsStaticObjects = abX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

void cEntityWrapperDecal::SetAffectPrimitive(bool abX)
{
	if(mbAffectsPrimitives==abX)
		return;
	mbAffectsPrimitives = abX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

void cEntityWrapperDecal::SetAffectEntity(bool abX)
{
	if(mbAffectsEntities==abX)
		return;
	mbAffectsEntities = abX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::SetMaxTriangles(int alX)
{
	if(alX!=-1 && mlMaxTriangles==alX)
		return;
	mlMaxTriangles = alX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::SetMaterial(const tString& asX)
{
	if(msMaterial==asX)
		return;
	msMaterial = asX;

	mbDecalUpdated = true;
	mbMaterialUpdated = true;
	mbEntityUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::SetOffset(float afX)
{
	if(mfOffset==afX)
		return;

	mfOffset = afX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::SetColor(const cColor& aX)
{
	if(mColor==aX)
		return;

	mColor = aX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::SetUVSubDivisions(const cVector2l& avX)
{
	if(mvSubDivisions==avX)
		return;

	mvSubDivisions = avX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::SetCurrentSubDiv(int alX)
{
	if(mlCurrentSubDiv==alX)
		return;

	mlCurrentSubDiv = alX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::UpdateDecal()
{
	if(mpEngineEntity==NULL)
	{
		if(CreateEngineEntity()==false)
			return;

		mbDecalUpdated=false;
		mbGeometryUpdated=false;
	}

	if(cEntityWrapperTypeDecal::IsForcingUpdate()==false && mbDecalUpdated==false)
		return;
	mbDecalUpdated = false;

	if(cEntityWrapperTypeDecal::IsForcingUpdate() || mbGeometryUpdated)
	{
		cEngineEntityGeneratedMesh* pMeshEnt = (cEngineEntityGeneratedMesh*)mpEngineEntity;
		if(pMeshEnt->ReCreate(CreateDecalMesh())==false)
		{
			hplDelete(mpEngineEntity);
			mpEngineEntity = NULL;

			return;
		}
	}


	mbGeometryUpdated = false;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::SetAbsPosition(const cVector3f& avPosition)
{
	iEntityWrapper::SetAbsPosition(avPosition);
	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::SetAbsRotation(const cVector3f& avRotation)
{
	iEntityWrapper::SetAbsRotation(avRotation);
	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::SetAbsScale(const cVector3f& avScale, int alAxis)
{
	iEntityWrapper::SetAbsScale(avScale);
	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::UpdateEntity()
{
	if(mbDeployed==false) return;
	
	UpdateMatrix();
	UpdateDecal();

	mpIcon->SetVisible(mpEngineEntity==NULL);
}

//-----------------------------------------------------------------------------------------

iVertexBuffer* cEntityWrapperDecal::BuildDecalVertexBuffer(cWorld* apWorld, cDecalCreator* apCreator,
													  const cVector3f& avPos, const cVector3f& avSize, float afOffset,
													  const cVector3f& avRight, const cVector3f& avUp, const cVector3f& avFwd,
													  const tString& asMaterial, const cColor& aCol,
													  const cVector2l& avSubDivs,int alSubDiv, int alMaxTris,
													  bool abAffectStaticObject, bool abAffectPrimitive, bool abAffectEntity)
{
	/////////////////////////////////////////
	// Set up decal parameters
	apCreator->SetDecalPosition(avPos);
	apCreator->SetDecalSize(avSize);
	apCreator->SetDecalOffset(afOffset);
	apCreator->SetDecalRight(avRight,false);
	apCreator->SetDecalUp(avUp,false);
	apCreator->SetDecalForward(avFwd, false);

	// Debug
	//Log("[Decal] Pos:%s Size:%s Right:%s Up:%s Forward:%s\n", avPos.ToFileString().c_str(), avSize.ToFileString().c_str(),
	//														  avRight.ToFileString().c_str(), avUp.ToFileString().c_str(), avFwd.ToFileString().c_str());

	apCreator->SetMaterial(asMaterial);
	apCreator->SetColor(aCol);
	apCreator->SetUVSubDivisions(avSubDivs);
	apCreator->SetCurrentSubDiv(alSubDiv);
	apCreator->SetMaxTrianglesPerDecal( (alMaxTris>=0)? alMaxTris : 
														cEntityWrapperTypeDecal::GetGlobalMaxTriangles() );

	if(apCreator->IsUpdated()==false)
		return apCreator->GetVB();
	
	apCreator->ClearMeshes();

	/////////////////////////////
	// Get Containers
    iRenderableContainer *pContainers[2] ={
		apWorld->GetRenderableContainer(eWorldContainerType_Dynamic),
		apWorld->GetRenderableContainer(eWorldContainerType_Static),
	};

	/////////////////////////////
	// Iterate nodes to get geometry
	for(int i=0; i<2; ++i)
	{
		pContainers[i]->UpdateBeforeRendering();
		IterateRenderableNode(pContainers[i]->GetRoot(), apCreator, 
								abAffectStaticObject, abAffectPrimitive, abAffectEntity);
	}

	apCreator->CanCreateDecal();
	
	return apCreator->GetVB();
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

cEntityIcon* cEntityWrapperDecal::CreateIcon()
{
	return hplNew(cEntityIcon,(this, "Decal"));
}

//-----------------------------------------------------------------------------------------

cMesh* cEntityWrapperDecal::CreateDecalMesh()
{
	if(mbDeployed==false)
		return NULL;

	cMatrixf mtxOrientation = mmtxRotate.GetTranspose();

	iEditorWorld* pWorld= GetEditorWorld();
	cDecalCreator* pCreator = pWorld->GetEditor()->GetEngine()->GetGraphics()->GetDecalCreator();

	///////////////////////////////////////////////////////////////////////////////////
	// If this entity comes from loaded data, the mesh object in data will not be null,
	// return it, else, just build new mesh data.
	if(mpData)
	{
		cMesh* pDecalMesh = ((cEntityWrapperDataDecal*)mpData)->GetMesh();
		if(pDecalMesh!=NULL)
			return pDecalMesh;
	}
	//////////////////////////////////////////
	// Check if current parameters are valid
	iVertexBuffer* pVB = BuildDecalVertexBuffer(pWorld->GetWorld(), pCreator, 
							mvPosition, mvScale, mfOffset, 
							mtxOrientation.GetRight(), mtxOrientation.GetUp(), mtxOrientation.GetForward(),
							msMaterial, mColor,	
							mvSubDivisions, mlCurrentSubDiv, 
							mlMaxTriangles,
							mbAffectsStaticObjects, mbAffectsPrimitives, mbAffectsEntities);

	if(pVB && pVB->GetVertexNum()!=0)
		return pCreator->CreateDecalMesh();

	return NULL;
}

//-----------------------------------------------------------------------------------------

iEngineEntity* cEntityWrapperDecal::CreateSpecificEngineEntity()
{
	return hplNew(cEngineEntityGeneratedMesh,(this, CreateDecalMesh()));
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

void cEntityWrapperDecal::IterateRenderableNode(iRenderableContainerNode *apNode, cDecalCreator* apCreator,
										bool abAffectStaticObject, bool abAffectPrimitive, bool abAffectEntity)
{
	apNode->UpdateBeforeUse();

	if(apNode->GetParent()!=NULL)
	{
		cBoundingVolume* pDecalBV = apCreator->GetDecalBoundingVolume();
		if(cMath::CheckAABBIntersection(apNode->GetMin(), apNode->GetMax(), pDecalBV->GetMin(), pDecalBV->GetMax())==false) return;
	}

	/////////////////////////////
	//Iterate objects
	if(apNode->HasObjects())
	{
		tRenderableListIt it = apNode->GetObjectList()->begin();
		for(; it != apNode->GetObjectList()->end(); ++it)
		{
			//////////////////////////////////////////
			// Check if mesh object
			iRenderable *pObject = *it;
			if(pObject->GetRenderType() != eRenderableType_SubMesh) continue;

			cSubMeshEntity* pSubMesh = (cSubMeshEntity*)pObject;
			iEntityWrapper* pEnt = (iEntityWrapper*) pSubMesh->GetUserData();
			if(pEnt==NULL ||
				pEnt->IsAffectedByDecal(abAffectStaticObject, abAffectPrimitive, abAffectEntity)==false)
				continue;

			apCreator->AddSubMesh(pSubMesh);
		}
	}

	////////////////////////
	//Iterate children
	if(apNode->HasChildNodes())
	{
		tRenderableContainerNodeListIt childIt = apNode->GetChildNodeList()->begin();
		for(; childIt != apNode->GetChildNodeList()->end(); ++childIt)
		{
			iRenderableContainerNode *pChildNode = *childIt;
			IterateRenderableNode(pChildNode, apCreator, 
									abAffectStaticObject, abAffectPrimitive, abAffectEntity);
		}
	}
}

//-----------------------------------------------------------------------------------------

