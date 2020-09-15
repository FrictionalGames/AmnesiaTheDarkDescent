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

#ifndef ED_CAMERA_H
#define ED_CAMERA_H

//--------------------------------------------------------------------

#include "../Common/StdAfx.h"

#include "../Common/EdGrid.h"

using namespace hpl;

//--------------------------------------------------------------------

class cEdCamera;
class cViewportClick;

class iEdViewport;

//--------------------------------------------------------------------

class iCamController
{
public:
	iCamController(cEdCamera* apCam, const tWString& asName);

	const tWString& GetName() { return msName; }

	virtual bool OnViewportMouseDown(const cViewportClick&)=0;
	virtual bool OnViewportMouseUp(const cViewportClick&)=0;

	virtual bool IsActive()=0;

	virtual void LockToGrid(bool)=0;
	virtual bool IsLockedToGrid()=0;

	virtual void UpdateInput()=0;
	virtual void Update(const cViewportClick&);

	virtual bool LoadState(cXmlElement*);
	virtual bool SaveState(cXmlElement*);
protected:
	virtual void OnUpdate(const cViewportClick&)=0;

	cEdCamera* mpCam;
	tWString msName;

	bool mbForceUpdate;
};

//--------------------------------------------------------------------

typedef std::vector<iCamController*> tCamControllerVec;

//--------------------------------------------------------------------

class cEdCamRenderer
{
public:
	cEdCamRenderer(const tWString& asName, eRenderer aX) : msName(asName), mRenderer(aX)
	{}

	tWString msName;
	eRenderer mRenderer;
};

//--------------------------------------------------------------------

class iCamMode
{
public:
	iCamMode(cEdCamera*, int, const tWString&);
	virtual ~iCamMode();

	cEdCamera* GetCam() { return mpCam; }

	int GetID() { return mlID; }
	const tWString& GetName() { return msName; }

	void SetActive(bool);

	void AddCamController(iCamController*);
	void SetController(int);
	void SetController(const tWString&);
	iCamController* GetCurrentController() { return mpCurrentController; }

	int GetNumControllers();
	iCamController* GetController(int alX);

	void AddRenderer(const tWString&, eRenderer);
	void SetCurrentRenderer(int);
	void SetCurrentRenderer(const tWString&);
	cEdCamRenderer* GetCurrentRenderer() { return mpCurrentRenderer; }

	int GetNumRenderers();
	cEdCamRenderer* GetRenderer(int);

	virtual void GetViewSpacePositionAndSize(const cVector3f&, const cVector3f&, cVector3f&, cVector3f&) {}

	bool LoadState(cXmlElement*);
	bool SaveState(cXmlElement*);


	virtual bool AllowGridPlaneSwitch() { return true; }
	virtual bool AllowLockToGridSwitch() { return true; }

	virtual void FocusSpecific(const cVector3f& avCenter, const cVector3f& avDiff) {}

protected:
	virtual void OnSetActive(bool)=0;

	tCamControllerVec mvControllers;
	iCamController* mpCurrentController;

	std::vector<cEdCamRenderer> mvRenderers;
	cEdCamRenderer* mpCurrentRenderer;

	cEdCamera* mpCam;
	int mlID;
	tWString msName;
};

//--------------------------------------------------------------------

typedef std::vector<iCamMode*> tCamModeVec;

//--------------------------------------------------------------------

class iCameraZoomFunc
{
public:
	iCameraZoomFunc() {}
	//{
	//	mfMaxDistance = 50;
	//}
	iCameraZoomFunc(float afMinZoom, float afMaxZoom) {}
	//{
	//	mfMinZoom = afMinZoom;
	//	mfMaxZoom = afMaxZoom;
	//}

	virtual float GetOrthoViewSizeFromZoom(float afX)=0;
	virtual float GetDistToTargetFromZoom(float afX)=0;

	virtual float GetZoomFromDistToTarget(float afX)=0;
	virtual float GetZoomFromOrthoViewSize(float afX)=0;

protected:
	float mfMinZoom;
	float mfMaxZoom;

