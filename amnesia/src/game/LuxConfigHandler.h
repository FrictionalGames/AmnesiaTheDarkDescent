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

#ifndef LUX_CONFIG_HANDLER_H
#define LUX_CONFIG_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxConfigHandler
{
public:	
	cLuxConfigHandler();
	~cLuxConfigHandler();

	void LoadMainConfig();
	void SaveMainConfig();

	bool ShowRestartWarning(cGuiSet* apSet, void* apObject, tGuiCallbackFunc apCallback);

	void SetGameNeedsRestart() { mbGameNeedsRestart=true; mbRestartDialogShown=false; }

	// Variables
	bool mbLoadDebugMenu;
	bool mbFirstStart;
	tString msScreenShotExt;

	bool mbCreateAndLoadCompressedMaps;
	bool mbForceCacheLoadingAndSkipSaving;

	tString msLangFile;
	
	cVector2l mvScreenSize;
    int mlDisplay;
	bool mbFullscreen;
	bool mbVSync;
	bool mbAdaptiveVSync;
	int mlTextureQuality;
	int mlTextureFilter;
	float mfTextureAnisotropy;
	int mlShadowQuality;
	int mlShadowRes;

	bool mbSSAOActive;
	int mlSSAOSamples;
	int mlSSAOResolution; //0= medium(div2), 1=high (same as screen resolution)
	
	int mlParallaxQuality;
	bool mbParallaxEnabled;

	bool mbOcclusionTestLights;
	
	bool mbEdgeSmooth;
		
	bool mbWorldReflection;
	bool mbRefraction;
	bool mbShadowsActive;

	bool mbForceShaderModel3And4Off;

	bool mbFastPhysicsLoad;
	bool mbFastStaticLoad;
	bool mbFastEntityLoad;

	int mlSoundDevID;
	int mlMaxSoundChannels;
	int mlSoundStreamBuffers;
	int mlSoundStreamBufferSize;

	
private:
	bool mbGameNeedsRestart;
	bool mbRestartDialogShown;
};

//----------------------------------------------


#endif // LUX_DEBUG_HANDLER_H
