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

#include "system/SerializeClass.h"


#include "system/LowLevelSystem.h"

#include "system/String.h"
#include "system/Platform.h"

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"
#include "system/Container.h"

#include "impl/tinyXML/tinyxml.h"

#include "resources/BinaryBuffer.h"

#define ZLIB_WINAPI
#include <zlib.h>
#include <stdio.h>

namespace hpl {

	#define kSavedDataCRCKey (0x12AD11A1)
	
	//////////////////////////////////////////////////////////////////////////
	// SERIALIZEABLE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SERIALIZE SAVED CLASS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSerializeSavedClass::cSerializeSavedClass(const char* asName,const char *asParent,
							cSerializeMemberField* apMemberFields, size_t alSize,
							iSerializable* (*apCreateFunc)())
	{
		msName = asName;
		msParentName = asParent;
		mpMemberFields = apMemberFields;
		mlSize = alSize;
		mpCreateFunc = apCreateFunc;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SERIALIZE MEMBER FIELD ITERATOR
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSerializeMemberFieldIterator::cSerializeMemberFieldIterator(cSerializeSavedClass *apTopClass)
	{
		mlFieldNum =0;
		mpSavedClass = apTopClass;

		//Make sure that the first field type is not null
		if(mpSavedClass && mpSavedClass->mpMemberFields[mlFieldNum].mType == eSerializeType_NULL)
		{
			if(mpSavedClass->msParentName[0] != '\0')
			{
				cSerializeSavedClass *pClass = cSerializeClass::GetClass(mpSavedClass->msParentName);
				if(pClass) {
					mpSavedClass = pClass;
					mlFieldNum =0;
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	bool cSerializeMemberFieldIterator::HasNext()
	{
		if(mpSavedClass == NULL ||
			mpSavedClass->mpMemberFields[mlFieldNum].mType == eSerializeType_NULL)
		{
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	cSerializeMemberField* cSerializeMemberFieldIterator::GetNext()
	{
		cSerializeMemberField *pField = &mpSavedClass->mpMemberFields[mlFieldNum];
		mlFieldNum++;

		if(mpSavedClass->mpMemberFields[mlFieldNum].mType == eSerializeType_NULL)
		{
			if(mpSavedClass->msParentName[0] != '\0')
			{
				cSerializeSavedClass *pClass = cSerializeClass::GetClass(mpSavedClass->msParentName);
				if(pClass) {
					mpSavedClass = pClass;
					mlFieldNum =0;
				}
			}
		}

		return pField;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SERIALIZE CLASS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSerializeSavedClass gvSerializeTempClasses[kMaxSerializeClasses];
	int glSerializeTempClassesNum =0;

	//Define static variables
	tSerializeSavedClassMap cSerializeClass::m_mapSavedClasses;
	bool cSerializeClass::mbDataSetup = false;
	tString cSerializeClass::msTempString = "";
	char cSerializeClass::msTempCharArray[2048];

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC SERIALIZE CLASS METHODS
	//////////////////////////////////////////////////////////////////////////

	static bool gbLog=false;
	static int glTabs=0;
	static tString gsTabString="";

    const char*	GetTabs()
	{
		gsTabString ="";
		for(int i=0; i<glTabs;++i) gsTabString += "   ";
		return gsTabString.c_str();
	}

	cSerializeClass::cSerializeClass(const char* asName,const char* asParent,
									cSerializeMemberField* apMemberFields,size_t alSize,
									iSerializable* (*apCreateFunc)())
	{
		gvSerializeTempClasses[glSerializeTempClassesNum] = cSerializeSavedClass(asName,asParent,
													apMemberFields,alSize,apCreateFunc);

		glSerializeTempClassesNum++;
	}

	//-----------------------------------------------------------------------

	void cSerializeClass::SetLog(bool abX)
	{
		gbLog = abX;
	}

	bool cSerializeClass::GetLog()
	{
		return gbLog;
	}

	//-----------------------------------------------------------------------

	void cSerializeClass::PrintMembers(iSerializable* apData)
	{
		SetUpData();

		cSerializeMemberFieldIterator classIt = GetMemberFieldIterator(apData);

		Log("Members of class '%s':\n", apData->Serialize_GetTopClass().c_str());
		while(classIt.HasNext())
		{
			cSerializeMemberField *pField = classIt.GetNext();

			Log(" '%s':'%s' type: %d offset: %d\n",pField->msName.c_str(),
				ValueToString(apData,pField->mlOffset,pField->mType),
				pField->mType, pField->mlOffset);
		}
	}

	//-----------------------------------------------------------------------

	bool cSerializeClass::SaveToFile(iSerializable* apData, const tWString &asFile,const tString &asRoot, bool abCompressAndCrc)
	{
		SetUpData();

		glTabs=0;

		TiXmlDocument* pXmlDoc = hplNew( TiXmlDocument, () );

		//Create root
		TiXmlElement XmlRoot(asRoot.c_str());
		TiXmlElement* pRootElem = static_cast<TiXmlElement*>(pXmlDoc->InsertEndChild(XmlRoot));

		SaveToElement(apData,"",pRootElem);

		///////////////////////////////
		//Normal Save
		if(abCompressAndCrc==false)
		{
			FILE *pFile = cPlatform::OpenFile(asFile, _W("w+"));
			if(pFile==NULL)
			{
				Error("Unable to open serialized file '%s' as w+! Invalid filepointer returned!\n", cString::To8Char(asFile).c_str());
				return false;
			}

			// Enable buffered writing to disk
			char buffer[8*1024];
			setvbuf(pFile, buffer, _IOFBF, 8*1024);

			bool bRet = pXmlDoc->SaveFile(pFile);
			if(bRet==false)
				Error("Couldn't save class to '%s'\n", asFile.c_str());
		
			if(pFile) fclose(pFile);

			hplDelete(pXmlDoc);
			return bRet;
		}
		///////////////////////////////
		//Compressed Save
		else
		{
			/////////////////////////////
			// Get the data
			tString sData;
			sData << *pXmlDoc;

			/////////////////////////////
			// Compress the data
			cBinaryBuffer destBuffer;

			destBuffer.AddCRC_Begin();

			if(destBuffer.CompressAndAdd(&sData[0], sData.size()+1)==false)
			{
				Error("Unable to compress data for serialized data '%s'!\n", cString::To8Char(asFile).c_str());
				return false;
			}

			destBuffer.AddCRC_End(kSavedDataCRCKey);

			/////////////////////////////
			// Save the file
			if(destBuffer.Save(asFile)==false)
			{
				Error("Unable to save serialized file '%s'!\n", cString::To8Char(asFile).c_str());
				return false;
			}

			return true;
		}
	}

	//-----------------------------------------------------------------------


	void cSerializeClass::SaveToElement(iSerializable* apData,const tString &asName, TiXmlElement *apParent,
										bool abIsPointer)
	{
		SetUpData();

		if(apData==NULL) return;

		//Create element
		TiXmlElement* pClassElem=NULL;

		if(abIsPointer)
		{
			TiXmlElement XmlClassElem("class_ptr");
			pClassElem = static_cast<TiXmlElement*>(apParent->InsertEndChild(XmlClassElem));
		}
		else
		{
			TiXmlElement XmlClassElem("class");
			pClassElem = static_cast<TiXmlElement*>(apParent->InsertEndChild(XmlClassElem));
		}

		
		//Set attributes
		pClassElem->SetAttribute("type",apData->Serialize_GetTopClass().c_str());
		pClassElem->SetAttribute("name",asName.c_str());

		if(gbLog) Log("---Saving class '%s' Begin---\n",apData->Serialize_GetTopClass().c_str());

		cSerializeMemberFieldIterator classIt = GetMemberFieldIterator(apData);
		
		while(classIt.HasNext())
		{
			cSerializeMemberField *pField = classIt.GetNext();
		
			if(gbLog) Log(" Field : '%s', MainType: %d Type: %d\n",pField->msName.c_str(), pField->mMainType, pField->mType);

			switch(pField->mMainType)
			{
				// VARIABLE /////////////////////////////////
				case eSerializeMainType_Variable:
				{
					SaveVariable(pClassElem,pField, apData);
					break;
				}
				// ARRAY ////////////////////////////////////
				case eSerializeMainType_Array:
				{
					SaveArray(pClassElem,pField, apData);
					break;
				}
				// CONTAINER ////////////////////////////////////
				case eSerializeMainType_Container:
				{
					SaveContainer(pClassElem,pField, apData);
					break;
				}
			}
		}

		if(gbLog) Log("---Done saving class '%s' ---\n",apData->Serialize_GetTopClass().c_str());
  	}

	//-----------------------------------------------------------------------

	bool cSerializeClass::LoadFromFile(iSerializable* apData, const tWString &asFile, bool abCompressedAndCrc)
	{
		SetUpData();

		glTabs=0;

		//Load document
		TiXmlDocument* pXmlDoc = hplNew( TiXmlDocument,() );

		///////////////////////////////
		//Normal Load
		if(abCompressedAndCrc==false)
		{
			#ifdef WIN32
				FILE *pFile = _wfopen(asFile.c_str(),_W("rb"));
			#else
				FILE *pFile = fopen(cString::To8Char(asFile).c_str(),"rb");
			#endif

			if(pFile==NULL)
			{
				Error("Unable to open serialized file '%s' as rb! Invalid filepointer returned!\n", cString::To8Char(asFile).c_str());
				return false;
			}
			if(pXmlDoc->LoadFile(pFile)==false)
			{
				Error("Couldn't load saved class file '%s' from %s!\n",
										cString::To8Char(asFile).c_str(),
										pXmlDoc->ErrorDesc());
				if(pFile) fclose(pFile);
				hplDelete(pXmlDoc);
				return false;
			}
			if(pFile) fclose(pFile);
		}
		///////////////////////////////
		//Compressed Load
		else
		{
			////////////////////////////////
			// Get compressed data
			cBinaryBuffer compBuffer;
			if(compBuffer.Load(asFile)==false)
			{
				hplDelete(pXmlDoc);	
				Error("Unable to open serialized file '%s'!\n", cString::To8Char(asFile).c_str());
				return false;
			}

			////////////////////////////////
			// Check CRC
			if(compBuffer.CheckInternalCRC(kSavedDataCRCKey)==false)
			{
				hplDelete(pXmlDoc);	
				Error("CRC check for serialized file '%s' failed!\n", cString::To8Char(asFile).c_str());
				return false;
			}

			////////////////////////////////
			// Uncompress data
			cBinaryBuffer textBuffer;
			if(textBuffer.DecompressAndAddFromBuffer(&compBuffer, false)==false)
			{
				hplDelete(pXmlDoc);
				Error("Unable to decompress serialized data '%s'!\n", cString::To8Char(asFile).c_str());
				return false;
			}

			////////////////////
			//Load xml
			pXmlDoc->Parse(textBuffer.GetDataPointer());
		}


		//Get root
		TiXmlElement* pRootElem = pXmlDoc->RootElement();

		//Get first, there should only be ONE class at the root.
		TiXmlElement* pClassElem = pRootElem->FirstChildElement("class");

		LoadFromElement(apData,pClassElem);

		hplDelete(pXmlDoc);
		return true;
	}

	//-----------------------------------------------------------------------

	void cSerializeClass::LoadFromElement(iSerializable* apData, TiXmlElement *apElement,
											bool abIsPointer)
	{
		SetUpData();

		cSerializeSavedClass *pClass = GetClass(apData->Serialize_GetTopClass());
		if(pClass==NULL) return;

		if(gbLog) {
			Log("%sBegin class %s\n",GetTabs(),pClass->msName);
			++glTabs;
		}

		TiXmlElement *pMemberElem = apElement->FirstChildElement();
		for(; pMemberElem != NULL; pMemberElem = pMemberElem->NextSiblingElement())
		{
			tString sMainType = pMemberElem->Value();

			if(gbLog) Log("%sMember field type %s\n",GetTabs(),sMainType.c_str());

			if(sMainType == "var")
			{
				LoadVariable(pMemberElem,apData,pClass);
			}
			else if(sMainType == "array")
			{
				LoadArray(pMemberElem,apData,pClass);
			}
			else if(sMainType == "class")
			{
				LoadClass(pMemberElem,apData,pClass);
			}
			else if(sMainType == "class_ptr")
			{
				LoadClassPointer(pMemberElem,apData,pClass);
			}
			else if(sMainType == "container")
			{
				LoadContainer(pMemberElem,apData,pClass);
			}
		}

		if(gbLog) {
			--glTabs;
			Log("%sEnd class %s\n",GetTabs(),pClass->msName);
		}
	}

	//-----------------------------------------------------------------------

	cSerializeSavedClass * cSerializeClass::GetClass(const tString &asName)
	{
		SetUpData();
		
		tSerializeSavedClassMapIt it = m_mapSavedClasses.find(asName);
		if(it == m_mapSavedClasses.end()) {
			Warning("Serialize class '%s' not found!\n",asName.c_str());
			return NULL;
		}
        
		return &it->second;
	}

	//-----------------------------------------------------------------------

	cSerializeMemberFieldIterator cSerializeClass::GetMemberFieldIterator(iSerializable* apData)
	{
		cSerializeSavedClass *pClass = GetClass(apData->Serialize_GetTopClass());

		return cSerializeMemberFieldIterator(pClass);
	}

	//-----------------------------------------------------------------------

	#define ValueTypePointer(aObject, aOffset, aType)	(aType*)(((char*)aObject)+aOffset)
	#define ValuePointer(aObject, aOffset)				(((char*)aObject)+aOffset)
	#define PointerValue(aPointer,aType)				(*((aType*)aPointer))

	const char* cSerializeClass::ValueToString(void* apData, size_t alOffset, eSerializeType aType)
	{
		void *pVal = ValuePointer(apData,alOffset);

		switch(aType)
		{
			/////////// BOOL ////////////////////////////////
			case eSerializeType_Bool:
			{
				if(PointerValue(pVal,bool)==0) return "false";
				else return "true";
				break;
			}

			/////////// INT 32 ////////////////////////////////
			case eSerializeType_Int32:
			{
				sprintf(msTempCharArray, "%d", PointerValue(pVal,int));
				return msTempCharArray; break;
			}

			/////////// FLOAT 32 ////////////////////////////////
			case eSerializeType_Float32:
			{
				sprintf(msTempCharArray, "%f", PointerValue(pVal,float));
				return msTempCharArray; break;
			}

			/////////// STRING ////////////////////////////////
			case eSerializeType_String:
			{
				tString &sVal = PointerValue(pVal,tString);
				return sVal.c_str(); break;
			}

			/////////// VECTOR 2L ////////////////////////////////
			case eSerializeType_Vector2l:
			{
				cVector2l &vVec = PointerValue(pVal,cVector2l);
				sprintf(msTempCharArray, "%d %d",vVec.x, vVec.y);
				return msTempCharArray; break;
			}

			/////////// VECTOR 2F ////////////////////////////////
			case eSerializeType_Vector2f:
			{
				cVector2f &vVec = PointerValue(pVal,cVector2f);
				sprintf(msTempCharArray, "%f %f",vVec.x, vVec.y);
				return msTempCharArray; break;
			}

			/////////// VECTOR 3L ////////////////////////////////
			case eSerializeType_Vector3l:
			{
				cVector3l &vVec = PointerValue(pVal,cVector3l);
				sprintf(msTempCharArray, "%d %d %d",vVec.x, vVec.y,vVec.z);
				return msTempCharArray; break;
			}

			/////////// VECTOR 3F ////////////////////////////////
			case eSerializeType_Vector3f:
			{
				cVector3f &vVec = PointerValue(pVal,cVector3f);
				sprintf(msTempCharArray, "%f %f %f",vVec.x, vVec.y, vVec.z);
				return msTempCharArray; break;
			}

			/////////// MATRIX F ////////////////////////////////
			case eSerializeType_Matrixf:
			{
				cMatrixf &Mtx = PointerValue(pVal,cMatrixf);
				sprintf(msTempCharArray, "%f %f %f %f "
										 "%f %f %f %f "
										 "%f %f %f %f "
										 "%f %f %f %f",
										Mtx.m[0][0],Mtx.m[0][1],Mtx.m[0][2],Mtx.m[0][3],
										Mtx.m[1][0],Mtx.m[1][1],Mtx.m[1][2],Mtx.m[1][3],
										Mtx.m[2][0],Mtx.m[2][1],Mtx.m[2][2],Mtx.m[2][3],
										Mtx.m[3][0],Mtx.m[3][1],Mtx.m[3][2],Mtx.m[3][3]);
				return msTempCharArray; break;
			}

			/////////// COLOR ////////////////////////////////
			case eSerializeType_Color:
			{
				cColor Col = PointerValue(pVal,cColor);
				sprintf(msTempCharArray, "%f %f %f %f",Col.r, Col.g, Col.b, Col.a);
				return msTempCharArray; break;
			}

			/////////// RECT 2L ////////////////////////////////
			case eSerializeType_Rect2l:
			{
				cRect2l &vR = PointerValue(pVal,cRect2l);
				sprintf(msTempCharArray, "%d %d %d %d",vR.x, vR.y, vR.w, vR.h);
				return msTempCharArray; break;
			}

			/////////// RECT 2F ////////////////////////////////
			case eSerializeType_Rect2f:
				{
					cRect2f &vR = PointerValue(pVal,cRect2f);
					sprintf(msTempCharArray, "%f %f %f %f",vR.x, vR.y, vR.w, vR.h);
					return msTempCharArray; break;
				}

			/////////// PLANE F ////////////////////////////////
			case eSerializeType_Planef:
			{
				cPlanef &vP = PointerValue(pVal,cPlanef);
				sprintf(msTempCharArray, "%f %f %f %f",vP.a, vP.b, vP.c, vP.d);
				return msTempCharArray; break;
			}

			/////////// WSTRING ////////////////////////////////
			case eSerializeType_WString:
			{
				msTempString = "";
				tWString &wsString = PointerValue(pVal,tWString);
				for(size_t i=0; i<wsString.length(); ++i)
				{
					msTempString += cString::ToString((int)wsString[i]) + " ";
				}
				return msTempString.c_str(); break;
			}

		}


		return "";
	}

	//-----------------------------------------------------------------------

	void cSerializeClass::StringToValue(void* apData, size_t alOffset, eSerializeType aType,
										const char* asVal)
	{
		void *pVal = ValuePointer(apData,alOffset);

		switch(aType)
		{
			/////////// BOOL ////////////////////////////////
			case eSerializeType_Bool:
			{
				PointerValue(pVal,bool) = cString::ToBool(asVal,false);
				break;
			}

			////////// INT 32 ////////////////////////////////
			case eSerializeType_Int32:
			{
				PointerValue(pVal,int) = cString::ToInt(asVal,0);
				break;
			}
			////////// FLOAT 32 ////////////////////////////////
			case eSerializeType_Float32:
			{
				PointerValue(pVal,float) = cString::ToFloat(asVal,0);
				break;
			}
			////////// STRING ////////////////////////////////
			case eSerializeType_String:
			{
				PointerValue(pVal,tString) = cString::ToString(asVal,"");
				break;
			}

			/////////// VECTOR 2L ////////////////////////////////
			case eSerializeType_Vector2l:
			{
				tIntVec vVals; vVals.reserve(2);
				cString::GetIntVec(asVal,vVals,NULL);

				PointerValue(pVal,cVector2l).FromVec(&vVals[0]);

				break;
			}

			/////////// VECTOR 2F ////////////////////////////////
			case eSerializeType_Vector2f:
			{
				tFloatVec vVals; vVals.reserve(2);
				cString::GetFloatVec(asVal,vVals,NULL);

				PointerValue(pVal,cVector2f).FromVec(&vVals[0]);

				break;
			}

			/////////// VECTOR 3L ////////////////////////////////
			case eSerializeType_Vector3l:
			{
				tIntVec vVals; vVals.reserve(3);
				cString::GetIntVec(asVal,vVals,NULL);

				PointerValue(pVal,cVector3l).FromVec(&vVals[0]);

				break;
			}

			/////////// VECTOR 3F ////////////////////////////////
			case eSerializeType_Vector3f:
			{
				tFloatVec vVals; vVals.reserve(3);
				cString::GetFloatVec(asVal,vVals,NULL);

				PointerValue(pVal,cVector3f).FromVec(&vVals[0]);

				break;
			}

			/////////// MATRIX F ////////////////////////////////
			case eSerializeType_Matrixf:
			{
				tFloatVec vVals; vVals.reserve(16);
				cString::GetFloatVec(asVal,vVals,NULL);

				PointerValue(pVal,cMatrixf).FromVec(&vVals[0]);

				break;
			}

			/////////// COLOR ////////////////////////////////
			case eSerializeType_Color:
			{
				tFloatVec vVals; vVals.reserve(4);
				cString::GetFloatVec(asVal,vVals,NULL);

				PointerValue(pVal,cColor).FromVec(&vVals[0]);

				break;
			}

			/////////// RECT 2L ////////////////////////////////
			case eSerializeType_Rect2l:
			{
				tIntVec vVals; vVals.reserve(4);
				cString::GetIntVec(asVal,vVals,NULL);

				PointerValue(pVal,cRect2l).FromVec(&vVals[0]);

				break;
			}

			/////////// RECT 2F ////////////////////////////////
			case eSerializeType_Rect2f:
			{
				tFloatVec vVals; vVals.reserve(4);
				cString::GetFloatVec(asVal,vVals,NULL);

				PointerValue(pVal,cRect2f).FromVec(&vVals[0]);

				break;
			}

			/////////// PLANE 2F ////////////////////////////////
			case eSerializeType_Planef:
			{
				tFloatVec vVals; vVals.reserve(4);
				cString::GetFloatVec(asVal,vVals,NULL);

				PointerValue(pVal,cPlanef).FromVec(&vVals[0]);

				break;
			}

			/////////// WSTRING ////////////////////////////////
			case eSerializeType_WString:
			{
				PointerValue(pVal,tWString) = cString::Get16BitFromArray(asVal);
				break;
			}

		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC SERIALIZE CLASS METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cSerializeClass::SaveVariable(TiXmlElement *apElement, cSerializeMemberField *apField,
										iSerializable* apData)
	{
		//CLASS
		if(apField->mType == eSerializeType_Class)
		{
			void *pClassData = ValuePointer(apData,apField->mlOffset);

			SaveToElement((iSerializable*)pClassData,apField->msName,apElement);
		}
		//CLASS POINTER
		else if(apField->mType == eSerializeType_ClassPointer)
		{
			iSerializable** pFieldData = (iSerializable**)ValuePointer(apData,apField->mlOffset);

			SaveToElement(*pFieldData,apField->msName,apElement,true);
		}
		//NORMAL VAR
		else
		{
			TiXmlElement XmlVarElem("var");
			TiXmlElement* pVarElem = static_cast<TiXmlElement*>(apElement->InsertEndChild(XmlVarElem));

			pVarElem->SetAttribute("type",apField->mType);
			pVarElem->SetAttribute("name",apField->msName.c_str());
			pVarElem->SetAttribute("val",ValueToString(apData,apField->mlOffset,apField->mType));
		}
	}

	//-----------------------------------------------------------------------

	void cSerializeClass::SaveArray(TiXmlElement *apElement, cSerializeMemberField *apField,
									iSerializable* apData)
	{
		void *pArrayData = ValuePointer(apData,apField->mlOffset);

		TiXmlElement XmlArrayElem("array");
		TiXmlElement* pArrayElem = static_cast<TiXmlElement*>(apElement->InsertEndChild(XmlArrayElem));

		pArrayElem->SetAttribute("type",apField->mType);
		pArrayElem->SetAttribute("name",apField->msName.c_str());
		pArrayElem->SetAttribute("size",(int)apField->mlArraySize);


		// CLASS ////////////////////////////////////////////
		if(apField->mType == eSerializeType_Class)
		{
			cSerializeSavedClass *pClass = GetClass(((iSerializable*)pArrayData)->Serialize_GetTopClass());
			size_t lClassSize = pClass->mlSize;

			for(size_t i=0; i< apField->mlArraySize; i++)
			{
				size_t lOffset = lClassSize * i;

				SaveToElement((iSerializable*)ValuePointer(pArrayData,lOffset),"",pArrayElem);
			}
		}
		// CLASS POINTER /////////////////////////////////////
		else if(apField->mType == eSerializeType_ClassPointer)
		{
			iSerializable ** pClassDataPtr = (iSerializable **)pArrayData;

			if(*pClassDataPtr==NULL){
				Warning("Array %s is NULL!\n",apField->msName.c_str());
				return;
			}

			pArrayElem->SetAttribute("class_type",(*pClassDataPtr)->Serialize_GetTopClass().c_str());

			for(size_t i=0; i< apField->mlArraySize; i++)
			{
				size_t lOffset = sizeof(void*) * i;

				iSerializable **pValue = (iSerializable**)ValuePointer(pArrayData,lOffset);
				SaveToElement(*pValue,"",pArrayElem,true);
			}
		}
		// VARIABLE /////////////////////////////////////////
		else
		{
			for(size_t i=0; i< apField->mlArraySize; i++)
			{
				size_t lOffset = SizeOfType(apField->mType)* i;

				TiXmlElement XmlClassElem("var");
				TiXmlElement* pVarElem = static_cast<TiXmlElement*>(pArrayElem->InsertEndChild(XmlClassElem));

				pVarElem->SetAttribute("val",ValueToString(pArrayData,lOffset,apField->mType));
			}
		}
	}

	//-----------------------------------------------------------------------

	void cSerializeClass::SaveContainer(TiXmlElement *apElement, cSerializeMemberField *apField,
		iSerializable* apData)
	{
		iContainer* pCont = (iContainer*)ValuePointer(apData,apField->mlOffset);

		iContainerIterator* pContIt = pCont->CreateIteratorPtr();

		TiXmlElement XmlArrayElem("container");
		TiXmlElement* pArrayElem = static_cast<TiXmlElement*>(apElement->InsertEndChild(XmlArrayElem));

		pArrayElem->SetAttribute("type",apField->mType);
		pArrayElem->SetAttribute("name",apField->msName.c_str());

		// CLASS ////////////////////////////////////////////
		if(apField->mType == eSerializeType_Class)
		{
			pArrayElem->SetAttribute("class_type",apField->msClassName.c_str());

			while(pContIt->HasNext())
			{
				iSerializable *pData =  (iSerializable*)pContIt->NextPtr();

                SaveToElement(pData,"",pArrayElem);
			}
		}
		// CLASS POINTER /////////////////////////////////////
		else if(apField->mType == eSerializeType_ClassPointer)
		{
			while(pContIt->HasNext())
			{
				iSerializable **pData =  (iSerializable**)pContIt->NextPtr();

				SaveToElement(*pData,"",pArrayElem,true);
			}
		}
		// VARIABLE /////////////////////////////////////////
		else
		{
			while(pContIt->HasNext())
			{
				TiXmlElement XmlClassElem("var");
				TiXmlElement* pVarElem = static_cast<TiXmlElement*>(pArrayElem->InsertEndChild(XmlClassElem));

				 void *pData = const_cast<void *>(pContIt->NextPtr());

				pVarElem->SetAttribute("val",ValueToString(pData,0,apField->mType));
			}
		}

		hplDelete(pContIt);
	}


	//-----------------------------------------------------------------------


	void cSerializeClass::LoadVariable(TiXmlElement *apElement, iSerializable* apData,
										cSerializeSavedClass *apClass)
	{
		tString sName = cString::ToString(apElement->Attribute("name"),"");
		const char* sVal = apElement->Attribute("val");
		eSerializeType type = cString::ToInt(apElement->Attribute("type"),eSerializeMainType_NULL);

		if(gbLog) Log("%s Saving variable: %s val: %s type: %d\n",GetTabs(),sName.c_str(),sVal,(int)type);

		cSerializeMemberField *pField = GetMemberField(sName,apClass);
		if(pField==NULL) return;

		StringToValue(apData, pField->mlOffset,type,sVal);
   	}

	//-----------------------------------------------------------------------

	void cSerializeClass::LoadArray(TiXmlElement *apElement, iSerializable* apData,cSerializeSavedClass *apClass)
	{
		tString sName = cString::ToString(apElement->Attribute("name"),"");
		tString sClassType = cString::ToString(apElement->Attribute("class_type"),"");
		eSerializeType type = cString::ToInt(apElement->Attribute("type"),eSerializeMainType_NULL);
		size_t lSize = cString::ToInt(apElement->Attribute("size"),0);

		if(gbLog) {
			Log("%s Begin Saving array: '%s' classtype: %s type %d\n",GetTabs(),sName.c_str(),sClassType.c_str(),type);
			++glTabs;
		}

		cSerializeMemberField *pField = GetMemberField(sName,apClass);
		if(pField==NULL) return;

		void *pArrayData = ValuePointer(apData,pField->mlOffset);

		// CLASS ////////////////////////////////////////////
		if(pField->mType == eSerializeType_Class)
		{
			cSerializeSavedClass *pClass = GetClass(((iSerializable*)pArrayData)->Serialize_GetTopClass());
			size_t lClassSize = pClass->mlSize;

			size_t lCount=0;
			TiXmlElement *pVarElem = apElement->FirstChildElement();
			for(; pVarElem != NULL; pVarElem = pVarElem->NextSiblingElement(),++lCount)
			{
				size_t lOffset = lCount * lClassSize;

				LoadFromElement( (iSerializable*)ValuePointer(pArrayData,lOffset),pVarElem);
			}
		}
		// CLASS POINTER ////////////////////////////////////////////
		else if(pField->mType == eSerializeType_ClassPointer)
		{
			size_t lCount=0;
			TiXmlElement *pVarElem = apElement->FirstChildElement();
			for(; pVarElem != NULL; pVarElem = pVarElem->NextSiblingElement(),++lCount)
			{
				size_t lOffset = sizeof(void*) * lCount;
				iSerializable **pValuePtr = (iSerializable**)ValuePointer(pArrayData,lOffset);

				tString sClassType = cString::ToString(pVarElem->Attribute("type"),"");
				cSerializeSavedClass *pSavedClass = GetClass(sClassType);
				if(pSavedClass==NULL) continue;

				if(gbLog) Log("%s Element Class pointer: %s\n",GetTabs(),sClassType.c_str());

				//If NULL, then just create else delete and then create-
				//virtual pointers here...yes yes...
				if(*pValuePtr == NULL) {
					*pValuePtr = pSavedClass->mpCreateFunc();
				}
				else {
					hplDelete(*pValuePtr);
					*pValuePtr = pSavedClass->mpCreateFunc();
				}

				LoadFromElement(*pValuePtr ,pVarElem);
			}
		}
		// VARIABLE /////////////////////////////////////////
		else
		{
			size_t lCount=0;
			TiXmlElement *pVarElem = apElement->FirstChildElement();
			for(; pVarElem != NULL; pVarElem = pVarElem->NextSiblingElement(),++lCount)
			{
				const char* sVal = pVarElem->Attribute("val");

				if(gbLog) Log("%s Element variable val '%s'\n",GetTabs(),sVal);

				StringToValue(pArrayData,lCount * SizeOfType(type),type,sVal);
			}
		}

		if(gbLog) {
			--glTabs;
			Log("%s End Saving array: %s\n",GetTabs(),sName.c_str());
		}
	}

	//-----------------------------------------------------------------------

	void cSerializeClass::LoadClass(TiXmlElement *apElement, iSerializable* apData,cSerializeSavedClass *apClass)
	{
		tString sName = cString::ToString(apElement->Attribute("name"),"");

		cSerializeMemberField *pField = GetMemberField(sName,apClass);
		if(pField==NULL) return;

		iSerializable *pClassData = (iSerializable*)ValuePointer(apData,pField->mlOffset);

		if(gbLog) Log("%s Saving class name: '%s' top class: '%s'\n",GetTabs(),sName.c_str(),pClassData->Serialize_GetTopClass().c_str());

		LoadFromElement(pClassData,apElement);
	}

	//-----------------------------------------------------------------------

	void cSerializeClass::LoadClassPointer(TiXmlElement *apElement, iSerializable* apData,cSerializeSavedClass *apClass)
	{
		tString sName = cString::ToString(apElement->Attribute("name"),"");
		tString sType = cString::ToString(apElement->Attribute("type"),"");
		bool bNull = cString::ToBool(apElement->Attribute("null"),false);

		cSerializeMemberField *pField = GetMemberField(sName,apClass);
		if(pField==NULL) return;

		iSerializable **pClassDataPtr = (iSerializable**)ValuePointer(apData,pField->mlOffset);

		if(gbLog) Log("%s Saving classpointer name: '%s' type: '%s' null: %d\n",GetTabs(),sName.c_str(),
																				sType.c_str(),bNull?1:0);

		//TODO: Question is here if previous data should be deleted.
		if(bNull)
		{
			*pClassDataPtr = NULL;
			return;
		}

		//If it is NULL create new, else assume it is already created.
		if(*pClassDataPtr == NULL)
		{
			cSerializeSavedClass* pNewClass = GetClass(sType);
			*pClassDataPtr = pNewClass->mpCreateFunc();
		}

		LoadFromElement(*pClassDataPtr,apElement);
	}

	//-----------------------------------------------------------------------

	void cSerializeClass::LoadContainer(TiXmlElement *apElement, iSerializable* apData,cSerializeSavedClass *apClass)
	{
		tString sName = cString::ToString(apElement->Attribute("name"),"");
		eSerializeType type = cString::ToInt(apElement->Attribute("type"),eSerializeMainType_NULL);

		cSerializeMemberField *pField = GetMemberField(sName,apClass);
		if(pField==NULL) return;

		iContainer *pCont = (iContainer*)ValuePointer(apData,pField->mlOffset);

		if(gbLog){
			Log("%s Begin save container name: '%s' type %d\n",GetTabs(),sName.c_str(),type);
			++glTabs;
		}

		// CLASS ////////////////////////////////////////////
		if(pField->mType == eSerializeType_Class)
		{
			tString sClassType = cString::ToString(apElement->Attribute("class_type"),"");
			pCont->Clear();

			cSerializeSavedClass *pSavedClass = GetClass(sClassType);
			if(pSavedClass==NULL) return;

			TiXmlElement *pVarElem = apElement->FirstChildElement();
			for(; pVarElem != NULL; pVarElem = pVarElem->NextSiblingElement())
			{
				if(gbLog) Log("%sCreating element class %s\n",GetTabs(),sClassType.c_str());

				iSerializable *pData = pSavedClass->mpCreateFunc();

				LoadFromElement(pData,pVarElem);
				pCont->AddVoidClass(pData);

				hplDelete(pData);
			}
		}
		// CLASS POINTER ////////////////////////////////////////////
		else if(pField->mType == eSerializeType_ClassPointer)
		{
			//This should really be smart pointers.
			//Delete all and clear
			if(gbLog) Log("%sClearing container and deleting elements\n",GetTabs());

			iContainerIterator *pContIt = pCont->CreateIteratorPtr();
			while(pContIt->HasNext()){
				iSerializable *pContData = (iSerializable*)pContIt->NextPtr();
				hplDelete(pContData);
			}
			hplDelete(pContIt);
            if ( pCont->Size() > 0 )
            {
			    pCont->Clear();
            }

			TiXmlElement *pVarElem = apElement->FirstChildElement();
			for(; pVarElem != NULL; pVarElem = pVarElem->NextSiblingElement())
			{
				tString sClassType = cString::ToString(pVarElem->Attribute("type"),"");
				cSerializeSavedClass *pSavedClass = GetClass(sClassType);
				if(pSavedClass==NULL) continue;

				if(gbLog) Log("%s Container member class pointer '%s'\n",GetTabs(),sClassType.c_str());

				iSerializable *pData = pSavedClass->mpCreateFunc();

				LoadFromElement(pData,pVarElem,true);
				pCont->AddVoidPtr((void**)&pData);
			}
		}
		// VARIABLE /////////////////////////////////////////
		else
		{
			pCont->Clear();

			TiXmlElement *pVarElem = apElement->FirstChildElement();
			for(; pVarElem != NULL; pVarElem = pVarElem->NextSiblingElement())
			{
				const char* sVal = pVarElem->Attribute("val");
				void *pData = hplMalloc(SizeOfType(type));

				if(gbLog) Log("%s Element var val '%s' type: %d\n",GetTabs(),sVal,type);

				StringToValue(pData,0,type,sVal);
				pCont->AddVoidClass(pData);

				hplFree(pData);
			}
		}

		if(gbLog){
			--glTabs;
			Log("%s End save container name: '%s'\n",GetTabs(),sName.c_str());
		}
	}

	//-----------------------------------------------------------------------

	cSerializeMemberField * cSerializeClass::GetMemberField(const tString &asName,cSerializeSavedClass* apClass)
	{
		cSerializeMemberFieldIterator classIt = cSerializeMemberFieldIterator(apClass);

		while(classIt.HasNext())
		{
			cSerializeMemberField* pField = classIt.GetNext();

			if(asName == pField->msName) return pField;
		}

		Warning("Couldn't find member field '%s' in class '%s'\n",asName.c_str(), apClass->msName);
		return NULL;
	}

	//-----------------------------------------------------------------------

	size_t cSerializeClass::SizeOfType(eSerializeType aType)
	{
		switch(aType)
		{
			case eSerializeType_Bool:		return sizeof(bool);
			case eSerializeType_Int32:		return sizeof(int);
			case eSerializeType_Float32:	return sizeof(float);
			case eSerializeType_String:		return sizeof(tString);
			case eSerializeType_Vector2l:	return sizeof(cVector2l);
			case eSerializeType_Vector2f:	return sizeof(cVector2f);
			case eSerializeType_Vector3l:	return sizeof(cVector3l);
			case eSerializeType_Vector3f:	return sizeof(cVector3f);
			case eSerializeType_Matrixf:	return sizeof(cMatrixf);
			case eSerializeType_Color:		return sizeof(cColor);
			case eSerializeType_Rect2l:		return sizeof(cRect2l);
			case eSerializeType_Rect2f:		return sizeof(cRect2f);
			case eSerializeType_Planef:		return sizeof(cPlanef);
			case eSerializeType_WString:	return sizeof(tWString);
		}

		return 0;
	}

	//-----------------------------------------------------------------------


	void cSerializeClass::SetUpData()
	{
		if(mbDataSetup) return;

		mbDataSetup = true;

		for(int i=0; i< glSerializeTempClassesNum; i++)
		{
			m_mapSavedClasses.insert(tSerializeSavedClassMap::value_type(
							tString(gvSerializeTempClasses[i].msName),gvSerializeTempClasses[i])
							);
		}
	}

	//-----------------------------------------------------------------------

	void cSerializeClass::FillSaveClassMembersList(tSerializeSavedClassList *apList, cSerializeSavedClass* apClass)
	{
		if(apClass==NULL) return;
		apList->push_back(apClass);
		FillSaveClassMembersList(apList, GetClass(apClass->msParentName));
	}


	//-----------------------------------------------------------------------

}
