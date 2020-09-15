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

#ifndef HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_BOX_PARTICLE_SYSTEM_H
#define HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_BOX_PARTICLE_SYSTEM_H

#include "../common/StdAfx.h"
using namespace hpl;

#include "EditorWindowEntityEditBox.h"

//----------------------------------------------------------------------

class iEditorBase;

class cEditorEditModeSelect;
class cEntityWrapperParticleSystem;

//----------------------------------------------------------------------

class cEditorWindowEntityEditBoxParticleSystem : public cEditorWindowEntityEditBox
{
public:
	cEditorWindowEntityEditBoxParticleSystem(cEditorEditModeSelect* apEditMode,cEntityWrapperParticleSystem* apPS);
	virtual ~cEditorWindowEntityEditBoxParticleSystem();

	void Create();
protected:
	void AddPropertySetPS(cWidgetTab* apParentTab);

	void OnUpdate(float afTimeStep);

	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);

	bool BrowseButton_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(BrowseButton_OnPressed);

	bool FilePSCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(FilePSCallback);

	bool WindowSpecificInputCallback(iEditorInput* apInput);

	tWStringVec mvLoadedFiles;
	tWString msLastPath;

	cWidgetTab* mpTabGeneral;
	cWidgetTab* mpTabPS;

	cEntityWrapperParticleSystem* mpEntity;

	cWidgetLabel* mpLabelFile;
	cWidgetTextBox* mpInputFile;
	cWidgetButton* mpButtonPSBrowse;

	cEditorInputBool* mpInpFadeAtDistance;
	cEditorInputColorFrame* mpInpColor;
	cEditorInputNumber* mpInpMinFadeDistanceEnd;
	cEditorInputNumber* mpInpMinFadeDistanceStart;
	cEditorInputNumber* mpInpMaxFadeDistanceStart;
	cEditorInputNumber* mpInpMaxFadeDistanceEnd;
};

//----------------------------------------------------------------------


#endif // HPLEDITOR_EDITOR_WINDOW_ENTITY_EDIT_BOX_PARTICLE_SYSTEM_H
