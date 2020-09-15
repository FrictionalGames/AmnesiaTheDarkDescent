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

#include "EditorWindowObjectBrowser.h"
#include "EditorThumbnailBuilder.h"
#include "EditorGrid.h"

#include "EditorWorld.h"

#include <algorithm>

//-------------------------------------------------------------------



//-------------------------------------------------------------------
//-------------------------------------------------------------------

iEditorObjectIndexEntryMeshObject::iEditorObjectIndexEntryMeshObject(iEditorObjectIndexDir* apDir) : iEditorObjectIndexEntry(apDir)
{
}

//-------------------------------------------------------------------

tWString& iEditorObjectIndexEntryMeshObject::GetMeshFileName()
{
	if(msMeshFileName==_W(""))
		msMeshFileName = GetFileNameRelPath();
	
	return msMeshFileName;
}

//-------------------------------------------------------------------

bool iEditorObjectIndexEntryMeshObject::CreateFromFile(const tWString& asFilename)
{
	if(iEditorObjectIndexEntry::CreateFromFile(asFilename)==false)
		return false;

	iEditorObjectIndex* pIndex = mpParentDir->GetIndex();
	
	cMeshEntity* pMeshEntity = CreateTempEntity(pIndex->GetEditor()->GetTempWorld());

	mlTriangleCount = pMeshEntity->GetMesh()->GetTriangleCount();

	mvBVMin = 999999999.0f;
	mvBVMax = -999999999.0f;
	for(int i=0;i<pMeshEntity->GetSubMeshEntityNum();++i)
	{
		cSubMeshEntity* pSubMeshEntity = pMeshEntity->GetSubMeshEntity(i);

		cBoundingVolume* pBV = pSubMeshEntity->GetBoundingVolume();
		cMath::ExpandAABB(mvBVMin, mvBVMax, pBV->GetMin(), pBV->GetMax());
	}
	
	return true;
}

//-------------------------------------------------------------------

bool iEditorObjectIndexEntryMeshObject::CreateFromXmlElement(cXmlElement* apElement)
{
	if(iEditorObjectIndexEntry::CreateFromXmlElement(apElement)==false)
		return false;

	mlTriangleCount = apElement->GetAttributeInt("TriCount");
	mvBVMin = apElement->GetAttributeVector3f("BVMin");
	mvBVMax = apElement->GetAttributeVector3f("BVMax");

	return true;
}

//-------------------------------------------------------------------

void iEditorObjectIndexEntryMeshObject::Save(cXmlElement* apElement)
{
	iEditorObjectIndexEntry::Save(apElement);

	apElement->SetAttributeInt("TriCount", mlTriangleCount);
	apElement->SetAttributeVector3f("BVMin", mvBVMin);
	apElement->SetAttributeVector3f("BVMax", mvBVMax);
}

//-------------------------------------------------------------------

void iEditorObjectIndexEntryMeshObject::BuildThumbnail()
{
	iEditorObjectIndex* pIndex = mpParentDir->GetIndex();
	iEditorBase* pEditor = pIndex->GetEditor();
	cEditorThumbnailBuilder* pTmbBuilder = pEditor->GetThumbnailBuilder();
	
	////////////////////////////////////////////////////////////////
	// Create thumbnail if it doesn't exist or the object was updated
	bool bIsUpdated = IsUpdated();
	if(bIsUpdated || cPlatform::FileExists(GetThumbnailFilename())==false)
	{
		pEditor->ShowLoadingWindow(_W("Loading"), _W("Creating Thumbnail"));
		cMeshEntity* pEnt = CreateTempEntity(pEditor->GetTempWorld());

		//pEditor->GetThumbnailBuilder()->BuildThumbnailFromMesh(sMeshFilename,sThumbnailFilename);
		pTmbBuilder->BuildThumbnailFromMeshEntity(pEnt, _W(""));

		if(bIsUpdated==false)
			pIndex->SetRefreshThumbnails(true);
	}
}

//-------------------------------------------------------------------

cMeshEntity* iEditorObjectIndexEntryMeshObject::CreateTempEntity(cWorld* apWorld)
{
	iEditorObjectIndex* pIndex = mpParentDir->GetIndex();
	iEditorBase* pEditor = pIndex->GetEditor();
	cResources* pResources = pEditor->GetEngine()->GetResources();

	tString sSourceFile = cString::To8Char(GetFileNameFullPath());

	cMesh* pMesh = pResources->GetMeshManager()->CreateMesh(sSourceFile);
    if(pMesh==NULL)
		return NULL;

	cMeshEntity* pMeshEntity = apWorld->CreateMeshEntity("TempObject_"+msFileName, pMesh);
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

cEditorWindowObjectBrowser::cEditorWindowObjectBrowser(iEditorEditMode* apEditMode,
													   const tWStringVec& avBaseDirs,
													   bool abAddCategoryHeaders) : iEditModeObjectCreatorWindow(apEditMode)
																						 
{
	mvBaseDirs = avBaseDirs;
	mbAddCategoryHeaders = abAddCategoryHeaders;
	
	mpPreviewEntity = NULL;
	mpCurrentIndex = NULL;
}

cEditorWindowObjectBrowser::~cEditorWindowObjectBrowser()
{
	mpObjectList->ClearItems();
	mvCurrentListedEntries.clear();

	if(mpPreviewEntity)
		mpEditor->GetEditorWorld()->GetWorld()->DestroyMeshEntity(mpPreviewEntity);

	STLMapDeleteAll(mmapObjectIndices);
}

//-------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

iEditorObjectIndexEntryMeshObject* cEditorWindowObjectBrowser::GetSelectedObject()
{
	int lIndex = mpObjectList->GetSelectedItem();

	if(lIndex==-1 || lIndex >(int)mvCurrentListedEntries.size())
		return NULL;

	return mvCurrentListedEntries[lIndex];
}

//-------------------------------------------------------------------

//-------------------------------------------------------------------

void cEditorWindowObjectBrowser::Reset()
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

bool cEditorWindowObjectBrowser::ObjectSets_OnChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	BuildObjectList();
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowObjectBrowser,ObjectSets_OnChange);

