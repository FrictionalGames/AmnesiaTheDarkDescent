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

#include "../Common/EdObjectIcon.h"
#include "../Common/EdScnObject.h"
#include "../Common/EdWorld.h"
#include "../Common/EdResource.h"
#include "../Common/EdWindowViewport.h"
#include "../Common/Editor.h"

///////////////////////////////////////////////////////////////////
// ENTITY ICON
///////////////////////////////////////////////////////////////////

//------------------------------------------------------------------

cObjectIcon::cObjectIcon(iEdScnObject* apParent, const tString& asIconGfxName)
{
	mbVisible = true;

	mpParent = apParent;
	cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();
	mpDummy = pWorld->CreateDummyRenderable("Dummy_" + cString::To8Char(apParent->GetName().c_str()));
	mpDummy->SetRenderableUserData(mpParent);
	//for(int i=0;i<2;++i)
	//	mvIconGfx[i] = NULL;

	cEdLoader::LoadTexture2D("Billboard" + asIconGfxName + ".tga",			&mvIconGfx[0]);
	cEdLoader::LoadTexture2D("Billboard" + asIconGfxName + "Selected.tga",	&mvIconGfx[1]);
}

//------------------------------------------------------------------

cObjectIcon::~cObjectIcon()
{
	mpParent->GetWorld()->GetEngWorld()->DestroyDummyRenderable(mpDummy);
	cTextureManager* pTexMgr = mpParent->GetWorld()->GetEditor()->GetEngine()->GetResources()->GetTextureManager();
	for(int i=0; i<2; ++i)
		if(mvIconGfx[i]) pTexMgr->Destroy(mvIconGfx[i]);
}

//------------------------------------------------------------------

bool cObjectIcon::Check2DBoxIntersect(iEdViewport* apViewport, const cRect2l& aBox)
{
	cRect2l rect;
	cFrustum* pFrustum = apViewport->GetEngCamera()->GetFrustum();
	cBoundingVolume* pBV = GetPickBV(apViewport, cVector3f(0.06f));

	if(pFrustum->CollideBoundingVolume(pBV)== eCollision_Outside)
		return false;

	cMath::GetClipRectFromBV(rect,*pBV,pFrustum,apViewport->GetGuiViewportSizeInt(), pFrustum->GetFOV()*0.5f);

	return cMath::CheckRectIntersection(aBox, rect);
}

bool cObjectIcon::CheckRayIntersect(iEdViewport* apViewport, cVector3f* apIntersection, tVector3fVec* apTriangle, float *apT)
{
	cBoundingVolume* pBV = GetPickBV(apViewport, 0.06f);
	return cMath::CheckBVIntersection(*apViewport->GetRayBV(), *pBV) && 
		cMath::CheckAABBLineIntersection(pBV->GetMin(), pBV->GetMax(), apViewport->GetUnprojectedStart(),
											apViewport->GetUnprojectedEnd(), apIntersection, apT);
}

//------------------------------------------------------------------

void cObjectIcon::DrawIcon(iEdViewport* apViewport, 
						   cRendererCallbackFunctions* apFunctions, 
						   bool abIsSelected,
						   bool abIsActive,
						   const cColor& aColor,
						   const cColor& aDisabledCol)
{
	if(mbVisible==false)
		return;

	iTexture* pIcon = mvIconGfx[0];

	if(pIcon)
	{
		const cVector3f& vPos = mpParent->GetTranslation();
		apViewport->DrawBillboard(pIcon, vPos, 0.1f, cColor(0.75f,1), apFunctions);
        if(abIsSelected)
		{
			apFunctions->SetBlendMode(eMaterialBlendMode_Add);
			apViewport->DrawBillboard(mvIconGfx[1], vPos, 0.1f, aColor*cColor(0.75f, 1), apFunctions);
			apFunctions->SetBlendMode(eMaterialBlendMode_None);
		}
	}

	/*if(mvIconGfx[abIsSelected])
	{
		cColor bbColor = abIsSelected ? cColor(1,1) : (abIsActive ? cColor(0.5f,1) : aDisabledCol);
		apViewport->DrawBillboard(mvIconGfx[abIsSelected], mpParent->GetTranslation(), 0.1f, bbColor, apFunctions);
	}*/
}

//-------------------------------------------------------------------

cRect2l cObjectIcon::GetIconClipRectangle(iEdViewport* apViewport, iEdScnObject* apObj)
{
	cVector3f vPos, vSize;
	apViewport->GetViewSpacePositionAndSize(apObj->GetTranslation(), 0.06f, vPos, vSize);
	cVector3f vUpperLeftCorner =  vPos - cVector3f(0.1f,0.1f,0);

	return cRect2l((int)vUpperLeftCorner.x, (int)vUpperLeftCorner.y, 1, 1);
}

//-------------------------------------------------------------------

cBoundingVolume* cObjectIcon::GetPickBV(iEdViewport* apViewport, const cVector3f& avSize)
{
	mIconBV.SetPosition(mpParent->GetTranslation());

	if(apViewport==NULL)
	{
		mIconBV.SetSize(0.06f);
	}
	else
	{
		cVector3f vPosition, vSize;
		apViewport->GetViewSpacePositionAndSize(mpParent->GetTranslation(), avSize, vPosition, vSize);
		
		mIconBV.SetSize(vSize);
	}

	return &mIconBV;
}

