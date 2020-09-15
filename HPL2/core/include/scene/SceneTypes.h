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

#ifndef HPL_SCENE_TYPES_H
#define HPL_SCENE_TYPES_H

#include <list>
#include <vector>

namespace hpl 
{

	//-----------------------------------------

	enum eWorldContainerType
	{
		eWorldContainerType_Static,
		eWorldContainerType_Dynamic,
		eWorldContainerType_LastEnum,
	};

	//-----------------------------------------

	typedef tFlag tObjectVariabilityFlag;

	#define eObjectVariabilityFlag_Static	(0x00000001)
	#define eObjectVariabilityFlag_Dynamic	(0x00000002)

	#define eObjectVariabilityFlag_All		(eObjectVariabilityFlag_Static | eObjectVariabilityFlag_Dynamic)


	typedef tFlag tWorldDestroyAllFlag;

	#define eWorldDestroyAllFlag_SkipStaticEntities	(0x00000001)
	#define eWorldDestroyAllFlag_SkipPhysics		(0x00000002)

	
    //-----------------------------------------

	enum eLightBoxBlendFunc
	{
		eLightBoxBlendFunc_Replace,
		eLightBoxBlendFunc_Add,
		eLightBoxBlendFunc_LastEnum
	};

	//-----------------------------------------

	enum eViewportMessage
	{
		eViewportMessage_OnPreWorldDraw,
		eViewportMessage_OnPostWorldDraw,
		eViewportMessage_LastEnum
	};

	enum eRendererMessage
	{
		eRendererMessage_PostSolid,
		eRendererMessage_PostTranslucent,
		eRendererMessage_PostGBuffer,
		eRendererMessage_LastEnum
	};

	//-----------------------------------------

	enum eBillboardType
	{
		eBillboardType_Point,
		eBillboardType_Axis,
		eBillboardType_FixedAxis,
		eBillboardType_LastEnum
	};
	
    //-----------------------------------------
	
	class iViewportCallback
	{
	public:
		virtual void OnPreWorldDraw()=0;
		virtual void OnPostWorldDraw()=0;

		void RunMessage(eViewportMessage aMessage)
		{
			switch(aMessage)
			{
			case eViewportMessage_OnPreWorldDraw:
				OnPreWorldDraw(); break;
			case eViewportMessage_OnPostWorldDraw:
				OnPostWorldDraw(); break;
			}
		}
	};

	//------------------------------------------

	class iEntity3D;

	class iEntityCallback
	{
	public:
		virtual void OnTransformUpdate(iEntity3D * apEntity)=0;
	};

	typedef std::list<iEntityCallback*> tEntityCallbackList;
	typedef tEntityCallbackList::iterator tEntityCallbackListIt;

	//------------------------------------------

	class cRendererCallbackFunctions;

	class iRendererCallback
	{
	public:
		virtual void OnPostSolidDraw(cRendererCallbackFunctions *apFunctions)=0;
		virtual void OnPostTranslucentDraw(cRendererCallbackFunctions *apFunctions)=0;
		virtual void OnPostPostGBufferDraw(cRendererCallbackFunctions *apFunctions){}

		void RunMessage(eRendererMessage aMessage, cRendererCallbackFunctions *apFunctions)
		{
			switch(aMessage)
			{
			case eRendererMessage_PostSolid:
				OnPostSolidDraw(apFunctions); break;
			case eRendererMessage_PostTranslucent:
				OnPostTranslucentDraw(apFunctions); break;
			case eRendererMessage_PostGBuffer:
				OnPostPostGBufferDraw(apFunctions); break;
			}
		}
	};

	//------------------------------------------

	typedef std::list<iViewportCallback*> tViewportCallbackList;
	typedef tViewportCallbackList::iterator tViewportCallbackListIt;

	typedef std::list<iRendererCallback*> tRendererCallbackList;
	typedef tRendererCallbackList::iterator tRendererCallbackListIt;

    //------------------------------------------
	
	class iRenderableContainerNode;

	typedef std::set<iRenderableContainerNode*> tRenderableContainerNodeSet;
	typedef tRenderableContainerNodeSet::iterator tRenderableContainerNodeSetIt;

	typedef std::list<iRenderableContainerNode*> tRenderableContainerNodeList;
	typedef tRenderableContainerNodeList::iterator tRenderableContainerNodeListIt;

	//------------------------------------------
	
	class iEntity3D;

	typedef std::list<iEntity3D*> tEntity3DList;
	typedef tEntity3DList::iterator tEntity3DListIt;

	typedef cSTLIterator<iEntity3D*,tEntity3DList,tEntity3DListIt> cEntity3DIterator;

