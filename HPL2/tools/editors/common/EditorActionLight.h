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

#include "StdAfx.h"

#include "EditorAction.h"


class cEditorEditModeSelect;
class iEditorWorld;

class cEntityWrapperLight;
class cEntityWrapperLightBox;
class cEntityWrapperLightSpot;

//------------------------------------------------------------------------------

enum eEditorEntityLightType;

//////////////////////////////////////////////
// General Light properties

enum eLightIntProperty
{
	eLightIntProperty_GoboAnimMode,
};


enum eLightBoolProperty
{
	eLightBoolProperty_FlickerActive,
	eLightBoolProperty_FlickerFade,
	eLightBoolProperty_CastShadows,
	eLightBoolProperty_ShadowsAffectStatic,
	eLightBoolProperty_ShadowsAffectDynamic,

	eLightBoolProperty_LastEnum,
};

enum eLightColorProperty
{
	eLightColorProperty_Diffuse,
	eLightColorProperty_FlickerOff,

	eLightColorProperty_LastEnum,
};

enum eLightFloatProperty
{
	eLightFloatProperty_Radius,

	eLightFloatProperty_GoboAnimFrameTime, 
	eLightFloatProperty_FlickerOnMinLength,
	eLightFloatProperty_FlickerOnMaxLength,
	eLightFloatProperty_FlickerOffMinLength,
	eLightFloatProperty_FlickerOffMaxLength,
	eLightFloatProperty_FlickerOnRadius,
	eLightFloatProperty_FlickerOffRadius,
	eLightFloatProperty_FlickerOnFadeMinLength,
	eLightFloatProperty_FlickerOnFadeMaxLength,
	eLightFloatProperty_FlickerOffFadeMinLength,
	eLightFloatProperty_FlickerOffFadeMaxLength,
	
	eLightFloatProperty_LastEnum,	
};

enum eLightStringProperty
{
	eLightStringProperty_Gobo,
	eLightStringProperty_GoboAnimMode,
	eLightStringProperty_FalloffMap,
	eLightStringProperty_FlickerOnSound,
	eLightStringProperty_FlickerOffSound,
	eLightStringProperty_FlickerOnPS,
	eLightStringProperty_FlickerOffPS,
	eLightStringProperty_ShadowResolution,

	eLightStringProperty_LastEnum,
};

//////////////////////////////////////
// Specific Spotlight properties

enum eLightSpotFloatProperty
{
	eLightSpotFloatProperty_FOV,
	eLightSpotFloatProperty_Aspect,
	eLightSpotFloatProperty_NearClipPlane,
	
	eLightSpotFloatProperty_LastEnum,	
};


enum eLightSpotStringProperty
{
	eLightSpotStringProperty_FalloffMap,
	
	eLightSpotStringProperty_LastEnum,
};

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////
// LIGHT SET INT PROPERTY ACTION
////////////////////////////////////////////////////////////
/*
//------------------------------------------------------------------------------

class cEditorActionLightSetIntProperty : public iEditorActionObjectSetProperty<cEntityWrapperLight, int>
{
public:
	cEditorActionLightSetIntProperty(int alID, iEditorWorld* apWorld, eLightIntProperty aProp, int alX);

	void Apply(const int& alX);
	static int FetchOldValue(iEditorWorld* apEditorWorld, int alID, int alProperty);

protected:
	///////////////////////////////////
	// Data
};*/

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////
// LIGHT SET STRING ACTION
////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

class cEditorActionLightSetStringProperty : public iEditorActionWorldModifier
{
public:
	cEditorActionLightSetStringProperty(iEditorWorld* apEditorWorld, int alID,
									  eLightStringProperty aType,
									  const tString& asNewValue);

	void DoModify();
	void UndoModify();

protected:
	///////////////////////////////////
	// Data
	int mlID;

	eLightStringProperty mPropertyType;

	tString msOldValue;
	tString msNewValue;
	
};

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////
// LIGHT SET FLOAT ACTION
////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

