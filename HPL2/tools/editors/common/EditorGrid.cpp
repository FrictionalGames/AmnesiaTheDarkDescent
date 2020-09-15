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

#include "EditorGrid.h"

#include "EditorBaseClasses.h"
#include "EditorViewport.h"

//--------------------------------------------------------

bool cEditorGrid::mbSnapToGrid = true;
float cEditorGrid::mfSnapSeparation = 0.25f;

//--------------------------------------------------------

cEditorGrid::cEditorGrid(iEditorViewport* apViewport)
{
	mpViewport = apViewport;
	mfSize = 40;
	mColor = cColor(0.5f,0.3f);
}

//--------------------------------------------------------

/** Given a World Position as parameter, return the closest point on the grid
 *
 * \param avWorldPos 
 * \return 
 */
cVector3f cEditorGrid::GetSnappedPosOnGrid(const cVector3f& avWorldPos, bool abForceSnap)
{
	if(abForceSnap==false && mbSnapToGrid==false)
		return avWorldPos;

	return GetProjectedPosOnPlane(GetSnappedPosInSpace(avWorldPos));
}

//--------------------------------------------------------

cVector3f cEditorGrid::GetSnappedPosInSpace(const cVector3f& avWorldPos)
{
	float fInvSep = 1/mfSnapSeparation;
	cVector3f vSnapPos;
	vSnapPos.x = cMath::RoundToInt(avWorldPos.x*fInvSep) * mfSnapSeparation;
	vSnapPos.y = cMath::RoundToInt(avWorldPos.y*fInvSep) * mfSnapSeparation;
	vSnapPos.z = cMath::RoundToInt(avWorldPos.z*fInvSep) * mfSnapSeparation;

	return vSnapPos;
}

//--------------------------------------------------------

void cEditorGrid::Draw(cRendererCallbackFunctions *apFunctions, const cVector3f& avPos)
{
	if(mbVisible==false || mfSnapSeparation<=0)
		return;

	cVector3f vAdd1 = cVector3f(0);
	cVector3f vAdd2 = cVector3f(0);

	float fHalfGridSize = mfSize*0.5f;

	cVector3f vStartCorner = avPos;
	cVector3f vLineStart1 = cVector3f(0);
	cVector3f vLineStart2 = cVector3f(0);
	cVector3f vLineEnd1 = cVector3f(0);
	cVector3f vLineEnd2 = cVector3f(0);

	cVector3f vAxisStart1;
	cVector3f vAxisStart2;
	cVector3f vAxisEnd1;
	cVector3f vAxisEnd2;
	cColor axisColor1;
	cColor axisColor2;

	float fHeight = GetHeight();
	
	float fCount = 0;

	//////////////////////////////////////////
	// Set up drawing depending on grid plane
	switch(mNormal)
	{
	case ePlaneNormal_X:
		vAxisStart1 = cVector3f(0,-fHalfGridSize, 0);
		vAxisEnd1 = cVector3f(0,fHalfGridSize, 0);
		axisColor1 = cColor(0,1,0,1);
		vAxisStart2 = cVector3f(0,0,-fHalfGridSize);
		vAxisEnd2 = cVector3f(0,0,fHalfGridSize);
		axisColor2 = cColor(0,0,1,1);

		vAdd1.y = mfSnapSeparation;
		vAdd2.z = mfSnapSeparation;
		vStartCorner.x = fHeight;
		vStartCorner.y -= fHalfGridSize;
		vStartCorner.z -= fHalfGridSize;
		vLineEnd1.z = mfSize;
		vLineEnd2.y = mfSize;
		
		break;
	case ePlaneNormal_Y:
		vAxisStart1 = cVector3f(-fHalfGridSize, 0, 0);
		vAxisEnd1 = cVector3f(fHalfGridSize, 0, 0);
		axisColor1 = cColor(1,0,0,1);
		vAxisStart2 = cVector3f(0,0,-fHalfGridSize);
		vAxisEnd2 = cVector3f(0,0,fHalfGridSize);
		axisColor2 = cColor(0,0,1,1);

		vAdd1.x = mfSnapSeparation;
		vAdd2.z = mfSnapSeparation;
		vStartCorner.y = fHeight;
		vStartCorner.x -= fHalfGridSize;
		vStartCorner.z -= fHalfGridSize;
		vLineEnd1.z = mfSize;
		vLineEnd2.x = mfSize;
		
		break;
	case ePlaneNormal_Z:
		vAxisStart1 = cVector3f(-fHalfGridSize, 0, 0);
		vAxisEnd1 = cVector3f(fHalfGridSize, 0, 0);
		axisColor1 = cColor(1,0,0,1);
		vAxisStart2 = cVector3f(0,-fHalfGridSize,0);
		vAxisEnd2 = cVector3f(0,fHalfGridSize,0);
		axisColor2 = cColor(0,1,0,1);

		vAdd1.x = mfSnapSeparation;
		vAdd2.y = mfSnapSeparation;
		vStartCorner.z = fHeight;
		vStartCorner.x -= fHalfGridSize;
		vStartCorner.y -= fHalfGridSize;
		vLineEnd1.y = mfSize;
		vLineEnd2.x = mfSize;
		
		break;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Start actual drawing: Starting from one of the grid corners, this loop draws 2 perpendicular lines per iteration, 
	// then adds the grid separation for the next one, until the grid size is covered
	vLineStart1 = GetSnappedPosOnGrid(vStartCorner,true);
	vLineStart2 = vLineStart1;

	
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);
	
	//apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
	
	apFunctions->SetProgram(NULL);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetMatrix(NULL);

	while(fCount <= mfSize)
	{
		apFunctions->GetLowLevelGfx()->DrawLine(vLineStart1, vLineStart1+vLineEnd1, mColor);
		apFunctions->GetLowLevelGfx()->DrawLine(vLineStart2, vLineStart2+vLineEnd2, mColor);

		vLineStart1 += vAdd1;
		vLineStart2 += vAdd2;
        
		fCount += mfSnapSeparation;
	}

	//apFunctions->GetLowLevelGfx()->DrawLine(vAxisStart1, vAxisEnd1, axisColor1);
	//apFunctions->GetLowLevelGfx()->DrawLine(vAxisStart2, vAxisEnd2, axisColor2);

	apFunctions->SetBlendMode(eMaterialBlendMode_None);
}
//--------------------------------------------------------

void cEditorGrid::OnPlaneModified()
{
	mpViewport->GetVCamera()->OnGridUpdated();
}

//--------------------------------------------------------
