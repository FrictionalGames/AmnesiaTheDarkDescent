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

#ifndef MODEL_EDITOR_WINDOW_PHYSICS_TEST_H
#define MODEL_EDITOR_WINDOW_PHYSICS_TEST_H

//--------------------------------------------------------------------

#include "../common/EditorWindow.h"
#include "../common/EditorViewport.h"

class cBodyPicker;
class cModelEditorWindowPhysicsTest;


class cBodyPicker : public iPhysicsRayCallback
{
public:
	cBodyPicker();

	void Clear();

	bool OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams);

	iPhysicsBody* mpPickedBody;
	cVector3f mvPos;
	cVector3f mvLocalPos;
	float mfLastT;
	float mfDist;
};

class cPhysicsTestRenderCallback : public iRendererCallback
{
public:
	cPhysicsTestRenderCallback()
	{
		mbDrawDebug=false;
		mbDrawSkeleton=false;
	}

	void OnPostSolidDraw(cRendererCallbackFunctions *apFunctions);

	void OnPostTranslucentDraw(cRendererCallbackFunctions *apFunctions){}

	void DrawSkeletonRec(cRendererCallbackFunctions* apFunctions, cNode3D* apBoneState);

	cModelEditorWindowPhysicsTest* mpWindow;
	cBodyPicker* mpBodyPicker;

	cVector3f mvDragPos;

	bool mbDrawDebug;
	bool mbDrawSkeleton;

protected:
    
};

//--------------------------------------------------------------------

class cModelEditorWindowPhysicsTest : public iEditorWindow, public iEditorViewport
{
	friend class cPhysicsTestRenderCallback;
public:
	cModelEditorWindowPhysicsTest(iEditorBase* apEditor, iFrameBuffer* apFB);
	~cModelEditorWindowPhysicsTest();

	int GetSelectedAnimation(); 
	void SetSelectedAnimation(int alX);

protected:
	bool WindowSpecificInputCallback(iEditorInput* apInput);

	void SetUpRender();
	void SetUpModel();

	void UpdateBuoyancy();

	void OnInitLayout();
	void OnSetActive(bool abX);

	bool OnViewportUpdate(const cGuiMessageData& aData);
	bool OnViewportMouseDown(const cGuiMessageData& aData);
	bool OnViewportMouseUp(const cGuiMessageData& aData);

	bool OnViewportKeyPress(const cGuiMessageData& aData);
	bool OnViewportKeyRelease(const cGuiMessageData& aData);

	cVector2f mvFrameBufferSize;

	bool mbViewMode;

	//////////////////////////////////////
	// Test settings layout
	cEditorInputEnum* mpInpAnimations;
	cEditorInputBool* mpInpShowDebug;
	cEditorInputBool* mpInpShowSkeleton;
	cEditorInputBool* mpInpMainLight;
	cEditorInputBool* mpInpBackLight;
	cEditorInputBool* mpInpBuoyancyActive;
	cEditorInputNumber* mpInpBuoyancyDensity;
	cEditorInputNumber* mpInpBuoyancyLinearViscosity;
	cEditorInputNumber* mpInpBuoyancyAngularViscosity;
	cEditorInputNumber* mpInpBuoyancySurfaceHeight;

	iWidget* mpPrevAttention;

	cWorld* mpTestWorld;
	iPhysicsWorld *mpTestPhysicsWorld;
	std::vector<iPhysicsJoint*> mvJoints;
	cBodyPicker mBodyPicker;
	cPhysicsTestRenderCallback mRenderCallback;
	iPhysicsBody* mpFloorBody;

	iLight* mpMainLight;
	iLight* mpBackLight;
	cMeshEntity* mpFloor;
	cMeshEntity* mpWater;
	cMeshEntity* mpTestEntity;

	//////////////
	// Water data
	static bool mbBuoyancyActive;
	static float mfBuoyancyDensity;
	static float mfBuoyancyLinearViscosity;
	static float mfBuoyancyAngularViscosity;
	static cPlanef mBuoyancySurface;
};

//--------------------------------------------------------------------

#endif // MODEL_EDITOR_WINDOW_PHYSICS_TEST_H



