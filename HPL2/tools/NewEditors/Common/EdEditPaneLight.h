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

#ifndef ED_EDIT_BOX_LIGHT_H
#define ED_EDIT_BOX_LIGHT_H

#include "../Common/EdScnObjEditPane.h"

//--------------------------------------------------------------------

class cEdEditPaneLight : public iEdScnObjEditPane
{
public:
	cEdEditPaneLight(iEdScnObject*);
	~cEdEditPaneLight();

	void Create();

protected:
	void OnUpdate();

	void AddPropertyGobo(cWidgetTab*);
	void AddPropertyFalloffMap(cWidgetTab*);
	void AddPropertyRadius(cWidgetTab*);
	void AddPropertyDiffuseColor(cWidgetTab*);
	void AddPropertyCastShadows(cWidgetTab*);

	void AddPropertySetFlicker(cWidgetTab*);
	
	void AddPropertySetBox(cWidgetTab*);
	void AddPropertySetPoint(cWidgetTab*);
	void AddPropertySetSpot(cWidgetTab*);

	
	bool InputCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(InputCallback);
	

	bool WindowSpecificInputCallback(iEdInput*);

	///////////////////////////////////////////////////
	// Data
	tWString msTempTextureFilename;
	tWStringVec mvLoadedFiles;

	cColor mDiffuseCol;
	cColor mSpecularCol;
	cColor mFlickerOffCol;

	// Layout
	cWidgetTab* mpTabGeneral;
	cWidgetTab* mpTabFlicker;
	cWidgetTab* mpTabLightParameters;

	cWidgetDummy* mpGroupRadius;
	cEdInputNumber* mpInpRadius;

	cWidgetDummy* mpGroupGobo;
	cEdInputFile* mpInpGobo;
	cEdInputEnum* mpInpGoboAnimMode;
	cEdInputNumber* mpInpGoboAnimFrameTime;

	cWidgetDummy* mpGroupFalloff;
	cEdInputFile* mpInpFalloffMap;

	cEdInputColor* mpInpDiffuse;

	cWidgetDummy* mpGroupDiffuse;
	cWidgetLabel* mpLabelDiffuseColor;
	cWidgetLabel* mpLabelDiffuseColorExp;
	cWidgetFrame* mpFrameDiffuseColor;

	cWidgetDummy* mpGroupShadows;
	cEdInputBool* mpInpCastShadows;
	cEdInputEnum* mpInpShadowRes;
	cEdInputBool* mpInpShadowAffectStatic;
	cEdInputBool* mpInpShadowAffectDynamic;

	///////////////////////////////
	// Flicker stuff
	cEdInputBool* mpInpFlickerActive;

	// On params
	cWidgetDummy* mpGFlickerOn;
	cEdInputNumber* mpInpFlickerOnMinLength;
	cEdInputNumber* mpInpFlickerOnMaxLength;

	cEdInputFile* mpInpFlickerOnSound;
	cEdInputFile* mpInpFlickerOnPS;

	// Off params
	cWidgetDummy* mpGFlickerOff;

	cEdInputNumber* mpInpFlickerOffMinLength;
	cEdInputNumber* mpInpFlickerOffMaxLength;
	
	cEdInputFile* mpInpFlickerOffSound;
	cEdInputFile* mpInpFlickerOffPS;

	cEdInputNumber* mpInpFlickerOffRadius;
	cEdInputColor* mpInpFlickerOffColor;

	// Fade
	cWidgetDummy* mpGFlickerFade;

	cEdInputBool* mpInpFlickerFade;

	cEdInputNumber* mpInpFlickerFadeOnMinLength;
	cEdInputNumber* mpInpFlickerFadeOnMaxLength;
	cEdInputNumber* mpInpFlickerFadeOffMinLength;
	cEdInputNumber* mpInpFlickerFadeOffMaxLength;

	/////////////////////////////////
	// Box Light specific
	cEdInputEnum* mpInpBoxBlendFunc;
	cEdInputVec3* mpInpBoxSize;

	///////////////////////////////////
	// Spot Light specific
	cEdInputNumber* mpInpSpotFOV;
	cEdInputNumber* mpInpSpotAspect;
	cEdInputNumber* mpInpSpotNearClipPlane;
	cEdInputFile* mpInpSpotFalloffMap;
};



#endif // HPLEDITOR_EDITOR_WINDOW_EDIT_BOX_LIGHT_H
