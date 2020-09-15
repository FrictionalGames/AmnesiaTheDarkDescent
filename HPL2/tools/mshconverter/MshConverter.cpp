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

#include "hpl.h"

#include "impl/MeshLoaderMSH.h"
#include "impl/MeshLoaderCollada.h"
#include "resources/WorldLoaderHplMap.h"


using namespace hpl;

cEngine *gpEngine=NULL;

cMeshLoaderMSH *gpMeshLoaderMSH=NULL;
cMeshLoaderCollada *gpMeshLoaderCollada=NULL;

//------------------------------------------

bool gbDirs = false;
bool gbDirs_SubDirs = false;
int glFileType = 0;		//0 = model, 1=anim, 2=map
tWString gsFilePath = _W("");
bool gbForce = false;

//Was messy to get working, skipping:
bool gbGenerateAIPaths=false;
tWString gsPathNodeSetupFile = _W("");

//------------------------------------------

class cPathNodeData
{
public:
	tString msName;
	tString msNodeName;
	cVector3f mvBodySize;
	bool mbNodeAtCenter;
	int mlMinEdges;
	int mlMaxEdges;
	float mfMaxEdgeDistance;
	float mfMaxHeight;
};
std::vector<cPathNodeData> gvPathNodeData;

//--------------------------------------------------------------------------------


//Skipping this for now!
class cSimpleObjectLoader : public cEntityLoader_Object
{
public:
	cSimpleObjectLoader(const tString &asName) : cEntityLoader_Object(asName)
	{
	}

	void BeforeLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars)
	{

	}
	void AfterLoad(cXmlElement *apRootElem, const cMatrixf &a_mtxTransform,cWorld *apWorld, cResourceVarsObject *apInstanceVars)
	{

	}
};

//--------------------------------------------------------------------------------


class cSimplePathNodeLoader : public iAreaLoader
{
public:
	cSimplePathNodeLoader(const tString& asName) : iAreaLoader(asName)
	{

	}

	void Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld)
	{
		apWorld->AddAINode(asName,alID, "Default", a_mtxTransform.GetTranslation()+cVector3f(0,0.05f,0));
	}
};

//------------------------------------------

void ParseCommandLine(const tString &asCommandLine)
{
	tStringVec args;
	tString sSepp = " ";
	cString::GetStringVec(asCommandLine, args,&sSepp);

	bool bCatchNextAsPathNodeFile=false;


	for(tStringVecIt it = args.begin(); it != args.end(); ++it)
	{
		tString sArg = *it;

		//Skipping node building for now!
		/*//////////////////////////////
		// Cath the pathnode setup file
		if(bCatchNextAsPathNodeFile)
		{
			gsPathNodeSetupFile = cString::To16Char(sArg);
			
			bCatchNextAsPathNodeFile = false;
			continue;
		}*/

		//////////////////////////////
		// Type of conversion wanted
		if(sArg == "-dir")
		{
			gbDirs = true;
		}
		//////////////////////////////
		// If sub directories shall be included
		else if(sArg == "-subdirs")
		{
			gbDirs_SubDirs = true;
		}
		//////////////////////////////
		// If it is animation we want to load
		else if(sArg == "-anim")
		{
			glFileType = 1;
		}
		//////////////////////////////
		// If it is animation we want to load
		else if(sArg == "-map")
		{
			glFileType = 2;
		}
		else if(sArg == "-force")
		{
			gbForce = true;
		}
		/*else if(sArg == "-pathnodesetup")
		{
			gbGenerateAIPaths = true;
			bCatchNextAsPathNodeFile = true;
		}*/
		//////////////////////////////
		// The file path
		else
		{
			gsFilePath = cString::To16Char(sArg);
		}

	}
	if (!gbDirs && gbDirs_SubDirs) {
		gbDirs_SubDirs = false;
	}
}

//------------------------------------------

int GetCacheFileWantedVersion(int alFileType)
{
	if(alFileType == 0) return MSH_FORMAT_VERSION;
	if(alFileType == 1) return MSH_FORMAT_VERSION;
	if(alFileType == 2) return MAP_CACHE_FORMAT_VERSION;

	return 0;
}

int GetFileVersion(const tWString &asFile)
{
	FILE *pFile = cPlatform::OpenFile(asFile, _W("rb"));
	if(pFile==NULL) return -1;
    
	int lOut;
	fread(&lOut, sizeof(int), 1, pFile); //Magic number
	fread(&lOut, sizeof(int), 1, pFile); // File version
	fclose(pFile);

	return lOut;
}

tWString GetCacheFileExt(int alFileType)
{
	if(alFileType == 0) return _W("msh");
	if(alFileType == 1) return _W("anm");
	if(alFileType == 2) return _W("map_cache");
	
	return _W("non_existing_ext");
}

