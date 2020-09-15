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

#include "EditorActionsBodies.h"

#include "EditorBaseClasses.h"
#include "EditorGrid.h"

#include "EntityWrapperBodyShape.h"
#include "EntityWrapperBody.h"
#include "EntityWrapperJoint.h"
#include "EntityWrapperJointBall.h"
#include "EntityWrapperJointHinge.h"
#include "EntityWrapperJointScrew.h"
#include "EntityWrapperJointSlider.h"

#include "EditorWorld.h"

#include "EditorSelection.h"


cEditorActionBodyAddShapes::cEditorActionBodyAddShapes(iEditorWorld* apWorld, int alBodyID, const tIntList& alstShapeIDs) : iEditorActionAggregateAddComponents(apWorld, alBodyID, alstShapeIDs)
{
}

//-------------------------------------------------------------

iEntityWrapperAggregate* cEditorActionBodyAddShapes::GetAggregateFromEntity(iEntityWrapper* apEntity)
{
	return ((cEntityWrapperBodyShape*)apEntity)->GetParentBody();
}

//-------------------------------------------------------------

///////////////////////////////////////////////////////////////
// DETACH BODY SHAPES
///////////////////////////////////////////////////////////////

//-------------------------------------------------------------

cEditorActionBodyRemoveShapes::cEditorActionBodyRemoveShapes(iEditorWorld* apEditorWorld, 
															 const tIntList& alstBodyShapeIDs) : iEditorActionAggregateRemoveComponents(apEditorWorld, alstBodyShapeIDs)
{
/*	mlstBodyShapeIDs = alstBodyShapeIDs;

	tIntListIt it = mlstBodyShapeIDs.begin();
	for(;it!=mlstBodyShapeIDs.end();++it)
	{
		int lID = *it;

		cEntityWrapperBodyShape* pShape = (cEntityWrapperBodyShape*)mpEditorWorld->GetEntity(lID);
		if(pShape)
		{
			int lBodyID = -1;
			if(pShape->GetParentBody())
				lBodyID = pShape->GetParentBody()->GetID();
			mlstBodyIDs.push_back(lBodyID);
		}
	}
	*/
}

//-------------------------------------------------------------

/*
void cEditorActionBodyShapeDetachFromBody::DoModify()
{
	tIntListIt it = mlstBodyShapeIDs.begin();
	for(;it!=mlstBodyShapeIDs.end();++it)
	{
		int lID = *it;
		cEntityWrapperBodyShape* pShape = (cEntityWrapperBodyShape*)mpEditorWorld->GetEntity(lID);

		cEntityWrapperBody* pBody = pShape->GetParentBody();
		if(pBody)
		{
			pBody->RemoveShape(pShape);
			if(pBody->HasNoShapes())
			{
				mpEditorWorld->DestroyEntityWrapper(pBody);
			}
		}
	}
}

//-------------------------------------------------------------

void cEditorActionBodyShapeDetachFromBody::UndoModify()
{
	tIntListIt it = mlstBodyShapeIDs.begin();
	tIntListIt itBodies = mlstBodyIDs.begin();
	for(;it!=mlstBodyShapeIDs.end();++it, ++itBodies)
	{
		int lID = *it;
		int lBodyID = *itBodies;

		if(lBodyID>=0)
		{
			cEntityWrapperBody* pBody = (cEntityWrapperBody*)mpEditorWorld->GetEntity(lBodyID);
			if(pBody==NULL)
			{
//				pBody = (cEntityWrapperBody*) mpEditorWorld->CreateEntityWrapper(eEditorEntityType_Body, lBodyID, "");
			}

			cEntityWrapperBodyShape* pShape = (cEntityWrapperBodyShape*)mpEditorWorld->GetEntity(lID);
//			pBody->AddShape(pShape);
		}
	}
}
*/

iEntityWrapperAggregate* cEditorActionBodyRemoveShapes::GetAggregateFromEntity(iEntityWrapper* apEntity)
{
	return ((cEntityWrapperBodyShape*)apEntity)->GetParentBody();
}

//-------------------------------------------------------------
//-------------------------------------------------------------

//-------------------------------------------------------------

///////////////////////////////////////////////////////////////
// REMOVE SHAPES FROM BODY
///////////////////////////////////////////////////////////////

//-------------------------------------------------------------
/*
cEditorActionBodyRemoveShapes::cEditorActionBodyRemoveShapes(int alID, iEditorWorld* apEditorWorld, const tIntList& alstBodyShapeIDs) : iEditorActionWorldModifier("Remove shapes", apEditorWorld)
{
	mlID = alID;
	mlstBodyShapeIDs = alstBodyShapeIDs;
}

//-------------------------------------------------------------

void cEditorActionBodyRemoveShapes::DoModify()
{
	cEntityWrapperBody* pBody = (cEntityWrapperBody*)mpEditorWorld->GetEntity(mlID);

	tIntList::const_iterator it = mlstBodyShapeIDs.begin();

	for(;it!=mlstBodyShapeIDs.end();++it)
	{
		cEntityWrapperBodyShape* pShape = (cEntityWrapperBodyShape*)mpEditorWorld->GetEntity(*it);
        pBody->RemoveShape(pShape);
	}

	/////////////////////
	// Remove body if no shapes left
	if(pBody->HasNoShapes())
	{
		mpEditorWorld->DestroyEntityWrapper(pBody);
	}
	else
	{
//		pBody->UpdateGlobalBoundingVolume();
	}
}

//-------------------------------------------------------------

void cEditorActionBodyRemoveShapes::UndoModify()
{
	cEntityWrapperBody* pBody = (cEntityWrapperBody*)mpEditorWorld->GetEntity(mlID);
	if(pBody==NULL)
	{
		//pBody = (cEntityWrapperBody*)mpEditorWorld->CreateEntityWrapper(eEditorEntityType_Body,mlID, "");
	}

	tIntList::const_iterator it = mlstBodyShapeIDs.begin();

	for(;it!=mlstBodyShapeIDs.end();++it)
	{
		cEntityWrapperBodyShape* pShape = (cEntityWrapperBodyShape*)mpEditorWorld->GetEntity(*it);
       // pBody->AddShape(pShape);
	}

	//pBody->UpdateGlobalBoundingVolume();
}

*/
