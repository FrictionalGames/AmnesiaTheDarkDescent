/* =========================================================================================== */
/* FMOD Dynamic DLL loading header. Copyright (c), Firelight Technologies Pty, Ltd. 1999-2004. */
/* =========================================================================================== */

#ifndef _FMODDYN_H_
#define _FMODDYN_H_

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
  #include <windows.h>
#else
  #include <dlfcn.h>
  #include <string.h>
#endif
#include <stdlib.h>

#include "fmod.h"

typedef struct
{
    void *module;

    signed char       (F_API *FSOUND_SetOutput)(int outputtype);
    signed char       (F_API *FSOUND_SetDriver)(int driver);
    signed char       (F_API *FSOUND_SetMixer)(int mixer);
    signed char       (F_API *FSOUND_SetBufferSize)(int len_ms);
    signed char       (F_API *FSOUND_SetHWND)(void *hwnd);
    signed char       (F_API *FSOUND_SetMinHardwareChannels)(int min);
    signed char       (F_API *FSOUND_SetMaxHardwareChannels)(int max);
    signed char       (F_API *FSOUND_SetMemorySystem)(void *pool, int poollen, FSOUND_ALLOCCALLBACK useralloc, FSOUND_REALLOCCALLBACK userrealloc, FSOUND_FREECALLBACK userfree);
    signed char       (F_API *FSOUND_Init)(int mixrate, int maxsoftwarechannels, unsigned int flags);
    void              (F_API *FSOUND_Close)();
    void              (F_API *FSOUND_Update)();   /* you must call this once a frame */
    void              (F_API *FSOUND_SetSpeakerMode)(unsigned int speakermode);
    void              (F_API *FSOUND_SetSFXMasterVolume)(int volume);
    void              (F_API *FSOUND_SetPanSeperation)(float pansep);
    void              (F_API *FSOUND_File_SetCallbacks)(FSOUND_OPENCALLBACK  useropen, FSOUND_CLOSECALLBACK userclose, FSOUND_READCALLBACK userread, FSOUND_SEEKCALLBACK  userseek, FSOUND_TELLCALLBACK  usertell);
    int               (F_API *FSOUND_GetError)();
    float             (F_API *FSOUND_GetVersion)();
    int               (F_API *FSOUND_GetOutput)();
    void *            (F_API *FSOUND_GetOutputHandle)();
    int               (F_API *FSOUND_GetDriver)();
    int               (F_API *FSOUND_GetMixer)();
    int               (F_API *FSOUND_GetNumDrivers)();
    const char *      (F_API *FSOUND_GetDriverName)(int id);
    signed char       (F_API *FSOUND_GetDriverCaps)(int id, unsigned int *caps);
    int               (F_API *FSOUND_GetOutputRate)();
    int               (F_API *FSOUND_GetMaxChannels)();
    int               (F_API *FSOUND_GetMaxSamples)();
    unsigned int      (F_API *FSOUND_GetSpeakerMode)();
    int               (F_API *FSOUND_GetSFXMasterVolume)();
    signed char       (F_API *FSOUND_GetNumHWChannels)(int *num2d, int *num3d, int *total);
    int               (F_API *FSOUND_GetChannelsPlaying)();
    float             (F_API *FSOUND_GetCPUUsage)();
    void              (F_API *FSOUND_GetMemoryStats)(unsigned int *currentalloced, unsigned int *maxalloced);
    FSOUND_SAMPLE *   (F_API *FSOUND_Sample_Load)(int index, const char *name_or_data, unsigned int mode, int offset, int length);
    FSOUND_SAMPLE *   (F_API *FSOUND_Sample_Alloc)(int index, int length, unsigned int mode, int deffreq, int defvol, int defpan, int defpri);
    void              (F_API *FSOUND_Sample_Free)(FSOUND_SAMPLE *sptr);
    signed char       (F_API *FSOUND_Sample_Upload)(FSOUND_SAMPLE *sptr, void *srcdata, unsigned int mode);
    signed char       (F_API *FSOUND_Sample_Lock)(FSOUND_SAMPLE *sptr, int offset, int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2);
    signed char       (F_API *FSOUND_Sample_Unlock)(FSOUND_SAMPLE *sptr, void *ptr1, void *ptr2, unsigned int len1, unsigned int len2);
    signed char       (F_API *FSOUND_Sample_SetMode)(FSOUND_SAMPLE *sptr, unsigned int mode);
    signed char       (F_API *FSOUND_Sample_SetLoopPoints)(FSOUND_SAMPLE *sptr, int loopstart, int loopend);
    signed char       (F_API *FSOUND_Sample_SetDefaults)(FSOUND_SAMPLE *sptr, int deffreq, int defvol, int defpan, int defpri);
    signed char       (F_API *FSOUND_Sample_SetDefaultsEx)(FSOUND_SAMPLE *sptr, int deffreq, int defvol, int defpan, int defpri, int varfreq, int varvol, int varpan);
    signed char       (F_API *FSOUND_Sample_SetMinMaxDistance)(FSOUND_SAMPLE *sptr, float min, float max);
    signed char       (F_API *FSOUND_Sample_SetMaxPlaybacks)(FSOUND_SAMPLE *sptr, int max);
    FSOUND_SAMPLE *   (F_API *FSOUND_Sample_Get)(int sampno);
    const char *      (F_API *FSOUND_Sample_GetName)(FSOUND_SAMPLE *sptr);
    unsigned int      (F_API *FSOUND_Sample_GetLength)(FSOUND_SAMPLE *sptr);
    signed char       (F_API *FSOUND_Sample_GetLoopPoints)(FSOUND_SAMPLE *sptr, int *loopstart, int *loopend);
    signed char       (F_API *FSOUND_Sample_GetDefaults)(FSOUND_SAMPLE *sptr, int *deffreq, int *defvol, int *defpan, int *defpri);
    signed char       (F_API *FSOUND_Sample_GetDefaultsEx)(FSOUND_SAMPLE *sptr, int *deffreq, int *defvol, int *defpan, int *defpri, int *varfreq, int *varvol, int *varpan);
    unsigned int      (F_API *FSOUND_Sample_GetMode)(FSOUND_SAMPLE *sptr);
    signed char       (F_API *FSOUND_Sample_GetMinMaxDistance)(FSOUND_SAMPLE *sptr, float *min, float *max);
    int               (F_API *FSOUND_PlaySound)(int channel, FSOUND_SAMPLE *sptr);
    int               (F_API *FSOUND_PlaySoundEx)(int channel, FSOUND_SAMPLE *sptr, FSOUND_DSPUNIT *dsp, signed char startpaused);
    signed char       (F_API *FSOUND_StopSound)(int channel);
    signed char       (F_API *FSOUND_SetFrequency)(int channel, int freq);
    signed char       (F_API *FSOUND_SetVolume)(int channel, int vol);
    signed char       (F_API *FSOUND_SetVolumeAbsolute)(int channel, int vol);
    signed char       (F_API *FSOUND_SetPan)(int channel, int pan);
    signed char       (F_API *FSOUND_SetSurround)(int channel, signed char surround);
    signed char       (F_API *FSOUND_SetMute)(int channel, signed char mute);
    signed char       (F_API *FSOUND_SetPriority)(int channel, int priority);
    signed char       (F_API *FSOUND_SetReserved)(int channel, signed char reserved);
    signed char       (F_API *FSOUND_SetPaused)(int channel, signed char paused);
    signed char       (F_API *FSOUND_SetLoopMode)(int channel, unsigned int loopmode);
    signed char       (F_API *FSOUND_SetCurrentPosition)(int channel, unsigned int offset);
    signed char       (F_API *FSOUND_3D_SetAttributes)(int channel, const float *pos, const float *vel);
    signed char       (F_API *FSOUND_3D_SetMinMaxDistance)(int channel, float min, float max);
    signed char       (F_API *FSOUND_IsPlaying)(int channel);
    int               (F_API *FSOUND_GetFrequency)(int channel);
    int               (F_API *FSOUND_GetVolume)(int channel);
    int               (F_API *FSOUND_GetAmplitude)(int channel);
    int               (F_API *FSOUND_GetPan)(int channel);
    signed char       (F_API *FSOUND_GetSurround)(int channel);
    signed char       (F_API *FSOUND_GetMute)(int channel);
    int               (F_API *FSOUND_GetPriority)(int channel);
    signed char       (F_API *FSOUND_GetReserved)(int channel);
    signed char       (F_API *FSOUND_GetPaused)(int channel);
    unsigned int      (F_API *FSOUND_GetLoopMode)(int channel);
    unsigned int      (F_API *FSOUND_GetCurrentPosition)(int channel);
    FSOUND_SAMPLE *   (F_API *FSOUND_GetCurrentSample)(int channel);
    signed char       (F_API *FSOUND_GetCurrentLevels)(int channel, float *l, float *r);
    int               (F_API *FSOUND_GetNumSubChannels)(int channel);
    int               (F_API *FSOUND_GetSubChannel)(int channel, int subchannel);
    signed char       (F_API *FSOUND_3D_GetAttributes)(int channel, float *pos, float *vel);
    signed char       (F_API *FSOUND_3D_GetMinMaxDistance)(int channel, float *min, float *max);
    void              (F_API *FSOUND_3D_SetDopplerFactor)(float scale);
    void              (F_API *FSOUND_3D_SetDistanceFactor)(float scale);
    void              (F_API *FSOUND_3D_SetRolloffFactor)(float scale);
    void              (F_API *FSOUND_3D_Listener_SetCurrent)(int current, int numlisteners);  /* use this if you use multiple listeners / splitscreen */
    void              (F_API *FSOUND_3D_Listener_SetAttributes)(const float *pos, const float *vel, float fx, float fy, float fz, float tx, float ty, float tz);
    void              (F_API *FSOUND_3D_Listener_GetAttributes)(float *pos, float *vel, float *fx, float *fy, float *fz, float *tx, float *ty, float *tz);
    int               (F_API *FSOUND_FX_Enable)(int channel, unsigned int fx);    /* See FSOUND_FX_MODES */
    signed char       (F_API *FSOUND_FX_Disable)(int channel);
    signed char       (F_API *FSOUND_FX_SetChorus)(int fxid, float WetDryMix, float Depth, float Feedback, float Frequency, int Waveform, float Delay, int Phase);
    signed char       (F_API *FSOUND_FX_SetCompressor)(int fxid, float Gain, float Attack, float Release, float Threshold, float Ratio, float Predelay);
    signed char       (F_API *FSOUND_FX_SetDistortion)(int fxid, float Gain, float Edge, float PostEQCenterFrequency, float PostEQBandwidth, float PreLowpassCutoff);
    signed char       (F_API *FSOUND_FX_SetEcho)(int fxid, float WetDryMix, float Feedback, float LeftDelay, float RightDelay, int PanDelay);
    signed char       (F_API *FSOUND_FX_SetFlanger)(int fxid, float WetDryMix, float Depth, float Feedback, float Frequency, int Waveform, float Delay, int Phase);
    signed char       (F_API *FSOUND_FX_SetGargle)(int fxid, int RateHz, int WaveShape);
    signed char       (F_API *FSOUND_FX_SetI3DL2Reverb)(int fxid, int Room, int RoomHF, float RoomRolloffFactor, float DecayTime, float DecayHFRatio, int Reflections, float ReflectionsDelay, int Reverb, float ReverbDelay, float Diffusion, float Density, float HFReference);
    signed char       (F_API *FSOUND_FX_SetParamEQ)(int fxid, float Center, float Bandwidth, float Gain);
    signed char       (F_API *FSOUND_FX_SetWavesReverb)(int fxid, float InGain, float ReverbMix, float ReverbTime, float HighFreqRTRatio);  
    signed char       (F_API *FSOUND_Stream_SetBufferSize)(int ms);      /* call this before opening streams, not after */
    FSOUND_STREAM *   (F_API *FSOUND_Stream_Open)(const char *name_or_data, unsigned int mode, int offset, int length);
    FSOUND_STREAM *   (F_API *FSOUND_Stream_Create)(FSOUND_STREAMCALLBACK callback, int length, unsigned int mode, int samplerate, void *userdata);
    signed char       (F_API *FSOUND_Stream_Close)(FSOUND_STREAM *stream);
    int               (F_API *FSOUND_Stream_Play)(int channel, FSOUND_STREAM *stream);
    int               (F_API *FSOUND_Stream_PlayEx)(int channel, FSOUND_STREAM *stream, FSOUND_DSPUNIT *dsp, signed char startpaused);
    signed char       (F_API *FSOUND_Stream_Stop)(FSOUND_STREAM *stream);
    signed char       (F_API *FSOUND_Stream_SetPosition)(FSOUND_STREAM *stream, unsigned int position);
    unsigned int      (F_API *FSOUND_Stream_GetPosition)(FSOUND_STREAM *stream);
    signed char       (F_API *FSOUND_Stream_SetTime)(FSOUND_STREAM *stream, int ms);
    int               (F_API *FSOUND_Stream_GetTime)(FSOUND_STREAM *stream);
    int               (F_API *FSOUND_Stream_GetLength)(FSOUND_STREAM *stream);
    int               (F_API *FSOUND_Stream_GetLengthMs)(FSOUND_STREAM *stream);
    signed char       (F_API *FSOUND_Stream_SetMode)(FSOUND_STREAM *stream, unsigned int mode);
    unsigned int      (F_API *FSOUND_Stream_GetMode)(FSOUND_STREAM *stream);
    signed char       (F_API *FSOUND_Stream_SetLoopPoints)(FSOUND_STREAM *stream, unsigned int loopstartpcm, unsigned int loopendpcm);
    signed char       (F_API *FSOUND_Stream_SetLoopCount)(FSOUND_STREAM *stream, int count);
    int               (F_API *FSOUND_Stream_GetOpenState)(FSOUND_STREAM *stream);
    FSOUND_SAMPLE *   (F_API *FSOUND_Stream_GetSample)(FSOUND_STREAM *stream);   /* every stream contains a sample to playback on */
    FSOUND_DSPUNIT *  (F_API *FSOUND_Stream_CreateDSP)(FSOUND_STREAM *stream, FSOUND_DSPCALLBACK callback, int priority, void *userdata);
    signed char       (F_API *FSOUND_Stream_SetEndCallback)(FSOUND_STREAM *stream, FSOUND_STREAMCALLBACK callback, void *userdata);
    signed char       (F_API *FSOUND_Stream_SetSyncCallback)(FSOUND_STREAM *stream, FSOUND_STREAMCALLBACK callback, void *userdata);
    FSOUND_SYNCPOINT *(F_API *FSOUND_Stream_AddSyncPoint)(FSOUND_STREAM *stream, unsigned int pcmoffset, const char *name);
    signed char       (F_API *FSOUND_Stream_DeleteSyncPoint)(FSOUND_SYNCPOINT *point);
    int               (F_API *FSOUND_Stream_GetNumSyncPoints)(FSOUND_STREAM *stream);
    FSOUND_SYNCPOINT *(F_API *FSOUND_Stream_GetSyncPoint)(FSOUND_STREAM *stream, int index);
    char *            (F_API *FSOUND_Stream_GetSyncPointInfo)(FSOUND_SYNCPOINT *point, unsigned int *pcmoffset);
    signed char       (F_API *FSOUND_Stream_SetSubStream)(FSOUND_STREAM *stream, int index);
    int               (F_API *FSOUND_Stream_GetNumSubStreams)(FSOUND_STREAM *stream);
    signed char       (F_API *FSOUND_Stream_SetSubStreamSentence)(FSOUND_STREAM *stream, const int *sentencelist, int numitems);
    signed char       (F_API *FSOUND_Stream_GetNumTagFields)(FSOUND_STREAM *stream, int *num);
    signed char       (F_API *FSOUND_Stream_GetTagField)(FSOUND_STREAM *stream, int num, int *type, char **name, void **value, int *length);
    signed char       (F_API *FSOUND_Stream_FindTagField)(FSOUND_STREAM *stream, int type, const char *name, void **value, int *length);
    signed char       (F_API *FSOUND_Stream_Net_SetProxy)(const char *proxy);
    signed char       (F_API *FSOUND_Stream_Net_SetTimeout)(int timeout);
    char *            (F_API *FSOUND_Stream_Net_GetLastServerStatus)();
    signed char       (F_API *FSOUND_Stream_Net_SetBufferProperties)(int buffersize, int prebuffer_percent, int rebuffer_percent);
    signed char       (F_API *FSOUND_Stream_Net_GetBufferProperties)(int *buffersize, int *prebuffer_percent, int *rebuffer_percent);
    signed char       (F_API *FSOUND_Stream_Net_SetMetadataCallback)(FSOUND_STREAM *stream, FSOUND_METADATACALLBACK callback, void *userdata);
    signed char       (F_API *FSOUND_Stream_Net_GetStatus)(FSOUND_STREAM *stream, int *status, int *bufferpercentused, int *bitrate, unsigned int *flags);
    signed char       (F_API *FSOUND_CD_Play)(char drive, int track);
    void              (F_API *FSOUND_CD_SetPlayMode)(char drive, signed char mode);
    signed char       (F_API *FSOUND_CD_Stop)(char drive);
    signed char       (F_API *FSOUND_CD_SetPaused)(char drive, signed char paused);
    signed char       (F_API *FSOUND_CD_SetVolume)(char drive, int volume);
    signed char       (F_API *FSOUND_CD_SetTrackTime)(char drive, unsigned int ms);
    signed char       (F_API *FSOUND_CD_OpenTray)(char drive, signed char open);
    signed char       (F_API *FSOUND_CD_GetPaused)(char drive);
    int               (F_API *FSOUND_CD_GetTrack)(char drive);
    int               (F_API *FSOUND_CD_GetNumTracks)(char drive);
    int               (F_API *FSOUND_CD_GetVolume)(char drive);
    int               (F_API *FSOUND_CD_GetTrackLength)(char drive, int track); 
    int               (F_API *FSOUND_CD_GetTrackTime)(char drive);
    FSOUND_DSPUNIT *  (F_API *FSOUND_DSP_Create)(FSOUND_DSPCALLBACK callback, int priority, void *userdata);
    void              (F_API *FSOUND_DSP_Free)(FSOUND_DSPUNIT *unit);
    void              (F_API *FSOUND_DSP_SetPriority)(FSOUND_DSPUNIT *unit, int priority);
    int               (F_API *FSOUND_DSP_GetPriority)(FSOUND_DSPUNIT *unit);
    void              (F_API *FSOUND_DSP_SetActive)(FSOUND_DSPUNIT *unit, signed char active);
    signed char       (F_API *FSOUND_DSP_GetActive)(FSOUND_DSPUNIT *unit);
    FSOUND_DSPUNIT *  (F_API *FSOUND_DSP_GetClearUnit)();
    FSOUND_DSPUNIT *  (F_API *FSOUND_DSP_GetSFXUnit)();
    FSOUND_DSPUNIT *  (F_API *FSOUND_DSP_GetMusicUnit)();
    FSOUND_DSPUNIT *  (F_API *FSOUND_DSP_GetFFTUnit)();
    FSOUND_DSPUNIT *  (F_API *FSOUND_DSP_GetClipAndCopyUnit)();
    signed char       (F_API *FSOUND_DSP_MixBuffers)(void *destbuffer, void *srcbuffer, int len, int freq, int vol, int pan, unsigned int mode);
    void              (F_API *FSOUND_DSP_ClearMixBuffer)();
    int               (F_API *FSOUND_DSP_GetBufferLength)();      /* Length of each DSP update */
    int               (F_API *FSOUND_DSP_GetBufferLengthTotal)(); /* Total buffer length due to FSOUND_SetBufferSize */
    float *           (F_API *FSOUND_DSP_GetSpectrum)();          /* Array of 512 floats - call FSOUND_DSP_SetActive(FSOUND_DSP_GetFFTUnit(), TRUE)) for this to work. */
    signed char       (F_API *FSOUND_Reverb_SetProperties)(const FSOUND_REVERB_PROPERTIES *prop);
    signed char       (F_API *FSOUND_Reverb_GetProperties)(FSOUND_REVERB_PROPERTIES *prop);
    signed char       (F_API *FSOUND_Reverb_SetChannelProperties)(int channel, const FSOUND_REVERB_CHANNELPROPERTIES *prop);
    signed char       (F_API *FSOUND_Reverb_GetChannelProperties)(int channel, FSOUND_REVERB_CHANNELPROPERTIES *prop);
    signed char       (F_API *FSOUND_Record_SetDriver)(int outputtype);
    int               (F_API *FSOUND_Record_GetNumDrivers)();
    const char *      (F_API *FSOUND_Record_GetDriverName)(int id);
    int               (F_API *FSOUND_Record_GetDriver)();
    signed char       (F_API *FSOUND_Record_StartSample)(FSOUND_SAMPLE *sptr, signed char loop);
    signed char       (F_API *FSOUND_Record_Stop)();
    int               (F_API *FSOUND_Record_GetPosition)();  
    FMUSIC_MODULE *   (F_API *FMUSIC_LoadSong)(const char *name);
    FMUSIC_MODULE *   (F_API *FMUSIC_LoadSongEx)(const char *name_or_data, int offset, int length, unsigned int mode, const int *samplelist, int samplelistnum);
    int               (F_API *FMUSIC_GetOpenState)(FMUSIC_MODULE *mod);
    signed char       (F_API *FMUSIC_FreeSong)(FMUSIC_MODULE *mod);
    signed char       (F_API *FMUSIC_PlaySong)(FMUSIC_MODULE *mod);
    signed char       (F_API *FMUSIC_StopSong)(FMUSIC_MODULE *mod);
    void              (F_API *FMUSIC_StopAllSongs)();
    signed char       (F_API *FMUSIC_SetZxxCallback)(FMUSIC_MODULE *mod, FMUSIC_CALLBACK callback);
    signed char       (F_API *FMUSIC_SetRowCallback)(FMUSIC_MODULE *mod, FMUSIC_CALLBACK callback, int rowstep);
    signed char       (F_API *FMUSIC_SetOrderCallback)(FMUSIC_MODULE *mod, FMUSIC_CALLBACK callback, int orderstep);
    signed char       (F_API *FMUSIC_SetInstCallback)(FMUSIC_MODULE *mod, FMUSIC_CALLBACK callback, int instrument);
    signed char       (F_API *FMUSIC_SetSample)(FMUSIC_MODULE *mod, int sampno, FSOUND_SAMPLE *sptr);
    signed char       (F_API *FMUSIC_SetUserData)(FMUSIC_MODULE *mod, void *userdata);
    signed char       (F_API *FMUSIC_OptimizeChannels)(FMUSIC_MODULE *mod, int maxchannels, int minvolume);
    signed char       (F_API *FMUSIC_SetReverb)(signed char reverb);             /* MIDI only */
    signed char       (F_API *FMUSIC_SetLooping)(FMUSIC_MODULE *mod, signed char looping);
    signed char       (F_API *FMUSIC_SetOrder)(FMUSIC_MODULE *mod, int order);
    signed char       (F_API *FMUSIC_SetPaused)(FMUSIC_MODULE *mod, signed char pause);
    signed char       (F_API *FMUSIC_SetMasterVolume)(FMUSIC_MODULE *mod, int volume);
    signed char       (F_API *FMUSIC_SetMasterSpeed)(FMUSIC_MODULE *mode, float speed);
    signed char       (F_API *FMUSIC_SetPanSeperation)(FMUSIC_MODULE *mod, float pansep);
    const char *      (F_API *FMUSIC_GetName)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetType)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetNumOrders)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetNumPatterns)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetNumInstruments)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetNumSamples)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetNumChannels)(FMUSIC_MODULE *mod);
    FSOUND_SAMPLE *   (F_API *FMUSIC_GetSample)(FMUSIC_MODULE *mod, int sampno);
    int               (F_API *FMUSIC_GetPatternLength)(FMUSIC_MODULE *mod, int orderno);
    signed char       (F_API *FMUSIC_IsFinished)(FMUSIC_MODULE *mod);
    signed char       (F_API *FMUSIC_IsPlaying)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetMasterVolume)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetGlobalVolume)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetOrder)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetPattern)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetSpeed)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetBPM)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetRow)(FMUSIC_MODULE *mod);
    signed char       (F_API *FMUSIC_GetPaused)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetTime)(FMUSIC_MODULE *mod);
    int               (F_API *FMUSIC_GetRealChannel)(FMUSIC_MODULE *mod, int modchannel);
    unsigned int      (F_API *FMUSIC_GetUserData)(FMUSIC_MODULE *mod);
} FMOD_INSTANCE;


