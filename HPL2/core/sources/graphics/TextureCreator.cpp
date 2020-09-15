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

#include "graphics/TextureCreator.h"

#include "system/LowLevelSystem.h"

#include "graphics/LowLevelGraphics.h"
#include "graphics/Texture.h"

#include "resources/Resources.h"

#include "math/Math.h"

#include <algorithm>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cTextureCreator::cTextureCreator(iLowLevelGraphics *apLowLevelGraphics, cResources *apResources)
	{
		mpLowLevelGraphics = apLowLevelGraphics;
		mpResources = apResources;
	}

	//-----------------------------------------------------------------------

	cTextureCreator::~cTextureCreator()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	static bool SortFunc_ScatterOffset(const cVector2f& avVec1, const cVector2f& avVec2)
	{
		return avVec1.Length() > avVec2.Length(); 
	}	

	void cTextureCreator::GenerateScatterDiskMap3D(iTexture *apOffsetTexture, int alSize, int alSamples, bool abSortSamples)
	{
		////////////////////
		//Test so sizes are correct
		if(cMath::IsPow2(alSize)==false) FatalError("OffsetTexture 2D size size non-pow2\n");
		if(cMath::IsPow2(alSamples)==false) FatalError("OffsetTexture 2D samples size non-pow2\n");

		////////////////////
		// Calculate size of grid
		int lGridSize = (int)(sqrt((float)alSamples)+0.5f);
		std::vector<tVector2fVec> vOffsetsVec;
		vOffsetsVec.resize(alSize*alSize);
		tVector2fVec vTempGridArray;
		vTempGridArray.resize(lGridSize * lGridSize);

		////////////////////////////
		//Setup texture data
		std::vector<unsigned char> vTextureData;
		cVector3l vTextureSize(alSize, alSize, alSamples/2);
		vTextureData.resize(4*vTextureSize.x*vTextureSize.y*vTextureSize.z,0);

		//Log("GRIDSIZE: %d\n",lGridSize);

		////////////////////////////
		//Generate all the different sample collections
		for(size_t sample_num=0; sample_num<vOffsetsVec.size(); ++sample_num)				
		{
			////////////////////////////
			// Fills a square "grid" size, and then normalizes each pos to be 0 - 1
			for(int y=0;y<lGridSize; ++y)
				for(int x=0;x<lGridSize; ++x)
				{
					//+0.5, because we want center
					cVector2f vPos((float)x + 0.5f, (float) y+0.5f);

					//Randomize point from, but let it never leave the grid square.
					vPos += cMath::RandRectVector2f(-0.5f, 0.5f);

					//Normalize position
					vPos *= 1.0f / (float)lGridSize;

					//Add to array
					vTempGridArray[y*lGridSize + x] = vPos;
				}

				////////////////////////////
				//Debug:
				/*Log("---- Grid%d ------\n",sample_num);
				for(int y=0;y<lGridSize; ++y)
				for(int x=0;x<lGridSize; ++x)
				{
				cVector2f vOffset = vTempGridArray[y*lGridSize + x];
				Log(" (%f:%f)",vOffset.x,vOffset.y);
				if(x==lGridSize-1)Log("\n");
				}
				Log(" --\n");*/

				//Get the offsets and put the once we want from the grid here.		
				tVector2fVec& vOffsets = vOffsetsVec[sample_num];
				vOffsets.resize(alSamples);

				float fSampleAdd = ((float)vTempGridArray.size()) / ((float)alSamples);
				float fCurrentSample =0;

				for(size_t i=0; i<vOffsets.size(); ++i)
				{
					int lIdx = (int)fCurrentSample;	

					vOffsets[i] = vTempGridArray[lIdx];

					fCurrentSample+= fSampleAdd;
				}


				////////////////////////////
				// Warp grid so coordinates become spherical
				for(size_t i=0; i<vOffsets.size(); ++i)
				{
					cVector2f vPos = vOffsets[i];

					vOffsets[i].x = sqrtf(vPos.y) * cos(k2Pif * vPos.x);
					vOffsets[i].y = sqrtf(vPos.y) * sin(k2Pif * vPos.x);
				}

				///////////////////////////////
				// Sort by length (if set)
				if(abSortSamples)
				{
					std::sort(vOffsets.begin(),vOffsets.end(), SortFunc_ScatterOffset);
				}

				//Debug:
				/*cVector2f vSum(0);
				for(size_t i=0; i<vOffsets.size(); ++i)
				{
				int lX =  (int)((vOffsets[i].x+1)*0.5f*255.0f);
				int lY =  (int)((vOffsets[i].y+1)*0.5f*255.0f);

				Log("%d pos(%f:%f) Len: %f: Bytes: %.3d:%.3d\n", i,vOffsets[i].x,vOffsets[i].y,vOffsets[i].Length(),lX, lY);

				vSum += vOffsets[i];
				}
				cVector2f vAvg = vSum / (float)vOffsets.size();
				Log("Average: %s\n",vAvg.ToString().c_str());
				Log("---------------\n");*/


				//Set all samples between 0 and 1
				for(size_t i=0; i<vOffsets.size(); ++i)
				{
					vOffsets[i].x = (vOffsets[i].x+1)*0.5f;
					vOffsets[i].y = (vOffsets[i].y+1)*0.5f;
				}

				///////////////////////////////
				// Add the samples to the texture data
				for(int depth=0; depth<vTextureSize.z; ++depth)
				{
					int lOffset = depth*vTextureSize.x*vTextureSize.y*4 + (int)sample_num*4;	
					unsigned char* pPixelData = &vTextureData[lOffset];				

					int lSample = depth*2;

					//RG
					pPixelData[0] = (int)(vOffsets[lSample].x*255.0f);
					pPixelData[1] = (int)(vOffsets[lSample].y*255.0f);

					//BA
					pPixelData[2] = (int)(vOffsets[lSample+1].x*255.0f);
					pPixelData[3] = (int)(vOffsets[lSample+1].y*255.0f);
				}


		}

		////////////////////////////
		//Create texture data
		apOffsetTexture->CreateFromRawData(vTextureSize,ePixelFormat_RGBA, &vTextureData[0]);
		apOffsetTexture->SetWrapSTR(eTextureWrap_Repeat);
		apOffsetTexture->SetFilter(eTextureFilter_Nearest); //Do not use when using lightspace as lookup

		//Debug texture data:
		/*for(int z=0; z<vTextureSize.z;++z)
		for(int y=0; y<vTextureSize.y;++y)
		for(int x=0; x<vTextureSize.x;++x)
		{
		if(x==0 && y==0){
		Log("---- Depth %d -----\n", z);
		}

		int lOffset =	z*vTextureSize.x*vTextureSize.y*4 + 
		y*vTextureSize.x*4 + 
		x*4;	
		unsigned char* pPixelData = &vTextureData[lOffset];

		Log("(%.3d %.3d %.3d %.3d) ",pPixelData[0],pPixelData[1],pPixelData[2],pPixelData[3]);

		if(x==vTextureSize.x-1) Log("\n");
		}*/

	}

	//-----------------------------------------------------------------------

	void cTextureCreator::GenerateScatterDiskMap2D(iTexture *apOffsetTexture, int alSize, int alSamples, bool abSortSamples)
	{
		////////////////////
		//Test so sizes are correct
		if(cMath::IsPow2(alSize)==false) FatalError("OffsetTexture 2D size size non-pow2\n");
		if(cMath::IsPow2(alSamples)==false) FatalError("OffsetTexture 2D samples size non-pow2\n");

		////////////////////
		// Calculate size of grid
		int lGridSize = (int)(sqrt((float)alSamples)+0.5f);
		std::vector<tVector2fVec> vOffsetsVec;
		vOffsetsVec.resize(alSize*alSize);
		tVector2fVec vTempGridArray;
		vTempGridArray.resize(lGridSize * lGridSize);

		////////////////////////////
		//Setup texture data
		std::vector<unsigned char> vTextureData;
		cVector3l vTextureSize(alSize, alSize, alSamples/2);
		vTextureData.resize(4*vTextureSize.x*vTextureSize.y*vTextureSize.z,0);

		////////////////////////////
		//Generate all the different sample collections
		for(size_t sample_num=0; sample_num<vOffsetsVec.size(); ++sample_num)				
		{
			////////////////////////////
			// Fills a square "grid" size, and then normalizes each pos to be 0 - 1
			for(int y=0;y<lGridSize; ++y)
				for(int x=0;x<lGridSize; ++x)
				{
					//+0.5, because we want center
					cVector2f vPos((float)x + 0.5f, (float) y+0.5f);

					//Randomize point from, but let it never leave the grid square.
					vPos += cMath::RandRectVector2f(-0.5f, 0.5f);

					//Normalize position
					vPos *= 1.0f / (float)lGridSize;

					//Add to array
					vTempGridArray[y*lGridSize + x] = vPos;
				}

				//Get the offsets and put the once we want from the grid here.		
				tVector2fVec& vOffsets = vOffsetsVec[sample_num];
				vOffsets.resize(alSamples);

				float fSampleAdd = ((float)vTempGridArray.size()) / ((float)alSamples);
				float fCurrentSample =0;

				for(size_t i=0; i<vOffsets.size(); ++i)
				{
					int lIdx = (int)fCurrentSample;	

					vOffsets[i] = vTempGridArray[lIdx];

					fCurrentSample+= fSampleAdd;
				}


				////////////////////////////
				// Warp grid so coordinates become spherical
				for(size_t i=0; i<vOffsets.size(); ++i)
				{
					cVector2f vPos = vOffsets[i];

					vOffsets[i].x = sqrtf(vPos.y) * cos(k2Pif * vPos.x);
					vOffsets[i].y = sqrtf(vPos.y) * sin(k2Pif * vPos.x);
				}

				///////////////////////////////
				// Sort by length (if set)
				if(abSortSamples)
				{
					std::sort(vOffsets.begin(),vOffsets.end(), SortFunc_ScatterOffset);
				}


				//Set all samples between 0 and 1
				for(size_t i=0; i<vOffsets.size(); ++i)
				{
					vOffsets[i].x = (vOffsets[i].x+1)*0.5f;
					vOffsets[i].y = (vOffsets[i].y+1)*0.5f;
				}

				///////////////////////////////
				// Add the samples to the texture data
				for(int depth=0; depth<vTextureSize.z; ++depth)
				{
					int lOffset = depth*vTextureSize.x*vTextureSize.y*4 + (int)sample_num*4;	

					unsigned char* pPixelData = &vTextureData[lOffset];				

					int lSample = depth*2;

					//RG
					pPixelData[0] = (int)(vOffsets[lSample].x*255.0f);
					pPixelData[1] = (int)(vOffsets[lSample].y*255.0f);

					//BA
					pPixelData[2] = (int)(vOffsets[lSample+1].x*255.0f);
					pPixelData[3] = (int)(vOffsets[lSample+1].y*255.0f);
				}


		}

		////////////////////////////
		//Create texture data
		apOffsetTexture->CreateFromRawData(cVector3l(vTextureSize.x, vTextureSize.y*vTextureSize.z,1),ePixelFormat_RGBA, &vTextureData[0]);
		apOffsetTexture->SetWrapSTR(eTextureWrap_Repeat);
		apOffsetTexture->SetFilter(eTextureFilter_Nearest); //Do not use when using lightspace as lookup
	}	

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------

}
