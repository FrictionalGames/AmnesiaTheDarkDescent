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

#ifndef HPLEDITOR_EDITOR_VIEWPORT_H
#define HPLEDITOR_EDITOR_VIEWPORT_H

#include "../common/StdAfx.h"

#include "EditorGrid.h"

using namespace hpl;

//--------------------------------------------

class iEditorBase;
class iEditorWorld;
class iEditorViewport;
class cEditorViewportCamera;
class cEditorGrid;
class cEditorSelection;

//--------------------------------------------

enum eEditorCameraType
{
	eEditorCameraType_OrthoFront,
	eEditorCameraType_OrthoRight,
	eEditorCameraType_OrthoTop,
	eEditorCameraType_Orbit,
	eEditorCameraType_Fly,
};

//--------------------------------------------

enum eEditorCameraMoveType
{
	eEditorCameraMoveType_LockToGrid,
	eEditorCameraMoveType_Free,

	eEditorCameraMoveType_LastEnum,
};

//--------------------------------------------

class iViewportCameraZoomFunc
{
public:
	iViewportCameraZoomFunc()
	{
		mfMaxDistance = 50;
	}
	iViewportCameraZoomFunc(float afMinZoom, float afMaxZoom)
	{
		mfMinZoom = afMinZoom;
		mfMaxZoom = afMaxZoom;
	}

	float GetOrthoViewSizeFromZoom(float afX);
	float GetDistToTargetFromZoom(float afX);

	float GetZoomFromDistToTarget(float afX);
	float GetZoomFromOrthoViewSize(float afX);

protected:
	float mfMinZoom;
	float mfMaxZoom;

	float mfMaxDistance;
};

//--------------------------------------------

class cEditorViewportCamera
{
	friend class cEditorGrid;
public:
	cEditorViewportCamera(){ }
	cEditorViewportCamera(iEditorViewport* apViewport);
	~cEditorViewportCamera();

	cCamera* GetEngineCamera() { return mpEngineCamera; }

	eEditorCameraType GetCameraType() { return mCamType; }
	void SetCameraType(eEditorCameraType aType);
	bool IsOrtho();

	void LockToGrid(bool abX);
	bool IsLockedToGrid() { return mbLockToGrid; }
	
	void SetKeepOrientation(bool abX) { mbKeepOrientation = abX; }
	void SetOrientation(const cVector3f& avForward, const cVector3f& avUp, const cVector3f& avRight);

	bool KeepsOrientation() { return mbKeepOrientation; }
	const cVector3f& GetOrientationForward() { return mvForward; }
	const cVector3f& GetOrientationUp() { return mvUp; }
	const cVector3f& GetOrientationRight() { return mvRight; }

	iViewportCameraZoomFunc* GetZoomFunc() { return &mpCurrentZoomFunc; }

	void SetZoom(float afX, bool abComputeFromDist=false);
	void SetCameraPosition(const cVector3f& avPosition);
	void SetCameraSphCoords(const cVector2f& avSphCoords);
	void SetTargetPosition(const cVector3f& avTargetPos);
	void SetTargetSphCoords(const cVector2f& avSphCoords);

	float GetZoom() { return mfZoom; }
	const cVector3f& GetCameraPosition();
	const cVector2f& GetCameraSphCoords();
	const cVector3f& GetTargetPosition();
	const cVector2f& GetTargetSphCoords();

	void FetchSettings();

	void SetTumbleActive(bool abX) { mbTumbleActive = abX; }
	void SetTrackActive(bool abX);
	void SetZoomActive(bool abX) { mbZoomActive = abX; }

	bool IsTracking() { return mbTrackActive; }

	void Tumble(const cVector2f& avParams);
	void Track(const cVector2f& avParams);
	void Zoom(const cVector2f& avParams, bool abUseWheel=false);

	const cVector3f& GetTrackRefMousePos() { return mvTrackRefMousePos; }
	const cVector3f& GetTrackNewMousePos() { return mvTrackNewMousePos; }
	const cVector3f& GetTrackRefTargetPos() { return mvTrackRefTargetPos; }
	const cVector3f& GetTrackRefCameraPos() { return mvTrackRefCameraPos; }

	void FocusOnSelection(cEditorSelection* apSelection);

	void UpdateCamera(const cVector2f& avMouseRel);

	cVector3f GetSmoothedDisplacement(const cVector3f& avDisp, float afMaxLength=5.0f, float afTurnTo=1.0f);

	const cVector3f& GetPointFromClampedDistance(const cVector3f& avCenter, cVector3f& avPos, float afMaxDistance);

