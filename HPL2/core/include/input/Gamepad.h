/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HPL_GAMEPAD_H
#define HPL_GAMEPAD_H

#include "input/InputTypes.h"
#include "input/InputDevice.h"

#include "math/MathTypes.h"

namespace hpl {

	//------------------------------

	class iGamepad : public iInputDevice
	{
	public:
		iGamepad(tString asName, int alIndex);
		virtual ~iGamepad(){}
		
		/**
		 * 
		 * \return the system name of the gamepad device
		 */
		virtual tString GetGamepadName()=0;
		
		/**
		 * 
		 * \return the number of buttons in the gamepad 
		 */
		virtual int GetNumButtons()=0;
		/**
		 * 
		 * \return the number of axes in the gamepad 
		 */
		virtual int GetNumAxes()=0;
		/**
		 * 
		 * \return the number of hats in the gamepad 
		 */
		virtual int GetNumHats()=0;
		/**
		 * 
		 * \return the number of trackballs in the gamepad 
		 */
		virtual int GetNumBalls()=0;
		/**
		 * 
 		 * Can be checked many times to see all updated input
		 * \return  inputs
		 */
		virtual cGamepadInputData GetInputUpdate()=0;
		/**
		 * 
		 * \return true If ANY input is updated
		 */
		virtual bool HasInputUpdates()=0;
		/**
		 * 
		 * \param aButton The button to check 
		 * \return true if pressed else false 
		 */
		virtual bool ButtonIsDown(eGamepadButton aButton)=0;
		/**
		 * Can be checked many times to see all button presses
		 * \return key that is currently pressed. eKey_NONE is no key. 
		 */
		virtual cGamepadInputData GetButton()=0;
		/**
		 *
		 * \return If ANY button is pressed
		 */
		virtual bool ButtonIsPressed()=0;
		/**
		 *
		 * \return button that has been released
		 */
		virtual cGamepadInputData GetReleasedButton()=0;
		/**
		 *
		 * \return If ANY button is released
		 */
		virtual bool ButtonIsReleased()=0;
		/**
		 * \param aAxis the axis to get the value from.
		 * \return The normalized input value for the axis.
		 */
		virtual float GetAxisValue(eGamepadAxis aAxis)=0;
		/**
		 * 
		 * \return Threshold indicating the at which start value input will be considered.
		 */
		virtual float GetAxisDeadZoneRadiusValue()=0;
		/**
		 * \param New value for the dead zone radius 
		 */
		virtual void SetAxisDeadZoneRadiusValue(float afValue)=0;
		/**
		 * \param aHat the hat to get the state from.
		 * \return The state of the hat.
		 */
		virtual cGamepadInputData GetUpdatedAxis()=0;
		/**
		 * \param aHat the hat to get the state from.
		 * \return The state of the hat.
		 */
		virtual bool AxesUpdated()=0;
		/**
		 * \param aHat the hat to get the state from.
		 * \return The state of the hat.
		 */
		static eGamepadAxisRange GetAxisRangeFromValue(float afValue);
		/**
		 * \param aHat the hat to get the state from.
		 * \return The state of the hat.
		 */
		virtual eGamepadHatState GetHatCurrentState(eGamepadHat aHat)=0;
		/**
		 * \param aHat the hat to get the state from.
		 * \return The state of the hat.
		 */
		virtual bool HatIsInState(eGamepadHat aHat, eGamepadHatState aState)=0;
		/**
		 * \param aHat the hat to get the state from.
		 * \return The state of the hat.
		 */
		virtual cGamepadInputData GetHatState()=0;
		/**
		 * \param aHat the hat to get the state from.
		 * \return The state of the hat.
		 */
		virtual bool HatsChanged()=0;
		/**
		 * \param aBall the ball to get the absolute position from.
		 * \return Absolute position for the ball.
		 */
		virtual cVector2l GetBallAbsPos(eGamepadBall aBall)=0;
		/**
		 * \param aBall the ball to get the Relative position from.
		 * \return Relative position for the ball.
		 */
		virtual cVector2l GetBallRelPos(eGamepadBall aBall)=0;
		/**
		 * \param aButton The button to change to string.
		 * \return The name of the button as a string. 
		 */
		static tString ButtonToString(eGamepadButton aButton);
		/**
		 * \param tString Name of the button
		 * \return enum of the button.
		 */
		static eGamepadButton StringToButton(const tString&);
		/**
		 * \param aButton The button to change to string.
		 * \return The name of the button as a string. 
		 */
		static tString AxisToString(eGamepadAxis aAxis);
		/**
		 * \param tString Name of the axis.
		 * \return enum of the axis.
		 */
		static eGamepadAxis StringToAxis(const tString&);
		/**
		 * \param aButton The button to change to string.
		 * \return The name of the button as a string. 
		 */
		static tString AxisRangeToString(eGamepadAxisRange aAxis);
		/**
		 * \param tString Name of the range
		 * \return enum of the range.
		 */
		static eGamepadAxisRange StringToAxisRange(const tString&);
		/**
		 * \param aButton The hat to change to string.
		 * \return The name of the hat as a string. 
		 */
		static tString HatToString(eGamepadHat aButton);
		/**
		 * \param tString Name of the hat
		 * \return enum of the hat.
		 */
		static eGamepadHat StringToHat(const tString&);
		/**
		 * \param aButton The hat state to change to string.
		 * \return The name of the hat state as a string. 
		 */
		static tString HatStateToString(eGamepadHatState aButton);
		/**
		 * \param tString Name of the hat state
		 * \return enum of the hat state.
		 */
		static eGamepadHatState StringToHatState(const tString&);
		/**
		 * \param aButton The trackball to change to string.
		 * \return The name of the trackball as a string. 
		 */
		static tString BallToString(eGamepadBall aButton);
		/**
		 * \param tString Name of the trackball
		 * \return enum of the trackball.
		 */
		static eGamepadBall StringToBall(const tString&);
	protected:
		int mlIndex;
	};

	//------------------------------

};

#endif // HPL_GAMEPAD_H
