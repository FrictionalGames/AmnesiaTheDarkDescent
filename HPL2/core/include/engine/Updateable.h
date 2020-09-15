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

#ifndef HPL_UPDATEABLE_H
#define HPL_UPDATEABLE_H

#include "engine/EngineTypes.h"
#include "system/SystemTypes.h"

namespace hpl {
	
	class iUpdateable
	{
	public:
		iUpdateable(const tString& asName) : msName(asName){}
		virtual ~iUpdateable() {}

		virtual void OnPostBufferSwap(){}

		virtual void OnStart(){}

		virtual void OnDraw(float afFrameTime){}
		virtual void OnPostRender(float afFrameTime){}
		
		virtual void PreUpdate(float afTimeStep){}
		virtual void Update(float afTimeStep){}
		virtual void PostUpdate(float afTimeStep){}

		virtual void OnPauseUpdate(float afTimeStep){}

		virtual void OnQuit(){}
		virtual void OnExit(){}

		virtual void Reset(){}

		virtual void OnEnterContainer(const tString& asOldContainer){}
		virtual void OnLeaveContainer(const tString& asNewContainer){}

		virtual void AppGotInputFocus(){}
		virtual void AppGotMouseFocus(){}
		virtual void AppGotVisibility(){}

		virtual void AppLostInputFocus(){}
		virtual void AppLostMouseFocus(){}
		virtual void AppLostVisibility(){}

		virtual void AppDeviceWasPlugged(){}
		virtual void AppDeviceWasRemoved(){}

		void RunMessage(eUpdateableMessage aMessage, float afX)
		{
			switch(aMessage)
			{
				case eUpdateableMessage_OnPostBufferSwap:
					OnPostBufferSwap(); break;
				case eUpdateableMessage_OnStart:
					OnStart(); break;
				case eUpdateableMessage_OnDraw:
					OnDraw(afX); break;
				case eUpdateableMessage_OnPostRender:
					OnPostRender(afX); break;
				case eUpdateableMessage_PreUpdate:
					PreUpdate(afX); break;
				case eUpdateableMessage_Update:
					Update(afX); break;
				case eUpdateableMessage_PostUpdate:
					PostUpdate(afX); break;
				case eUpdateableMessage_OnQuit:
					OnQuit(); break;
				case eUpdateableMessage_OnExit:
					OnExit(); break;
				case eUpdateableMessage_Reset:
					Reset(); break;
				case eUpdateableMessage_OnPauseUpdate:
					OnPauseUpdate(afX);break;
				
				case eUpdateableMessage_AppGotInputFocus:
					AppGotInputFocus();break;
				case eUpdateableMessage_AppGotMouseFocus:
					AppGotMouseFocus();break;
				case eUpdateableMessage_AppGotVisibility:
					AppGotVisibility();break;

				case eUpdateableMessage_AppLostInputFocus:
					AppLostInputFocus();break;
				case eUpdateableMessage_AppLostMouseFocus:
					AppLostMouseFocus();break;
				case eUpdateableMessage_AppLostVisibility:
					AppLostVisibility();break;

				case eUpdateableMessage_AppDeviceWasPlugged:
					AppDeviceWasPlugged();break;
				case eUpdateableMessage_AppDeviceWasRemoved:
					AppDeviceWasRemoved();break;
			};
		}
		
		const tString& GetName(){ return msName;}

	private:
		tString msName;
	};
};

#endif // HPL_UPDATEABLE_H
