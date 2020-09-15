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

#include "QualityChooser.h"
#include "impl/tinyXML/tinyxml.h"
#include <algorithm>
#include <cctype>

#ifndef __APPLE__
#include "FL/fl_ask.H"
#endif

//-----------------------------------------------------------

int cVideoCardSeries::GetNumericDifference(const tString& asCode, int alNumericCount)
{
	int lCode = cString::ToInt(cString::Sub(asCode,0, alNumericCount).c_str(), 0);

	int lOwnCode = cString::ToInt(cString::ReplaceCharTo(GetCode(), cString::ToString(cQualityChooser::GetNumWildcard()), "0").c_str(), 0);

	return lCode-lOwnCode;
}

//-----------------------------------------------------------

tString cVideoCardSeries::ToString()
{
	return mpModel->GetName() + " " + GetCode();
}

//-----------------------------------------------------------
//-----------------------------------------------------------

bool ModelComparator(cVideoCardModel* aModel1, cVideoCardModel* aModel2)
{
	tStringVec vModel1Name, vModel2Name;
	cString::GetStringVec(aModel1->GetName(), vModel1Name);
	cString::GetStringVec(aModel2->GetName(), vModel2Name);

	return (vModel2Name.size()<vModel1Name.size());
}

//-----------------------------------------------------------

cQualityChooser::cQualityChooser(const tString& asDBFile)
{
	msDBFile = asDBFile;

	mpCurrentManufacturer = NULL;
	mpCurrentModel = NULL;
	mpCurrentSeries = NULL;
	mpCurrentClosestSeries = NULL;

	BuildDatabase();
}

//-----------------------------------------------------------

void cQualityChooser::BuildDatabase()
{
	TiXmlDocument pDoc(msDBFile.c_str());

	if(pDoc.LoadFile()==false)
	{
#if defined __APPLE__
		cPlatform::CreateMessageBox(eMsgBoxType_Warning,L"Error Parsing Card Database", L"Could not parse the card database");
#else
        fl_message_title("Error Parsing Card Database");
		fl_message("Could not parse card database\n");
#endif
		return;
	}

	TiXmlElement* root = pDoc.RootElement();
	TiXmlNode* pModelParent = root->FirstChildElement("Models");
	////////////////////////
	// Get Manufacturers
	//for(pManufNode = root->FirstChild(); pManufNode != NULL; pManufNode = pManufNode->NextSibling())
	//{
	//	if(pManufNode==NULL)
	//		break;

	//	TiXmlElement* pManufElem = pManufNode->ToElement();
	//	tString sManufName = cString::ToLowerCase(pManufElem->Attribute("Name"));

	//	cVideoCardManufacturer* pManufacturer = new cVideoCardManufacturer(sManufName);
	//	mmapManufacturers.insert(std::pair<tString, cVideoCardManufacturer*>(sManufName, pManufacturer));

		////////////////////////////////////////////////////
		// Manufacturer created, get models for it now
		TiXmlNode* pModelNode;
		for(pModelNode = pModelParent->FirstChild();pModelNode!=NULL; pModelNode = pModelNode->NextSibling())
		{
			if(pModelNode==NULL)
				break;

			TiXmlElement* pModelElem = pModelNode->ToElement();

			tString sModelName = cString::ToLowerCase(pModelElem->Attribute("Name"));
			cVideoCardModel* pModel = new cVideoCardModel(sModelName);

			mvModels.push_back(pModel);

			///////////////////////////////////////////////
			// Get Series for current model
			TiXmlNode* pSeriesNode;
			std::vector<cVideoCardSeries*> vSeries;
			for(pSeriesNode = pModelElem->FirstChild();pSeriesNode!=NULL; pSeriesNode = pSeriesNode->NextSibling())
			{
				if(pSeriesNode==NULL)
					break;

				TiXmlElement* pSeriesElem = pSeriesNode->ToElement();
				tString sSeriesCode = cString::ToLowerCase(pSeriesElem->Attribute("Code"));

				eQRating rating = eQRating_Unknown;
				pSeriesElem->QueryIntAttribute("Rating", (int*)&rating);


				vSeries.push_back( new cVideoCardSeries(pModel, sSeriesCode, rating));
			}

			mmapSeries.insert(std::pair<cVideoCardModel*, std::vector<cVideoCardSeries*> >(pModel, vSeries));
		}

		printf("Pre-Sort\n");
		for(size_t i=0;i<mvModels.size();++i)
		{
			const tString& sModelName = mvModels[i]->GetName();
			printf("%s\n", sModelName.c_str());
		}

		// Sort models by name length (int words)
		sort(mvModels.begin(), mvModels.end(), ModelComparator);
		printf("Post-Sort\n");

		for(size_t i=0;i<mvModels.size();++i)
		{
			const tString& sModelName = mvModels[i]->GetName();
			printf("%s\n", sModelName.c_str());
		}
			
		//mmapModels.insert(std::pair<cVideoCardManufacturer*, std::vector<cVideoCardModel*> >(pManufacturer, vModels));
	//}
}

