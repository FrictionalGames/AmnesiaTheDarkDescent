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

#ifndef HPL_CRC_H
#define HPL_CRC_H

#include <cstddef>

namespace hpl {
	//----------------------------------------

	typedef unsigned int tCRCKey;

	//----------------------------------------

	class cCRCTable
	{
	public:
		cCRCTable () : mKey (0) {}
		
		void Init (tCRCKey key);
		
		tCRCKey operator [] (unsigned i){return mTable [i];}

	private:
		tCRCKey mTable [256];
		tCRCKey mKey;
	};

	//----------------------------------------

	class cCRC
	{
	public:
		cCRC (tCRCKey key) : mKey (key), mRegister(0)
		{
			mTable.Init (key);
		}

		void PutByte (unsigned aByte);
		void PutData(char *apData, size_t alSize);

		tCRCKey Done ()
		{
			tCRCKey temp = mRegister;
			mRegister = 0;
			return temp;
		}

	private:
		static cCRCTable mTable;
		tCRCKey mKey;	
		tCRCKey mRegister;
	};

	//----------------------------------------
}

#endif // HPL_CRC_H
