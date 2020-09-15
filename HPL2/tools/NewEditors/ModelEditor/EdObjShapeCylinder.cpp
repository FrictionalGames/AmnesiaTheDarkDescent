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

#include "../ModelEditor/EdObjShapeCylinder.h"
#include "../Common/Editor.h"

#include "../ModelEditor/EdObjShapeManipulators.h"

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// TYPE SHAPE - CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cTypeShapeCylinder::cTypeShapeCylinder() : iShapeSubType(_W("Cylinder"))
{
	mbScalable = false;
	AddVec3f(eShapeVec3f_Size, "ShapeSize", cVector3f(1,1,0), ePropStep_PreCreate, false);
}

cTypeShapeCylinder::~cTypeShapeCylinder()
{
}

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// TYPE SHAPE - PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// TYPE SHAPE - PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

void cTypeShapeCylinder::SetUpManipulators()
{
	iEdScnObjType::SetUpManipulators();
	AddManipulator(hplNew(cRadiusHeightManipulator,()));
}

iEdObjectData* cTypeShapeCylinder::CreateTypeSpecificData()
{
	return hplNew(cEdObjShapeCylinderData,(this));
}

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// SHAPE DATA - CONSTRUCTORS
////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cEdObjShapeCylinderData::cEdObjShapeCylinderData(iEdObjectType* apType) : iEdObjShapeData(apType)
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

iEdObject* cEdObjShapeCylinderData::CreateTypeSpecificObject()
{
	return hplNew(cEdObjShapeCylinder,(this));
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SHAPE - CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

cEdObjShapeCylinder::cEdObjShapeCylinder(iEdObjectData* apData) : iEdObjShape(apData)
												
{
}

cEdObjShapeCylinder::~cEdObjShapeCylinder()
{
}

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// SHAPE - PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------

cMesh* cEdObjShapeCylinder::CreateShape()
{
	tString sMat = GetShapeMaterialFile(HasParentBody());

	cMesh* pMesh = NULL;
	cMeshCreator* pCreator = GetWorld()->GetEditor()->GetEngine()->GetGraphics()->GetMeshCreator();
	return pCreator->CreateCylinder("", cVector2f(mvShapeSize.x, mvShapeSize.y), iEdObjShape::mlMeshResolution, sMat);
}

//---------------------------------------------------------------------------


