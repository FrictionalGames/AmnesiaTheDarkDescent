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

#ifndef HPL_PREPROCESS_PARSER_H
#define HPL_PREPROCESS_PARSER_H

#include "system/SystemTypes.h"

namespace hpl {

	//---------------------------------------

	typedef std::map<tString, tString> tParseVarMap;
	typedef tParseVarMap::iterator tParseVarMapIt;

	
	class cParserVarContainer
	{
	public:
		void Add(const tString& asName, const tString& asVal="");
		void Add(const tString& asName, int alVal);
		void Add(const tString& asName, float afVal);
		bool Remove(const tString& asName);
		tString* Get(const tString& asName);
		
		tParseVarMap* GetMapPtr(){ return &m_mapVars;}

		void Clear();

	private:
		tParseVarMap m_mapVars;
	};

	//---------------------------------------

	enum eParserSymbol
	{
		eParserSymbol_Text,
		eParserSymbol_Keyword,
		eParserSymbol_Operator,
		eParserSymbol_Variable,
		eParserSymbol_LastEnum,
	};

	enum eParserKeyword
	{
		eParserKeyword_Define,
		eParserKeyword_Ifdef,
		eParserKeyword_Else,
		eParserKeyword_Elseif,
		eParserKeyword_Endif,
		eParserKeyword_Include,
		eParserKeyword_LastEnum,
	};

	enum eParserOperator
	{
		eParserOperator_And,
		eParserOperator_Or,
		eParserOperator_LastEnum
	};

	//---------------------------------------

	class cParserSymbolText;
	class cParserSymbolKeyword;
	class cParserSymbolOperator;
	class cParserSymbolVariable;
	
	class iParserSymbol
	{
	public:
		iParserSymbol(int alRow) : mlRow(alRow){}

		virtual eParserSymbol GetType()=0;

		cParserSymbolText* ToText();
		cParserSymbolKeyword* ToKeyword();
		cParserSymbolOperator* ToOperator();
		cParserSymbolVariable* ToVariable();

		int mlRow;
	};

	//---------------------------------------

	class cParserSymbolText : public iParserSymbol
	{
	public:
		cParserSymbolText(const tString& asText, int alRow) : msText(asText), iParserSymbol(alRow){}

		eParserSymbol GetType(){ return eParserSymbol_Text;}

		tString msText;
	};

	class cParserSymbolKeyword : public iParserSymbol
	{
	public:
		cParserSymbolKeyword(eParserKeyword aKeyword, int alRow) : mKeyword(aKeyword), iParserSymbol(alRow){}

		eParserSymbol GetType(){ return eParserSymbol_Keyword;}

		eParserKeyword mKeyword;
	};
	
	class cParserSymbolOperator : public iParserSymbol
	{
	public:
		cParserSymbolOperator(eParserOperator aOp, int alRow) : mOp(aOp), iParserSymbol(alRow) {}

		eParserSymbol GetType(){ return eParserSymbol_Operator;}

		eParserOperator mOp;
	};

	class cParserSymbolVariable : public iParserSymbol
	{
	public:
		cParserSymbolVariable(const tString& asName, int alRow) : msName(asName), iParserSymbol(alRow) {}

		eParserSymbol GetType(){ return eParserSymbol_Variable;}

		tString msName;
	};

	//---------------------------------------

	enum eSymbolProcess
	{
		eSymbolProcess_PureText,
		eSymbolProcess_Line,
		eSymbolProcess_Variable,
		eSymbolProcess_LastEnum,
	};
	
	//---------------------------------------
	
	typedef std::list<iParserSymbol*> tParserSymbolList;
	typedef tParserSymbolList::iterator tParserSymbolListIt;

	//---------------------------------------

	class cPreprocessParser
	{
	public:
		cPreprocessParser();
		~cPreprocessParser();

        bool Parse(	const tString* apInput, tString *apOutput, cParserVarContainer *apVarContainer,
					const tWString& asDir=_W(""));

		cParserVarContainer* GetEnvVarContainer(){ return &mEnvironmentVars;}
		cParserVarContainer* GetParsingVarContainer(){ return &mParsingVars;}
		
	private:
		bool CharIsVariableValid(char alChar);
		bool VariableExists(const tString &asName);
		tString* GetVar(const tString &asName);

		bool EndOfInput();
		void GetNextString();
		eParserKeyword StringToKeyword(const tString& asString);
		eParserOperator StringToOperator(const tString& asString);
		bool ParseStringToSymbol(eSymbolProcess aProcess, const tString& asString);
		
		bool EndOfSymbols();
		void GetNextSymbol();
		bool ParseBooleanDefineStatement(bool& abStatmentValue);
		void AddSymbol(iParserSymbol* apSymbol);
		bool ParseSymbol(iParserSymbol *apSymbol);
		bool ParseText(cParserSymbolText *apText);
		bool ParseVariable(cParserSymbolVariable *apVar);
		bool ParseOperator(cParserSymbolOperator *apOp);
		bool ParseKeyword(cParserSymbolKeyword *apKeyword);

		cParserVarContainer *mpCurrentVars;
		cParserVarContainer mEnvironmentVars;
		cParserVarContainer mParsingVars;
		
		tWString msCurrentDirectory;
        const tString *mpCurrentInput;
		tString *mpCurrentOutput;
		cParserVarContainer *mpCurrentVarContainer;

		tString msCurrentString;
		int mlInputPos;
		eSymbolProcess mProcess;
		int mlCurrentRow;
		int mlRowCount;
		
		tParserSymbolList mlstSymbols;
		iParserSymbol *mpCurrentSymbol;
		tParserSymbolListIt mSymbolIt;
	
	};

};
#endif // HPL_PREPROCESS_PARSER_H