class cEditorActionLightSetFloatProperty : public iEditorActionWorldModifier
{
public:
	cEditorActionLightSetFloatProperty(iEditorWorld* apEditorWorld, int alID,
									   eLightFloatProperty aType, 
									   float afNewValue, float afNewValueExt=0);
	void DoModify();
	
	void UndoModify();

protected:
	void Apply(float afX, float afXExt);
	///////////////////////////////////
	// Data
	int mlID;
	eLightFloatProperty mPropertyType;

	float mfOldValue;
	float mfOldValueExt;

	float mfNewValue;
	float mfNewValueExt;
};

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////
// LIGHT SET COLOR ACTION
////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

class cEditorActionLightSetColorProperty : public iEditorActionWorldModifier
{
public:
	cEditorActionLightSetColorProperty(iEditorWorld* apEditorWorld, int alID,
									  eLightColorProperty aType,
									  cColor acolNewValue);

	void DoModify();
	void UndoModify();

protected:
	///////////////////////////////////
	// Data
	int mlID;
	eLightColorProperty mPropertyType;

	cColor mcolOldValue;
	cColor mcolNewValue;
	
};

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////
// LIGHT SET BOOL ACTION
////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

class cEditorActionLightSetBoolProperty : public iEditorActionWorldModifier
{
public:
	cEditorActionLightSetBoolProperty(iEditorWorld* apEditorWorld, int alID,
									  eLightBoolProperty aType,
									  bool abNewValue);

	void DoModify();
	void UndoModify();

protected:
	///////////////////////////////////
	// Data
	int mlID;
	eLightBoolProperty mPropertyType;

	bool mbOldValue;
	bool mbNewValue;
	
};

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////
// LIGHT BOX SET SIZE ACTION
////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

class cEditorActionLightBoxSetSizeProperty : public iEditorActionWorldModifier
{
public:
	cEditorActionLightBoxSetSizeProperty(iEditorWorld* apEditorWorld, int alID,
										 const cVector3f& avNewValue);

	void DoModify();
	void UndoModify();

protected:
	///////////////////////////////////
	// Data
	int mlID;

	cVector3f mvOldValue;
	cVector3f mvNewValue;
	
};

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////
// LIGHT BOX SET BLEND FUNC ACTION
////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

class cEditorActionLightBoxSetBlendFuncProperty : public iEditorActionWorldModifier
{
public:
	cEditorActionLightBoxSetBlendFuncProperty(iEditorWorld* apEditorWorld, int alID,
											  eLightBoxBlendFunc aFunc);

	void DoModify();
	void UndoModify();

protected:
	///////////////////////////////////
	// Data
	int mlID;

	eLightBoxBlendFunc mOldValue;
	eLightBoxBlendFunc mNewValue;
	
};

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////
// LIGHT SPOT SET STRING ACTION
////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

class cEditorActionLightSpotSetStringProperty : public iEditorActionWorldModifier
{
public:
	cEditorActionLightSpotSetStringProperty(iEditorWorld* apEditorWorld, int alID,
									  eLightSpotStringProperty aType,
									  const tString& asNewValue);

	void DoModify();
	void UndoModify();

protected:
	///////////////////////////////////
	// Data
	int mlID;
	eLightSpotStringProperty mPropertyType;

	tString msOldValue;
	tString msNewValue;
	
};

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////
// LIGHT SPOT SET FLOAT ACTION
////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------

class cEditorActionLightSpotSetFloatProperty : public iEditorActionWorldModifier
{
public:
	cEditorActionLightSpotSetFloatProperty(iEditorWorld* apEditorWorld, int alID,
									   eLightSpotFloatProperty aType, 
									   float afNewValue);
	void DoModify();
	void UndoModify();

protected:
	///////////////////////////////////
	// Data
	int mlID;
	eLightSpotFloatProperty mPropertyType;

	float mfOldValue;
	float mfNewValue;
};

//------------------------------------------------------------------------------
