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

#ifndef ED_MODULE_H
#define ED_MODULE_H

//-----------------------------------------------

#include "../common/StdAfx.h"

using namespace hpl;
using namespace std;

//-----------------------------------------------

class iEditor;

//-----------------------------------------------

enum eEdModuleMsg
{
	eEdModuleMsg_Init,
	eEdModuleMsg_Reset,
	eEdModuleMsg_Update,
	
	eEdModuleMsg_LoadGlobalCfg,
	eEdModuleMsg_LoadLocalCfg,

	eEdModuleMsg_LoadWorld,
};

//-----------------------------------------------

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
 *
 */
class iEdModule
{
public:
	iEdModule(iEditor*, const tWString&);
	virtual ~iEdModule();
	
	iEditor* GetEditor()				{ return mpEditor; }
	const tWString& GetName()			{ return msName; }

	void RunMessage(eEdModuleMsg, void*);
protected:
	virtual void OnInit()				{}
	virtual void OnReset()				{}
	virtual void OnUpdate()				{}

	virtual void OnLoadGlobalConfig()	{}
	virtual void OnLoadLocalConfig()	{}

	virtual void OnLoadWorld()			{}

	/////////////////////////////////////////
	// Data
	iEditor* mpEditor;
	tWString msName;
};

//-----------------------------------------------

typedef std::vector<iEdModule*> tEdModuleVec;

//-----------------------------------------------

#endif // ED_MODULE_H
              