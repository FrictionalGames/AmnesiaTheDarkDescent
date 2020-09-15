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

#ifndef HPL_SCRIPT_MANAGER_H
#define HPL_SCRIPT_MANAGER_H

#include "resources/ResourceManager.h"

namespace hpl {
	
	class cSystem;
	class cResources;
	class iScript;

	class cScriptManager : public iResourceManager
	{
	public:
		cScriptManager(cSystem* apSystem,cResources *apResources);
		~cScriptManager();

		/**
		 * Create a new script.
		 * \param asName name of the script.
		 * \return 
		 */
		iScript* CreateScript(const tString& asName, tString *apCompileMessages=NULL);

		void Destroy(iResourceBase* apResource);
		void Unload(iResourceBase* apResource);

	private:
		cSystem* mpSystem;
		cResources *mpResources;
	};

};
#endif // HPL_SCRIPT_MANAGER_H
