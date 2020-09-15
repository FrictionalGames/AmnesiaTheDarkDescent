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

#include "../Common/EdClipPlane.h"
#include "../Common/EdScnWorld.h"
#include "../Common/EdClipPlaneActions.h"
#include "../Common/Editor.h"

//------------------------------------------------------------------

cEdClipPlane::cEdClipPlane(iEdScnWorld* apMap, int alID)
{
	mpMap = apMap;
	mlID = alID;

	mbCullingOnPositiveSide = true;
	mbActive = true;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
/*
bool cEdClipPlane::PointIsOnCullingSide(const cVector3f& avPos)
{
	float fHeight = GetHeight();
	if(mbCullingOnPositiveSide)
		return avPos.v[mNormal] >= fHeight;
	else
		return avPos.v[mNormal] <= fHeight;
}
*/
//------------------------------------------------------------------

void cEdClipPlane::SetCullingOnPositiveSide(bool abX)
{
	if(mbCullingOnPositiveSide==abX)
		return;

	mbCullingOnPositiveSide = abX;

	OnPlaneModified();
}

//------------------------------------------------------------------

void cEdClipPlane::Draw(cRendererCallbackFunctions* apFunctions, const cVector3f& avPos)
{
	if(IsActive()==false) return;

	cVector3f vPlaneNormal = GetPlane().GetNormal();
	cVector3f vCenter = vPlaneNormal*GetHeight();
	float fSign[] = { -1, 1 };
	//vCenter + vPlaneNormal*3*fSign[mbCullingOnPositiveSide]
	apFunctions->GetLowLevelGfx()->DrawBoxMinMax(GetProjectedPosOnPlane(vCenter-20), GetProjectedPosOnPlane(vCenter+20), cColor(1,1));
	//cEdHelper::DrawPyramid(apFunctions, vCenter, vCenter + vPlaneNormal*fSign[mbCullingOnPositiveSide]*5, 1, cColor(1,0,0,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vCenter, vCenter + vPlaneNormal*fSign[mbCullingOnPositiveSide]*5, cColor(1,1));
}

//------------------------------------------------------------------

iEdAction* cEdClipPlane::CreateSetHeightsAction(const cVector3f& avX)
{
	return hplNew(cEdActionClipPlaneSetHeight,(this, avX));
}

iEdAction* cEdClipPlane::CreateSetNormalAction(int alNormal)
{
	return hplNew(cEdActionClipPlaneSetNormal,(this, alNormal));
}

iEdAction* cEdClipPlane::CreateSetActiveAction(bool abX)
{
	return hplNew(cEdActionClipPlaneSetActive,(this, abX));
}

iEdAction* cEdClipPlane::CreateSetCullSideAction(bool abX)
{
	return hplNew(cEdActionClipPlaneSetCullingOnPositiveSide,(this, abX));
}

//------------------------------------------------------------------

void cEdClipPlane::SetActive(bool abX)
{
	if(mbActive==abX)
		return;

	mbActive = abX;

	OnPlaneModified();
}

//------------------------------------------------------------------

void cEdClipPlane::Load(cXmlElement* apElement)
{
	SetActive(apElement->GetAttributeBool("Active"));
	SetCullingOnPositiveSide(apElement->GetAttributeBool("CullPos"));
	SetHeights(apElement->GetAttributeVector3f("Heights"));
	SetPlaneNormal(GetPlaneNormalFromString(apElement->GetAttributeString("Plane")));
}

//------------------------------------------------------------------

void cEdClipPlane::Save(cXmlElement* apElement)
{
	cXmlElement* pPlane = apElement->CreateChildElement("ClipPlane");
	
	pPlane->SetAttributeBool("Active", IsActive());
	pPlane->SetAttributeBool("CullPos", GetCullingOnPositiveSide());
	pPlane->SetAttributeVector3f("Heights", GetHeights());
	pPlane->SetAttributeString("Plane", cString::To8Char(GetPlaneString()));
}

//------------------------------------------------------------------
//------------------------------------------------------------------

void cEdClipPlane::OnPlaneModified()
{
	mpMap->GetEditor()->BroadcastModuleMsg(eEdModuleMsg_WorldParamsChange, NULL);
	//mpMap->SetClipPlanesUpdated();
}

//------------------------------------------------------------------


