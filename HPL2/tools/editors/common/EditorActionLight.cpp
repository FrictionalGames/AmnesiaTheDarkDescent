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

#include "EditorActionLight.h"

#include "EditorAction.h"

#include "EditorWorld.h"

#include "EntityWrapperLightBox.h"
#include "EntityWrapperLightPoint.h"
#include "EntityWrapperLightSpot.h"

//-------------------------------------------------------------
//-------------------------------------------------------------
////////////////////////////////////////////////////////////
// LIGHT SET INT PROPERTY
////////////////////////////////////////////////////////////
//-------------------------------------------------------------
//-------------------------------------------------------------

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------
/*
cEditorActionLightSetIntProperty::cEditorActionLightSetIntProperty(int alID, 
																   iEditorWorld* apEditorWorld,
																   eLightIntProperty aProp,
																   int alX) : iEditorActionObjectSetProperty<cEntityWrapperLight,int>("Set light int prop",
																																	  apEditorWorld, alID, aProp, alX, FetchOldValue(apEditorWorld, alID, aProp))
{
}

//-------------------------------------------------------------

///////////////////////////////////////////////////////////////
// PROTECTED METHODS
///////////////////////////////////////////////////////////////

//-------------------------------------------------------------

//-------------------------------------------------------------

void cEditorActionLightSetIntProperty::Apply(const int& alX)
{
	cEntityWrapperLight* pLight = (cEntityWrapperLight*) mpEditorWorld->GetEntity(mlEntityID);

	switch(mlProperty)
	{
	case eLightIntProperty_GoboAnimMode:
		pLight->SetGoboAnimMode(alX);
		break;
	}
}

//-------------------------------------------------------------

int cEditorActionLightSetIntProperty::FetchOldValue(iEditorWorld* apEditorWorld, int alID, int alProperty)
{
	cEntityWrapperLight* pLight = (cEntityWrapperLight*) apEditorWorld->GetEntity(alID);

	int lTemp;

	switch(alProperty)
	{
	case eLightIntProperty_GoboAnimMode:
		lTemp = pLight->GetGoboAnimMode();
		break;
	}

	return lTemp;
}

//-------------------------------------------------------------
*/

//-------------------------------------------------------------
//-------------------------------------------------------------
////////////////////////////////////////////////////////////
// LIGHT SET STRING PROPERTY ACTION
////////////////////////////////////////////////////////////
//-------------------------------------------------------------
//-------------------------------------------------------------

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

cEditorActionLightSetStringProperty::cEditorActionLightSetStringProperty(iEditorWorld* apEditorWorld, int alID, 
																	   eLightStringProperty aType,
																	   const tString& asNewValue) : iEditorActionWorldModifier("Set Light string", apEditorWorld)
{
	mpEditorWorld = apEditorWorld;
	mlID = alID;
	mPropertyType = aType;
	msNewValue = asNewValue;

	cEntityWrapperLight* pLight = (cEntityWrapperLight*)mpEditorWorld->GetEntity(mlID);
	switch(mPropertyType)
	{
	case eLightStringProperty_Gobo:
		msOldValue = pLight->GetGoboFilename();
		break;
	case eLightStringProperty_FalloffMap:
		msOldValue = pLight->GetFalloffMap();
		break;
	case eLightStringProperty_FlickerOnSound:
		msOldValue = pLight->GetFlickerOnSound();
		break;
	case eLightStringProperty_FlickerOffSound:
		msOldValue = pLight->GetFlickerOffSound();
		break;
	case eLightStringProperty_FlickerOnPS:
		msOldValue = pLight->GetFlickerOnPS();
		break;
	case eLightStringProperty_FlickerOffPS:
		msOldValue = pLight->GetFlickerOffPS();
		break;
	case eLightStringProperty_ShadowResolution:
		msOldValue = pLight->GetShadowResolution();
		break;
	default:
		break;
	}
}

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

