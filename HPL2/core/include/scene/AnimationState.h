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

#ifndef HPL_ANIMATION_STATE_H
#define HPL_ANIMATION_STATE_H

#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"

#include "engine/SaveGame.h"

namespace hpl {

	class cAnimation;
	class cAnimationManager;

	//---------------------------------------------
	
	class cAnimationEvent
	{
	public:
		float mfTime;
		eAnimationEventType mType;
		tString msValue;
	};

	//---------------------------------------------

	class cAnimationState
	{
	public:
		cAnimationState(cAnimation* apAnimation,const tString &asName,
						cAnimationManager *apAnimationManager);
		~cAnimationState();

        const tString& GetName(){ return msName;}

		void Update(float afTimeStep);

		bool DataIsInMeshFile(){return mpAnimationManager==NULL;}

		bool IsFading();

		/**
		 * If the animation has reached the end.
		 */
		bool IsOver();

		void FadeIn(float afTime);
		void FadeOut(float afTime);

		void SetLength(float afLength);
		float GetLength();

		void SetWeight(float afWeight);
		float GetWeight();

        void SetSpeed(float afSpeed);
		float GetSpeed();

		void SetBaseSpeed(float afSpeed);
		float GetBaseSpeed();

		void SetTimePosition(float afPosition);
        float GetTimePosition();
		float GetPreviousTimePosition();

		/**
		 * Set the relative postion. 0 = start, 1 = end
		 * \param afPosition
		 */
		void SetRelativeTimePosition(float afPosition);

		/**
		* Get the relative postion. 0 = start, 1 = end
		*/
		float GetRelativeTimePosition();

		bool IsActive();
		void SetActive(bool abActive);

		bool IsLooping();
		void SetLoop(bool abLoop);

		bool IsPaused();
		void SetPaused(bool abPaused);

		void SetSpecialEventTime(float afT){ mfSpecialEventTime = afT;}
		float GetSpecialEventTime(){ return mfSpecialEventTime;}
		bool IsAfterSpecialEvent();
		bool IsBeforeSpecialEvent();

		void AddTimePosition(float afAdd);

		cAnimation* GetAnimation();

		cAnimationEvent *CreateEvent();
		cAnimationEvent *GetEvent(int alIdx);
		int GetEventNum();

		float GetFadeStep(){ return mfFadeStep;}
		void SetFadeStep(float afX){ mfFadeStep = afX;}
	
	private:
		tString msName;

		cAnimationManager *mpAnimationManager;

		cAnimation* mpAnimation;

		std::vector<cAnimationEvent*> mvEvents;

		//Properties of the animation
		float mfLength;
		float mfWeight;
		float mfSpeed;
		float mfTimePos;
		float mfPrevTimePos;

		float mfBaseSpeed;

		float mfSpecialEventTime;

		bool mbActive;
		bool mbLoop;
		bool mbPaused;

		//properties for update
		float mfFadeStep;
	};

};
#endif // HPL_ANIMATION_STATE_H
