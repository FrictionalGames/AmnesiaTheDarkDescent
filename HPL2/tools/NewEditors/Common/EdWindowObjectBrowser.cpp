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

#include "../Common/EdWindowObjectBrowser.h"
#include "../Common/EdThumbnailBuilder.h"
#include "../Common/EdGrid.h"

#include "../Common/EdWorld.h"

#include <algorithm>

//-------------------------------------------------------------------



//-------------------------------------------------------------------
//-------------------------------------------------------------------

iEdObjectIndexEntryMeshObject::iEdObjectIndexEntryMeshObject(iEdObjectIndexDir* apDir) : iEdObjectIndexEntry(apDir)
{
}

//-------------------------------------------------------------------

tWString& iEdObjectIndexEntryMeshObject::GetMeshFileName()
{
	if(msMeshFileName==_W(""))
		msMeshFileName = GetFileNameRelPath();
	
	return msMeshFileName;
}

//-------------------------------------------------------------------

bool iEdObjectIndexEntryMeshObject::CreateFromFile(const tWString& asFilename)
{
	if(iEdObjectIndexEntry::CreateFromFile(asFilename)==false)
		return false;

	iEdObjectIndex* pIndex = mpParentDir->GetIndex();
	
	cMeshEntity* pMeshEntity = CreateTempObject(GetDir()->GetIndex()->GetEditor()->GetWorld()->GetTempEngWorld());

	mlTriangleCount = pMeshEntity->GetMesh()->GetTriangleCount();

	mvBVMin = pMeshEntity->GetBoundingVolume()->GetLocalMin();
	mvBVMax = pMeshEntity->GetBoundingVolume()->GetLocalMax();
	
	return true;
}

//-------------------------------------------------------------------

bool iEdObjectIndexEntryMeshObject::CreateFromXmlElement(cXmlElement* apElement)
{
	if(iEdObjectIndexEntry::CreateFromXmlElement(apElement)==false)
		return false;

	mlTriangleCount = apElement->GetAttributeInt("TriCount");
	mvBVMin = apElement->GetAttributeVector3f("BVMin");
	mvBVMax = apElement->GetAttributeVector3f("BVMax");

	return true;
}

//-------------------------------------------------------------------

void iEdObjectIndexEntryMeshObject::Save(cXmlElement* apElement)
{
	iEdObjectIndexEntry::Save(apElement);

	apElement->SetAttributeInt("TriCount", mlTriangleCount);
	apElement->SetAttributeVector3f("BVMin", mvBVMin);
	apElement->SetAttributeVector3f("BVMax", mvBVMax);
}

//-------------------------------------------------------------------

void iEdObjectIndexEntryMeshObject::BuildThumbnail()
{
	iEdObjectIndex* pIndex = mpParentDir->GetIndex();
	iEditor* pEditor = pIndex->GetEditor();
	cEdThumbnailBuilder* pTmbBuilder = pEditor->GetThumbnailBuilder();
	
	////////////////////////////////////////////////////////////////
	// Create thumbnail if it doesn't exist or the object was updated
	bool bIsUpdated = IsUpdated();
	if(bIsUpdated || cPlatform::FileExists(GetThumbnailFilename())==false)
	{
		//pEditor->ShowLoadingWindow(_W("Loading"), _W("Creating Thumbnail"));
		cMeshEntity* pEnt = CreateTempObject(pTmbBuilder->GetWorld());

		pTmbBuilder->BuildThumbnailFromMeshEntity(pEnt, _W(""));

		if(bIsUpdated==false)
			pIndex->SetRefreshThumbnails(true);
	}
}

//-------------------------------------------------------------------