//------------------------------------------

void GenerateAIPaths(cWorld *apWorld)
{
	for(size_t i=0; i<gvPathNodeData.size(); ++i)
	{
		cPathNodeData& data = gvPathNodeData[i];

		apWorld->CreateAINodeContainer( data.msName, data.msNodeName, 
			data.mvBodySize, data.mbNodeAtCenter,
			data.mlMinEdges , data.mlMaxEdges , 
			data.mfMaxEdgeDistance, data.mfMaxHeight);
	}
}

//------------------------------------------

bool ConvertFile(const tWString &asFile)
{
	//Check so file exists
	if(cPlatform::FileExists(asFile)==false)
	{
		Error("Could not find file %s\n", cString::To8Char(asFile).c_str());
		return false;
	}
	
	//Check if cache file exists, is newer and correct version. 
	//If so, skip it
	tWString sCacheFileExt = GetCacheFileExt(glFileType);
	tWString sMSHPath = cString::SetFileExtW(asFile, sCacheFileExt);
	if(	gbForce == false &&
		cPlatform::FileExists(sMSHPath) && 
		cPlatform::FileModifiedDate(sMSHPath) > cPlatform::FileModifiedDate(asFile) &&
		GetFileVersion(sMSHPath) == GetCacheFileWantedVersion(glFileType))
	{
		return false;
	}

	//If forcing, remove previous cache file
	if(gbForce && cPlatform::FileExists(sMSHPath))
	{
		cPlatform::RemoveFile(sMSHPath);
	}

	printf(" Converting '%s'....", cString::GetFileName(cString::To8Char(asFile)).c_str());
	unsigned long lStartTime = cPlatform::GetApplicationTime();

    bool bFailed = false;

	//Convert Map
	if(glFileType == 2)
	{
		tWorldLoadFlag lFlags =0;
		
		if(gbGenerateAIPaths)
		{
			//Do nothing?
		}
		else
		{
			lFlags |= eWorldLoadFlag_NoEntities;
		}
		//lFlags |= eWorldLoadFlag_NoGameEntities;
		//lFlags |= eWorldLoadFlag_FastStaticLoad;

		cWorld *pWorld = gpEngine->GetResources()->GetWorldLoaderHandler()->LoadWorld(asFile, lFlags);
		if(pWorld)
		{
			//if(gbGenerateAIPaths) GenerateAIPaths(pWorld);

			gpEngine->GetScene()->DestroyWorld(pWorld);
		}
		else
		{
			bFailed = true;
		}
	}
	//Convert Animation
	else if(glFileType == 1)
	{
		cAnimation *pAnim = gpMeshLoaderCollada->LoadAnimation(asFile);
		if(pAnim)	{
			gpMeshLoaderMSH->SaveAnimation(pAnim, sMSHPath);
			hplDelete(pAnim);
		}
		else		bFailed = true;
	}
	//Convert Mesh
	else
	{
		cMesh *pMesh = gpMeshLoaderCollada->LoadMesh(asFile,eMeshLoadFlag_NoMaterial);
		if(pMesh)	{
			gpMeshLoaderMSH->SaveMesh(pMesh, sMSHPath);
			hplDelete(pMesh);
		}
		else		bFailed = true;
	}

	if(bFailed)	printf(" failed!\n");
	else		printf(" done! (%lums)\n", cPlatform::GetApplicationTime()-lStartTime);

	return !bFailed;
}



//------------------------------------------

void ConvertFilesInDir(const tWString &asDir, const tWString &asMask)
{
	tWStringList lstFiles;
	cPlatform::FindFilesInDir(lstFiles, asDir, asMask);

	//////////////////////////
	//Iterate files and convert
	if(lstFiles.empty()==false) printf("Current Dir: '%s'\n", cString::To8Char(asDir).c_str());
	for(tWStringListIt it = lstFiles.begin(); it != lstFiles.end(); ++it)
	{
		ConvertFile(cString::SetFilePathW(*it, asDir) );
	}
	
	if(gbDirs_SubDirs==false) return;
	
	//////////////////////////
	//Iterate folders and convert
	tWStringList lstFolders;
	cPlatform::FindFoldersInDir(lstFolders, asDir, false);
	for(tWStringListIt it = lstFolders.begin(); it != lstFolders.end(); ++it)
	{
		//printf("\n");
		ConvertFilesInDir(cString::SetFilePathW(*it, asDir), asMask);
	}
}

void ConvertInDirs()
{
	if(gsFilePath == _W(""))
	{
		printf("No path specified!\n");
		return;
	}

	tWString sDir = cString::GetFilePathW(gsFilePath);
	tWString sMask = cString::GetFileNameW(gsFilePath);
	
	ConvertFilesInDir(sDir, sMask);
}

