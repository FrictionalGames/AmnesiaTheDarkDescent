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

#ifndef HPLEDITOR_SURFACE_PICKER_H
#define HPLEDITOR_SURFACE_PICKER_H

#include "EntityPicker.h"

enum eSurfaceType
{
	eSurfaceType_StaticObject,
	eSurfaceType_Primitive,
	eSurfaceType_Entity,

	eSurfaceType_LastEnum,
};

class cSurfacePicker : public cEntityPicker
{
public:
	cSurfacePicker(iEditorWorld*);

	void SetAffectType(int,bool, bool abTranslate=true);

	bool HasPickedSurface() { return mpClosestSurfaceEntity!=NULL; }
	const cVector3f& GetPositionInSurface() { return mvIntersection; }
	const cVector3f& GetUpVector() { return mbAverageNormal?mvAverageUp:mvTriangleNormal; }
	const cVector3f& GetRightVector() { return mbAverageNormal?mvAverageRight:mvRight; }
	const cVector3f& GetForwardVector() { return mbAverageNormal?mvAverageForward:mvForward; }
	cMatrixf GetOrientationMatrix();

	const std::vector<iEntityWrapper*>& GetFilteredEntities() { return mvFilteredEntities; }
	const tSubMeshEntityVec& GetFilteredSubMeshes() { return mvFilteredSubMeshes; }

	bool SetAverageNormal(bool abX) { mbAverageNormal=abX; }
	void SetAverageVolumeSize(const cVector3f&);
	void SetAverageBackfacingAngleThreshold(float);

	bool IsAffected(int, bool abTranslate=true);
protected:
	int ToSurfaceTypeID(int);
	int ToEntityTypeID(int);

	void OnDraw(cRendererCallbackFunctions* apFunctions);

	void Iterate();
	void IterateRenderableNode(iRenderableContainerNode *apNode, const cVector3f& avStart, const cVector3f& avEnd, cBoundingVolume *apBV, int* apTriIndex, bool abAveragingNormal=false);
	bool CheckObjectIntersection(iRenderable *apObject, const cVector3f& avStart, const cVector3f& avEnd, cBoundingVolume *apBV, int *apTriIndex, bool abAveragingNormal=false);

	void ComputeAverageNormal(const cVector3f&);

	float mfMinT;
	float mfRayLength;
	tBoolVec mvAffectedSurface;

	iEntityWrapper* mpClosestSurfaceEntity;
	cSubMeshEntity* mpClosestMeshEntity;
	tVector3fVec mvTriangle;
	cVector3f mvIntersection;
	cVector3f mvTriangleNormal;
	cVector3f mvRight;
	cVector3f mvForward;

	cVector3f mvRightBase;
	cVector3f mvForwardBase;

	std::vector<iEntityWrapper*> mvFilteredEntities;
	tSubMeshEntityVec mvFilteredSubMeshes;
	bool mbAverageNormal;
	cBoundingVolume mAvgVolume;
	float mfBackfacingAngleThreshold;
	tSubMeshEntityVec mvSubMeshesToAverage;

	tVector3fVec mvTrianglesToAverage;

	cVector3f mvAverageRight;
	cVector3f mvAverageUp;
	cVector3f mvAverageForward;
};

#endif // HPLEDITOR_SURFACE_PICKER_H
