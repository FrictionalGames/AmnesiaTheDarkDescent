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

#ifndef HPLEDITOR_ENTITY_WRAPPER_PRIMITIVE_PLANE_H
#define HPLEDITOR_ENTITY_WRAPPER_PRIMITIVE_PLANE_H

#include "EntityWrapperPrimitive.h"

//---------------------------------------------------------------

#define PlanePropIdStart 120

enum ePrimitivePlaneVec3f
{
	ePrimitivePlaneVec3f_TileAmount = PlanePropIdStart,
	ePrimitivePlaneVec3f_TileOffset,
	ePrimitivePlaneVec3f_StartCorner,
	ePrimitivePlaneVec3f_EndCorner,

	ePrimitivePlaneVec3f_LastEnum
};

enum ePrimitivePlaneFloat
{
	ePrimitivePlaneFloat_TextureAngle = PlanePropIdStart,

	ePrimitivePlaneFloat_LastEnum,
};

enum ePrimitivePlaneBoolProperty
{
	ePrimitivePlaneBool_AlignToWorld = PlanePropIdStart,

	ePrimitivePlaneBool_LastEnum
};

//---------------------------------------------------------------

class cEntityWrapperTypePrimitivePlane : public iEntityWrapperTypePrimitive
{
public:
	cEntityWrapperTypePrimitivePlane();

protected:
	iEntityWrapperData* CreateSpecificData();
};

//---------------------------------------------------------------

class cEntityWrapperDataPrimitivePlane : public iEntityWrapperData
{
public:
	cEntityWrapperDataPrimitivePlane(iEntityWrapperType*);

	void CopyFromEntity(iEntityWrapper*);
	bool Load(cXmlElement*);
	bool SaveSpecific(cXmlElement*);
	
protected:
	iEntityWrapper* CreateSpecificEntity();

	int mlNormalAxisIndex;
	tVector2fVec mvUVCorners;
};

//------------------------------------------------------------------------

class cEntityWrapperPrimitivePlane : public iEntityWrapperPrimitive
{
public:
	cEntityWrapperPrimitivePlane(iEntityWrapperData* apData);

	bool SetProperty(int, const cVector3f&);
	bool SetProperty(int, const bool&);
	bool SetProperty(int, const float&);

	bool GetProperty(int, cVector3f&);
	bool GetProperty(int, bool&);
	bool GetProperty(int, float&);

	void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected, const cColor& aHighlightCol=cColor(1,1), const cColor& aDisabledCol=cColor(0.5f,1));

	void SetTileAmount(const cVector3f& avTileAmount);
	void SetTileOffset(const cVector3f& avOffset);
	void SetTextureAngle(float afX);

	cVector3f& GetTileAmount() { return mvTileAmount; }
	cVector3f& GetTileOffset() { return mvTileOffset; }
	float GetTextureAngle() { return mfTextureAngle; }

	void SetAlignToWorld(bool abX) { mbAlignToWorldCoords = abX; UpdateUVMapping();}
	bool GetAlignToWorld() { return mbAlignToWorldCoords; }

	void UpdateEntity();

	void SetStartCorner(const cVector3f&);
	void SetEndCorner(const cVector3f&);

	const cVector3f& GetStartCorner() { return mvStartCorner; }
	const cVector3f& GetEndCorner() { return mvEndCorner; }

	const tVector2fVec& GetUVCorners() { return mvUVCorners; }
	int GetNormalAxisIndex() { return mlNormalAxisIndex; }

	void SetAbsScale(const cVector3f& avScale, int alAxis=-1);

protected:
	int ComputeNormalAxisIndex();
	void UpdateUVMapping();
	cMesh* CreatePrimitiveMesh();
	cVector3f mvStartCorner;
	cVector3f mvEndCorner;

	cVector3f mvTileAmount;
	cVector3f mvTileOffset;
	float mfTextureAngle;
	int mlNormalAxisIndex;
	bool mbAlignToWorldCoords;

	cVector2f mvStartCornerUV;
	cVector2f mvEndCornerUV;

	tVector2fVec mvUVCorners;
};

//------------------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_PRIMITIVE_H