	//------------------------------------------

	class cNode3D;

	typedef std::list<cNode3D*> tNode3DList;
	typedef tNode3DList::iterator tNode3DListIt;

	typedef cSTLIterator<cNode3D*,tNode3DList,tNode3DListIt> cNode3DIterator;

	
	//------------------------------------------

	class iLight;
	class cMeshEntity;
	class cSubMeshEntity;
	class cBillboard;
	class cBeam;
	class cParticleSystem;
    class cSoundEntity;
	class cGuiSetEntity;
	class cAINodeContainer;
	class cAStarHandler;
	class cRopeEntity;
	class cFogArea;
	class cAnimationState;
	class cDummyRenderable;

	//------------------------------------------
	
	typedef std::list<iLight*> tLightList;
	typedef std::list<iLight*>::iterator tLightListIt;

	typedef std::list<cMeshEntity*> tMeshEntityList;
	typedef std::list<cMeshEntity*>::iterator tMeshEntityListIt;

	typedef std::list<cSubMeshEntity*> tSubMeshEntityList;
	typedef std::list<cSubMeshEntity*>::iterator tSubMeshEntityListIt;

	typedef std::list<cBillboard*> tBillboardList;
	typedef std::list<cBillboard*>::iterator tBillboardListIt;

	typedef std::list<cBeam*> tBeamList;
	typedef std::list<cBeam*>::iterator tBeamListIt;

	typedef std::list<cParticleSystem*> tParticleSystemList;
	typedef tParticleSystemList::iterator tParticleSystemListIt;

	typedef std::list<cSoundEntity*> tSoundEntityList;
	typedef std::list<cSoundEntity*>::iterator tSoundEntityListIt;

	typedef std::list<cGuiSetEntity*> tGuiSetEntityList;
	typedef std::list<cGuiSetEntity*>::iterator tGuiSetEntityListIt;

	typedef std::list<cRopeEntity*> tRopeEntityList;
	typedef std::list<cRopeEntity*>::iterator tRopeEntityListIt;

	typedef std::list<cFogArea*> tFogAreaList;
	typedef std::list<cFogArea*>::iterator tFogAreaListIt;

	typedef std::list<cDummyRenderable*> tDummyRenderableList;
	typedef tDummyRenderableList::iterator tDummyRenderableListIt;

	typedef std::list<cAINodeContainer*> tAINodeContainerList;
	typedef std::list<cAINodeContainer*>::iterator tAINodeContainerListIt;

	typedef std::list<cAStarHandler*> tAStarHandlerList;
	typedef std::list<cAStarHandler*>::iterator tAStarHandlerIt;

	typedef std::vector<cAnimationState*> tAnimationStateVec;
	typedef tAnimationStateVec::iterator tAnimationStateVecIt;

	typedef std::map<tString, int> tAnimationStateIndexMap;
	typedef tAnimationStateIndexMap::iterator tAnimationStateIndexMapIt;

	//-------------------------------------------------------------------

	typedef cSTLIterator<cMeshEntity*, tMeshEntityList, tMeshEntityListIt> cMeshEntityIterator;
	typedef cSTLIterator<cBillboard*, tBillboardList, tBillboardListIt> cBillboardIterator;
	typedef cSTLIterator<iLight*, tLightList, tLightListIt> cLightListIterator;
	typedef cSTLIterator<cParticleSystem*, tParticleSystemList, tParticleSystemListIt> cParticleSystemIterator;
	typedef cSTLIterator<cSoundEntity*, tSoundEntityList, tSoundEntityListIt> cSoundEntityIterator;
	typedef cSTLIterator<cBeam*, tBeamList, tBeamListIt> cBeamIterator;
	typedef cSTLIterator<cGuiSetEntity*, tGuiSetEntityList, tGuiSetEntityListIt> cGuiSetEntityIterator;
	typedef cSTLIterator<cRopeEntity*, tRopeEntityList, tRopeEntityListIt> cRopeEntityIterator;
	typedef cSTLIterator<cFogArea*, tFogAreaList, tFogAreaListIt> cFogAreaIterator;
	typedef cSTLIterator<cDummyRenderable*, tDummyRenderableList, tDummyRenderableListIt> cDummyRenderableIterator;

	//------------------------------------------

	class iLight;
	
	typedef std::set<iLight*> tLightSet;
	typedef tLightSet::iterator tLightSetIt;

	typedef std::vector<iLight*> tLightVec;
	typedef tLightVec::iterator tLightVecIt;


	//------------------------------------------


}

#endif // HPL_SCENE_TYPES_H
