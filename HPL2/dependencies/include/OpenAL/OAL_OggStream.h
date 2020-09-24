/**
	@file OAL_OggStream.h
	@author Luis Rodero
	@date 2006-10-06
	@version 0.1
	Declaration of class for containing Ogg Vorbis streams
*/

#ifndef _OAL_OGGSTREAM_H
#define _OAL_OGGSTREAM_H

#include "OAL_Stream.h"
#include "OAL_Helper.h"

#include "OAL_OggHelper.h"

#include "system/MemoryManager.h"
#include "system/LowLevelSystem.h"

//------------------------------------------------------------------------

class cOAL_OggStream : public cOAL_Stream
{
public:
	cOAL_OggStream();
	~cOAL_OggStream();
	
	void Seek(float afWhere, bool abForceRebuffer);
	
protected:
	bool CreateFromFile(const wstring &asFileName);
	void Destroy();

	bool Stream(cOAL_Buffer* apDestBuffer);
	double GetTime();

	OggVorbis_File movStreamHandle;
	int mlCurrent_section;
	bool mbIsValidHandle;
};

//------------------------------------------------------------------------

#endif	// _OAL_SAMPLE_H
