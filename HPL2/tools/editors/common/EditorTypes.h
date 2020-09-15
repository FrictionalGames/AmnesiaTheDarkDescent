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

#ifndef HPLEDITOR_EDITOR_TYPES_H
#define HPLEDITOR_EDITOR_TYPES_H

#include "../common/StdAfx.h"

using namespace hpl;

//-------------------------------------------------------------

class iEditorAction;
class iEditorWindow;
class iEditorEditMode;
class iEntityWrapperType;
class iEntityWrapper;
class iEntityWrapperData;

class iEditorObjectIndex;
class iEditorObjectIndexDir;
class iEditorObjectIndexEntry;

class cEntityWrapperStaticObject;
class cEntityWrapperBodyShape;


enum eEditorInputType
{
	eEditorInputType_Bool,
	eEditorInputType_String,
	eEditorInputType_File,
	eEditorInputType_Number,
	eEditorInputType_Vec2,
	eEditorInputType_Vec3,
	eEditorInputType_Enum,

	eEditorInputType_LastEnum,
};

enum eEditorInputLayoutStyle
{
	eEditorInputLayoutStyle_RowLabelOnLeft,
	eEditorInputLayoutStyle_RowLabelOnTop,
	eEditorInputLayoutStyle_ColumnLabelOnLeft,
	eEditorInputLayoutStyle_ColumnLabelOnTop,

	eEditorInputLayoutStyle_LastEnum,
};

enum eEditorInputCallback
{
	eEditorInputCallback_ValueEnter,
	eEditorInputCallback_ValueChange,
	eEditorInputCallback_Browse,

	eEditorInputCallback_LastEnum,
};

enum eEditorEntityType
{
	////////////////////////////////////
	// Level Editor & Model Editor
	eEditorEntityType_StaticObject,
	eEditorEntityType_Light,
	eEditorEntityType_Entity,
	eEditorEntityType_Billboard,
	eEditorEntityType_Sound,
	eEditorEntityType_ParticleSystem,
	eEditorEntityType_Area,
	eEditorEntityType_Primitive,
	eEditorEntityType_Decal,
	eEditorEntityType_FogArea,

	////////////////////////////////////
	// Model Editor specific
	eEditorEntityType_BodyShape,
	eEditorEntityType_Body,
	eEditorEntityType_Joint,
	eEditorEntityType_SubMesh,
	eEditorEntityType_Bone,

	////////////////////////////////////
	// Particle Editor
	eEditorEntityType_ParticleEmitter,

	////////////////////////////////////
	// Misc
	eEditorEntityType_Compound,

	eEditorEntityType_LastEnum
};

//-------------------------------------------------------------

enum eEditorBodyShape
{
	eEditorBodyShape_Box,
	eEditorBodyShape_Cylinder,
	eEditorBodyShape_Capsule,
	eEditorBodyShape_Sphere,
	eEditorBodyShape_LastEnum,
};

//-------------------------------------------------------------

enum eEditorEntityJointType
{
	eEditorEntityJointType_Ball,
	eEditorEntityJointType_Hinge,
	eEditorEntityJointType_Screw,
	eEditorEntityJointType_Slider,

	eEditorEntityJointType_LastEnum,
};

//-------------------------------------------------------------

enum eEditorPrimitiveType
{
	eEditorPrimitiveType_Plane,

	eEditorPrimitiveType_LastEnum,
};

//-------------------------------------------------------------

enum eEditorTextureType
{
	eEditorTextureType_1D,
	eEditorTextureType_2D,
	eEditorTextureType_3D,
	eEditorTextureType_CubeMap,

	eEditorTextureType_LastEnum,
};

enum eEditorTextureResourceType
{
	eEditorTextureResourceType_1D,
	eEditorTextureResourceType_2D,
	eEditorTextureResourceType_3D,
	eEditorTextureResourceType_CubeMap,
	eEditorTextureResourceType_Anim,

	eEditorTextureResourceType_LastEnum,
};

enum eEditorResourceType
{
	eEditorResourceType_Material,
	eEditorResourceType_Texture,
	eEditorResourceType_Sound,
	eEditorResourceType_ParticleSystem,
	eEditorResourceType_EntityFile,
	eEditorResourceType_Model,
	eEditorResourceType_ModelAnim,

	eEditorResourceType_LastEnum,
};

enum eEditorEntityLightType
{
	eEditorEntityLightType_Box,
	eEditorEntityLightType_Point,
	eEditorEntityLightType_Spot,

	eEditorEntityLightType_LastEnum,
};

//-------------------------------------------------------------

typedef std::list<iEditorAction*>	tEditorActionList;
typedef tEditorActionList::iterator	tEditorActionListIt;

typedef std::vector<cCamera*>		tCameraVec;
typedef tCameraVec::iterator		tCameraVecIt;

typedef std::list<iEditorWindow*>	tEditorWindowList;
typedef tEditorWindowList::iterator tEditorWindowListIt;

typedef std::vector<iEditorEditMode*> tEditorEditModeVec;
typedef tEditorEditModeVec::iterator  tEditorEditModeVecIt;

typedef std::list<iEntityWrapperType*>	tEntityTypeList;
typedef tEntityTypeList::iterator		tEntityTypeListIt;

typedef std::vector<iEntityWrapper*> tEntityWrapperVec;
typedef tEntityWrapperVec::iterator tEntityWrapperVecIt;

typedef std::list<iEntityWrapper*> tEntityWrapperList;
typedef tEntityWrapperList::iterator tEntityWrapperListIt;

typedef std::map<unsigned int, iEntityWrapper*> tEntityWrapperMap;
typedef tEntityWrapperMap::const_iterator tEntityWrapperMapConstIt;
typedef tEntityWrapperMap::iterator tEntityWrapperMapIt;

typedef std::list<cEntityWrapperStaticObject*> tEntityStaticObjectList;
typedef tEntityStaticObjectList::iterator tEntityStaticObjectListIt;

typedef std::vector<iEntityWrapperData*> tEntityDataVec;
typedef tEntityDataVec::iterator tEntityDataVecIt;

typedef std::list<cEntityWrapperBodyShape*> tEntityWrapperBodyShapeList;
typedef tEntityWrapperBodyShapeList::iterator tEntityWrapperBodyShapeListIt;

typedef std::map<tWString, iEditorObjectIndexDir*> tIndexDirMap;
typedef tIndexDirMap::iterator tIndexDirMapIt;

typedef std::vector<iEditorObjectIndexDir*> tIndexDirVec;
typedef tIndexDirVec::iterator tIndexDirVecIt;

typedef std::map<tString, iEditorObjectIndexEntry*> tIndexEntryMap;
typedef tIndexEntryMap::iterator tIndexEntryMapIt;
typedef tIndexEntryMap::const_iterator tIndexEntryMapConstIt;

class cEditorClipPlane;

typedef std::vector<cEditorClipPlane*> tEditorClipPlaneVec;
typedef tEditorClipPlaneVec::iterator  tEditorClipPlaneVecIt;

//-------------------------------------------------------------

typedef std::map<tString, float> tEditorSettingsMap;

#endif //HPLEDITOR_EDITOR_TYPES
