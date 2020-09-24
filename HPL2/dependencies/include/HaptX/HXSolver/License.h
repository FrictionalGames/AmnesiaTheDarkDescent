
//! File: <License.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _LICENSE_H_
#define _LICENSE_H_

#ifndef _WIN64
#include <EXECryptor.h>
#endif
#include "HXSolver.h"

namespace HaptX
{
	typedef unsigned long HX_RESULT;

	HX_RESULT HAPTX_SOLVER_SPEC		InitHaptX();
	HX_RESULT HAPTX_SOLVER_SPEC		InitHaptX(wchar_t* Company, wchar_t* Product,wchar_t* Version, wchar_t* Platform, wchar_t* LicenseKey);
	HX_RESULT						VerifyLicense(wchar_t* SeedStr, wchar_t* License);
	HX_RESULT						VerifyDeveloperLicense();
	bool							IsInitiated();
	int								GetLicenseType();
}

#endif
