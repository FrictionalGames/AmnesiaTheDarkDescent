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

#ifndef ED_VIEWPORT_H
#define ED_VIEWPORT_H

#include "../common/StdAfx.h"

#include "../Common/EdWindowHandler.h"
#include "../Common/EdGrid.h"

using namespace hpl;

//--------------------------------------------------------------------

class iEditor;
class iEdWorld;
class iEdViewport;
class cEdCamera;
class cEdGrid;
class cEdSelection;

class iCamMode;

class cViewportClick;

//--------------------------------------------------------------------

enum eEdCameraType
{
	eEdCameraType_OrthoFront,
	eEdCameraType_OrthoRight,
	eEdCameraType_OrthoTop,
	eEdCameraType_Perspective,
};

//--------------------------------------------------------------------

enum eEdCameraMoveType
{
	eEdCameraMoveType_LockToGrid,
	eEdCameraMoveType_Free,

	eEdCameraMoveType_LastEnum,
};

//--------------------------------------------------------------------

class cViewportClick
{
public:
	cViewportClick(iEdViewport*, const cGuiMessageData&);

	iEdViewport* mpViewport;

	cVector2f mvGlobalPos;
	cVector2f mvLocalPos;
	cVector2f mvRelPos;

	bool mbControl;
	bool mbShift;
	bool mbAlt;

	bool mbLeft;
	bool mbMid;
	bool mbRight;
	bool mbWheelUp;
	bool mbWheelDn;
};

//--------------------------------------------------------------------

class cViewportMove
{
public:
	cViewportMove(iEdViewport*, const cGuiMessageData&);

	iEdViewport* mpViewport;

	cVector2f mvGlobalPos;
	cVector2f mvLocalPos;
	cVector2f mvRelPos;
};

//--------------------------------------------------------------------

////////////////////////////////////////////////
// VIEWPORT RENDERER CALLBACK
////////////////////////////////////////////////

//--------------------------------------------------------------------

class cViewportCallback : public iRendererCallback
{
public:
	cViewportCallback(iEdViewport*);

	void OnPostSolidDraw(cRendererCallbackFunctions* apFunctions);
	void OnPostTranslucentDraw(cRendererCallbackFunctions* apFunctions);

	iEditor* mpEditor;
	iEdViewport* mpViewport;
};

//--------------------------------------------------------------------

class iEdViewport : public iEdPane
{
	friend class iEditor;
	friend class cEdCamera;
	friend class iCamMode;
	friend class cViewportCallback;
public:
	iEdViewport(iEditor*, const tWString&, cWorld* apWorld, iFrameBuffer* apFB=NULL, bool abDestroyFBOnExit=false);
	virtual ~iEdViewport();

	void SetStartMode(int alMode) { mlStartMode = alMode; }

	cEdGrid* GetGrid() { return mpGrid; }

	//void LogDebug();

	void SetViewportActive(bool abX);
	bool IsViewportActive() { return mbViewportActive; }

	cCamera* GetEngCamera();
	cEdCamera* GetCamera()		   { return mpCamera; }

	/////////////////////////////////////
	// Engine specific funcs
	iFrameBuffer* GetFrameBuffer() { return mpFB; }
	cViewport* GetEngineViewport() { return mpEngineViewport; }
	void SetRenderMode(eRenderer aRenderMode);
	void SetFrameBuffer(iFrameBuffer* apFB);
	void SetEngineViewportPositionAndSize(const cVector2l& avPos, const cVector2l& avSize);
	void SetEngineViewportSize(const cVector2l& avSize);
	void UpdateViewport();

	void SetGuiViewportPos(const cVector3f& avPos);
	const cVector3f& GetGuiViewportPos() { return mpImgViewport->GetGlobalPosition(); }
	void SetGuiViewportSize(const cVector2f& avSize);
	const cVector2f& GetGuiViewportSize() { return mvViewportSize; }
	cVector2l GetGuiViewportSizeInt() { return cVector2l((int)mvViewportSize.x, (int)mvViewportSize.y); }

	///////////////////////////////////////////////////////////////////
	// Mouse position
	const cVector2f& GetMouseViewportPosition();
	cVector2l GetMouseViewportPositionInt();

	const cVector3f& GetMouseWorldPosition(bool abForceUpdate=false, bool abCustomRayDist=false, float afCustomRayDist=200);
	const cVector3f& GetGridCenter();

	///////////////////////////////////////////////////////////////////
	// Unprojection helpers
	const cVector3f& GetUnprojectedStart();
	const cVector3f& GetUnprojectedDir();
	const cVector3f& GetUnprojectedEnd();
	cBoundingVolume* GetRayBV();
	float GetRayEndDistance() { return mfRayEndDistance; }

	void GetViewSpacePositionAndSize(const cVector3f&, const cVector3f&, cVector3f&, cVector3f&);

	void DrawBillboard(iTexture*, const cVector3f&, const cVector2f&, const cColor&, cRendererCallbackFunctions*);
	void DrawSolidColorMesh(cMeshEntity*, const cColor&, cRendererCallbackFunctions*);
	void DrawSolidColorVertexBuffer(iVertexBuffer*, cMatrixf*, const cColor&, cRendererCallbackFunctions*, bool abUsePreamble=true);

	void DrawArrow(cRendererCallbackFunctions* apFunctions, const cMatrixf&, float, bool, const cVector2f&, const cColor&, float afOrthoConstant=0.1f, float afPerspConstant=4);

	void AddViewportCallback(iRendererCallback* apCallback);

	void UpdateCameraPlanes();

