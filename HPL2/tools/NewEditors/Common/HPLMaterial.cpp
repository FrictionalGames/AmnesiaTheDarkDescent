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

#include "../Common/HPLMaterial.h"
#include "../Common/Editor.h"
#include "../Common/EdResource.h"

#include "../Common/EdWindowMaterialEditor.h"
#include "../Common/EdCamera.h"

//------------------------------------------------------------------------------------

tString cHPLMaterial::gsTextureTypeStrings[] = 
{
	"1D",
	"2D",
	"Rect",
	"Cube",
	"3D",

	""
};

tString cHPLMaterial::gsWrapTypeStrings[] =
{
	"Repeat",
	"Clamp",
	"ClampToEdge",
	"ClampToBorder",

	""
};

tString cHPLMaterial::gsAnimModeStrings[] =
{
	"None",
	"Loop",
	"Oscillate",

	""
};

tString cHPLMaterial::gsBlendModeStrings[] = 
{
	"None",
	"Add",
	"Mul",
	"Mulx2",
	"Alpha",
	"PremulAlpha",

	""
};

//------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////
// MATERIAL CLASS - CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

cEdMaterialClass::cEdMaterialClass(cEdWindowMaterialEditor* apWindow)
{
	mpWindow = apWindow;

	mpMatType = NULL;
}

cEdMaterialClass::~cEdMaterialClass()
{
	STLDeleteAll(mvVariables);
}

//------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////
// MATERIAL CLASS - PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

bool cEdMaterialClass::Create(iMaterialType* apType)
{
	mpMatType = apType;

	return Create();
}

bool cEdMaterialClass::Create()
{
	if(mpMatType==NULL)
		return false;

	bool bSuccessfullyCreated = true;

	cEngine* pEng = mpWindow->GetEditor()->GetEngine();
	cResources* pRes = pEng->GetResources();
	iXmlDocument* pTempDoc = pRes->GetLowLevel()->CreateXmlDocument();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create an XML element holding the material variable data to convert to iEditorVar via CreateVariableFromElement
	for(int i=0;i<mpMatType->GetUserVariableNum();++i)
	{
		cMaterialUserVariable* pMaterialVar = mpMatType->GetUserVariable(i);
		cXmlElement* pMaterialVarData = pTempDoc->CreateChildElement("Var");

		pMaterialVarData->SetAttributeString("Name", pMaterialVar->msName);
		pMaterialVarData->SetAttributeString("Type", pEng->GetEngineTypeString(pMaterialVar->mType));
		pMaterialVarData->SetAttributeString("DefaultValue",	pMaterialVar->msValue);
		pMaterialVarData->SetAttributeString("Description",		pMaterialVar->msDescription);

		// Set up enum values, if any
		if(pMaterialVar->mvEnumValues.empty()==false)
		{
            cXmlElement* pEnumValues = pMaterialVarData->CreateChildElement("EnumValues");
			for(size_t j=0;j<pMaterialVar->mvEnumValues.size();++j)
			{
				cXmlElement* pValue = pEnumValues->CreateChildElement("Value");
				pValue->SetAttributeString("Name", pMaterialVar->mvEnumValues[j]);
			}
		}

		iEdVar* pVar = CreateVariableFromElement(pMaterialVarData);
		if(pVar==NULL)
		{
			bSuccessfullyCreated = false;
			break;
		}

		mvVariables.push_back(pVar);
	}

	pRes->DestroyXmlDocument(pTempDoc);
    
	return bSuccessfullyCreated;
}

//------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////
// MATERIAL CLASS - PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

void cEdMaterialClass::AddVariablesToInstance(cEdClassInstance* apInstance)
{
	for(size_t i=0;i<mvVariables.size(); ++i)
	{
		iEdVar* pVar = mvVariables[i];
		apInstance->AddVarInstance(pVar->CreateInstance());
	}
}

//------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////
// HPL TEXTURE WRAPPER - CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

