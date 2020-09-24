
//! File: <Macro.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_MACRO_H_
#define _HAPTX_MACRO_H_


#include <stdio.h>

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

// HAPTX_FN provides the filename and linenumber for use with various
// ValueError constructors. For example:
// throw Error::BadIndex(34,"FooList:add",HAPTX_FN);
#define HAPTX_FN __WFILE__,__LINE__


// HAPTX_VALUE_ERROR creates a subclass of ValueError for the given
// value type and class name and places it in the HaptX::Error
// namespace.
#define HAPTX_VALUE_ERROR(value_type, error_name)                       				\
namespace Error																			\
{																						\
	class error_name : public ValueError<value_type>									\
	{																					\
	public:																				\
		error_name(ValueType _value, const std::wstring& _location = L"unspecified",	\
                   const std::wstring& _filename = L"", const int& _linenumber = 0) :	\
				   ValueError<ValueType>(_value,_location,_filename,_linenumber) {}		\
																						\
		error_name(ValueType _value, const std::wstring& _filename,						\
                   const int& _linenumber) :											\
				   ValueError<ValueType >(_value,_filename,_linenumber) {}              \
																						\
	protected:																			\
		const char* ErrorName() const { return #error_name; }							\
	};																					\
}

#endif
