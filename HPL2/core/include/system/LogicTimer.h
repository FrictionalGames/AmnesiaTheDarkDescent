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

#ifndef HPL_LOGICTIMER_H
#define HPL_LOGICTIMER_H


namespace hpl {

	class iLowLevelSystem;

	class cLogicTimer
	{
	public:
		/**
		 *
		 * \param alUpdatesPerSec Number of updates per second.
		 * \param *apLowLevelSystem 
		 * \return 
		 */
		cLogicTimer(int alUpdatesPerSec,iLowLevelSystem *apLowLevelSystem);
		~cLogicTimer();

		/**
		 * Reset the time. Do this when the logical update of the game has been ide, ie while loading.
		 */
		void Reset();
		/**
		 * Check with the time if it is time for another logical update
		 * \return true if logic should be updated, else false.
		 */
		bool WantUpdate();

		/**
		 * Resets various variables that makes the graphics is never frozen.
		 */
		void EndUpdateLoop();

		/**
		 * Set the number of times per second to update
		 * \param alUpdatesPerSec 
		 */
		void SetUpdatesPerSec(int alUpdatesPerSec);

		/**
		 * Sets the maximum updates in a row.
		 * \param alUpdatesPerSec 
		 */
		void SetMaxUpdates(int alMax);

		/**
		 * Get the number of updates per second.
		 */
		int GetUpdatesPerSec();
		/**
		 * Get the size of each step in seconds.
		 */
		float GetStepSize();

		double GetLocalTime(){ return mlLocalTime;}
		double GetLocalTimeAdd(){ return mlLocalTimeAdd;}

		void SetSpeedMul(float afX){ mfSpeedMul = afX;}
		
	private:
		void Update();

		double mlLocalTime;
		double mlLocalTimeAdd;

		double mfSpeedMul;

		int mlMaxUpdates;
		int mlUpdateCount;

		iLowLevelSystem *mpLowLevelSystem;
	};

};
#endif // HPL_LOGICTIMER_H
