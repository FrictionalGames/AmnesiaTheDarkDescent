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

#ifndef HPLEDITOR_EDITOR_HELPER_H
#define HPLEDITOR_EDITOR_HELPER_H

#include "StdAfx.h"

using namespace hpl;

#include "EditorTypes.h"

//------------------------------------------------------------------------

class iEditorBase;
class iEditorWorld;

class cEditorWindowViewport;

//------------------------------------------------------------------------

class cEditorHelper
{
public:
	///////////////////////////////////////////////
	// Picking helpers
	static cVector2f GetInvYMousePos(const cVector2f& avMousePos, const cVector2f& avViewportSize);

	static void CalculateRayBoundingBox(const cVector3f& avStart, const cVector3f& avEnd, cVector3f &avMin, cVector3f &avMax);
	static bool CheckEntityAgainstRay(iEntityWrapper* apEntity, iEntityWrapper* apCurrentPickedEntity,
							   const cVector3f& avRayBVMin, const cVector3f& avRayBVMax, 
							   const cVector3f& avRayStart, const cVector3f& avRayEnd, 
							   float* apfMinDistance, cVector3f* apIntersectionPos=NULL, tVector3fVec* apTriangle=NULL);
	static bool CheckEntityAgainstBox(iEntityWrapper* apEntity, const cRect2l& aMouseRect, const cVector2l& avViewportSize, cFrustum* apFrustum);

	static bool CheckScreenSpaceMouseMeshIntersect(cCamera* apCamera, const cVector2f& avMousePos,  
												   const cVector3f& avRayStart, const cVector3f& avRayEnd,
												   const cVector2f& avViewportSize, cMeshEntity* apMesh,
												   cVector3f* apIntersectionPos, tVector3fVec* apTriangle=NULL);

	static bool CheckScreenSpaceMouseSubMeshIntersect(const cVector2f& avInvYMousePos,
													  const cVector3f& avRayStart, const cVector3f& avRayEnd,
													  const cVector2f& avViewportSize, cSubMeshEntity* apSubMesh,
													  const cMatrixf& amtxCameraViewMatrix, 
													  const cMatrixf& amtxCameraProjMatrix,
													  const cMatrixf& amtxSubMeshWorldMatrix,
													  cVector3f* apIntersectionPos, tVector3fVec* apTriangle=NULL);

	static bool CheckScreenSpaceMouseVertexBufferIntersect(const cVector2f& avInvYMousePos,
														   const cVector3f& avRayStart, const cVector3f& avRayEnd,
														   const cVector2f& avViewportSize, iVertexBuffer* apVtxBuffer,
														   const cMatrixf& amtxCameraViewMatrix,
														   const cMatrixf& amtxCameraProjMatrix,
														   const cMatrixf& amtxVtxBufferWorldMatrix,
														   cVector3f* apIntersectionPos, tVector3fVec* apTriangle=NULL);

	static bool CheckRayMeshEntityIntersect(const cVector3f& avRayStart, const cVector3f& avRayEnd,
											cMeshEntity* apObject, cVector3f* apIntersection, tVector3fVec* apTriangle=NULL, float* apT=NULL);

	static bool CheckRaySubMeshEntityIntersect(const cVector3f& avRayStart, const cVector3f& avRayEnd,
												cSubMeshEntity* apObject, cVector3f* apIntersection, float* apT, unsigned int* apTriangleIndex, tVector3fVec* apTriangle=NULL);



	static void GetTrianglesInsideClipPlanes(std::vector<cPlanef>& avPlanes, tEntityWrapperList& alstEntities, const cVector3f avBaseNormal, tVector3fVec& avTriangles, tVector3fVec& avNormals);

	static void GetTrianglesIntersectingSphere(const cVector3f&, float, tSubMeshEntityVec& avEntities, const cVector3f&, tVector3fVec&, tVector3fVec&);

    //////////////////////////////////////
	// Misc helpers
	static void GetIDsFromEntityList(const tEntityWrapperList &alstEntities, tIntList& alstEntityIDs, bool abClearList=true);

	static bool LoadTextureResource(eEditorTextureResourceType aTexType, const tString& asFile, iTexture** apTexture, const tString& asAnimMode="none", float afFrameTime=0 );

	static bool LoadEntityFile(int alID, const tString& asName, const tString& asFilename, cMeshEntity** apEntity, bool abLoadAsTemp=false);



	static bool LoadResourceFile(eEditorResourceType aResType, const tString& asFile, void** apEditorResource=NULL, int alID=-1);


	static void DrawBillboard(iTexture *apGfx, const cVector3f& avWorldPosition,const cVector2f& avSize,const cColor& aColor,  
						cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions);

	static void DrawPyramid(cRendererCallbackFunctions* apFunctions, const cVector3f& avBaseCenter, const cVector3f& avTop, float afHalfWidth, const cColor& aColor);

	static iVertexBuffer* CreatePyramidVtxBuffer(cGraphics* apGfx, const cVector3f& avBaseCenter, const cVector3f& avTip, float afHalfWidth, const cColor& aColor);

	static void GetViewSpacePosAndSize(cCamera* apCamera, const cVector3f& avWorldPosition, const cVector3f& avWorldSize, cVector3f& avViewSpacePosition, cVector3f& avViewSpaceSize);

	static iEditorBase* mpEditor;
};

//------------------------------------------------------------------------

#endif // HPLEDITOR_EDITOR_HELPER_H
