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

#include "../ModelEditor/EdObjShapeCapsule.h"

#include "../Common/Editor.h"

#include "../ModelEditor/EdObjShapeManipulators.h"

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// TYPE SHAPE - CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cTypeShapeCapsule::cTypeShapeCapsule() : iShapeSubType(_W("Capsule"))
{
	mbScalable = false;
	AddVec3f(eShapeVec3f_Size, "ShapeSize", cVector3f(1,1,0), ePropStep_PreCreate, false);
}

cTypeShapeCapsule::~cTypeShapeCapsule()
{
}

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// TYPE SHAPE - PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// TYPE SHAPE - PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void cTypeShapeCapsule::SetUpManipulators()
{
	iEdScnObjType::SetUpManipulators();
	AddManipulator(hplNew(cRadiusHeightManipulator,()));
}

iEdObjectData* cTypeShapeCapsule::CreateTypeSpecificData()
{
	return hplNew(cEdObjShapeCapsuleData,(this));
}

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SHAPE DATA - CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjShapeCapsuleData::cEdObjShapeCapsuleData(iEdObjectType* apType) : iEdObjShapeData(apType)
{
}

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SHAPE DATA - PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SHAPE DATA - PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

iEdObject* cEdObjShapeCapsuleData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjShapeCapsule,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SHAPE - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

cEdObjShapeCapsule::cEdObjShapeCapsule(iEdObjectData* apData) : iEdObjShape(apData)
{
}

cEdObjShapeCapsule::~cEdObjShapeCapsule()
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SHAPE - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SHAPE - PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cMesh* cEdObjShapeCapsule::CreateShape()
{
	tString sMat = GetShapeMaterialFile(HasParentBody());

	cMesh* pMesh = NULL;
	cMeshCreator* pCreator = GetWorld()->GetEditor()->GetEngine()->GetGraphics()->GetMeshCreator();
	return pCreator->CreateCapsule("", cVector2f(mvShapeSize.x, mvShapeSize.y), iEdObjShape::mlMeshResolution, iEdObjShape::mlMeshResolution, sMat); 
}

//---------------------------------------------------------------------------