//-----------------------------------------------------------

eQRating cQualityChooser::GetQualityRatingByCardString(const tString& asCardString, tString& asMsg)
{
	///////////////////////////////////////////
	// Create a vector with the string pieces in lowercase
	tStringVec vCardStrVec;
	cString::GetStringVec(cString::ToLowerCase(asCardString), vCardStrVec);

	ParseSeries(vCardStrVec);

	if(mpCurrentSeries!=NULL)
		return mpCurrentSeries->GetRating();
	if(mpCurrentClosestSeries!=NULL)
	{
		asMsg = mpCurrentClosestSeries->ToString();
		return mpCurrentClosestSeries->GetRating();
	}

	return eQRating_Unknown;
}

//-----------------------------------------------------------

void cQualityChooser::ParseManufacturer(tStringVec& avCardStringVec)
{
	/*
	mpCurrentManufacturer = NULL;
	for(int i=0;i<(int)avCardStringVec.size();++i)
	{
		const tString& sTemp = avCardStringVec[i];
		tManufacturerMapConstIt it = mmapManufacturers.find(sTemp);

		if(it!=mmapManufacturers.end())
		{
			mpCurrentManufacturer = it->second;
			remove(avCardStringVec.begin(), avCardStringVec.end(), sTemp);

			break;
		}
	}
	*/
}

//-----------------------------------------------------------

void cQualityChooser::ParseModel(tStringVec& avCardStringVec)
{
	mpCurrentModel = NULL;
	
	//ParseManufacturer(avCardStringVec);
	//if(mpCurrentManufacturer==NULL)
	//	return;

	bool bFound = false;

	tString sSep = " ";
	for(size_t i=0;i<mvModels.size();++i)
	{
		cVideoCardModel* pModel = mvModels[i];
		tStringVec vModelStrVec;
		int lFoundPieces = 0;
		cString::GetStringVec(pModel->GetName(),vModelStrVec, &sSep);

		tStringVecIt it = vModelStrVec.begin();
		for(;it!=vModelStrVec.end();++it)
		{
			tStringVecIt itCardStr = find(avCardStringVec.begin(),avCardStringVec.end(), *it);
			if(itCardStr!=avCardStringVec.end())
				lFoundPieces++;
		}

		if(lFoundPieces == (int)vModelStrVec.size())
		{
			mpCurrentModel = pModel;

			for(size_t j=0;j<vModelStrVec.size();++j)
			{
				size_t lSizeBefore = avCardStringVec.size();
				tString sPieceToRemove = vModelStrVec[j];

				avCardStringVec.erase(remove(avCardStringVec.begin(), avCardStringVec.end(), sPieceToRemove));
			}

			break;
		}
	}
}

//-----------------------------------------------------------