//-------------------------------------------------------------------

bool cEditorWindowObjectBrowser::ObjectList_OnChangeSelection(iWidget* apWidget, const cGuiMessageData& aData)
{
	UpdateObjectInfo();
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowObjectBrowser, ObjectList_OnChangeSelection);

//-------------------------------------------------------------------

bool cEditorWindowObjectBrowser::Input_OnTextBoxEnter(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowObjectBrowser, Input_OnTextBoxEnter);

//-------------------------------------------------------------------

bool cEditorWindowObjectBrowser::Refresh_OnPressed(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mpCurrentIndex && mpCurrentIndex->Refresh())
		UpdateObjectList();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cEditorWindowObjectBrowser, Refresh_OnPressed);

//-------------------------------------------------------------------

void cEditorWindowObjectBrowser::OnInitLayout()
{
	//////////////////////
	// Set up layout
    mpBGFrame->SetSize(cVector2f(200,700));

	cVector3f vPos;
	cVector2f vSize;

	///////////////////////////////////////////////////
	// Object Selection (sets, list..)
	mpSelectionGroup = mpSet->CreateWidgetGroup(cVector3f(5,8,0.1f), cVector2f(190,205) , _W(""), mpBGFrame);

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
	mpInfoGroup = mpSet->CreateWidgetGroup(cVector3f(5,220,0.1f), cVector2f(190,280),_W("Object info"), mpBGFrame);

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

void cEditorWindowObjectBrowser::BuildObjectSetList()
{
	mvDirectories.clear();
	mpObjectSets->ClearItems();

	for(int i=0;i<(int)mvBaseDirs.size();++i)
        BuildObjectSetListHelper(mvBaseDirs[i],0);	
}

//-------------------------------------------------------------------

void cEditorWindowObjectBrowser::BuildObjectSetListHelper(const tWString& asFolder, int alLevel)
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

void cEditorWindowObjectBrowser::WriteInvalidFileListToFile(tWString& asFolder, tWStringList& alstInvalidFiles)
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

void cEditorWindowObjectBrowser::BuildObjectList()
{
	mpCurrentIndex = CreateIndex(mpObjectSets->GetText());

	UpdateObjectList();
}

//-------------------------------------------------------------------

void cEditorWindowObjectBrowser::ClearObjectList()
{
	mvCurrentListedEntries.clear();
	mpObjectList->ClearItems();
	UpdateObjectInfo();
}

//-------------------------------------------------------------------

void cEditorWindowObjectBrowser::UpdateObjectList()
{
	ClearObjectList();
	AddEntriesInDirToList(mpCurrentIndex->GetRootDir(), mvCurrentListedEntries); 
}

//-------------------------------------------------------------------

void cEditorWindowObjectBrowser::UpdateObjectInfo()
{
	iEditorObjectIndexEntryMeshObject* pObj = GetSelectedObject();
	cGui* pGui = mpEditor->GetEngine()->GetGui();

	tWString sBVSize;
	tWString sTriCount;

	cGuiGfxElement* pImg = mpThumbnail->GetImage();
	if(pImg!=NULL)
		pGui->DestroyGfx(pImg);

	cWorld* pWorld = mpEditor->GetEditorWorld()->GetWorld();

	if(mpPreviewEntity)
		pWorld->DestroyMeshEntity(mpPreviewEntity);
	mpPreviewEntity = pObj?pObj->CreateTempEntity(pWorld):NULL;

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

iEditorObjectIndex* cEditorWindowObjectBrowser::CreateIndex(const tWString& asFolder)
{
	///////////////////////////////////////////////////////////
	// First try to return an already created index
	std::map<tWString, iEditorObjectIndex*>::iterator it = mmapObjectIndices.find(asFolder);
    if(it!=mmapObjectIndices.end())
	{
		iEditorObjectIndex* pIndex = it->second;
		// Should the index be refreshed here?
		pIndex->Refresh();
		return pIndex;
	}

	mpEditor->ShowLoadingWindow(_W("Loading"), _W("Loading"));

	iEditorObjectIndex* pIndex = NULL;
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

void cEditorWindowObjectBrowser::AddEntriesInDirToList(iEditorObjectIndexDir* apDir, std::vector<iEditorObjectIndexEntryMeshObject*>& avEntries)
{
	const tIndexEntryMap& mapEntries = apDir->GetEntries();
	tIndexEntryMapConstIt itEntries = mapEntries.begin();
	for(;itEntries!=mapEntries.end();++itEntries)
	{
		iEditorObjectIndexEntry* pEntry = itEntries->second;

		mpObjectList->AddItem(pEntry->GetEntryName());
		avEntries.push_back((iEditorObjectIndexEntryMeshObject*)pEntry);
	}

	tIndexDirMap& mapSubDirs = (tIndexDirMap&)apDir->GetSubDirs();
	tIndexDirMapIt itSubDirs = mapSubDirs.begin();
	for(;itSubDirs!=mapSubDirs.end();++itSubDirs)
	{
		iEditorObjectIndexDir* pSubDir = itSubDirs->second;

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