	void SetClearColor(const cColor& aX);
	const cColor& GetClearColor();

	void SetFocused(bool);
	virtual bool IsFocused();

	void SetNormalSize(const cVector2f& avX) { mvNormalSize = avX; }
	void SetNormalPosition(const cVector3f& avX) { mvNormalPosition = avX; }
	void SetEnlargedSize(const cVector2f& avX) { mvEnlargedSize = avX; }
	void SetEnlargedPosition(const cVector3f& avX) { mvEnlargedPosition = avX; }

	void SetEngineViewportNormalPosition(const cVector2l& avX) { mvNormalFBPos = avX; }
	void SetEngineViewportNormalSize(const cVector2l& avX) { mvNormalFBSize = avX; }

	void SetEnlarged(bool abX);
	bool IsEnlarged() { return (GetBG()->GetSize()==mvEnlargedSize); }

	static void SetCamPlanes(const cVector2f& avX);
	static const cVector2f& GetCamPlanes() { return mvCamPlanes; }

	virtual bool LoadState(cXmlElement*);
	virtual bool SaveState(cXmlElement*, cXmlElement**);
protected:
	///////////////////////////////////
	// GUI Callbacks

	// Update
	bool Viewport_OnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Viewport_OnUpdate);
	bool Viewport_OnGotFocus(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Viewport_OnGotFocus);
	bool Viewport_OnLostFocus(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Viewport_OnLostFocus);

	// Specific update code for derived classes
	virtual bool OnViewportUpdate(const cGuiMessageData& aData) { return true; }
	virtual bool OnViewportGotFocus(const cGuiMessageData& aData) { return true; }
	virtual bool OnViewportLostFocus(const cGuiMessageData& aData) { return true; }

	// Draw
	bool Viewport_OnDraw(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Viewport_OnDraw);

	virtual void OnPostSolidDraw(cRendererCallbackFunctions*)			{}
	virtual void OnPostTranslucentDraw(cRendererCallbackFunctions*)		{}

	// Specific draw code for derived classes
	virtual bool OnViewportDraw(const cGuiMessageData& aData) { return true; }

	// Input
	bool Viewport_OnMouseDown(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Viewport_OnMouseDown);
	bool Viewport_OnMouseUp(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Viewport_OnMouseUp);
	bool Viewport_OnMouseMove(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Viewport_OnMouseMove);
	bool Viewport_OnKeyPress(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Viewport_OnKeyPress);
	bool Viewport_OnKeyRelease(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Viewport_OnKeyRelease);

	// Specific input handling for derived classes
	virtual bool OnViewportMouseMove(const cGuiMessageData& aData) { return true; }
	virtual bool OnViewportMouseDown(const cGuiMessageData& aData) { return true; }
	virtual bool OnViewportMouseUp(const cGuiMessageData& aData) { return true; }
	virtual bool OnViewportKeyPress(const cGuiMessageData& aData) { return true; }
	virtual bool OnViewportKeyRelease(const cGuiMessageData& aData) { return true; }

	///////////////////////////////////
	// Own Funcs
	//void SetUpFrameBuffer(iFrameBuffer* apFB);
	void CreateGuiViewport(iWidget* apParentWidget);
	void UpdateUnprojection();

	virtual void OnSetLock(bool){}

	virtual void OnSetCamMode(iCamMode*);

	virtual void OnReset();

	virtual void OnSetActive(bool);

	iEditor* mpEditor;
	cEngine* mpEngine;
	cGraphics* mpGfx;
	cViewport* mpEngineViewport;

	tRendererCallbackList mlstOwnCallbacks;

	int mlStartMode;

	cEdCamera* mpCamera;

	bool mbViewportNeedsUpdate;
	bool mbViewportActive;

	bool mbDestroyFBOnExit;

	//////////////////////////////
	// Render to texture stuff
	iFrameBuffer* mpFB;
	iTexture* mpRenderTarget;
	cVector2f mvUVStart;
	cVector2f mvUVSize;
	cVector2f mvUVEnd;
	cVector2l mvEngineViewportSize;
	cVector2l mvEngineViewportPos;

	//////////////////////////////
	// Mouse Position, ray direction stuff
	cVector2f mvMouseScreenPos;
	cVector2f mvMouseViewportPos;
	cVector3f mvMouseWorldPos;

	bool mbMousePositionUpdated;
	bool mbUnprojectionUpdated;
	float mfRayEndDistance;
	cVector3f mvUnprojectedStart;
	cVector3f mvUnprojectedDir;
	cVector3f mvUnprojectedEnd;
	cBoundingVolume mRayBV;

	cVector3f mvViewportPos;
	cVector2f mvViewportSize;
	cWidgetImage* mpImgViewport;
	eRenderer mRenderMode;

	cEdGrid* mpGrid;

	iRendererCallback* mpViewportCallback;

	static int mlViewportCount;

	static bool mbCamPlanesUpdated;
	static cVector2f mvCamPlanes;

	//////////////////////////////////
	// GUI Viewport params
	cVector3f mvNormalPosition;
	cVector3f mvEnlargedPosition;
	cVector2f mvNormalSize;
	cVector2f mvEnlargedSize;

	cVector2l mvNormalFBPos;
	cVector2l mvNormalFBSize;

	// Helper data for drawing billboards
	static iVertexBuffer* mpVBBillboard;

	static tVertexVec mvArrowQuads[4];

	static iGpuProgram* mpProgDrawSolid;
};

//--------------------------------------------------------------------

typedef std::vector<iEdViewport*> tViewportVec;

//--------------------------------------------------------------------

#endif // ED_VIEWPORT_H
