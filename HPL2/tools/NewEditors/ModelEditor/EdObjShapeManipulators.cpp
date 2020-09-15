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

#include "../ModelEditor/EdObjShapeManipulators.h"
#include "../Common/EdSelection.h"

#include "../ModelEditor/EdObjShape.h"

//-----------------------------------------------------------------------------------

iShapeSizeManipulator::iShapeSizeManipulator(const tWString& asName) : cScaleManipulator(asName)
{
}

iEdAction* iShapeSizeManipulator::CreateAction()
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpTargetObject);
	iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

	return pShape->CreateSetVector3fAction(eShapeVec3f_Size, mvTempScale);
}

//-----------------------------------------------------------------------------------

cRadiusManipulator::cRadiusManipulator() : iShapeSizeManipulator(_W("ShapeRadius"))
{
}

void cRadiusManipulator::Init()
{
	AddComponent(hplNew(cScaleAxis,(this, cMath::MatrixRotateZ(cMath::ToRad(-90)))));
}

void cRadiusManipulator::StoreOldTransform()
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpTargetObject);
	iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

	mvOldScale = pShape->GetSize();
}

void cRadiusManipulator::StoreCurrentTransform()
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpTargetObject);
	iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

	mvTempScale = pShape->GetSize();
}

void cRadiusManipulator::SetTransform()
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpTargetObject);
	iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

	pShape->SetSize(cVector3f(mvTempScale.x,0,0));
}

void cRadiusManipulator::FallToOldTransform()
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpTargetObject);
	iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

	pShape->SetSize(cVector3f(mvOldScale.x,0,0));
}

//-----------------------------------------------------------------------------------

cRadiusHeightManipulator::cRadiusHeightManipulator() : iShapeSizeManipulator(_W("ShapeRadiusHeight"))
{
}

void cRadiusHeightManipulator::Init()
{
	AddComponent(hplNew(cScaleAxis,(this, cMath::MatrixRotateZ(cMath::ToRad(-90)))));
	AddComponent(hplNew(cScaleAxis,(this, cMatrixf::Identity)));
}

void cRadiusHeightManipulator::StoreOldTransform()
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpTargetObject);
	iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

	mvOldScale = pShape->GetSize();
}

void cRadiusHeightManipulator::StoreCurrentTransform()
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpTargetObject);
	iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

	mvTempScale = pShape->GetSize();
}

void cRadiusHeightManipulator::SetTransform()
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpTargetObject);
	iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

	pShape->SetSize(cVector3f(mvTempScale.x,mvTempScale.y,0));
}

void cRadiusHeightManipulator::FallToOldTransform()
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpTargetObject);
	iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

	pShape->SetSize(cVector3f(mvOldScale.x,mvOldScale.y,0));
}

//-----------------------------------------------------------------------------------

cSizeManipulator::cSizeManipulator() : iShapeSizeManipulator(_W("ShapeSize"))
{
}

void cSizeManipulator::StoreOldTransform()
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpTargetObject);
	iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

	mvOldScale = pShape->GetSize();
}

void cSizeManipulator::StoreCurrentTransform()
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpTargetObject);
	iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

	mvTempScale = pShape->GetSize();
}

void cSizeManipulator::SetTransform()
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpTargetObject);
	iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

	pShape->SetSize(mvTempScale);
}

void cSizeManipulator::FallToOldTransform()
{
	cEdSelection* pSelection = static_cast<cEdSelection*>(mpTargetObject);
	iEdObjShape* pShape = static_cast<iEdObjShape*>(pSelection->GetComponent(0));

	pShape->SetSize(mvOldScale);
}

//-----------------------------------------------------------------------------------