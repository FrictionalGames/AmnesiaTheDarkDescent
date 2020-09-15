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

#include "../Common/EdActionMisc.h"
#include "../Common/EdAxisAlignedPlane.h"

#include "../Common/EdWorld.h"

//--------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////

//--------------------------------------------------------------

iEdActionAxisAlignedPlane::iEdActionAxisAlignedPlane(iEdAxisAlignedPlane* apPlane) : iEdAction(_W("Change editor plane properties"))
{
	mpPlane = apPlane;
}

//--------------------------------------------------------------

cEdActionPlaneChangeNormal::cEdActionPlaneChangeNormal(iEdAxisAlignedPlane* apPlane, int alNormal) : iEdActionAxisAlignedPlane(apPlane)
{
	mlOldNormal = apPlane->GetPlaneNormal();
	mlNewNormal = alNormal%(int)ePlaneNormal_LastEnum;
}

void cEdActionPlaneChangeNormal::Do()
{
	mpPlane->SetPlaneNormal((ePlaneNormal)mlNewNormal);
}

void cEdActionPlaneChangeNormal::Undo()
{
	mpPlane->SetPlaneNormal((ePlaneNormal)mlOldNormal);
}

//--------------------------------------------------------------

cEdActionPlaneChangeHeight::cEdActionPlaneChangeHeight(iEdAxisAlignedPlane* apPlane, const cVector3f& avHeight) : iEdActionAxisAlignedPlane(apPlane)
{
    mvOldHeights = mpPlane->GetHeights();
	mvNewHeights = avHeight;
}

void cEdActionPlaneChangeHeight::Do()
{
	mpPlane->SetHeights(mvNewHeights);
}

void cEdActionPlaneChangeHeight::Undo()
{
	mpPlane->SetHeights(mvOldHeights);
}

//--------------------------------------------------------------

