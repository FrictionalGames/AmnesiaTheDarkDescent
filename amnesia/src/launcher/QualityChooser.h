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

#ifndef QUALITY_CHOOSER_H
#define QUALITY_CHOOSER_H

#include "hpl.h"
using namespace hpl;

//-----------------------------------------------------------

enum eQRating
{
	eQRating_Unsupported = -1,

	eQRating_Low = 0,
	eQRating_Medium = 1,
	eQRating_High = 2,

	eQRating_Unknown,
};

//-----------------------------------------------------------

class cVideoCardManufacturer
{
public:
	cVideoCardManufacturer(const tString& asName) : msName(asName) {}
	const tString& GetName() { return msName; }
protected:
	tString msName;
};

//-----------------------------------------------------------

typedef std::map<tString, cVideoCardManufacturer*> tManufacturerMap;
typedef tManufacturerMap::const_iterator tManufacturerMapConstIt;

//-----------------------------------------------------------

class cVideoCardModel
{
public:
	cVideoCardModel(const tString& asName) : msName(asName) {}
	const tString& GetName() { return msName; }
protected:
	//cVideoCardManufacturer* mpManufacturer;
	tString msName;
};

//-----------------------------------------------------------

typedef std::map<tString, cVideoCardModel*> tModelMap;
typedef tModelMap::const_iterator tModelMapConstIt;

//-----------------------------------------------------------

class cVideoCardSeries
{
public:
	cVideoCardSeries(cVideoCardModel* apModel, const tString& asCode, eQRating aRating) : mpModel(apModel), msCode(asCode), mRating(aRating) {}

	//cVideoCardManufacturer* GetManufacturer();
	cVideoCardModel* GetModel();

	const tString& GetCode() { return msCode; }
	eQRating GetRating() { return mRating; }

	int GetNumericDifference(const tString& asCode, int alNumCount);

	tString ToString();
protected:
	cVideoCardModel* mpModel;
	tString msCode;
	eQRating mRating;
};

//-----------------------------------------------------------

typedef std::map<tString, cVideoCardSeries*> tSeriesMap;
typedef tSeriesMap::const_iterator tSeriesMapConstIt;

//-----------------------------------------------------------

class cQualityChooser
{
public:
	cQualityChooser(const tString& asDBFile);

	eQRating GetQualityRatingByCardString(const tString& asCardString, tString& asMsg);

	static char GetAlphaWildcard() { return '*'; }
	static char GetNumWildcard() { return '?'; }

protected:

	void BuildDatabase();

	void ParseManufacturer(tStringVec& avCardStringVec);
	void ParseModel(tStringVec& avCardStringVec);
	void ParseSeries(tStringVec& avCardStringVec);

	tString msDBFile;

	//std::map<tString, cVideoCardManufacturer*> mmapManufacturers;
	//std::map<cVideoCardManufacturer*, std::vector<cVideoCardModel*> > mmapModels;
	std::map<cVideoCardModel*, std::vector<cVideoCardSeries*> > mmapSeries;

	std::vector<cVideoCardModel*> mvModels;

	cVideoCardManufacturer* mpCurrentManufacturer;
	cVideoCardModel* mpCurrentModel;
	cVideoCardSeries* mpCurrentSeries;
	cVideoCardSeries* mpCurrentClosestSeries;
};

//-----------------------------------------------------------

#endif // QUALITY_CHOOSER_H
