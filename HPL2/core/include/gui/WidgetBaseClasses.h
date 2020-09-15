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

#ifndef HPL_WIDGET_BASE_CLASSES_H
#define HPL_WIDGET_BASE_CLASSES_H

#include "gui/GuiTypes.h"

namespace hpl {

	class cWidgetMultiPropertyItem;
	class cWidgetItemProperty;
	class cWidgetItem;

	class iWidgetItemContainer;

	//--------------------------------------

	typedef std::vector<cWidgetItem*> tWidgetItemVec;
	typedef tWidgetItemVec::iterator tWidgetItemVecIt;

	typedef std::vector<cWidgetItemProperty*> tWidgetItemPropertyVec;
	typedef tWidgetItemPropertyVec::iterator tWidgetItemPropertyVecIt;

	enum eItemPropertyType
	{
		eItemPropertyType_String,
		eItemPropertyType_Image,

		eItemProperty_LastEnum
	};

	enum eItemSelectType
	{
		eItemSelectType_Select,
		eItemSelectType_Toggle,
		eItemSelectType_Deselect,
	};

	//--------------------------------------

	/////////////////////////////////////////////////////////////
	// ITEM PROPERTY CLASS
	/////////////////////////////////////////////////////////////

	//--------------------------------------

	class cWidgetItemProperty
	{
	public:
		cWidgetItemProperty(const tWString& asText);
		cWidgetItemProperty(const cGuiGfxElement* apGfx);

		tWString& GetText() { return msText; }
		cGuiGfxElement* GetGfx() { return mpGfx; }

		eItemPropertyType GetType() { return mType; }

	protected:
		tWString msText;
		cGuiGfxElement* mpGfx;

		eItemPropertyType mType;
	};

	//--------------------------------------

	/////////////////////////////////////////////////////////////
	// ITEM CLASS
	/////////////////////////////////////////////////////////////

	//--------------------------------------

	class cWidgetItem
	{
	public:
		cWidgetItem(iWidgetItemContainer* apCont);
        ~cWidgetItem();

		void SetIndex(int alIdx) { mlIndex = alIdx; }
		int GetIndex() { return mlIndex; }

		const tWString& GetText()const { return msText; }
        cWidgetItemProperty* GetProperty(int alIdx) const;

		void AddProperty(const tWString& asText);
        void AddProperty(const cGuiGfxElement* apGfx);
		
		int GetPropertyNum();

		bool IsSelectable() { return mbSelectable; }
		void SetSelectable(bool abX) { mbSelectable = abX; }
		bool IsSelected() { return mbSelectable && mbSelected; }
		void SetSelected(bool abX);

		void SetText(const tWString& asText);

		void SetUserData(void* apData) { mpUserData = apData; }
		void* GetUserData() { return mpUserData; }

		void SetUserValue(int alValue) { mlUserValue = alValue; }
		int GetUserValue() { return mlUserValue; }

	protected:
		iWidgetItemContainer* mpCont;
		int mlIndex;
		bool mbSelectable;
		bool mbSelected;

		tWString msText;
		tWidgetItemPropertyVec mvProperties;

		void* mpUserData;
		int mlUserValue;
	};

	//--------------------------------------
	
	/////////////////////////////////////////////////////////////
	// ITEM CONTAINER CLASS
	/////////////////////////////////////////////////////////////

	//--------------------------------------
	
	class iWidgetItemContainer
	{
	public:
		~iWidgetItemContainer();

		cWidgetItem* AddItem(const tString& asItem);
		cWidgetItem* AddItem(const tWString &asItem);
		cWidgetItem* AddItem();
		void RemoveItem(int alX);
		void RemoveItem(const tWString &asItem);

		void ClearItems();

		cWidgetItem* GetItem(int alX) const;
		const tWString& GetItemText(int alX) const;
		void SetItemText(int alX, const tWString& asText);
		int GetItemNum() const;
		bool HasItem(const tWString &asItem);
		bool HasItem(const tString &asItem);

		virtual void AddItemToSelection(cWidgetItem* apItem) {}
		virtual void RemoveItemFromSelection(cWidgetItem* apItem) {}

	protected:
		virtual void UpdateProperties()=0;

		tWidgetItemVec mvItems;
	};

	//--------------------------------------
	
	/////////////////////////////////////////////////////////////
	// FILE BROWSING COMPONENT CLASS
	/////////////////////////////////////////////////////////////

	//--------------------------------------

	enum eFileBrowserFileType
	{
		eFileBrowserFileType_Graphics,
		eFileBrowserFileType_Models,
		eFileBrowserFileType_Sounds,
		eFileBrowserFileType_Text,
		eFileBrowserFileType_Other,
		eFileBrowserFileType_Folder,

		eFileBrowserFileType_LastEnum
	};

	class cFileBrowserCategory
	{
	public:
		tWString msName;
		tWStringVec mvFilters;
	};

	//--------------------------------------

	typedef std::vector<cFileBrowserCategory*> tFileBrowserCategoryVec;
	typedef tFileBrowserCategoryVec::iterator tFileBrowserCategoryVecIt;

	//--------------------------------------

	class iFileBrowser
	{
	public:
		iFileBrowser(const tWString& asStartPath, bool abAddHiddenFiles = false);
		virtual ~iFileBrowser();

		void InitBrowser();

		int AddCategory(const tWString& asCategory, const tWString& asFilter=_W(""));
		void AddFilter(const int alCategoryId, const tWString& asFilter);

		void SetPathFromString(const tWString &asPath);

		bool NavigateTo(const tWString& asPath);
		bool NavigateUp();

		void GetFilesAndFoldersInCurrentPath(int alCategoryIndex, tWStringList& alstDestination);
		eFileBrowserFileType GetFileTypeByName(const tWString& asFilename);
		tWString GetHumanReadableSize(unsigned long alSize);


		tWString InterpretPath(const tWString& asPath);

		tWString GetCurrentFullPath();

		virtual void OnAddFilter()=0;
		virtual void OnNavigate()=0;

		//////////////////////////////
		// Data
		bool mbAddHiddenFiles;

		tWString msStartPath;

		tFileBrowserCategoryVec mvCategories;

		tWStringList mlstFiles;
		tWStringVec mvCurrentDirFullPath;

		tWStringVec mvSystemRootFolders;
		std::vector<tWStringVec> mvFileTypeExtensions;

        static tWString msGameDir;
        static tWString msPersonalDir;

		static tWStringVec mvSizeStrings;
	};

	//--------------------------------------

};
#endif // HPL_WIDGET_BASE_CLASSES_H
