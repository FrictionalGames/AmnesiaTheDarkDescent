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

#include "engine/ScriptFuncs.h"

#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "engine/Engine.h"
#include "graphics/Graphics.h"
#include "sound/Sound.h"
#include "resources/Resources.h"
#include "math/Math.h"
#include "scene/Scene.h"
#include "scene/Light.h"
#include "scene/World.h"
#include "scene/SoundEntity.h"
#include "input/Input.h"
#include "system/System.h"
#include "system/LowLevelSystem.h"
#include "sound/MusicHandler.h"
#include "resources/SoundManager.h"
#include "resources/SoundEntityManager.h"
#include "resources/TextureManager.h"
#include "sound/SoundData.h"
#include "sound/SoundChannel.h"
#include "sound/SoundHandler.h"
#include "sound/SoundEntityData.h"
#include "system/String.h"
#include "physics/PhysicsJoint.h"
#include "physics/PhysicsJointHinge.h"
#include "physics/PhysicsJointScrew.h"
#include "physics/PhysicsJointSlider.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsController.h"
#include "system/Script.h"
#include "scene/ParticleSystem.h"
#include "scene/MeshEntity.h"
#include "scene/BillBoard.h"
#include "scene/Beam.h"
#include "graphics/Renderer.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// JOINT CALLBACK
	//////////////////////////////////////////////////////////////////////////

	cScriptJointCallback::cScriptJointCallback(cScene *apScene)
	{
		mpScene = apScene;

		msMaxFunc = "";
		msMinFunc = "";
	}

	void cScriptJointCallback::OnMinLimit(iPhysicsJoint *apJoint)
	{
		/*if(msMinFunc!="")
		{
			iScript *pScript = mpScene->GetWorld()->GetScript();

			tString sCommand = msMinFunc + "(\"" + apJoint->GetName() + "\")";
			if(pScript->Run(sCommand)==false){
				Warning("Couldn't run script command '%s'\n",sCommand.c_str());
			}

		}*/
	}

	void cScriptJointCallback::OnMaxLimit(iPhysicsJoint *apJoint)
	{
		/*if(msMaxFunc!="")
		{
			iScript *pScript = mpScene->GetWorld()->GetScript();

			tString sCommand = msMaxFunc + "(\"" + apJoint->GetName() + "\")";
			if(pScript->Run(sCommand)==false){
				Warning("Couldn't run script command '%s'\n",sCommand.c_str());
			}
		}*/
	}

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	static cResources *gpResources=NULL;
	static cSystem *gpSystem=NULL;
	static cInput *gpInput=NULL;
	static cGraphics *gpGraphics=NULL;
	static cScene *gpScene=NULL;
	static cSound *gpSound=NULL;
	static cEngine *gpGame=NULL;

	//-----------------------------------------------------------------------

	static void __stdcall Print(std::string asText)
	{
		Log(asText.c_str());
	}
	
	static std::string __stdcall FloatToString(float afX)
	{
		char sTemp[30];
		sprintf(sTemp,"%f",afX);
		return (std::string) sTemp;
	}
	
	static std::string __stdcall IntToString(int alX)
	{
		char sTemp[30];
		sprintf(sTemp,"%d",alX);
		return (std::string) sTemp;
	}
	
	static float __stdcall RandFloat(float afMin, float afMax)
	{
		return cMath::RandRectf(afMin,afMax);
	}
	
	static int __stdcall RandInt(int alMin, int alMax)
	{
		return cMath::RandRectl(alMin,alMax);
	}
	
	static bool __stdcall StringContains(std::string asString, std::string asSubString)
	{
		return cString::GetLastStringPos(asString,asSubString)>=0;
	}
	
	static void __stdcall ResetLogicTimer()
	{
		gpGame->ResetLogicTimer();
	}
	
	

	//-----------------------------------------------------------------------


	/////////////////////////////////////////////////////////////////////////
	/////// LOCAL VARS //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	static void __stdcall CreateLocalVar(std::string asName, int alVal)
	{
		if(gpGame->GetLocalVar(asName)==NULL)
		{
			cScriptVar* pVar = gpGame->CreateLocalVar(asName);
			pVar->mlVal = alVal;
		}
	}
	
	static void __stdcall SetLocalVar(std::string asName, int alVal)
	{
		cScriptVar* pVar = gpGame->CreateLocalVar(asName);
		pVar->mlVal = alVal;
	}
	
	static void __stdcall AddLocalVar(std::string asName, int alVal)
	{
		cScriptVar* pVar = gpGame->CreateLocalVar(asName);
		pVar->mlVal += alVal;
	}

	static int __stdcall GetLocalVar(std::string asName)
	{
		cScriptVar* pVar = gpGame->GetLocalVar(asName);
		if(pVar==NULL)
		{
			Error("Couldn't find local var '%s'\n",asName.c_str());
			return 0;
		}
		return pVar->mlVal;
	}
	
	//-----------------------------------------------------------------------
	/////////////////////////////////////////////////////////////////////////
	/////// GLOBAL VARS //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	static void __stdcall CreateGlobalVar(std::string asName, int alVal)
	{
		if(gpGame->GetGlobalVar(asName)==NULL)
		{
			cScriptVar* pVar = gpGame->CreateGlobalVar(asName);
			pVar->mlVal = alVal;
		}
	}
	
	static void __stdcall SetGlobalVar(std::string asName, int alVal)
	{
		cScriptVar* pVar = gpGame->CreateGlobalVar(asName);
		pVar->mlVal = alVal;
	}


	static void __stdcall AddGlobalVar(std::string asName, int alVal)
	{
		cScriptVar* pVar = gpGame->CreateGlobalVar(asName);
		pVar->mlVal += alVal;
	}


	static int __stdcall GetGlobalVar(std::string asName)
	{
		cScriptVar* pVar = gpGame->GetGlobalVar(asName);
		if(pVar==NULL)
		{
			Error("Couldn't find global var '%s'\n",asName.c_str());
			return 0;
		}
		return pVar->mlVal;
	}
	
	//-----------------------------------------------------------------------



	void cScriptFuncs::Init(	cGraphics* apGraphics,
				cResources *apResources,
				cSystem *apSystem,
				cInput *apInput,
				cScene *apScene,
				cSound *apSound,
				cEngine *apGame
			)
	{
		gpGraphics = apGraphics;
		gpResources = apResources;
		gpSystem = apSystem;
		gpInput = apInput;
		gpScene = apScene;
		gpSound = apSound;
		gpGame = apGame;

		//General
		/*gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(Print));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(FloatToString));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(IntToString));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(RandFloat));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(RandInt));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(StringContains));

		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(ResetLogicTimer));

		//Renderer
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetAmbientColor));

		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetSkybox));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetSkyboxActive));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetSkyboxColor));

		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(CreateParticleSystemOnCamera));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetFogActive));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetFogCulling));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetFogProperties));

		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetSectorProperties));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetSectorPortalActive));


		//Resources
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(PreloadSound));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(Translate));
		
		//Mesh Entity
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetMeshActive));
		
		//Beams
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(CreateBeam));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(DestroyBeam));

		//Particle systems
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetParticleSystemActive));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(CreateParticleSystem));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(KillParticleSystem));

		//Light
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(FadeLight3D));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(AttachBillboardToLight3D));

		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetLight3DVisible));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetLight3DFlickerActive));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetLight3DFlicker));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetLight3DOnlyAffectInSector));

		//Sound
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(PlayMusic));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(StopMusic));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(PlaySoundEntity));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(StopSoundEntity));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(FadeInSoundEntity));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(FadeOutSoundEntity));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(PlayGuiSound));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(CreateSoundEntity));


		//Physics
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetJointCallback));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(BreakJoint));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(GetJointProperty));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(GetBodyProperty));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetBodyProperty));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetJointProperty));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(AttachBodiesWithJoint));


		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetJointControllerActive));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(ChangeJointController));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetJointControllerPropertyFloat));


		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(AddBodyForce));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(AddBodyImpulse));

		//Local vars
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(CreateLocalVar));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetLocalVar));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(AddLocalVar));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(GetLocalVar));

		//Global vars
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(CreateGlobalVar));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(SetGlobalVar));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(AddGlobalVar));
		gpSystem->GetLowLevel()->AddScriptFunc(SCRIPT_REGISTER_FUNC(GetGlobalVar));*/
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------



	//-----------------------------------------------------------------------

}
