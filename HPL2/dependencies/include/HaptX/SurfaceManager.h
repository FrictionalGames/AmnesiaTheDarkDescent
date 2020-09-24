
//! File: <SurfaceManager.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef	_HAPTX_SURFACEMANAGER_H_
#define _HAPTX_SURFACEMANAGER_H_

#include <map>
#include <list>

#include "HXEngine.h"
#include "HXSolver/LinearAlgebra.h"
#include "HXSolver/HaptXDefines.h"

namespace HaptX
{
	// forward declarations
	class Surface;
	class SurfaceManager;

	SurfaceManager*	GetSurfaceManager();		// Global function. Get a pointer to the SurfaceManager.
	void			CreateSurfaceManager();		// Global function. Create a SurfaceManager.
	void			ReleaseSurfaceManager();	// Global function. Release the SurfaceManager.

	// The SurfaceManager handles all registered HaptX Surfaces. When a surface gets registered in the
	// SurfaceManager an ID is assigned to the surface. The SurfaceManager also takes care of deletion 
	// of surfaces. Before a surface is deleted, the SurfaceManager makes sure there are no references 
	// to the particular surface still in use in the HaptX loop. The SurfaceManager also has functionality 
	// for enabling and disabling Surface Haptics.
	class SurfaceManager
	{
	public:
					SurfaceManager();								// Constructor.
					~SurfaceManager();								// Destructor.

		iSurfaceID	RegisterSurface(Surface* surface);				// Puts the surface in the m_mapSurfaces and assigns an ID to it. The ID is returned.
		Surface*	GetSurface(iSurfaceID surfaceID);				// Returns a pointer to the surface instance with the given ID. Null if the id doesn't exist.
		void		SurfacesSetEnabled(bool enable);				// Enables or disables all Surface Haptics.
		bool		SurfacesIsEnabled();							// Returns whether Surface Haptics is enabled or not.
		bool		SurfaceExists(iSurfaceID surfaceID);			// Returns true if a surface with the surfaceID exists.
		bool		DestroySurface(iSurfaceID surfaceID);			// Places the surface instance in the m_listDeadSurfaces. A HapticShape holding an ID to a deleted surface will not be rendered.
		bool		CleanUpDeadSurfaces();							// Tries to delete the surfaces in m_listDeadSurfaces. If the HaptX loop still holds a reference to a surface it is not deleted.

	protected:
		int								m_nextID;					// The ID that the next surface will get.
		bool							m_bSurfaceHapticsEnabled;	// No HaptX Shapes are rendered if set to false.
		std::map<iSurfaceID, Surface*>	m_mapSurfaces;				// A map which contains all surface instances and their surface ID's.
		std::list<Surface*>				m_listDeadSurfaces;			// A list of all surfaces that should be destroyed as soon as the HaptX loop has no more references to them.
	};
}

#endif
