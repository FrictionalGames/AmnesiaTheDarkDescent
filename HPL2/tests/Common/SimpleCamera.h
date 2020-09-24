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

#include "hpl.h"

using namespace hpl;

class cSimpleCamera : public iUpdateable
{
public:
	cSimpleCamera(const tString& asAppName,cEngine *apGame, cWorld *apWorld, float afSpeed,cVector3f avStartPos,bool abShowFPS, bool abLoadSkin=true);
	~cSimpleCamera();

	void Update(float afFrameTime);
	
	void OnDraw(float afFrameTime);

	void OnPostRender(float afFrameTime);

	void AppGotMouseFocus();
	void AppLostMouseFocus();
	
	void SetActive(bool abX){ mbActive = abX;}

	cGuiSet* GetSet(){ return mpGuiSet;}

	cCamera* GetCamera(){ return mpCamera;}
	cViewport* GetViewport(){ return mpViewport;}

	iFontData* GetFont(){ return mpFont; }

	float GetSpeed(){ return mfSpeed;}
	void SetSpeed(float afSpeed){ mfSpeed = afSpeed;}

	void SetDrawFPS(bool abX){ mbDrawFPS =abX;}

	void SetMouseMode(bool abActive);
	bool GetMouseMode(){ return mbMouseActive;}

	void SetCameraInMouseMode(bool abX){ mbCameraInMouseMode=abX;}
	void SetMatrixFlyCamera(bool abX);

	void ListContainerNodeData(iRenderableContainerNode *apNode, int alLevel);

	cMatrixf m_mtxLastView;
private:
	void CalculateMouseCamera();

	cEngine *mpEngine;

	tString msAppName;
	
	cGui *mpGui;
	cGuiSet *mpGuiSet;
	cGuiSkin *mpGuiSkin;

	cGuiGfxElement *mpTestGfx;
	
	iFontData* mpFont;
	cCamera* mpCamera;
	cViewport *mpViewport;

	cVector3f mvMouseCameraPos;
	cVector3f mvMouseCameraAngles;
	float mfMouseCameraDistance; 
    
	float mfSpeed;
	bool mbActive;
	bool mbMouseActive;
	bool mbCameraInMouseMode;
	bool mbMatrixFlyCamera;

	bool mbDrawFPS;

	std::list<float> mlstFrameTimes;
	float mfTakeFrameTimeCount;
	float mfAvgFrameTime;
};
