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

#ifndef HPL_CONFIGFILE_H
#define HPL_CONFIGFILE_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

class TiXmlDocument;

namespace hpl {
	class cFileSearcher;

	class cConfigFile
	{
	public:
		cConfigFile(const tWString& asFile,  const tWString& asAltPath = _W(""));
		~cConfigFile();
		
		/**
		 * Loads data from file
		 * \return true if loading ws ok, else false
		 */
		bool Load();
		/**
		 * Saves the data to file
		 * \return true if loading ws ok, else false
		 */
		bool Save();

		void SetUseCRC(bool abX){ mbUseCRC = abX;}

		void SetFileLocation(const tWString& asFile){ msFile = asFile;}
		const tWString& GetFileLocation(){ return msFile;}
		
		void SetString(const tString& asLevel, const tString& asName, const tString& asVal);
		void SetInt(const tString& asLevel, const tString& asName, int alVal);
		void SetFloat(const tString& asLevel, const tString& asName, float afVal);
		void SetBool(const tString& asLevel, const tString& asName, bool abVal);
		
		void SetVector2f(const tString& asLevel, const tString& asName, const cVector2f& avVal);
		void SetVector3f(const tString& asLevel, const tString& asName, const cVector3f& avVal);
		void SetVector2l(const tString& asLevel, const tString& asName, const cVector2l& avVal);
		void SetVector3l(const tString& asLevel, const tString& asName, const cVector3l& avVal);


		tString GetString(const tString& asLevel, const tString& asName, const tString& asDefault);
		tWString GetStringW(const tString& asLevel, const tString& asName, const tWString& asDefault);

		int GetInt(const tString& asLevel, const tString& asName, int alDefault);
		float GetFloat(const tString& asLevel, const tString& asName, float afDefault);
		bool GetBool(const tString& asLevel, const tString& asName, bool abDefault);
		
		cVector2f GetVector2f(const tString& asLevel, const tString& asName, const cVector2f& avDefault);
		cVector3f GetVector3f(const tString& asLevel, const tString& asName, const cVector3f& avDefault);

		cVector2l GetVector2l(const tString& asLevel, const tString& asName, const cVector2l& avDefault);
		cVector3l GetVector3l(const tString& asLevel, const tString& asName, const cVector3l& avDefault);

		
		cColor GetColor(const tString& asLevel, const tString& asName, const cColor& aDefault);

	private:
		tWString msFile;
		cFileSearcher *mpFileSearcher;

		TiXmlDocument *mpXmlDoc;

		bool mbUseCRC;

		const char* GetCharArray(tString asLevel, tString asName);
	};

};
#endif // HPL_CONFIGFILE_H
