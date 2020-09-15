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

#ifndef HPL_RESOURCES_H
#define HPL_RESOURCES_H

#include <map>
#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "resources/ResourcesTypes.h"

#include "engine/Updateable.h"

class TiXmlElement;

namespace hpl {

	class cImageEntity;

	class iLowLevelResources;
	class iLowLevelGraphics;
	class iLowLevelSystem;
	class cSystem;
	class iResourceManager;
	class cFileSearcher;
	class cImageManager;
	class cGpuShaderManager;
	class cParticleManager;
	class cSoundManager;
	class cFontManager;
	class cScriptManager;
	class cTextureManager;
	class cMaterialManager;
	class cSoundEntityManager;
	class cAnimationManager;
	class cEntFileManager;
	class cMeshManager;
	class cVideoManager;
	class cConfigFile;
	class cArea2D;
	class cSound;
	class cMeshLoaderHandler;
	class cBitmapLoaderHandler;
	class cWorldLoaderHandler;
	class cVideoLoaderHandler;
	class cScene;
	class cGraphics;
	class iRenderable;
	class cWorld;
	class iEntity3D;
	class cLanguageFile;
	class cGui;
	class cPhysics;
	class iXmlDocument;
	class cXmlElement;
	class cBinaryBuffer;

	//-------------------------------------------------------

	typedef std::map<tString, tString> tResourceVarMap;
	typedef tResourceVarMap::iterator tResourceVarMapIt;
	
	//-------------------------------------------------------
	
	class cResourceVarsObject
	{
	public:
		void LoadVariables(cXmlElement *apRootElem);
		void SetUserVariable(const tString& asName, const tString& asValue);
		tString* GetUserVariable(const tString& asName);

		void AddVarBool(const tString& asName, bool abDefault=false);
		void AddVarInt(const tString& asName, int alDefault=0);
		void AddVarFloat(const tString& asName, float afDefault=0);
		void AddVarVector2f(const tString& asName, const cVector2f& avDefault=0);
		void AddVarVector3f(const tString& asName, const cVector3f& avDefault=0);
		void AddVarColor(const tString& asName, const cColor& aDefault=0);
		void AddVarString(const tString& asName, const tString& alDefault="");

		bool GetVarBool(const tString& asName, bool abDefault=false);
		int GetVarInt(const tString& asName, int alDefault=0);
		float GetVarFloat(const tString& asName, float afDefault=0);
		cVector2f GetVarVector2f(const tString& asName, const cVector2f& avDefault=0);
		cVector3f GetVarVector3f(const tString& asName, const cVector3f& avDefault=0);
		cColor GetVarColor(const tString& asName, const cColor& aDefault=cColor(1,1));
		tString GetVarString(const tString& asName, const tString& asDefault="");

	protected:
		tResourceVarMap m_mapVars;
	};


	//-------------------------------------------------------

	class iMapDataLoader : public cResourceVarsObject
	{
	public:
		iMapDataLoader(const tString& asName): msName(asName){}

		const tString& GetName(){ return msName;}

		
	protected:
		tString msName;
	};
	
	//-------------------------------------------------------
	
	class iEntityLoader : public iMapDataLoader
	{
	public:
		iEntityLoader(const tString& asName): iMapDataLoader(asName), mbCreatesStaticEntity(false){}
		virtual ~iEntityLoader(){}

		bool GetCreatesStaticEntity(){ return mbCreatesStaticEntity; }
		
		virtual iEntity3D* Load(const tString &asName, int alID, bool abActive, cXmlElement* apRootElem, 
								const cMatrixf &a_mtxTransform, const cVector3f &avScale, 
								cWorld *apWorld, const tString &asFileName, const tWString &asFullPath, cResourceVarsObject *apInstanceVars)=0;

	protected:
		bool mbCreatesStaticEntity;
	};

	typedef std::map<tString,iEntityLoader*> tEntityLoaderMap;
	typedef tEntityLoaderMap::iterator tEntityLoaderMapIt;

	//-------------------------------------------------------

	class iAreaLoader : public iMapDataLoader
	{
	public:
		iAreaLoader(const tString& asName): iMapDataLoader(asName), mbCreatesStaticArea(false){}
		virtual ~iAreaLoader(){}

		bool GetCreatesStaticArea(){ return mbCreatesStaticArea; }
		const tString& GetName(){ return msName;}

		virtual void Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld)=0;

