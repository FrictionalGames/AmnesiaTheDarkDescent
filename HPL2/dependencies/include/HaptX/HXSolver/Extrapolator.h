
//! File: <Exrapolator.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_EXTRAPOLATOR_H_
#define _HAPTX_EXTRAPOLATOR_H_

#include "mgTime.h"
#include "LinearAlgebra.h"

// operators used by the LinearExtrapolator: T = T, T - T, T * mgFloat

// Using the extrapolator (alternative A):
// Init: myExtrapolator.Init(init_value, T(0.0...0.0));
// Render Loop: myValue = myExtrapolator.GetOldValue(); myExtrapolator.SetNewValue(new_value);
// Haptic Loop: myExtrapolator.UpdateExtrapolation(); myValue += myExtrapolator.GetExtrapolation();

// Using the extrapolator (alternative B: if we know that the haptic loop runs at 1000Hz and not less or more):
// Init: myExtrapolator.Init(init_value, T(0.0...0.0)); myExtrapolator.ManuallySetUpdateExtrapolation(1000);
// Render Loop: myValue = myExtrapolator.GetOldValue(); myExtrapolator.SetNewValue(new_value);
// Haptic Loop: myValue += myExtrapolator.GetExtrapolation();

// Using the extrapolator (alternative C):
// Init: myExtrapolator.Init(init_value, T(0.0...0.0));
// Render Loop: myExtrapolator.SetOldValue(myValue); myExtrapolator.SetNewValue(new_value);
// Haptic Loop: myExtrapolator.UpdateExtrapolation(); myValue += myExtrapolator.GetExtrapolation();

// Using the extrapolator (alternative D: if we know that the haptic loop runs at 1000Hz and not less or more):
// Init: myExtrapolator.Init(init_value, T(0.0...0.0)); myExtrapolator.ManuallySetUpdateExtrapolation(1000);
// Render Loop: myExtrapolator.SetOldValue(myValue); myExtrapolator.SetNewValue(new_value);
// Haptic Loop: myValue += myExtrapolator.GetExtrapolation();

// TODO (lukas): Alternative B, C and D have not been tested.

#define EXTRAPOLATOR_UPDATE_FREQ	0.2		// time in seconds to next FPS update

namespace HaptX
{
	template<class T>
	class LinearExtrapolator
	{
	public:
		LinearExtrapolator() : m_divisor(0.0), m_fpsCore(1.0), m_fpsApp(1.0), m_frameCountCore(1), m_frameCountApp(1)
		{
		}

		LinearExtrapolator(const T& init_value, bool set_for_old_value)
		{
			Init(init_value,set_for_old_value);
		}

		LinearExtrapolator(const T& init_value, const T& init_extrapolator)
		{
			Init(init_value,init_extrapolator);
		}

		~LinearExtrapolator()
		{
		}

		void Init(const T& init_value, bool set_for_old_value)
		{
			m_divisor = 0.0;
			m_fpsCore = 1.0;
			m_fpsApp = 1.0;
			m_frameCountCore = 1;
			m_frameCountApp = 1;

			if (set_for_old_value)
			{
				m_oldValue = init_value;
			}
			else
			{
				m_plusValue = init_value;
			}
		}

		void Init(const T& init_value, const T& init_extrapolator)
		{
			m_divisor = 0.0;
			m_fpsCore = 1.0;
			m_fpsApp = 1.0;
			m_frameCountCore = 1;
			m_frameCountApp = 1;

			m_oldValue = init_value;
			m_plusValue = init_extrapolator;
		}

		void SetNewValue(const T& new_value)
		{
			Time time_new;
			m_frameCountApp++;

			// compute FPS
			if (mgAbs(time_new - m_lastTime) >= EXTRAPOLATOR_UPDATE_FREQ)
			{
				m_fpsApp = (HaptX::mgFloat)m_frameCountApp / abs(time_new - m_lastTime);
				if (m_fpsApp == 0.0) m_fpsApp = HaptX::Util::epsilon;

				m_fpsCore = (HaptX::mgFloat)m_frameCountCore / abs(time_new - m_lastTime);
				if (m_fpsCore == 0.0) m_fpsCore = HaptX::Util::epsilon;
						
				m_frameCountApp = 0;
				m_frameCountCore = 0;
				m_lastTime = time_new;
						
				m_divisor = m_fpsCore / m_fpsApp;
				if (m_divisor <= 0.0) m_divisor = HaptX::Util::epsilon;
				m_divisor = 1.0 / m_divisor;

				if (m_fpsCore < m_fpsApp) m_divisor = 0.0;
			}

			m_plusValue = (new_value - m_oldValue) * m_divisor;
			m_oldValue = new_value;
		}

		void ManuallySetUpdateExtrapolation(int frame_count)
		{
			m_frameCountCore = (frame_count < 1)? 1 : frame_count;
		}

		inline void SetOldValue(const T& p)
		{
			m_oldValue = p;
		}

		inline void UpdateExtrapolation()
		{
			m_frameCountCore++;
		}

		inline T GetExtrapolation()
		{
			return m_plusValue;
		}

		inline T GetOldValue()
		{
			return m_oldValue;
		}

		void operator = (const LinearExtrapolator<T>& v)
		{
			m_fpsCore			= v.m_fpsCore;
			m_fpsApp			= v.m_fpsApp;
			m_frameCountCore	= v.m_frameCountCore;
			m_frameCountApp		= v.m_frameCountApp;
			m_oldValue			= v.m_oldValue;
			m_plusValue			= v.m_plusValue;
		}

	private:
		T				m_oldValue;
		T				m_plusValue;
		HaptX::Time		m_lastTime;
		int				m_frameCountCore;
		int				m_frameCountApp;
		HaptX::mgFloat	m_fpsCore;
		HaptX::mgFloat	m_fpsApp;
		HaptX::mgFloat	m_divisor;
	};
}

#endif
