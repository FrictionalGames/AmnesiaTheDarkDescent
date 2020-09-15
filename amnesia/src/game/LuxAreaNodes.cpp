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

#include "LuxAreaNodes.h"

#include "LuxMap.h"

//////////////////////////////////////////////////////////////////////////
// PLAYER START
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxNode_PlayerStart::cLuxNode_PlayerStart(const tString& asName)
{
	msName = asName;
	mvPos =0;
}

//-----------------------------------------------------------------------

cLuxAreaNodeLoader_PlayerStart::cLuxAreaNodeLoader_PlayerStart(const tString& asName) : iAreaLoader(asName)
{
	mbCreatesStaticArea = true;
}

//-----------------------------------------------------------------------

void cLuxAreaNodeLoader_PlayerStart::Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld)
{
	cLuxMap *pMap = gpBase->mpCurrentMapLoading;
	if(pMap==NULL)
	{
		cStartPosEntity* pPos = apWorld->CreateStartPos(asName);
		pPos->SetMatrix(a_mtxTransform);

		return;
	}

	cLuxNode_PlayerStart *pNode = hplNew(cLuxNode_PlayerStart, (asName));
	pNode->mvPos = a_mtxTransform.GetTranslation() + cVector3f(0,0.05f, 0);
	
	cVector3f vForward = cMath::MatrixMul(a_mtxTransform.GetRotation(), cVector3f(0,0,1));
	
	pNode->mfAngle = -cMath::GetAngleFromPoints2D(0, cVector2f(vForward.x, vForward.z));

	//pNode->mfAngle = cMath::Vector3Angle(vForward, cVector3f(0,0,1));
	//if(cMath::Vector3Dot(vForward, cVector3f(1,0,0)) < 0) pNode->mfAngle = -pNode->mfAngle;	

	pMap->AddPlayerStart(pNode);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PATH NODE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxAreaNodeLoader_PathNode::cLuxAreaNodeLoader_PathNode(const tString& asName)  : iAreaLoader(asName) 
{

	mbCreatesStaticArea = true;
}

//-----------------------------------------------------------------------

void cLuxAreaNodeLoader_PathNode::Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld)
{
	apWorld->AddAINode(asName,alID, "Default", a_mtxTransform.GetTranslation()+cVector3f(0,0.05f,0));
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// POS NODE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxAreaNodeLoader_PosNode::cLuxAreaNodeLoader_PosNode(const tString& asName) : iAreaLoader(asName)
{
	mbCreatesStaticArea = true;
}

//-----------------------------------------------------------------------

void cLuxAreaNodeLoader_PosNode::Load(const tString &asName, int alID, bool abActive, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld *apWorld)
{
	cLuxMap *pMap = gpBase->mpCurrentMapLoading;

	cLuxNode_Pos *pNode = hplNew(cLuxNode_Pos, (asName));
	pNode->mvPos = a_mtxTransform.GetTranslation();

	pMap->AddPosNode(pNode);
}

//-----------------------------------------------------------------------


