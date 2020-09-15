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

#include "graphics/AnimationTrack.h"

#include "math/Math.h"
#include "graphics/Animation.h"
#include "system/LowLevelSystem.h"
#include "scene/Node3D.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cAnimationTrack::cAnimationTrack(const tString &asName, tAnimTransformFlag aTransformFlags, 
									cAnimation *apParent)
	{
		msName = asName;
		mTransformFlags = aTransformFlags;
		mpParent = apParent;

		mfMaxFrameTime = 0;

		mlNodeIdx = -1;
	}

	//-----------------------------------------------------------------------

	cAnimationTrack::~cAnimationTrack()
	{
		STLDeleteAll(mvKeyFrames);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cAnimationTrack::ResizeKeyFrames(int alSize)
	{
		mvKeyFrames.reserve(alSize);
	}
	
	//-----------------------------------------------------------------------

	cKeyFrame* cAnimationTrack::CreateKeyFrame(float afTime)
	{
		cKeyFrame* pFrame = hplNew( cKeyFrame,());
		pFrame->time = afTime;

		//Check so that this is the first
        if(afTime > mfMaxFrameTime || mvKeyFrames.empty())
		{
			mvKeyFrames.push_back(pFrame);
			mfMaxFrameTime = afTime;
		}
		else
		{
			tKeyFramePtrVecIt it = mvKeyFrames.begin();
			for(; it != mvKeyFrames.end(); it++)
			{
				if(afTime < (*it)->time)
				{
					break;
				}
			}
			mvKeyFrames.insert(it,pFrame);
		}
		
        return pFrame;
	}

	//-----------------------------------------------------------------------

	void cAnimationTrack::ClearKeyFrames()
	{
		STLDeleteAll(mvKeyFrames);
		mvKeyFrames.clear();
	}

	//-----------------------------------------------------------------------

	void cAnimationTrack::ApplyToNode(cNode3D* apNode, float afTime, float afWeight, bool bLoop)
	{
		if(mvKeyFrames.empty()) return;

		cKeyFrame Frame = GetInterpolatedKeyFrame(afTime);
        		
		//Scale
		//Skip this for now...
		/*cVector3f vOne(1,1,1);
		cVector3f vScale = (Frame.scale - vOne)*afWeight + vOne;
		apNode->AddScale(vScale);*/

		//Rotation
		cQuaternion qRot = cMath::QuaternionSlerp(afWeight, cQuaternion::Identity, Frame.rotation, true);
		apNode->AddRotation(qRot);
		
		//Translation
		cVector3f vTrans = Frame.trans * afWeight;
		apNode->AddTranslation(vTrans);
	}

	//-----------------------------------------------------------------------

	cKeyFrame cAnimationTrack::GetInterpolatedKeyFrame(float afTime, bool bLoop)
	{
		cKeyFrame ResultKeyFrame;
		ResultKeyFrame.time = afTime;

		if(mvKeyFrames.empty())
		{
			ResultKeyFrame.rotation = cQuaternion(1,0,0,0);
			ResultKeyFrame.trans = 0;
			return ResultKeyFrame;
		}

		cKeyFrame *pKeyFrameA = NULL;
		cKeyFrame *pKeyFrameB = NULL;
		
		float fT = GetKeyFramesAtTime(afTime, &pKeyFrameA, &pKeyFrameB, bLoop);

		
        if(fT == 0.0f)
		{
			ResultKeyFrame.rotation = pKeyFrameA->rotation;
			ResultKeyFrame.trans = pKeyFrameA->trans;
		}
		else
		{	
			//Do a linear interpolation
			//This should include spline stuff later on.

            
			cQuaternion rotA = pKeyFrameA->rotation;
			cQuaternion rotB = pKeyFrameB->rotation;
            /*
            if ( rotA.v.x == -1 * rotB.v.x
                && rotA.v.y == -1 * rotB.v.y 
                && rotA.v.z == -1 * rotB.v.z
                && rotA.w == -1 * rotB.w )
            {
                ResultKeyFrame.rotation = rotB;
            }
            else
            {
                ResultKeyFrame.rotation = cMath::QuaternionSlerp(fT, rotA, 
													rotB, true);
            }*/

            rotA.Normalize();
            rotB.Normalize();

			if ( rotA.w < 0 )
            {
                rotA = rotA * -1.0f;
            }
			
            if ( rotB.w < 0 )
            {
                rotB = rotB * -1.0f;
            }


            ResultKeyFrame.rotation = cMath::QuaternionSlerp(fT, rotA, 
													rotB, true);

            /*

            ResultKeyFrame.rotation = cMath::QuaternionSlerp(fT, pKeyFrameA->rotation, 
													pKeyFrameB->rotation, true);
                                                    */
			ResultKeyFrame.trans = pKeyFrameA->trans * (1 - fT) + pKeyFrameB->trans * fT;
		}

		return ResultKeyFrame;
	}

	//-----------------------------------------------------------------------
	
	float cAnimationTrack::GetKeyFramesAtTime(float afTime, cKeyFrame** apKeyFrameA,cKeyFrame** apKeyFrameB, bool bLoop)
	{
		float fTotalAnimLength = mpParent->GetLength();

		// Wrap time 
		//Not sure it is a good idea to clamp the length.
		//But wrapping screws loop mode up. 
		//Wrap(..., totalLength + kEpislon), migh work though.
		afTime = cMath::Clamp(afTime, 0, fTotalAnimLength);

		//If longer than max time return last frame and first
		if(afTime >= mfMaxFrameTime)
		{
			*apKeyFrameA = mvKeyFrames[mvKeyFrames.size()-1];
			*apKeyFrameB = mvKeyFrames[0];
			
			//Get T between end to start again. (the last frame doesn't mean the anim is over. 
			// In that case wrap to the first frame).
			float fDeltaT = fTotalAnimLength - (*apKeyFrameA)->time;
			
			//If animation time is >= max time might as well just return the last frame.
			//Not sure if this is good for some looping anims, in that case check the code.
			return 0.0f;//(afTime - (*apKeyFrameA)->time) / fDeltaT;
		}

		//Get the number of frames
		const int lSize = (int)mvKeyFrames.size();

		//Find the second frame.
		int lIdxB=-1;
		for(int i=0; i< lSize; i++)
		{
			if(afTime <= mvKeyFrames[i]->time)
			{
				lIdxB = i;
				break;
			}
		}
		
		//If first frame was found, the lowest time is not 0. 
		//If so return the first frame only.
		if(lIdxB == 0)
		{
			*apKeyFrameA = mvKeyFrames[0];
			*apKeyFrameB = mvKeyFrames[0];
			return 0.0f;
		}
		
		//Get the frames
		*apKeyFrameA = mvKeyFrames[lIdxB-1];
		*apKeyFrameB = mvKeyFrames[lIdxB];
        
		float fDeltaT = (*apKeyFrameB)->time - (*apKeyFrameA)->time;
        
		return (afTime - (*apKeyFrameA)->time) / fDeltaT;
	}

	//-----------------------------------------------------------------------



	void cAnimationTrack::Smooth(float afAmount,float afPow,  int alSamples, bool abTranslation, bool abRotation)
	{
		/*/////////////////////////////////
		// Create temp key frames
		tKeyFramePtrVec vTempKeyFrames;
		vTempKeyFrames.resize(mvKeyFrames.size());
		for(size_t i=0; i<vTempKeyFrames.size(); ++i)
		{
			cKeyFrame* pFrame = hplNew( cKeyFrame,());
			*pFrame = *mvKeyFrames[i];
            vTempKeyFrames[i] = pFrame;
		}

		/////////////////////////////////
		// Go through all keyframes and calculate a smooth version
		// TODO: Add translation and also be able to turn rot / trans off
		for(size_t i=0; i<mvKeyFrames.size(); ++i)
		{
			cKeyFrame* pFrame = vTempKeyFrames[i];
			Log(" KeyFrame %d\n", i);
			
			float fTime = pFrame->time;
            float fMulSum =0;
			cVector3f vRotSum(0);
            			
			for(int sample=0; sample<alSamples;++sample)
			{
				float fT = ( ((float)sample) / ((float)alSamples));
				float fMul = powf(1.0f - fT, afPow);

				Log("   Sample %d t: %f mul: %f\n", sample, fT, fMul);
				
				float fTimeAdd = fT * afAmount;
				
				///////////////////
				//Rotation
				if(abRotation)
				{
					if(fTime + fTimeAdd <= mpParent->GetLength())
					{
						vRotSum = vRotSum + GetBaseAnglesAtTime(fTime + fTimeAdd) * fMul;
						fMulSum += fMul;
						Log("    Rot: %s\n", cMath::Vector3ToDeg(GetBaseAnglesAtTime(fTime + fTimeAdd)).ToString().c_str());
					}
					
					if(sample != 0 && fTime - fTimeAdd >= 0)
					{
						vRotSum = vRotSum + GetBaseAnglesAtTime(fTime - fTimeAdd) * fMul;
						fMulSum += fMul;
						Log("    Rot: %s\n", cMath::Vector3ToDeg(GetBaseAnglesAtTime(fTime - fTimeAdd)).ToString().c_str());
					}
				}
			}
			
			if(abRotation)
			{
				cVector3f vRot = vRotSum *(1.0f / fMulSum);
				Log("  Result: %s\n", cMath::Vector3ToDeg(vRot).ToString().c_str());
				cMatrixf mtxRot = cMath::MatrixRotate(vRot, eEulerRotationOrder_XYZ);
				cQuaternion qRot;
				qRot.FromRotationMatrix(mtxRot);

				pFrame->rotation = qRot;
			}
		}

		//////////////////////////////
		// Set temp data to normal
		for(size_t i=0; i<vTempKeyFrames.size(); ++i)
		{
			cKeyFrame* pTempFrame = vTempKeyFrames[i];
			cKeyFrame* pFrame = mvKeyFrames[i];
			*pFrame = *pTempFrame;
		}
		

		/////////////////////////////////
		// Delete temp key frames
		STLDeleteAll(vTempKeyFrames);*/
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
}