cMeshEntity* iEdObjectIndexEntryMeshObject::CreateTempObject(cWorld* apWorld)
{
	iEdObjectIndex* pIndex = mpParentDir->GetIndex();
	iEditor* pEditor = pIndex->GetEditor();
	cResources* pResources = pEditor->GetEngine()->GetResources();

	tString sSourceFile = cString::To8Char(GetFileNameFullPath());

	cMesh* pMesh = pResources->GetMeshManager()->CreateMesh(sSourceFile);
    if(pMesh==NULL)
		return NULL;

	cMeshEntity* pMeshEntity = apWorld->CreateMeshEntity("TempObject_"+msFileName, pMesh);
	Log("Created TempObject %p\n", pMeshEntity);

	pMeshEntity->SetSourceFile(sSourceFile);
	pMeshEntity->SetActive(false);
	pMeshEntity->SetVisible(false);

	return pMeshEntity;
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

cEdWindowObjectBrowser::cEdWindowObjectBrowser(iEdEditMode* apEditMode,
													   const tWStringVec& avBaseDirs,
													   bool abAddCategoryHeaders) : iEdScnObjCreatorPane(apEditMode)
																						 
{
	mvBaseDirs = avBaseDirs;
	mbAddCategoryHeaders = abAddCategoryHeaders;
	
	mpPreviewEntity = NULL;
	mpCurrentIndex = NULL;
}

cEdWindowObjectBrowser::~cEdWindowObjectBrowser()
{
	mpObjectList->ClearItems();
	mvCurrentListedEntries.clear();

	//if(mpPreviewEntity)
	//	mpEditor->GetWorld()->GetEngWorld()->DestroyMeshEntity(mpPreviewEntity);

	STLMapDeleteAll(mmapObjectIndices);
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

iEdObjectIndexEntryMeshObject* cEdWindowObjectBrowser::GetSelectedObject()
{
	int lIndex = mpObjectList->GetSelectedItem();

	if(lIndex==-1 || lIndex >(int)mvCurrentListedEntries.size())
		return NULL;

	return mvCurrentListedEntries[lIndex];
}

//-------------------------------------------------------------------

//-------------------------------------------------------------------

void cEdWindowObjectBrowser::Reset()
{
	mpObjectList->SetSelectedItem(-1);
	UpdateObjectInfo();
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

//-------------------------------------------------------------------

bool cEdWindowObjectBrowser::ObjectSets_OnChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	BuildObjectList();
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowObjectBrowser,ObjectSets_OnChange);

//-------------------------------------------------------------------

bool cEdWindowObjectBrowser::ObjectList_OnChangeSelection(iWidget* apWidget, const cGuiMessageData& aData)
{
	UpdateObjectInfo();
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowObjectBrowser, ObjectList_OnChangeSelection);

//-------------------------------------------------------------------

bool cEdWindowObjectBrowser::Input_OnTextBoxEnter(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowObjectBrowser, Input_OnTextBoxEnter);

//-------------------------------------------------------------------

bool cEdWindowObjectBrowser::Refresh_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mpCurrentIndex && mpCurrentIndex->Refresh())
		UpdateObjectList();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEdWindowObjectBrowser, Refresh_OnPressed);

//-------------------------------------------------------------------

void cEdWindowObjectBrowser::OnCreateLayout()
{
	//////////////////////
	// Set up layout
	iEdScnObjCreatorPane::OnCreateLayout();
	SetHeight(700);

	cVector3f vPos;
	cVector2f vSize;

	///////////////////////////////////////////////////
	// Object Selection (sets, list..)
	mpSelectionGroup = mpSet->CreateWidgetGroup(cVector3f(5,8,0.1f), cVector2f(190,205) , _W(""), GetBG());

	mpObjectSets = mpSet->CreateWidgetComboBox(cVector3f(10,10,0.1f), cVector2f(170,25),_W(""), mpSelectionGroup);
	mpObjectSets->AddCallback(eGuiMessage_SelectionChange,this, kGuiCallback(ObjectSets_OnChange));
	mpObjectSets->SetCanEdit(false);

	mpObjectList = mpSet->CreateWidgetListBox(cVector3f(10,45,0.1f), cVector2f(170,120),mpSelectionGroup);
	mpObjectList->SetDefaultFontSize(11);
	mpObjectList->SetBackgroundZ(0.001f);
	mpObjectList->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(ObjectList_OnChangeSelection));

	mpButtonRefresh = mpSet->CreateWidgetButton(cVector3f(10, 170, 0.1f), cVector2f(170, 25), _W("Refresh"), mpSelectionGroup);
	mpButtonRefresh->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Refresh_OnPressed));

    ///////////////////////////////////////////////
	// Object info: BV Size, poly count, thumbnail
	mpInfoGroup = mpSet->CreateWidgetGroup(cVector3f(5,220,0.1f), cVector2f(190,280),_W("Object info"), GetBG());

	vPos = cVector3f(5,170,0.1f);
	vSize = cVector2f(50,25);

	for(int i=0;i<2;++i)
	{
		mvLabelBVSize[i] = mpSet->CreateWidgetLabel(vPos,vSize,_W(""), mpInfoGroup);
		mvLabelBVSize[i]->SetDefaultFontSize(12.5f);

		mvLabelPolyCount[i] = mpSet->CreateWidgetLabel(vPos + cVector3f(0,40,0),vSize,_W(""), mpInfoGroup);		
		mvLabelPolyCount[i]->SetDefaultFontSize(12.5f);

		vPos.y += 20;
		
		
	}

	mvLabelBVSize[0]->SetText(_W("BB Size:"));
	mvLabelPolyCount[0]->SetText(_W("Polygon Count:"));

	mpLabelThumbnail = mpSet->CreateWidgetLabel(cVector3f(30,15,0.1f), cVector2f(50,25), _W("Thumbnail"), mpInfoGroup);
	mpLabelThumbnail->SetDefaultFontSize(12.5f);

	cWidgetFrame* pThumbFrame = mpSet->CreateWidgetFrame(cVector3f(30,35,0.5f),128,true,mpInfoGroup);
	pThumbFrame->SetBackGroundColor(cColor(0.4f,1));
	pThumbFrame->SetBackgroundZ(0);
	pThumbFrame->SetDrawBackground(true);

	mpThumbnail = mpSet->CreateWidgetImage("",cVector3f(0,0,1.1f),128,eGuiMaterial_Diffuse, false, pThumbFrame);

	BuildObjectSetList();
}

