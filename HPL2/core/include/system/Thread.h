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

#ifndef HPL_THREAD_H
#define HPL_THREAD_H


namespace hpl {

	enum eThreadPrio
	{
		eThreadPrio_Low,
		eThreadPrio_Normal,
		eThreadPrio_High,

		eThreadPrio_LastEnum,
	};

	class iThreadClass
	{
	public:
		virtual ~iThreadClass() {}
		virtual void UpdateThread()=0;
	};

	class iThread
	{
	public:
		iThread();
		virtual ~iThread(){}

		void SetThreadClass(iThreadClass* apThreadClass) { mpThreadClass = apThreadClass; }
		void SetUpdateRate(float afUpdateRate);
		void SetSleepTime(unsigned int alSleepTime) { mlSleepTime = alSleepTime; }

		bool IsActive() { return mbThreadActive; }

		unsigned long GetSleepTime() { return mlSleepTime; }
		
        virtual void Start()=0;
		virtual void Stop()=0;
		virtual void Sleep(unsigned int alSleepTime)=0;

		virtual void SetPriority(eThreadPrio aPrio)=0;
		eThreadPrio GetPriority() { return mPrio; }

	protected:
		static int MainThreadFunc(void* apThread);
		virtual int TranslateEnginePrio(eThreadPrio aPrio)=0;

		unsigned long mlSleepTime;
		bool mbThreadActive;

	private:
		iThreadClass* mpThreadClass;
		eThreadPrio mPrio;
	};
};
#endif // HPL_THREAD_H
