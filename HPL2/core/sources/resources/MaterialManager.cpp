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

#include "resources/MaterialManager.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/System.h"
#include "system/Platform.h"

#include "graphics/Graphics.h"
#include "graphics/Material.h"
#include "graphics/MaterialType.h"
#include "graphics/LowLevelGraphics.h"

#include "resources/TextureManager.h"
#include "resources/Resources.h"
#include "resources/LowLevelResources.h"
#include "resources/XmlDocument.h"

#include "impl/tinyXML/tinyxml.h"



namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// BLANK MATERIAL
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	class cMaterialManagerBlankMaterialType_Vars : public iMaterialVars
	{
	};

	class cMaterialManagerBlankMaterialType : public iMaterialType
	{
	public:
		cMaterialManagerBlankMaterialType() : iMaterialType(NULL, NULL){}

		void LoadData(){}
		void DestroyData(){}

		void DestroyProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, char alSkeleton){}

		bool SupportsHWSkinning(){ return false; }

		iTexture* GetTextureForUnit(cMaterial *apMaterial,eMaterialRenderMode aRenderMode, int alUnit){ return NULL;}
		iTexture* GetSpecialTexture(cMaterial *apMaterial, eMaterialRenderMode aRenderMode,iRenderer *apRenderer, int alUnit){ return NULL; }

		iGpuProgram* GetGpuProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, char alSkeleton){return NULL;}

		void SetupTypeSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram,iRenderer *apRenderer){ }
		void SetupMaterialSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, cMaterial *apMaterial,iRenderer *apRenderer){ }
		void SetupObjectSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, iRenderable *apObject,iRenderer *apRenderer){ }

		iMaterialVars* CreateSpecificVariables(){ return hplNew(cMaterialManagerBlankMaterialType_Vars,());}
		void LoadVariables(cMaterial *apMaterial, cResourceVarsObject *apVars){ }
		void GetVariableValues(cMaterial *apMaterial, cResourceVarsObject *apVars){ }

		void CompileMaterialSpecifics(cMaterial *apMaterial){}
	};
	
	cMaterialManagerBlankMaterialType gBlankMaterialType;
    

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterialManager::cMaterialManager(cGraphics* apGraphics,cResources *apResources)
		: iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),apResources->GetLowLevelSystem())
	{
		mpGraphics = apGraphics;
		mpResources = apResources;

		mlTextureSizeDownScaleLevel =0;
		mTextureFilter = eTextureFilter_Bilinear;
		mfTextureAnisotropy = 1.0f;

		mbDisableRenderDataLoading = false;

		mlIdCounter =0;
	}

	cMaterialManager::~cMaterialManager()
	{
        DestroyAll();

		Log(" Done with materials\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial* cMaterialManager::CreateMaterial(const tString& asName)
	{
		if(asName=="")
			return NULL;

		tWString sPath;
		cMaterial* pMaterial;
		tString asNewName;

		BeginLoad(asName);

		asNewName = cString::SetFileExt(asName,"mat");

		pMaterial = static_cast<cMaterial*>(this->FindLoadedResource(asNewName,sPath));

		if(pMaterial==NULL && sPath!=_W(""))
		{
			pMaterial = LoadFromFile(asNewName,sPath);

			if(pMaterial==NULL){
				Error("Couldn't load material '%s'\n",asNewName.c_str());
				EndLoad();
				return NULL;
			}

			AddResource(pMaterial);
		}

		if(pMaterial)pMaterial->IncUserCount();
		else Error("Couldn't create material '%s'\n",asNewName.c_str());

		EndLoad();
		return pMaterial;
	}

	//-----------------------------------------------------------------------

	void cMaterialManager::Update(float afTimeStep)
	{
		
	}

	//-----------------------------------------------------------------------

	void cMaterialManager::Unload(iResourceBase* apResource)
	{

	}
	//-----------------------------------------------------------------------

	void cMaterialManager::Destroy(iResourceBase* apResource)
	{
		apResource->DecUserCount();

		if(apResource->HasUsers()==false)
		{
			RemoveResource(apResource);
			hplDelete(apResource);
		}
	}

	//-----------------------------------------------------------------------

	void cMaterialManager::SetTextureFilter(eTextureFilter aFilter)
	{
		if(aFilter == mTextureFilter) return;
		mTextureFilter = aFilter;
		
		tResourceBaseMapIt it = m_mapResources.begin();
		for(; it != m_mapResources.end(); ++it)
		{
			cMaterial *pMat = static_cast<cMaterial*>(it->second);

            for(int i=0; i<eMaterialTexture_LastEnum; ++i)
			{
				iTexture *pTex = pMat->GetTexture((eMaterialTexture)i);
				if(pTex)pTex->SetFilter(aFilter);
			}
		}
	}

	//-----------------------------------------------------------------------
	
	void cMaterialManager::SetTextureAnisotropy(float afX)
	{
		if(afX <1.0 || mpGraphics->GetLowLevel()->GetCaps(eGraphicCaps_AnisotropicFiltering)==0)
		{
			return;
		}
		if(afX > (float) mpGraphics->GetLowLevel()->GetCaps(eGraphicCaps_MaxAnisotropicFiltering))
		{
			return;
		}

		if(mfTextureAnisotropy == afX) return;
		mfTextureAnisotropy = afX;

		tResourceBaseMapIt it = m_mapResources.begin();
		for(; it != m_mapResources.end(); ++it)
		{
			cMaterial *pMat = static_cast<cMaterial*>(it->second);

			for(int i=0; i<eMaterialTexture_LastEnum; ++i)
			{
				iTexture *pTex = pMat->GetTexture((eMaterialTexture)i);
				if(pTex)pTex->SetAnisotropyDegree(mfTextureAnisotropy);
			}
		}
	}

	//-----------------------------------------------------------------------

	tString cMaterialManager::GetPhysicsMaterialName(const tString& asName)
	{
		tWString sPath;
		cMaterial* pMaterial;
		tString asNewName;

		asNewName = cString::SetFileExt(asName,"mat");

		pMaterial = static_cast<cMaterial*>(this->FindLoadedResource(asNewName,sPath));

		if(pMaterial==NULL && sPath!=_W(""))
		{
			FILE *pFile = cPlatform::OpenFile(sPath, _W("rb"));
			if(pFile==NULL) return "";

			TiXmlDocument *pDoc = hplNew( TiXmlDocument, () );
			if(!pDoc->LoadFile(pFile))
			{
				fclose(pFile);
				hplDelete(pDoc);
				return "";
			}
			fclose(pFile);

			TiXmlElement *pRoot = pDoc->RootElement();

			TiXmlElement *pMain = pRoot->FirstChildElement("Main");
			if(pMain==NULL){
				hplDelete(pDoc);
				Error("Main child not found in '%s'\n",sPath.c_str());
				return "";
			}

			tString sPhysicsName = cString::ToString(pMain->Attribute("PhysicsMaterial"),"Default");

			hplDelete(pDoc);

			return sPhysicsName;
		}

		if(pMaterial)
			return pMaterial->GetPhysicsMaterial();
		else
			return "";
	}

	//-----------------------------------------------------------------------

	cMaterial* cMaterialManager::CreateCustomMaterial(const tString& asName, iMaterialType *apMaterialType)
	{
		cMaterial* pMat = hplNew( cMaterial, (asName, cString::To16Char(asName), mpGraphics, mpResources, apMaterialType) );
		pMat->IncUserCount();
		AddResource(pMat);
		return pMat;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	cMaterial* cMaterialManager::LoadFromFile(const tString& asName,const tWString& asPath)
	{
		//Log("Load material: %s\n", asName.c_str());

		iXmlDocument* pDoc = mpResources->GetLowLevel()->CreateXmlDocument();
		if(pDoc->CreateFromFile(asPath)==false)
		{
			mpResources->DestroyXmlDocument(pDoc);
			return NULL;
		}

		cXmlElement* pMain = pDoc->GetFirstElement("Main");
		if(pMain==NULL)
		{
			mpResources->DestroyXmlDocument(pDoc);
			Error("Main child not found.\n");
			return NULL;
		}

		tString sType = pMain->GetAttributeString("Type");
		if(sType=="")
		{
			mpResources->DestroyXmlDocument(pDoc);
			Error("Type not found.\n");
			return NULL;
		}
        
		/////////////////////////////
		// Get General Propertries
		bool bDepthTest = pMain->GetAttributeBool("DepthTest", true);
		float fValue = pMain->GetAttributeFloat("Value", 1);
		tString sPhysicsMatName = pMain->GetAttributeString("PhysicsMaterial", "Default");
		tString sBlendMode = pMain->GetAttributeString("BlendMode", "Add");

		/////////////////////////////
		// Make a "fake" material, with a blank type
		if(mbDisableRenderDataLoading)
		{
			cMaterial* pMat = hplNew( cMaterial, (asName, asPath, mpGraphics, mpResources, &gBlankMaterialType) );
			pMat->SetPhysicsMaterial(sPhysicsMatName);
			
			mpResources->DestroyXmlDocument(pDoc);
			return pMat;
		}

		/////////////////////////////
		// CreateType
		iMaterialType *pMatType = mpGraphics->GetMaterialType(sType);
		if(pMatType ==NULL)
		{
			mpResources->DestroyXmlDocument(pDoc);
			Error("Invalid material type '%s'!\n",sType.c_str());
			return NULL;
		}
		cMaterial* pMat = hplNew( cMaterial, (asName, asPath, mpGraphics, mpResources, pMatType) );
		
		pMat->SetDepthTest(bDepthTest);
		pMat->SetPhysicsMaterial(sPhysicsMatName);
        if(pMatType->IsTranslucent())
			pMat->SetBlendMode(GetBlendMode(sBlendMode));

		///////////////////////////
		//Textures
		cXmlElement* pTexRoot = pDoc->GetFirstElement("TextureUnits");
		if(pTexRoot==NULL){
			mpResources->DestroyXmlDocument(pDoc);
			Error("TextureUnits child not found.\n");
			return NULL;
		}

		//Log("Material %s\n",asName.c_str());
		
		for(int i=0; i< pMatType->GetUsedTextureNum(); ++i)
		{
			cMaterialUsedTexture* pUsedTexture = pMatType->GetUsedTexture(i);
			iTexture *pTex = NULL;

			tString sTextureType = GetTextureString(pUsedTexture->mType);
			//Log("Trying to load type: %s\n",sTextureType.c_str());
			
			cXmlElement* pTexChild = pTexRoot->GetFirstElement(sTextureType.c_str());
			if(pTexChild==NULL){
				//Log(" Texture unit element missing!\n");
				/*hplDelete(pMat);
				return NULL;*/
				continue;
			}

			eTextureType type = GetType(pTexChild->GetAttributeString("Type", ""));
			tString sFile = pTexChild->GetAttributeString("File", "");
			bool bMipMaps = pTexChild->GetAttributeBool("MipMaps", true);
			bool bCompress = pTexChild->GetAttributeBool("Compress", false);
			eTextureWrap wrap = GetWrap(pTexChild->GetAttributeString("Wrap", ""));
			
			eTextureAnimMode animMode = GetAnimMode(pTexChild->GetAttributeString("AnimMode", "None"));
			float fFrameTime = pTexChild->GetAttributeFloat("AnimFrameTime", 1.0f);
			
			if(sFile=="") continue;
			
			if(cString::GetFilePath(sFile).length() <= 1)
			{
				sFile = cString::SetFilePath(sFile, cString::To8Char(cString::GetFilePathW(asPath)));
			}
			//Log("Texture: '%s'\n",sFile.c_str());
			//Log(" Loading!\n");

			if(animMode != eTextureAnimMode_None)
			{
				pTex = mpResources->GetTextureManager()->CreateAnim(sFile,bMipMaps,type,eTextureUsage_Normal,mlTextureSizeDownScaleLevel);
			}
			else
			{
				
				if(type == eTextureType_1D)
				{
					pTex = mpResources->GetTextureManager()->Create1D(sFile,bMipMaps,
																			eTextureUsage_Normal,
																			mlTextureSizeDownScaleLevel);
				}
				else if(type == eTextureType_2D)
				{
					pTex = mpResources->GetTextureManager()->Create2D(sFile,bMipMaps, eTextureType_2D,
																		eTextureUsage_Normal,
																		mlTextureSizeDownScaleLevel);
				}
				else if(type == eTextureType_3D)
				{
					pTex = mpResources->GetTextureManager()->Create3D(sFile,bMipMaps,
																		eTextureUsage_Normal,
																		mlTextureSizeDownScaleLevel);
				}
				else if(type == eTextureType_CubeMap)
				{
					//Check for DDS ending and load cubemap as file.
					pTex = mpResources->GetTextureManager()->CreateCubeMap(sFile,bMipMaps,
																			eTextureUsage_Normal,
																			mlTextureSizeDownScaleLevel);
				}
			}

			if(pTex==NULL)
			{
				mpResources->DestroyXmlDocument(pDoc);
				hplDelete(pMat);
				return NULL;
			}

			pTex->SetFrameTime(fFrameTime);
			pTex->SetAnimMode(animMode);

			pTex->SetWrapSTR(wrap);
			
			pTex->SetFilter(mTextureFilter);
			pTex->SetAnisotropyDegree(mfTextureAnisotropy);

			pMat->SetTexture(pUsedTexture->mType, pTex);
		}

		///////////////////////////
		//Animations
		cXmlElement* pUvAnimRoot = pDoc->GetFirstElement("UvAnimations");
		if(pUvAnimRoot)
		{
			cXmlNodeListIterator it = pUvAnimRoot->GetChildIterator();
			while(it.HasNext())
			{
				cXmlElement* pAnimElem = it.Next()->ToElement();

				eMaterialUvAnimation animType = GetUvAnimType(pAnimElem->GetAttributeString("Type").c_str());	
				eMaterialAnimationAxis animAxis = GetAnimAxis(pAnimElem->GetAttributeString("Axis").c_str());	
				float fSpeed = pAnimElem->GetAttributeFloat("Speed",0);
				float fAmp = pAnimElem->GetAttributeFloat("Amplitude",0);

				pMat->AddUvAnimation(animType,fSpeed,fAmp, animAxis);
			}
		}


		///////////////////////////
		//Variables
		cXmlElement* pUserVarsRoot = pDoc->GetFirstElement("SpecificVariables");
		cResourceVarsObject userVars;
		if(pUserVarsRoot) userVars.LoadVariables(pUserVarsRoot);
		
		pMatType->LoadVariables(pMat, &userVars);
		
		
		///////////////////////////
		//End
		
		mpResources->DestroyXmlDocument(pDoc);
			
		pMat->Compile();
		
		return pMat;
	}

	//-----------------------------------------------------------------------

	eTextureType cMaterialManager::GetType(const tString& asType)
	{
		if(cString::ToLowerCase(asType) == "cube") return eTextureType_CubeMap;
		else if(cString::ToLowerCase(asType) == "1d") return eTextureType_1D;
		else if(cString::ToLowerCase(asType) == "2d") return eTextureType_2D;
		else if(cString::ToLowerCase(asType) == "3d") return eTextureType_3D;

		return eTextureType_2D;
	}
	//-----------------------------------------------------------------------
	
	tString cMaterialManager::GetTextureString(eMaterialTexture aType)
	{
		switch(aType)
		{
			case eMaterialTexture_Diffuse: return "Diffuse";
			case eMaterialTexture_Alpha: return "Alpha";
			case eMaterialTexture_NMap: return "NMap";
			case eMaterialTexture_Height: return "Height";
			case eMaterialTexture_Illumination: return "Illumination";
			case eMaterialTexture_Specular: return "Specular";
			case eMaterialTexture_CubeMap: return "CubeMap";
			case eMaterialTexture_DissolveAlpha: return "DissolveAlpha";
			case eMaterialTexture_CubeMapAlpha: return "CubeMapAlpha";
		}

		return "";
	}
	
	//-----------------------------------------------------------------------

	eTextureWrap cMaterialManager::GetWrap(const tString& asType)
	{
		if(cString::ToLowerCase(asType) == "repeat") return eTextureWrap_Repeat;
		else if(cString::ToLowerCase(asType) == "clamp") return eTextureWrap_Clamp;
		else if(cString::ToLowerCase(asType) == "clamptoedge") return eTextureWrap_ClampToEdge;

		return eTextureWrap_Repeat;
	}

	eTextureAnimMode cMaterialManager::GetAnimMode(const tString& asType)
	{
		if(cString::ToLowerCase(asType) == "none") return eTextureAnimMode_None;
		else if(cString::ToLowerCase(asType) == "loop") return eTextureAnimMode_Loop;
		else if(cString::ToLowerCase(asType) == "oscillate") return eTextureAnimMode_Oscillate;

		return eTextureAnimMode_None;
	}

	//-----------------------------------------------------------------------

	eMaterialBlendMode cMaterialManager::GetBlendMode(const tString& asType)
	{
		tString sLow = cString::ToLowerCase(asType);
		if(sLow == "add")	return eMaterialBlendMode_Add;
		if(sLow == "mul")	return eMaterialBlendMode_Mul;
		if(sLow == "mulx2")	return eMaterialBlendMode_MulX2;
		if(sLow == "alpha")	return eMaterialBlendMode_Alpha;
		if(sLow == "premulalpha")	return eMaterialBlendMode_PremulAlpha;

		Warning("Material BlendMode '%s' does not exist!\n",asType.c_str());

		return eMaterialBlendMode_Add;
	}

	//-----------------------------------------------------------------------
	
	eMaterialUvAnimation cMaterialManager::GetUvAnimType(const char* apString)
	{
		if(apString==NULL){ 
			Error("Uv animation attribute Type does not exist!\n");
			return eMaterialUvAnimation_LastEnum;
		}
		
		tString sLow = cString::ToLowerCase(apString);
		
		if(sLow == "translate") return eMaterialUvAnimation_Translate;
		if(sLow == "sin") return eMaterialUvAnimation_Sin;
		if(sLow == "rotate") return eMaterialUvAnimation_Rotate;
		
		Error("Invalid uv animation type %s\n",apString);
		return eMaterialUvAnimation_LastEnum;
	}
	
	eMaterialAnimationAxis cMaterialManager::GetAnimAxis(const char* apString)
	{
		if(apString==NULL){ 
			Error("Uv animation attribute Axis does not exist!\n");
			return eMaterialAnimationAxis_LastEnum;
		}

		tString sLow = cString::ToLowerCase(apString);

		if(sLow == "x") return eMaterialAnimationAxis_X;
		if(sLow == "y") return eMaterialAnimationAxis_Y;
		if(sLow == "z") return eMaterialAnimationAxis_Z;

		Error("Invalid animation axis %s\n",apString);
		return eMaterialAnimationAxis_LastEnum;
	}

	//-----------------------------------------------------------------------
}