//-------------------------------------------------------------------

void cEdWindowObjectBrowser::BuildObjectSetList()
{
	mvDirectories.clear();
	mpObjectSets->ClearItems();

	for(int i=0;i<(int)mvBaseDirs.size();++i)
        BuildObjectSetListHelper(mvBaseDirs[i],0);	
}

//-------------------------------------------------------------------

void cEdWindowObjectBrowser::BuildObjectSetListHelper(const tWString& asFolder, int alLevel)
{
	tWStringList lstObjectDirs;

	cPlatform::FindFoldersInDir(lstObjectDirs, asFolder, false);

	tWStringListIt it = lstObjectDirs.begin();

	for(;it!=lstObjectDirs.end();++it)
	{
		tWString sDir = cString::AddSlashAtEndW(asFolder) + *it;

		tWString sItem;
		sItem.append(alLevel,_W('-'));
		sItem += *it;

		mvDirectories.push_back(sDir);
		mpObjectSets->AddItem(sItem);
	}
}

//-------------------------------------------------------------------

void cEdWindowObjectBrowser::WriteInvalidFileListToFile(tWString& asFolder, tWStringList& alstInvalidFiles)
{
	/*
	if(alstInvalidFiles.empty())
		return;

	tString sInvalidFileList = cString::To8Char(mpEditor->GetMainLookUpDir(eDir_Home)) + mpEditor->GetInvalidFilesListFilename();
	tString sFolder = cString::ReplaceCharTo(cString::To8Char(asFolder), "/", "\\");

	FILE* pFile = fopen(sInvalidFileList.c_str(), "a");
	tWStringListIt it = alstInvalidFiles.begin();

	fprintf(pFile, "Found %d invalid entries in folder %s :\n", alstInvalidFiles.size(), sFolder.c_str());
	for(;it!=alstInvalidFiles.end();++it)
	{
		tWString& sFilename = *it;

		fprintf(pFile, "\t-%s\n", cString::To8Char(sFilename).c_str());
	}
	fprintf(pFile,"\n");

	fclose(pFile);*/
}

//-------------------------------------------------------------------

void cEdWindowObjectBrowser::BuildObjectList()
{
	mpCurrentIndex = CreateIndex(mpObjectSets->GetText());

	UpdateObjectList();
}

//-------------------------------------------------------------------

void cEdWindowObjectBrowser::ClearObjectList()
{
	mvCurrentListedEntries.clear();
	mpObjectList->ClearItems();
	UpdateObjectInfo();
}

//-------------------------------------------------------------------

void cEdWindowObjectBrowser::UpdateObjectList()
{
	ClearObjectList();
	AddEntriesInDirToList(mpCurrentIndex->GetRootDir(), mvCurrentListedEntries); 
}

//-------------------------------------------------------------------

