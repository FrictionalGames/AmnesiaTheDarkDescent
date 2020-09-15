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

#ifndef HPL_XML_DOCUMENT_H
#define HPL_XML_DOCUMENT_H

#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"

namespace hpl {

	//-------------------------------------
	
	enum eXmlNodeType
	{
		eXmlNodeType_Element,

		eXmlNodeType_LastEnum
	};
	
	//-------------------------------------
	
	class iXmlNode;
	class cXmlElement;

	typedef std::list<iXmlNode*> tXmlNodeList;
	typedef tXmlNodeList::iterator tXmlNodeListIt;

	typedef cSTLIterator<iXmlNode*, tXmlNodeList, tXmlNodeListIt> cXmlNodeListIterator;

	class iXmlNode
	{
	public:
		iXmlNode(eXmlNodeType aType, iXmlNode *apParent, const tString& asValue);
		virtual ~iXmlNode();
				
		const tString& GetValue(){ return msValue;}
		void SetValue(const tString& asValue){ msValue = asValue;}

		eXmlNodeType GetType() { return mType;}

		iXmlNode* GetParent(){ return mpParent;}

		cXmlElement* ToElement();
		cXmlElement* GetFirstElement();
		cXmlElement* GetFirstElement(const tString& asName);
		cXmlElement* CreateChildElement(const tString& asName="");

		void AddChild(iXmlNode* apNode);
		void DestroyChild(iXmlNode* apNode);

		iXmlNode* GetFirstOfType(eXmlNodeType aType);
		iXmlNode* GetFirstOfType(eXmlNodeType aType, const tString& asName);
		
		cXmlNodeListIterator GetChildIterator();

		void DestroyChildren();
	private:
		eXmlNodeType mType;
		tString msValue;

		iXmlNode *mpParent;

		tXmlNodeList mlstChildren;
	};
	
	//-------------------------------------
	
	typedef std::map<tString, tString> tAttributeMap;
	typedef tAttributeMap::iterator tAttributeMapIt;

	class cXmlElement : public iXmlNode
	{
	public:
		cXmlElement(const tString& asName, iXmlNode* apParent);
		virtual ~cXmlElement();
		
		const char* GetAttribute(const tString& asName);
		
		tString GetAttributeString(const tString& asName, const tString& asDefault="");
		float GetAttributeFloat(const tString& asName, float afDefault=0);
		int GetAttributeInt(const tString& asName, int alDefault=0);
		bool GetAttributeBool(const tString& asName, bool abDefault=false);
		cVector2f GetAttributeVector2f(const tString& asName, const cVector2f& avDefault=0);
		cVector3f GetAttributeVector3f(const tString& asName, const cVector3f& avDefault=0);
		cColor GetAttributeColor(const tString& asName, const cColor& aDefault=cColor(0,0));

		void SetAttribute(const tString& asName, const char* asVal);
		
		void SetAttributeString(const tString& asName, const tString& asVal);
		void SetAttributeFloat(const tString& asName, float afVal);
		void SetAttributeInt(const tString& asName, int alVal);
		void SetAttributeBool(const tString& asName, bool abVal);
		void SetAttributeVector2f(const tString& asName, const cVector2f& avVal);
		void SetAttributeVector3f(const tString& asName, const cVector3f& avVal);
		void SetAttributeColor(const tString& asName, const cColor& aVal);


		tAttributeMap* GetAttributeMap(){ return &m_mapAttributes;}
		
	private:
		tAttributeMap m_mapAttributes;
	};

	//-------------------------------------

	class iXmlDocument : public cXmlElement
	{
	public:
		iXmlDocument(const tString& asName);
		virtual ~iXmlDocument();

		void SetPath(const tWString& asPath) { msFile = asPath; }
		const tWString& GetPath() { return msFile; }
		
		bool CreateFromFile(const tWString& asPath);
		bool Save();
		bool SaveToFile(const tWString& asPath);

		const tString& GetErrorDesc() { return msErrorDesc; }
		int GetErrorRow() { return mlErrorRow; }
		int GetErrorCol() { return mlErrorCol; }

		virtual void SaveToString(tString *apDestData)=0;
		virtual bool CreateFromString(const tString& asData)=0;

	protected:
		void SaveErrorInfo(const tString& asDesc, int alRow, int alCol) { msErrorDesc = asDesc; mlErrorRow = alRow; mlErrorCol = alCol; }

	private:
		virtual bool LoadDataFromFile(const tWString& asPath)=0;
		virtual bool SaveDataToFile(const tWString& asPath)=0;
		
		tWString msFile;

		tString msErrorDesc;
		int		mlErrorRow;
		int		mlErrorCol;
	};

};
#endif // HPL_XML_DOCUMENT_H
