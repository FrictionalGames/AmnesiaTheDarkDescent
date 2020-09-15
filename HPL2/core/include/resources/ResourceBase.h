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

#ifndef HPL_RESOURCEBASE_H
#define HPL_RESOURCEBASE_H

#include <time.h>
#include "system/SystemTypes.h"
#include "system/LowLevelSystem.h"

namespace hpl {

	class iResourceBase
	{
	public:
		
		iResourceBase(const tString& asName, const tWString& asFullPath,unsigned long alPrio);

		virtual ~iResourceBase();
				
		/**
		 * virtual bool Reload()=0; 
		 * \return true is reload was succesful, else false.
		 */
		virtual bool Reload()=0;

		/**
		 * Free most the memory, save info to get started again. 
		 */
		virtual void Unload()=0;

        /**
         * Free all memory.
         */
        virtual void Destroy()=0;

		const tString& GetName(){return msName;}
		unsigned long GetHandle(){return mlHandle;}
		void SetHandle(unsigned long alHandle){mlHandle=alHandle;}
		
		void SetFullPath(const tWString& asPath);
		const tWString& GetFullPath(){return msFullPath;}
		
		unsigned long GetTime(){return mlTime;}
		unsigned long GetPrio(){return mlPrio;}
		unsigned long GetSize(){return mlSize;}

		void SetLogDestruction(bool abX){ mbLogDestruction = abX;}
		
		unsigned int GetUserCount(){return mlUserCount;}
		void IncUserCount();
		void DecUserCount(){if(mlUserCount>0)mlUserCount--;}
		bool HasUsers(){ return mlUserCount>0;}

		static bool GetLogCreateAndDelete(){ return mbLogCreateAndDelete;}
		static void SetLogCreateAndDelete(bool abX){ mbLogCreateAndDelete = abX;}

	protected:
		static bool mbLogCreateAndDelete;
		
		tString msName;
		
		unsigned int mlPrio; //dunno if this will be of any use.
		unsigned long mlTime; //Time for creation.
		unsigned long mlSize; //for completion. Not used yet.
		
		unsigned int mlUserCount;
        unsigned long mlHandle;
		bool mbLogDestruction;
	
	private:
		tWString msFullPath;
	};

};
#endif // HPL_RESOURCEBASE_H