void cQualityChooser::ParseSeries(tStringVec& avCardStringVec)
{
	mpCurrentSeries = NULL;
	mpCurrentClosestSeries = NULL;

	ParseModel(avCardStringVec);
	if(mpCurrentModel==NULL)
		return;

	std::vector<cVideoCardSeries*> vSeries = mmapSeries.find(mpCurrentModel)->second;
	if(avCardStringVec.empty() && vSeries.empty()==false)
	{
		mpCurrentSeries = vSeries.front();

		return;
	}

	char alphaWildcard = GetAlphaWildcard();
	char numWildcard = GetNumWildcard();

	for(size_t i=0;i<avCardStringVec.size() && mpCurrentSeries==NULL;++i)
	{
		const tString& sTemp = avCardStringVec[i];

		int lPrefixCount = 0;
		int lNumericCount = 0;
		int lSuffixCount = 0;

		for(size_t j=0;j<sTemp.size();++j)
		{
			if(std::isdigit(sTemp[j]) || sTemp[j]==numWildcard)
			{
				if(lSuffixCount==0)
					++lNumericCount;
				else
					break;
			}
			else if(std::isalpha(sTemp[j]) || sTemp[j]==alphaWildcard)
			{
				if(lNumericCount==0)
				{
					++lPrefixCount;
				}
				else
				{
					++lSuffixCount;
				}
			}
			else
				break;
		}

		tStringVec vCardStrings;
		vCardStrings.push_back(cString::ToLowerCase(cString::Sub(sTemp, 0, lPrefixCount)));
		vCardStrings.push_back(cString::ToLowerCase(cString::Sub(sTemp, lPrefixCount+lNumericCount,lSuffixCount)));

		int lDifferenceRange = (int) (2.0f*pow(10.0f,lNumericCount-1));
		for(int j=0;j<(int)vSeries.size();++j)
		{
			cVideoCardSeries* pSeries = vSeries[j];
			bool bPrefixSuffixMatch = true;

			const tString& sCode = pSeries->GetCode();
			std::vector<tString> vCodeStrings;
			vCodeStrings.push_back(cString::ToLowerCase(cString::Sub(sCode, 0, lPrefixCount)));
			vCodeStrings.push_back(cString::ToLowerCase(cString::Sub(sCode, lPrefixCount+lNumericCount,lSuffixCount)));
			
			/////////////////////////////////////////
			// Check prefix and suffix match
			for(int k=0;k<(int)vCardStrings.size();++k)
			{
				const tString& sCardString = vCardStrings[k];
				const tString& sCodeString = vCodeStrings[k];
				
				if(sCodeString.find(alphaWildcard)==tString::npos && 
					(sCardString.length()!=sCodeString.length() || sCardString.length()>sCodeString.length()))
				{
					bPrefixSuffixMatch = false;
					break;
				}

				for(int c=0;c<(int)sCodeString.size();++c)
				{
					if(sCodeString[c]!=alphaWildcard && 
						sCardString[c]!=sCodeString[c])
					{
						bPrefixSuffixMatch = false;
						break;
					}
				}

				if(bPrefixSuffixMatch==false)
					break;
			}

			if(bPrefixSuffixMatch==false)
				continue;

			/////////////////////////////////////////
			// Check card series number
			tString sCardNumber = cString::Sub(sTemp, lPrefixCount, lNumericCount);
			tString sCodeNumber = cString::Sub(sCode, lPrefixCount, lNumericCount);

			if(sCardNumber.length()!=sCodeNumber.length())
				continue;

			bool bSeriesFound = true;

			for(int c=0;c<(int)sCodeNumber.size();++c)
			{
				if(sCodeNumber[c]!=numWildcard && 
					sCardNumber[c]!=sCodeNumber[c])
				{
					bSeriesFound = false;
					break;
				}
			}

			if(bSeriesFound)
			{
				mpCurrentSeries = pSeries;
				break;
			}
			else
			{
				if(pSeries->GetNumericDifference(sCardNumber, lNumericCount)<lDifferenceRange)
					mpCurrentClosestSeries = pSeries;
			}
		}
	}
}

//-----------------------------------------------------------
