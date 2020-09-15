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

#ifndef ED_SURFACE_PICKER_H
#define ED_SURFACE_PICKER_H

#include "../Common/EdObjectPicker.h"
#include "../Common/EdUIPicker.h"

//----------------------------------------------------------------------------------

class iEdObjectType;
class cUIPickFilter;

//----------------------------------------------------------------------------------

class cEdSurfacePicker : public cEdObjectPicker
{
public:
	cEdSurfacePicker(iEdWorld*);

	void RegisterSurfaceType(iEdObjectType*);
	void SetAffectType(iEdObjectType*, bool);

	void SetAffectedTypes(const tTypeFilterMap&);
	const tTypeFilterMap& GetAffectedTypes();

	bool HasPickedSurface() { return mpClosestSurfaceObject!=NULL; }
	const cVector3f& GetPositionInSurface() { return mvIntersection; }
	const cVector3f& GetUpVector() { return mbAverageNormal?mvAverageUp:mvTriangleNormal; }
	const cVector3f& GetRightVector() { return mbAverageNormal?mvAverageRight:mvRight; }
	const cVector3f& GetForwardVector() { return mbAverageNormal?mvAverageForward:mvForward; }
	cMatrixf GetOrientationMatrix();

	//const std::vector<iEdScnObject*>& GetFilteredEntities() { return mvFilteredEntities; }
	//const tSubMeshEntityVec& GetFilteredSubMeshes() { return mvFilteredSubMeshes; }

	void SetAverageNormal(bool abX) { mbAverageNormal=abX; }
	void SetAverageVolumeSize(const cVector3f&);
	void SetAverageBackfacingAngleThreshold(float);

	bool Affects(iEdScnObject*);
protected:
	int ToSurfaceTypeID(int);
	int ToEntityTypeID(int);

	void OnDraw(cRendererCallbackFunctions* apFunctions);

	void Iterate();
	void IterateRenderableNode(iRenderableContainerNode *apNode, const cVector3f& avStart, const cVector3f& avEnd, cBoundingVolume *apBV, int* apTriIndex, bool abAveragingNormal=false);
	bool CheckObjectIntersection(iRenderable *apObject, const cVector3f& avStart, const cVector3f& avEnd, cBoundingVolume *apBV, int *apTriIndex, bool abAveragingNormal=false);

	void ComputeAverageNormal(const cVector3f&);

	///////////////////////////////////////////////////////////////////
	// Data
	float mfMinT;
	float mfRayLength;

	iEdScnObject* mpClosestSurfaceObject;
	cSubMeshEntity* mpClosestMeshEntity;
	tVector3fVec mvTriangle;
	cVector3f mvIntersection;
	cVector3f mvTriangleNormal;
	cVector3f mvRight;
	cVector3f mvForward;

	cVector3f mvRightBase;
	cVector3f mvForwardBase;

	//std::vector<iEdScnObject*> mvFilteredEntities;
	//tSubMeshEntityVec mvFilteredSubMeshes;

	bool mbAverageNormal;
	cBoundingVolume mAvgVolume;
	float mfBackfacingAngleThreshold;
	tSubMeshEntityVec mvSubMeshesToAverage;

	tVector3fVec mvTrianglesToAverage;

	cVector3f mvAverageRight;
	cVector3f mvAverageUp;
	cVector3f mvAverageForward;

	cUIPickFilter* mpSurfaceFilter;
};

//----------------------------------------------------------------------------------

#endif // ED_SURFACE_PICKER_H
