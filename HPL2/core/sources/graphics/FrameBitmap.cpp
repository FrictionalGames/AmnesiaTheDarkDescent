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

#include "graphics/FrameBitmap.h"

#include "math/Math.h"

#include "system/LowLevelSystem.h"
#include "system/Platform.h"
#include "system/String.h"

#include "graphics/FrameTexture.h"
#include "graphics/FrameSubImage.h"
#include "graphics/Bitmap.h"
#include "graphics/Texture.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// FRAME BITMAP IMAGE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cFBitmapImage::~cFBitmapImage()
	{
		if(mpSubImage) mpSubImage->mpFrameBitmapImage = NULL;
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cFrameBitmap::cFrameBitmap(cBitmap *apBitmap,  cFrameTexture *apFrmTex, int alHandle) : iFrameBase()
	{
		mpBitmap = apBitmap;
		mpFrameTexture = apFrmTex;
		//TODO: Make a filling method
		mpBitmap->Clear(cColor(1,0,1,0),0,0);
		mlMinHole = 6;
		mlHandle = alHandle;
		mbIsFull = false;
		mbIsLocked = false;
		mlPicCount =0;
		mlAdditionsSinceReorganization =0;

		mbNeedNeedReorganisation = false;

		//Root node in rect tree
		mRects.Insert(cFBitmapRect(0,0,mpBitmap->GetWidth(), mpBitmap->GetHeight(),-1));
	}

	cFrameBitmap::~cFrameBitmap()
	{
		STLDeleteAll(mlstImages);
		hplDelete(mpBitmap);
		mpBitmap = NULL;
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	#define DEBUG_BTREE (false)

	cFrameSubImage *cFrameBitmap::AddBitmap(cBitmap *apSrc, const tWString& asFullPath, cFrameSubImage *apSubImageCreated, bool *apFoundNode)
	{
		cFBitmapImage *pBitmapImage=NULL;
		cFrameSubImage *pImage=NULL;
		//source size
		//+2 because we are gonna have a border to get rid if some antialiasing problems
		int lSW = apSrc->GetWidth()+2; 
		int lSH = apSrc->GetHeight()+2;
		
		//destination size
		int lDW = mpBitmap->GetWidth();
		int lDH = mpBitmap->GetHeight();

		cVector2l vPos;
		
		bool bFoundEmptyNode = false;
		bool bFoundNode = false;
		//Debug
		int node=0;

		if(DEBUG_BTREE)Log("**** Image %d *****\n",mlPicCount);

		//Get the leaves of the tree and search it for a good pos.
		const tRectTreeNodeVec& lstNodes =  mRects.GetLeafList();
		tRectTreeNodeVecConstIt it;
		for(it = lstNodes.begin();it!=lstNodes.end();++it)
		{
			if(DEBUG_BTREE)Log("Checking node %d:\n",node++);
			tRectTreeNode *TopNode = *it;
			cFBitmapRect* pData = TopNode->GetData();
			
			//Check if the space is free
			if(pData->mlHandle<0)
			{
				if(DEBUG_BTREE)Log("Found free node\n");
				bFoundEmptyNode = true; //An empty node was found.. bitmap not full yet.
				
				//Check if the Image fits in the rect
				cRect2l NewRect = cRect2l(pData->mRect.x,pData->mRect.y,lSW, lSH);
				if(DEBUG_BTREE)Log("Fit: [%d:%d:%d:%d] in [%d:%d:%d:%d]\n",
						NewRect.x,NewRect.y,NewRect.w,NewRect.h,
						pData->mRect.x,pData->mRect.y,pData->mRect.w,pData->mRect.h);
				
				if(cMath::CheckRectFit(NewRect, pData->mRect))
				{
					if(DEBUG_BTREE)Log("The node fits!\n");
					bFoundNode = true;

                    //If the bitmap fits perfectly add the node without splitting
					if(MinimumFit(NewRect,pData->mRect))
					{
						if(DEBUG_BTREE)Log("Minimum fit!\n");
						pData->mRect = NewRect;
						pData->mlHandle = 1;
					}
					//If there is still space left, make new nodes.
					else
					{
						if(DEBUG_BTREE)Log("Normal fit!\n");
						//Insert 2 children for the top node (lower and upper part.
						tRectTreeNode* UpperNode;
						//Upper
						UpperNode = mRects.InsertAt(cFBitmapRect(NewRect.x,NewRect.y,
													pData->mRect.w,NewRect.h,-2),TopNode,
													eBinTreeNode_Left);
						
						//Lower
						mRects.InsertAt(cFBitmapRect(NewRect.x,NewRect.y+NewRect.h,
										pData->mRect.w,pData->mRect.h-NewRect.h,-3),TopNode,
										eBinTreeNode_Right);

						//Split the Upper Node into 2 nodes.
						pData = UpperNode->GetData();//Get the data for the upper node.
						//Upper split, this is the new bitmap
						mRects.InsertAt(cFBitmapRect(NewRect.x,NewRect.y,
										NewRect.w,NewRect.h,2),UpperNode,
										eBinTreeNode_Left);
						
						//Lower split, this is empty
						mRects.InsertAt(cFBitmapRect(NewRect.x+NewRect.w,NewRect.y,
										pData->mRect.w-NewRect.w,NewRect.h,-4),UpperNode,
										eBinTreeNode_Right);
					}
					
					vPos = cVector2l(NewRect.x+1,NewRect.y+1);//+1 for the right pos		            
					
					//Draw corners for border
					mpBitmap->Blit(	apSrc,cVector3l(NewRect.x, NewRect.y, 0),
									cVector3l(1,1,1),0);
					mpBitmap->Blit(	apSrc,cVector3l(NewRect.x + apSrc->GetWidth()+1, NewRect.y,0), 
									cVector3l(1,1,1), cVector3l(apSrc->GetWidth()-1,0,0) );
					mpBitmap->Blit(	apSrc,cVector3l(NewRect.x + apSrc->GetWidth()+1, NewRect.y + apSrc->GetHeight()+1,0), 
									cVector3l(1,1,1), cVector3l(apSrc->GetWidth()-1,apSrc->GetHeight()-1,0) );
					mpBitmap->Blit(	apSrc,cVector3l(NewRect.x, NewRect.y + apSrc->GetHeight()+1,0), 
									cVector3l(1,1,1), cVector3l(0,apSrc->GetHeight()-1,0) );
					
					//Draw sides for border
					mpBitmap->Blit(	apSrc,cVector3l(NewRect.x+1, NewRect.y, 0),
									cVector3l(apSrc->GetWidth(),1,1),0);
					mpBitmap->Blit(	apSrc,cVector3l(NewRect.x+1, NewRect.y+apSrc->GetHeight()+1, 0),
									cVector3l(apSrc->GetWidth(),1,1), cVector3l(0,apSrc->GetHeight()-1,0));
					mpBitmap->Blit(	apSrc,cVector3l(NewRect.x, NewRect.y+1, 0),
									cVector3l(1,apSrc->GetHeight(),1),0);
					mpBitmap->Blit(	apSrc,cVector3l(NewRect.x+apSrc->GetWidth()+1, NewRect.y+1, 0),
									cVector3l(1,apSrc->GetHeight(),1),cVector3l(apSrc->GetWidth()-1,0,0));
										
					//Draw the final
					mpBitmap->Blit(apSrc,cVector3l(NewRect.x+1,NewRect.y+1,0), apSrc->GetSize(),0);
					
					//Add image data
					pBitmapImage = hplNew( cFBitmapImage, () );
					//Connect the subimage with the frame image.
					if(apSubImageCreated){
						pBitmapImage->mpSubImage = apSubImageCreated;
						apSubImageCreated->mpFrameBitmapImage = pBitmapImage;
					}
					pBitmapImage->mRect.x = NewRect.x+1; pBitmapImage->mRect.y = NewRect.y+1;
					pBitmapImage->mRect.w = apSrc->GetWidth(); pBitmapImage->mRect.h = apSrc->GetHeight();
					mlstImages.push_back(pBitmapImage);
					
					mlAdditionsSinceReorganization++;
					mlPicCount++;
					mpFrameTexture->SetPicCount(mlPicCount);
					break;
				}
			}
		}

		if(bFoundNode && apSubImageCreated == NULL)
		{
			//Create the image resource
			pImage = hplNew( cFrameSubImage, (cString::To8Char(apSrc->GetFileName()),asFullPath, 
							mpFrameTexture, this,
							cRect2l(vPos,cVector2l(lSW-2,lSH-2)),//-2 to get the correct size.
							cVector2l(mpBitmap->GetWidth(),mpBitmap->GetHeight()),
							mlHandle, pBitmapImage) );
			
			pBitmapImage->mpSubImage = pImage;

			if(!bFoundEmptyNode)
			{
				mbIsFull = true;
			}

			mbIsUpdated = true;
		}

		if(apFoundNode) *apFoundNode = bFoundNode;

		/// LAST DEBUG ///
		if(DEBUG_BTREE)
		{
			Log("Current Tree begin:\n");
			const tRectTreeNodeVec& lstNodes =  mRects.GetNodeList();
			tRectTreeNodeVecConstIt it;
			int node=0;
			for(it = lstNodes.begin();it!=lstNodes.end();++it)
			{
				cRect2l Rect = (*it)->GetData()->mRect;
				int h = (*it)->GetData()->mlHandle;
				Log(" %d: [%d:%d:%d:%d]:%d\n",node,Rect.x,Rect.y,Rect.w,Rect.h,h);
				node++;
			}
			Log("Current Tree end:\n");
			Log("-----------------\n");

			Log("Current Leaves begin:\n");
			const tRectTreeNodeVec& lstLeafNodes =  mRects.GetLeafList();
			node=0;
			for(it = lstLeafNodes.begin();it!=lstLeafNodes.end();++it)
			{
				cRect2l Rect = (*it)->GetData()->mRect;
				int h = (*it)->GetData()->mlHandle;
				Log(" %d: [%d:%d:%d:%d]: %d\n",node,Rect.x,Rect.y,Rect.w,Rect.h,h);
				node++;
			}
			Log("Current Tree end:\n");
			Log("-----------------\n");
		}

		
		return pImage;
	}

	//-----------------------------------------------------------------------

	bool cFrameBitmap::MinimumFit(cRect2l aSrc,cRect2l aDest)
	{
		if(aDest.w-aSrc.w<mlMinHole && aDest.h-aSrc.h<mlMinHole) return true;

		return false;
	}

	//-----------------------------------------------------------------------

	bool cFrameBitmap::IsUpdated()
	{
		return mbIsUpdated;
	}
    
	//-----------------------------------------------------------------------

	bool cFrameBitmap::IsFull()
	{
		return mbIsFull;
	}

	//-----------------------------------------------------------------------

	void cFrameBitmap::SetNeedReorganisation()
	{
		mbNeedNeedReorganisation = true;
	}

	//-----------------------------------------------------------------------

	void cFrameBitmap::UpdateBeforeDraw()
	{
		if(mbNeedNeedReorganisation)
		{
			Reorganize();
			mbNeedNeedReorganisation = false;
		}

		FlushToTexture();
	}

	//-----------------------------------------------------------------------

	class cBitmapSubImageComb
	{
	public: 
		cBitmapSubImageComb(cBitmap *apBitmap, cFrameSubImage *apSubImage){
			mpBitmap = apBitmap;
			mpSubImage = apSubImage;
		}
		~cBitmapSubImageComb(){
			hplDelete( mpBitmap );
		}

		cBitmap *mpBitmap;
		cFrameSubImage *mpSubImage;
	};

	class cFrameBitmapCompare
	{
	public:
		bool operator()(cBitmapSubImageComb* apCombA,cBitmapSubImageComb* apCombB) const
		{
			const cVector3l& vSizeA = apCombA->mpBitmap->GetSize();
			const cVector3l& vSizeB = apCombB->mpBitmap->GetSize();

			//Sort by width, then height
            //if(vSizeA.x != vSizeB.x) return vSizeA.x > vSizeB.x;
			//return vSizeA.y > vSizeB.y;

			//Sort by greatest length
			int lMaxA = vSizeA.x > vSizeA.y ? vSizeA.x : vSizeA.y;
			int lMaxB = vSizeB.x > vSizeB.y ? vSizeB.x : vSizeB.y;
			if(lMaxA != lMaxB) return lMaxA > lMaxB;

			//If equal go by area
			return vSizeA.x*vSizeA.y > vSizeB.x *vSizeB.y;
		}
	};

	typedef std::multiset<cBitmapSubImageComb*,cFrameBitmapCompare> tFrameBitmapSortSet;
	typedef tFrameBitmapSortSet::iterator tFrameBitmapSortSetIt;

	typedef std::list<cBitmapSubImageComb*> tFrameBitmapCombList;
	typedef tFrameBitmapCombList::iterator tFrameBitmapCombListIt;

	//-----------------------------------------------------------------------


	void cFrameBitmap::Reorganize()
	{
		tFrameBitmapSortSet setSortedBitmaps;
		tFrameBitmapCombList lstBitmaps;

		bool mbLogTime = false;

		///////////////////////////////////////
		//Iterate nodes and retrive all bitmaps
		unsigned long lStartTime = cPlatform::GetApplicationTime();
		
		cFBitmapImageListIt imageIt = mlstImages.begin();
		for(; imageIt != mlstImages.end(); ++imageIt)
		{
			cFBitmapImage* pImage = *imageIt;
			if(pImage->mpSubImage==NULL) continue;
			
			//Log("Node: %dx%d (%d, %d)\n",pRect->mRect.w,pRect->mRect.h,pRect->mRect.x,pRect->mRect.y);

			//Create bitmap according to node rect
            cBitmap *pBitmap = hplNew( cBitmap, ());
			//-2 since we do not want borders
			pBitmap->CreateData(cVector3l(pImage->mRect.w,pImage->mRect.h,1),ePixelFormat_RGBA,0,0);

			//Copy from from bitmap to temp bitmap. +1 because we do not want borders.
			pBitmap->Blit(mpBitmap, 0, pBitmap->GetSize(),cVector3l(pImage->mRect.x, pImage->mRect.y,0));

			//Add bitmap to list
			cBitmapSubImageComb *pBmpComb = hplNew( cBitmapSubImageComb,(pBitmap,pImage->mpSubImage) );
			setSortedBitmaps.insert(pBmpComb);
			lstBitmaps.push_back(pBmpComb);
		}
		if(mbLogTime)Log(" getting image data took: %dms\n",cPlatform::GetApplicationTime()-lStartTime);
        

		///////////////////////////////////////
		//Clear Frame data
		lStartTime = cPlatform::GetApplicationTime();

		ClearAddedImages();//Clears the tree and image list.

		if(mbLogTime)Log(" clearing data took: %dms\n",cPlatform::GetApplicationTime()-lStartTime);


		lStartTime = cPlatform::GetApplicationTime();

		///////////////////////////////////////
		//Add all bitmaps again
		bool bAllNodesFit = false;
		tFrameBitmapSortSetIt bmpIt = setSortedBitmaps.begin();
		for(; bmpIt != setSortedBitmaps.end(); ++bmpIt)
		{
			cBitmapSubImageComb *pComb = *bmpIt;
			
			AddBitmap(pComb->mpBitmap, pComb->mpSubImage->GetFullPath(), pComb->mpSubImage, &bAllNodesFit);
			if(bAllNodesFit==false) break;
		}
		
		///////////////////////////////////////
		// Check if all could be added
		// If no room for bitmap was found, need to add again, according to previous order
		if(bAllNodesFit==false)
		{
			ClearAddedImages();
            
			for(tFrameBitmapCombListIt it = lstBitmaps.begin(); it != lstBitmaps.end(); ++it)
			{
				cBitmapSubImageComb *pComb = *it;

				bool bFit = false;
				AddBitmap(pComb->mpBitmap, pComb->mpSubImage->GetFullPath(), pComb->mpSubImage, &bFit);
				if(bFit==false){
					Error("When reorganizing images according to previous order an image still does not fit!!!\n");
					pComb->mpSubImage->mpFrameBitmapImage = NULL;
				}
			}
		}


		if(mbLogTime)Log(" adding again took: %dms\n",cPlatform::GetApplicationTime()-lStartTime);

		
		//Do this on demand instead...
		imageIt = mlstImages.begin();
		for(; imageIt != mlstImages.end(); ++imageIt)
		{
			cFBitmapImage* pImage = *imageIt;
			if(pImage->mpSubImage) pImage->mpSubImage->SetNeedUpdateUvs();
		}

		
		///////////////////////////////////////
		//Clean up and exit
		STLDeleteAll(lstBitmaps);
		
		mlAdditionsSinceReorganization =0;
		//Set as updated!
		mbIsUpdated = true;
	}
	
	//-----------------------------------------------------------------------

	bool cFrameBitmap::FlushToTexture()
	{
		if(mbIsUpdated)
		{
			mpFrameTexture->GetTexture()->CreateFromBitmap(mpBitmap);
			mpFrameTexture->GetTexture()->SetWrapS(eTextureWrap_ClampToEdge);
			mpFrameTexture->GetTexture()->SetWrapT(eTextureWrap_ClampToEdge);

			//mpFrameTexture->SetPicCount(mlPicCount);
			mbIsUpdated = false;
			return true;
		}
		else
		{
			return false;
		}
	}

	//-----------------------------------------------------------------------

	void cFrameBitmap::ClearAddedImages()
	{
		mpBitmap->Clear(cColor(1,0,1,0),0,0);

		mlPicCount =0;
		STLDeleteAll(mlstImages);
		mlstImages.clear();

		//Clear entire tree
		mRects.Clear();

		//Add a new Root node in rect tree
		mRects.Insert(cFBitmapRect(0,0,mpBitmap->GetWidth(), mpBitmap->GetHeight(),-1));
	}
	
	//-----------------------------------------------------------------------
}
