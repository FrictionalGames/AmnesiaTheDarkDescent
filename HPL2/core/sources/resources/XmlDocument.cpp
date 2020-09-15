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

#include "resources/XmlDocument.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// NODE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iXmlNode::iXmlNode(eXmlNodeType aType, iXmlNode *apParent, const tString& asValue)
	{
		mType = aType;
		msValue = asValue;	
		mpParent = apParent;
	}
	//-----------------------------------------------------------------------

	iXmlNode::~iXmlNode()
	{
		DestroyChildren();
	}

	//-----------------------------------------------------------------------

	cXmlElement* iXmlNode::ToElement()
	{
		if(mType == eXmlNodeType_Element)	return static_cast<cXmlElement*> ( this );
		else								return NULL;
	}

	cXmlElement* iXmlNode::GetFirstElement()
	{
		return static_cast<cXmlElement*>(GetFirstOfType(eXmlNodeType_Element));
	}

	cXmlElement* iXmlNode::GetFirstElement(const tString& asName)
	{
		return static_cast<cXmlElement*>(GetFirstOfType(eXmlNodeType_Element, asName));
	}

	cXmlElement * iXmlNode::CreateChildElement(const tString& asName)
	{
		cXmlElement *pElement = hplNew(cXmlElement, (asName,this));

		AddChild(pElement);

		return pElement;
	}

	//-----------------------------------------------------------------------

	void iXmlNode::AddChild(iXmlNode* apNode)
	{
		mlstChildren.push_back(apNode);
	}
	
	void iXmlNode::DestroyChild(iXmlNode* apNode)
	{
		STLFindAndDelete(mlstChildren, apNode);
	}

	//-----------------------------------------------------------------------

	iXmlNode* iXmlNode::GetFirstOfType(eXmlNodeType aType)
	{
		if(mlstChildren.empty()) return NULL;
		
		tXmlNodeListIt it = mlstChildren.begin();
		iXmlNode *pNode = *it;
		while(pNode->GetType() != eXmlNodeType_Element)
		{
			++it;
			if(it == mlstChildren.end()) return NULL;
			pNode = *it;
		}

		return pNode;
	}

	//-----------------------------------------------------------------------

	iXmlNode* iXmlNode::GetFirstOfType(eXmlNodeType aType, const tString& asName)
	{
		if(mlstChildren.empty()) return NULL;

		tXmlNodeListIt it = mlstChildren.begin();
		iXmlNode *pNode = *it;
		while(	pNode->GetType() != eXmlNodeType_Element ||
				pNode->GetValue() != asName )
		{
			++it;
			if(it == mlstChildren.end()) return NULL;
			pNode = *it;
		}

		return pNode;
	}

	//-----------------------------------------------------------------------

	cXmlNodeListIterator iXmlNode::GetChildIterator()
	{
		return cXmlNodeListIterator(&mlstChildren);
	}

	//-----------------------------------------------------------------------

	void iXmlNode::DestroyChildren()
	{
		STLDeleteAll(mlstChildren);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// ELEMENT
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cXmlElement::cXmlElement(const tString& asName, iXmlNode* apParent) : iXmlNode(eXmlNodeType_Element,apParent,asName)
	{
	}

	cXmlElement::~cXmlElement()
	{

	}
	//-----------------------------------------------------------------------

	const char* cXmlElement::GetAttribute(const tString& asName)
	{
		tAttributeMapIt it = m_mapAttributes.find(asName);
		if(it != m_mapAttributes.end())
		{
			return it->second.c_str();
		}
		return NULL;
	}

	//-----------------------------------------------------------------------

	tString cXmlElement::GetAttributeString(const tString& asName, const tString& asDefault)
	{
		const char* pString = GetAttribute(asName);
		if(pString)	return pString;
		else		return asDefault;
	}

	float cXmlElement::GetAttributeFloat(const tString& asName, float afDefault)
	{
		const char* pString = GetAttribute(asName);
		return cString::ToFloat(pString,afDefault);
	}
	int cXmlElement::GetAttributeInt(const tString& asName, int alDefault)
	{
		const char* pString = GetAttribute(asName);
		return cString::ToInt(pString,alDefault);
	}
	bool cXmlElement::GetAttributeBool(const tString& asName, bool abDefault)
	{
		const char* pString = GetAttribute(asName);
		return cString::ToBool(pString,abDefault);
	}
	cVector2f cXmlElement::GetAttributeVector2f(const tString& asName, const cVector2f& avDefault)
	{	
		const char* pString = GetAttribute(asName);
		return cString::ToVector2f(pString,avDefault);
		
	}
	cVector3f cXmlElement::GetAttributeVector3f(const tString& asName, const cVector3f& avDefault)
	{
		const char* pString = GetAttribute(asName);
		return cString::ToVector3f(pString,avDefault);
	}
	cColor cXmlElement::GetAttributeColor(const tString& asName, const cColor& aDefault)
	{
		const char* pString = GetAttribute(asName);
		return cString::ToColor(pString,aDefault);
	}

	//-----------------------------------------------------------------------

	void cXmlElement::SetAttribute(const tString& asName, const char* asVal)
	{
		tAttributeMapIt it = m_mapAttributes.find(asName);
		if(it != m_mapAttributes.end())
		{
			it->second = asVal;
		}
		else
		{
			m_mapAttributes.insert(tAttributeMap::value_type(asName, asVal));
		}
	}

	//-----------------------------------------------------------------------

	static char gvTempStringBuffer[512];

	void cXmlElement::SetAttributeString(const tString& asName, const tString& asVal)
	{
		SetAttribute(asName, asVal.c_str());
	}
	void cXmlElement::SetAttributeFloat(const tString& asName, float afVal)
	{
		sprintf(gvTempStringBuffer, "%g", afVal);
		SetAttribute(asName, gvTempStringBuffer);
	}
	void cXmlElement::SetAttributeInt(const tString& asName, int alVal)
	{
		sprintf(gvTempStringBuffer, "%d", alVal);
		SetAttribute(asName, gvTempStringBuffer);
	}
	void cXmlElement::SetAttributeBool(const tString& asName, bool abVal)
	{
		SetAttribute(asName, abVal ? "true" : "false");
	}
	void cXmlElement::SetAttributeVector2f(const tString& asName, const cVector2f& avVal)
	{
		SetAttribute(asName,avVal.ToFileString().c_str());
	}
	void cXmlElement::SetAttributeVector3f(const tString& asName, const cVector3f& avVal)
	{
		SetAttribute(asName, avVal.ToFileString().c_str());
	}
	void cXmlElement::SetAttributeColor(const tString& asName, const cColor& aVal)
	{
		SetAttribute(asName, aVal.ToFileString().c_str());
	}
	
	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iXmlDocument::iXmlDocument(const tString& asName) : cXmlElement(asName, NULL)
	{
		msFile = _W("");
	}

	iXmlDocument::~iXmlDocument()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool iXmlDocument::CreateFromFile(const tWString& asPath)
	{
		bool bRet = LoadDataFromFile(asPath);
		if(bRet==false)
		{
			Log("Failed parsing of XML document %s in line %d, column %d: %s\n", cString::To8Char(asPath).c_str(), 
																					mlErrorRow, mlErrorCol, msErrorDesc.c_str());
		}

		msFile = asPath;

		return bRet;
	}

	//-----------------------------------------------------------------------
	
	bool iXmlDocument::Save()
	{
		return SaveToFile(msFile);
	}

	bool iXmlDocument::SaveToFile(const tWString& asPath)
	{
		return SaveDataToFile(asPath);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	
	//-----------------------------------------------------------------------
}
