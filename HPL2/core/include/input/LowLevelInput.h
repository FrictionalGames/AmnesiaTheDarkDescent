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

#ifndef HPL_LOWLEVELINPUT_H
#define HPL_LOWLEVELINPUT_H

namespace hpl {

	class iMouse;
	class iKeyboard;
	class iGamepad;

	class iLowLevelInput
	{
	public:
		virtual ~iLowLevelInput(){}
		
		/**
		 * Lock all input to current window.
		 * \param abX 
		 * \return 
		 */
		virtual void LockInput(bool abX)=0;
		/**
		 * Use relative mouse mode
		 * \param abX
		 * \return
		 */
		virtual void RelativeMouse(bool abX)=0;
		/**
		 * Called by cInput
		 */
		virtual void BeginInputUpdate()=0;
		/**
		 * called by cInput
		 */
		virtual void EndInputUpdate()=0;

		virtual bool isQuitMessagePosted()=0;
        virtual void resetQuitMessagePosted()=0;

		/**
		 * 
		 */
		virtual void InitGamepadSupport()=0;
		virtual void DropGamepadSupport()=0;

		virtual int GetPluggedGamepadNum()=0;

		virtual iMouse* CreateMouse()=0;
		virtual iKeyboard* CreateKeyboard()=0;
		virtual iGamepad* CreateGamepad(int alIndex)=0;
	};
};
#endif // HPL_LOWLEVELINPUT_H
