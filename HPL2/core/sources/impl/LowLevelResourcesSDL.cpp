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

#include "impl/LowLevelResourcesSDL.h"
#include "impl/MeshLoaderMSH.h"
#include "impl/MeshLoaderFBX.h"
#include "impl/MeshLoaderCollada.h"
#include "impl/VideoStreamTheora.h"
#include "impl/XmlDocumentTiny.h"
#include "impl/BitmapLoaderDevilDDS.h"
#include "impl/BitmapLoaderDevilMisc.h"

#include "system/String.h"

#include "resources/MeshLoaderHandler.h"
#include "resources/VideoLoaderHandler.h"
#include "resources/BitmapLoaderHandler.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelResourcesSDL::cLowLevelResourcesSDL(iLowLevelGraphics *apLowLevelGraphics)
	{
		mpLowLevelGraphics = apLowLevelGraphics;
	}

	//-----------------------------------------------------------------------

	cLowLevelResourcesSDL::~cLowLevelResourcesSDL()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHOD
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cLowLevelResourcesSDL::AddBitmapLoaders(cBitmapLoaderHandler* apHandler)
	{
		apHandler->AddLoader(hplNew( cBitmapLoaderDevilDDS,()));
		apHandler->AddLoader(hplNew( cBitmapLoaderDevilMisc,()));
	}

	//-----------------------------------------------------------------------

	void cLowLevelResourcesSDL::AddMeshLoaders(cMeshLoaderHandler* apHandler)
	{
		cMeshLoaderMSH *pLoaderMSH = hplNew( cMeshLoaderMSH,(mpLowLevelGraphics));
		apHandler->AddLoader(pLoaderMSH);
		apHandler->AddLoader(hplNew( cMeshLoaderCollada,(mpLowLevelGraphics, pLoaderMSH, true)));
		apHandler->AddLoader(hplNew( cMeshLoaderFBX,(mpLowLevelGraphics, pLoaderMSH, true)));
	}

	//-----------------------------------------------------------------------

	void cLowLevelResourcesSDL::AddVideoLoaders(cVideoLoaderHandler* apHandler)
	{
		apHandler->AddLoader(hplNew( cVideoStreamTheora_Loader,()));
	}

	//-----------------------------------------------------------------------
	
	iXmlDocument* cLowLevelResourcesSDL::CreateXmlDocument(const tString& asName)
	{
		return hplNew( cXmlDocumentTiny,(asName) );
	}

	//-----------------------------------------------------------------------

}