void cEditorActionLightSetStringProperty::DoModify()
{
	cEntityWrapperLight* pLight = (cEntityWrapperLight*)mpEditorWorld->GetEntity(mlID);
	if(pLight)
	{
		switch(mPropertyType)
		{
		case eLightStringProperty_Gobo:
			pLight->SetGobo(msNewValue);
			break;
		case eLightStringProperty_GoboAnimMode:
			pLight->SetGoboAnimMode(msNewValue);
			break;
		case eLightStringProperty_FalloffMap:
			pLight->SetFalloffMap(msNewValue);
			break;
		case eLightStringProperty_FlickerOnSound:
			pLight->SetFlickerOnSound(msNewValue);
			break;
		case eLightStringProperty_FlickerOffSound:
			pLight->SetFlickerOffSound(msNewValue);
			break;
		case eLightStringProperty_FlickerOnPS:
			pLight->SetFlickerOnPS(msNewValue);
			break;
		case eLightStringProperty_FlickerOffPS:
			pLight->SetFlickerOffPS(msNewValue);
			break;
		case eLightStringProperty_ShadowResolution:
			pLight->SetShadowResolution(msNewValue);
			break;
		default:
			break;
		}
	}
}

//-------------------------------------------------------------

void cEditorActionLightSetStringProperty::UndoModify()
{
	cEntityWrapperLight* pLight = (cEntityWrapperLight*)mpEditorWorld->GetEntity(mlID);
	if(pLight)
	{
		switch(mPropertyType)
		{
		case eLightStringProperty_Gobo:
			pLight->SetGobo(msOldValue);
			break;
		case eLightStringProperty_GoboAnimMode:
			pLight->SetGoboAnimMode(msOldValue);
			break;
		case eLightStringProperty_FalloffMap:
			pLight->SetFalloffMap(msOldValue);
			break;
		case eLightStringProperty_FlickerOnSound:
			pLight->SetFlickerOnSound(msOldValue);
			break;
		case eLightStringProperty_FlickerOffSound:
			pLight->SetFlickerOffSound(msOldValue);
			break;
		case eLightStringProperty_FlickerOnPS:
			pLight->SetFlickerOnPS(msOldValue);
			break;
		case eLightStringProperty_FlickerOffPS:
			pLight->SetFlickerOffPS(msOldValue);
			break;
		case eLightStringProperty_ShadowResolution:
			pLight->SetShadowResolution(msOldValue);
			break;
		default:
			break;
		}
	}
}
//-------------------------------------------------------------
//-------------------------------------------------------------
////////////////////////////////////////////////////////////
// LIGHT SET FLOAT ACTION
////////////////////////////////////////////////////////////
//-------------------------------------------------------------
//-------------------------------------------------------------

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

cEditorActionLightSetFloatProperty::cEditorActionLightSetFloatProperty(iEditorWorld* apEditorWorld, int alID, 
									   eLightFloatProperty aType, 
									   float afNewValue, float afNewValueExt) : iEditorActionWorldModifier("Set light float property", apEditorWorld)
{
	mpEditorWorld = apEditorWorld;
	mlID = alID;
	mPropertyType = aType;
	mfNewValue = afNewValue;
	mfNewValueExt = afNewValueExt;

	cEntityWrapperLight* pLight = (cEntityWrapperLight*)apEditorWorld->GetEntity(mlID);
	switch(mPropertyType)
	{
	case eLightFloatProperty_Radius:
		mfOldValue = pLight->GetRadius();
		break;
	case eLightFloatProperty_FlickerOnMinLength:
		mfOldValue = pLight->GetFlickerOnMinLength();
		mfOldValueExt = pLight->GetFlickerOnMaxLength();
		break;
	case eLightFloatProperty_FlickerOnMaxLength:
		mfOldValue = pLight->GetFlickerOnMaxLength();
		mfOldValueExt = pLight->GetFlickerOnMinLength();
		break;
	case eLightFloatProperty_FlickerOffMinLength:
		mfOldValue = pLight->GetFlickerOffMinLength();
		mfOldValueExt = pLight->GetFlickerOffMaxLength();
		break;
	case eLightFloatProperty_FlickerOffMaxLength:
		mfOldValue = pLight->GetFlickerOffMaxLength();
		mfOldValueExt = pLight->GetFlickerOffMinLength();
		break;
	case eLightFloatProperty_FlickerOffRadius:
		mfOldValue = pLight->GetFlickerOffRadius();
		break;
	case eLightFloatProperty_FlickerOnFadeMinLength:
		mfOldValue = pLight->GetFlickerOnFadeMinLength();
		mfOldValueExt = pLight->GetFlickerOnFadeMaxLength();
		break;
	case eLightFloatProperty_FlickerOnFadeMaxLength:
		mfOldValue = pLight->GetFlickerOnFadeMaxLength();
		mfOldValueExt = pLight->GetFlickerOnFadeMinLength();
		break;
	case eLightFloatProperty_FlickerOffFadeMinLength:
		mfOldValue = pLight->GetFlickerOffFadeMinLength();
		mfOldValueExt = pLight->GetFlickerOffFadeMaxLength();
		break;
	case eLightFloatProperty_FlickerOffFadeMaxLength:
		mfOldValue = pLight->GetFlickerOnFadeMaxLength();
		mfOldValueExt = pLight->GetFlickerOnFadeMinLength();
		break;
	}
}

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

