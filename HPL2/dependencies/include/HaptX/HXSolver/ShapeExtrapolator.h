
//! File: <ShapeExtrapolator.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_SHAPE_EXTRAPOLATOR_H_
#define _HAPTX_SHAPE_EXTRAPOLATOR_H_

#include <list>

#include "LinearAlgebra.h"
#include "Extrapolator.h"
#include "../ShapeManager.h"

#pragma warning(disable: 4251)	//! Disable warning bescause STL templates are exported implicitly.

namespace HaptX
{
	// a class that is added to the list of currently active extrapolated shapes
	class ShapeExtrapolationInfo
	{
	public:
		ShapeExtrapolationInfo(HapticShape* hs, Vectors::Matrix4f transform);
		ShapeExtrapolationInfo();
		~ShapeExtrapolationInfo();

		void operator = (const ShapeExtrapolationInfo& p);
		bool operator == (const ShapeExtrapolationInfo& p);
		bool operator != (const ShapeExtrapolationInfo& p);

	public:
		HapticShape*							m_hapticShape;				// the shape that is extrapolated
		LinearExtrapolator<Vectors::Matrix4f>	m_extrapolator;				// extrapolator
	};


	// manager that handles all the shapes that are extrapolated
	class HAPTX_SOLVER_SPEC ShapeExtrapolationManager
	{
	public:
		ShapeExtrapolationManager();
		~ShapeExtrapolationManager();

		void ShapeDestroy(HapticShape* hs);									// called when a haptic shape is deleted

		void AddShape(HapticShape* hs, Vectors::Matrix4f transform);		// add a new shape to extrapolate

		void UpdateRenderLoop();											// called each frame in the render loop
		void UpdateHapticLoop();											// called each frame in the haptic loop

	private:
		std::list<ShapeExtrapolationInfo>	m_shapeExtrapolationList;			// list with extrapolated shapes
	};

	HAPTX_SOLVER_SPEC ShapeExtrapolationManager*	GetShapeExtrapolationManager();				// Global function to get a pointer to the ShapeExtrapolationManager
	HAPTX_SOLVER_SPEC void							CreateShapeExtrapolationManager();			// Global function to create a ShapeExtrapolationManager
	HAPTX_SOLVER_SPEC void							ReleaseShapeExtrapolationManager();			// Global function to release the ShapeExtrapolationManager
}

#pragma warning(default: 4251)	//! Disable warning bescause STL templates are exported implicitly.

#endif
