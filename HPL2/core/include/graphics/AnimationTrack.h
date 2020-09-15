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

#ifndef HPL_ANIMATION_TRACK_H
#define HPL_ANIMATION_TRACK_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"

namespace hpl {

	class cAnimation;
	class cNode3D;

	class cAnimationTrack
	{
	public:
		cAnimationTrack(const tString &asName, tAnimTransformFlag aTransformFlags, cAnimation *apParent);
		~cAnimationTrack();

		void ResizeKeyFrames(int alSize);

		/**
		 * Creates a new key frame. These should be added in sequential order.
		 * \param afTime the time for the key frame.
		 */
		cKeyFrame* CreateKeyFrame(float afTime);
		void ClearKeyFrames();
		
		inline cKeyFrame* GetKeyFrame(int alIndex){ return mvKeyFrames[alIndex];}
		inline int GetKeyFrameNum(){ return (int) mvKeyFrames.size();}

		inline tAnimTransformFlag GetTransformFlags(){ return mTransformFlags;}
		
		/**
		 * Apply the animation to a node. The method uses Node->AddXXX() so Update matrix must be called
		 * for the transformation to be applied.
		 * \param apNode The node with it's base pose
		 * \param afTime The time at which to apply the animation
		 * \param afWeight The weight of the animation, a value from 0 to 1.
		 */
		void ApplyToNode(cNode3D* apNode, float afTime, float afWeight,bool bLoop=true);

		/**
		 * Get a KeyFrame that contains an interpolated value.
		 * \param afTime The time from which to create the key frame.
		 */
		cKeyFrame GetInterpolatedKeyFrame(float afTime,bool bLoop=true);

        /**
         * Gets key frames between for a specific time.
         * \param afTime The time
         * \param &apKeyFrameA The frame that is equal to or before time
         * \param &apKeyFrameB The frame that is after time. 
         * \return Weight of the different frames. 0 = 100% A, 1 = 100% B 0.5 = 50% A and 50% B
         */
        float GetKeyFramesAtTime(float afTime, cKeyFrame** apKeyFrameA,cKeyFrame** apKeyFrameB,bool bLoop=true);

		void Smooth(float afAmount, float afPow, int alSamples,bool abTranslation, bool abRotation);
		
		const tString& GetName(){ return msName;}
		
		void SetNodeIndex(int alIndex){ mlNodeIdx = alIndex;}
		int GetNodeIndex(){ return mlNodeIdx;}

	private:
		tString msName;

		int mlNodeIdx;

		tKeyFramePtrVec mvKeyFrames;
		tAnimTransformFlag mTransformFlags;

		float mfMaxFrameTime;
		
        cAnimation* mpParent;
	};

};
#endif // HPL_ANIMATION_TRACK_H
