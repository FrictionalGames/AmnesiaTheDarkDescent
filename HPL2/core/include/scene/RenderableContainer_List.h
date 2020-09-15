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

#ifndef HPL_RENDERABLE_CONTAINER_LIST_H
#define HPL_RENDERABLE_CONTAINER_LIST_H

#include "scene/RenderableContainer.h"

namespace hpl {

	//-------------------------------------------
	
	class cRCNode_List : public iRenderableContainerNode
	{
	friend class cRenderableContainer_List;
	public:
		cRCNode_List();
		~cRCNode_List();

	private:
	};

	//-------------------------------------------
	
	class cRenderableContainer_List : public iRenderableContainer
	{
	public:
		cRenderableContainer_List();
		~cRenderableContainer_List();

		void Add(iRenderable *apRenderable);
		void Remove(iRenderable *apRenderable);

		iRenderableContainerNode* GetRoot();

        void Compile();	

		void RenderDebug(cRendererCallbackFunctions *apFunctions);

	private:
		cRCNode_List mRoot;
	};

	//-------------------------------------------
};
#endif // HPL_RENDERABLE_CONTAINER_LIST_H
