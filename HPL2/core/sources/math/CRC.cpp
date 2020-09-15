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

#include "math/CRC.h"

//----------------------------------------

namespace hpl {

	cCRCTable cCRC::mTable;

	void cCRCTable::Init (tCRCKey aKey)
	{
		if (aKey == mKey) return;
		
		mKey = aKey;

		// for all possible byte values
		for (unsigned i = 0; i < 256; ++i)
		{
			tCRCKey reg = i << 24;
			// for all bits in a byte
			for (int j = 0; j < 8; ++j)
			{
				bool topBit = (reg & 0x80000000) != 0;
				reg <<= 1;
				if (topBit)
					reg ^= mKey;
			}
			mTable [i] = reg;
		}
	}

	//----------------------------------------

	void cCRC::PutByte (unsigned aByte)
	{
		unsigned top = mRegister >> 24;
		top ^= aByte;
		mRegister = (mRegister << 8) ^ mTable [top];
	}

	//----------------------------------------

	void cCRC::PutData(char *apData, size_t alSize)
	{
		size_t lCount = alSize;
		unsigned char *pCurrentData = (unsigned char*)apData;
		while(lCount)
		{
			PutByte(*pCurrentData);
			
			++pCurrentData;
			--lCount;
		}
	}
	
	//----------------------------------------
}
