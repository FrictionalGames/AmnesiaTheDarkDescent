/**
	@file OAL_OggSample.h
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Derived class for containing Ogg Vorbis Sample data 
*/

#ifndef _OAL_OGGSAMPLE_H
#define _OAL_OGGSAMPLE_H

#include "OAL_Sample.h"

#include "OAL_OggHelper.h"
//-------------------------------------------------------------------------------

class cOAL_OggSample : public cOAL_Sample
{
public:
	bool CreateFromFile(const wstring& asFilename);	
protected:
};

//-------------------------------------------------------------------------------

#endif	// _OAL_SAMPLE_H