void cEdWindowObjectBrowser::UpdateObjectInfo()
{
	iEdObjectIndexEntryMeshObject* pObj = GetSelectedObject();
	cGui* pGui = mpEditor->GetEngine()->GetGui();

	tWString sBVSize;
	tWString sTriCount;

	cGuiGfxElement* pImg = mpThumbnail->GetImage();
	if(pImg!=NULL)
		pGui->DestroyGfx(pImg);

	cWorld* pWorld = mpEditor->GetWorld()->GetEngWorld();

	if(mpPreviewEntity)
		pWorld->DestroyMeshEntity(mpPreviewEntity);
	mpPreviewEntity = pObj?pObj->CreateTempObject(pWorld):NULL;

	if(pObj==NULL)
	{
		sBVSize = _W("");
		sTriCount = _W("");

		pImg = NULL;
	}
	else
	{
		cVector3f vBVSize = pObj->GetBVMax() - pObj->GetBVMin();
		
		sBVSize = _W("(");
		for(int i=0;i<3;++i)
		{
			sBVSize += cString::ToStringW(vBVSize.v[i],3,true);
			if(i!=2) sBVSize += _W(",");
		}
		sBVSize+=_W(")");

		sTriCount = cString::ToStringW(pObj->GetTriangleCount());
		tString sThumbFile = cString::To8Char(pObj->GetThumbnailFilename());
		pImg = pGui->CreateGfxImage(sThumbFile,eGuiMaterial_Diffuse);
	}

	mvLabelBVSize[1]->SetText(sBVSize);
	mvLabelPolyCount[1]->SetText(sTriCount);

	mpThumbnail->SetImage(pImg);
}

//-------------------------------------------------------------------

iEdObjectIndex* cEdWindowObjectBrowser::CreateIndex(const tWString& asFolder)
{
	///////////////////////////////////////////////////////////
	// First try to return an already created index
	std::map<tWString, iEdObjectIndex*>::iterator it = mmapObjectIndices.find(asFolder);
    if(it!=mmapObjectIndices.end())
	{
		iEdObjectIndex* pIndex = it->second;
		// Should the index be refreshed here?
		pIndex->Refresh();
		return pIndex;
	}

	//mpEditor->ShowLoadingWindow(_W("Loading"), _W("Loading"));

	iEdObjectIndex* pIndex = NULL;
	for(int i=0;i<(int)mvBaseDirs.size();++i)
	{
		tWString sFolder = cString::AddSlashAtEndW(mvBaseDirs[i]) + asFolder;
		if(cPlatform::FolderExists(sFolder)==false)
			continue;

		pIndex = CreateSpecificIndex(mpEditor, sFolder);
		if(pIndex)
		{
			mmapObjectIndices[asFolder] = pIndex;
			pIndex->Create();

			break;
		}
	}


	return pIndex;
}

//-------------------------------------------------------------------

void cEdWindowObjectBrowser::AddEntriesInDirToList(iEdObjectIndexDir* apDir, std::vector<iEdObjectIndexEntryMeshObject*>& avEntries)
{
	const tIndexEntryMap& mapEntries = apDir->GetEntries();
	tIndexEntryMapConstIt itEntries = mapEntries.begin();
	for(;itEntries!=mapEntries.end();++itEntries)
	{
		iEdObjectIndexEntry* pEntry = itEntries->second;

		mpObjectList->AddItem(pEntry->GetEntryName());
		avEntries.push_back((iEdObjectIndexEntryMeshObject*)pEntry);
	}

	tIndexDirMap& mapSubDirs = (tIndexDirMap&)apDir->GetSubDirs();
	tIndexDirMapIt itSubDirs = mapSubDirs.begin();
	for(;itSubDirs!=mapSubDirs.end();++itSubDirs)
	{
		iEdObjectIndexDir* pSubDir = itSubDirs->second;

		if(mbAddCategoryHeaders && apDir->GetParentDir()==NULL)
		{
			cWidgetItem* pDirHeader = mpObjectList->AddItem((pSubDir->IsExpanded()?_W("- "):_W("+ ")) + pSubDir->GetDirName());
			pDirHeader->SetSelectable(false);
			avEntries.push_back(NULL);
		}

		AddEntriesInDirToList(pSubDir, avEntries);
	}
}

//-------------------------------------------------------------------


