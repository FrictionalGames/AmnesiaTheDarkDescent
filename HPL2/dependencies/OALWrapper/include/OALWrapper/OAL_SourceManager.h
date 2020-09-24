/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
#ifndef _OAL_SOURCEMANAGER_H
#define _OAL_SOURCEMANAGER_H

#include "OAL_Types.h"
#include "OAL_Helper.h"
#include "OAL_LoggerObject.h"

class cOAL_SourceManager : public iOAL_LoggerObject
{
public:

	cOAL_SourceManager();
	~cOAL_SourceManager();

	cOAL_Source* GetSource ( int alSourceHandle, bool abSkipRefCountCheck = false );
	cOAL_Source* GetAvailableSource ( unsigned int alPriority, int alNumOfVoices );

	bool Initialize( bool abManageVoices, int alNumSourcesHint, bool abUseThreading, int alUpdateFreq, int alEFXSends = 0 );
	void Destroy ( );

	void AddActiveStream ( cOAL_Source *apSource );
	void UpdateStreaming ( );

	inline void ReserveVoices ( int alNum ) { if (!mbManageVoices) alNum = 1; mlAvailableVoices -= alNum; }
	inline void ReleaseVoices ( int alNum ) { if (!mbManageVoices) alNum = 1; mlAvailableVoices += alNum; }

	inline bool IsThreadAlive ( ) { return mbUseThreading; }
	inline int	GetThreadWaitTime ( ) { return mlThreadWaitTime; }

	inline int GetNumVoices()	{ return mlNumOfVoices; }

private:

	void LockStreamList();
	void UnlockStreamList();

	int GetUnpackedSourceId(int alHandle);
	int GetUnpackedRefCount(int alHandle);
	
	bool mbManageVoices; 

	int mlNumOfVoices;
	int mlAvailableVoices;

    SDL_mutex*			mpStreamListMutex;
	SDL_Thread*			mpUpdaterThread;
	int					mlThreadWaitTime;
	bool				mbUseThreading;

	tSourceVec mvSources;
	tSourceList mlstStreamingSources;

};


#endif 


