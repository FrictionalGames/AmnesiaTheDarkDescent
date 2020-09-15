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

#include "system/PreprocessParser.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/Platform.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// VAR CONTAINER
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cParserVarContainer::Add(const tString& asName, const tString& asVal)
	{
		//If it already exists, just change value
		tParseVarMapIt it = m_mapVars.find(asName);
		if(it != m_mapVars.end())
		{
			it->second = asVal;
			return;
		}
		
		m_mapVars.insert(tParseVarMap::value_type(asName,asVal));
	}

	void cParserVarContainer::Add(const tString& asName, int alVal)
	{
		Add(asName, cString::ToString(alVal));
	}
	
	void cParserVarContainer::Add(const tString& asName, float afVal)
	{	
		char sTemp[1024];
		sprintf(sTemp, "%f", afVal);

		Add(asName, sTemp);
	}

	//-----------------------------------------------------------------------

	bool cParserVarContainer::Remove(const tString& asName)
	{
		tParseVarMapIt it = m_mapVars.find(asName);
		if(it == m_mapVars.end()) return false;

		m_mapVars.erase(it);
		return true;
	}

	//-----------------------------------------------------------------------

	tString* cParserVarContainer::Get(const tString& asName)
	{
		tParseVarMapIt it = m_mapVars.find(asName);
		if(it == m_mapVars.end()) return NULL;
        
		return &it->second;
	}

	
	//-----------------------------------------------------------------------
	
	void cParserVarContainer::Clear()
	{
		m_mapVars.clear();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SYMBOL
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cParserSymbolText* iParserSymbol::ToText()
	{
		return static_cast<cParserSymbolText*>(this);
	}

	cParserSymbolKeyword* iParserSymbol::ToKeyword()
	{
		return static_cast<cParserSymbolKeyword*>(this);
	}

	cParserSymbolOperator* iParserSymbol::ToOperator()
	{
		return static_cast<cParserSymbolOperator*>(this);
	}

	cParserSymbolVariable* iParserSymbol::ToVariable()
	{
		return static_cast<cParserSymbolVariable*>(this);
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cPreprocessParser::cPreprocessParser()
	{
		
	}

	//-----------------------------------------------------------------------

	cPreprocessParser::~cPreprocessParser()
	{
		STLDeleteAll(mlstSymbols);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cPreprocessParser::Parse(const tString* apInput, tString *apOutput, cParserVarContainer *apVarContainer,const tWString& asDir)
	{
		//////////////////////////////
		// Set up data
		mpCurrentInput = apInput;
		mpCurrentOutput = apOutput;
		mpCurrentVarContainer = apVarContainer;
		mpCurrentVars = apVarContainer;

		mParsingVars.Clear();

		msCurrentDirectory = asDir;
		msCurrentString = "";
		mlInputPos =0;
		mProcess = eSymbolProcess_PureText;
		mlCurrentRow =1;
		mlRowCount =1;
		
		STLDeleteAll(mlstSymbols);
		mlstSymbols.clear();

		//////////////////////////////
		// Parse input into symbols
		int lCount =0;
        while(EndOfInput() == false)
		{
			eSymbolProcess currentProcess = mProcess;
			GetNextString();
		
		    ParseStringToSymbol(currentProcess, msCurrentString);
		}
		
		//////////////////////////////
		// Parse symbols into output
		
		mSymbolIt = mlstSymbols.begin();
		if(mSymbolIt == mlstSymbols.end())
		{
			Error("Parser failed: No symbols to parse!\n");
		}
		mpCurrentSymbol = *mSymbolIt;

		while(EndOfSymbols() == false)
		{
		    if(ParseSymbol(mpCurrentSymbol)==false) return false;
		}
		
		return true;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cPreprocessParser::CharIsVariableValid(char alChar)
	{
		return (alChar >='0' && alChar <= '9') || (alChar>='A' && alChar<='Z') || (alChar>='a' && alChar<='z');
	}

	//-----------------------------------------------------------------------

	bool cPreprocessParser::VariableExists(const tString &asName)
	{
		return GetVar(asName) != NULL;
	}

	//-----------------------------------------------------------------------

	tString* cPreprocessParser::GetVar(const tString &asName)
	{
		tString *pVar = mEnvironmentVars.Get(asName);
		if(mpCurrentVars && pVar == NULL) pVar = mpCurrentVars->Get(asName);
		if(pVar == NULL) pVar = mParsingVars.Get(asName);

		return pVar;
	}

	//-----------------------------------------------------------------------
	
	bool cPreprocessParser::EndOfInput()
	{
		if(mlInputPos >= (int)mpCurrentInput->length()) return true;
		
		return false;	
	}

	//-----------------------------------------------------------------------

	static bool DivideWordsToSymbols(eSymbolProcess aProcess)
	{
		if(aProcess == eSymbolProcess_PureText) return false;
		return true;
	}

	//-----------------------------------------------------------------------

	void cPreprocessParser::GetNextString()
	{
		msCurrentString = "";
		mlCurrentRow = mlRowCount;
		do 
		{
			//Get character
			char lChar = (*mpCurrentInput)[mlInputPos];
			if(lChar == '\n' || lChar == '\r') mlRowCount++;

			//printf("process: %d ", mProcess);
			//if(lChar == '\n')		printf("char: newline\n");
			//else if(lChar == '\r')	printf("char: return\n");
			//else					printf("char: '%c'\n",lChar);
			
			//////////////////////////////////
			// Look for symbols
            if(DivideWordsToSymbols(mProcess))
			{
				////////////////////////
				//Space or newline
				if(lChar == ' ' || lChar == '\n' || lChar == '\r' || lChar == '\t')
				{
					if(mProcess != eSymbolProcess_PureText)
					{
						if( lChar=='\n' || lChar=='\r' || mProcess == eSymbolProcess_Variable)
							mProcess = eSymbolProcess_PureText;
					}
					
					mlInputPos++;
					
					//Dunno if the continue is used for anything...
					//if(msCurrentString.size() ==0)
					//	continue;
					//else
					return;
				}
				////////////////////////
				//Variable
				else if(lChar == '$')
				{
					mlInputPos++;
					if(mProcess == eSymbolProcess_Variable)
					{
						mProcess = eSymbolProcess_PureText;
						return;
					}
				}
				////////////////////////
				//Non number or character
				else if( CharIsVariableValid(lChar)==false )
				{
					if(mProcess == eSymbolProcess_Variable)
					{
						mProcess = eSymbolProcess_PureText;
						return;
					}
					else
					{
						mlInputPos++;
						msCurrentString += lChar;
					}
				}
				////////////////////////
				//Normal character
				else
				{
					mlInputPos++;
                    msCurrentString += lChar;
				}
			}
			//////////////////////////////////
			// All normal character to text
			else
			{
				//////////////////////////////////
				//Line is full of symbols
				if(lChar == '@')
				{
					//If previous was text, remove any spaces or tabs before @
					if(mProcess == eSymbolProcess_PureText)
					{
						if(msCurrentString.size() >0)
						{
							char lLastChar = msCurrentString[msCurrentString.size()-1];
							while(lLastChar == ' ' || lLastChar=='\t')
							{
								msCurrentString.resize(msCurrentString.size()-1);
								if(msCurrentString.empty()) break;
								lLastChar = msCurrentString[msCurrentString.size()-1];
							}
						}
					}
					
					mlInputPos++;
					mProcess = eSymbolProcess_Line;
					return;
				}
				//////////////////////////////////
				//Word is variable
				else if(lChar == '$')
				{
					mlInputPos++;
					mProcess = eSymbolProcess_Variable;
					return;
				}
				//////////////////////////////////
				//Normal character
				else
				{
					mlInputPos++;
					msCurrentString += lChar;
				}
			}
			
			//////////////////////////////////
			//Check if end of string is reached.
			if(EndOfInput())
			{
				return;
			}
		} while(true);
	}

	//-----------------------------------------------------------------------

	eParserKeyword cPreprocessParser::StringToKeyword(const tString& asString)
	{
		if(asString =="define") return eParserKeyword_Define;
		if(asString =="ifdef")	return eParserKeyword_Ifdef;
		if(asString =="else")	return eParserKeyword_Else;
		if(asString =="elseif") return eParserKeyword_Elseif;
		if(asString =="endif")	return eParserKeyword_Endif;
		if(asString =="include")return eParserKeyword_Include;
		
		return eParserKeyword_LastEnum;
	}
	
	eParserOperator cPreprocessParser::StringToOperator(const tString& asString)
	{
		if(asString =="&&") return eParserOperator_And;
		if(asString =="||") return eParserOperator_Or;
		
		return eParserOperator_LastEnum;
	}

	//-----------------------------------------------------------------------

	bool cPreprocessParser::ParseStringToSymbol(eSymbolProcess aProcess, const tString& asString)
	{
		//printf("Processing row %d: '%s' with process %d\n",mlCurrentRow ,asString.c_str(), aProcess);

		//////////////////////////////////////
		// Text
		if(aProcess == eSymbolProcess_PureText)
		{
			AddSymbol( hplNew(cParserSymbolText,(asString,mlCurrentRow) ) );
		}
		//////////////////////////////////////
		// Variable
		else if(aProcess == eSymbolProcess_Variable)
		{
			//Check so the variable is not used a space or empty
			if(asString.size()==0) return true;
			if(asString[0] == ' ' || asString[0] == '\t') return true;
			
			AddSymbol( hplNew(cParserSymbolVariable,(asString,mlCurrentRow) ) );
		}
		//////////////////////////////////////
		// Line
		else if(aProcess == eSymbolProcess_Line)
		{
			//Check so the variable is not used a space or empty
			if(asString.size()==0) return true;
			if(asString[0] == ' ' || asString[0] == '\t') return true;
			
			///////////////////////
			//Keyword
			eParserKeyword lKeyword = StringToKeyword(asString);
			if(lKeyword != eParserKeyword_LastEnum)
			{
				AddSymbol( hplNew(cParserSymbolKeyword,(lKeyword,mlCurrentRow) ) );	
				return true;
			}
			///////////////////////
			//Operator
			eParserOperator lOp = StringToOperator(asString);
			if(lOp != eParserOperator_LastEnum)
			{
				AddSymbol( hplNew(cParserSymbolOperator,(lOp,mlCurrentRow) ) );
				return true;
			}
			///////////////////////
			//Variable
			AddSymbol( hplNew(cParserSymbolVariable,(asString,mlCurrentRow) ) );
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cPreprocessParser::EndOfSymbols()
	{
		return mSymbolIt == mlstSymbols.end();
	}

	//-----------------------------------------------------------------------

	void cPreprocessParser::GetNextSymbol()
	{
		if(EndOfSymbols()){
			mpCurrentSymbol = NULL;
			return;
		}
        ++mSymbolIt;
		if(EndOfSymbols()){
			mpCurrentSymbol = NULL;
			return;
		}
		
		mpCurrentSymbol = *mSymbolIt;
	}

	//-----------------------------------------------------------------------

	void cPreprocessParser::AddSymbol(iParserSymbol* apSymbol)
	{
		//printf("Adding symbol type: %d\n",apSymbol->GetType());

		mlstSymbols.push_back(apSymbol);
	}

	//-----------------------------------------------------------------------

	bool cPreprocessParser::ParseSymbol(iParserSymbol *apSymbol)
	{
		//printf("Parsing symbol: %d\n",apSymbol->GetType());

		if(apSymbol==NULL)
		{
			Error("Parser failed: Symbol passed as nullpointer!\n");
			return false;
		}

		switch(apSymbol->GetType())
		{
		case eParserSymbol_Text:		return ParseText(apSymbol->ToText()); 
		case eParserSymbol_Variable:	return ParseVariable(apSymbol->ToVariable());
		case eParserSymbol_Operator:	return ParseOperator(apSymbol->ToOperator());
		case eParserSymbol_Keyword:		return ParseKeyword(apSymbol->ToKeyword());
		}
		
		Error("Parser failed: Invalid symbol type %d on row %d!\n",apSymbol->GetType() ,apSymbol->mlRow);

		return false;
	}

	//-----------------------------------------------------------------------

	bool cPreprocessParser::ParseText(cParserSymbolText *apText)
	{
		//printf(" Adding '%s'\n", apText->msText.c_str());
		*mpCurrentOutput += apText->msText;
		GetNextSymbol();

		return true;
	}
	
	//-----------------------------------------------------------------------
	
	bool cPreprocessParser::ParseVariable(cParserSymbolVariable *apVar)
	{
		tString *pVar = GetVar(apVar->msName);
		if(pVar)
		{
			*mpCurrentOutput += *pVar;
		}
		else
		{
			Warning("Parser warning: Variable '%s' on row %d was not found!\n", apVar->msName.c_str(), apVar->mlRow);
		}

		GetNextSymbol();

		return true;
	}

	//-----------------------------------------------------------------------
	
	bool cPreprocessParser::ParseOperator(cParserSymbolOperator *apOp)
	{
		Error("Parser failed: Unexpected operator on row %d!\n", apOp->mlRow);
		GetNextSymbol();

		return true;
	}

	//-----------------------------------------------------------------------

	bool cPreprocessParser::ParseBooleanDefineStatement(bool& abStatmentValue)
	{
		//Comment:
		// This is not a very good parse for a boolean statement and it will only be useful if
		// && and || are not mixed. So make sure to update if any more complex stuff is needed.

		abStatmentValue = false;
		eParserOperator prevOp = eParserOperator_LastEnum;

		///////////////////////////////////////
		//Iterate all variables in statement and check
        while(true)
		{
			//////////////////////////////
			//This value must be a variable
			if(EndOfSymbols() || mpCurrentSymbol->GetType() != eParserSymbol_Variable)
			{
				Error("Parser failed: Invalid symbol type %d at row %d!\n", mpCurrentSymbol->GetType(), mpCurrentSymbol->mlRow);
				return false;
			}

			//////////////////////////////
			//Check if variable exists
			tString &sName = mpCurrentSymbol->ToVariable()->msName;
			if(VariableExists(sName))
			{
				if(prevOp != eParserOperator_And) abStatmentValue = true;
			}
			else
			{
				if(prevOp == eParserOperator_And) abStatmentValue = false;
			}

			//////////////////////////////
			//Check for any operator, if non exit
			GetNextSymbol();
			if(EndOfSymbols() || mpCurrentSymbol->GetType() != eParserSymbol_Operator)
			{
				break;	
			}
            else
			{
				prevOp = mpCurrentSymbol->ToOperator()->mOp;
				GetNextSymbol();
			}
		}

		return true;
	}

	//-----------------------------------------------------------------------

	static bool KeyWordIsOfType(iParserSymbol *apSymbol, eParserKeyword* apKeywordArray, int alAmount)
	{
		if(apSymbol==NULL) return false;
		if(apSymbol->GetType()!= eParserSymbol_Keyword) return false;
		
		for(int i=0; i<alAmount; ++i)
		{
			if(apSymbol->ToKeyword()->mKeyword == apKeywordArray[i]) return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------

	
	bool cPreprocessParser::ParseKeyword(cParserSymbolKeyword *apKeyword)
	{
		eParserKeyword lKeyword = apKeyword->mKeyword;

		//printf("Parsing keyword %d at row %d\n",apKeyword->mKeyword, apKeyword->mlRow);
		
		////////////////////////////////////////
		//Ifdef
		if(lKeyword == eParserKeyword_Ifdef)
		{
			//////////////////////////////////////
			// Set up variables
			eParserKeyword vKeywords[] = {eParserKeyword_Endif,eParserKeyword_Else,eParserKeyword_Elseif};
			int lKeywordNum = 3;
			bool bStatementWasTrue = false;
			bool bHasFoundCorrectPassage = false;
            eParserKeyword currentKeyword = lKeyword;
			
			/////////////////////////////////////////////////
			//Go through symbols until endif or end of symbols has been encountered
			while( currentKeyword != eParserKeyword_Endif && EndOfSymbols()==false)
			{
				//printf(" current keyword: %d\n",currentKeyword);

				if(	currentKeyword == eParserKeyword_Ifdef || 
					currentKeyword == eParserKeyword_Elseif)
				{
					GetNextSymbol();
					if(ParseBooleanDefineStatement(bStatementWasTrue)==false)
					{
						return false;
					}
				}
				else
				{
					GetNextSymbol();
				}
				
				/////////////////////////////
				//Statement true, parse until endif is found
				if( bHasFoundCorrectPassage == false &&
					(bStatementWasTrue || currentKeyword ==eParserKeyword_Else) )
				{
					bHasFoundCorrectPassage = true;
					while(	EndOfSymbols()==false && 
							KeyWordIsOfType(mpCurrentSymbol,vKeywords,lKeywordNum)==false )
					{
						ParseSymbol(mpCurrentSymbol);
					}
					
				}
				/////////////////////////////
				//Statement NOT true. Skip parsing until next valid keyword.
				else
				{
					int lIfDefCount =0;

					while(	EndOfSymbols()==false && 
							(KeyWordIsOfType(mpCurrentSymbol,vKeywords,lKeywordNum)==false || lIfDefCount>0) )
					{
						//if an idef, we need to skip all else, elseif, etc until a endif is found.
						if(	mpCurrentSymbol->GetType()==eParserSymbol_Keyword)
						{
							if(mpCurrentSymbol->ToKeyword()->mKeyword == eParserKeyword_Ifdef)
							{
								++lIfDefCount;
							}
							else if(mpCurrentSymbol->ToKeyword()->mKeyword == eParserKeyword_Endif)
							{
								--lIfDefCount;
							}
						}
						GetNextSymbol();
						
					}
				}
				if(EndOfSymbols()==false)
					currentKeyword = mpCurrentSymbol->ToKeyword()->mKeyword;
			}
			GetNextSymbol();
			
		}
		////////////////////////////////////////
		//Define
		else if(lKeyword == eParserKeyword_Define)
		{
			cParserSymbolVariable *pVarName;
			cParserSymbolVariable *pVarVal;
			for(int i=0; i<2; ++i)
			{
				GetNextSymbol();
				if(mpCurrentSymbol->GetType() != eParserSymbol_Variable)
				{
					Error("Parser failed: Keyword define at row %d does not meet syntax: '@define var value'!\n", apKeyword->mlRow);
					return false;
				}
				if(i==0)				
					pVarName = mpCurrentSymbol->ToVariable();
				else
					pVarVal = mpCurrentSymbol->ToVariable();
			}
			
            mParsingVars.Add(pVarName->msName, pVarVal->msName);
			GetNextSymbol();
		}
		////////////////////////////////////////
		//Include
		else if(lKeyword == eParserKeyword_Include)
		{
			GetNextSymbol();
			if(mpCurrentSymbol->GetType() != eParserSymbol_Variable)
			{
				Error("Parser failed: Keyword include at row %d does not meet syntax: '@include file.ext'!\n", apKeyword->mlRow);
				return false;
			}
			tString sFile = mpCurrentSymbol->ToVariable()->msName;
			tWString sPath;
			if(msCurrentDirectory!=_W(""))
				sPath = cString::SetFilePathW(cString::To16Char(sFile), msCurrentDirectory);
			else
				sPath = cString::To16Char(sFile);
			
			if(cPlatform::FileExists(sPath))
			{
				unsigned int lFileSize = cPlatform::GetFileSize(sPath);
				tString sFileData;
				sFileData.resize(lFileSize);
				cPlatform::CopyFileToBuffer(sPath,&sFileData[0],lFileSize);
				
				*mpCurrentOutput += sFileData;
			}
			else
			{
				Error("Parser failed: Could not find include file '%s' in path: '%s' at line %d!\n",sFile.c_str(),cString::To8Char(sPath).c_str(),
																									apKeyword->mlRow);
				return false;
			}

			GetNextSymbol();
		}
		////////////////////////////////////////
		//Anything else
		else
		{
			Error("Parser failed: Keyword %d at row %d is not valid!\n", lKeyword, apKeyword->mlRow);
			return false;
		}
		
		return true;
	}
	
	//-----------------------------------------------------------------------

}
