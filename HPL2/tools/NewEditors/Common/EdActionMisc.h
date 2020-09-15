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

#ifndef ED_ACTION_MISC_H
#define ED_ACTION_MISC_H

#include "../Common/EdAction.h"

//--------------------------------------------------------------

////////////////////////////////////////////////////////////
// AA PLANE RELATED
////////////////////////////////////////////////////////////

//--------------------------------------------------------------

class iEdAxisAlignedPlane;

//--------------------------------------------------------------

class iEdActionAxisAlignedPlane : public iEdAction
{
public:
	iEdActionAxisAlignedPlane(iEdAxisAlignedPlane* apPlane);

protected:
	iEdAxisAlignedPlane* mpPlane;
};

class cEdActionPlaneChangeNormal : public iEdActionAxisAlignedPlane
{
public: 
	cEdActionPlaneChangeNormal(iEdAxisAlignedPlane* apPlane,
									int alNormal);

	bool Init() { return mlNewNormal!=mlOldNormal; }

	void Do();
	void Undo();
protected:
	int mlNewNormal;
	int mlOldNormal;
};

class cEdActionPlaneChangeHeight : public iEdActionAxisAlignedPlane
{
public:
	cEdActionPlaneChangeHeight(iEdAxisAlignedPlane* apPlane,
								   const cVector3f& avX);

	bool Init() { return true; }

	void Do();
	void Undo();
protected:
	cVector3f mvNewHeights;
	cVector3f mvOldHeights;
};

//--------------------------------------------------------------

#endif // ED_ACTION_MISC_H
