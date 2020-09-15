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

#ifndef HPL_INPUTDEVICE_H
#define HPL_INPUTDEVICE_H

#include "system/SystemTypes.h"
#include "input/InputTypes.h"

namespace hpl {
	
	class iInputDevice
	{
	public:
		iInputDevice(tString asName,eInputDeviceType aType);
		virtual ~iInputDevice(){}
			
		/**
		 *
		 * \return name of the device
		 */
		tString GetName();
		
		/**
		 *
		 * \return type of device.
		 */
		eInputDeviceType GetType();
	    
		/**
		 * Update the device, called by cInput
		 */
		virtual void Update()=0;

	private:
		tString msName;
		eInputDeviceType mType;
	};

};
#endif // HPL_INPUTDEVICE_H
