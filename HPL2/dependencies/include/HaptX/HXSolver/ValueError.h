
//! File: <ValueError.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_VALUEERROR_H_
#define _HAPTX_VALUEERROR_H_

#include "Error.h"

namespace HaptX
{
	namespace Error
	{
		// ValueError is a template to instantiate a sub-class of Error.
		// Unfortunately, string literals cannot be used as template
		// parameters, so ValueError has a pure virtual member function
		// const char* ErrorName() that is used to print the name of the
		// error. ValueError should be subclassed using the VALUE_ERROR
		// macro from Macro.h.
		// Example:
		// HAPTX_VALUE_ERROR(const int, BadIndex)
		// declares a subclass of ValueError named BadIndex.
		template<class T>
		class ValueError : public ErrorBase
		{
		public:
			typedef T ValueType;
			inline ValueError(ValueType _value, const std::wstring& _location = "unspecified", const std::wstring& _filename = "", const int& _linenumber = 0) : ErrorBase(_location,_filename,_linenumber), value(_value)
			{
			}

			inline ValueError(ValueType _value, const std::wstring& _filename, const int& _linenumber ) : ErrorBase(_filename,_linenumber), value(_value)
			{
			}
      
			virtual void print(std::wostream &o) const
			{
				WCHAR Tmp[255];
				#if _MSC_VER == 1400
					size_t Len;
					mbstowcs_s(&Len,Tmp,255,ErrorName(),255);
				#else
					mbstowcs(Tmp,ErrorName(),255);
				#endif

				o << Tmp;
				o << " : ";
				o << value;
		
				o << " (";
				o << m_location;
				if (m_filename.length())
				{
					if (m_location.length()) o << " ";
					o << m_filename;
					if (m_linenumber != 0) o << ":" << m_linenumber;
				}
				o << ")";
			}
      
		protected:
			// ErrorName must be overriden by subclasses to provide the name
			// of the class. The HAPTX_VALUE_ERROR macro in Macro.h may be used to
			// easily sub-class ValueError.
			virtual const char* ErrorName() const = 0;
      
		public:
			ValueType value;
		};
	}
}

#endif
