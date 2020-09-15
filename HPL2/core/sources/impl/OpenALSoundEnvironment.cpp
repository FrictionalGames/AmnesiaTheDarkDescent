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

#include "impl/OpenALSoundEnvironment.h"
#include "impl/tinyXML/tinyxml.h"

namespace hpl
{

	cOpenALSoundEnvironment::cOpenALSoundEnvironment ( ) /*:	
										mfDensity(1.0f), 
										mfDiffusion(1.0f),
										mfGain(0.32f),
										mfGainHF(0.89f),
										mfGainLF(0.0f),
										mfDecayTime(1.49f),
										mfDecayHFRatio(0.83f),
										mfDecayLFRatio(1.0f),
										mfReflectionsGain(0.05f),
										mfReflectionsDelay(0.007f),
										mfLateReverbGain(1.25f),
										mfLateReverbDelay(0.011f),
										mfEchoTime(0.25f),
										mfEchoDepth(0.0f),
										mfModulationTime(0.25f),
										mfModulationDepth(0.0f),
										mfAirAbsorptionGainHF(0.994f),
										mfHFReference(5000.0f),
										mfLFReference(250.0f),
										mfRoomRolloffFactor(0.0f),
										mbDecayHFLimit(true)*/
	{
	}

	cOpenALSoundEnvironment::~cOpenALSoundEnvironment()
	{
	}
		
	bool cOpenALSoundEnvironment::CreateFromFile(const tString &asFile)
	{
		tString strType;
		TiXmlDocument doc;
		if (!doc.LoadFile(asFile.c_str()))
			return false;

		TiXmlElement* pMain = doc.FirstChildElement("SoundEnvironment")->FirstChildElement("Main");
		if (pMain)
		{
			strType = pMain->Attribute("Type");
			mstrName = pMain->Attribute("Name");
		}


		float* pfTemp;

		TiXmlElement* pParams = doc.FirstChildElement("SoundEnvironment")->FirstChildElement("Parameters");

		if ( (pParams == NULL) || (strType.compare("OpenAL")!=0) )
		{
			doc.Clear();
			return false;
		}

        mfDensity = cString::ToFloat(pParams->Attribute("Density"),0);
		mfDiffusion = cString::ToFloat(pParams->Attribute("Diffusion"),0);
		mfGain = cString::ToFloat(pParams->Attribute("Gain"),0);
		mfGainHF = cString::ToFloat(pParams->Attribute("GainHF"),0);
		mfGainLF = cString::ToFloat(pParams->Attribute("GainLF"),0); 
		mfDecayTime = cString::ToFloat(pParams->Attribute("DecayTime"),0);
		mfDecayHFRatio = cString::ToFloat (pParams->Attribute("DecayHFRatio"),0);
		mfDecayLFRatio = cString::ToFloat (pParams->Attribute("DecayLFRatio"),0);
		mfReflectionsGain = cString::ToFloat(pParams->Attribute("ReflectionsGain"),0);
		mfReflectionsDelay = cString::ToFloat(pParams->Attribute("ReflectionsDelay"),0);
		pfTemp = cString::ToVector3f(pParams->Attribute("ReflectionsPan"),cVector3f(0)).v;
		mfReflectionsPan[0] = pfTemp[0];
		mfReflectionsPan[1] = pfTemp[1];
		mfReflectionsPan[2] = pfTemp[2];
		mfLateReverbGain = cString::ToFloat(pParams->Attribute("LateReverbGain"),0);
		mfLateReverbDelay = cString::ToFloat(pParams->Attribute("LateReverbDelay"),0);
		pfTemp = cString::ToVector3f(pParams->Attribute("LateReverbPan"),cVector3f(0)).v;
		mfLateReverbPan[0] = pfTemp[0];
		mfLateReverbPan[1] = pfTemp[1];
		mfLateReverbPan[2] = pfTemp[2];
		mfEchoTime = cString::ToFloat(pParams->Attribute("EchoTime"),0);
		mfEchoDepth = cString::ToFloat(pParams->Attribute("EchoDepth"),0);
		mfModulationTime = cString::ToFloat(pParams->Attribute("ModulationTime"),0);
		mfModulationDepth = cString::ToFloat(pParams->Attribute("ModulationDepth"),0);
		mfAirAbsorptionGainHF = cString::ToFloat(pParams->Attribute("AirAbsorptionGainHF"),0);
		mfHFReference = cString::ToFloat(pParams->Attribute("HFReference"),0);
		mfLFReference = cString::ToFloat(pParams->Attribute("LFReference"),0);
		mfRoomRolloffFactor =cString::ToFloat(pParams->Attribute("RoomRolloffFactor"),0);
		mbDecayHFLimit = cString::ToInt(pParams->Attribute("DecayHFLimit"),0);
		
		doc.Clear();
		pParams = NULL;

		return true;
	}

};