cHplTexture::cHplTexture(cHPLMaterial* apMat, eMaterialTexture aUnit)
{
	mpMat = apMat;
	mUnit = aUnit;

	mpEngTexture = NULL;

	mTimeStamp.hours=0;
	mTimeStamp.minutes=0;
	mTimeStamp.seconds=0;
	mTimeStamp.month_day=0;
	mTimeStamp.year=0;
	mTimeStamp.year_day=0;
	mTimeStamp.week_day=0;
	mTimeStamp.month=0;

	Reset();
}

//------------------------------------------------------------------------------------

cHplTexture::~cHplTexture()
{
	if(mpEngTexture)
		mpMat->GetEditor()->GetEngine()->GetGraphics()->DestroyTexture(mpEngTexture);
}

//------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////
// HPL TEXTURE WRAPPER - PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

void cHplTexture::Reset()
{
	mbValid = false;
	mbEnabled = false;
	mbNeedsReload = true;
	mbUpdated = true;

	mbMipMaps=true;
	mbCompressed=false;

	msFile=_W("");
	msWrap="Repeat";
	msAnimMode = "None";
	mfFrameTime = 0;
}

//------------------------------------------------------------------------------------

void cHplTexture::Load(cXmlElement* apElement)
{
    mbMipMaps = apElement->GetAttributeBool("MipMaps");
	mbCompressed = apElement->GetAttributeBool("Compress");
	msFile = cString::To16Char(apElement->GetAttributeString("File"));
	msWrap = apElement->GetAttributeString("Wrap", "Repeat");
	msAnimMode = apElement->GetAttributeString("AnimMode", "None");
	mfFrameTime = apElement->GetAttributeFloat("AnimFrameTime");
}

//------------------------------------------------------------------------------------

void cHplTexture::Save(cXmlElement* apElement)
{
	cMaterialManager* pManMgr = mpMat->mpMatEditor->GetEditor()->GetEngine()->GetResources()->GetMaterialManager();
	apElement->SetValue(pManMgr->GetTextureString(mUnit));

	apElement->SetAttributeBool("MipMaps", mbMipMaps);
	apElement->SetAttributeBool("Compress", mbCompressed);
	apElement->SetAttributeString("File", cString::To8Char(msFile));
	apElement->SetAttributeString("Type", msType);
	apElement->SetAttributeString("Wrap", msWrap);
	apElement->SetAttributeString("AnimMode", msAnimMode);
	apElement->SetAttributeFloat("AnimFrameTime", mfFrameTime);
}

//------------------------------------------------------------------------------------

void cHplTexture::Reload()
{
	if(mbNeedsReload)
	{
		mbValid = false;

		if(msFile==_W(""))
			return;

		cEngine* pEng = mpMat->mpMatEditor->GetEditor()->GetEngine();
		cGraphics* pGfx = pEng->GetGraphics();
		cResources* pRes = pEng->GetResources();
		cMaterialManager* pMatMgr = pRes->GetMaterialManager();
		cFileSearcher* pSearcher = pRes->GetFileSearcher();
		cBitmapLoaderHandler* pLoader = pRes->GetBitmapLoaderHandler();

		tWString sFullPath = pSearcher->GetFilePath(cString::To8Char(msFile));
		//Log("Loading texture %s...", cString::To8Char(sFullPath).c_str());
		cBitmap* pBmp = pLoader->LoadBitmap(sFullPath, 0);
		if(pBmp==NULL)
		{
			//Log("failed\n");
			return;
		}

		mTimeStamp = cPlatform::FileModifiedDate(sFullPath);
		eTextureType type = GetTextureTypeFromBitmap(pBmp);
		if(mpEngTexture==NULL || mpEngTexture && type!=mpEngTexture->GetType())
		{
			if(mpEngTexture) pGfx->DestroyTexture(mpEngTexture);
			mpEngTexture = pGfx->CreateTexture(pMatMgr->GetTextureString(mUnit), type, eTextureUsage_Normal);
		}
		mpEngTexture->SetUseMipMaps(mbMipMaps);

		mbValid = (mpEngTexture!=NULL && mpEngTexture->CreateFromBitmap(pBmp));
		if(mbValid)
		{
			//Log("success\n");
			mbNeedsReload = false;
			msType = cHPLMaterial::gsTextureTypeStrings[type];
			mpEngTexture->SetFullPath(sFullPath);
		}
		
		hplDelete(pBmp);
	}
}