	void SetCameraPlanes(const cVector2f& avX);

protected:
	void UpdateCameraParams();
	void UpdateEngineCamera();

	void OnGridUpdated();

	float BinaryDistanceSearch(float& afStart, float& afEnd, const cBoundingVolume& aBV);
	bool BoundingVolumeFitsInView(const cBoundingVolume& aBV);

	iEditorViewport* mpViewport;
	cCamera* mpEngineCamera;
	eEditorCameraType mCamType;

	/////////////////////////////////////////////////
	// TODO: this as a pointer makes the thing crash on exit. Check why!
	iViewportCameraZoomFunc mpCurrentZoomFunc;
	float mfZoom;
	cVector3f mvCameraPosition;
	cVector3f mvTargetPosition;
	bool mbCameraUpdated;
	bool mbCameraPosUpdated;
	bool mbCameraPosNeedsUpdate;
	bool mbCameraSphCoordsNeedUpdate;
	bool mbTargetPosUpdated;
	bool mbTargetPosNeedsUpdate;
	bool mbTargetSphCoordsNeedUpdate;
	float mfTrackMouseMaxDistanceToTarget;

	bool mbLockToGrid;

	bool mbTumbleActive;
	bool mbTrackActive;
	bool mbZoomActive;

	cVector3f mvTrackRefMousePos;
	cVector3f mvTrackNewMousePos;
	cVector3f mvTrackRefTargetPos;
	cVector3f mvTrackRefCameraPos;
	cVector3f mvTrackCamTargetOffset;

	////////////////////////////
	// Orbit override
	bool mbKeepOrientation;
	cVector3f mvForward;
	cVector3f mvUp;
	cVector3f mvRight;

	////////////////////////////
	// Ortho
	float mfOrthoViewSize;
	////////////////////////////
	// Orbit
	float mfDistToTarget;
	cVector2f mvCameraSphCoords;
	////////////////////////////
	// Fly
	cVector2f mvTargetSphCoords;

	////////////////////////////
	// Settings (performance, customization)
	float mfMinZoom;
	float mfMaxZoom;
	float mfTumbleFactor;
	float mfTrackFactor;
	float mfMouseDragZoomFactor;
	float mfMouseWheelZoomFactor;
};

//--------------------------------------------

class iEditorViewport
{
	friend class cEditorViewportCamera;
public:
	iEditorViewport(iEditorBase* apEditor, cWorld* apWorld, iFrameBuffer* apFB=NULL, bool abDestroyFBOnExit=false);
	virtual ~iEditorViewport();

	/////////////////////////////////////
	// Editor helper stuff
	iEditorBase* GetEditorBase() { return mpEditorBase; }

	cEditorGrid* GetGrid() { return mpGrid; }

	void LogDebug();


	void SetViewportActive(bool abX);
	bool IsViewportActive() { return mbViewportActive; }

	cCamera* GetCamera() { return mCamera.GetEngineCamera(); }
	cEditorViewportCamera* GetVCamera() { return &mCamera; }

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

	const cVector2f& GetMouseViewportPosition();
	cVector2l GetMouseViewportPositionInt();

	const cVector3f& GetMouseWorldPosition(bool abForceUpdate=false);
	const cVector3f& GetGridCenter();

	const cVector3f& GetUnprojectedStart();
	const cVector3f& GetUnprojectedDir();
	const cVector3f& GetUnprojectedEnd();
	cBoundingVolume* GetRayBV();
	float GetRayEndDistance() { return mfRayEndDistance; }

	void AddViewportCallback(iRendererCallback* apCallback);

	void UpdateCameraPlanes();

	void SetClearColor(const cColor& aX);
	const cColor& GetClearColor();

	static void SetCamPlanes(const cVector2f& avX) { mvCamPlanes = avX; }
	static const cVector2f& GetCamPlanes() { return mvCamPlanes; }
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
	void CreateFrameBuffer(const cVector2l& avSize);
	void SetUpFrameBuffer(iFrameBuffer* apFB);
	void CreateGuiViewport(iWidget* apParentWidget);
	void UpdateUnprojection();

	iEditorBase* mpEditorBase;
	cEngine* mpEngine;
	cGraphics* mpGfx;
	cGuiSet* mpGuiSet;
	cViewport* mpEngineViewport;
	cEditorViewportCamera mCamera;

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

	cEditorGrid* mpGrid;

	iRendererCallback* mpViewportCallback;

	static int mlViewportCount;

	static bool mbCamPlanesUpdated;
	static cVector2f mvCamPlanes;
};

#endif