static FMOD_INSTANCE *FMOD_CreateInstance(char *dllName)
{
    FMOD_INSTANCE *instance;

    instance = (FMOD_INSTANCE *)calloc(sizeof(FMOD_INSTANCE), 1);
    if (!instance)
    {
        return NULL;
    }

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
    instance->module = LoadLibrary(dllName);
#else
    instance->module = dlopen(dllName, RTLD_LAZY);
#endif
    if (!instance->module)
    {
        free(instance);
        return NULL;
    }

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
    #define F_GETPROC(_x, _y)                                                                       \
    {                                                                                             \
        *((unsigned int *)&instance->_x) = (unsigned int)GetProcAddress((HMODULE)instance->module, _y);    \
        if (!instance->_x)                                                                        \
        {                                                                                         \
            FreeLibrary((HMODULE)instance->module);                                                        \
            free(instance);                                                                       \
            return NULL;                                                                          \
        }                                                                                         \
    }
#else
    #define F_GETPROC(_x, _y)                                                                       \
    {                                                                                             \
        char tmp[] = _y;                                                                          \
        *(strchr(tmp, '@')) = 0;                                                                  \
        *((unsigned int *)&instance->_x) = (unsigned int)dlsym(instance->module, &tmp[1]);        \
        if (!instance->_x)                                                                        \
        {                                                                                         \
            dlclose(instance->module);                                                            \
            free(instance);                                                                       \
            return NULL;                                                                          \
        }                                                                                         \
    }
#endif

    F_GETPROC(FSOUND_SetOutput, "_FSOUND_SetOutput@4");
    F_GETPROC(FSOUND_SetDriver, "_FSOUND_SetDriver@4");
    F_GETPROC(FSOUND_SetMixer, "_FSOUND_SetMixer@4");
    F_GETPROC(FSOUND_SetBufferSize, "_FSOUND_SetBufferSize@4");
    F_GETPROC(FSOUND_SetHWND, "_FSOUND_SetHWND@4");
    F_GETPROC(FSOUND_SetMinHardwareChannels, "_FSOUND_SetMinHardwareChannels@4");
    F_GETPROC(FSOUND_SetMaxHardwareChannels, "_FSOUND_SetMaxHardwareChannels@4");
    F_GETPROC(FSOUND_SetMemorySystem, "_FSOUND_SetMemorySystem@20");
    F_GETPROC(FSOUND_Init, "_FSOUND_Init@12");
    F_GETPROC(FSOUND_Close, "_FSOUND_Close@0");
    F_GETPROC(FSOUND_Update, "_FSOUND_Update@0");
    F_GETPROC(FSOUND_SetSFXMasterVolume, "_FSOUND_SetSFXMasterVolume@4");
    F_GETPROC(FSOUND_SetPanSeperation, "_FSOUND_SetPanSeperation@4");
    F_GETPROC(FSOUND_SetSpeakerMode, "_FSOUND_SetSpeakerMode@4");
    F_GETPROC(FSOUND_GetError, "_FSOUND_GetError@0");
    F_GETPROC(FSOUND_GetVersion, "_FSOUND_GetVersion@0");
    F_GETPROC(FSOUND_GetOutput, "_FSOUND_GetOutput@0");
    F_GETPROC(FSOUND_GetOutputHandle, "_FSOUND_GetOutputHandle@0");
    F_GETPROC(FSOUND_GetDriver, "_FSOUND_GetDriver@0");
    F_GETPROC(FSOUND_GetMixer, "_FSOUND_GetMixer@0");
    F_GETPROC(FSOUND_GetNumDrivers, "_FSOUND_GetNumDrivers@0");
    F_GETPROC(FSOUND_GetDriverName, "_FSOUND_GetDriverName@4");
    F_GETPROC(FSOUND_GetDriverCaps, "_FSOUND_GetDriverCaps@8");
    F_GETPROC(FSOUND_GetOutputRate, "_FSOUND_GetOutputRate@0");
    F_GETPROC(FSOUND_GetMaxChannels, "_FSOUND_GetMaxChannels@0");
    F_GETPROC(FSOUND_GetMaxSamples, "_FSOUND_GetMaxSamples@0");
    F_GETPROC(FSOUND_GetSpeakerMode, "_FSOUND_GetSpeakerMode@0");
    F_GETPROC(FSOUND_GetSFXMasterVolume, "_FSOUND_GetSFXMasterVolume@0");
    F_GETPROC(FSOUND_GetNumHWChannels, "_FSOUND_GetNumHWChannels@12");
    F_GETPROC(FSOUND_GetChannelsPlaying, "_FSOUND_GetChannelsPlaying@0");
    F_GETPROC(FSOUND_GetCPUUsage, "_FSOUND_GetCPUUsage@0");
    F_GETPROC(FSOUND_GetMemoryStats, "_FSOUND_GetMemoryStats@8");
    F_GETPROC(FSOUND_Sample_Load, "_FSOUND_Sample_Load@20");
    F_GETPROC(FSOUND_Sample_Alloc, "_FSOUND_Sample_Alloc@28");
    F_GETPROC(FSOUND_Sample_Free, "_FSOUND_Sample_Free@4");
    F_GETPROC(FSOUND_Sample_Upload, "_FSOUND_Sample_Upload@12");
    F_GETPROC(FSOUND_Sample_Lock, "_FSOUND_Sample_Lock@28");
    F_GETPROC(FSOUND_Sample_Unlock, "_FSOUND_Sample_Unlock@20");
    F_GETPROC(FSOUND_Sample_SetMode, "_FSOUND_Sample_SetMode@8");
    F_GETPROC(FSOUND_Sample_SetLoopPoints, "_FSOUND_Sample_SetLoopPoints@12");
    F_GETPROC(FSOUND_Sample_SetDefaults, "_FSOUND_Sample_SetDefaults@20");
    F_GETPROC(FSOUND_Sample_SetDefaultsEx, "_FSOUND_Sample_SetDefaultsEx@32");
    F_GETPROC(FSOUND_Sample_SetMinMaxDistance, "_FSOUND_Sample_SetMinMaxDistance@12");
    F_GETPROC(FSOUND_Sample_SetMaxPlaybacks, "_FSOUND_Sample_SetMaxPlaybacks@8");
    F_GETPROC(FSOUND_Sample_Get, "_FSOUND_Sample_Get@4");
    F_GETPROC(FSOUND_Sample_GetName, "_FSOUND_Sample_GetName@4");
    F_GETPROC(FSOUND_Sample_GetLength, "_FSOUND_Sample_GetLength@4");
    F_GETPROC(FSOUND_Sample_GetLoopPoints, "_FSOUND_Sample_GetLoopPoints@12");
    F_GETPROC(FSOUND_Sample_GetDefaults, "_FSOUND_Sample_GetDefaults@20");
    F_GETPROC(FSOUND_Sample_GetDefaultsEx, "_FSOUND_Sample_GetDefaultsEx@32");
    F_GETPROC(FSOUND_Sample_GetMode, "_FSOUND_Sample_GetMode@4");
    F_GETPROC(FSOUND_Sample_GetMinMaxDistance, "_FSOUND_Sample_GetMinMaxDistance@12");
    F_GETPROC(FSOUND_PlaySound, "_FSOUND_PlaySound@8");
    F_GETPROC(FSOUND_PlaySoundEx, "_FSOUND_PlaySoundEx@16");
    F_GETPROC(FSOUND_StopSound, "_FSOUND_StopSound@4");
    F_GETPROC(FSOUND_SetFrequency, "_FSOUND_SetFrequency@8");
    F_GETPROC(FSOUND_SetVolume, "_FSOUND_SetVolume@8");
    F_GETPROC(FSOUND_SetVolumeAbsolute, "_FSOUND_SetVolumeAbsolute@8");
    F_GETPROC(FSOUND_SetPan, "_FSOUND_SetPan@8");
    F_GETPROC(FSOUND_SetSurround, "_FSOUND_SetSurround@8");
    F_GETPROC(FSOUND_SetMute, "_FSOUND_SetMute@8");
    F_GETPROC(FSOUND_SetPriority, "_FSOUND_SetPriority@8");
    F_GETPROC(FSOUND_SetReserved, "_FSOUND_SetReserved@8");
    F_GETPROC(FSOUND_SetPaused, "_FSOUND_SetPaused@8");
    F_GETPROC(FSOUND_SetLoopMode, "_FSOUND_SetLoopMode@8");
    F_GETPROC(FSOUND_SetCurrentPosition, "_FSOUND_SetCurrentPosition@8");
    F_GETPROC(FSOUND_3D_SetAttributes, "_FSOUND_3D_SetAttributes@12");
    F_GETPROC(FSOUND_3D_SetMinMaxDistance, "_FSOUND_3D_SetMinMaxDistance@12");
    F_GETPROC(FSOUND_IsPlaying, "_FSOUND_IsPlaying@4");
    F_GETPROC(FSOUND_GetFrequency, "_FSOUND_GetFrequency@4");
    F_GETPROC(FSOUND_GetVolume, "_FSOUND_GetVolume@4");
    F_GETPROC(FSOUND_GetAmplitude, "_FSOUND_GetAmplitude@4");
    F_GETPROC(FSOUND_GetPan, "_FSOUND_GetPan@4");
    F_GETPROC(FSOUND_GetSurround, "_FSOUND_GetSurround@4");
    F_GETPROC(FSOUND_GetMute, "_FSOUND_GetMute@4");
    F_GETPROC(FSOUND_GetPriority, "_FSOUND_GetPriority@4");
    F_GETPROC(FSOUND_GetReserved, "_FSOUND_GetReserved@4");
    F_GETPROC(FSOUND_GetPaused, "_FSOUND_GetPaused@4");
    F_GETPROC(FSOUND_GetLoopMode, "_FSOUND_GetLoopMode@4");
    F_GETPROC(FSOUND_GetCurrentPosition, "_FSOUND_GetCurrentPosition@4");
    F_GETPROC(FSOUND_GetCurrentSample, "_FSOUND_GetCurrentSample@4");
    F_GETPROC(FSOUND_GetCurrentLevels, "_FSOUND_GetCurrentLevels@12");
    F_GETPROC(FSOUND_GetNumSubChannels, "_FSOUND_GetNumSubChannels@4");
    F_GETPROC(FSOUND_GetSubChannel, "_FSOUND_GetSubChannel@8");
    F_GETPROC(FSOUND_3D_GetAttributes, "_FSOUND_3D_GetAttributes@12");
    F_GETPROC(FSOUND_3D_GetMinMaxDistance, "_FSOUND_3D_GetMinMaxDistance@12");
    F_GETPROC(FSOUND_3D_Listener_SetCurrent, "_FSOUND_3D_Listener_SetCurrent@8");
    F_GETPROC(FSOUND_3D_Listener_SetAttributes, "_FSOUND_3D_Listener_SetAttributes@32");
    F_GETPROC(FSOUND_3D_Listener_GetAttributes, "_FSOUND_3D_Listener_GetAttributes@32");
    F_GETPROC(FSOUND_3D_SetDopplerFactor, "_FSOUND_3D_SetDopplerFactor@4");
    F_GETPROC(FSOUND_3D_SetDistanceFactor, "_FSOUND_3D_SetDistanceFactor@4");
    F_GETPROC(FSOUND_3D_SetRolloffFactor, "_FSOUND_3D_SetRolloffFactor@4");
    F_GETPROC(FSOUND_FX_Enable, "_FSOUND_FX_Enable@8");
    F_GETPROC(FSOUND_FX_Disable, "_FSOUND_FX_Disable@4");
    F_GETPROC(FSOUND_FX_SetChorus, "_FSOUND_FX_SetChorus@32");
    F_GETPROC(FSOUND_FX_SetCompressor, "_FSOUND_FX_SetCompressor@28");
    F_GETPROC(FSOUND_FX_SetDistortion, "_FSOUND_FX_SetDistortion@24");
    F_GETPROC(FSOUND_FX_SetEcho, "_FSOUND_FX_SetEcho@24");
    F_GETPROC(FSOUND_FX_SetFlanger, "_FSOUND_FX_SetFlanger@32");
    F_GETPROC(FSOUND_FX_SetGargle, "_FSOUND_FX_SetGargle@12");
    F_GETPROC(FSOUND_FX_SetI3DL2Reverb, "_FSOUND_FX_SetI3DL2Reverb@52");
    F_GETPROC(FSOUND_FX_SetParamEQ, "_FSOUND_FX_SetParamEQ@16");
    F_GETPROC(FSOUND_FX_SetWavesReverb, "_FSOUND_FX_SetWavesReverb@20");
    F_GETPROC(FSOUND_Stream_Open, "_FSOUND_Stream_Open@16");
    F_GETPROC(FSOUND_Stream_Create, "_FSOUND_Stream_Create@20");
    F_GETPROC(FSOUND_Stream_Play, "_FSOUND_Stream_Play@8");
    F_GETPROC(FSOUND_Stream_PlayEx, "_FSOUND_Stream_PlayEx@16");
    F_GETPROC(FSOUND_Stream_Stop, "_FSOUND_Stream_Stop@4");
    F_GETPROC(FSOUND_Stream_Close, "_FSOUND_Stream_Close@4");
    F_GETPROC(FSOUND_Stream_SetEndCallback, "_FSOUND_Stream_SetEndCallback@12");
    F_GETPROC(FSOUND_Stream_SetSyncCallback, "_FSOUND_Stream_SetSyncCallback@12");
    F_GETPROC(FSOUND_Stream_GetSample, "_FSOUND_Stream_GetSample@4");
    F_GETPROC(FSOUND_Stream_CreateDSP, "_FSOUND_Stream_CreateDSP@16");
    F_GETPROC(FSOUND_Stream_SetBufferSize, "_FSOUND_Stream_SetBufferSize@4");
    F_GETPROC(FSOUND_Stream_SetPosition, "_FSOUND_Stream_SetPosition@8");
    F_GETPROC(FSOUND_Stream_GetPosition, "_FSOUND_Stream_GetPosition@4");
    F_GETPROC(FSOUND_Stream_SetTime, "_FSOUND_Stream_SetTime@8");
    F_GETPROC(FSOUND_Stream_GetTime, "_FSOUND_Stream_GetTime@4");
    F_GETPROC(FSOUND_Stream_GetLength, "_FSOUND_Stream_GetLength@4");
    F_GETPROC(FSOUND_Stream_GetLengthMs, "_FSOUND_Stream_GetLengthMs@4");
    F_GETPROC(FSOUND_Stream_SetMode, "_FSOUND_Stream_SetMode@8");
    F_GETPROC(FSOUND_Stream_GetMode, "_FSOUND_Stream_GetMode@4");
    F_GETPROC(FSOUND_Stream_SetSubStream, "_FSOUND_Stream_SetSubStream@8");
    F_GETPROC(FSOUND_Stream_GetNumSubStreams, "_FSOUND_Stream_GetNumSubStreams@4");
    F_GETPROC(FSOUND_Stream_SetSubStreamSentence, "_FSOUND_Stream_SetSubStreamSentence@12");
    F_GETPROC(FSOUND_Stream_SetLoopPoints, "_FSOUND_Stream_SetLoopPoints@12");
    F_GETPROC(FSOUND_Stream_SetLoopCount, "_FSOUND_Stream_SetLoopCount@8");
    F_GETPROC(FSOUND_Stream_AddSyncPoint, "_FSOUND_Stream_AddSyncPoint@12");
    F_GETPROC(FSOUND_Stream_DeleteSyncPoint, "_FSOUND_Stream_DeleteSyncPoint@4");
    F_GETPROC(FSOUND_Stream_GetNumSyncPoints, "_FSOUND_Stream_GetNumSyncPoints@4");
    F_GETPROC(FSOUND_Stream_GetSyncPoint, "_FSOUND_Stream_GetSyncPoint@8");
    F_GETPROC(FSOUND_Stream_GetSyncPointInfo, "_FSOUND_Stream_GetSyncPointInfo@8");
    F_GETPROC(FSOUND_Stream_GetOpenState, "_FSOUND_Stream_GetOpenState@4");
    F_GETPROC(FSOUND_Stream_GetNumTagFields, "_FSOUND_Stream_GetNumTagFields@8");
    F_GETPROC(FSOUND_Stream_GetTagField, "_FSOUND_Stream_GetTagField@24");
    F_GETPROC(FSOUND_Stream_FindTagField, "_FSOUND_Stream_FindTagField@20");
    F_GETPROC(FSOUND_Stream_Net_SetProxy, "_FSOUND_Stream_Net_SetProxy@4");
    F_GETPROC(FSOUND_Stream_Net_GetLastServerStatus, "_FSOUND_Stream_Net_GetLastServerStatus@0");
    F_GETPROC(FSOUND_Stream_Net_SetBufferProperties, "_FSOUND_Stream_Net_SetBufferProperties@12");
    F_GETPROC(FSOUND_Stream_Net_GetBufferProperties, "_FSOUND_Stream_Net_GetBufferProperties@12");
    F_GETPROC(FSOUND_Stream_Net_SetMetadataCallback, "_FSOUND_Stream_Net_SetMetadataCallback@12");
    F_GETPROC(FSOUND_Stream_Net_GetStatus, "_FSOUND_Stream_Net_GetStatus@20");
    F_GETPROC(FSOUND_CD_Play, "_FSOUND_CD_Play@8");
    F_GETPROC(FSOUND_CD_SetPlayMode, "_FSOUND_CD_SetPlayMode@8");
    F_GETPROC(FSOUND_CD_Stop, "_FSOUND_CD_Stop@4");
    F_GETPROC(FSOUND_CD_SetPaused, "_FSOUND_CD_SetPaused@8");
    F_GETPROC(FSOUND_CD_SetVolume, "_FSOUND_CD_SetVolume@8");
    F_GETPROC(FSOUND_CD_SetTrackTime, "_FSOUND_CD_SetTrackTime@8");
    F_GETPROC(FSOUND_CD_OpenTray, "_FSOUND_CD_OpenTray@8");
    F_GETPROC(FSOUND_CD_GetPaused, "_FSOUND_CD_GetPaused@4");
    F_GETPROC(FSOUND_CD_GetTrack, "_FSOUND_CD_GetTrack@4");
    F_GETPROC(FSOUND_CD_GetNumTracks, "_FSOUND_CD_GetNumTracks@4");
    F_GETPROC(FSOUND_CD_GetVolume, "_FSOUND_CD_GetVolume@4");
    F_GETPROC(FSOUND_CD_GetTrackLength, "_FSOUND_CD_GetTrackLength@8");
    F_GETPROC(FSOUND_CD_GetTrackTime, "_FSOUND_CD_GetTrackTime@4");
    F_GETPROC(FSOUND_DSP_Create, "_FSOUND_DSP_Create@12");
    F_GETPROC(FSOUND_DSP_Free, "_FSOUND_DSP_Free@4");
    F_GETPROC(FSOUND_DSP_SetPriority, "_FSOUND_DSP_SetPriority@8");
    F_GETPROC(FSOUND_DSP_GetPriority, "_FSOUND_DSP_GetPriority@4");
    F_GETPROC(FSOUND_DSP_SetActive, "_FSOUND_DSP_SetActive@8");
    F_GETPROC(FSOUND_DSP_GetActive, "_FSOUND_DSP_GetActive@4");
    F_GETPROC(FSOUND_DSP_GetClearUnit, "_FSOUND_DSP_GetClearUnit@0");
    F_GETPROC(FSOUND_DSP_GetSFXUnit, "_FSOUND_DSP_GetSFXUnit@0");
    F_GETPROC(FSOUND_DSP_GetMusicUnit, "_FSOUND_DSP_GetMusicUnit@0");
    F_GETPROC(FSOUND_DSP_GetClipAndCopyUnit, "_FSOUND_DSP_GetClipAndCopyUnit@0");
    F_GETPROC(FSOUND_DSP_GetFFTUnit, "_FSOUND_DSP_GetFFTUnit@0");
    F_GETPROC(FSOUND_DSP_MixBuffers, "_FSOUND_DSP_MixBuffers@28");
    F_GETPROC(FSOUND_DSP_ClearMixBuffer, "_FSOUND_DSP_ClearMixBuffer@0");
    F_GETPROC(FSOUND_DSP_GetBufferLength, "_FSOUND_DSP_GetBufferLength@0");
    F_GETPROC(FSOUND_DSP_GetBufferLengthTotal, "_FSOUND_DSP_GetBufferLengthTotal@0");
    F_GETPROC(FSOUND_DSP_GetSpectrum, "_FSOUND_DSP_GetSpectrum@0");
    F_GETPROC(FSOUND_Reverb_SetProperties, "_FSOUND_Reverb_SetProperties@4");
    F_GETPROC(FSOUND_Reverb_GetProperties, "_FSOUND_Reverb_GetProperties@4");
    F_GETPROC(FSOUND_Reverb_SetChannelProperties, "_FSOUND_Reverb_SetChannelProperties@8");
    F_GETPROC(FSOUND_Reverb_GetChannelProperties, "_FSOUND_Reverb_GetChannelProperties@8");
    F_GETPROC(FSOUND_Record_SetDriver, "_FSOUND_Record_SetDriver@4");
    F_GETPROC(FSOUND_Record_GetNumDrivers, "_FSOUND_Record_GetNumDrivers@0");
    F_GETPROC(FSOUND_Record_GetDriverName, "_FSOUND_Record_GetDriverName@4");
    F_GETPROC(FSOUND_Record_GetDriver, "_FSOUND_Record_GetDriver@0");
    F_GETPROC(FSOUND_Record_StartSample, "_FSOUND_Record_StartSample@8");
    F_GETPROC(FSOUND_Record_Stop, "_FSOUND_Record_Stop@0");
    F_GETPROC(FSOUND_Record_GetPosition, "_FSOUND_Record_GetPosition@0");
    F_GETPROC(FSOUND_File_SetCallbacks, "_FSOUND_File_SetCallbacks@20");
    F_GETPROC(FMUSIC_LoadSong, "_FMUSIC_LoadSong@4");
    F_GETPROC(FMUSIC_LoadSongEx, "_FMUSIC_LoadSongEx@24");
    F_GETPROC(FMUSIC_GetOpenState, "_FMUSIC_GetOpenState@4");
    F_GETPROC(FMUSIC_FreeSong, "_FMUSIC_FreeSong@4");
    F_GETPROC(FMUSIC_PlaySong, "_FMUSIC_PlaySong@4");
    F_GETPROC(FMUSIC_StopSong, "_FMUSIC_StopSong@4");
    F_GETPROC(FMUSIC_StopAllSongs, "_FMUSIC_StopAllSongs@0");
    F_GETPROC(FMUSIC_SetZxxCallback, "_FMUSIC_SetZxxCallback@8");
    F_GETPROC(FMUSIC_SetRowCallback, "_FMUSIC_SetRowCallback@12");
    F_GETPROC(FMUSIC_SetOrderCallback, "_FMUSIC_SetOrderCallback@12");
    F_GETPROC(FMUSIC_SetInstCallback, "_FMUSIC_SetInstCallback@12");
    F_GETPROC(FMUSIC_SetSample, "_FMUSIC_SetSample@12");
    F_GETPROC(FMUSIC_SetUserData, "_FMUSIC_SetUserData@8");
    F_GETPROC(FMUSIC_OptimizeChannels, "_FMUSIC_OptimizeChannels@12");
    F_GETPROC(FMUSIC_SetReverb, "_FMUSIC_SetReverb@4");
    F_GETPROC(FMUSIC_SetLooping, "_FMUSIC_SetLooping@8");
    F_GETPROC(FMUSIC_SetOrder, "_FMUSIC_SetOrder@8");
    F_GETPROC(FMUSIC_SetPaused, "_FMUSIC_SetPaused@8");
    F_GETPROC(FMUSIC_SetMasterVolume, "_FMUSIC_SetMasterVolume@8");
    F_GETPROC(FMUSIC_SetMasterSpeed, "_FMUSIC_SetMasterSpeed@8");
    F_GETPROC(FMUSIC_SetPanSeperation, "_FMUSIC_SetPanSeperation@8");
    F_GETPROC(FMUSIC_GetName, "_FMUSIC_GetName@4");
    F_GETPROC(FMUSIC_GetType, "_FMUSIC_GetType@4");
    F_GETPROC(FMUSIC_GetNumOrders, "_FMUSIC_GetNumOrders@4");
    F_GETPROC(FMUSIC_GetNumPatterns, "_FMUSIC_GetNumPatterns@4");
    F_GETPROC(FMUSIC_GetNumInstruments, "_FMUSIC_GetNumInstruments@4");
    F_GETPROC(FMUSIC_GetNumSamples, "_FMUSIC_GetNumSamples@4");
    F_GETPROC(FMUSIC_GetNumChannels, "_FMUSIC_GetNumChannels@4");
    F_GETPROC(FMUSIC_GetSample, "_FMUSIC_GetSample@8");
    F_GETPROC(FMUSIC_GetPatternLength, "_FMUSIC_GetPatternLength@8");
    F_GETPROC(FMUSIC_IsFinished, "_FMUSIC_IsFinished@4");
    F_GETPROC(FMUSIC_IsPlaying, "_FMUSIC_IsPlaying@4");
    F_GETPROC(FMUSIC_GetMasterVolume, "_FMUSIC_GetMasterVolume@4");
    F_GETPROC(FMUSIC_GetGlobalVolume, "_FMUSIC_GetGlobalVolume@4");
    F_GETPROC(FMUSIC_GetOrder, "_FMUSIC_GetOrder@4");
    F_GETPROC(FMUSIC_GetPattern, "_FMUSIC_GetPattern@4");
    F_GETPROC(FMUSIC_GetSpeed, "_FMUSIC_GetSpeed@4");
    F_GETPROC(FMUSIC_GetBPM, "_FMUSIC_GetBPM@4");
    F_GETPROC(FMUSIC_GetRow, "_FMUSIC_GetRow@4");
    F_GETPROC(FMUSIC_GetPaused, "_FMUSIC_GetPaused@4");
    F_GETPROC(FMUSIC_GetTime, "_FMUSIC_GetTime@4");
    F_GETPROC(FMUSIC_GetRealChannel, "_FMUSIC_GetRealChannel@8");
    F_GETPROC(FMUSIC_GetUserData, "_FMUSIC_GetUserData@4");

    return instance;
}

static void FMOD_FreeInstance(FMOD_INSTANCE *instance)
{
    if (instance)
    {
        if (instance->module)
        {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
            FreeLibrary((HMODULE)instance->module);
#else
            dlclose(instance->module);
#endif
        }
        free(instance);
    }
}

#endif

