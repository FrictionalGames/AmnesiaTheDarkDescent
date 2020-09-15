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

#include "../Common/EdObjPrimitivePlane.h"

#include "../Common/EdWorld.h"
#include "../Common/Editor.h"
#include "../Common/EdWindowViewport.h"
//#include "EditorHelper.h"

#include "../Common/EdEngineObject.h"


//---------------------------------------------------------------------------

cPrimitiveMeshCreatorPlane::cPrimitiveMeshCreatorPlane() : iPrimitiveMeshCreator()
{
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::OnViewportMouseDown(const cViewportClick& aClick)
{
	mvCorners.clear();

	mvCorners.push_back(aClick.mpViewport->GetMouseWorldPosition());
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::OnViewportMouseUp(const cViewportClick& aClick)
{
	mvCorners.push_back(aClick.mpViewport->GetMouseWorldPosition());
	//StoreMousePosition();
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::StoreMousePosition()
{
//	cVector3f &vMousePos = mpEditMode->GetEditor()->GetPosOnGridFromMousePos();
//	mvCorners.push_back(aData.mpViewport->GetMouseWorldPosition());
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::Draw(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions)
{
	if(mvCorners.size()==1)
	{
		apFunctions->SetProgram(NULL);
		apFunctions->SetTextureRange(NULL,0);
	
		apFunctions->SetMatrix(NULL);
		
		cVector3f vFirstCorner = mvCorners[0];
		cVector3f vMousePos = apViewport->GetMouseWorldPosition();

		apFunctions->GetLowLevelGfx()->DrawBoxMinMax(vFirstCorner, vMousePos,cColor(1,1));
		apFunctions->GetLowLevelGfx()->DrawSphere((vFirstCorner + vMousePos)*0.5f, 0.1f, cColor(1,0,0,1));
	}
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::SetUpData(iEdWindow* apWindow, iEdObjectData* apData)
{
	iPrimitiveMeshCreator::SetUpData(apWindow, apData);

	SortCorners();

	cVector3f vEndCorner = mvCorners[1]-mvCorners[0];
	apData->SetVec3f(ePrimitivePlaneVec3f_StartCorner, 0);
	apData->SetVec3f(ePrimitivePlaneVec3f_EndCorner, vEndCorner);
	apData->SetVec3f(eScnVec3f_Translation, mvCorners[0]);

	for(int i=0;i<3;++i)
		if(vEndCorner.v[i]==0) vEndCorner.v[i] = 1;
	apData->SetVec3f(eScnVec3f_Scale, vEndCorner);
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::SortCorners()
{
	////////////////////////////////////////////////
	// Really strange algo to determine the corner 
	// which is "most negative" (closest to (-inf,-inf) in the plane of creation)
	cVector3f vCorner1 = mvCorners[0];
	cVector3f vCorner2 = mvCorners[1];
	cVector3f vDiff = vCorner2 - vCorner1;
	int lFirstNegativeIndex = -1;
	int lFirstPositiveIndex = -1;
	int lNumNegativeCoords = 0;
	for(int i=0;i<3;++i)
	{
		if(vDiff.v[i]<0) 
		{
			lNumNegativeCoords++;
			if(lFirstNegativeIndex==-1) 
				lFirstNegativeIndex = i;
		}
		else if(vDiff.v[i]>0)
		{
			if(lFirstPositiveIndex==-1) 
				lFirstPositiveIndex = i;
		}
	}

	if(lNumNegativeCoords==0)
	{
	}
	else if(lNumNegativeCoords==2)
	{
		//////////////////////////////////////
		// Store the corners backwards
		mvCorners.clear();
		mvCorners.push_back(vCorner2);
		mvCorners.push_back(vCorner1);
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////////////////////////
		// Bake the corners to store, since we are not that interested in the ones given by the user
		float fWidth = cMath::Abs(vCorner2.v[lFirstNegativeIndex]-vCorner1.v[lFirstNegativeIndex]);
		float fHeight = cMath::Abs(vCorner2.v[lFirstPositiveIndex]-vCorner1.v[lFirstPositiveIndex]);
        
		cVector3f vAdd1 = cVector3f(0);
		cVector3f vAdd2 = cVector3f(0);
		vAdd1.v[lFirstNegativeIndex] = fWidth;
		vAdd2.v[lFirstPositiveIndex] = fHeight;
		
		mvCorners.clear();
		mvCorners.push_back(vCorner1-vAdd1);
		mvCorners.push_back(vCorner1+vAdd2);
	}
}

//---------------------------------------------------------------------------

void cPrimitiveMeshCreatorPlane::Reset()
{
	mvCorners.clear();
}

//---------------------------------------------------------------------------

bool cPrimitiveMeshCreatorPlane::IsDoneCreating()
{
	if(mvCorners.size()!=2)
		return false;

    int lNumEqualCoords=0;
	for(int i=0;i<3;++i)
	{
		if(mvCorners[0].v[i]==mvCorners[1].v[i]) lNumEqualCoords++;
	}

	return lNumEqualCoords<2;
}

//---------------------------------------------------------------------------

cTypePlane::cTypePlane() : iPrimitiveSubType(_W("Plane"), "Plane", hplNew(cPrimitiveMeshCreatorPlane,()))
{
	AddVec3f(ePrimitivePlaneVec3f_StartCorner, "StartCorner", 0, ePropStep_PreCreate);
	AddVec3f(ePrimitivePlaneVec3f_EndCorner, "EndCorner",0, ePropStep_PreCreate);

	AddVec3f(ePrimitivePlaneVec3f_TileAmount, "TileAmount", 1);
	AddVec3f(ePrimitivePlaneVec3f_TileOffset, "TileOffset", 0);
	AddFloat(ePrimitivePlaneFloat_TextureAngle, "TextureAngle", 0);
	AddBool(ePrimitivePlaneBool_AlignToWorld, "AlignToWorldCoords", false);
}

//---------------------------------------------------------------------------

iEdObjectData* cTypePlane::CreateTypeSpecificData()
{
	return hplNew(cEdObjPlaneData,(this));
}

//---------------------------------------------------------------------------

cEdObjPlaneData::cEdObjPlaneData(iEdObjectType* apType) : iEdScnObjData(apType)
{
}

//---------------------------------------------------------------------------

void cEdObjPlaneData::CopyFromObject(iEdObject* apObj)
{
	iEdScnObjData::CopyFromObject(apObj);
	cEdObjPlane* pPlane = (cEdObjPlane*)apObj;
	mlNormalAxisIndex = pPlane->GetNormalAxisIndex();
	mvUVCorners = pPlane->GetUVCorners();
}

//---------------------------------------------------------------------------

bool cEdObjPlaneData::LoadSpecificData(cXmlElement* apElement)
{
	if(iEdScnObjData::LoadSpecificData(apElement)==false)
		return false;

	cVector3f vEnd = (GetVec3f(ePrimitivePlaneVec3f_EndCorner)-GetVec3f(ePrimitivePlaneVec3f_StartCorner))*GetVec3f(eScnVec3f_Scale);
	SetVec3f(ePrimitivePlaneVec3f_StartCorner, 0);
	SetVec3f(ePrimitivePlaneVec3f_EndCorner, vEnd);

	cVector3f vScale = GetVec3f(eScnVec3f_Scale);
	for(int i=0;i<3;++i)
	{
		if(vEnd.v[i]==0) vScale.v[i] = 1;
		else
			vScale.v[i] = vEnd.v[i];
	}

	SetVec3f(eScnVec3f_Scale, vScale);

	return true;
}

//---------------------------------------------------------------------------

bool cEdObjPlaneData::SaveSpecificData(cXmlElement* apElement)
{
	if(iEdScnObjData::SaveSpecificData(apElement)==false)
		return false;

	cVector3f vScale = GetVec3f(eScnVec3f_Scale);
	const cVector3f& vStartCorner = GetVec3f(ePrimitivePlaneVec3f_StartCorner);
	cVector3f vEndCorner;
	//Log("Saving plane\n");
	//Log("\tNormal axis index: %d\n", mlNormalAxisIndex);
	for(int i=0;i<3;++i)
	{
		//Log("\t\tvStartCorner.v[%d] == %f, vScale.v[%d] == %f\n", i, vStartCorner.v[i], i, vScale.v[i]);
		if(i!=mlNormalAxisIndex)
            vEndCorner.v[i] = vStartCorner.v[i] + vScale.v[i];
		else
			vEndCorner.v[i] = vStartCorner.v[i];
	}
	//Log("\tStartCorner: %s EndCorner: %s\n", vStartCorner.ToFileString().c_str(), vEndCorner.ToFileString().c_str());

	apElement->SetAttributeVector3f("EndCorner", vEndCorner);
	apElement->SetAttributeVector3f("Scale", cVector3f(1));

	for(int i=0;i<4;++i)
		apElement->SetAttributeVector2f("Corner" + cString::ToString(i+1) + "UV", mvUVCorners[i]);

	return true;
}

//---------------------------------------------------------------------------

iEdObject* cEdObjPlaneData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjPlane,(this));
}

//------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

cEdObjPlane::cEdObjPlane(iEdObjectData* apData) : iEdScnObjPrimitive(apData)
{
	mlNormalAxisIndex=-1;
	mbAlignToWorldCoords = false;

	mvTileOffset = cVector3f(0);
	mvTileAmount = cVector3f(1);

	mvStartCorner = 999.0f;
	mvEndCorner = -999.0f;
}

//------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

//------------------------------------------------------------------------

bool cEdObjPlane::GetProperty(int alPropID, bool& abX)
{
	switch(alPropID)
	{
	case ePrimitivePlaneBool_AlignToWorld:
		abX = GetAlignToWorld();
		break;
	default:
		return iEdScnObjPrimitive::GetProperty(alPropID, abX);
	}

	return true;
}


bool cEdObjPlane::GetProperty(int alPropID, cVector3f& avX)
{
	switch(alPropID)
	{
	case ePrimitivePlaneVec3f_TileAmount:
		avX = GetTileAmount();
		break;
	case ePrimitivePlaneVec3f_TileOffset:
		avX = GetTileOffset();
		break;
	case ePrimitivePlaneVec3f_StartCorner:
		avX = GetStartCorner();
		break;
	case ePrimitivePlaneVec3f_EndCorner:
		avX = GetEndCorner();
		break;
	default:
		return iEdScnObject::GetProperty(alPropID, avX);
	}

	return true;
}

bool cEdObjPlane::GetProperty(int alPropID, float& afX)
{
	switch(alPropID)
	{
	case ePrimitivePlaneFloat_TextureAngle:
		afX = GetTextureAngle();
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, afX);
	}

	return true;
}


bool cEdObjPlane::SetProperty(int alPropID, const bool abX)
{
	switch(alPropID)
	{
	case ePrimitivePlaneBool_AlignToWorld:
		SetAlignToWorld(abX);
		break;
	default:
		return iEdScnObjPrimitive::SetProperty(alPropID, abX);
	}

	return true;
}


bool cEdObjPlane::SetProperty(int alPropID, const cVector3f& avX)
{
	switch(alPropID)
	{
	case ePrimitivePlaneVec3f_TileAmount:
		SetTileAmount(avX);
		break;
	case ePrimitivePlaneVec3f_TileOffset:
		SetTileOffset(avX);
		break;
	case ePrimitivePlaneVec3f_StartCorner:
		SetStartCorner(avX);
		break;
	case ePrimitivePlaneVec3f_EndCorner:
		SetEndCorner(avX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, avX);
	}

	return true;
}

bool cEdObjPlane::SetProperty(int alPropID, const float afX)
{
	switch(alPropID)
	{
	case ePrimitivePlaneFloat_TextureAngle:
		SetTextureAngle(afX);
		break;
	default:
		return iEdScnObject::SetProperty(alPropID, afX);
	}

	return true;
}

//------------------------------------------------------------------------

void cEdObjPlane::SetTileAmount(const cVector3f& avTileAmount)
{
	if(mvTileAmount==avTileAmount) return;
	mvTileAmount = avTileAmount;

	UpdateUVMapping();
}

//------------------------------------------------------------------------

void cEdObjPlane::SetTileOffset(const cVector3f& avOffset)
{
	if(mvTileOffset==avOffset) return;
	mvTileOffset=avOffset;

	UpdateUVMapping();	
}

//------------------------------------------------------------------------

void cEdObjPlane::SetTextureAngle(float afX)
{
	if(mfTextureAngle==afX) return;
	mfTextureAngle = afX;

	UpdateUVMapping();
}

//------------------------------------------------------------------------

void cEdObjPlane::SetScale(const cVector3f& avScale)
{
	iEdScnObject::SetScale(avScale);
	for(int i=0;i<3;++i)
	{
		if(mvScale.v[i]<0)
			mvScale.v[i]=0;
	}
}

//------------------------------------------------------------------------

void cEdObjPlane::SetStartCorner(const cVector3f& avX)
{
	if(mvStartCorner==avX)
		return;

	mvStartCorner = avX;
	mlNormalAxisIndex = ComputeNormalAxisIndex();
}

void cEdObjPlane::SetEndCorner(const cVector3f& avX)
{
	if(mvEndCorner==avX)
		return;

	mvEndCorner = avX;
	mlNormalAxisIndex = ComputeNormalAxisIndex();
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------

bool cEdObjPlane::OnUpdate(bool abForce)
{
	if(iEdScnObject::OnUpdate(abForce)==false)
		return false;

	UpdateUVMapping();

	return true;
}

//------------------------------------------------------------------------

int cEdObjPlane::ComputeNormalAxisIndex()
{
	//Log("\tComputing plane normal axis index...\n");
	cVector3f vDiff = mvEndCorner-mvStartCorner;
	//Log("\tvDiff = %s\n", vDiff.ToFileString().c_str());
	for(int i=0;i<3;++i)
		if(vDiff.v[i]<kEpsilonf)  return i;

	return -1;
}

//------------------------------------------------------------------------

void cEdObjPlane::UpdateUVMapping()
{
	//////////////////////////////////////////////////////////////
	// VERY IMPORTANT TO CLEAR THE CURRENT UVS (for god's sake)
	mvUVCorners.clear();
	

	iVertexBuffer* pVB = ((cEngineGeneratedMesh*)mpEngObject)->GetVertexBuffer();

	int lNumVertices = pVB->GetVertexNum();
	float* pVertexCoords = pVB->GetFloatArray(eVertexBufferElement_Position);
	float* pTexCoords = pVB->GetFloatArray(eVertexBufferElement_Texture0);

	for(int i=0;i<lNumVertices;++i)
	{
		cVector3f vCoords = cVector3f(pVertexCoords[0],pVertexCoords[1], pVertexCoords[2]);
		if(mbAlignToWorldCoords)
			vCoords = cMath::MatrixMul(mmtxTransform, vCoords);
		else
			vCoords = cMath::MatrixMul(mmtxScale, vCoords);

		vCoords = cMath::MatrixMul(cMath::MatrixRotateY(mfTextureAngle), vCoords);

		if(mlNormalAxisIndex==0)
			pTexCoords[0] = vCoords.v[1]*mvTileAmount.v[1] + mvTileOffset.v[1];
		else
			pTexCoords[0] = vCoords.v[0]*mvTileAmount.v[0] + mvTileOffset.v[0];

		if(mlNormalAxisIndex==2)
			pTexCoords[1] = vCoords.v[1]*mvTileAmount.v[1] + mvTileOffset.v[1];
		else
			pTexCoords[1] = vCoords.v[2]*mvTileAmount.v[2] + mvTileOffset.v[2];

		mvUVCorners.push_back(cVector2f(pTexCoords[0],pTexCoords[1]));

		pVertexCoords+=4;
		pTexCoords+=3;
	}

	pVB->UpdateData(eVertexElementFlag_Texture0, false);
}

//------------------------------------------------------------------------

cMesh* cEdObjPlane::CreatePrimitiveMesh()
{
	cVector3f vEndCorner = mvEndCorner;
	for(int i=0;i<3;++i)
	{
		if(vEndCorner.v[i]!=0)
			vEndCorner.v[i] /= vEndCorner.v[i];
	}
	cMesh* pMesh = GetWorld()->GetEditor()->GetEngine()->GetGraphics()->GetMeshCreator()->CreatePlane("", 0, vEndCorner, 
																											cVector2f(1,0), 0, 
																											cVector2f(0,1), 1, 
																											msMaterial);

	return pMesh;
}

//------------------------------------------------------------------------

/*
void cEdObjPlane::SaveToElement(cXmlElement* apElement)
{
	EntityWrapperPrimitive::SaveToElement(apElement);
	apElement->SetValue("Plane");

	apElement->SetAttributeVector3f("Scale", cVector3f(1));

	apElement->SetAttributeVector3f("StartCorner", mvStartCorner);
	cVector3f vEndCorner;
	for(int i=0;i<3;++i)
	{
		if(i!=mlNormalAxisIndex)
            vEndCorner.v[i] = mvStartCorner.v[i] + mvScale.v[i];
		else
			vEndCorner.v[i] = mvStartCorner.v[i];
	}
	apElement->SetAttributeVector3f("EndCorner", vEndCorner);

	for(int i=0;i<4;++i)
		apElement->SetAttributeVector2f("Corner" + cString::ToString(i+1) + "UV", mvUVCorners[i]);
	
	apElement->SetAttributeVector3f("TileAmount", mvTileAmount);
	apElement->SetAttributeVector3f("TileOffset", mvTileOffset);
	apElement->SetAttributeBool("AlignToWorldCoords", mbAlignToWorldCoords);
}
*/
//------------------------------------------------------------------------

void cEdObjPlane::UpdateEntity()
{
	//iEntityWrapper::UpdateEntity();
	
	//UpdateUVMapping();
}

//------------------------------------------------------------------------
