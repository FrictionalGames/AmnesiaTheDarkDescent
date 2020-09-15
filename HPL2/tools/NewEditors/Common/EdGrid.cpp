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

#include "../Common/EdGrid.h"

#include "../Common/Editor.h"
#include "../Common/EdViewport.h"
#include "../Common/EdCamera.h"
#include "../Common/EdGridActions.h"

//--------------------------------------------------------

bool cEdGrid::mbSnapToGrid = true;
float cEdGrid::mfSize = 40;
float cEdGrid::mfSnapSeparation = 0.25f;
float cEdGrid::mfInvSnapSeparation = 1/cEdGrid::mfSnapSeparation;
float cEdGrid::mfMinSnapSep = 0.1f;
float cEdGrid::mfMaxSnapSep = 1.0f;

//--------------------------------------------------------

cEdGrid::cEdGrid(iEdViewport* apViewport)
{
	mpViewport = apViewport;
	mColor = cColor(0.5f,0.25f);
}

//--------------------------------------------------------

void cEdGrid::SetSnapSeparation(const float afSeparation)
{
	if(mfSnapSeparation==afSeparation) return;

	mfSnapSeparation = cMath::Clamp(afSeparation, mfMinSnapSep, mfMaxSnapSep);
	mfInvSnapSeparation = 1/mfSnapSeparation;
}

//--------------------------------------------------------

/** Given a World Position as parameter, return the closest point on the grid
 *
 * \param avWorldPos 
 * \return 
 */
cVector3f cEdGrid::GetSnappedPosOnGrid(const cVector3f& avWorldPos, bool abForceSnap)
{
	if(abForceSnap==false && mbSnapToGrid==false)
		return avWorldPos;

	return GetProjectedPosOnPlane(GetSnappedPosInSpace(avWorldPos));
}

//--------------------------------------------------------

cVector3f cEdGrid::GetSnappedPosInSpace(const cVector3f& avWorldPos, bool abSnapX, bool abSnapY, bool abSnapZ)
{
	cVector3f vSnapPos;
	vSnapPos.x = GetSnappedValue(avWorldPos.x, abSnapX, mfSnapSeparation);
	vSnapPos.y = GetSnappedValue(avWorldPos.y, abSnapY, mfSnapSeparation);
	vSnapPos.z = GetSnappedValue(avWorldPos.z, abSnapZ, mfSnapSeparation);

	return vSnapPos;
}

//--------------------------------------------------------

float cEdGrid::GetSnappedValue(const float afX, bool abSnap, float afSnapValue)
{
	if(mbSnapToGrid && abSnap)
		return cMath::RoundToInt(afX*mfInvSnapSeparation) * afSnapValue;
	else
		return afX;
}

//--------------------------------------------------------

iEdAction* cEdGrid::CreateSetSnapSeparationAction(float afX)
{
	return hplNew(cEdActionGridChangeSnapSeparation,(this, afX));
}

//--------------------------------------------------------

void cEdGrid::Draw(cRendererCallbackFunctions* apFunctions)
{
	Draw(apFunctions, mpViewport->GetGridCenter());
}

void cEdGrid::Draw(cRendererCallbackFunctions *apFunctions, const cVector3f& avPos)
{
	if(mbVisible==false)
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
	
	apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
	
	apFunctions->SetProgram(NULL);
	apFunctions->SetTextureRange(NULL,0);
	apFunctions->SetMatrix(NULL);

	float fCount = 0;
	while(fCount <= mfSize)
	{
		// XXX: Draw lines by pieces with fading alpha values.
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

void cEdGrid::OnPlaneModified()
{
	mpViewport->GetCamera()->OnGridUpdated();

	mpViewport->GetEditor()->BroadcastModuleMsg(eEdModuleMsg_ViewportParamsChange, mpViewport);
}

//--------------------------------------------------------
