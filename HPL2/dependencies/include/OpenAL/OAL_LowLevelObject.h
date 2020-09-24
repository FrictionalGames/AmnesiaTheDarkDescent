/**
	@file OAL_Init.h
	@author Luis Rodero
	@date 2007-04-10
	@version 0.1
	Interface for classes handling any OpenAL Object
*/

#ifndef _OAL_LOWLEVELOBJECT_H
#define _OAL_LOWLEVELOBJECT_H

#include "OAL_LoggerObject.h"

class iOAL_LowLevelObject : public iOAL_LoggerObject
{
public:

	iOAL_LowLevelObject(const string& asObjType) {}
	~iOAL_LowLevelObject() {}

	////////////////////////////////////////////
	// Creation / destruction of OpenAL Objects

	virtual bool CreateLowLevelID(void) = 0;
	virtual bool DestroyLowLevelID(void) = 0;

	////////////////////////////////////////////
	// Methods for helping in the reinitialization process (not implemented)
	virtual void SaveObjectState(void) = 0;
	virtual void RestoreObjectState(void) = 0;

	////////////////////////////////////////////
	// Method for checking if object is valid
	virtual bool IsValidObject() = 0;

	////////////////////////////////////////////
	// Method for returning the low level ID
	ALuint GetObjectID() { return mlObjectId; }
	ALuint* GetObjectIDPointer() { return &mlObjectId; }

	bool GetStatus() { return mbStatus; }

protected:
	ALuint mlObjectId;
	bool mbStatus;
};


#endif


