
//! File: <SimpleVector.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_SIMPLEVECTOR_H_
#define _HAPTX_SIMPLEVECTOR_H_

#include <vector>
#include <memory>

namespace HaptX
{
	template< class T >
	struct SimpleVector
	{
	public:
		SimpleVector() : _size(0), first(0)							{}
		SimpleVector(const size_t& n) : _size(n), first(new T[n])	{}
		~SimpleVector() 
		{
			if (first) delete [] first;
		}

		SimpleVector& operator = (SimpleVector& rhs)
		{
			resize(rhs.size());
			T *r = rhs.begin();
			for (T* i = begin(); i != end(); )
			{
				*i++ = *r++;
			}

			return *this;
		}
	
		T& operator [] (const size_t& i)
		{
			return first[i];
		}

		const T& operator [] (const size_t& i) const
		{ 
			return first[i];
		}

		void push_back(T& t)
		{
			grow();
			first[_size-1] = t;
		}

		void grow()
		{
			resize( _size+1 );
		}

		void resize(size_t s)
		{
			if (s > _size)
			{
				T* save = first;
				first = new T[s];
				for (size_t i = 0; i < _size; ++i)
				{
					first[i] = save[i];
				}
				
				if (save) delete [] save;
				
				_size = s;
			}
			else
			{
				T* save = first;
				first = new T[s];
				for(size_t i = 0; i < s; ++i)
				{
					first[i] = save[i];
				}

				if (save) delete [] save;

				_size = s;		
			}
		}

		T* begin()
		{
			return first; 
		}

		const T* begin() const 
		{ 
			return first;
		}

		T* end()
		{ 
			return first + _size;
		}

		const T* end() const
		{ 
			return first + _size;
		}

		size_t size() const
		{
			return _size;
		}

	private:
		T*		first;
		size_t	_size;
	};
}

#endif
