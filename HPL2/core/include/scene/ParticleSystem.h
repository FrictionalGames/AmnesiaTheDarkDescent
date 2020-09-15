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

#ifndef HPL_PARTICLE_SYSTEM_H
#define HPL_PARTICLE_SYSTEM_H

#include "system/SystemTypes.h"

#include "scene/Entity3D.h"
#include "resources/ResourceBase.h"


namespace hpl {

	//----------------------------------------------------

	class cGraphics;
	class cResources;
	class iParticleEmitter;
	class iParticleEmitterData;
	class cParticleManager;
	class cXmlElement;

	//----------------------------------------------------

	class cParticleSystem;

	class cParticleSystemData : public iResourceBase
	{
	public:
		cParticleSystemData(const tString &asName,cResources* apResources,cGraphics *apGraphics);
		virtual ~cParticleSystemData();

		cParticleSystem* Create(tString asName, cVector3f avSize);

		bool LoadFromFile(const tWString &asFile);
		bool LoadFromElement(cXmlElement* apElement);

		void AddEmitterData(iParticleEmitterData *apData);

		bool Reload(){ return false;}
		void Unload(){}
		void Destroy(){}

		iParticleEmitterData* GetEmitterData(int alIdx) const { return mvEmitterData[alIdx]; }

	private:
		cResources* mpResources;
		cGraphics *mpGraphics;

		std::vector<iParticleEmitterData*> mvEmitterData;
	};

	//----------------------------------------------------


	class cParticleSystem : public iEntity3D
	{
	#ifdef __GNUC__
		typedef iEntity3D __super;
	#endif
	friend class iParticleEmitter;
	public:
		cParticleSystem(	const tString asName,cParticleSystemData *apData, 
							cResources *apResources, cGraphics *apGraphics);
		~cParticleSystem();

		bool IsVisible(){ return mbIsVisible;}
		void SetVisible(bool abVisible);

		void UpdateLogic(float afTimeStep);

		bool IsDead();
		bool IsDying();
		bool GetRemoveWhenDead() { return mbRemoveWhenDead; }
		void SetRemoveWhenDead(bool abX) { mbRemoveWhenDead = abX; }

		void Kill();
		void KillInstantly();

		void AddEmitter(iParticleEmitter* apEmitter);
		iParticleEmitter* GetEmitter(int alIdx);
		int GetEmitterNum();

		tString GetEntityType(){ return "ParticleSystem3D"; }

		void SetDataName(const tString &asName) {msDataName = asName;}
		void SetDataSize(const cVector3f &avSize) {mvDataSize = avSize;}

        const tString& GetDataName() { return msDataName;}
		const cVector3f& GetDataSize() { return mvDataSize;}

		void SetColor(const cColor& aColor){ mColor = aColor;}
		void SetFadeAtDistance(bool abX){ mbFadeAtDistance = abX;}
		void SetMinFadeDistanceStart(float afX){ mfMinFadeDistanceStart = afX;}
		void SetMinFadeDistanceEnd(float afX){ mfMinFadeDistanceEnd = afX;}
		void SetMaxFadeDistanceStart(float afX){ mfMaxFadeDistanceStart = afX;}
		void SetMaxFadeDistanceEnd(float afX){ mfMaxFadeDistanceEnd = afX;}

		cColor GetColor(){ return mColor;}
		bool GetFadeAtDistance(){ return mbFadeAtDistance;}
		float GetMinFadeDistanceStart(){ return mfMinFadeDistanceStart;}
		float GetMinFadeDistanceEnd(){ return mfMinFadeDistanceEnd;}
		float GetMaxFadeDistanceStart(){ return mfMaxFadeDistanceStart;}
		float GetMaxFadeDistanceEnd(){ return mfMaxFadeDistanceEnd;}
		
		void SetParticleManager(cParticleManager *apParticleManager)
		{
			mpParticleManager = apParticleManager;
		}

		cParticleSystemData *GetData(){ return mpData;}

	private:
		cResources *mpResources;
		cGraphics *mpGraphics;
		cParticleManager *mpParticleManager;
		cParticleSystemData *mpData;

		std::vector<iParticleEmitter*> mvEmitters;

		bool mbRemoveWhenDead;

		bool mbIsVisible;

		cColor mColor;
		bool mbFadeAtDistance;
		float mfMinFadeDistanceStart;
		float mfMinFadeDistanceEnd;
		float mfMaxFadeDistanceStart;
		float mfMaxFadeDistanceEnd;

		tString msDataName;
		cVector3f mvDataSize;

		bool mbFirstUpdate;
	};

};
#endif // HPL_PARTICLE_SYSTEM_H