	protected:
		bool mbCreatesStaticArea;
	};

	typedef std::map<tString,iAreaLoader*> tAreaLoaderMap;
	typedef tAreaLoaderMap::iterator tAreaLoaderMapIt;

	//-------------------------------------------------------

	// Should this be made a map so you can do 
	// GetManager(tString) when getting a manager?
	// This way you would be able to add your own resource types
	// easily.
	typedef std::list<iResourceManager*> tResourceManagerList;
	typedef tResourceManagerList::iterator tResourceManagerListIt;

	//-------------------------------------------------------

	
	class cResources : public iUpdateable
	{
	public:
		cResources(iLowLevelResources *apLowLevelResources,iLowLevelGraphics *apLowLevelGraphics);
		~cResources();

		void Init(cGraphics* apGraphics,cSystem *apSystem, cSound* apSound, cScene *apScene, cGui *apGui, cPhysics *apPhysics);

		void Update(float afTimeStep);

		iLowLevelResources* GetLowLevel();
		cFileSearcher* GetFileSearcher();

		bool AddResourceDir(const tWString &asDir, bool abAddSubDirectories, const tString &asMask = "*.*");
		void ClearResourceDirs();

		bool AddLanguageFile(const tString &asFilePath, bool abAddResourceDirs, const tWString &asAltPath = _W(""));
		void ClearTranslations();
		const tWString& Translate(const tString& asCat, const tString& asName);

		void AddEntityLoader(iEntityLoader* apLoader, bool abSetAsDefault=false);
		iEntityLoader* GetEntityLoader(const tString& asName);

		void AddAreaLoader(iAreaLoader* apLoader, bool abSetAsDefault=false);
		iAreaLoader* GetAreaLoader(const tString& asName);

		bool LoadResourceDirsFile(const tString &asFile, const tWString &asAltPath = _W(""));

		iXmlDocument* LoadXmlDocument(const tString& asFile);
		void DestroyXmlDocument(iXmlDocument* apDoc);

		cBinaryBuffer* LoadBinaryBuffer(const tString& asFile);
		void DestroyBinaryBuffer(cBinaryBuffer* apFile);

		cMeshLoaderHandler* GetMeshLoaderHandler(){ return mpMeshLoaderHandler;}
		cBitmapLoaderHandler* GetBitmapLoaderHandler(){ return mpBitmapLoaderHandler;}
		cWorldLoaderHandler* GetWorldLoaderHandler(){ return mpWorldLoaderHandler;}
		cVideoLoaderHandler* GetVideoLoaderHandler(){ return mpVideoLoaderHandler;}

		cImageManager* GetImageManager(){return mpImageManager;}
		cGpuShaderManager* GetGpuShaderManager(){return mpGpuShaderManager;}
		cParticleManager* GetParticleManager(){ return mpParticleManager;}
		cSoundManager* GetSoundManager(){ return mpSoundManager;}
		cFontManager* GetFontManager(){ return mpFontManager;}
		cScriptManager* GetScriptManager(){ return mpScriptManager;}
		cTextureManager* GetTextureManager(){ return mpTextureManager;}
		cMaterialManager* GetMaterialManager(){ return mpMaterialManager;}
		cMeshManager* GetMeshManager(){ return mpMeshManager;}
		cSoundEntityManager* GetSoundEntityManager(){ return mpSoundEntityManager;}
		cAnimationManager* GetAnimationManager(){ return mpAnimationManager;}
		cVideoManager* GetVideoManager(){ return mpVideoManager;}
		cEntFileManager* GetEntFileManager(){ return mpEntFileManager; }

		iLowLevelSystem* GetLowLevelSystem(){ return mpLowLevelSystem;}

		static void SetForceCacheLoadingAndSkipSaving(bool abX){ mbForceCacheLoadingAndSkipSaving = abX;}
		static bool GetForceCacheLoadingAndSkipSaving(){ return mbForceCacheLoadingAndSkipSaving ;}

		static void SetCreateAndLoadCompressedMaps(bool abX){ mbCreateAndLoadCompressedMaps = abX;}
		static bool GetCreateAndLoadCompressedMaps(){ return mbCreateAndLoadCompressedMaps ;}
		
	private:
		iLowLevelResources *mpLowLevelResources;
		iLowLevelGraphics *mpLowLevelGraphics;
		iLowLevelSystem *mpLowLevelSystem;
		cFileSearcher *mpFileSearcher;

        tResourceManagerList mlstManagers;
		cImageManager *mpImageManager;
		cGpuShaderManager *mpGpuShaderManager;
		cParticleManager* mpParticleManager;
		cSoundManager* mpSoundManager;
		cFontManager* mpFontManager;
		cScriptManager* mpScriptManager;
		cTextureManager* mpTextureManager;
		cMaterialManager* mpMaterialManager;
		cSoundEntityManager* mpSoundEntityManager;
		cAnimationManager *mpAnimationManager;
		cVideoManager *mpVideoManager;
		cEntFileManager *mpEntFileManager;

		cLanguageFile *mpLanguageFile;

		cMeshManager* mpMeshManager;
		
		cMeshLoaderHandler* mpMeshLoaderHandler;
		cBitmapLoaderHandler* mpBitmapLoaderHandler;
		cWorldLoaderHandler *mpWorldLoaderHandler;
		cVideoLoaderHandler *mpVideoLoaderHandler;
				
		tEntityLoaderMap m_mEntityLoaders;
		iEntityLoader* mpDefaultEntityLoader;

		tAreaLoaderMap m_mAreaLoaders;
		iAreaLoader* mpDefaultAreaLoader;

		tXmlDocumentList mlstXmlDocuments;
		tBinaryBufferList mlstBinBuffers;

		tWString mwsEmptyString;

		static bool mbForceCacheLoadingAndSkipSaving;
		static bool mbCreateAndLoadCompressedMaps;
	};

};
#endif // HPL_RESOURCES_H
