
//! File: <StateStack.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_STATESTACK_H_
#define _HAPTX_STATESTACK_H_

#include <vector>
#include <stack>

#include "ValueError.h"

namespace HaptX 
{
	// Error thrown by StateStack.
	HAPTX_VALUE_ERROR(const int, EmptyStack);

	// StateStack maintains a stack of state descriptions appropriate for
	// a traversal into and out of coordinate spaces.
	// Typically, reset will be called to install a single T instance
	// on the stack. When moving into a nested coordinate space we Push()
	// to duplicate the top of the stack. Then we can modify the new top
	// of stack as appropriate for the nested space, finally we Pop() to
	// return to the enclosing coordinate space.
	template<class T>
	class StateStack
	{
	public:
		inline void			Reset();								// Reset sets the stack to contain a single new T instance.
		inline void			Push() throw(Error::EmptyStack);		// Push a copy of the top of the stack onto the stack.
		inline void			Pop() throw(Error::EmptyStack);			// Pop the stack
		inline T&			Top() throw(Error::EmptyStack);			// Get the top of the stack.
		inline const T&		Top() const throw(Error::EmptyStack);	// Get the top of the stack.
		inline bool			Empty() const;							// Test for emptyness

	protected:
		std::stack<T,std::vector<T> > stack;
	};

  
	//! inline member functions
  
	//! reset sets the stack to contain a single new T instance
	template<class T>
	inline void StateStack<T>::Reset()
	{
		while (!stack.empty()) stack.pop();
		stack.push(T());
	}

	//! push a copy of the top of the stack onto the stack
	template< class T >
	inline void StateStack<T>::Push() throw( Error::EmptyStack )
	{
		if (stack.empty()) throw Error::EmptyStack(0,L"StateStack<T>::push");
		stack.push(stack.top());
	}
	
	//! pop the stack
	template<class T>
	inline void StateStack<T>::Pop() throw(Error::EmptyStack)
	{
		if (stack.empty()) throw Error::EmptyStack(0,L"StateStack<T>::pop");
		stack.pop();
	}

	//! get the top of the stack
	template<class T>
	inline T& StateStack<T>::Top() throw(Error::EmptyStack)
	{
		if (stack.empty()) throw Error::EmptyStack(0,L"StateStack<T>::getTop");
		return stack.top();
	}

	//! get the top of the stack
	template<class T>
	inline const T &StateStack<T>::Top() const throw(Error::EmptyStack)
	{
		if (stack.empty()) throw Error::EmptyStack(0,L"StateStack<T>::getTop");
		return stack.top();
	}

	//! test for emptyness  
	template<class T>
	inline bool StateStack<T>::Empty() const
	{
		return stack.empty();
	}
}

#endif
