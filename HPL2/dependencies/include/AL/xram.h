#include <al.h>

// X-RAM Function pointer definitions
typedef ALboolean (__cdecl *EAXSetBufferMode)(ALsizei n, ALuint *buffers, ALint value);
typedef ALenum    (__cdecl *EAXGetBufferMode)(ALuint buffer, ALint *value);

//////////////////////////////////////////////////////////////////////////////
// Query for X-RAM extension
//
// if (alIsExtensionPresent("EAX-RAM") == AL_TRUE)
//		X-RAM Extension found
//  
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// X-RAM enum names
//
// "AL_EAX_RAM_SIZE"
// "AL_EAX_RAM_FREE"
// "AL_STORAGE_AUTOMATIC"
// "AL_STORAGE_HARDWARE"
// "AL_STORAGE_ACCESSIBLE"
//
// Query enum values using alGetEnumValue, for example
// 
// long lRamSizeEnum = alGetEnumValue("AL_EAX_RAM_SIZE")
//
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Query total amount of X-RAM
//
// long lTotalSize = alGetInteger(alGetEnumValue("AL_EAX_RAM_SIZE")
//
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Query free X-RAM available
//
// long lFreeSize = alGetInteger(alGetEnumValue("AL_EAX_RAM_FREE")
//
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Query X-RAM Function pointers
// 
// Use typedefs defined above to get the X-RAM function pointers using
// alGetProcAddress
//
// EAXSetBufferMode eaxSetBufferMode;
// EAXGetBufferMode eaxGetBufferMode;
//
// eaxSetBufferMode = (EAXSetBufferMode)alGetProcAddress("EAXSetBufferMode");
// eaxGetBufferMode = (EAXGetBufferMode)alGetProcAddress("EAXGetBufferMode");
//
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Force an Open AL Buffer into X-RAM (good for non-streaming buffers)
//
// ALuint uiBuffer;
// alGenBuffers(1, &uiBuffer);
// eaxSetBufferMode(1, &uiBuffer, alGetEnumValue("AL_STORAGE_HARDWARE"));
// alBufferData(...);
//
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Force an Open AL Buffer into 'accessible' (currently host) RAM (good for streaming buffers)
//
// ALuint uiBuffer;
// alGenBuffers(1, &uiBuffer);
// eaxSetBufferMode(1, &uiBuffer, alGetEnumValue("AL_STORAGE_ACCESSIBLE"));
// alBufferData(...);
//
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Put an Open AL Buffer into X-RAM if memory is available, otherwise use
// host RAM.  This is the default mode.
//
// ALuint uiBuffer;
// alGenBuffers(1, &uiBuffer);
// eaxSetBufferMode(1, &uiBuffer, alGetEnumValue("AL_STORAGE_AUTOMATIC"));
// alBufferData(...);
//
//////////////////////////////////////////////////////////////////////////////
