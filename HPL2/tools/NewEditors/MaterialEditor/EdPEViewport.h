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

#ifndef ED_PE_VIEWPORT_H
#define ED_PE_VIEWPORT_H

#include "../common/StdAfx.h"

using namespace hpl;

#include "../Common/EdViewport.h"

//--------------------------------------------------------------------

class iEditor;

class cEdGrid;
class cDrawGridCallback;
class cEdWindowViewport;

//--------------------------------------------------------------------

////////////////////////////////////////////////
// VIEWPORT WINDOW
////////////////////////////////////////////////

//--------------------------------------------------------------------

class cEdPEViewport : public iEdViewport
{
public:
	cEdPEViewport(iEditor* apEditor, iFrameBuffer* apFB=NULL);
	~cEdPEViewport();

	void SetDrawGrid(bool abX) { mbDrawGrid = abX; }
	bool GetDrawGrid() { return mbDrawGrid; }

	void SetDrawAxes(bool abX) { mbDrawAxes = abX; }
	bool GetDrawAxes() { return mbDrawAxes; }

	void SetDrawDebug(bool abX) { mbDrawDebug = abX; }
	bool GetDrawDebug() { return mbDrawDebug; }

	void Load(cXmlElement* apElement);
	void Save(cXmlElement* apElement);

protected:
	///////////////////////////
	// Own functions

	// iEditorViewport Callbacks implementation
	bool OnViewportUpdate(const cGuiMessageData& aData);
	bool OnViewportGotFocus(const cGuiMessageData& aData);
	bool OnViewportLostFocus(const cGuiMessageData& aData);

	bool OnViewportDraw(const cGuiMessageData& aData);
	void OnPostSolidDraw(cRendererCallbackFunctions*);

	bool OnViewportMouseMove(const cGuiMessageData& aData);
	bool OnViewportMouseDown(const cGuiMessageData& aData);
	bool OnViewportMouseUp(const cGuiMessageData& aData);
	bool OnViewportKeyPress(const cGuiMessageData& aData);
	bool OnViewportKeyRelease(const cGuiMessageData& aData);
	
	void OnSetCamMode(iCamMode*);

	void UpdateMenu();

    ///////////////////////////
	// Implemented Functions
	void OnCreateLayout();
	void OnReset();
	void OnUpdate();

	void OnSetSize(const cVector2f&);
	void OnSetActive(bool abX);

	bool WindowSpecificInputCallback(iEdInput*) { return true; }

	///////////////////////////////////
	// Data
	iWidget* mpPrevAttention;

	bool mbDrawGrid;
	bool mbDrawDebug;
	bool mbDrawAxes;

	// Layout specific
	cWidgetLabel* mpPresetLabel;

	cGuiGfxElement* mpGfxRedFrame;
	cGuiGfxElement* mpGfxPointerTumble;
	cGuiGfxElement* mpGfxPointerTrack;
	cGuiGfxElement* mpGfxPointerZoom;
};

//---------------------------------------------------------

#endif //ED_PE_VIEWPORT_H

