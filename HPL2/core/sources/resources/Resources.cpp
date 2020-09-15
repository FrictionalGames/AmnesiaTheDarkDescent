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

#include "resources/Resources.h"

#include "resources/LowLevelResources.h"
#include "resources/FileSearcher.h"
#include "resources/ImageManager.h"
#include "resources/GpuShaderManager.h"
#include "resources/ParticleManager.h"
#include "resources/SoundManager.h"
#include "resources/FontManager.h"
#include "resources/ScriptManager.h"
#include "resources/TextureManager.h"
#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "resources/MeshLoaderHandler.h"
#include "resources/SoundEntityManager.h"
#include "resources/AnimationManager.h"
#include "resources/VideoManager.h"
#include "resources/EntFileManager.h"
#include "resources/ConfigFile.h"
#include "resources/LanguageFile.h"
#include "resources/XmlDocument.h"
#include "resources/BitmapLoaderHandler.h"
#include "resources/WorldLoaderHandler.h"
#include "resources/VideoLoaderHandler.h"
#include "resources/BinaryBuffer.h"

#include "resources/WorldLoaderHplMap.h"

#include "system/System.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/Platform.h"

#include "impl/tinyXML/tinyxml.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cResources::mbForceCacheLoadingAndSkipSaving = false;
	bool cResources::mbCreateAndLoadCompressedMaps= false; 

	//-----------------------------------------------------------------------

	cResources::cResources(iLowLevelResources *apLowLevelResources,iLowLevelGraphics *apLowLevelGraphics)
		: iUpdateable("Resources")
	{
		mpLowLevelResources = apLowLevelResources;
		mpLowLevelGraphics = apLowLevelGraphics;

		mpFileSearcher = hplNew( cFileSearcher, () );

		mpDefaultEntityLoader = NULL;
		mpDefaultAreaLoader = NULL;

		mpLanguageFile = NULL;
	}

	//-----------------------------------------------------------------------

	cResources::~cResources()
	{
		Log("Exiting Resources Module\n");
		Log("--------------------------------------------------------\n");

		STLMapDeleteAll(m_mEntityLoaders);
		STLMapDeleteAll(m_mAreaLoaders);

		STLDeleteAll(mlstXmlDocuments);
		STLDeleteAll(mlstBinBuffers);
		
		hplDelete(mpFontManager);
		hplDelete(mpScriptManager);
		hplDelete(mpParticleManager);
		hplDelete(mpSoundManager);
		hplDelete(mpMeshManager);
		hplDelete(mpMaterialManager);
		hplDelete(mpGpuShaderManager);
		hplDelete(mpImageManager);
		hplDelete(mpTextureManager);
		hplDelete(mpSoundEntityManager);
		hplDelete(mpAnimationManager);
		hplDelete(mpVideoManager);
		hplDelete(mpEntFileManager);
		
		Log(" All resources deleted\n");

		hplDelete(mpFileSearcher);
		
		hplDelete(mpMeshLoaderHandler);
		hplDelete(mpBitmapLoaderHandler);
		hplDelete(mpWorldLoaderHandler);
		hplDelete(mpVideoLoaderHandler);

		if(mpLanguageFile) hplDelete(mpLanguageFile);
		
		mlstManagers.clear();
		Log("--------------------------------------------------------\n\n");
	}

	//-----------------------------------------------------------------------
	
	//////////////////////////////////////////////////////////////////////////
	// MAP DATA VARS OBJECT
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	void cResourceVarsObject::LoadVariables(cXmlElement *apRootElem)
	{
		//////////////////////
		//Clear any existing variables
		m_mapVars.clear();

		////////////////////
		//Iterate variables and add them to map.
		cXmlNodeListIterator varIt = apRootElem->GetChildIterator();
		while(varIt.HasNext())
		{
			cXmlElement *pVarElem = varIt.Next()->ToElement();

			tString sName = pVarElem->GetAttributeString("Name");
			tString sValue = pVarElem->GetAttributeString("Value");

			m_mapVars.insert(tResourceVarMap::value_type(sName, sValue));
		}
	}

	//-----------------------------------------------------------------------

	void cResourceVarsObject::SetUserVariable(const tString& asName, const tString& asValue)
	{
		m_mapVars[asName] = asValue;
	}

	//-----------------------------------------------------------------------

	tString* cResourceVarsObject::GetUserVariable(const tString& asName)
	{
		tResourceVarMapIt it = m_mapVars.find(asName);
		if(it == m_mapVars.end()) {
			//Warning("Did not find user variable '%s'\n", asName.c_str());
			return NULL;
		}

		return &(it->second);
	}
	//-----------------------------------------------------------------------

	void cResourceVarsObject::AddVarBool(const tString& asName, bool abDefault)
	{
		SetUserVariable(asName, abDefault? "true" : "false");
	}

	void cResourceVarsObject::AddVarInt(const tString& asName, int alDefault)
	{
		SetUserVariable(asName, cString::ToString(alDefault));
	}

	void cResourceVarsObject::AddVarFloat(const tString& asName, float afDefault)
	{
		SetUserVariable(asName, cString::ToString(afDefault,-1,true));
	}

	void cResourceVarsObject::AddVarVector2f(const tString& asName, const cVector2f& avDefault)
	{
		tString sVector = avDefault.ToString();
		sVector = cString::ReplaceCharTo(sVector,":"," ");
		SetUserVariable(asName, sVector);
	}

	void cResourceVarsObject::AddVarVector3f(const tString& asName, const cVector3f& avDefault)
	{
		tString sVector = avDefault.ToString();
		sVector = cString::ReplaceCharTo(sVector,":"," ");
		SetUserVariable(asName, sVector);
	}

	void cResourceVarsObject::AddVarColor(const tString& asName, const cColor& aDefault)
	{
		tString sColor = aDefault.ToString();
		sColor = cString::ReplaceCharTo(sColor,":"," ");
		SetUserVariable(asName, sColor);
	}

	void cResourceVarsObject::AddVarString(const tString& asName, const tString& asDefault)
	{
		SetUserVariable(asName, asDefault);
	}

	//-----------------------------------------------------------------------

	bool cResourceVarsObject::GetVarBool(const tString& asName, bool abDefault)
	{
		tString *pVal = GetUserVariable(asName);
		return cString::ToBool(pVal ? pVal->c_str() : NULL,abDefault);
	}

	int cResourceVarsObject::GetVarInt(const tString& asName, int alDefault)
	{
		tString *pVal = GetUserVariable(asName);
		return cString::ToInt(pVal ? pVal->c_str() : NULL,alDefault);
	}

	float cResourceVarsObject::GetVarFloat(const tString& asName, float afDefault)
	{
		tString *pVal = GetUserVariable(asName);
		return cString::ToFloat(pVal ? pVal->c_str() : NULL,afDefault);
	}

	cVector2f cResourceVarsObject::GetVarVector2f(const tString& asName, const cVector2f& avDefault)
	{
		tString *pVal = GetUserVariable(asName);
		return cString::ToVector2f(pVal ? pVal->c_str() : NULL,avDefault);
	}

	cVector3f cResourceVarsObject::GetVarVector3f(const tString& asName, const cVector3f& avDefault)
	{
		tString *pVal = GetUserVariable(asName);
		return cString::ToVector3f(pVal ? pVal->c_str() : NULL,avDefault);
	}

	cColor cResourceVarsObject::GetVarColor(const tString& asName, const cColor& aDefault)
	{
		tString *pVal = GetUserVariable(asName);
		return cString::ToColor(pVal ? pVal->c_str() : NULL,aDefault);
	}

	tString cResourceVarsObject::GetVarString(const tString& asName, const tString& asDefault)
	{
		tString *pVal = GetUserVariable(asName);
		return cString::ToString(pVal ? pVal->c_str() : NULL,asDefault);
	}

	//-----------------------------------------------------------------------
	

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void cResources::Init(	cGraphics* apGraphics,cSystem *apSystem, cSound* apSound, cScene *apScene,
							cGui *apGui, cPhysics *apPhysics)
	{
		Log("Initializing Resources Module\n");
		Log("--------------------------------------------------------\n");

		mpLowLevelSystem = apSystem->GetLowLevel();

		Log(" Creating loader handlers \n");

		mpMeshLoaderHandler = hplNew( cMeshLoaderHandler,(this, apScene) );
		mpBitmapLoaderHandler = hplNew( cBitmapLoaderHandler,(this, apGraphics) );
		mpWorldLoaderHandler = hplNew( cWorldLoaderHandler,(this, apGraphics,apScene,apPhysics) );
		mpVideoLoaderHandler = hplNew( cVideoLoaderHandler,(this, apGraphics) );

		Log(" Creating resource managers\n");

		mpImageManager = hplNew( cImageManager,(this,mpLowLevelGraphics,mpLowLevelSystem) );
		mlstManagers.push_back(mpImageManager);
		mpGpuShaderManager = hplNew( cGpuShaderManager,(mpFileSearcher,mpLowLevelGraphics,mpLowLevelResources,mpLowLevelSystem) );
		mlstManagers.push_back(mpGpuShaderManager);
		mpParticleManager = hplNew( cParticleManager,(apGraphics, this) );
		mlstManagers.push_back(mpParticleManager);
		mpSoundManager = hplNew( cSoundManager,(apSound, this) );
		mlstManagers.push_back(mpParticleManager);
		mpFontManager = hplNew( cFontManager,(apGraphics,apGui, this) );
		mlstManagers.push_back(mpFontManager);
		mpScriptManager = hplNew( cScriptManager,(apSystem, this) );
		mlstManagers.push_back(mpScriptManager);
		mpTextureManager = hplNew( cTextureManager,(apGraphics, this) );
		mlstManagers.push_back(mpTextureManager);
		mpMaterialManager = hplNew( cMaterialManager,(apGraphics, this) );
		mlstManagers.push_back(mpMaterialManager);
		mpMeshManager = hplNew( cMeshManager,(apGraphics, this) );
		mlstManagers.push_back(mpMeshManager);
		mpSoundEntityManager = hplNew( cSoundEntityManager,(apSound, this) );
		mlstManagers.push_back(mpSoundEntityManager);
		mpAnimationManager = hplNew( cAnimationManager,(apGraphics, this) );
		mlstManagers.push_back(mpAnimationManager);
		mpVideoManager = hplNew( cVideoManager,(apGraphics, this) );
		mlstManagers.push_back(mpVideoManager);
		mpEntFileManager = hplNew( cEntFileManager,(this) );
		mlstManagers.push_back(mpEntFileManager);

		Log(" Adding loaders to handlers \n");

		//Low level resources will load non-propitary formats.
		mpLowLevelResources->AddBitmapLoaders(mpBitmapLoaderHandler);
		mpLowLevelResources->AddMeshLoaders(mpMeshLoaderHandler);
		mpLowLevelResources->AddVideoLoaders(mpVideoLoaderHandler);
		
		//Add properitary formats directly
        mpWorldLoaderHandler->AddLoader(hplNew(cWorldLoaderHplMap, () ));		
		
		Log("--------------------------------------------------------\n\n");
	}
	
	//-----------------------------------------------------------------------

	void cResources::Update(float afTimeStep)
	{
		tResourceManagerListIt it = mlstManagers.begin();
		for(; it != mlstManagers.end(); ++it)
		{
			iResourceManager *pManager = *it;

			pManager->Update(afTimeStep);
		}
	}

	//-----------------------------------------------------------------------

	cFileSearcher* cResources::GetFileSearcher()
	{
		return mpFileSearcher;
	}

	//-----------------------------------------------------------------------

	/**
	 * \todo File searcher should check so if the dir is allready added and if so return false and not add
	 * \param &asDir 
	 * \param &asMask 
	 * \return 
	 */
	bool cResources::AddResourceDir(const tWString &asDir, bool abAddSubDirectories, const tString &asMask)
	{
		mpFileSearcher->AddDirectory(asDir, asMask, abAddSubDirectories);
		if(iResourceBase::GetLogCreateAndDelete())
			Log(" Added resource directory '%s'\n",cString::To8Char(asDir).c_str());
		return true;
	}

	void cResources::ClearResourceDirs()
	{	
		mpFileSearcher->ClearDirectories();
	}

	//-----------------------------------------------------------------------

	bool cResources::AddLanguageFile(const tString &asFilePath, bool abAddResourceDirs, const tWString &asAltPath)
	{
		if(mpLanguageFile == NULL)
		{
			mpLanguageFile = hplNew( cLanguageFile, (this) );
		}

		return mpLanguageFile->AddFromFile(cString::To16Char(asFilePath), abAddResourceDirs, asAltPath);
	}

	void cResources::ClearTranslations()
	{
		if(mpLanguageFile)
		{
			hplDelete(mpLanguageFile);
			mpLanguageFile = NULL;
		}
	}

	const tWString& cResources::Translate(const tString& asCat, const tString& asName)
	{
		if(mpLanguageFile)
		{
			return mpLanguageFile->Translate(asCat,asName);
		}
		else
		{
			return mwsEmptyString;
		}
	}

	//-----------------------------------------------------------------------

	void cResources::AddEntityLoader(iEntityLoader* apLoader, bool abSetAsDefault)
	{
		m_mEntityLoaders.insert(tEntityLoaderMap::value_type(apLoader->GetName(), apLoader));

		if(abSetAsDefault){
			mpDefaultEntityLoader = apLoader;
		}
	}

	iEntityLoader* cResources::GetEntityLoader(const tString& asName)
	{
		tEntityLoaderMapIt it = m_mEntityLoaders.find(asName);
		if(it == m_mEntityLoaders.end()){
			Warning("No loader for type '%s' found!\n",asName.c_str());
			
			if(mpDefaultEntityLoader){
				Log("Using default loader!\n");
				return mpDefaultEntityLoader;
			}
			else {
				return NULL;
			}
		}

		return it->second;
	}

	//-----------------------------------------------------------------------


	void cResources::AddAreaLoader(iAreaLoader* apLoader, bool abSetAsDefault)
	{
		m_mAreaLoaders.insert(tAreaLoaderMap::value_type(apLoader->GetName(), apLoader));

		if(abSetAsDefault){
			mpDefaultAreaLoader = apLoader;
		}
	}

	iAreaLoader* cResources::GetAreaLoader(const tString& asName)
	{
		tAreaLoaderMapIt it = m_mAreaLoaders.find(asName);
		if(it == m_mAreaLoaders.end()){
			Warning("No loader for area type '%s' found!\n",asName.c_str());

			if(mpDefaultAreaLoader){
				Log("Using default loader!\n");
				return mpDefaultAreaLoader;
			}
			else {
				return NULL;
			}
		}

		return it->second;
	}
	
	//-----------------------------------------------------------------------

	bool cResources::LoadResourceDirsFile(const tString &asFile, const tWString &asAltPath)
	{
		iXmlDocument* pDoc = mpLowLevelResources->CreateXmlDocument();
		if(pDoc->CreateFromFile(cString::To16Char(asFile))==false)
		{
			Error("Couldn't load XML file '%s'!\n",asFile.c_str());
			hplDelete( pDoc);
			return false;
		}

		//Get the root.
		cXmlNodeListIterator it = pDoc->GetChildIterator();
		while(it.HasNext())
		{
			cXmlElement *pChildElem = it.Next()->ToElement();

			tString sPath = pChildElem->GetAttributeString("Path");
			if(sPath==""){
				continue;
			}

			bool bAddSubDirs = pChildElem->GetAttributeBool("AddSubDirs",false);

			if(sPath[0]=='/' || sPath[0]=='\\') sPath = cString::Sub(sPath, 1);

			tWString tsPath = cString::To16Char(sPath);
			if (asAltPath.length() > 0) {
				AddResourceDir(asAltPath + tsPath,bAddSubDirs);
			}
			AddResourceDir(tsPath,bAddSubDirs);
		}

		hplDelete( pDoc);
		return true;
	}

	//-----------------------------------------------------------------------

	iXmlDocument* cResources::LoadXmlDocument(const tString& asFile)
	{
		tWString sPath = mpFileSearcher->GetFilePath(asFile);
		if(sPath == _W("")){
			Error("Could not load XML document '%s'\n",asFile.c_str());
			return NULL;
		}
		
        iXmlDocument *pDoc = mpLowLevelResources->CreateXmlDocument();
		if(pDoc->CreateFromFile(sPath)==false)
		{
			Error("Could not parse/load XML from %s\n",cString::To8Char(sPath).c_str());
			hplDelete( pDoc );
			return NULL;
		}

		mlstXmlDocuments.push_back(pDoc);

		return pDoc;
	}
	
	void cResources::DestroyXmlDocument(iXmlDocument* apDoc)
	{
		STLFindAndDelete(mlstXmlDocuments,apDoc);
	}

	//-----------------------------------------------------------------------

	cBinaryBuffer* cResources::LoadBinaryBuffer(const tString& asFile)
	{
		tWString sPath = mpFileSearcher->GetFilePath(asFile);
		if(sPath == _W("")){
			Error("Could not find filepath for binary buffer '%s'\n",asFile.c_str());
			return NULL;
		}

		cBinaryBuffer *pFile = hplNew(cBinaryBuffer, (sPath) );
		if(pFile->Load()==false)
		{
			Error("Could not load binary file from %s\n",cString::To8Char(sPath).c_str());
			hplDelete( pFile );
			return NULL;
		}

		mlstBinBuffers.push_back(pFile);

		return pFile;
	}
	
	void cResources::DestroyBinaryBuffer(cBinaryBuffer* apFile)
	{
		STLFindAndDelete(mlstBinBuffers,apFile);
	}

	//-----------------------------------------------------------------------
	
	iLowLevelResources* cResources::GetLowLevel()
	{
		return mpLowLevelResources;
	}

	//-----------------------------------------------------------------------

}
