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

#ifndef HPL_FRAMEBITMAP_H
#define HPL_FRAMEBITMAP_H

#include "graphics/FrameBase.h"
#include "system/BinTree.h"
#include "math/MathTypes.h"

namespace hpl {

	class cFrameTexture;
	class cFrameSubImage;
	class cBitmap;
	
	//The frames bitmap + rect class
	class cFBitmapRect
	{
	public:
		cFBitmapRect(){mlHandle=-1;}
		cFBitmapRect(int x,int y,int w,int h, int alHandle){
			mRect = cRect2l(x,y,w,h);
			mlHandle = alHandle;
		}
				
		cRect2l mRect;
		int mlHandle;
	};

	//----------------------------------------

	class cFBitmapImage
	{
	public:
		~cFBitmapImage();

		cRect2l mRect;
		cFrameSubImage *mpSubImage;
	};

	//----------------------------------------
	
    typedef BinTree<cFBitmapRect> tRectTree;
	typedef BinTreeNode<cFBitmapRect> tRectTreeNode;
	typedef std::vector<tRectTreeNode*> tRectTreeNodeVec;
	typedef tRectTreeNodeVec::iterator tRectTreeNodeVecIt;
	typedef tRectTreeNodeVec::const_iterator tRectTreeNodeVecConstIt;

	typedef std::list<cFBitmapImage*> cFBitmapImageList;
	typedef cFBitmapImageList::iterator cFBitmapImageListIt;
		
	class cFrameBitmap : public iFrameBase
	{
	public:
		cFrameBitmap(cBitmap *apBitmap, cFrameTexture *apFrmTex, int alHandle);
		~cFrameBitmap();

		cFrameSubImage * AddBitmap(cBitmap *apSrc, const tWString& asFullPath, cFrameSubImage *apSubImageCreated, bool *apFoundNode=NULL);
		bool MinimumFit(cRect2l aSrc,cRect2l aDest);
		bool IsFull();
		bool IsUpdated();

		bool IsLocked()const { return mbIsLocked;}
		void SetLocked(bool abX){ mbIsLocked = abX; }

		void SetNeedReorganisation();

		void UpdateBeforeDraw();
		
		void Reorganize();
		bool FlushToTexture();

		cFrameTexture* GetFrameTexture(){ return mpFrameTexture;}

		int GetHandle()const{ return mlHandle; }

		int GetAdditionsSinceReorganization(){ return mlAdditionsSinceReorganization;}
	private:
		void ClearAddedImages();

		cBitmap* mpBitmap;
		cFrameTexture* mpFrameTexture;
		
		tRectTree mRects;

		cFBitmapImageList mlstImages;
		
		int mlMinHole;
		int mlHandle;
		bool mbIsFull;
		bool mbIsUpdated;
		bool mbIsLocked;
		bool mbNeedNeedReorganisation;

		int mlAdditionsSinceReorganization;
	};

};
#endif // HPL_FRAMEBITMAP_H
