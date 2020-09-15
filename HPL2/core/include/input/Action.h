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

#ifndef HPL_ACTION_H
#define HPL_ACTION_H

#include "system/SystemTypes.h"
#include "input/InputTypes.h"

namespace hpl {


	class iSubAction
	{
	friend class cAction;
	public:
		virtual ~iSubAction(){}

		void Update(float afTimeStep);

		/**
		* Update special logic for the action. Normally empty
		*/
		virtual void UpdateLogic(float afTimeStep){}


		/**
		* Filled in by the class that inherits from Action.
		* \return true if the action is being triggered
		*/
		virtual bool IsTriggerd()=0;

		/**
		* 
		* \return A value from the input, ie the relative mouse x position.
		*/
		virtual float GetValue()=0;

		/**
		* The name of the input, ie for keyboard the name of the key is returned.
		*/
		virtual tString GetInputName()=0;

		/**
		* The name of the input type.
		*/
		virtual tString GetInputType()=0;
	};

	//---------------------------------------------------------

	typedef std::vector<iSubAction*> tSubActionVec;
	typedef tSubActionVec::iterator tSubActionVecIt;

	class cInput;

	//---------------------------------------------------------

	class cAction
	{
	public:
		cAction(const tString& asName,int alId, cInput *apInput);
		~cAction();

		void AddKey(eKey aKey);
		void AddMouseButton(eMouseButton aButton);
		void AddGamepadButton(int alPadIndex, eGamepadButton aButton);
		void AddGamepadAxis(int alPadIndex, eGamepadAxis aAxis, eGamepadAxisRange aRange, float afMinThreshold, float afMaxThreshold=1.0f); 
		void AddGamepadHat(int alPadIndex, eGamepadHat aHat, eGamepadHatState aHatState);

		void AddSubAction(iSubAction *apSubAction);

		void ClearSubActions();

		/**
		* returns true if the action is triggered now
		*/
		bool IsTriggerd();

		/**
		* returns true if the action just was triggered, else false
		*/
		bool WasTriggerd();

		/**
		* returns true if the action just became triggered, else false
		*/
		bool BecameTriggerd();

		/**
		* returns true if the action just was double triggered (double clicked), else false
		*/
		bool DoubleTriggerd(float afLimit);

		/**
		 * Resets the action to the current state, so no actions are triggered.
		 */
		void ResetToCurrentState();

		/**
		*Update the Action, called by cInput
		*/
		void Update(float afTimeStep);

		iSubAction *GetSubAction(int alIdx){ return mvSubActions[alIdx];}
		int GetSubActionNum(){ return (int)mvSubActions.size();}

		
		const tString& GetName(){ return msName;}
		int GetId(){ return mlId; }
	private:
		tString msName;
		int mlId;
		cInput *mpInput;

		bool mbBecameTriggerd;
		bool mbIsTriggerd;

		bool mbDoubleTrigger_Down;
		bool mbDoubleTrigger_Triggered;

		bool mbIsDown;

		double mfTimeCount;

		tSubActionVec mvSubActions;
		
	};

	//---------------------------------------------------------

};
#endif // HPL_ACTION_H
