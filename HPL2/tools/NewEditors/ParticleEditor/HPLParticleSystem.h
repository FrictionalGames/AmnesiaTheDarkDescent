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

#ifndef ED_HPL_PARTICLE_SYSTEM_H
#define ED_HPL_PARTICLE_SYSTEM_H

//----------------------------------------------------------

#include "../Common/EdWorld.h"

typedef std::vector<cMeshEntity*> tMeshEntityVec;
typedef std::vector<iPhysicsBody*> tPhysicsBodyVec;

//----------------------------------------------------------

class cEdPSRoot : public iEdObjectType
{
public:
	cEdPSRoot();

protected:
	iEdObjectData* CreateTypeSpecificData();
};

//----------------------------------------------------------

class cPESessionData : public iEdSessionData
{
public:
	cPESessionData(iEdWorld*);

	bool Load(cXmlElement*);
	bool Save(cXmlElement*);
};

//----------------------------------------------------------

class cHplParticleSystem : public iEdWorld
{
public:
	cHplParticleSystem(iEditor*);
	~cHplParticleSystem();

	void SetShowWalls(bool);
	void SetShowFloor(bool);
	
	bool GetShowWalls() { return mbShowWalls; }
	bool GetShowFloor() { return mbShowFloor; }

	void AddBGColor(const tWString&, const cColor&);
	void SetSelectableBGColor(int);
	int GetSelectableBGColorNum() { return (int)mvBGColors.size(); }
	const tWString&	 GetSelectableBGColorName(int alIdx) { return mvBGColorNames[alIdx]; }
	const cColor& GetSelectableBGColor(int alIdx) { return mvBGColors[alIdx]; }

	bool LoadFromFile(const tWString&);

protected:
	iEdSessionData* CreateSessionData();

	void OnInit();
	void OnReset();
	void OnUpdate();

	void OnWorldAddRemObject();
	void OnWorldObjModify();

	bool LoadContentsFromXmlDoc(iXmlDocument*);
	bool SaveContentsToXmlDoc(iXmlDocument*);

	void UpdatePreviewPS();
	bool CheckDataIsValid();

	bool mbShowWalls;
	bool mbShowFloor;

	iPhysicsWorld*		mpPhysicsWorld;

	bool				mbLoading;

	bool				mbPreviewPSUpdated;
	float				mfPreviewPSPollTimer;
	iXmlDocument*		mpPreviewPSData;
	cParticleSystem*	mpPreviewPS;

	tMeshEntityVec		mvWalls;
	tPhysicsBodyVec		mvWallBodies;

	tWStringVec			mvBGColorNames;
	tColorVec			mvBGColors;
};

//----------------------------------------------------------

#endif // ED_HPL_PARTICLE_SYSTEM_H
