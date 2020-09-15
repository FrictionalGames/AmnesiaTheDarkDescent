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

#include "EditorClipPlane.h"
#include "EntityWrapper.h"
#include "EditorWorld.h"
#include "EditorHelper.h"

//------------------------------------------------------------------

cEditorClipPlane::cEditorClipPlane(iEditorWorld* apWorld)
{
	mpWorld = apWorld;
	mbCullingOnPositiveSide = true;
	mbActive = true;
}

//------------------------------------------------------------------
//------------------------------------------------------------------

bool cEditorClipPlane::PointIsOnCullingSide(const cVector3f& avPos)
{
	float fHeight = GetHeight();
	if(mbCullingOnPositiveSide)
	{
		return avPos.v[mNormal] >= fHeight;
	}
	else
		return avPos.v[mNormal] <= fHeight;
}

//------------------------------------------------------------------

void cEditorClipPlane::SetCullingOnPositiveSide(bool abX)
{
	if(mbCullingOnPositiveSide==abX)
		return;

	mbCullingOnPositiveSide = abX;

	OnPlaneModified();
}

//------------------------------------------------------------------

void cEditorClipPlane::Draw(cRendererCallbackFunctions* apFunctions, const cVector3f& avPos)
{
	if(IsActive()==false) return;

	cVector3f vPlaneNormal = GetPlane().GetNormal();
	cVector3f vCenter = vPlaneNormal*GetHeight();
	float fSign[] = { -1, 1 };
	//vCenter + vPlaneNormal*3*fSign[mbCullingOnPositiveSide]
	apFunctions->GetLowLevelGfx()->DrawBoxMinMax(GetProjectedPosOnPlane(vCenter-20), GetProjectedPosOnPlane(vCenter+20), cColor(1,1));
	//cEditorHelper::DrawPyramid(apFunctions, vCenter, vCenter + vPlaneNormal*fSign[mbCullingOnPositiveSide]*5, 1, cColor(1,0,0,1));
	apFunctions->GetLowLevelGfx()->DrawLine(vCenter, vCenter + vPlaneNormal*fSign[mbCullingOnPositiveSide]*5, cColor(1,1));
}

//------------------------------------------------------------------

void cEditorClipPlane::SetActive(bool abX)
{
	if(mbActive==abX)
		return;

	mbActive = abX;

	OnPlaneModified();
}

//------------------------------------------------------------------

void cEditorClipPlane::Load(cXmlElement* apElement)
{
	SetActive(apElement->GetAttributeBool("Active"));
	SetCullingOnPositiveSide(apElement->GetAttributeBool("CullPos"));
	SetHeights(apElement->GetAttributeVector3f("Heights"));
	SetPlaneNormal(GetPlaneNormalFromString(apElement->GetAttributeString("Plane")));
}

//------------------------------------------------------------------

void cEditorClipPlane::Save(cXmlElement* apElement)
{
	apElement->SetValue("ClipPlane");
	apElement->SetAttributeBool("Active", IsActive());
	apElement->SetAttributeBool("CullPos", GetCullingOnPositiveSide());
	apElement->SetAttributeVector3f("Heights", GetHeights());
	apElement->SetAttributeString("Plane", cString::To8Char(GetPlaneString()));
}

//------------------------------------------------------------------
//------------------------------------------------------------------

void cEditorClipPlane::OnPlaneModified()
{
	mpWorld->SetClipPlanesUpdated();
}

//------------------------------------------------------------------


