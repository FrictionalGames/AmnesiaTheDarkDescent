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

#include "../Common/EdObjDecal.h"

#include "../Common/Editor.h"

#include "../Common/EdScnWorld.h"
#include "../Common/EdWindowViewport.h"
#include "../Common/EdEditPaneDecal.h"
//#include "EditorHelper.h"
#include "../Common/EdEditModeDecals.h"

#include "../Common/EdObjectIcon.h"

#include "../Common/EdEngineObject.h"

#include "../Common/EdSurfacePicker.h"
#include "../Common/EdUIPicker.h"

#include <algorithm>

//-----------------------------------------------------------------------------------------

////////////////////////////////
// Static global decal variables
//  Min and max for decal offset. Actual offset value should pick a random value between these two.
float cTypeDecal::mfDecalOffsetMin = 0.005f;
float cTypeDecal::mfDecalOffsetMax = 0.01f;

//	Global max triangles per decal.
int cTypeDecal::mlGlobalMaxTriangles = 300;
bool cTypeDecal::mbForcingUpdate = false;

//------------------------------------------------------------------------------------------

cFileIndex* cTypeDecal::mpIndex = NULL;

//-----------------------------------------------------------------------------------------

cTypeDecal::cTypeDecal() : iEdScnObjType(_W("Decal"), "Decal")
{
	AddInt(eDecalInt_FileIndex, "MaterialIndex", ePropStep_PreCreate);
	AddString(eDecalStr_Material, "Material", "", ePropStep_PreCreate, false);
	AddInt(eDecalInt_CurrentSubDiv, "CurrentSubDiv", 0, ePropStep_PreCreate);
	AddInt(eDecalInt_MaxTris, "MaxTriangles", -1, ePropStep_PreCreate);

	AddFloat(eDecalFloat_Offset, "Offset", 0, ePropStep_PreCreate);
	AddVec2f(eDecalVec2f_SubDivs, "SubDiv", 1, ePropStep_PreCreate);
	AddColor(eDecalCol_Color, "Color", cColor(1), ePropStep_PreCreate);

	AddBool(eDecalBool_AffectStatic, "OnStatic", true, ePropStep_PreCreate);
	AddBool(eDecalBool_AffectPrimitive, "OnPrimitive", true, ePropStep_PreCreate);
	AddBool(eDecalBool_AffectEntity, "OnEntity", true, ePropStep_PreCreate);
}

//-----------------------------------------------------------------------------------------

bool cTypeDecal::Init()
{
	bool bInit = iEdScnObjType::Init();

	if(mpIndex==NULL)
		mpIndex = mpWorld->CreateFileIndex("Decals");

	return bInit;
}

//-----------------------------------------------------------------------------------------

iEdObjectData* cTypeDecal::CreateTypeSpecificData()
{
	return hplNew(cEdObjDecalData,(this));
}

cEdObjDecalData::cEdObjDecalData(iEdObjectType* apType) : iEdScnObjData(apType)
{
	mpDecal = NULL;
	mpMesh = NULL;
}

//-----------------------------------------------------------------------------------------

void cEdObjDecalData::CopyFromObject(iEdObject* apObject)
{
	iEdScnObjData::CopyFromObject(apObject);
	mpDecal = static_cast<cEdObjDecal*>(apObject);

	mmapFilter = mpDecal->GetAffectedTypes();

	cIndexedFile* pFile = mpDecal->GetMatFile();

	SetInt(eDecalInt_FileIndex, pFile? pFile->mlID : -1);
}

//-----------------------------------------------------------------------------------------

