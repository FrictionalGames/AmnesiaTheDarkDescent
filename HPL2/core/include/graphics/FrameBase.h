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

#ifndef HPL_FRAMEBASE_H
#define HPL_FRAMEBASE_H

#include "math/MathTypes.h"

namespace hpl {
	
	class iFrameBase
	{
	public:
		iFrameBase(){
			mlPicCount = 0;
		}
		~iFrameBase() { }

		void SetPicCount(int alPicCount){mlPicCount=alPicCount;}
		void DecPicCount(){if(mlPicCount>0)mlPicCount--;}
		int GetPicCount(){return mlPicCount;}
		bool IsEmpty() {return mlPicCount<=0;}
		
	protected:
        int mlPicCount;
	};

};
#endif // HPL_FRAMEBASE_H