void cEditorActionLightSetFloatProperty::DoModify()
{
	Apply(mfNewValue, mfNewValueExt);	
}

void cEditorActionLightSetFloatProperty::UndoModify()
{
	Apply(mfOldValue, mfOldValueExt);	
}

//-------------------------------------------------------------

void cEditorActionLightSetFloatProperty::Apply(float afX, float afXExt)
{
	cEntityWrapperLight* pLight = (cEntityWrapperLight*)mpEditorWorld->GetEntity(mlID);

	if(pLight)
	{
		switch(mPropertyType)
		{
		case eLightFloatProperty_Radius:
			pLight->SetRadius(afX);
			break;
		case eLightFloatProperty_GoboAnimFrameTime:
			pLight->SetGoboAnimFrameTime(afX);
			break;
		case eLightFloatProperty_FlickerOnMinLength:
			pLight->SetFlickerOnMinLength(afX);
			//pLight->SetFlickerOnMaxLength(afXExt);
			pLight->UpdateFlickerParams();
			break;
		case eLightFloatProperty_FlickerOnMaxLength:
			pLight->SetFlickerOnMaxLength(afX);
			//pLight->SetFlickerOnMinLength(afXExt);
			pLight->UpdateFlickerParams();
			break;
		case eLightFloatProperty_FlickerOffMinLength:
			pLight->SetFlickerOffMinLength(afX);
			//pLight->SetFlickerOffMinLength(afXExt);
			pLight->UpdateFlickerParams();
			break;
		case eLightFloatProperty_FlickerOffMaxLength:
			pLight->SetFlickerOffMaxLength(afX);
			//pLight->SetFlickerOffMinLength(afXExt);
			pLight->UpdateFlickerParams();
			break;
		case eLightFloatProperty_FlickerOffRadius:
			pLight->SetFlickerOffRadius(afX);
			pLight->UpdateFlickerParams();
			break;
		case eLightFloatProperty_FlickerOnFadeMinLength:
			pLight->SetFlickerOnFadeMinLength(afX);
			//pLight->SetFlickerOnFadeMaxLength(afXExt);
			pLight->UpdateFlickerParams();
			break;
		case eLightFloatProperty_FlickerOnFadeMaxLength:
			pLight->SetFlickerOnFadeMaxLength(afX);
			//pLight->SetFlickerOnFadeMinLength(afXExt);
			pLight->UpdateFlickerParams();
			break;
		case eLightFloatProperty_FlickerOffFadeMinLength:
			pLight->SetFlickerOffFadeMinLength(afX);
			//pLight->SetFlickerOffFadeMaxLength(afXExt);
			pLight->UpdateFlickerParams();
			break;
		case eLightFloatProperty_FlickerOffFadeMaxLength:
			pLight->SetFlickerOffFadeMaxLength(afX);
			//pLight->SetFlickerOnFadeMinLength(afXExt);
			pLight->UpdateFlickerParams();
			break;
		}
	}
}


//-------------------------------------------------------------
//-------------------------------------------------------------
////////////////////////////////////////////////////////////
// LIGHT SET COLOR ACTION
////////////////////////////////////////////////////////////
//-------------------------------------------------------------
//-------------------------------------------------------------

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

