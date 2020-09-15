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

#ifndef ED_HPL_ENTITY_ACTIONS_H
#define ED_HPL_ENTITY_ACTIONS_H

#include "../Common/EdAction.h"
#include "../Common/EdVar.h"
#include "../ModelEditor/HPLEntity.h"

class cHplEntity;
class cEdUserClass;

class cEdActionHplEntitySetClass : public iEdActionWorldModifier
{
public:
	cEdActionHplEntitySetClass(cHplEntity* apEnt, cEdUserClass* apClass);

	bool Init();

protected:
	void DoModify();
	void UndoModify();

	cEdUserClass* mpClass;
	cEdUserClass* mpOldClass;

	tVarValueMap mmapOldValues;
};

class cEdActionHplEntitySetVarValue : public iEdActionWorldModifier
{
public:
	cEdActionHplEntitySetVarValue(cHplEntity* apEnt, const tWString& asVarName, const tWString& asValue);

	bool Init();

protected:
	void DoModify();
	void UndoModify();
	void Apply(const tWString&);

	tWString msVarName;
	tWString msValue;
	tWString msOldValue;
};

class cEdActionHplEntitySetAnimations : public iEdActionWorldModifier
{
public:
	cEdActionHplEntitySetAnimations(cHplEntity* apEnt, const tAnimWrapperVec& avAnimations);

	bool Init();

protected:
	void DoModify();
	void UndoModify();
	void Apply(const tAnimWrapperVec&);

	tAnimWrapperVec mvOldAnims;
	tAnimWrapperVec mvNewAnims;
};


class cEdActionHplEntityImportMesh : public iEdActionWorldModifier
{
public:
	cEdActionHplEntityImportMesh(iEdWorld*, const tString& asFilename);

	bool Init();

protected:
	void DoModify();
	void UndoModify();

	void Apply(const tString&, const tIntList&, const tIntList&);

	tString msOldMeshFilename;
	tString msNewMeshFilename;

	tIntList mlstSubMeshIDs;
	tIntList mlstOldSubMeshIDs;

	tIntList mlstBoneIDs;
	tIntList mlstOldBoneIDs;

	tIntList mlstOldSelectedIDs;
};


#endif // ED_HPL_ENTITY_ACTIONS_H
