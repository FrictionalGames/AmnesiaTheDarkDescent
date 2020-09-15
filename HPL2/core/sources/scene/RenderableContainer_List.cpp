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

#include "scene/RenderableContainer_List.h"

#include "system/LowLevelSystem.h"

#include "graphics/Renderable.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// NODE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cRCNode_List::cRCNode_List()
	{

	}

	//-----------------------------------------------------------------------

	cRCNode_List::~cRCNode_List()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cRenderableContainer_List::cRenderableContainer_List()
	{
		mRoot.mpParent = NULL;
	}
	
	cRenderableContainer_List::~cRenderableContainer_List()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cRenderableContainer_List::Add(iRenderable *apRenderable)
	{
		mRoot.mlstObjects.push_back(apRenderable);
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_List::Remove(iRenderable *apRenderable)
	{
		mRoot.mlstObjects.remove(apRenderable);
	}

	//-----------------------------------------------------------------------

	iRenderableContainerNode* cRenderableContainer_List::GetRoot()
	{
		return &mRoot;
	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_List::Compile()
	{

	}

	//-----------------------------------------------------------------------

	void cRenderableContainer_List::RenderDebug(cRendererCallbackFunctions *apFunctions)
	{

	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------

}
