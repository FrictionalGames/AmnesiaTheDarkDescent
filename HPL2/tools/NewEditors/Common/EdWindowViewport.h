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

#ifndef ED_WINDOW_VIEWPORT_H
#define ED_WINDOW_VIEWPORT_H

#include "../common/StdAfx.h"

using namespace hpl;

#include "../Common/EdWindowHandler.h"
#include "../Common/EdViewport.h"
#include "../Common/EdCamera.h"

//--------------------------------------------------------------------

class iEditor;

//--------------------------------------------------------------------

class iEdWindowViewportCamController : public iCamController
{
public:
	iEdWindowViewportCamController(cEdCamera* apCam, const tWString& asName);

	bool IsViewModeActive() { return mbViewMode; }

	void UpdateInput();

	bool mbViewMode;
};

//--------------------------------------------------------------------

class cCamControllerOrtho : public iEdWindowViewportCamController
{
public:
	cCamControllerOrtho(cEdCamera* apCam);

	void LockToGrid(bool) {}
	bool IsLockedToGrid() { return true; }

	bool IsActive();

	bool OnViewportMouseDown(const cViewportClick&);
	bool OnViewportMouseUp(const cViewportClick&);

	bool LoadState(cXmlElement*);
	bool SaveState(cXmlElement*);

protected:
	void OnUpdate(const cViewportClick&);

	void Track(const cVector2f&);
	void Zoom(const cVector2f&);

	bool mbIsTracking;
	bool mbIsZooming;

	cVector3f mvTrackRefCamPos, mvTrackRefTargetPos, mvTrackRefMousePos, mvTrackNewMousePos, mvTrackCamTargetOffset;
};

//--------------------------------------------------------------------

class cCamControllerMaya : public iEdWindowViewportCamController
{
public:
	cCamControllerMaya(cEdCamera* apCam);

	void LockToGrid(bool);
	bool IsLockedToGrid() { return mbLockedToGrid; }

	bool IsActive();

	bool OnViewportMouseDown(const cViewportClick&);
	bool OnViewportMouseUp(const cViewportClick&);

protected:
	void OnUpdate(const cViewportClick&);

	void Tumble(const cVector2f&);
	void Track(const cVector2f&);
	void Zoom(const cVector2f&);

	bool mbIsTumbling;
	bool mbIsTracking;
	bool mbIsZooming;

	float mfTumbleFactor;
	float mfTrackFactor;
	float mfZoomFactor;

	// Track data
	bool mbLockedToGrid;

	cVector3f mvTrackRefMousePos;
	cVector3f mvTrackNewMousePos;
	cVector3f mvTrackRefTargetPos;
	cVector3f mvTrackRefCameraPos;
	cVector3f mvTrackCamTargetOffset;
};

//--------------------------------------------------------------------

class cCamModeOrtho : public iCamMode
{
public:
	cCamModeOrtho(cEdCamera*, int, const tWString&, ePlaneNormal, 
					const cVector3f&, const cVector3f&, const cVector3f&);

	void GetViewSpacePositionAndSize(const cVector3f&, const cVector3f&, cVector3f&, cVector3f&);

	bool AllowGridPlaneSwitch() { return false; }
	bool AllowLockToGridSwitch() { return false; }

	void FocusSpecific(const cVector3f& avCenter, const cVector3f& avDiff);

protected:
	void OnSetActive(bool);

	ePlaneNormal mPlane;
	cVector3f mvRight, mvUp, mvForward;
};

//--------------------------------------------------------------------

class cCamModePerspective : public iCamMode
{
public:
	cCamModePerspective(cEdCamera*, int);

	void GetViewSpacePositionAndSize(const cVector3f&, const cVector3f&, cVector3f&, cVector3f&);

	void FocusSpecific(const cVector3f& avCenter, const cVector3f& avDiff);

protected:
	void OnSetActive(bool);
};

//--------------------------------------------------------------------

////////////////////////////////////////////////
// VIEWPORT WINDOW
////////////////////////////////////////////////

//--------------------------------------------------------------------

class cEdWindowViewport : public iEdViewport
{
public:
	cEdWindowViewport(iEditor* apEditor, iFrameBuffer* apFB=NULL);
	~cEdWindowViewport();

	void SetDrawGrid(bool abX) { mbDrawGrid = abX; }
	bool GetDrawGrid() { return mbDrawGrid; }

	void SetDrawAxes(bool abX) { mbDrawAxes = abX; }
	bool GetDrawAxes() { return mbDrawAxes; }

	void SetDrawDebug(bool abX) { mbDrawDebug = abX; }
	bool GetDrawDebug() { return mbDrawDebug; }

	void FocusOnBoundingVolume(cBoundingVolume*);

	bool LoadState(cXmlElement* apElement);
	bool SaveState(cXmlElement* apElement, cXmlElement** apStateElem);

	iEdAction* CreateFocusAction();
protected:
	///////////////////////////
	// Own functions

	// iEditorViewport Callbacks implementation
	bool OnViewportUpdate(const cGuiMessageData& aData);
	bool OnViewportGotFocus(const cGuiMessageData& aData);
	bool OnViewportLostFocus(const cGuiMessageData& aData);

	bool OnViewportDraw(const cGuiMessageData& aData);

	bool OnViewportMouseMove(const cGuiMessageData& aData);
	bool OnViewportMouseDown(const cGuiMessageData& aData);
	bool OnViewportMouseUp(const cGuiMessageData& aData);
	bool OnViewportKeyPress(const cGuiMessageData& aData);
	bool OnViewportKeyRelease(const cGuiMessageData& aData);

	void OnSetLock(bool);
	void OnSetCamMode(iCamMode*);

    ///////////////////////////
	// Implemented Functions
	void OnCreateLayout();

	void OnSetSize(const cVector2f&);

	bool WindowSpecificInputCallback(iEdInput*) { return true; }

	void OnPostSolidDraw(cRendererCallbackFunctions* apFunctions);
	void OnPostTranslucentDraw(cRendererCallbackFunctions* apFunctions);

	///////////////////////////////////
	// Data
	iWidget* mpPrevAttention;

	bool mbDrawGrid;
	bool mbDrawDebug;
	bool mbDrawAxes;

	// Layout specific
	cWidgetLabel* mpPresetLabel;

	cGuiGfxElement* mpGfxRedFrame;
	cGuiGfxElement* mpGfxPointerTumble;
	cGuiGfxElement* mpGfxPointerTrack;
	cGuiGfxElement* mpGfxPointerZoom;
};

//---------------------------------------------------------

#endif //ED_WINDOW_VIEWPORT_H

