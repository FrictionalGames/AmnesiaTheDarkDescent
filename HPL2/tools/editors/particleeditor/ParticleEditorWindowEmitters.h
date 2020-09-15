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

#ifndef HPLEDITOR_PARTICLE_EDITOR_WINDOW_EMITTERS_H
#define HPLEDITOR_PARTICLE_EDITOR_WINDOW_EMITTERS_H

#include "../common/EditorWindow.h"

class iEditorBase;
class cParticleEditorWindowEmitterParams;
class cEntityWrapperParticleEmitter;


//-------------------------------------------------------------

class cParticleEditorWindowEmitters : public iEditorWindow
{
public:
	cParticleEditorWindowEmitters(iEditorBase* apEditor);
	~cParticleEditorWindowEmitters();

	void SelectEntry(int alIdx);
	void Reset();

protected:
	void OnInitLayout();
	void OnUpdate(float afTimeStep);

	bool WindowSpecificInputCallback(iEditorInput* apInput);

	bool Button_OnPressed(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(Button_OnPressed);


	cParticleEditorWindowEmitterParams* mpWEmitterParams;

	bool mbEmittersUpdated;
	cEditorInputEnum* mpEnumEmitters;
	std::vector<cEntityWrapperParticleEmitter*> mvEmitters;

	cWidgetButton* mpBNewEmitter;
	cWidgetButton* mpBCopyEmitter;
	cWidgetButton* mpBDeleteEmitter;
};

//-------------------------------------------------------------


#endif // HPLEDITOR_PARTICLE_EDITOR_WINDOW_EMITTERS_H
