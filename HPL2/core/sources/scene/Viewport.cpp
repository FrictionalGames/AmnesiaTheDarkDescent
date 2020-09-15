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

#include "scene/Viewport.h"

#include "graphics/Renderer.h"

#include "scene/Scene.h"
#include "scene/World.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cViewport::cViewport(cScene *apScene)
	{
		mpScene = apScene;

		mbActive = true;
		mbVisible = true;

		mpRenderSettings = hplNew( cRenderSettings, () );

		mpWorld = NULL;
		mpCamera = NULL;
		mpRenderer = NULL;
		mpPostEffectComposite = NULL;

		mbIsListener = false;
	}

	//-----------------------------------------------------------------------

	cViewport::~cViewport()
	{
		hplDelete( mpRenderSettings );
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cViewport::SetWorld(cWorld *apWorld)
	{ 
		if(mpWorld != NULL && mpScene->WorldExists(mpWorld))
		{
			mpWorld->SetIsSoundEmitter(false);
		}

		mpWorld = apWorld;
		if(mpWorld) mpWorld->SetIsSoundEmitter(true);

		mpRenderSettings->ResetVariables();
	}
	

	//-----------------------------------------------------------------------

	void cViewport::AddGuiSet(cGuiSet *apSet)
	{
		mlstGuiSets.push_back(apSet);
	}
	void cViewport::RemoveGuiSet(cGuiSet *apSet)
	{
		STLFindAndRemove(mlstGuiSets, apSet);
	}
	cGuiSetListIterator cViewport::GetGuiSetIterator()
	{
		return cGuiSetListIterator(&mlstGuiSets);
	}

	//-----------------------------------------------------------------------

	void cViewport::AddViewportCallback(iViewportCallback *apCallback)
	{
		mlstCallbacks.push_back(apCallback);
	}
	
	void cViewport::RemoveViewportCallback(iViewportCallback *apCallback)
	{
		STLFindAndRemove(mlstCallbacks, apCallback);
	}

	void cViewport::RunViewportCallbackMessage(eViewportMessage aMessage)
	{
		tViewportCallbackListIt it = mlstCallbacks.begin();
		for(; it != mlstCallbacks.begin(); ++it)
		{
			iViewportCallback *pCallback = *it;

			pCallback->RunMessage(aMessage);
		}

	}

	//-----------------------------------------------------------------------

	void cViewport::AddRendererCallback(iRendererCallback *apCallback)
	{
		mlstRendererCallbacks.push_back(apCallback);
	}

	void cViewport::RemoveRendererCallback(iRendererCallback *apCallback)
	{
		STLFindAndRemove(mlstRendererCallbacks, apCallback);
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	

	//-----------------------------------------------------------------------

}
