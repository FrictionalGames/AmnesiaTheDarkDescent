
//! File: <Version.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_VERSION_H_
#define _HAPTX_VERSION_H_

#include "HXSolver.h"
#include <string>

namespace HaptX
{
	HINSTANCE GetInst();

	bool UpdateVersionValues();

	// Returns the build version of the current dll's used e.g. "Microsoft Visual C++ 8.0"
	char* GetBuildSystem();

	// GetMajorVersion returns the major version of HaptX e.g 1 if the version is 1.7.0.57
	unsigned int GetMajorVersion();
  
	// GetMinorVersion returns the minor version of HaptX e.g 7 if the version is 1.7.0.57
	unsigned int GetMinorVersion();
  
	// GetRevisionVersion returns the revision version or patch version of HaptX e.g 0 if the version is 1.7.0.57
	unsigned int GetRevisionVersion();
  
	// GetBuildVersion returns the build version of HaptX e.g 57 if the version is 1.7.0.57
	unsigned int GetBuildVersion();

	bool HAPTX_SOLVER_SPEC GetHXVersion(unsigned int& Maj,unsigned int& Min, unsigned int& Rev, unsigned int& Build);
}

#endif