//------------------------------------------------------------------------------------

void cHplTexture::Update()
{
	cEngine* pEng = mpMat->mpMatEditor->GetEditor()->GetEngine();
	cGraphics* pGfx = pEng->GetGraphics();
	cResources* pRes = pEng->GetResources();
	cMaterialManager* pMatMgr = pRes->GetMaterialManager();

	if(mbUpdated)
	{
		mpMat->mbPreviewUpdated = true;

		Reload();
		if(mbValid==false)
			return;
		
		eTextureWrap wrap = pMatMgr->GetWrap(msWrap);
		mpEngTexture->SetWrapR(wrap);
		mpEngTexture->SetWrapS(wrap);
		mpEngTexture->SetWrapT(wrap);

		mpEngTexture->SetAnimMode(pMatMgr->GetAnimMode(msAnimMode));
		mpEngTexture->SetFrameTime(mfFrameTime);

		mbUpdated = false;
	}
}

//------------------------------------------------------------------------------------

void cHplTexture::CreateFromTexture(iTexture* apTexture)
{
	Reset();
	SetNeedsReload();
	SetUpdated();

	if(apTexture==NULL)
		return;

	mbEnabled = true;

	mbCompressed = apTexture->IsCompressed();
	mbMipMaps = apTexture->UsesMipMaps();
	msFile = apTexture->GetFullPath();
	msWrap = cHPLMaterial::gsWrapTypeStrings[apTexture->GetWrapR()];
	msAnimMode = cHPLMaterial::gsAnimModeStrings[apTexture->GetAnimMode()];
	mfFrameTime = apTexture->GetFrameTime();
}

//------------------------------------------------------------------------------------

bool cHplTexture::CheckFileIsUpdated()
{
	if(msFile==_W(""))
		return false;

	tWString sFullPath = this->mpMat->mpMatEditor->GetEditor()->GetEngine()->GetResources()->GetFileSearcher()->GetFilePath(cString::To8Char(msFile));
	cDate newTimeStamp = cPlatform::FileModifiedDate(sFullPath);
	bool bUpdated = (mTimeStamp!=newTimeStamp);

	if(bUpdated)
	{
		SetNeedsReload();
		SetUpdated();
	}

	return bUpdated;
}

//------------------------------------------------------------------------------------

eTextureType cHplTexture::GetTextureTypeFromBitmap(cBitmap* apBmp)
{
	if(apBmp->GetNumOfImages()==6)
		return eTextureType_CubeMap;

	const cVector3l& vSize = apBmp->GetSize();
	if(vSize.z==1)
	{
		if(vSize.y==1)
			return eTextureType_1D;
		
		if(vSize.x%2!=0 || 
			vSize.y%2!=0)
			return eTextureType_Rect;

		return eTextureType_2D;
	}
	else
		return eTextureType_3D;
}

//------------------------------------------------------------------------------------

void cHplTexture::SetEnabled(bool abX)
{
	if(mbEnabled==abX)
		return;

	mbEnabled = abX;
	SetUpdated();
}

void cHplTexture::SetFile(const tWString& asX)
{
	if(msFile==asX)
		return;

	SetNeedsReload();
	SetUpdated();
	msFile = asX;

	mbEnabled = true;
}

//------------------------------------------------------------------------------------

void cHplTexture::SetWrap(const tWString& asX)
{
	tString sWrap = cString::To8Char(asX);
	if(msWrap==sWrap)
		return;

	SetUpdated();
	msWrap = sWrap;
}

//------------------------------------------------------------------------------------

void cHplTexture::SetAnimMode(const tWString& asX)
{
	tString sAnimMode = cString::To8Char(asX);
	if(msAnimMode==sAnimMode)
		return;

	SetUpdated();
	msAnimMode = sAnimMode;
}

//------------------------------------------------------------------------------------