cEditorActionLightSetColorProperty::cEditorActionLightSetColorProperty(iEditorWorld* apEditorWorld, int alID,
																	   eLightColorProperty aType,
																	   cColor acolNewValue) : iEditorActionWorldModifier("Set Light color", apEditorWorld)
{
	mpEditorWorld = apEditorWorld;
	mlID = alID;
	mPropertyType = aType;
	mcolNewValue = acolNewValue;

	cEntityWrapperLight* pLight = (cEntityWrapperLight*)apEditorWorld->GetEntity(mlID);
	switch(mPropertyType)
	{
	case eLightColorProperty_Diffuse:
		mcolOldValue = pLight->GetDiffuseColor();
		break;
	case eLightColorProperty_FlickerOff:
		mcolOldValue = pLight->GetFlickerOffColor();
		break;
	default:
		break;
	}
}

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

void cEditorActionLightSetColorProperty::DoModify()
{
	cEntityWrapperLight* pLight = (cEntityWrapperLight*)mpEditorWorld->GetEntity(mlID);

	if(pLight)
	{
		switch(mPropertyType)
		{
		case eLightColorProperty_Diffuse:
			pLight->SetDiffuseColor(mcolNewValue);
			break;
		case eLightColorProperty_FlickerOff:
			pLight->SetFlickerOffColor(mcolNewValue);
			pLight->UpdateFlickerParams();
			break;
		default:
			break;
		}
	}
}

//-------------------------------------------------------------

void cEditorActionLightSetColorProperty::UndoModify()
{
	cEntityWrapperLight* pLight = (cEntityWrapperLight*)mpEditorWorld->GetEntity(mlID);

	if(pLight)
	{
		switch(mPropertyType)
		{
		case eLightColorProperty_Diffuse:
			pLight->SetDiffuseColor(mcolOldValue);
			break;
		case eLightColorProperty_FlickerOff:
			pLight->SetFlickerOffColor(mcolOldValue);
			pLight->UpdateFlickerParams();
			break;
		default:
			break;
		}
	}
}

//-------------------------------------------------------------
//-------------------------------------------------------------
////////////////////////////////////////////////////////////
// LIGHT SET BOOL ACTION
////////////////////////////////////////////////////////////
//-------------------------------------------------------------
//-------------------------------------------------------------

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

cEditorActionLightSetBoolProperty::cEditorActionLightSetBoolProperty(iEditorWorld* apEditorWorld, int alID, 
																	 eLightBoolProperty aType, 
																	 bool abNewValue)  : iEditorActionWorldModifier("Set light bool property",apEditorWorld)
{
	mpEditorWorld = apEditorWorld;
	mlID = alID;
	mPropertyType = aType;
	mbNewValue = abNewValue;

	cEntityWrapperLight* pLight = (cEntityWrapperLight*)mpEditorWorld->GetEntity(mlID);
	switch(mPropertyType)
	{
	case eLightBoolProperty_FlickerActive:
		mbOldValue = pLight->GetFlickerActive();
		break;
	case eLightBoolProperty_FlickerFade:
		mbOldValue = pLight->GetFlickerFade();
		break;
	case eLightBoolProperty_CastShadows:
		mbOldValue = pLight->GetCastShadows();
		break;
	case eLightBoolProperty_ShadowsAffectStatic:
		mbOldValue = pLight->GetShadowsAffectStatic();
		break;
	case eLightBoolProperty_ShadowsAffectDynamic:
		mbOldValue = pLight->GetShadowsAffectDynamic();
		break;
	default:
		break;
	}
}

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

void cEditorActionLightSetBoolProperty::DoModify()
{
	cEntityWrapperLight* pLight = (cEntityWrapperLight*)mpEditorWorld->GetEntity(mlID);
	if(pLight)
	{
		switch(mPropertyType)
		{
		case eLightBoolProperty_FlickerActive:
			pLight->SetFlickerActive(mbNewValue);
			break;
		case eLightBoolProperty_FlickerFade:
			pLight->SetFlickerFade(mbNewValue);
			pLight->UpdateFlickerParams();
			break;
		case eLightBoolProperty_CastShadows:
			pLight->SetCastShadows(mbNewValue);
			break;
		case eLightBoolProperty_ShadowsAffectStatic:
			pLight->SetShadowsAffectStatic(mbNewValue);
			break;
		case eLightBoolProperty_ShadowsAffectDynamic:
			pLight->SetShadowsAffectDynamic(mbNewValue);
			break;
		}
	}
}

//-------------------------------------------------------------

