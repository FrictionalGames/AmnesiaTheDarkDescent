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

#ifndef HPLEDITOR_ENTITY_WRAPPER_FACTORY_H
#define HPLEDITOR_ENTITY_WRAPPER_FACTORY_H

#include "../common/StdAfx.h"

#include "EditorTypes.h"

using namespace hpl;

//-----------------------------------------------------------------------

class iEntityWrapper;

class iEditorWorld;

enum eEditorEntityType;
enum eEditorEntityLightType;

//-----------------------------------------------------------------------

class cEntityWrapperFactory
{
public:

	static iEntityWrapper* CreateEntityWrapper(iEditorWorld* apEditorWorld, eEditorEntityType aType, 
											   int alID, const tString& asName, 
											   const tString& asFilename="",
											   const cVector3f& avPosition=0,
											   const cVector3f& avRotation=0,
											   const cVector3f& avScale=1,
											   int alEntitySubType=-1);

	static iEntityWrapper* CreateEntityWrapperPrimitive(iEditorWorld* apEditorWorld, int alID, const tString& asName, tVector3fList& alstParameters);

	static iEntityWrapper* CreateEntityWrapperFromEntityWrapperData(iEditorWorld* apEditorWorld, iEntityWrapperData* apData);

	static iEntityWrapper* CreateEntityWrapperFromXMLElement(iEditorWorld* apEditorWorld, cXmlElement* apXmlData);
	
	///////////////////////////////////////////////////
	// Destruction
	static void DestroyEntityWrapper(iEntityWrapper* apEntity, bool abRemoveFromWorld=true);
private:


	static void SetUpEntityWrapper(iEntityWrapper* apEntity,
								   const cVector3f& avPosition,
								   const cVector3f& avRotation,
								   const cVector3f& avScale);

	static bool CheckIsCreated(iEntityWrapper* apEntity);
};

//-----------------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_FACTORY_H