void cHplTexture::SetCompressed(bool abX)
{
	if(mbCompressed==abX)
		return;

	SetNeedsReload();
	SetUpdated();
	mbCompressed = abX;
}

//------------------------------------------------------------------------------------

void cHplTexture::SetUseMipMaps(bool abX)
{
	if(mbMipMaps==abX)
		return;

	SetNeedsReload();
	SetUpdated();
	mbMipMaps = abX;
}

//------------------------------------------------------------------------------------

void cHplTexture::SetFrameTime(float afX)
{
	if(mfFrameTime==afX)
		return;

	SetUpdated();
	mfFrameTime = afX;
}

//------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////
// HPL MATERIAL WRAPPER - CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

cHPLMaterial::cHPLMaterial(iEditor* apEditor) : iEdWorld(apEditor, _W("Material"), _W("mat"), "Material", hplNew(cEdDummyRoot,()))
{
	mpMatEditor = NULL;
	mpEngMaterial = NULL;

	for(int i=0;i<eMaterialTexture_LastEnum;++i)
		mvTextures.push_back(hplNew(cHplTexture,(this, (eMaterialTexture)i)));

	mpMatClass = NULL;
	mpClassInstance = NULL;
}

//------------------------------------------------------------------------------------

cHPLMaterial::~cHPLMaterial()
{
	if(mpEngMaterial)
		mpEditor->GetEngine()->GetResources()->GetMaterialManager()->Destroy(mpEngMaterial);

	for(size_t i=0;i<mvTextures.size();++i)
		hplDelete(mvTextures[i]);

	SetType(_W(""));
}

//------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////
// HPL TEXTURE WRAPPER - PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

bool cHPLMaterial::IsSaveable()
{
	bool bSaveable = false;
	for(size_t i=0;i<mvTextures.size();++i)
	{
		cHplTexture* pTex = mvTextures[i];
		if(pTex->IsEnabled()==false || pTex->IsValid()==false)
			continue;

		bSaveable = true;
		break;
	}

	return bSaveable;
}

//------------------------------------------------------------------------------------

bool cHPLMaterial::LoadFromFile(const tWString& asFilename)
{
	if(mpMatEditor->IsStandalone()==false)
		mpMatEditor->DispatchMessage(eEdModuleMsg_Reset, NULL);
	else
		mpEditor->Reset();

	cMaterialManager* pManager = mpMatEditor->GetEditor()->GetEngine()->GetResources()->GetMaterialManager();
	cMaterial* pMat = pManager->CreateMaterial(cString::To8Char(asFilename));

	if(pMat==NULL)
	{
		Log("Failed to load material\n");
		return false;
	}
	msFilename = asFilename;
	
	SetType(cString::To16Char(pMat->GetType()->GetName()));
	SetDepthTest(pMat->GetDepthTest());
	SetUseAlpha(pMat->GetUseAlphaDissolveFilter());
	SetPhysicsMaterial(cString::To16Char(pMat->GetPhysicsMaterial()));

	tWString sBlendMode = cString::To16Char(gsBlendModeStrings[pMat->GetBlendMode()]);
	if(sBlendMode==_W("None"))
		SetBlendMode(_W("Add"));
	else
		SetBlendMode(sBlendMode);

	for(int i=0;i<eMaterialTexture_LastEnum;++i)
	{
		iTexture* pTexture = pMat->GetTexture((eMaterialTexture)i);
		cHplTexture* pTextureWrapper = mvTextures[i];

		pTextureWrapper->CreateFromTexture(pTexture);
		pTextureWrapper->Update();
	}

	//////////////////////////////////////////////////
	// Load UV Animations
	ClearUVAnims();
	for(int i=0;i<pMat->GetUvAnimationNum();++i)
	{
		cMaterialUvAnimation* pAnim = pMat->GetUvAnimation(i);
		AddUVAnim(pAnim->mType, pAnim->mfSpeed, pAnim->mfAmp, pAnim->mAxis);
	}

	//////////////////////////////////////////////////
	// Load variables
	if(mpClassInstance)
	{
		cResourceVarsObject* pVars = pMat->GetVarsObject();
		mpClassInstance->LoadFromResourceVarsObject(pVars);
		hplDelete(pVars);
	}

	if(mpMatEditor->IsStandalone())
		mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldLoad, NULL);
	else
		OnWorldLoad();

	SetUpdated();
	
	pManager->Destroy(pMat);


	return true;
}