void cEditorActionLightSetBoolProperty::UndoModify()
{
	cEntityWrapperLight* pLight = (cEntityWrapperLight*)mpEditorWorld->GetEntity(mlID);
	if(pLight)
	{
		switch(mPropertyType)
		{
		case eLightBoolProperty_FlickerActive:
			pLight->SetFlickerActive(mbOldValue);
			break;
		case eLightBoolProperty_FlickerFade:
			pLight->SetFlickerFade(mbOldValue);
			pLight->UpdateFlickerParams();
			break;
		case eLightBoolProperty_CastShadows:
			pLight->SetCastShadows(mbOldValue);
			break;
		case eLightBoolProperty_ShadowsAffectStatic:
			pLight->SetShadowsAffectStatic(mbOldValue);
			break;
		case eLightBoolProperty_ShadowsAffectDynamic:
			pLight->SetShadowsAffectDynamic(mbOldValue);
			break;
		}
	}
}

//-------------------------------------------------------------
//-------------------------------------------------------------
////////////////////////////////////////////////////////////
// LIGHT BOX SET SIZE ACTION
////////////////////////////////////////////////////////////
//-------------------------------------------------------------
//-------------------------------------------------------------

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

cEditorActionLightBoxSetSizeProperty::cEditorActionLightBoxSetSizeProperty(iEditorWorld* apEditorWorld, int alID,
																		   const cVector3f& avNewValue)  : iEditorActionWorldModifier("Set box light size", apEditorWorld)
{
	mpEditorWorld = apEditorWorld;
	mlID = alID;
	mvNewValue = avNewValue;

	cEntityWrapperLightBox* pLight = (cEntityWrapperLightBox*)mpEditorWorld->GetEntity(mlID);
	
	mvOldValue = pLight->GetSize();
}

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

void cEditorActionLightBoxSetSizeProperty::DoModify()
{
	cEntityWrapperLightBox* pLight = (cEntityWrapperLightBox*)mpEditorWorld->GetEntity(mlID);

	if(pLight)
	{
		pLight->SetSize(mvNewValue);
	}
}

//-------------------------------------------------------------

void cEditorActionLightBoxSetSizeProperty::UndoModify()
{
	cEntityWrapperLightBox* pLight = (cEntityWrapperLightBox*)mpEditorWorld->GetEntity(mlID);

	if(pLight)
	{
		pLight->SetSize(mvOldValue);
	}
}


//-------------------------------------------------------------
//-------------------------------------------------------------
////////////////////////////////////////////////////////////
// LIGHT BOX SET BLEND FUNC ACTION
////////////////////////////////////////////////////////////
//-------------------------------------------------------------
//-------------------------------------------------------------

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

cEditorActionLightBoxSetBlendFuncProperty::cEditorActionLightBoxSetBlendFuncProperty(iEditorWorld* apEditorWorld,int alID,
																					 eLightBoxBlendFunc aFunc)  : iEditorActionWorldModifier("Set box light blend function", apEditorWorld)
{
	mpEditorWorld = apEditorWorld;
	mlID = alID;
	mNewValue = aFunc;
	
	cEntityWrapperLightBox* pLight = (cEntityWrapperLightBox*)mpEditorWorld->GetEntity(mlID);
	
	mOldValue = pLight->GetBlendFunc();
}

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

void cEditorActionLightBoxSetBlendFuncProperty::DoModify()
{
	cEntityWrapperLightBox* pLight = (cEntityWrapperLightBox*)mpEditorWorld->GetEntity(mlID);

	if(pLight)
	{
		pLight->SetBlendFunc(mNewValue);
	}
}

//-------------------------------------------------------------

void cEditorActionLightBoxSetBlendFuncProperty::UndoModify()
{
	cEntityWrapperLightBox* pLight = (cEntityWrapperLightBox*)mpEditorWorld->GetEntity(mlID);

	if(pLight)
	{
		pLight->SetBlendFunc(mOldValue);
	}
}

//-------------------------------------------------------------
//-------------------------------------------------------------
////////////////////////////////////////////////////////////
// LIGHT SPOT SET STRING ACTION
////////////////////////////////////////////////////////////
//-------------------------------------------------------------
//-------------------------------------------------------------

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