//------------------------------------------

void ConvertFile()
{
	if(gsFilePath == _W(""))
	{
		printf("No file specified!\n");
		return;
	}

	ConvertFile(gsFilePath);
}

//------------------------------------------

void LoadPathNodeDataFile(const tWString &asFilePath)
{
	///////////////////////
	// Load Document
	iXmlDocument *pDoc = gpEngine->GetResources()->GetLowLevel()->CreateXmlDocument();
	if(pDoc->CreateFromFile(asFilePath)==false)
	{
		Error("Could not parse/load XML from %s\n",cString::To8Char(asFilePath).c_str());
		hplDelete( pDoc );
		return;
	}

	///////////////////////
	// Load the node types
	cXmlNodeListIterator childIt = pDoc->GetChildIterator();
	while(childIt.HasNext())
	{
		cXmlElement *pElem = childIt.Next()->ToElement();

		cPathNodeData pathNodeData;
		pathNodeData.msName = pElem->GetAttributeString("Name","");
		pathNodeData.msNodeName = pElem->GetAttributeString("NodeName","");
		pathNodeData.mvBodySize = pElem->GetAttributeVector3f("BodySize",0);
		pathNodeData.mbNodeAtCenter = pElem->GetAttributeBool("NodeAtCenter",false);
		pathNodeData.mlMinEdges = pElem->GetAttributeInt("MinEdges",0);
		pathNodeData.mlMaxEdges = pElem->GetAttributeInt("MaxEdges",0);
		pathNodeData.mfMaxEdgeDistance = pElem->GetAttributeFloat("MaxEdgeDistance",0);
		pathNodeData.mfMaxHeight = pElem->GetAttributeFloat("MaxHeight",0);

		gvPathNodeData.push_back(pathNodeData);
	}

	hplDelete( pDoc );
}

//------------------------------------------

void Init()
{
	gpMeshLoaderMSH = static_cast<cMeshLoaderMSH*>(gpEngine->GetResources()->GetMeshLoaderHandler()->GetLoaderForFile(".msh"));
	gpMeshLoaderCollada = static_cast<cMeshLoaderCollada*>(gpEngine->GetResources()->GetMeshLoaderHandler()->GetLoaderForFile(".dae"));

	gpEngine->GetPhysics()->LoadSurfaceData("materials.cfg");

	if(gbGenerateAIPaths)
	{
		gpEngine->GetResources()->AddEntityLoader(hplNew(cSimpleObjectLoader,("Object")),true);
		gpEngine->GetResources()->AddAreaLoader(hplNew(cSimplePathNodeLoader,("PathNode")));
	}

	if(glFileType == 2)
	{
		gpEngine->GetResources()->LoadResourceDirsFile("resources.cfg");
		gpEngine->GetResources()->GetMaterialManager()->SetDisableRenderDataLoading(true);
	}

	//Normal loading for maps!
	if(glFileType != 2)
		gpMeshLoaderCollada->SetLoadAndSaveMSHFormat(false);

	if(gbGenerateAIPaths)
		LoadPathNodeDataFile(gsPathNodeSetupFile);
}

//------------------------------------------

void Exit()
{

}

//------------------------------------------

#ifdef WIN32
	#include <Windows.h>

#endif

#ifdef WIN32
	int main(int argc, const char* argv[] )
	{
		tString asCommandLine;
		for(int i=1; i<argc; ++i)
		{
			asCommandLine += argv[i];
			if(i!=argc-1) asCommandLine += " ";
		}

#else
	int hplMain(const tString &asCommandLine)
	{
#endif

	//Load blackbox
#ifdef WIN32
	HINSTANCE hBlackBoxLib = LoadLibrary( "BlackBox.dll" );
#endif

	cEngineInitVars vars;
	gpEngine = CreateHPLEngine(eHplAPI_OpenGL, 0, &vars);
	
	ParseCommandLine(asCommandLine);
	Init();

	printf("-------- MSH CONVERSION STARTED! -----------\n\n");

	if(gbDirs)	ConvertInDirs();
	else		ConvertFile();

	printf("\n-------- MSH CONVERSION DONE! -----------\n");		
	
	Exit();
	DestroyHPLEngine(gpEngine);

#ifdef WIN32
	if(hBlackBoxLib) FreeLibrary(hBlackBoxLib);
#endif
	
	return 0;
}

#ifdef WIN32
	int hplMain(const tString &asCommandLine){return -1;}
#endif

#ifdef __APPLE__
extern "C" int SDL_main(int argc, char *argv[]);
int main(int argc, char * argv[]) {
    return SDL_main(argc, argv);
}
#endif