//------------------------------------------------------------------------------------

bool cHPLMaterial::SaveToFile(const tWString& asFilename)
{
	cResources* pRes = mpMatEditor->GetEditor()->GetEngine()->GetResources();
	cMaterialManager* pManager = pRes->GetMaterialManager();
	iXmlDocument* pDoc = pRes->GetLowLevel()->CreateXmlDocument();

	pDoc->SetValue("Material");
	cXmlElement* pMain = pDoc->CreateChildElement("Main");
	pMain->SetAttributeString("Type", cString::To8Char(msType));
	pMain->SetAttributeBool("DepthTest", mbDepthTest);
	pMain->SetAttributeBool("UseAlpha", mbUseAlpha);
	pMain->SetAttributeString("PhysicsMaterial", cString::To8Char(msPhysicsMat));
	pMain->SetAttributeString("BlendMode", cString::To8Char(msBlendMode));

	bool bSave = true;
	cXmlElement* pUnits = pDoc->CreateChildElement("TextureUnits");

	iMaterialType* pType = GetTypePointer();
	for(int i=0;i<pType->GetUsedTextureNum();++i)
	{
		cHplTexture* pTex = mvTextures[pType->GetUsedTexture(i)->mType];
		if(pTex->IsEnabled()==false || pTex->IsValid()==false)
			continue;

		cXmlElement* pUnit = pUnits->CreateChildElement();
		pTex->Save(pUnit);
	}

	/////////////////////////////////////////////////////
	// Save UV animations
	if(mvUVAnimations.empty()==false)
	{
		cXmlElement* pXmlAnims = pDoc->CreateChildElement("UvAnimations");
		tString sTypeStrings[] = 
		{
			"Translate",
			"Sin",
			"Rotate"
		};
		tString sAxisStrings[] = 
		{
			"X",
			"Y",
			"Z"
		};

		for(size_t i=0;i<mvUVAnimations.size();++i)
		{
			cMaterialUvAnimation* pAnim = &(mvUVAnimations[i]);
			cXmlElement* pXmlAnim = pXmlAnims->CreateChildElement("Animation");
			pXmlAnim->SetAttributeString("Type", sTypeStrings[pAnim->mType]);
			pXmlAnim->SetAttributeString("Axis", sAxisStrings[pAnim->mAxis]);
			pXmlAnim->SetAttributeFloat("Speed", pAnim->mfSpeed);
			pXmlAnim->SetAttributeFloat("Amplitude", pAnim->mfAmp);
		}
	}

	///////////////////////////////////////////
	// Save user variables
	if(mpClassInstance)
	{
		cXmlElement* pXmlUserVars = pDoc->CreateChildElement("SpecificVariables");

		mpClassInstance->Save(pXmlUserVars);
	}

	if(bSave)
	{
		pDoc->SaveToFile(asFilename);
		msFilename = asFilename;
		AddRecentFile(msFilename);

		if(mpMatEditor->IsStandalone())
			mpEditor->BroadcastModuleMsg(eEdModuleMsg_WorldSave, NULL);
		else
			OnWorldSave();


		SetUpdated();
	}

	pRes->DestroyXmlDocument(pDoc);

	UpdateMaterialInMemory(cString::To8Char(asFilename));

	return bSave;
}


//------------------------------------------------------------------------------------