cEditorActionLightSpotSetStringProperty::cEditorActionLightSpotSetStringProperty(iEditorWorld* apEditorWorld, int alID,
																	   eLightSpotStringProperty aType,
																	   const tString& asNewValue) : iEditorActionWorldModifier("Set Light string", apEditorWorld)
{
	mpEditorWorld = apEditorWorld;
	mlID = alID;
	mPropertyType = aType;
	msNewValue = asNewValue;

	cEntityWrapperLightSpot* pLight = (cEntityWrapperLightSpot*)mpEditorWorld->GetEntity(mlID);	
	switch(mPropertyType)
	{
	case eLightSpotStringProperty_FalloffMap:
		msOldValue = pLight->GetSpotFalloffMap();
		break;
	default:
		break;
	}
}

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

void cEditorActionLightSpotSetStringProperty::DoModify()
{
	cEntityWrapperLightSpot* pLight = (cEntityWrapperLightSpot*)mpEditorWorld->GetEntity(mlID);
	if(pLight)
	{
		switch(mPropertyType)
		{
		case eLightSpotStringProperty_FalloffMap:
			pLight->SetSpotFalloffMap(msNewValue);
			break;
		default:
			break;
		}
	}
}

//-------------------------------------------------------------

void cEditorActionLightSpotSetStringProperty::UndoModify()
{
	cEntityWrapperLightSpot* pLight = (cEntityWrapperLightSpot*)mpEditorWorld->GetEntity(mlID);
	if(pLight)
	{
		switch(mPropertyType)
		{
		case eLightSpotStringProperty_FalloffMap:
			pLight->SetSpotFalloffMap(msOldValue);
			break;
		default:
			break;
		}
	}
}

//-------------------------------------------------------------
//-------------------------------------------------------------
////////////////////////////////////////////////////////////
// LIGHT SPOT SET FLOAT ACTION
////////////////////////////////////////////////////////////
//-------------------------------------------------------------
//-------------------------------------------------------------

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

cEditorActionLightSpotSetFloatProperty::cEditorActionLightSpotSetFloatProperty(iEditorWorld* apEditorWorld, int alID, 
																			   eLightSpotFloatProperty aType, 
																			   float afNewValue) : iEditorActionWorldModifier("Set Spotlight float property", apEditorWorld)
{
	mpEditorWorld = apEditorWorld;
	mlID = alID;
	mPropertyType = aType;
	mfNewValue = afNewValue;

	cEntityWrapperLightSpot* pLight = (cEntityWrapperLightSpot*)mpEditorWorld->GetEntity(mlID);
	
	switch(mPropertyType)
	{
	case eLightSpotFloatProperty_FOV:
		mfOldValue = pLight->GetFOV();
		break;
	case eLightSpotFloatProperty_Aspect:
		mfOldValue = pLight->GetAspect();
		break;
	case eLightSpotFloatProperty_NearClipPlane:
		mfOldValue = pLight->GetNearClipPlane();
		break;
	default:
		break;
	}
}

//-------------------------------------------------------------
///////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////
//-------------------------------------------------------------

void cEditorActionLightSpotSetFloatProperty::DoModify()
{
	cEntityWrapperLightSpot* pLight = (cEntityWrapperLightSpot*)mpEditorWorld->GetEntity(mlID);

	if(pLight)
	{
		switch(mPropertyType)
		{
		case eLightSpotFloatProperty_FOV:
			pLight->SetFOV(mfNewValue);
			break;
		case eLightSpotFloatProperty_Aspect:
			pLight->SetAspect(mfNewValue);
			break;
		case eLightSpotFloatProperty_NearClipPlane:
			pLight->SetNearClipPlane(mfNewValue);
			break;
		default:
			break;
		}
	}
}

//-------------------------------------------------------------

void cEditorActionLightSpotSetFloatProperty::UndoModify()
{
	cEntityWrapperLightSpot* pLight = (cEntityWrapperLightSpot*)mpEditorWorld->GetEntity(mlID);

	if(pLight)
	{
		switch(mPropertyType)
		{
		case eLightSpotFloatProperty_FOV:
			pLight->SetFOV(mfOldValue);
			break;
		case eLightSpotFloatProperty_Aspect:
			pLight->SetAspect(mfOldValue);
			break;
		case eLightSpotFloatProperty_NearClipPlane:
			pLight->SetNearClipPlane(mfOldValue);
			break;
		default:
			break;
		}
	}
}

//-------------------------------------------------------------