	float mfMaxDistance;
};

//--------------------------------------------------------------------

class cEdCamera
{
	friend class cEdGrid;
	friend class iCamController;
	friend class iCamMode;

public:
	cEdCamera(iEdViewport*);
	~cEdCamera();

	iEdViewport* GetViewport() { return mpViewport; }
	cCamera* GetEngineCamera() { return mpEngineCamera; }

	void AddCamMode(iCamMode*);
	iCamMode* GetCurrentCamMode() { return mpCurrentMode; }
	void SetCameraMode(int);
	void SetCameraMode(const tWString&);
	void SetCameraMode(iCamMode*);

	int GetNumModes();
	iCamMode* GetMode(int);

	const tCamModeVec& GetModes() { return mvModes; }

	iCamController* GetCurrentController();

	bool IsOrtho();

//	void LockToGrid(bool abX);
//	bool IsLockedToGrid() { return mbLockToGrid; }
	
	void SetKeepOrientation(bool abX) { mbKeepOrientation = abX; }
	void SetOrientation(const cVector3f& avRight, const cVector3f& avUp, const cVector3f& avForward);

	bool KeepsOrientation() { return mbKeepOrientation; }
	const cVector3f& GetOrientationRight() { return mvRight; }
	const cVector3f& GetOrientationUp() { return mvUp; }
	const cVector3f& GetOrientationForward() { return mvForward; }

	//iViewportCameraZoomFunc* GetZoomFunc() { return &mpCurrentZoomFunc; }

	void SetCameraPosition(const cVector3f& avPosition);
	void SetCameraSphCoords(const cVector2f& avSphCoords);
	void SetTargetPosition(const cVector3f& avTargetPos);
	void SetTargetSphCoords(const cVector2f& avSphCoords);
	void SetDistToTarget(float);

	const cVector3f& GetCameraPosition();
	const cVector2f& GetCameraSphCoords();
	const cVector3f& GetTargetPosition();
	const cVector2f& GetTargetSphCoords();
	float GetDistToTarget();

	void FetchSettings();

	void FocusOnBoundingVolume(cBoundingVolume*);

	cVector3f GetSmoothedDisplacement(const cVector3f& avDisp, float afMaxLength=5.0f, float afTurnTo=1.0f);
	float GetCamTrackAngleLowerFactor(const cVector3f& avCam, const cVector3f& avMousePos);

	const cVector3f& GetPointFromClampedDistance(const cVector3f& avCenter, cVector3f& avPos, float afMaxDistance);

	void SetCameraPlanes(const cVector2f& avX);

	void UpdateEngineCamera();

	bool LoadState(cXmlElement*);
	bool SaveState(cXmlElement*);
protected:
	void UpdateCameraParams();

	void OnGridUpdated();

	iEdViewport* mpViewport;
	cCamera* mpEngineCamera;

	//iViewportCameraZoomFunc mpCurrentZoomFunc;
	//float mfZoom;

	////////////////////////////
	// Camera Params
	cVector3f mvCameraPosition;
	cVector3f mvTargetPosition;

	float mfDistToTarget;
	cVector2f mvCameraSphCoords;
	cVector2f mvTargetSphCoords;
	
	bool mbCameraUpdated;
	bool mbCameraPosUpdated;
	bool mbCameraPosNeedsUpdate;
	bool mbCameraSphCoordsNeedUpdate;
	bool mbTargetPosUpdated;
	bool mbTargetPosNeedsUpdate;
	bool mbTargetSphCoordsNeedUpdate;
	bool mbDistToTargetNeedsUpdate;
	float mfTrackMouseMaxDistanceToTarget;

	float mfTrackUnprojectDist;

	////////////////////////////
	// Orbit override
	bool mbKeepOrientation;
	cVector3f mvRight;
	cVector3f mvUp;
	cVector3f mvForward;

	////////////////////////////
	// Settings (performance, customization)

	tCamModeVec mvModes;
	iCamMode*	mpCurrentMode;
};

//--------------------------------------------------------------------

#endif 
