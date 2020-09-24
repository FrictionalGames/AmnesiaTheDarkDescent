/**
	@file OAL_OggSample.h
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Derived class for containing WAV Sample data 
*/

#ifndef _OAL_WAVSAMPLE_H
#define _OAL_WAVSAMPLE_H

#include "OpenAL/OAL_Sample.h"
#include "OpenAL/OAL_Helper.h"

//-----------------------------------------------------------

class cOAL_WAVSample : public cOAL_Sample
{
public:	
	bool CreateFromFile(const wstring& asFilename);
protected:
};

//-----------------------------------------------------------

#endif	// _OAL_SAMPLE_H
