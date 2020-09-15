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

#ifndef ED_WORLD_H
#define ED_WORLD_H

#include "../common/StdAfx.h"

using namespace std;
using namespace hpl;

//-------------------------------------------------------------

class iEditor;
class iTypeNode;
class iEdWorld;

//-------------------------------------------------------------

typedef std::vector<iTypeNode*> tTypeNodeVec;

//-------------------------------------------------------------

/**
 * \ingroup leveleditor
 *
 *
 *
 * \version 1.0
 * first version
 *
 * \date 10-27-2010
 *
 * \author Luis
 *
 */
class iTypeNode
{
public:
	iTypeNode(iEdWorld*, int, const tWString&, iTypeNode*);

	bool Init();


	/**
	 * Returns the type name
	 */
	const tWString& GetName() { return msName; }
	/**
	 * Returns the type's full name, in the form "supertype1name.supertype2name. ... supertypeNname.typename" 
	 */
	tWString GetFullName();

	/**
	 *	Returns true if the current type or one of its supertypes matches the criteria.
	 */
	bool IsA(int);
	bool IsA(const tWString&);

	/**
	 *	Gets the type matching the name given.
	 *  This should spread through the whole type hierarchy until the type is found.
	 */
	virtual iTypeNode* GetTypeByName(const tWString&);
	/**
	 *	Gets the type matching the ID given. 
	 *  This should spread through the whole type hierarchy until the type is found.
	 */
	virtual iTypeNode* GetTypeByID(int);
	/**
	 *	Gets the most appropriate type to load the given XML element. 
	 *  This should spread through the whole type hierarchy until the type is found.
	 */
	virtual iTypeNode* GetTypeByXmlElement(cXmlElement*);

protected:
	bool AddChildType(iTypeNode*);

	/////////////////////////////////////////////////
	// Interface for adding properties to a type / parameters yet to be decided
	void AddInt();
	void AddFloat();
	void AddBool();
	void AddStr();
	void AddColor();
	void AddVec2f();
	void AddVec3f();
	void AddTexture();
	void AddMaterial();
	void AddSound();
	void AddMesh();


	/////////////////////////////////////////////////
	// Interface for adding properties to a type
	iEdWorld* mpWorld;
	int mlID;
	tWString msName;

	iTypeNode* mpParentType;
	tTypeNodeVec mvChildTypes;
};

//-------------------------------------------------------------

/**
 * \ingroup leveleditor
 *
 *
 *
 * \version 1.0
 * first version
 *
 * \date 10-27-2010
 *
 * \author Luis
 *
 */
class iEdSessionData
{
public:
	virtual bool Save(cXmlElement*);
	virtual bool Load(cXmlElement*);
protected:
};

/**
 * \ingroup leveleditor
 *
 *
 *
 * \version 1.0
 * first version
 *
 * \date 10-26-2010
 *
 * \author Luis
 *
 */
class iEdWorld
{
public:
	iEdWorld(iEditor*, const tWString&, const tWString&, iTypeNode*);

	iTypeNode* GetRootType() { return mpRootType; }

	iTypeNode* GetTypeByName(const tWString&);
	iTypeNode* GetTypeByID(int);
	iTypeNode* GetTypeByXmlElement(cXmlElement*);

	virtual bool LoadFromFile()=0;
	virtual bool SaveToFile()=0;

protected:
	iEditor* mpEditor;

	tWString msName;
	tWString msFileExt;

	iTypeNode* mpRootType;
};

//-------------------------------------------------------------

#endif // ED_WORLD_H
