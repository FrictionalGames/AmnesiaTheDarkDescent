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

#include "engine/EngineTypes.h"

#include "system/LowLevelSystem.h"
#include "math/Math.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// MULTIPLE SETTINGS HANDLER
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	void cMultipleSettingsHandler::cGSEntry::FadeTo(float afDest, float afSpeed, bool abDestroyAtDest)
	{
		mfDest = afDest;
		mfSpeed = cMath::GetCorrectSignOfSpeed(mfVal, mfDest,afSpeed);
		mbDestroyAtDest = abDestroyAtDest;
	}

	void cMultipleSettingsHandler::cGSEntry::SetValAndDest(float afVal)
	{
		mfVal = afVal;
		mfDest = mfVal;
	}

	//-----------------------------------------------------------------------

	cMultipleSettingsHandler::cMultipleSettingsHandler()
	{

	}

	cMultipleSettingsHandler::~cMultipleSettingsHandler()
	{
		for(size_t i=0; i<mvEntries.size(); ++i)
		{
			if(mvEntries[i]) hplDelete(mvEntries[i]);
		}
	}

	//-----------------------------------------------------------------------

	cMultipleSettingsHandler::cGSEntry* cMultipleSettingsHandler::GetEntry(int alIdx, bool abForceCreation)
	{
		if(alIdx >= (int)mvEntries.size())
		{
			if(abForceCreation)
			{
				mvEntries.resize(alIdx+1, NULL);
				mvEntries[alIdx] = hplNew( cMultipleSettingsHandler::cGSEntry, () );
			}
			else
			{
				return NULL;
			}
		}
		else if(mvEntries[alIdx]==NULL && abForceCreation)
		{
			mvEntries[alIdx] = hplNew( cMultipleSettingsHandler::cGSEntry, () );
		}

		return mvEntries[alIdx];
	}

	//-----------------------------------------------------------------------

	int cMultipleSettingsHandler::CreateEntry()
	{
		for(size_t i=0; i< mvEntries.size(); ++i)
		{
			if(mvEntries[i] == NULL)
			{
				mvEntries[i] = hplNew( cMultipleSettingsHandler::cGSEntry, () );
				return (int)i;
			}
		}

		mvEntries.push_back( hplNew( cMultipleSettingsHandler::cGSEntry, () ) );
		return (int)mvEntries.size() - 1;
	}

	void cMultipleSettingsHandler::DestroyEntry(int alIdx)
	{
		if(mvEntries[alIdx])
		{
			hplDelete( mvEntries[alIdx] );
			mvEntries[alIdx] = NULL;
		}
	}

	//-----------------------------------------------------------------------

	float cMultipleSettingsHandler::CalcResults(tFlag aTypes, eMultipleSettingsCalcType aCalcType, float afDefault)
	{
		if(mvEntries.empty()) return afDefault;

		float fFinal= 0;
		bool bFirst = true;
		bool bUsedEntry=false;

		for(size_t i=0; i<mvEntries.size(); ++i)
		{
			cMultipleSettingsHandler::cGSEntry *pEntry = mvEntries[i];
			if(pEntry==NULL) continue;
			
			if( (pEntry->mTypes & aTypes) == 0) continue;
			
			bUsedEntry = true;

			switch(aCalcType)
			{
			case eMultipleSettingsCalcType_Mul:
				if(bFirst) fFinal =1;
				fFinal *= pEntry->mfVal;
				break;

			case eMultipleSettingsCalcType_Min:
				if(bFirst || fFinal > pEntry->mfVal) fFinal = pEntry->mfVal;
				break;

			case eMultipleSettingsCalcType_Max:
				if(bFirst || fFinal < pEntry->mfVal) fFinal = pEntry->mfVal;
				break;

			case eMultipleSettingsCalcType_Avg:
				if(bFirst) fFinal =0;
				fFinal += pEntry->mfVal;
				break;
			}

			
			bFirst = false;
		}

		if(bUsedEntry==false) return afDefault;

		if(aCalcType == eMultipleSettingsCalcType_Avg)
		{
			fFinal = fFinal / (float)mvEntries.size();
		}

		return fFinal;
	}

	//-----------------------------------------------------------------------

	void cMultipleSettingsHandler::Update(float afTimeStep)
	{
		for(size_t i=0; i<mvEntries.size();++i)
		{
			cMultipleSettingsHandler::cGSEntry *pEntry = mvEntries[i];
			if(pEntry==NULL) continue;

			if(pEntry->mfVal != pEntry->mfDest)
			{
				pEntry->mfVal += pEntry->mfSpeed * afTimeStep;
				if(pEntry->mfSpeed < 0 && pEntry->mfVal < pEntry->mfDest)
				{
					pEntry->mfVal = pEntry->mfDest;
				}
				else if(pEntry->mfSpeed > 0 && pEntry->mfVal > pEntry->mfDest)
				{
					pEntry->mfVal = pEntry->mfDest;
				}
			}

			if(pEntry->mbDestroyAtDest && pEntry->mfVal == pEntry->mfDest)
			{
				hplDelete(pEntry);
				mvEntries[i] = NULL;
			}
		}
	}

	//-----------------------------------------------------------------------



	//////////////////////////////////////////////////////////////////////////
	// SCRIPT VAR
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cScriptVar::cScriptVar()
	{
		
	}

	//-----------------------------------------------------------------------
	
	kBeginSerializeBase(cScriptVar)
	kSerializeVar(msName, eSerializeType_String)
	kSerializeVar(mlVal, eSerializeType_Int32)
	kEndSerialize()

	//-----------------------------------------------------------------------
}
