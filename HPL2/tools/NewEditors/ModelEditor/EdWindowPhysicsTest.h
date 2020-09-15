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

#ifndef ED_WINDOW_PHYSICS_TEST_H
#define ED_WINDOW_PHYSICS_TEST_H

//--------------------------------------------------------------------

#include "../Common/EdWindowHandler.h"
#include "../Common/EdViewport.h"

//--------------------------------------------------------------------

class cBodyPicker;
class cEdWindowPhysicsTest;

//--------------------------------------------------------------------

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

//--------------------------------------------------------------------

class cEdWindowPhysicsTest : public iEdViewport
{
	friend class cPhysicsTestRenderCallback;
public:
	cEdWindowPhysicsTest(iEditor* apEditor, iFrameBuffer* apFB);
	~cEdWindowPhysicsTest();

	int GetSelectedAnimation(); 
	void SetSelectedAnimation(int alX);

	bool IsFocused() { return true; }

protected:
	bool WindowSpecificInputCallback(iEdInput* apInput);

	void SetUpRender();
	void SetUpModel();

	void UpdateBuoyancy();

	void OnCreateLayout();
	void OnSetActive(bool abX);

	bool OnViewportUpdate(const cGuiMessageData& aData);
	bool OnViewportMouseDown(const cGuiMessageData& aData);
	bool OnViewportMouseUp(const cGuiMessageData& aData);

	bool OnViewportKeyPress(const cGuiMessageData& aData);
	bool OnViewportKeyRelease(const cGuiMessageData& aData);

	// Drawing funcs
	void OnPostSolidDraw(cRendererCallbackFunctions*);
	void DrawSkeletonRec(cRendererCallbackFunctions* apFunctions, cNode3D* apBoneState); 

	cVector2f mvFrameBufferSize;

	//////////////////////////////////////
	// Test settings layout
	cEdInputEnum* mpInpAnimations;
	cEdInputBool* mpInpShowDebug;
	cEdInputBool* mpInpShowSkeleton;
	cEdInputBool* mpInpMainLight;
	cEdInputBool* mpInpBackLight;
	cEdInputBool* mpInpBuoyancyActive;
	cEdInputNumber* mpInpBuoyancyDensity;
	cEdInputNumber* mpInpBuoyancyLinearViscosity;
	cEdInputNumber* mpInpBuoyancyAngularViscosity;
	cEdInputNumber* mpInpBuoyancySurfaceHeight;

	iWidget* mpPrevAttention;

	cWorld* mpTestWorld;
	iPhysicsWorld *mpTestPhysicsWorld;
	std::vector<iPhysicsJoint*> mvJoints;
	cBodyPicker mBodyPicker;
	iPhysicsBody* mpFloorBody;

	iLight* mpMainLight;
	iLight* mpBackLight;
	cMeshEntity* mpFloor;
	cMeshEntity* mpWater;
	cMeshEntity* mpTestEntity;

	cVector3f mvDragPos;

	bool mbDrawDebug;
	bool mbDrawSkeleton;

	bool mbViewportEnlarged;


	////////////////////
	// Persistent data
	// Animation
	static int mlLastSelectedAnimation;

	// Water data
	static bool mbBuoyancyActive;
	static float mfBuoyancyDensity;
	static float mfBuoyancyLinearViscosity;
	static float mfBuoyancyAngularViscosity;
	static cPlanef mBuoyancySurface;
};

//--------------------------------------------------------------------

#endif // MODEL_EDITOR_WINDOW_PHYSICS_TEST_H