void cHPLMaterial::UpdateMaterialInMemory(const tString& asName)
{
	cResources* pRes = mpMatEditor->GetEditor()->GetEngine()->GetResources();
	cMaterialManager* pMatMgr = pRes->GetMaterialManager();
	cTextureManager* pTexMgr = pRes->GetTextureManager();

	cMaterial* pMat = pMatMgr->CreateMaterial(asName);
	if(pMat==NULL)
		return;
	pMat->DecUserCount();
	if(pMat->HasUsers()==false)
		return;

	pMat->SetType(GetTypePointer());
	pMat->SetPhysicsMaterial(cString::To8Char(msPhysicsMat));
	pMat->SetBlendMode(pMatMgr->GetBlendMode(cString::To8Char(msBlendMode)));
	pMat->SetDepthTest(mbDepthTest);

	for(int i=0;i<eMaterialTexture_LastEnum;++i)
	{
		iTexture* pOldTex = pMat->GetTexture((eMaterialTexture)i);
		pMat->SetTexture((eMaterialTexture)i, NULL);

		if(pOldTex)
			pTexMgr->Destroy(pOldTex);

        iTexture* pEditingTex = mvTextures[i]->GetEngTexture();
        if(pEditingTex)
		{
			tString sName = cString::GetFileName(cString::To8Char(pEditingTex->GetFullPath()));
			bool bMipMaps = pEditingTex->UsesMipMaps();
			eTextureWrap wrap = pEditingTex->GetWrapR();
			iTexture* pNewTex = NULL;
			switch(pEditingTex->GetType())
			{
			case eTextureType_1D:
				pNewTex = pTexMgr->Create1D(sName, bMipMaps); 
				break;
			case eTextureType_2D:
				pNewTex = pTexMgr->Create2D(sName, bMipMaps);
				break;
			case eTextureType_3D:
				pNewTex = pTexMgr->Create3D(sName, bMipMaps);
				break;
			case eTextureType_CubeMap:
				pNewTex = pTexMgr->CreateCubeMap(sName, bMipMaps);
				break;
			case eTextureType_Rect:
				pNewTex = pTexMgr->Create2D(sName, bMipMaps, eTextureType_Rect);
				break;
			}
			if(pNewTex)
			{
				pNewTex->SetWrapR(wrap);
				pNewTex->SetWrapS(wrap);
				pNewTex->SetWrapT(wrap);
				pMat->SetTexture((eMaterialTexture)i, pNewTex);
			}
		}
	}
	pMat->ClearUvAnimations();
	for(int i=0;i<pMat->GetUvAnimationNum();++i)
	{
		cMaterialUvAnimation* pAnim = &(mvUVAnimations[i]);
		pMat->AddUvAnimation(pAnim->mType,pAnim->mfSpeed,pAnim->mfSpeed,pAnim->mAxis);
	}
	pMat->Compile();
}

//------------------------------------------------------------------------------------

