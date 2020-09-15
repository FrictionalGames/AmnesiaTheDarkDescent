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

#ifndef ED_RESOURCE_H
#define ED_RESOURCE_H

#include "../Common/StdAfx.h"
using namespace hpl;

//-------------------------------------------------------------

class iEdResource
{
public:
	iEdResource(const tWString&);

	const tWString& GetFileName();

	iResourceBase* CreateInstance();

protected:
	tWString msFile;
};

//-------------------------------------------------------------

typedef std::list<iEdResource*>		tEdResourceList;
typedef tEdResourceList::iterator	tEdResourceListIt;

//-------------------------------------------------------------

class iEdResourceCreator
{
public:
	iEdResourceCreator();

	iEdResource* CreateResource(const tWString&);
	iEdResource* GetResource(const tWString&);

protected:
	void AddToList(iEdResource*);
	void Remove(iEdResource*);

	tEdResourceList mlstLoadedResources;
};

//-------------------------------------------------------------

class cEdLoader
{
public:
	static bool LoadTexture(const tString&, eTextureType, iTexture** apTex=NULL);
	static bool LoadTexture1D(const tString&, iTexture** apTex=NULL);
	static bool LoadTexture2D(const tString&, iTexture** apTex=NULL);
	static bool LoadCubeMap(const tString&, iTexture** apTex=NULL);

	static bool LoadAnimTex(const tString&, eTextureType, const tString&, float);

	static bool LoadMaterial(const tString&, cMaterial** apMat=NULL);

	static bool LoadPS(const tString&, cParticleSystem** apPS=NULL);

	static bool LoadSound(const tString&, cSoundEntityData** apSnd=NULL);

	static bool LoadModel(const tString&, cMesh** apMesh=NULL);
	static bool LoadAnimModel(const tString&, cAnimation** apAnim=NULL);

	static bool LoadEntity(const tString&, cMeshEntity* apEnt=NULL);
};

//-------------------------------------------------------------

#endif // ED_RESOURCE_H
