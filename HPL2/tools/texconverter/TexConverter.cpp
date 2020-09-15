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

using namespace hpl;

extern bool RunProgram(const tWString& sPath, const tWString &sArg);


cEngine *gpEngine=NULL;

//------------------------------------------

bool gbDirs = false;
bool gbDirs_SubDirs = false;
tWString gsFilePath = _W("");

//------------------------------------------

void ParseCommandLine(int argc, const char* argv[])
{
	for(int i=1; i<argc; ++i) //0=prog name
	{
		tString sArg = argv[i];

		//////////////////////////////
		// Type of conversion wanted
		if(i==1 && sArg == "-dir")
		{
			gbDirs = true;
		}
		//////////////////////////////
		// If sub directories shall be included
		else if(i==2 && gbDirs==true && sArg == "-subdirs")
		{
			gbDirs_SubDirs = true;
		}
		//////////////////////////////
		// The file path
		else
		{
			gsFilePath = cString::To16Char(sArg);
		}

	}
}


//------------------------------------------

tWString ToFormat(ePixelFormat aPixelFormat)
{
	if(aPixelFormat == ePixelFormat_DXT1) return _W("DXT1");
	if(aPixelFormat == ePixelFormat_DXT2) return _W("DXT2");
	if(aPixelFormat == ePixelFormat_DXT3) return _W("DXT3");
	if(aPixelFormat == ePixelFormat_DXT4) return _W("DXT4");
	if(aPixelFormat == ePixelFormat_DXT5) return _W("DXT5");

	return _W("");
}

int GetMipMapNum(int alW, int alH)
{
	int lNum = 1;
	while(alW>1 || alH>1)
	{
		alW = alW >> 1;
		alH = alH >> 1;
		lNum++;
	}
	return lNum;
}
 

bool ConvertFile(const tWString &asFile)
{
	//Check so file exists
	if(cPlatform::FileExists(asFile)==false)
	{
		Error("Could not find file %s\n", cString::To8Char(asFile).c_str());
		return false;
	}

	printf(" Converting '%s'....\n", cString::GetFileName(cString::To8Char(asFile)).c_str());
	unsigned long lStartTime = cPlatform::GetApplicationTime();

	//Check so file is proper
	cBitmap* pBitMap = gpEngine->GetResources()->GetBitmapLoaderHandler()->LoadBitmap(asFile, 0);
	if(	pBitMap==NULL)
	{
		printf(" Could not load bitmap!\n");
		return false;
	}
	if(pBitMap->GetNumOfImages()>1)
	{
		printf(" Too many images (cubemap)!\n");
		return false;	
	}
	if(pBitMap->GetNumOfMipMaps() > 1)
	{
		printf(" Already have mipmaps!!\n");
		return false;
	}
	ePixelFormat pixelFormat = pBitMap->GetPixelFormat();
	if(PixelFormatIsCompressed(pixelFormat)==false)
	{
		printf(" Not compressed!!\n");
		return false;
	}
	
	
	///////////////////////////////
	//Get Argument
	tWString sArg;

	sArg += _W(" -f ")+ToFormat(pixelFormat);
	sArg += _W(" -mf TRIANGLE");
	//sArg += _W(" -m ")+ cString::ToStringW(GetMipMapNum(pBitMap->GetWidth(), pBitMap->GetHeight())); //<- skip so tool chooses!
	//sArg += _W(" -sx _test_"); //Only when testing!
	sArg += _W(" -nologo");
	sArg += _W(" \"")+asFile+_W("\"");

	//Log("Arg: '%s'\n", cString::To8Char(sArg).c_str());

	printf(" - - - - - -\n");
	///////////////////////////////
    //Run converter
	if(RunProgram(_W("texconv_dx.exe"), sArg)==false)
	{
		printf(" fail.\n");
		return false;
	}
	printf(" - - - - - -\n");

	/////////////////////////
	//Wait for converter to be done.
	//TODO:

	printf(" done! (%dms)\n", cPlatform::GetApplicationTime()-lStartTime);
	return true;
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

void Init()
{
}

void Exit()
{

}

//------------------------------------------

int main(int argc, const char* argv[] )
{
	cEngineInitVars vars;
	gpEngine = CreateHPLEngine(eHplAPI_OpenGL, 0, &vars);
	
	ParseCommandLine(argc, argv);
	Init();

	printf("-------- TEX CONVERSION STARTED! -----------\n\n");

	if(gbDirs)	ConvertInDirs();
	else		ConvertFile();

	printf("\n-------- TEX CONVERSION DONE! -----------\n");		
	
	Exit();
	DestroyHPLEngine(gpEngine);
	
	return 0;
}
int hplMain(const tString &asCommandline){ return -1;}

