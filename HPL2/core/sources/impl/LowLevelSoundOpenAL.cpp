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


#include "impl/LowLevelSoundOpenAL.h"
#include "system/String.h"
#include "impl/OpenALSoundData.h"
#include "impl/OpenALSoundEnvironment.h"

#include "math/Math.h"

#include "system/LowLevelSystem.h"

#ifdef USE_OALWRAPPER
# include "OALWrapper/OAL_Funcs.h"
# include "OALWrapper/OAL_Effect_Reverb.h"
#else
# include "OpenAL/OAL_Funcs.h"
# include "OpenAL/OAL_Effect_Reverb.h"
#endif

namespace hpl {

	//-----------------------------------------------------------------------

	cSoundDeviceIdentifierOpenAL::cSoundDeviceIdentifierOpenAL(int alID, const tString& asName, bool abDefault)
	{
		mlID = alID;
		msName = asName;
		mbDefault = abDefault;
	}

	//-----------------------------------------------------------------------

	void iLowLevelSound::PopulateAvailableSoundDevices(tSoundDeviceVec& avSoundDeviceVec)
	{
		STLDeleteAll(avSoundDeviceVec);
	
		bool bDefaultFound = false;
		tStringVec vDeviceNames = OAL_Info_GetOutputDevices();
		for(int i=0;i<(int)vDeviceNames.size();++i)
		{
			const tString& sDev = vDeviceNames[i];
			bool bIsDefault = false;

			if(bDefaultFound==false && cString::GetFirstStringPos(sDev, OAL_Info_GetDefaultOutputDevice())!=-1)
			{
				bDefaultFound = true;
				bIsDefault = true;
			}

			iSoundDeviceIdentifier* pSndDev = hplNew(cSoundDeviceIdentifierOpenAL,(i, sDev, bIsDefault));
			avSoundDeviceVec.push_back(pSndDev);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelSoundOpenAL::cLowLevelSoundOpenAL()
	{
		mvFormats[0] = "WAV";
		mvFormats[1] = "OGG";
		mvFormats[2] = "";
		mbInitialized = false;
		mbEnvAudioEnabled = false;
		mbNullEffectAttached = false;
	}

	//-----------------------------------------------------------------------

	cLowLevelSoundOpenAL::~cLowLevelSoundOpenAL()
	{
		if(mbInitialized)
			OAL_Close();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHOD
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iSoundData* cLowLevelSoundOpenAL::LoadSoundData(const tString& asName, const tWString& asFilePath,
												const tString& asType, bool abStream,bool abLoopStream)
	{
		cOpenALSoundData* pSoundData = hplNew( cOpenALSoundData, (asName,abStream) );
		pSoundData->SetLoopStream(abLoopStream);
		
		if(pSoundData->CreateFromFile(asFilePath)==false)
		{
			hplDelete(pSoundData);
			return NULL;
		}
		
		return pSoundData;
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOpenAL::GetSupportedFormats(tStringList &alstFormats)
	{
		int lPos = 0;

		while(mvFormats[lPos]!="")
		{
			alstFormats.push_back(mvFormats[lPos]);
			lPos++;
		}
	}
	//-----------------------------------------------------------------------
	
	void cLowLevelSoundOpenAL::UpdateSound(float afTimeStep)
	{
		OAL_Update();
	}
	
	//-----------------------------------------------------------------------

	void cLowLevelSoundOpenAL::SetListenerAttributes(const cVector3f &avPos,const cVector3f &avVel,
							const cVector3f &avForward,const cVector3f &avUp)
	{
		mvListenerPosition = avPos;
		mvListenerVelocity = avVel;
		mvListenerForward = avForward;
		mvListenerUp = avUp;

		mvListenerRight = cMath::Vector3Cross(mvListenerForward,mvListenerUp);

//		m_mtxListener = cMatrixf(
//				-mvListenerRight.x, -mvListenerRight.y,-mvListenerRight.z, avPos.x,
//				-mvListenerUp.x, -mvListenerUp.y,-mvListenerUp.z, avPos.y,
//				-mvListenerForward.x, -mvListenerForward.y,-mvListenerForward.z, avPos.z,
//				0, 0,0, 1
//			);
		m_mtxListener = cMatrixf::Identity;
		m_mtxListener.SetRight(mvListenerRight);
		m_mtxListener.SetUp(mvListenerUp);
		m_mtxListener.SetForward(mvListenerForward*-1);
		m_mtxListener = cMath::MatrixInverse(m_mtxListener);
		m_mtxListener.SetTranslation(mvListenerPosition);

		float fVel[3]={0,0,0};
		OAL_Listener_SetAttributes ( avPos.v, avVel.v, (avForward*(-1)).v, avUp.v );
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOpenAL::SetListenerPosition(const cVector3f &avPos)
	{
		mvListenerPosition = avPos;

		OAL_Listener_SetAttributes ( avPos.v, mvListenerVelocity.v, (mvListenerForward*(-1)).v, mvListenerUp.v );
	}
	
	//-----------------------------------------------------------------------
		
	void cLowLevelSoundOpenAL::SetListenerAttenuation (bool abEnabled)
	{
		mbListenerAttenuation = abEnabled;
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOpenAL::SetSetRolloffFactor(float afFactor)
	{
		OAL_SetRollOffFactor ( afFactor );
	}
	
	//-----------------------------------------------------------------------

	void cLowLevelSoundOpenAL::SetVolume(float afVolume)
	{
		mfVolume = afVolume;

		OAL_Listener_SetMasterVolume ( afVolume );
	}

	//-----------------------------------------------------------------------

	/*void cLowLevelSoundOpenAL::LogSoundStatus()
	{
		//mbLogSounds = !mbLogSounds;
		//OAL_SetLoggingEnabled ( mbLogSounds );
	}
*/
	//-----------------------------------------------------------------------

	void cLowLevelSoundOpenAL::Init(int alSoundDeviceID, bool abUseEnvAudio,int alMaxChannels, 
									int alStreamUpdateFreq, bool abUseThreading, bool abUseVoiceManagement,
									int alMaxMonoSourceHint, int alMaxStereoSourceHint,
									int alStreamingBufferSize, int alStreamingBufferCount, bool abEnableLowLevelLog)
	{

		// Any need to create this??
		//cOpenALSoundEnvironment* pEnv = hplNew(cOpenALSoundEnvironment,());

		mbLogSounds = abEnableLowLevelLog;
		
		Log(" Initializing OpenAL\n");

		cOAL_Init_Params initParams;
		initParams.mbVoiceManagement = abUseVoiceManagement;
		initParams.mlNumSourcesHint = alMaxChannels;
		initParams.mlUpdateFreq = alStreamUpdateFreq;
		initParams.mlStreamingBufferCount = alStreamingBufferCount;
		initParams.mlStreamingBufferSize = alStreamingBufferSize;
		initParams.mlMinMonoSourcesHint = alMaxMonoSourceHint;
		initParams.mlMinStereoSourcesHint = alMaxStereoSourceHint;
		initParams.mbUseThread = abUseThreading;
		initParams.mlOutputFreq = 44100;
		initParams.msDeviceName = "";
		initParams.mbUseEFX = abUseEnvAudio;

		if(mbLogSounds)
			Log("  Sound logging enabled\n");

		/////////////////////////////////////////////////////////
		// List all available devices
		Log("  Available OpenAL devices:\n");
		const tSoundDeviceVec& vSndDevices = GetAvailableSoundDevices();
		if(vSndDevices.empty())
			FatalError("   No sound devices available! Check your OpenAL installation\n");

		for(int i=0;i<(int)vSndDevices.size();++i)
		{
			iSoundDeviceIdentifier* pSndDev = vSndDevices[i];
			Log("   %i. %s", pSndDev->GetID(), pSndDev->GetName().c_str());
			Log("%s\n", pSndDev->IsDefault()?"(OpenAL default)":"");
		}

		/////////////////////////////////////////////////////////
		// Now get the demanded device out of the filtered list
		const tSoundDeviceVec& vValidSndDevices = GetFilteredSoundDevices();
		if(vValidSndDevices.empty())
		{
			Warning("  No valid sound devices available! Using implementation default\n");
			alSoundDeviceID = -2;
		}

		/////////////////////////////////////////////////////////
		// Check if demanded ID is inside bounds
		if(alSoundDeviceID>=0 && alSoundDeviceID<(int)vSndDevices.size())
		{
			iSoundDeviceIdentifier* pDev = vSndDevices[alSoundDeviceID];
				
			initParams.msDeviceName = pDev->GetName();
		}
		else
		{
			/////////////////////////////////////////////////////////////
			// If not, 2 options:
			//  - ID == -1 : pick first valid default device, meaning the first default in the 
			//                  filtered list, or if no default is found, first one in line
			//  - ID == -2 : pick first default device, meaning first default in the full
			//                  available device list
			//    Any other value will force driver preferred picking
			if(alSoundDeviceID==-1)
			{
				iSoundDeviceIdentifier* pValidDefaultDev = GetFirstValidDefaultDevice();

				if(pValidDefaultDev)
				{
					alSoundDeviceID = pValidDefaultDev->GetID();
					initParams.msDeviceName = pValidDefaultDev->GetName();
				}
				else
					alSoundDeviceID = -2;
			}

			if(alSoundDeviceID==-2)
			{
				iSoundDeviceIdentifier* pDefaultDev = GetFirstDefaultDevice();

				alSoundDeviceID = pDefaultDev->GetID();
				initParams.msDeviceName = pDefaultDev->GetName();
			}
		}

		Log("  Trying to open device \'%s\'... ", initParams.msDeviceName.c_str());
		tString sLogFileName = cString::ReplaceCharTo(initParams.msDeviceName, " ", "_");
		OAL_SetupLogging(mbLogSounds, eOAL_LogOutput_File, eOAL_LogVerbose_High, "HPL_OpenAL_" + sLogFileName);

		mbInitialized = OAL_Init(initParams);

		//////////////////////////////////////////////////////////////////////
		// If the above failed, try to get first valid device that works
		if(mbInitialized==false)
		{
			Log("Failed\n  Finding first device matching filter that works... ");

			for(int i=0;i<(int)vValidSndDevices.size();++i)
			{
				iSoundDeviceIdentifier* pDev = vValidSndDevices[i];
				
				initParams.msDeviceName = pDev->GetName();

				mbInitialized = OAL_Init(initParams);
				if(mbInitialized)
				{
					alSoundDeviceID = pDev->GetID();
					Log("  Device name: %s\n", pDev->GetName().c_str() );

					break;
				}
			}
		}

		///////////////////////////////////////////////////////////////
		// If all the above fails, try default device out of all available
		if(mbInitialized==false)
		{
			Log("Failed\n  Trying with default device... ");

			for(int i=0;i<(int)vSndDevices.size();++i)
			{
				iSoundDeviceIdentifier* pDev = vSndDevices[i];
				if(pDev->IsDefault()==false)
					continue;

				initParams.msDeviceName = pDev->GetName();

				mbInitialized = OAL_Init(initParams);
				if(mbInitialized)
				{
					alSoundDeviceID = pDev->GetID();
					Log("  Device name: %s\n", pDev->GetName().c_str() );

					break;
				}
			}
		}

		/////////////////////////////////////////////////
		// If the above fails, let OpenAL choose
		if(mbInitialized==false)
		{
			Log("Failed\n  Trying to let your OpenAL driver pick the device... ");

			initParams.msDeviceName = "";
			mbInitialized = OAL_Init(initParams);
			if (mbInitialized)
			{
				tString sDeviceName = tString(OAL_Info_GetDeviceName());
				for(int i=0; i<(int)vSndDevices.size(); ++i)
				{
					/////////////////////////////////////////////////////////////////////////////////////
					// There is chance the picked device name doesn't match any of the enumerated ones
					// but should be the first that has it as substring (not 100% sure about this though)
					if(cString::GetFirstStringPos(vSndDevices[i]->GetName(), sDeviceName)!=-1) {
						alSoundDeviceID = vSndDevices[i]->GetID();
						break;
					}
				}
				Log("  Device name: %s\n", sDeviceName.c_str());
			}
		}

		//////////////////////////////////////////////////////////////////////
		// If none of the above worked, then we are pretty much screwed
		if(mbInitialized==false)
		{
			FatalError("Failed! Check your OpenAL installation\n");
			return;
		}

		mlCurrentSoundDevID = alSoundDeviceID;
		Log("Success!\n");
	
		OAL_SetDistanceModel(eOAL_DistanceModel_None);

		
		//Log("  Device name: %s\n", OAL_Info_GetDeviceName() );
		Log("  Number of mono sources: %d\n", OAL_Info_GetNumSources());
		Log("  Streaming setup: %d Buffers x %d bytes each\n", OAL_Info_GetStreamBufferCount(), OAL_Info_GetStreamBufferSize());

        //Log(" Vendor name: %s\n", pDeviceInfo->sVendorName );
		//Log(" Renderer: %s\n", pDeviceInfo->sRenderer );
		//Log("  OpenAL Version Supported: %d.%d\n", OAL_Info_GetMajorVersion(), OAL_Info_GetMinorVersion() );
		//Log("  Available extensions:\n");
		//for ( int i = 0; i < OAL_ pDeviceInfo->vsExtensions->size(); ++i )
			//Log(" \t%s\n", (*pDeviceInfo->vsExtensions)[i].c_str());

		#ifdef WIN32
		if(abUseEnvAudio)
		{
			mbEnvAudioEnabled = OAL_Info_IsEFXActive();
			mpEffect = OAL_Effect_Reverb_Create();
			if (mbEnvAudioEnabled && (mpEffect != NULL))
			{
				//SetSoundEnvironment(NULL);
				OAL_EffectSlot_AttachEffect(0, (cOAL_Effect*)mpEffect);
				OAL_EffectSlot_SetAutoAdjust(0, false);

				SetEnvVolume(1.0f);

				Log("  Setting up Environmental Audio...Success.\n");
			}
			else
				Log("  Setting up Environmental Audio...Failed.\n");
		}
		#endif
			      
				
		//Default listener settings.
		float Pos[3] = {0,0,0};
		float Vel[3] = {0,0,0};
				
		mvListenerForward = cVector3f(0,0,1);
		mvListenerUp = cVector3f(0,1,0);

		OAL_Listener_SetAttributes ( Pos, Vel, mvListenerForward.v, mvListenerUp.v );

		//Default volume:
		SetVolume(1.0f);
	}

	//-----------------------------------------------------------------------


	void cLowLevelSoundOpenAL::SetEnvVolume( float afEnvVolume )
	{
		if (!mbEnvAudioEnabled)
			return;

		if (afEnvVolume <0)
			afEnvVolume = 0;
		if (afEnvVolume >1)
			afEnvVolume = 1;
		mfEnvVolume = afEnvVolume;
		OAL_EffectSlot_SetGain(0, mfEnvVolume);
	}

	//-----------------------------------------------------------------------

	iSoundDeviceIdentifier* cLowLevelSoundOpenAL::GetCurrentSoundDevice()
	{
		if(mlCurrentSoundDevID<0 || mlCurrentSoundDevID>=(int)mvSoundDevices.size())
			return GetFirstDefaultDevice();

		return mvSoundDevices[mlCurrentSoundDevID];
	}

	//-----------------------------------------------------------------------
	
	iSoundEnvironment* cLowLevelSoundOpenAL::LoadSoundEnvironment(const tString &asFilePath)
	{
		if (!mbEnvAudioEnabled)
			return NULL;
		
		/////////////////////////////////////////////
		///Check if sound already exist
		iSoundEnvironment *pSE = GetSoundEnvironmentFromFileName(asFilePath);
		if(pSE) return pSE;

		/////////////////////////////////////////////
		///Create new and load from file
		cOpenALSoundEnvironment* pSoundEnv = hplNew(cOpenALSoundEnvironment,());

		if (pSoundEnv->CreateFromFile(asFilePath)==false)
		{
			hplDelete(pSoundEnv);
			return NULL;
		}

		pSoundEnv->SetFileName(cString::ToLowerCase(asFilePath));

		mlstSoundEnv.push_back(pSoundEnv);

		//Log(" Created %d '%s'\n",pSoundEnv, pSoundEnv->GetName().c_str());

		return pSoundEnv;

	}

	//-----------------------------------------------------------------------
	
	void cLowLevelSoundOpenAL::SetSoundEnvironment ( iSoundEnvironment* apSoundEnv )
	{
		if (!mbEnvAudioEnabled)
			return;

		if (apSoundEnv == NULL)
		{
			//Log(" Setting null env\n");
			//if (mbNullEffectAttached == false)
			//{
			//	mbNullEffectAttached = true;
		
				OAL_Effect_Reverb_SetDensity(mpEffect, 0);
				OAL_Effect_Reverb_SetDiffusion(mpEffect, 0);
				OAL_Effect_Reverb_SetEchoTime (mpEffect, 0);
				OAL_Effect_Reverb_SetEchoDepth (mpEffect,0);
				OAL_Effect_Reverb_SetModulationTime(mpEffect,0);
				OAL_Effect_Reverb_SetModulationDepth(mpEffect,0);
				OAL_Effect_Reverb_SetGain(mpEffect,0);
				OAL_Effect_Reverb_SetGainHF(mpEffect,0);
				OAL_Effect_Reverb_SetGainLF(mpEffect,0);
				OAL_Effect_Reverb_SetReflectionsDelay(mpEffect,0);
				OAL_Effect_Reverb_SetReflectionsGain(mpEffect, 0);
				OAL_Effect_Reverb_SetLateReverbGain(mpEffect, 0);
				OAL_Effect_Reverb_SetLateReverbDelay( mpEffect, 0);
				OAL_Effect_Reverb_SetHFReference(mpEffect, 0);
				OAL_Effect_Reverb_SetLFReference(mpEffect, 0);
				OAL_Effect_Reverb_SetAirAbsorptionGainHF(mpEffect, 0);
				OAL_Effect_Reverb_SetDecayTime( mpEffect,0);
				OAL_Effect_Reverb_SetDecayHFRatio (mpEffect, 0);
				OAL_Effect_Reverb_SetDecayLFRatio (mpEffect, 0);
				OAL_Effect_Reverb_SetDecayHFLimit(mpEffect, 0);
				OAL_Effect_Reverb_SetRoomRolloffFactor(mpEffect, 0);
			//}
			return;
		}
		//Log(" Setting non-null env\n");

		cOpenALSoundEnvironment* pEnv = (cOpenALSoundEnvironment*) apSoundEnv;

		OAL_Effect_Reverb_SetDensity(mpEffect, pEnv->GetDensity());
		OAL_Effect_Reverb_SetDiffusion(mpEffect, pEnv->GetDiffusion());
		OAL_Effect_Reverb_SetEchoTime (mpEffect, pEnv->GetEchoTime());
		OAL_Effect_Reverb_SetEchoDepth (mpEffect, pEnv->GetEchoDepth());
		OAL_Effect_Reverb_SetModulationTime(mpEffect,pEnv->GetModulationTime());
		OAL_Effect_Reverb_SetModulationDepth(mpEffect,pEnv->GetModulationDepth());
		OAL_Effect_Reverb_SetGain(mpEffect,pEnv->GetGain());
		OAL_Effect_Reverb_SetGainHF(mpEffect,pEnv->GetGainHF());
		OAL_Effect_Reverb_SetGainLF(mpEffect,pEnv->GetGainLF());
		OAL_Effect_Reverb_SetReflectionsDelay(mpEffect, pEnv->GetReflectionsDelay());
		OAL_Effect_Reverb_SetReflectionsGain(mpEffect, pEnv->GetReflectionsGain());
		OAL_Effect_Reverb_SetLateReverbGain(mpEffect, pEnv->GetLateReverbGain());
		OAL_Effect_Reverb_SetLateReverbDelay( mpEffect, pEnv->GetLateReverbDelay());
		OAL_Effect_Reverb_SetHFReference(mpEffect, pEnv->GetHFReference());
		OAL_Effect_Reverb_SetLFReference(mpEffect, pEnv->GetLFReference());
		OAL_Effect_Reverb_SetAirAbsorptionGainHF(mpEffect, pEnv->GetAirAbsorptionGainHF());
		OAL_Effect_Reverb_SetDecayTime( mpEffect, pEnv->GetDecayTime());
		OAL_Effect_Reverb_SetDecayHFRatio (mpEffect, pEnv->GetDecayHFRatio());
		OAL_Effect_Reverb_SetDecayLFRatio (mpEffect, pEnv->GetDecayLFRatio());
		OAL_Effect_Reverb_SetDecayHFLimit(mpEffect, pEnv->GetDecayHFLimit());
		OAL_Effect_Reverb_SetRoomRolloffFactor(mpEffect, pEnv->GetRoomRolloffFactor());

		/*Log(" %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
			mpEffect->GetDensity(),
			mpEffect->GetDiffusion(),
            mpEffect->GetEchoTime(),
			mpEffect->GetEchoDepth(),
            mpEffect->GetModulationTime(),
            mpEffect->GetModulationDepth(),
            mpEffect->GetGain(),
			mpEffect->GetGainHF(),
            mpEffect->GetGainLF(),
            mpEffect->GetReflectionsDelay(),
            mpEffect->GetReflectionsGain(),
            mpEffect->GetLateReverbGain(),
            mpEffect->GetLateReverbDelay(),
			mpEffect->GetHFReference(),
			mpEffect->GetLFReference(),
			mpEffect->GetAirAbsorptionGainHF(),
			mpEffect->GetDecayTime(),
			mpEffect->GetDecayHFRatio(),
			mpEffect->GetDecayLFRatio(),
			mpEffect->GetRoomRolloffFactor());*/

//		if (mbNullEffectAttached)
//			mbNullEffectAttached = false;

        //OAL_UpdateEffectSlots();
		OAL_EffectSlot_AttachEffect(0, (cOAL_Effect*)mpEffect);

	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOpenAL::FadeSoundEnvironment( iSoundEnvironment* apSourceSoundEnv, iSoundEnvironment* apDestSoundEnv, float afT )
	{
		if (!mbEnvAudioEnabled)
			return;

		if (afT<0)
			afT = 0;
		if (afT>1)
			afT = 1;

		float fOneMinusT = 1-afT;

		if ((apSourceSoundEnv == NULL) && (apDestSoundEnv==NULL))
			return;

		cOpenALSoundEnvironment pEnv;

		cOpenALSoundEnvironment* pSrcEnv = (cOpenALSoundEnvironment*) apSourceSoundEnv;
		cOpenALSoundEnvironment* pDstEnv = (cOpenALSoundEnvironment*) apDestSoundEnv;

		if (pSrcEnv == NULL)
		{
			/*
			SetSoundEnvironment(apDestSoundEnv);
			OAL_EffectSlot_SetGain(0, afT * mfEnvVolume);
			return;
			*/
			pEnv.SetDensity(	pDstEnv->GetDensity()*afT	);
			pEnv.SetDiffusion(	pDstEnv->GetDiffusion()*afT	);
			pEnv.SetGain(		pDstEnv->GetGain()*afT);
			pEnv.SetGainHF(		pDstEnv->GetGainHF()*afT);
			pEnv.SetGainLF(		pDstEnv->GetGainLF()*afT);
			pEnv.SetDecayTime(	pDstEnv->GetDecayTime()*afT);
			pEnv.SetDecayLFRatio(	pDstEnv->GetDecayLFRatio()*afT);
			pEnv.SetReflectionsGain(	pDstEnv->GetReflectionsGain()*afT);
			pEnv.SetReflectionsDelay(	pDstEnv->GetReflectionsDelay()*afT);
			pEnv.SetLateReverbGain(pDstEnv->GetLateReverbGain()*afT);
			pEnv.SetLateReverbDelay(pDstEnv->GetLateReverbDelay()*afT);
			pEnv.SetEchoTime(pDstEnv->GetEchoTime()*afT);
			pEnv.SetEchoDepth(pDstEnv->GetEchoDepth()*afT);
			pEnv.SetModulationTime(pDstEnv->GetModulationTime()*afT);
			pEnv.SetModulationDepth(pDstEnv->GetModulationDepth()*afT);
			pEnv.SetAirAbsorptionGainHF(pDstEnv->GetAirAbsorptionGainHF()*afT);
			pEnv.SetHFReference(pDstEnv->GetHFReference()*afT);
			pEnv.SetLFReference(pDstEnv->GetLFReference()*afT);
			pEnv.SetRoomRolloffFactor(pDstEnv->GetRoomRolloffFactor()*afT);
			pEnv.SetDecayHFLimit(pDstEnv->GetDecayHFLimit());

		}
		else if (pDstEnv == NULL)
		{
			/*SetSoundEnvironment( apDestSoundEnv);//((*apSourceSoundEnv)*afT));
			OAL_EffectSlot_SetGain(0, fOneMinusT * mfEnvVolume);
			return;*/
			pEnv.SetDensity(	pSrcEnv->GetDensity()*fOneMinusT	);
			pEnv.SetDiffusion(	pSrcEnv->GetDiffusion()*fOneMinusT		);
			pEnv.SetGain(		pSrcEnv->GetGain()*fOneMinusT);
			pEnv.SetGainHF(		pSrcEnv->GetGainHF()*fOneMinusT);
			pEnv.SetGainLF(		pSrcEnv->GetGainLF()*fOneMinusT	);
			pEnv.SetDecayTime(	pSrcEnv->GetDecayTime()*fOneMinusT);
			pEnv.SetDecayLFRatio(pSrcEnv->GetDecayLFRatio()*fOneMinusT );
			pEnv.SetReflectionsGain(pSrcEnv->GetReflectionsGain()*fOneMinusT);
			pEnv.SetReflectionsDelay(pSrcEnv->GetReflectionsDelay()*fOneMinusT);
			pEnv.SetLateReverbGain(pSrcEnv->GetLateReverbGain()*fOneMinusT);
			pEnv.SetLateReverbDelay(pSrcEnv->GetLateReverbDelay()*fOneMinusT);
			pEnv.SetEchoTime(pSrcEnv->GetEchoTime()*fOneMinusT);
			pEnv.SetEchoDepth(pSrcEnv->GetEchoDepth()*fOneMinusT );
			pEnv.SetModulationTime(pSrcEnv->GetModulationTime()*fOneMinusT);
			pEnv.SetModulationDepth(pSrcEnv->GetModulationDepth()*fOneMinusT );
			pEnv.SetAirAbsorptionGainHF(pSrcEnv->GetAirAbsorptionGainHF()*fOneMinusT);
			pEnv.SetHFReference(pSrcEnv->GetHFReference()*fOneMinusT);
			pEnv.SetLFReference(pSrcEnv->GetLFReference()*fOneMinusT);
			pEnv.SetRoomRolloffFactor(pSrcEnv->GetRoomRolloffFactor()*fOneMinusT);
			pEnv.SetDecayHFLimit(pSrcEnv->GetDecayHFLimit());

		}
		else
		{
			pEnv.SetDensity(	pSrcEnv->GetDensity()*fOneMinusT	+	pDstEnv->GetDensity()*afT	);
			pEnv.SetDiffusion(	pSrcEnv->GetDiffusion()*fOneMinusT	+	pDstEnv->GetDiffusion()*afT	);
			pEnv.SetGain(		pSrcEnv->GetGain()*fOneMinusT		+	pDstEnv->GetGain()*afT);
			pEnv.SetGainHF(		pSrcEnv->GetGainHF()*fOneMinusT		+	 pDstEnv->GetGainHF()*afT);
			pEnv.SetGainLF(		pSrcEnv->GetGainLF()*fOneMinusT		+	 pDstEnv->GetGainLF()*afT);
			pEnv.SetDecayTime(	pSrcEnv->GetDecayTime()*fOneMinusT + pDstEnv->GetDecayTime()*afT);
			pEnv.SetDecayLFRatio(pSrcEnv->GetDecayLFRatio()*fOneMinusT + pDstEnv->GetDecayLFRatio()*afT);
			pEnv.SetReflectionsGain(pSrcEnv->GetReflectionsGain()*fOneMinusT + pDstEnv->GetReflectionsGain()*afT);
			pEnv.SetReflectionsDelay(pSrcEnv->GetReflectionsDelay()*fOneMinusT + pDstEnv->GetReflectionsDelay()*afT);
			pEnv.SetLateReverbGain(pSrcEnv->GetLateReverbGain()*fOneMinusT + pDstEnv->GetLateReverbGain()*afT);
			pEnv.SetLateReverbDelay(pSrcEnv->GetLateReverbDelay()*fOneMinusT + pDstEnv->GetLateReverbDelay()*afT);
			pEnv.SetEchoTime(pSrcEnv->GetEchoTime()*fOneMinusT + pDstEnv->GetEchoTime()*afT);
			pEnv.SetEchoDepth(pSrcEnv->GetEchoDepth()*fOneMinusT + pDstEnv->GetEchoDepth()*afT);
			pEnv.SetModulationTime(pSrcEnv->GetModulationTime()*fOneMinusT + pDstEnv->GetModulationTime()*afT);
			pEnv.SetModulationDepth(pSrcEnv->GetModulationDepth()*fOneMinusT + pDstEnv->GetModulationDepth()*afT);
			pEnv.SetAirAbsorptionGainHF(pSrcEnv->GetAirAbsorptionGainHF()*fOneMinusT + pDstEnv->GetAirAbsorptionGainHF()*afT);
			pEnv.SetHFReference(pSrcEnv->GetHFReference()*fOneMinusT + pDstEnv->GetHFReference()*afT);
			pEnv.SetLFReference(pSrcEnv->GetLFReference()*fOneMinusT + pDstEnv->GetLFReference()*afT);
			pEnv.SetRoomRolloffFactor(pSrcEnv->GetRoomRolloffFactor()*fOneMinusT + pDstEnv->GetRoomRolloffFactor()*afT);
			pEnv.SetDecayHFLimit(pDstEnv->GetDecayHFLimit());
		}
		
		SetSoundEnvironment(&pEnv);

	}

	//-----------------------------------------------------------------------

	///////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	///////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iSoundDeviceIdentifier* cLowLevelSoundOpenAL::GetFirstValidDefaultDevice()
	{
		const tSoundDeviceVec& vValidSndDevices = GetFilteredSoundDevices();

		/////////////////////////////////
		// Return first default device
		for(int i=0; i<(int)vValidSndDevices.size(); ++i)
		{
			iSoundDeviceIdentifier* pSndDev = vValidSndDevices[i];
			if(pSndDev->IsDefault())
				return pSndDev;
		}
		/////////////////////////////////
		// If no default device found in valid list, return 1st
		if(vValidSndDevices.empty()==false)
			return vValidSndDevices[0];

		return NULL;
	}


	//-----------------------------------------------------------------------

	iSoundDeviceIdentifier* cLowLevelSoundOpenAL::GetFirstDefaultDevice()
	{
		iSoundDeviceIdentifier* pSndDev = GetFirstValidDefaultDevice();

		if(pSndDev)
			return pSndDev;

		///////////////////////////////////////////////////////////////
		// This will be run if there are no filtered sound devices at all
		const tSoundDeviceVec& vSndDevices = GetAvailableSoundDevices();

		/////////////////////////////////
		// Return first default device in all devices
		for(int i=0; i<(int)vSndDevices.size(); ++i)
		{
			iSoundDeviceIdentifier* pSndDev = vSndDevices[i];
			if(pSndDev->IsDefault())
				return pSndDev;
		}

		////////////////////////////////
		// Unlikely to reach, but for completeness
		return vSndDevices[0];
	}

}