void cEdObjDecalData::CopyToObject(iEdObject* apObject, ePropStep aOrder)
{
	iEdScnObjData::CopyToObject(apObject, aOrder);

	cEdObjDecal* pDecal = static_cast<cEdObjDecal*>(apObject);
	pDecal->SetAffectedTypes(mmapFilter);
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecalData::LoadSpecificData(cXmlElement* apElement)
{
	if(iEdScnObjData::LoadSpecificData(apElement)==false)
		return false;

	iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(mpType->GetWorld());
	cEdSurfacePicker* pPicker = pWorld->GetSurfacePicker();

	mmapFilter = pPicker->GetAffectedTypes();

	tTypeFilterMapIt it = mmapFilter.begin();
	for(;it!=mmapFilter.end();++it)
	{
		iEdObjectType* pType = it->first;
		tWStringVec vTypeName;
		cString::GetStringVecW(pType->GetName(), vTypeName);

		tString& sTypeName = cString::To8Char(vTypeName[0]);

		bool bAffected = apElement->GetAttributeBool("On"+sTypeName, true);

		it->second = bAffected;
	}


	////////////////////////////////////////////
	// Load File index
	int lFileIndex = GetInt(eDecalInt_FileIndex);
	if(lFileIndex==-1)
	{
		tString sMat = GetString(eDecalStr_Material);
		cIndexedFile* pFile = cTypeDecal::mpIndex->CreateIndexedFile(sMat);

		SetInt(eDecalInt_FileIndex, pFile->mlID);
	}

	////////////////////////////////////////////
	// Link to file by index
	cIndexedFile* pFile = cTypeDecal::mpIndex->GetIndexedFile(lFileIndex);
	if(pFile==NULL)
	{
		//pWorld->SetLoadErrorMessage(_W("File index out of bounds!"));
		return false;
	}

	SetString(eDecalStr_Material, pFile->msRelativePath);

	////////////////////////////////////////////
	// Fix decal offset inside boundaries
	float fOffset = GetFloat(eDecalFloat_Offset);
	float fOffsetMin = cTypeDecal::GetDecalOffsetMin();
	float fOffsetMax = cTypeDecal::GetDecalOffsetMax();
	if(fOffset < fOffsetMin || 
		fOffset > fOffsetMax)
	{
		fOffset = cMath::RandRectf(fOffsetMin, fOffsetMax);

		SetFloat(eDecalFloat_Offset, fOffset);
	}

	cXmlElement* pMeshDataElem = apElement->GetFirstElement("DecalMesh");
	if(pMeshDataElem)
	{
		cEngine* pEng = pWorld->GetEditor()->GetEngine();
		mpMesh = cEngineFileLoading::LoadDecalMeshHelper(pMeshDataElem, 
															pEng->GetGraphics(), pEng->GetResources(), 
															cString::To8Char(GetName()), pFile->msRelativePath, GetColor(eDecalCol_Color)); 
	}
	else
		mpMesh = NULL;

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecalData::SaveSpecificData(cXmlElement* apElement)
{
	if(iEdScnObjData::SaveSpecificData(apElement)==false || mpDecal==NULL)
		return false;

	apElement->SetAttributeInt("MaterialIndex", mpDecal->GetMatFile()->mlID);
	tTypeFilterMapIt it = mmapFilter.begin();
	for(;it!=mmapFilter.end();++it)
	{
		iEdObjectType* pType = it->first;
		tWStringVec vTypeName;
		cString::GetStringVecW(pType->GetName(), vTypeName);

		tString& sTypeName = cString::To8Char(vTypeName[0]);

		apElement->SetAttributeBool("On"+sTypeName, it->second);
	}

	cEngineGeneratedMesh* pDecalMesh = static_cast<cEngineGeneratedMesh*>(mpDecal->GetEngObject());
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

iEdObject* cEdObjDecalData::CreateTypeSpecificObject()
{
	/*
	/////////////////////////////////////////////////////////////////////////////////////////
	// Check so the File Index is still valid ! (can become invalid after undoing a delete
	iEdWorld* pWorld = mpType->GetWorld();

	int lFileIndex = GetInt(eDecalInt_FileIndex);
	const tString& sCurrentFile = GetString(eDecalStr_Material);
	tWString sPath = pWorld->GetEditor()->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(sCurrentFile);
	int lFileIndexForCurrentFile = pWorld->AddFilenameToIndex("Decals", cString::To8Char(sPath));

	// the If first index and the one we just retrieved are different, update first
	if(lFileIndex!=lFileIndexForCurrentFile)
		SetInt(eDecalInt_FileIndex, lFileIndexForCurrentFile);
		*/

	return hplNew(cEdObjDecal,(this));
}

//-----------------------------------------------------------------------------------------

void cEdObjDecalData::SaveGeometryDataToString(tString& asOutput, int alNumElements, float* apData)
{
	for(int i=0;i<alNumElements;++i)
		asOutput += cString::ToString(apData[i], 3, true) + " ";
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

cEdObjDecal::cEdObjDecal(iEdObjectData* apData) : iEdScnObject(apData)
{
	mbDecalUpdated=true;
	mbGeometryUpdated=true;
	mbMaterialUpdated=true;

	mColor = 1;
	mvSubDivisions = 1;

	mpMatFile = NULL;
	
	/*
	mbAffectsStaticObjects = true;
	mbAffectsEntities = true;
	mbAffectsPrimitives = true;
	*/

	mbDeployed = false;
}

//-----------------------------------------------------------------------------------------

cEdObjDecal::~cEdObjDecal()
{
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecal::SetProperty(int alPropID, const int alX)
{
	switch(alPropID)
	{
	case eDecalInt_FileIndex:
		//SetFileIndex(alX);
		break;
	case eDecalInt_CurrentSubDiv:
		SetCurrentSubDiv(alX);
		break;
	case eDecalInt_MaxTris:
		SetMaxTriangles(alX);
		break;
	default:
		return iEdObject::SetProperty(alPropID, alX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecal::SetProperty(int alPropID, const float afX)
{
	switch(alPropID)
	{
	case eDecalFloat_Offset:
		SetOffset(afX);
		break;
	default:
		return iEdObject::SetProperty(alPropID, afX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecal::SetProperty(int alPropID, const bool abX)
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
		return iEdObject::SetProperty(alPropID, abX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecal::SetProperty(int alPropID, const tString& asX)
{
	switch(alPropID)
	{
	case eDecalStr_Material:
		SetMaterial(asX);
		break;
	default:
		return iEdObject::SetProperty(alPropID, asX);
	}
	
	return true;
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecal::SetProperty(int alPropID, const cVector2f& avX)
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
		return iEdObject::SetProperty(alPropID, avX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecal::SetProperty(int alPropID, const cColor& aX)
{
	switch(alPropID)
	{
	case eDecalCol_Color:
		SetColor(aX);
		break;
	default:
		return iEdObject::SetProperty(alPropID, aX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecal::GetProperty(int alPropID, int& alX)
{
	switch(alPropID)
	{
	case eDecalInt_FileIndex:
		//alX = GetFileIndex();
		break;
	case eDecalInt_CurrentSubDiv:
		alX = GetCurrentSubDiv();
		break;
	case eDecalInt_MaxTris:
		alX = GetMaxTriangles();
		break;
	default:
		return iEdObject::GetProperty(alPropID, alX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecal::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case eDecalFloat_Offset:
		afX = GetOffset();
		break;
	default:
		return iEdObject::GetProperty(alPropID, afX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecal::GetProperty(int alPropID, bool& abX)
{
	return iEdScnObject::GetProperty(alPropID, abX);
	/*
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
		return iEdObject::GetProperty(alPropID, abX);
	}
	*/

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecal::GetProperty(int alPropID, tString& asX)
{
	switch(alPropID)
	{
	case eDecalStr_Material:
		asX = GetMaterial();
		break;
	default:
		return iEdObject::GetProperty(alPropID, asX);
	}
	
	return true;
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecal::GetProperty(int alPropID, cVector2f& avX)
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
		return iEdObject::GetProperty(alPropID, avX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecal::GetProperty(int alPropID, cColor& aX)
{
	switch(alPropID)
	{
	case eDecalCol_Color:
		aX = GetColor();
		break;
	default:
		return iEdObject::SetProperty(alPropID, aX);
	}

	return true;
}

//-----------------------------------------------------------------------------------------

void cEdObjDecal::OnPostDeployAll(bool abX)
{
	mbDeployed = true;

	OnUpdate(false);

	iEdScnObject::OnPostDeployAll(abX);
}


//-----------------------------------------------------------------------------------------

void cEdObjDecal::OnDraw(const cModuleDrawData& aData)
{
	if(aData.mStep==eEdDrawStep_PostTrans)
		return;

	if(mpIcon)
		mpIcon->DrawIcon(aData.mpViewport, aData.mpFunctions, mbSelected, IsEnabled(), cColor(0.8f,1));
	if(mpEngObject) 
		mpEngObject->Draw(aData.mpViewport, aData.mpFunctions, mbSelected, true, cColor(0.8f,0.25f));
	//iEdScnObject::OnDraw(aData);
	if(mbSelected)
	{
		//cDecalCreator* pCreator = GetEdWorld()->GetEditor()->GetEngine()->GetGraphics()->GetDecalCreator();
		//pCreator->DrawDebug(apFunctions, true, true);
		cMatrixf mtxOrientation = GetRotationMatrix().GetTranspose();
		
		cMatrixf mtxTransform = GetWorldMatrix();
		aData.mpFunctions->SetTextureRange(NULL,0);
		aData.mpFunctions->SetDepthTest(true);
		aData.mpFunctions->SetMatrix(&mtxTransform);

		//aData.mpFunctions->GetLowLevelGfx()->DrawLine(0, mtxOrientation.GetRight()*mvScale.x*0.5f, cColor(1,0,0,1));
		//aData.mpFunctions->GetLowLevelGfx()->DrawLine(0, mtxOrientation.GetUp()*mvScale.y*0.5f, cColor(0,1,0,1));
		//aData.mpFunctions->GetLowLevelGfx()->DrawLine(0, mtxOrientation.GetForward()*mvScale.z*0.5f, cColor(0,0,1,1));

		/*apFunctions->SetTextureRange(NULL,0);
		apFunctions->SetDepthTest(true);
		apFunctions->SetMatrix(&mmtxTransform);
		apFunctions->GetLowLevelGfx()->DrawLine(0, cVector3f(1,0,0), cColor(1,0,0,1));
		apFunctions->GetLowLevelGfx()->DrawLine(0, cVector3f(0,1,0), cColor(0,1,0,1));
		apFunctions->GetLowLevelGfx()->DrawLine(0, cVector3f(0,0,1), cColor(0,0,1,1));
		*/
		
		aData.mpFunctions->GetLowLevelGfx()->DrawBoxMinMax(-0.5f, 0.5f, cColor(1));
	}

}


//-----------------------------------------------------------------------------------------

iEdEditPane* cEdObjDecal::CreateEditPane()
{
	return hplNew(cEdEditPaneDecal,(this));
}

//-----------------------------------------------------------------------------------------

void cEdObjDecal::SetAffectStatic(bool abX)
{
	/*
	if(mbAffectsStaticObjects==abX)
		return;
	mbAffectsStaticObjects = abX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
	*/
}

void cEdObjDecal::SetAffectPrimitive(bool abX)
{
	/*
	if(mbAffectsPrimitives==abX)
		return;
	mbAffectsPrimitives = abX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
	*/
}

void cEdObjDecal::SetAffectEntity(bool abX)
{
	/*
	if(mbAffectsEntities==abX)
		return;
	mbAffectsEntities = abX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
	*/
}

//-----------------------------------------------------------------------------------------

void cEdObjDecal::SetMaxTriangles(int alX)
{
	if(alX!=-1 && mlMaxTriangles==alX)
		return;
	mlMaxTriangles = alX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEdObjDecal::SetMatFile(cIndexedFile* apRes)
{
	if(mpMatFile==apRes)
		return;

	if(mpMatFile) mpMatFile->Release();

	mpMatFile = apRes;

	if(mpMatFile) mpMatFile->Assign();
}

void cEdObjDecal::SetMaterial(const tString& asX)
{
	if(mpMatFile && mpMatFile->msRelativePath==asX)
		return;

	cIndexedFile* pTemp = cTypeDecal::mpIndex->CreateIndexedFile(asX);

	SetMatFile(pTemp);

	if(mpEngObject)
	{
		hplDelete(mpEngObject);
		mpEngObject=NULL;
	}

	mbDecalUpdated = true;
	mbMaterialUpdated = true;

	SetUpEngineObject();

	SetUpdated();
}

//-----------------------------------------------------------------------------------------

void cEdObjDecal::SetOffset(float afX)
{
	if(mfOffset==afX)
		return;

	mfOffset = afX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEdObjDecal::SetColor(const cColor& aX)
{
	if(mColor==aX)
		return;

	mColor = aX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEdObjDecal::SetUVSubDivisions(const cVector2l& avX)
{
	if(mvSubDivisions==avX)
		return;

	mvSubDivisions = avX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEdObjDecal::SetCurrentSubDiv(int alX)
{
	if(mlCurrentSubDiv==alX)
		return;

	mlCurrentSubDiv = alX;

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEdObjDecal::UpdateDecal()
{
	if(mpEngObject==NULL)
	{
		if(SetUpEngineObject()==false)
			return;

		mbDecalUpdated=false;
		mbGeometryUpdated=false;
	}

	if(cTypeDecal::IsForcingUpdate()==false && mbDecalUpdated==false)
		return;

	mbDecalUpdated = false;

	if(cTypeDecal::IsForcingUpdate() || mbGeometryUpdated)
	{
		cEngineGeneratedMesh* pMeshEnt = static_cast<cEngineGeneratedMesh*>(mpEngObject);
		if(pMeshEnt->ReCreate(CreateDecalMesh())==false)
		{
			hplDelete(mpEngObject);
			mpEngObject = NULL;

			return;
		}
	}
	
	mbGeometryUpdated = false;
}

//-----------------------------------------------------------------------------------------

void cEdObjDecal::SetTranslation(const cVector3f& avPosition)
{
	iEdScnObject::SetTranslation(avPosition);

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEdObjDecal::SetRotation(const cVector3f& avRotation)
{
	iEdScnObject::SetRotation(avRotation);

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

void cEdObjDecal::SetScale(const cVector3f& avScale)
{
	iEdScnObject::SetScale(avScale);

	mbDecalUpdated = true;
	mbGeometryUpdated = true;
}

//-----------------------------------------------------------------------------------------

bool cEdObjDecal::OnUpdate(bool abForce)
{
	if(mbDeployed==false) return false;

	UpdateDecal();

	mpIcon->SetVisible(mpEngObject==NULL);

	return true;
}

//-----------------------------------------------------------------------------------------

iVertexBuffer* cEdObjDecal::BuildDecalVertexBuffer(cWorld* apWorld, cDecalCreator* apCreator,
													  const cVector3f& avPos, const cVector3f& avSize, float afOffset,
													  const cVector3f& avRight, const cVector3f& avUp, const cVector3f& avFwd,
													  const tString& asMaterial, const cColor& aCol,
													  const cVector2l& avSubDivs,int alSubDiv, int alMaxTris,
													  cEdSurfacePicker* apPicker, const tTypeFilterMap& amapTypeFilter)
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
														cTypeDecal::GetGlobalMaxTriangles() );

	if(apCreator->IsUpdated()==false)
		return apCreator->GetVB();
	
	apCreator->ClearMeshes();

	/////////////////////////////
	// Get Containers
    iRenderableContainer *pContainers[2] ={
		apWorld->GetRenderableContainer(eWorldContainerType_Dynamic),
		apWorld->GetRenderableContainer(eWorldContainerType_Static),
	};

	tTypeFilterMap mapPrevAffected = apPicker->GetAffectedTypes();
	apPicker->SetAffectedTypes(amapTypeFilter);

	/////////////////////////////
	// Iterate nodes to get geometry
	for(int i=0; i<2; ++i)
	{
		pContainers[i]->UpdateBeforeRendering();
		IterateRenderableNode(pContainers[i]->GetRoot(), apCreator, 
								apPicker);
	}

	apCreator->CanCreateDecal();

	apPicker->SetAffectedTypes(mapPrevAffected);
	
	return apCreator->GetVB();
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

cObjectIcon* cEdObjDecal::CreateIcon()
{
	return hplNew(cObjectIcon,(this, "Decal"));
}

//-----------------------------------------------------------------------------------------

cMesh* cEdObjDecal::CreateDecalMesh()
{
	if(mbDeployed==false)
		return NULL;

	cMatrixf mtxOrientation = GetRotationMatrix().GetTranspose();

	
	iEdScnWorld* pWorld = static_cast<iEdScnWorld*>(GetWorld());
	cDecalCreator* pCreator = pWorld->GetEditor()->GetEngine()->GetGraphics()->GetDecalCreator();

	///////////////////////////////////////////////////////////////////////////////////
	// If this entity comes from loaded data, the mesh object in data will not be null,
	// return it, else, just build new mesh data.
	if(mpData)
	{
		cEdObjDecalData* pData = static_cast<cEdObjDecalData*>(mpData);
		cMesh* pDecalMesh = pData->GetMesh();
		if(pDecalMesh!=NULL)
			return pDecalMesh;
	}
	//////////////////////////////////////////
	// Check if current parameters are valid
	iVertexBuffer* pVB = BuildDecalVertexBuffer(pWorld->GetEngWorld(), pCreator, 
							mvTranslation, mvScale, mfOffset, 
							mtxOrientation.GetRight(), mtxOrientation.GetUp(), mtxOrientation.GetForward(),
							mpMatFile->msRelativePath, mColor,	
							mvSubDivisions, mlCurrentSubDiv, 
							mlMaxTriangles,
							pWorld->GetSurfacePicker(), mmapAffectedTypes);

	if(pVB && pVB->GetVertexNum()!=0)
		return pCreator->CreateDecalMesh();

	return NULL;
}

//-----------------------------------------------------------------------------------------

iEngineObject* cEdObjDecal::CreateEngineObject()
{
	return hplNew(cEngineGeneratedMesh,(this, CreateDecalMesh()));
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

void cEdObjDecal::IterateRenderableNode(iRenderableContainerNode *apNode, cDecalCreator* apCreator,
										cEdSurfacePicker* apPicker)
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
		const tTypeFilterMap& mapAffectedTypes = apPicker->GetAffectedTypes();
		tRenderableListIt it = apNode->GetObjectList()->begin();
		for(; it != apNode->GetObjectList()->end(); ++it)
		{
			//////////////////////////////////////////
			// Check if mesh object
			iRenderable *pObject = *it;
			if(pObject->GetRenderType() != eRenderableType_SubMesh) continue;

			cSubMeshEntity* pSubMesh = static_cast<cSubMeshEntity*>(pObject);
			iEdScnObject* pObj = static_cast<iEdScnObject*>(pSubMesh->GetRenderableUserData());
			if(pObj==NULL || pObj->IsAffectedByDecal(apPicker)==false)
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
									apPicker);
		}
	}
}

//-----------------------------------------------------------------------------------------

