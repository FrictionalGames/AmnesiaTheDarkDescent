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

#include "../Common/EdEditModeLights.h"

#include "../Common/Editor.h"
#include "../Common/EdActionHandler.h"
#include "../Common/EdWorld.h"


#include "../Common/EdWindowViewport.h"
#include "../Common/EdWindowLights.h"

#include "../Common/ShapeCreator.h"

#include "../Common/EdObjLight.h"

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

cEdEditModeLights::cEdEditModeLights(iEditor* apEditor) : iEdEditModeScnObjCreator(apEditor, _W("Lights"))
{
	mpCurrentCreator = NULL;
}

//-----------------------------------------------------------------

cEdEditModeLights::~cEdEditModeLights()
{
}

//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

void cEdEditModeLights::DrawObjectPreview(iEdViewport* apViewport, cRendererCallbackFunctions *apFunctions, const cMatrixf& amtxTransform, bool abPreCreationActive)
{
	if(mpCurrentCreator) mpCurrentCreator->Draw(apViewport, apFunctions);
}

void cEdEditModeLights::OnViewportMouseDown(const cViewportClick& aClick)
{
	iEdEditModeObjectCreator::OnViewportMouseDown(aClick);
	if(mpCurrentCreator)
		mpCurrentCreator->OnViewportMouseDown(aClick);
}

void cEdEditModeLights::OnViewportMouseUp(const cViewportClick& aClick)
{
	if(mpCurrentCreator)
	{
		mpCurrentCreator->OnViewportMouseUp(aClick);
		if(mpCurrentCreator->IsDoneCreating())
		{
			iEdEditModeObjectCreator::OnViewportMouseUp(aClick);
			mpCurrentCreator->Reset();
		}
	}
	else
		iEdEditModeObjectCreator::OnViewportMouseUp(aClick);
}

void cEdEditModeLights::OnUpdate()
{
	iEdEditModeObjectCreator::OnUpdate();

	if(mpCurrentCreator) mpCurrentCreator->OnUpdate();
}
//-----------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

/*
bool cEdEditModeLights::SetUpCreationData(iEdObjectData* apData)
{
	if(iEdEditModeObjectCreator::SetUpCreationData(apData)==false)
		return false;

	return true;
}
*/
//-----------------------------------------------------------------

iEdWindow* cEdEditModeLights::CreateWindow()
{
	return hplNew(cEdWindowLights,(this));
}

//-----------------------------------------------------------------

iEdObjectType* cEdEditModeLights::CreateType()
{
	return hplNew(cTypeLight,());
}

//-----------------------------------------------------------------

void cEdEditModeLights::OnSetSubType(int alIdx)
{
	iLightSubType* pSubType = static_cast<iLightSubType*>(GetSelectedType());

	mpCurrentCreator = pSubType->GetShapeCreator();
}


//-----------------------------------------------------------------

