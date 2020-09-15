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

#include "impl/XmlDocumentTiny.h"

#include "system/LowLevelSystem.h"
#include "system/Platform.h"
#include "system/String.h"

#include "impl/tinyXML/tinyxml.h"
#include <stdio.h>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cXmlDocumentTiny::SaveToString(tString *apDestData)
	{
		TiXmlDocument *pXmlDoc = hplNew( TiXmlDocument, () );

		pXmlDoc->InsertEndChild(TiXmlElement(""));
		SaveToTinyXMLData(pXmlDoc->FirstChildElement(), this);

		*apDestData = "";
		*apDestData << *pXmlDoc;
		
		hplDelete( pXmlDoc );
	}
	
	//-----------------------------------------------------------------------

	bool cXmlDocumentTiny::CreateFromString(const tString& asData)
	{
		TiXmlDocument *pXmlDoc = hplNew( TiXmlDocument, () );
		
		pXmlDoc->Parse(asData.c_str());
		if(pXmlDoc->Error())
		{
			tString sErrorDesc = tString(pXmlDoc->ErrorDesc());
			int lErrorRow = pXmlDoc->ErrorRow();
			int lErrorCol = pXmlDoc->ErrorCol();

			SaveErrorInfo(sErrorDesc, lErrorRow, lErrorCol);

			hplDelete( pXmlDoc );
			return false;
		}

		DestroyChildren();
		LoadFromTinyXMLData(pXmlDoc->FirstChildElement(), this);

		hplDelete( pXmlDoc );
		return true;
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cXmlDocumentTiny::LoadDataFromFile(const tWString& asPath)
	{
		TiXmlDocument *pXmlDoc = hplNew( TiXmlDocument, () );
		if(CreateTinyXMLFromFile(pXmlDoc,asPath)==false)
		{
			tString sErrorDesc = tString(pXmlDoc->ErrorDesc());
			int lErrorRow = pXmlDoc->ErrorRow();
			int lErrorCol = pXmlDoc->ErrorCol();

			SaveErrorInfo(sErrorDesc, lErrorRow, lErrorCol);
	
			hplDelete( pXmlDoc );
			return false;
		}
		
		DestroyChildren();
		LoadFromTinyXMLData(pXmlDoc->FirstChildElement(), this);

        hplDelete( pXmlDoc );
		return true;
	}

	//-----------------------------------------------------------------------
	
	bool cXmlDocumentTiny::SaveDataToFile(const tWString& asPath)
	{
		TiXmlDocument *pXmlDoc = hplNew( TiXmlDocument, () );


		pXmlDoc->InsertEndChild(TiXmlElement(""));
		SaveToTinyXMLData(pXmlDoc->FirstChildElement(), this);

		bool bRet = SaveTinyXMLToFile(pXmlDoc,asPath);
		hplDelete( pXmlDoc );

		return bRet;
	}	

	//-----------------------------------------------------------------------

	void cXmlDocumentTiny::LoadFromTinyXMLData(TiXmlElement* apTinyElem, cXmlElement *apDestElem)
	{
		/////////////////////////////
		//Load the attributes
		apDestElem->SetValue(apTinyElem->Value());

        TiXmlAttribute *pAttrib = apTinyElem->FirstAttribute();
		for(; pAttrib != NULL; pAttrib = pAttrib->Next())
		{
			apDestElem->SetAttribute(pAttrib->Name(), pAttrib->Value());
		}

		/////////////////////////////
		//Load the elements
		TiXmlElement *pChildElem = apTinyElem->FirstChildElement();
		for(; pChildElem != NULL; pChildElem = pChildElem->NextSiblingElement())
		{
			cXmlElement *pDestChild = apDestElem->CreateChildElement();
			
            LoadFromTinyXMLData(pChildElem, pDestChild);
		}
	}

	//-----------------------------------------------------------------------

	void cXmlDocumentTiny::SaveToTinyXMLData(TiXmlElement* apTinyElem, cXmlElement *apSrcElem)
	{
		/////////////////////////////
		//Save the attributes
		apTinyElem->SetValue(apSrcElem->GetValue().c_str());

		tAttributeMap *pAttributeMap = apSrcElem->GetAttributeMap();
		tAttributeMapIt attrIt = pAttributeMap->begin();
		for(; attrIt != pAttributeMap->end(); ++attrIt)
		{
			apTinyElem->SetAttribute(attrIt->first.c_str(), attrIt->second.c_str());
		}

		/////////////////////////////
		//Save the elements
		cXmlNodeListIterator it = apSrcElem->GetChildIterator();
		while(it.HasNext())
		{
			cXmlElement *pChild = it.Next()->ToElement();
			
			TiXmlElement tempElem(pChild->GetValue().c_str());
			TiXmlElement* pTinyChild = static_cast<TiXmlElement*>(apTinyElem->InsertEndChild(tempElem));
			
			SaveToTinyXMLData(pTinyChild, pChild);
		}

	}

	//-----------------------------------------------------------------------

	bool cXmlDocumentTiny::CreateTinyXMLFromFile(TiXmlDocument* pDoc,const tWString& asPath)
	{
		FILE *pFile = cPlatform::OpenFile(asPath, _W("rb"));
		if(pFile==NULL) return false;
		
		bool bRet = pDoc->LoadFile(pFile);

		if(pFile) fclose(pFile);

		return bRet;
	}

	//-----------------------------------------------------------------------

	bool cXmlDocumentTiny::SaveTinyXMLToFile(TiXmlDocument* pDoc,const tWString& asPath)
	{
		if(asPath == _W("")) return false;
		
		FILE *pFile = cPlatform::OpenFile(asPath, _W("w+"));
		if (!pFile) return false;

		bool bRet = pDoc->SaveFile(pFile);

		if(pFile) fclose(pFile);

		return bRet;
	}

	//-----------------------------------------------------------------------
}
