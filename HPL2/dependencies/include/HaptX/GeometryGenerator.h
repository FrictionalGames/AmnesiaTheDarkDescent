
//! File: <GeometryGenerator.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _GEOMETRY_GENERATOR_H_
#define _GEOMETRY_GENERATOR_H_

#include "HXSolver/RealtimeTriangleSet.h"

namespace HaptX
{
	HX_RESULT GeometryCreateBox(RealtimeTriangleSet* rm, float xl, float yl, float zl, bool flipNormals, bool leftHanded);
	HX_RESULT GeometryCreateSphere(RealtimeTriangleSet* rm, float radius, int segments, bool flipNormals, bool leftHanded);
	HX_RESULT GeometryCreateCone(RealtimeTriangleSet* rm, float radius, float height, int segments, bool flipNormals, bool leftHanded);
	HX_RESULT GeometryCreateTube(RealtimeTriangleSet* rm, float bottomInnerRadius, float bottomOuterRadius, float topInnerRadius, float topOuterRadius, float height, int segments, bool flipNormals, bool leftHanded);
	HX_RESULT GeometryCreateCylinder(RealtimeTriangleSet* rm, float bottomRadius, float topRadius, float height, int segments, bool flipNormals, bool leftHanded);
};

#endif
