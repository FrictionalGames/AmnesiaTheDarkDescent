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

#ifndef ED_CLIP_PLANE_H
#define ED_CLIP_PLANE_H

#include "../Common/EdAxisAlignedPlane.h"

//------------------------------------------------------------------

class iEdScnWorld;
class cEdClipPlane;

//------------------------------------------------------------------

typedef std::list<cEdClipPlane*> tClipPlaneList;
typedef tClipPlaneList::iterator tClipPlaneListIt;

typedef std::map<int, cEdClipPlane*>	tClipPlaneMap;
typedef tClipPlaneMap::iterator			tClipPlaneMapIt;

//------------------------------------------------------------------

class cEdClipPlane : public iEdAxisAlignedPlane
{
public:
	cEdClipPlane(iEdScnWorld*, int);

	iEdScnWorld* GetMap() { return mpMap; }
	int GetID() { return mlID; }

	//bool PointIsOnCullingSide(const cVector3f& avPos);

	void SetCullingOnPositiveSide(bool abX);
	bool GetCullingOnPositiveSide() { return mbCullingOnPositiveSide; }

	void Draw(cRendererCallbackFunctions* apFunctions, const cVector3f& avPos);

	void SetActive(bool abX);
	bool IsActive() { return mbActive; }

	void Load(cXmlElement* apElement);
	void Save(cXmlElement* apElement);

	iEdAction* CreateSetHeightsAction(const cVector3f&);
	iEdAction* CreateSetNormalAction(int);
	iEdAction* CreateSetActiveAction(bool);
	iEdAction* CreateSetCullSideAction(bool);

protected:
	void OnPlaneModified();

	iEdScnWorld* mpMap;
	int mlID;
	bool mbCullingOnPositiveSide;
	bool mbActive;
};

//------------------------------------------------------------------

#endif	// ED_CLIP_PLANE_H
