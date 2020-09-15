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

#include "scene/Beam.h"

#include "impl/tinyXML/tinyxml.h"

#include "math/Math.h"
#include "system/String.h"
#include "system/Platform.h"

#include "resources/Resources.h"
#include "resources/MaterialManager.h"
#include "resources/FileSearcher.h"

#include "graphics/Graphics.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Material.h"
#include "graphics/MaterialType.h"
#include "graphics/LowLevelGraphics.h"

#include "scene/Camera.h"
#include "scene/World.h"
#include "scene/Scene.h"

#include "engine/Engine.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cBeam::cBeam(const tString asName, cResources *apResources,cGraphics *apGraphics) :
	iRenderable(asName)
	{
		mpMaterialManager = apResources->GetMaterialManager();
		mpFileSearcher = apResources->GetFileSearcher();
		mpLowLevelGraphics = apGraphics->GetLowLevel();
		
		msFileName = "";

		mvSize = 1;

		mbTileHeight = true;
		
		mColor = cColor(1,1,1,1);

		mpMaterial = NULL;

		mlLastRenderCount = -1;
		

		mpVtxBuffer = mpLowLevelGraphics->CreateVertexBuffer(
								eVertexBufferType_Hardware,
								eVertexBufferDrawType_Tri, eVertexBufferUsageType_Dynamic,4,6);
		mpVtxBuffer->CreateElementArray(eVertexBufferElement_Position,eVertexBufferElementFormat_Float,4);
		mpVtxBuffer->CreateElementArray(eVertexBufferElement_Normal,eVertexBufferElementFormat_Float,3);
		mpVtxBuffer->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Float,4);
		mpVtxBuffer->CreateElementArray(eVertexBufferElement_Texture0,eVertexBufferElementFormat_Float,3);

		cVector3f vCoords[4] = {cVector3f((mvSize.x/2),-(mvSize.y/2),0),
								cVector3f(-(mvSize.x/2),-(mvSize.y/2),0),
								cVector3f(-(mvSize.x/2),(mvSize.y/2),0),
								cVector3f((mvSize.x/2),(mvSize.y/2),0)};

		cVector3f vTexCoords[4] = {cVector3f(1,1,0),	//Bottom left
									cVector3f(-1,1,0),	//Bottom right
									cVector3f(-1,-1,0),	//Top left
									cVector3f(1,-1,0)};	//Top right

		for(int i=0;i<4;i++)
		{
			mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Position, vCoords[i]);
			mpVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, cColor(1,1,1,1));
			mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Texture0, (vTexCoords[i] + cVector2f(1,1))/2);
			mpVtxBuffer->AddVertexVec3f(eVertexBufferElement_Normal,cVector3f(0,0,1));
		}

		for(int i=0;i<3;i++) mpVtxBuffer->AddIndex(i);
		for(int i=2;i<5;i++) mpVtxBuffer->AddIndex(i==4?0:i);

		mpVtxBuffer->Compile(eVertexCompileFlag_CreateTangents);

		mpEnd = hplNew( cBeamEnd, (asName + "_end",this));
		mpEnd->AddCallback(&mEndCallback);
		
		//Some temp setup
		mBoundingVolume.SetSize(cVector3f(mvSize.x, mvSize.y, mvSize.x));

		mbApplyTransformToBV = false;
	}

	//-----------------------------------------------------------------------

	cBeam::~cBeam()
	{
		hplDelete(mpEnd);
		if(mpMaterial) mpMaterialManager->Destroy(mpMaterial);
		if(mpVtxBuffer) hplDelete(mpVtxBuffer);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cBeam::SetSize(const cVector2f& avSize)
	{
		mvSize = avSize;
		mBoundingVolume.SetSize(cVector3f(mvSize.x, mvSize.y, mvSize.x));
		
		SetTransformUpdated();
	}

	//-----------------------------------------------------------------------

	void cBeam::SetTileHeight(bool abX)
	{
		if(mbTileHeight == abX) return;

		mbTileHeight = abX;

		SetTransformUpdated();
	}

	//-----------------------------------------------------------------------

	void cBeam::SetMultiplyAlphaWithColor(bool abX)
	{
		if(mbMultiplyAlphaWithColor == abX) return;

		mbMultiplyAlphaWithColor = abX;
	}

	//-----------------------------------------------------------------------

	void cBeam::SetColor(const cColor &aColor)
	{
		if(mColor == aColor) return;

		mColor = aColor;

		float *pColors = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Color0);

		//Change "lower colors"
		if(mbMultiplyAlphaWithColor)
		{
			for(int i=0; i<2;++i)
			{
				pColors[0] = mColor.r * mColor.a;
				pColors[1] = mColor.g * mColor.a;
				pColors[2] = mColor.b * mColor.a;
				pColors[3] = mColor.a;
				pColors+=4;
			}
		}
		else
		{
			for(int i=0; i<2;++i)
			{
				pColors[0] = mColor.r;
				pColors[1] = mColor.g;
				pColors[2] = mColor.b;
				pColors[3] = mColor.a;
				pColors+=4;
			}
		}
		mpVtxBuffer->UpdateData(eVertexElementFlag_Color0,false);
	}

	//-----------------------------------------------------------------------

	void cBeam::SetMaterial(cMaterial * apMaterial)
	{
		mpMaterial = apMaterial;
	}

	//-----------------------------------------------------------------------

	cBoundingVolume* cBeam::GetBoundingVolume()
	{
		if(mbUpdateBoundingVolume)
		{
			cVector3f vMax = GetWorldPosition();
			cVector3f vMin = vMax;
			cVector3f vEnd = mpEnd->GetWorldPosition();

			if(vMax.x < vEnd.x) vMax.x = vEnd.x;
			if(vMax.y < vEnd.y) vMax.y = vEnd.y;
			if(vMax.z < vEnd.z) vMax.z = vEnd.z;

			if(vMin.x > vEnd.x) vMin.x = vEnd.x;
			if(vMin.y > vEnd.y) vMin.y = vEnd.y;
			if(vMin.z > vEnd.z) vMin.z = vEnd.z;

			vMin -= cVector3f(mvSize.x);
			vMax += cVector3f(mvSize.x);

			mBoundingVolume.SetLocalMinMax(vMin,vMax);

			mbUpdateBoundingVolume = false;
		}

		return &mBoundingVolume;
	}

	//-----------------------------------------------------------------------

	void cBeam::UpdateGraphicsForFrame(float afFrameTime)
	{
		if(	mlStartTransformCount == GetTransformUpdateCount() &&
			mlEndTransformCount == GetTransformUpdateCount())
		{
			return;
		}

		////////////////////////////////
		//Get Axis
		mvAxis = mpEnd->GetWorldPosition() - GetWorldPosition();
        
		mvMidPosition =GetWorldPosition() + mvAxis*0.5f;
		float fDist = mvAxis.Length();

		mvAxis.Normalize();

		////////////////////////////////
		//Update vertex buffer
		cVector2f vBeamSize = cVector2f(mvSize.x, fDist);

		float *pPos = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Position);
		float *pTex = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Texture0);

		cVector3f vCoords[4] = {cVector3f((vBeamSize.x/2),-(vBeamSize.y/2),0),
								cVector3f(-(vBeamSize.x/2),-(vBeamSize.y/2),0),
								cVector3f(-(vBeamSize.x/2),(vBeamSize.y/2),0),
								cVector3f((vBeamSize.x/2),(vBeamSize.y/2),0)};

		cVector3f vTexCoords[4];
		if(mbTileHeight)
		{
			vTexCoords[0] = cVector3f(1,1,0);	//Bottom left
			vTexCoords[1] = cVector3f(0,1,0);	//Bottom right
			vTexCoords[2] = cVector3f(0,-fDist/mvSize.y,0);	//Top left
			vTexCoords[3] = cVector3f(1,-fDist/mvSize.y,0);	//Top right
		}
		else
		{
			vTexCoords[0] = cVector3f(1,1,0);	//Bottom left
			vTexCoords[1] = cVector3f(0,1,0);	//Bottom right
			vTexCoords[2] = cVector3f(0,0,0);	//Top left
			vTexCoords[3] = cVector3f(1,0,0);	//Top right
		}

		for(int i=0; i<4;++i)
		{
			pPos[0] = vCoords[i].x;
			pPos[1] = vCoords[i].y;
			pPos[2] = vCoords[i].z;
			pPos+=4;

			pTex[0] = vTexCoords[i].x;
			pTex[1] = vTexCoords[i].y;
			pTex+=3;
		}

		if(mpMaterial->GetType()->IsTranslucent())
		{
			mpVtxBuffer->UpdateData(eVertexElementFlag_Position | eVertexElementFlag_Texture0,false);
		}
		else
		{
			mpVtxBuffer->UpdateData(eVertexElementFlag_Position | eVertexElementFlag_Texture0,false);
		}
		
		

	}

	//-----------------------------------------------------------------------

	cMatrixf* cBeam::GetModelMatrix(cFrustum *apFrustum)
	{
		if(apFrustum==NULL)return &GetWorldMatrix();

		m_mtxTempTransform = GetWorldMatrix();
		cVector3f vForward, vRight, vUp;

		cVector3f vCameraForward = apFrustum->GetOrigin() - GetWorldPosition();
		vCameraForward.Normalize();

		vUp = mvAxis;//cMath::MatrixMul(GetWorldMatrix().GetRotation(),mvAxis);
		//vUp.Normalize();
			
		if(vUp == vForward)
		{
			vRight = cMath::Vector3Cross(vUp, vCameraForward);
			Warning("Beam Right vector is not correct! Contact programmer!\n");
		}
		else
			vRight = cMath::Vector3Cross(vUp, vCameraForward);

		vRight.Normalize();
		vForward = cMath::Vector3Cross(vRight, vUp);

		//Set right vector
		m_mtxTempTransform.m[0][0] = vRight.x;
		m_mtxTempTransform.m[1][0] = vRight.y;
		m_mtxTempTransform.m[2][0] = vRight.z;

		//Set up vector
		m_mtxTempTransform.m[0][1] = vUp.x;
		m_mtxTempTransform.m[1][1] = vUp.y;
		m_mtxTempTransform.m[2][1] = vUp.z;

		//Set forward vector
		m_mtxTempTransform.m[0][2] = vForward.x;
		m_mtxTempTransform.m[1][2] = vForward.y;
		m_mtxTempTransform.m[2][2] = vForward.z;

		m_mtxTempTransform.SetTranslation(mvMidPosition);

		return &m_mtxTempTransform;
	}

	//-----------------------------------------------------------------------

	int cBeam::GetMatrixUpdateCount()
	{
		return GetTransformUpdateCount();
	}

	//-----------------------------------------------------------------------


	bool cBeam::LoadXMLProperties(const tString asFile)
	{
		msFileName = asFile;

		tString sNewFile = cString::SetFileExt(asFile,"beam");
		tWString sPath = mpFileSearcher->GetFilePath(sNewFile);
		if(sPath != _W(""))
		{
			FILE *pFile = cPlatform::OpenFile(sPath, _W("rb"));
			if(pFile==NULL) return false;
			TiXmlDocument *pDoc = hplNew( TiXmlDocument, () );
			if(pDoc->LoadFile(pFile))
			{
				TiXmlElement *pRootElem = pDoc->RootElement();

				TiXmlElement *pMainElem = pRootElem->FirstChildElement("MAIN");
				if(pMainElem!=NULL)
				{
					tString sMaterial = cString::ToString(pMainElem->Attribute("Material"),"");
					cVector2f vSize = cString::ToVector2f(pMainElem->Attribute("Size"),1);
					
					bool bTileHeight = cString::ToBool(pMainElem->Attribute("TileHeight"),true);
					bool bMultiplyAlphaWithColor = cString::ToBool(pMainElem->Attribute("MultiplyAlphaWithColor"),false);

					cColor StartColor = cString::ToColor(pMainElem->Attribute("StartColor"),cColor(1,1));
					cColor EndColor = cString::ToColor(pMainElem->Attribute("EndColor"),cColor(1,1));
					

					SetSize(vSize);
					SetTileHeight(bTileHeight);
					SetMultiplyAlphaWithColor(bMultiplyAlphaWithColor);
					SetColor(StartColor);
					mpEnd->SetColor(EndColor);

					/////////////////
					//Load material
					cMaterial *pMat = mpMaterialManager->CreateMaterial(sMaterial);
					if(pMat)	{
						SetMaterial(pMat);
					}
					else{
						Error("Couldn't load material '%s' in Beam file '%s'",
											sMaterial.c_str(), sNewFile.c_str());
						return false;
					}
				}
				else
				{
					Error("Cannot find main element in %s\n",sNewFile.c_str());
					return false;
				}
			}
			else
			{
				Error("Couldn't load file '%s'\n",sNewFile.c_str());
			}
			if(pFile) fclose(pFile);
			hplDelete(pDoc);
		}
		else
		{
			Error("Couldn't find file '%s'\n",sNewFile.c_str());
			return false;
		}
		
		return true;
	}

	//-----------------------------------------------------------------------

	bool cBeam::IsVisible()
	{
		if(mColor.r <= 0 && mColor.g <= 0 && mColor.b <= 0) return false;

		return mbIsVisible;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// BEAM END TRANSFORM UPDATE CALLBACK
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cBeamEnd_UpdateCallback::OnTransformUpdate(iEntity3D * apEntity)
	{
		cBeamEnd *pEnd = static_cast<cBeamEnd*>(apEntity);

		pEnd->mpBeam->SetTransformUpdated(true);
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// BEAM END
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cBeamEnd::SetColor(const cColor &aColor)
	{
		if(mColor == aColor) return;

		mColor = aColor;

		float *pColors = mpBeam->mpVtxBuffer->GetFloatArray(eVertexBufferElement_Color0);
		
		//Change "upper colors"
		pColors+= 4*2;
		if(mpBeam->mbMultiplyAlphaWithColor)
		{
			for(int i=0; i<2;++i)
			{
				pColors[0] = mColor.r * mColor.a;
				pColors[1] = mColor.g * mColor.a;
				pColors[2] = mColor.b * mColor.a;
				pColors[3] = mColor.a;
				pColors+=4;
			}
		}
		else
		{
			for(int i=0; i<2;++i)
			{
				pColors[0] = mColor.r;
				pColors[1] = mColor.g;
				pColors[2] = mColor.b;
				pColors[3] = mColor.a;
				pColors+=4;
			}
		}

		mpBeam->mpVtxBuffer->UpdateData(eVertexElementFlag_Color0,false);
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

}
