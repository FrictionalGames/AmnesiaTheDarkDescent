
//! File: <Error.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

//! Almost all usages of Error are via ValueError 

#pragma warning(disable: 4251)	//! Disable warning bescause STL templates are exported implicitly.

#ifndef _HAPTX_ERROR_H_
#define _HAPTX_ERROR_H_

#include <iostream>
#include <stdlib.h>

#include <string>

#include "HXSolver.h"
#include "Macro.h"

namespace HaptX 
{
	namespace Error
	{
		// The base class of the exceptions thrown in HaptX. 
		class HAPTX_SOLVER_SPEC HXException 
		{
		};

		// ErrorBase is the generic runtime error.
		class HAPTX_SOLVER_SPEC ErrorBase: public HXException 
		{
		public:

			// An error location may be specified in the constructor. Additionally, the filename and linenumber may be provided (see
			// the HAPTX_FN macro in Macro.h). This will be printed when the error is sent to an ostream and may include additional information. 
			inline ErrorBase(const std::wstring& _location = L"unspecified", const std::wstring& _filename = L"", const int &_linenumber = 0);
			inline ErrorBase(const std::wstring& _filename, const int& _linenumber);		// If the location is ommited, the filename and linenumber are used. 

			virtual void				Print(std::wostream& o) const;						// Print out a representation of the error to the given wostream. 
			inline const std::wstring&	GetLocation() const;								// Get the location. 

			friend std::wostream&		operator <<	(std::wostream& o, ErrorBase& e);		// The << operator simply calls the virtual print function. 
			
		public:
			std::wstring	m_location;					// The location of the error. This may include a reason. 
			std::wstring	m_filename;					// The location of the error. 
			int				m_linenumber;				// The line number of the error. 
		};


		//! An error location may be specified in the constructor.
		//! Additionally, the filename and linenumber may be provided (see
		//! the HAPTX_FN macro in Macro.h).
		//! This will be printed when the error is sent to an ostream and
		//! may include additional information. 
		inline ErrorBase::ErrorBase(const std::wstring& _location, const std::wstring& _filename, const int& _linenumber) : m_location(_location), m_filename(_filename), m_linenumber(_linenumber) 
		{
		}

		//! If the location is ommited, the filename and linenumber are used. 
		inline ErrorBase::ErrorBase(const std::wstring& _filename, const int& _linenumber) : m_location(L""), m_filename(_filename), m_linenumber(_linenumber)
		{
		}

		//! Print out a representation of the error to the given wostream. 
		inline void ErrorBase::Print(std::wostream& o) const 
		{
			o << "ErrorBase : " << m_location;
			if (m_filename.length()) 
			{
				if (m_location.length()) o << " ";
				o << m_filename;
				if (m_linenumber != 0) o << ":" << m_linenumber;
			}
		}
		
		//! Get the location. 
		inline const std::wstring& ErrorBase::GetLocation() const 
		{
			return m_location;
		}
	    
		//! The << operator simply calls the virtual print function.
		inline std::wostream& operator << (std::wostream& o, ErrorBase& e)
		{
			e.Print(o);
			return o;
		}

		//! QuitHaptX is an error that is thrown when the application
		//! wants to quit. This gives the main() function a chance to
		//! capture the quit error and perform cleanup functions before
		//! allowing the program to terminate.
		class HAPTX_SOLVER_SPEC QuitHaptX : public HXException 
		{
		};
	}
}

#endif
