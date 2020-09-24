
//! File: <ForceModelManager.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_FORCEMODELMANAGER_H_
#define _HAPTX_FORCEMODELMANAGER_H_

#include <map>
#include <list>

#include "HXEngine.h"
#include "HXSolver/HaptXDefines.h"

#pragma warning(disable: 4251)	//! Disable warning bescause STL templates are exported implicitly.

namespace HaptX
{
	//forward declaration
	class ForceModelManager;
	class ForceModel;

	ForceModelManager*	GetForceModelManager();			// Global function to get a pointer to the ForceModelManager.
	void				CreateForceModelManager();		// Global function to create a ForceModelManager.
	void				ReleaseForceModelManager();		// Global function to release the ForceModelManager.

	// The ForceModelManager handles all registerd ForceModels. When a ForceModel gets registered in the ForceModelManager
	// an ID is assigned to the ForceModel. The ForceModelManager is also taking care of the deletion of ForceModels and makes
	// sure that no references are held from the HaptX thread before deleting a ForceModel.
	// The ForceModelManager also has functionality for enabling and disabling all ForceModel Haptics.
	class ForceModelManager
	{
	public:
						ForceModelManager();							// Constructor.
						~ForceModelManager();							// Destructor.

		iForceModelID	RegisterForceModel(ForceModel* fm);				// Puts the ForceModel in the m_mapForceModels and assigns an ID to it. The ID is returned.
		ForceModel*		GetForceModel(iForceModelID forceModelID);		// Returns a pointer to the ForceModel instance with the given ID. Null if the id doesn't exist.
		void			ForceModelsSetEnabled(bool enable);				// Enables or disables all ForceModel Haptics.
		bool			ForceModelsIsEnabled();							// Returns if ForceModel Haptics is enabled or not.
		bool			ForceModelExists(iForceModelID forceModelID);	// Returns true if a ForceModel with the forceModelID exists.
		bool			DestroyForceModel(iForceModelID forceModelID);	// Places the ForceModel instance in the m_listDeadForceModels and removes it from all render lists. 
		bool			CleanUpDeadForceModels();						// Tries to delete the ForceModels in m_listDeadForceModels. If the HaptX thread still holds references to a ForceModel it is not deleted.
		
	protected:
		int									m_nextID;					// The ID that the next ForceModel will get.
		bool								m_bForceModelHapticsEnabled;// If false no ForceModels are used in the HaptX rendering.
		std::list<ForceModel*>				m_listDeadForceModels;		// A list of all ForceModels that should be destoyed as soon as the HaptX thread has no more references to it.
		std::map<iForceModelID,ForceModel*>	m_mapForceModels;			// A map which contains all ForceModel instances and their ID's.
	};
}

#endif
