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

#ifndef ED_SHAPE_CREATOR_H
#define ED_SHAPE_CREATOR_H

#include "../Common/StdAfx.h"
using namespace hpl;

class iEditor;
class iEdViewport;

class cViewportClick;


class iGuiTool
{
public:
	iGuiTool(iEditor*);

	virtual bool IsBusy()=0;

	void SetActive(bool);

	virtual void OnSetActive(bool);

	virtual void OnViewportMouseDown(const cViewportClick&)=0;
	virtual void OnViewportMouseUp(const cViewportClick&)=0;

	virtual void Draw(iEdViewport*, cRendererCallbackFunctions*)=0;

	virtual void OnUpdate()=0;

};

class iShapeCreator
{
public:
	iShapeCreator(iEditor* apEditor) : mpEditor(apEditor), mbCanDrag(true) {}
	void SetEditor(iEditor* apEditor) { mpEditor = apEditor; }

	virtual void OnViewportMouseDown(const cViewportClick&)=0;
	virtual void OnViewportMouseUp(const cViewportClick&)=0;

	virtual void Draw(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions)=0;

	virtual void OnUpdate()=0;

	virtual bool IsDoneCreating()=0;
	virtual void Reset()=0;

	void SetDraggingEnabled(bool abX) { mbCanDrag = abX; }
	bool IsDraggingEnabled() { return mbCanDrag; }

protected:
	void PushPoint(const cVector3f&);
	void PopPoint();
	void StoreMousePosition(const cViewportClick&);
	//virtual void OnStoreMousePosition(const cViewportClick&)=0;

	iEditor* mpEditor;
	bool mbCanDrag;
	tVector3fVec mvPoints;
	cVector3f mvTempPoint;

	cVector3f mvDebugPos;
};

#endif // ED_SHAPE_CREATOR_H