bool cHPLMaterial::ReloadTextures()
{
	for(size_t i=0;i<mvTextures.size();++i)
	{
		cHplTexture* pTex = mvTextures[i];
		if(pTex->CheckFileIsUpdated())
		{
			Log("Texture %s was update outside editor, updating...\n", cString::To8Char(pTex->GetFile()).c_str());
			pTex->Update();
			SetUpdated();

			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------------

void cHPLMaterial::SetType(const tWString& asType)
{
	if(msType==asType)
		return;

	mbPreviewUpdated = true;
	mbTypeChanged = true;
	msType = asType;

	/////////////////////////////////////////////
	// Type has changed, update user variables
	iMaterialType* pType = GetTypePointer();

	if(mpMatClass)
	{
		hplDelete(mpMatClass);
		mpMatClass = NULL;
	}
	if(mpClassInstance)
	{
		hplDelete(mpClassInstance);
		mpClassInstance = NULL;
	}

	mpMatClass = hplNew(cEdMaterialClass,(mpMatEditor));
	if(mpMatClass->Create(pType))
		mpClassInstance = mpMatClass->CreateInstance();
	else
	{
		hplDelete(mpMatClass);
		mpMatClass = NULL;
		mpClassInstance = NULL;
	}
}

//------------------------------------------------------------------------------------

void cHPLMaterial::SetPhysicsMaterial(const tWString& asMat)
{
	if(msPhysicsMat==asMat)
		return;

	mbPreviewUpdated = true;
	msPhysicsMat = asMat;
}

//------------------------------------------------------------------------------------

void cHPLMaterial::SetDepthTest(bool abX)
{
	if(mbDepthTest==abX)
		return;

	mbPreviewUpdated = true;
	mbDepthTest = abX;
}

//------------------------------------------------------------------------------------

void cHPLMaterial::SetUseAlpha(bool abX)
{
	if(mbUseAlpha==abX)
		return;

	mbPreviewUpdated = true;
	mbUseAlpha = abX;
}

//------------------------------------------------------------------------------------

void cHPLMaterial::SetBlendMode(const tWString& asMode)
{
	if(msBlendMode==asMode)
		return;

	mbPreviewUpdated = true;
	msBlendMode = asMode;
}

//------------------------------------------------------------------------------------

void cHPLMaterial::AddUVAnim(eMaterialUvAnimation aType, float afSpeed, float afAmp, eMaterialAnimationAxis aAxis)
{
	mvUVAnimations.push_back(cMaterialUvAnimation(aType, afSpeed, afAmp, aAxis));

	mbPreviewUpdated = true;
}

//------------------------------------------------------------------------------------

void cHPLMaterial::RemoveUVAnim(int alIdx)
{
	if(alIdx<0 || alIdx>(int)mvUVAnimations.size())
		return;

	std::vector<cMaterialUvAnimation> vNewAnims;
	for(size_t i=0;i<mvUVAnimations.size();++i)
	{
		if(i!=alIdx)
			vNewAnims.push_back(mvUVAnimations[i]);
	}
	mvUVAnimations = vNewAnims;
}

//------------------------------------------------------------------------------------

void cHPLMaterial::ClearUVAnims()
{
	mvUVAnimations.clear();
}

//------------------------------------------------------------------------------------

cMaterialUvAnimation* cHPLMaterial::GetUVAnim(int alIdx)
{
	if(alIdx<0 || alIdx>(int)mvUVAnimations.size())
		return NULL;

	return &(mvUVAnimations[alIdx]);
}

//------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////
// HPL TEXTURE WRAPPER - PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------

cMaterial* cHPLMaterial::GetPreviewMaterial()
{
	if(mbPreviewUpdated)
	{
		cEngine* pEng = mpMatEditor->GetEditor()->GetEngine();
		cGraphics* pGfx = pEng->GetGraphics();
		cMaterialManager* pManager = pEng->GetResources()->GetMaterialManager();

		iMaterialType* pType = GetTypePointer();
		if(pType==NULL)
			return NULL;

		//////////////////////////////////////////////
		// Destroy previous engine material, if needed
		if(mpEngMaterial==NULL || mpEngMaterial->GetType()!=pType)
		{
			if(mpEngMaterial) pManager->Destroy(mpEngMaterial);
			mpEngMaterial = pManager->CreateCustomMaterial("PreviewMat", pType);
			mpEngMaterial->SetAutoDestroyTextures(false);
		}

		mpEngMaterial->SetDepthTest(mbDepthTest);
		mpEngMaterial->SetBlendMode(pManager->GetBlendMode(cString::To8Char(msBlendMode)));

		for(int i=0;i<eMaterialTexture_LastEnum;++i)
		{
			iTexture* pTex = NULL;
			cHplTexture* pTexWrapper = mvTextures[i];
			if(pTexWrapper->IsEnabled() && pTexWrapper->IsValid())
				pTex = pTexWrapper->GetEngTexture();
			else
				pTex = mvDefaultTextures[i];
	
			mpEngMaterial->SetTexture((eMaterialTexture)i, pTex);
		}

		mpEngMaterial->ClearUvAnimations();
		for(size_t i=0;i<mvUVAnimations.size();++i)
		{
			cMaterialUvAnimation* pAnim = &(mvUVAnimations[i]);
			mpEngMaterial->AddUvAnimation(pAnim->mType,pAnim->mfSpeed, pAnim->mfAmp, pAnim->mAxis);
		}

		if(mpClassInstance)
		{
			cResourceVarsObject* pVarsObject = mpEngMaterial->GetVarsObject();
			for(int i=0;i<mpClassInstance->GetVarInstanceNum();++i)
			{
				cEdVarInstance* pVar = mpClassInstance->GetVarInstance(i);

				pVarsObject->SetUserVariable(cString::To8Char(pVar->GetName()), cString::To8Char(pVar->GetValue()));
			}
			mpEngMaterial->LoadVariablesFromVarsObject(pVarsObject);
			hplDelete(pVarsObject);
		}

		mpEngMaterial->Compile();

		mbPreviewUpdated=false;
	}

	return mpEngMaterial;
}

//------------------------------------------------------------------------------------

void cHPLMaterial::OnInit()
{
	cTextureManager* pTexMan = mpEditor->GetEngine()->GetResources()->GetTextureManager();
	mvDefaultTextures.resize(eMaterialTexture_LastEnum);
	mvDefaultTextures[eMaterialTexture_Diffuse] = pTexMan->Create2D("editor_default_diffuse.jpg", false);
	mvDefaultTextures[eMaterialTexture_NMap] = pTexMan->Create2D("editor_rect_nrm.jpg", false);
	for(int i=eMaterialTexture_NMap+1;i<eMaterialTexture_LastEnum;++i)
		mvDefaultTextures[i] = NULL;

	iEdWorld::OnInit();
}

void cHPLMaterial::OnReset()
{
	iEdWorld::OnReset();

    mbPreviewUpdated = true;
	mbTypeChanged = true;

	msType = _W("");
	msPhysicsMat = _W("Default");
	mbDepthTest = true;
	mbUseAlpha = false;
	msBlendMode = _W("Add");

	for(size_t i=0;i<mvTextures.size();++i)
		mvTextures[i]->Reset();

	mvUVAnimations.clear();

	GetPreviewMaterial();

	OnWorldParamsChange();
}

//------------------------------------------------------------------------------------

void cHPLMaterial::OnWorldLoad()
{
	iEdWorld::OnWorldLoad();

	OnWorldParamsChange();

	mpMatEditor->msLastTexturePath = msFilename;
	mpMatEditor->mbTypeChanged = true;
	mpMatEditor->SetUpVars();
	mpMatEditor->SetUpdated();
}

void cHPLMaterial::OnWorldSave()
{
	iEdWorld::OnWorldSave();

	OnWorldParamsChange();
}

//------------------------------------------------------------------------------------

void cHPLMaterial::OnWorldParamsChange()
{
	if(mpMatEditor->IsStandalone())
		iEdWorld::OnWorldParamsChange();
	else
	{
		tWString sCaptionText = msFilename.empty()? _W("Unnamed ") + msName +_W("") : cString::GetFileNameW(msFilename);
		if(IsModified()) sCaptionText += _W(" (modified)");
		mpMatEditor->GetBG()->SetText(sCaptionText + _W(" - ") + cString::To16Char(mpEditor->GetCaption()));
	}
}

//------------------------------------------------------------------------------------

iMaterialType* cHPLMaterial::GetTypePointer()
{
	cEngine* pEng = mpMatEditor->GetEditor()->GetEngine();
	cGraphics* pGfx = pEng->GetGraphics();

	return pGfx->GetMaterialType(cString::To8Char(msType));
}


//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// cEdDummyRoot ROOT - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEdDummyRoot::cEdDummyRoot() : iEdObjectType(_W("Root"), "")
{
}

//-------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// HPL PS ROOT - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

iEdObjectData* cEdDummyRoot::CreateTypeSpecificData()
{
	return NULL;
}
/*
//-------------------------------------------------------------------------

bool cHplParticleSystem::CheckDataIsValid()
{
	if(mmapObjects.empty())
		return false;

	bool bRet = true;
	tEdObjectMapIt it = mmapObjects.begin();
	for(;it!=mmapObjects.end();++it)
	{
		cEdObjParticleEmitter* pEmitter = static_cast<cEdObjParticleEmitter*>(it->second);
		if(cEdLoader::LoadMaterial(pEmitter->GetMaterial())==false)
		{
			bRet = false;
			break;
		}
	}

	return bRet;
}
*/

//-------------------------------------------------------------------------
