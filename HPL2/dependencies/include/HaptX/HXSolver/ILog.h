
//! File: <ILog.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_ILOG_H_
#define _HAPTX_ILOG_H_

#include <sstream>

#include "HXSolver.h"
#include "HaptXDefines.h"
#include "LinearAlgebra.h"

typedef void (*pfnMessageCallBack)(int Severity, std::wstring Message);

namespace HaptX
{
	HAPTX_SOLVER_SPEC void RegisterMessageCallback(int MinimumSeverity, pfnMessageCallBack CallbackFunc);
	HAPTX_SOLVER_SPEC void LogString(int Severity, std::wstring Message);

	// Macros to make it easier to output messages to the log. 
	#define HAPTX_LOG_DEBUG(expr)				\
	{											\
		std::wstringstream HXmsg;				\
		HXmsg << expr << std::endl;				\
		LogString(HX_DBG_MSG,HXmsg.str());		\
	}

	// Macros to make it easier to output messages to the log. 
	#define HAPTX_LOG_INFO(expr)				\
	{											\
		std::wstringstream HXmsg;				\
		HXmsg << expr << std::endl;				\
		LogString(HX_INFO_MSG,HXmsg.str());		\
	}

	// Macros to make it easier to output messages to the log. 
	#define HAPTX_LOG_WARNING(expr)				\
	{											\
		std::wstringstream HXmsg;				\
		HXmsg << expr << std::endl;				\
		LogString(HX_WARN_MSG,HXmsg.str());		\
	}

	// Macros to make it easier to output messages to the log. 
	#define HAPTX_LOG_ERROR(expr)				\
	{											\
		std::wstringstream HXmsg;				\
		HXmsg << expr << std::endl;				\
		LogString(HX_ERR_MSG,HXmsg.str());		\
	}

	#define HAPTX_LOG_HAPTICS(expr)				\
	{											\
		std::wstringstream HXmsg;				\
		HXmsg << expr << std::endl;				\
		LogString(HX_HAPTIC_LOG,HXmsg.str());	\
	}
}

#endif
