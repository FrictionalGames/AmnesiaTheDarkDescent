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

#include "resources/ResourceBase.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"


namespace hpl {

	bool iResourceBase::mbLogCreateAndDelete=false;


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	iResourceBase::iResourceBase(const tString& asName, const tWString& asFullPath,unsigned long alPrio){
		mlTime = (unsigned long)time(NULL);
		mlPrio = alPrio;
		mlHandle = 0;
		mlUserCount =0;
		msName = asName;
		mbLogDestruction = false;
		msFullPath = asFullPath;
	}

	iResourceBase::~iResourceBase()
	{
		if(mbLogDestruction && mbLogCreateAndDelete)
			Log("  Destroyed resource '%s'\n",msName.c_str());
	}
	//-----------------------------------------------------------------------

	void iResourceBase::IncUserCount()
	{
		mlUserCount++;
		mlTime = (unsigned long)time(NULL);
	}
	
	//-----------------------------------------------------------------------

	void iResourceBase::SetFullPath(const tWString& asPath)
	{
		msFullPath = asPath;
	}
	
	//-----------------------------------------------------------------------

}
