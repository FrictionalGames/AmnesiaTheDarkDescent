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

#ifndef HPL_GPU_SHADER_MANAGER_H
#define HPL_GPU_SHADER_MANAGER_H

#include "resources/ResourceManager.h"

#include "graphics/GPUShader.h"

namespace hpl {

	//------------------------------------

	class iLowLevelGraphics;
	class cParserVarContainer;
	class cPreprocessParser;

	//------------------------------------

	class cGpuShaderManager : public iResourceManager
	{
	public:
		cGpuShaderManager(cFileSearcher *apFileSearcher, iLowLevelGraphics *apLowLevelGraphics,
							iLowLevelResources *apLowLevelResources,iLowLevelSystem *apLowLevelSystem);
		~cGpuShaderManager();

		void CheckFeatureSupport();
						
		/**
		 * Creates a new GPU program
		 * \param asName name of the program 
		 * \param asEntry the entry point of the program (usually "main")
		 * \param aType type of the program
		 * \return 
		 */
		iGpuShader* CreateShader(const tString& asName,eGpuShaderType aType, cParserVarContainer *apVarContainer);

		void Destroy(iResourceBase* apResource);
		void Unload(iResourceBase* apResource);
	
	private:
		bool IsShaderSupported(const tString& asName, eGpuShaderType aType);

		iLowLevelGraphics *mpLowLevelGraphics;
		cPreprocessParser* mpPreprocessParser;
	};

};
#endif // HPL_GPU_SHADER_MANAGER_H
