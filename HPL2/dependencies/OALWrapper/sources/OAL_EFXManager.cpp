/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
#include "OALWrapper/OAL_EFXManager.h"
#include "OALWrapper/OAL_EffectSlot.h"
#include "OALWrapper/OAL_Effect.h"
#include "OALWrapper/OAL_Effect_Reverb.h"
#include "OALWrapper/OAL_Filter.h"
#include "OALWrapper/OAL_Device.h"

#include <SDL_thread.h>
#include <SDL_timer.h>
#include <SDL_version.h>

int SlotUpdaterThread(void* alUnusedArg);

extern cOAL_Device* gpDevice;

// Set EFX func pointers to null (maybe this should be in OAL_Device.cpp? )

// Effect Slots

LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots = NULL;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots = NULL;
LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot = NULL;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti = NULL;
LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv = NULL;
LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf = NULL;
LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv = NULL;
LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti = NULL;
LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv = NULL;
LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf = NULL;
LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv = NULL;

// Effects

LPALGENEFFECTS alGenEffects = NULL;
LPALDELETEEFFECTS alDeleteEffects = NULL;
LPALISEFFECT alIsEffect = NULL;
LPALEFFECTI alEffecti = NULL;
LPALEFFECTIV alEffectiv = NULL;
LPALEFFECTF alEffectf = NULL;
LPALEFFECTFV alEffectfv = NULL;
LPALGETEFFECTI alGetEffecti = NULL;
LPALGETEFFECTIV alGetEffectiv = NULL;
LPALGETEFFECTF alGetEffectf = NULL;
LPALGETEFFECTFV alGetEffectfv = NULL;

// Filters

LPALGENFILTERS alGenFilters = NULL;
LPALDELETEFILTERS alDeleteFilters = NULL;
LPALISFILTER alIsFilter = NULL;
LPALFILTERI alFilteri = NULL;
LPALFILTERIV alFilteriv = NULL;
LPALFILTERF alFilterf = NULL;
LPALFILTERFV alFilterfv = NULL;
LPALGETFILTERI alGetFilteri = NULL;
LPALGETFILTERIV alGetFilteriv = NULL;
LPALGETFILTERF alGetFilterf = NULL;
LPALGETFILTERFV alGetFilterfv = NULL;

cOAL_EFXManager::cOAL_EFXManager() : mlNumSlots(0), mpvSlots(NULL), mplstEffectList(NULL), mplstFilterList(NULL)  
{
}

cOAL_EFXManager::~cOAL_EFXManager()
{
}

bool cOAL_EFXManager::Initialize(int alNumSlotsHint, int alNumSends, bool abUseThreading, int alSlotUpdateFreq)
{
	DEF_FUNC_NAME(cOAL_EFXManager::Initialize);
	FUNC_USES_AL;
	
	ALuint lTempSlot[256];
    
	// Set up every EFX function pointer 

	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Initializing EFX Manager...\n" );
	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Initializing function pointers...\n" );



	// Slot funcs
	alGenAuxiliaryEffectSlots		=	(LPALGENAUXILIARYEFFECTSLOTS) alGetProcAddress ("alGenAuxiliaryEffectSlots");
	alDeleteAuxiliaryEffectSlots	=	(LPALDELETEAUXILIARYEFFECTSLOTS) alGetProcAddress ("alDeleteAuxiliaryEffectSlots");
	alIsAuxiliaryEffectSlot			=	(LPALISAUXILIARYEFFECTSLOT) alGetProcAddress ("alIsAuxiliaryEffectSlot");
	alAuxiliaryEffectSloti			=	(LPALAUXILIARYEFFECTSLOTI) alGetProcAddress ("alAuxiliaryEffectSloti");
	alAuxiliaryEffectSlotiv			=	(LPALAUXILIARYEFFECTSLOTIV) alGetProcAddress ("alAuxiliaryEffectSlotiv");
	alAuxiliaryEffectSlotf			=	(LPALAUXILIARYEFFECTSLOTF) alGetProcAddress ("alAuxiliaryEffectSlotf");
	alAuxiliaryEffectSlotfv			=	(LPALAUXILIARYEFFECTSLOTFV) alGetProcAddress ("alAuxiliaryEffectSlotfv");
	alGetAuxiliaryEffectSloti		=	(LPALGETAUXILIARYEFFECTSLOTI) alGetProcAddress ("alGetAuxiliaryEffectSloti");
	alGetAuxiliaryEffectSlotiv		=	(LPALGETAUXILIARYEFFECTSLOTIV) alGetProcAddress ("alGetAuxiliaryEffectSlotiv");
	alGetAuxiliaryEffectSlotf		=	(LPALGETAUXILIARYEFFECTSLOTF) alGetProcAddress ("alGetAuxiliaryEffectSlotf");
	alGetAuxiliaryEffectSlotfv		=	(LPALGETAUXILIARYEFFECTSLOTFV) alGetProcAddress ("alGetAuxiliaryEffectSlotfv");

	// Effect funcs
	alGenEffects	=	(LPALGENEFFECTS) alGetProcAddress ("alGenEffects");
	alDeleteEffects =	(LPALDELETEEFFECTS) alGetProcAddress ("alDeleteEffects");
	alIsEffect		=	(LPALISEFFECT) alGetProcAddress ("alIsEffect");
	alEffecti		=	(LPALEFFECTI) alGetProcAddress ("alEffecti");
	alEffectiv		=	(LPALEFFECTIV) alGetProcAddress ("alEffectiv");
	alEffectf		=	(LPALEFFECTF) alGetProcAddress ("alEffectf");
	alEffectfv		=	(LPALEFFECTFV) alGetProcAddress ("alEffectfv");
	alGetEffecti	=	(LPALGETEFFECTI) alGetProcAddress ("alGetEffecti");
	alGetEffectiv	=	(LPALGETEFFECTIV) alGetProcAddress ("alGetEffectiv");
	alGetEffectf	=	(LPALGETEFFECTF) alGetProcAddress ("alGetEffectf");
	alGetEffectfv	=	(LPALGETEFFECTFV) alGetProcAddress ("alGetEffectfv");
	
	// Filter funcs
	alGenFilters	= (LPALGENFILTERS) alGetProcAddress ("alGenFilters");
	alDeleteFilters = (LPALDELETEFILTERS) alGetProcAddress ("alDeleteFilters");
	alIsFilter		= (LPALISFILTER) alGetProcAddress ("alIsFilter");
	alFilteri		= (LPALFILTERI) alGetProcAddress ("alFilteri");
	alFilteriv		= (LPALFILTERIV) alGetProcAddress ("alFilteriv");
	alFilterf		= (LPALFILTERF) alGetProcAddress ("alFilterf");
	alFilterfv		= (LPALFILTERFV) alGetProcAddress ("alFilterfv");
	alGetFilteri	= (LPALGETFILTERI) alGetProcAddress ("alGetFilteri");
	alGetFilteriv	= (LPALGETFILTERIV) alGetProcAddress ("alGetFilteriv");
	alGetFilterf	= (LPALGETFILTERF) alGetProcAddress ("alGetFilterf");
	alGetFilterfv	= (LPALGETFILTERFV) alGetProcAddress ("alGetFilterfv");

	if (!(alGenAuxiliaryEffectSlots && alDeleteAuxiliaryEffectSlots && alIsAuxiliaryEffectSlot &&
		alAuxiliaryEffectSloti && alAuxiliaryEffectSlotiv && alAuxiliaryEffectSlotf && alAuxiliaryEffectSlotfv &&
		alGetAuxiliaryEffectSloti && alGetAuxiliaryEffectSlotiv && alGetAuxiliaryEffectSlotf && alGetAuxiliaryEffectSlotfv &&

		alGenEffects && alDeleteEffects && alIsEffect &&
		alEffecti && alEffectiv && alEffectf && alEffectfv && 
		alGetEffecti && alGetEffectiv && alGetEffectf && alGetEffectfv &&

		alGenFilters && alDeleteFilters && alIsFilter &&
		alFilteri && alFilteriv && alFilterf && alFilterfv &&
		alGetFilteri && alGetFilteriv && alGetFilterf && alGetFilterfv))
	{
		LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Error, "Failed initializing function pointers\n" );
		return false;
	}
	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Done\n" );

	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Calculating max slots...\n" );

	while ( mlNumSlots < alNumSlotsHint )
	{
		RUN_AL_FUNC(alGenAuxiliaryEffectSlots(1,&lTempSlot[mlNumSlots]));
		if (!AL_ERROR_OCCURED)
		{
			LogMsg("",eOAL_LogVerbose_High, eOAL_LogMsg_Info, "Effect Slot Object successfully created\n" );
			++mlNumSlots;
		}
		else
			break;
	}

	if ( mlNumSlots == 0 )
	{
		LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Error, "Error creating Slots. Failed to initialize EFX.\n" );
		return false;
	}

	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Created %d Low Level Effect Slots, %d hinted\n", mlNumSlots, alNumSlotsHint);

	RUN_AL_FUNC(alDeleteAuxiliaryEffectSlots ( mlNumSlots, lTempSlot ));
	
	mpvSlots = new tSlotVector;
	mpvSlots->reserve(mlNumSlots);

	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Creating High Level Effect Slots\n" );


	for ( int i = 0; i < mlNumSlots; i++ )
	{
		cOAL_EffectSlot *pSlot = new cOAL_EffectSlot(this, i);
		mpvSlots->push_back(pSlot);
	}

	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Done creating Effect Slots\n" );

	
	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Creating Filter and Effect containers\n" );
	mplstEffectList = new tOALEffectList;
	mplstFilterList = new tOALFilterList;

	if (!mplstEffectList || !mplstFilterList)
	{
		LogMsg("",eOAL_LogVerbose_Low, eOAL_LogMsg_Error, "Error creating containers. Failed to initialize EFX.\n" );
		return false;
	}
	else
		LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Done creating containers\n" );

	mlNumSends = alNumSends;
	mbUsingThread = abUseThreading;

	// Launch updater thread
	if (mbUsingThread)
	{
		LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Launching Slot updater thread...\n" );

		mlThreadWaitTime = 1000/alSlotUpdateFreq;
        
#if SDL_VERSION_ATLEAST(2, 0, 0)
		mpUpdaterThread = SDL_CreateThread ( SlotUpdaterThread, "EFX Slot Updater", NULL );
#else
		mpUpdaterThread = SDL_CreateThread ( SlotUpdaterThread, NULL );
#endif
	}
	
	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "EFX succesfully initialized.\n" );

	return true;


}

void cOAL_EFXManager::Destroy()
{
	tSlotVectorIt vSlotIterator;
	tOALEffectListIt lstEffectIterator;
	tOALFilterListIt lstFilterIterator;

	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Command, "Destroying EFX Manager...\n" );

	if ( mbUsingThread )							
	{
		LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Stopping Slot updater...\n" );
		mbUsingThread = false;
		SDL_WaitThread ( mpUpdaterThread, 0 );
		mpUpdaterThread = NULL;
	}

	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Destroying Effect Slots...\n" );

	if (mpvSlots)
	{
		for ( vSlotIterator = mpvSlots->begin(); vSlotIterator != mpvSlots->end(); ++vSlotIterator )
		{
			(*vSlotIterator)->Reset();
			delete (*vSlotIterator);
		}
		mpvSlots->clear();
		delete mpvSlots;
		
		mpvSlots = NULL;
	}

	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Destroying Effects...\n" );
	
	if (mplstEffectList)
	{
		for ( lstEffectIterator = mplstEffectList->begin(); lstEffectIterator != mplstEffectList->end(); ++lstEffectIterator )
		{
			delete (*lstEffectIterator);
		}
		mplstEffectList->clear();
		delete mplstEffectList;
		mplstEffectList = NULL;
	}

	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "Destroying Filters...\n" );

	if (mplstFilterList)
	{
		for ( lstFilterIterator = mplstFilterList->begin(); lstFilterIterator != mplstFilterList->end(); ++lstFilterIterator )
		{
			delete (*lstFilterIterator);
		}
		mplstFilterList->clear();
		delete mplstFilterList;
		mplstFilterList = NULL;
	}

	LogMsg("",eOAL_LogVerbose_Medium, eOAL_LogMsg_Info, "EFX Manager successfully destroyed\n" );

}

//////////////////////////////////////////////////////////////

cOAL_Filter* cOAL_EFXManager::CreateFilter()
{
    cOAL_Filter* pFilter = new cOAL_Filter;

	if(pFilter && pFilter->GetStatus())
	{
		mplstFilterList->push_back(pFilter);
		return pFilter;
	}
	else
	{
		delete  pFilter ;
		pFilter = NULL;
		
		return NULL;
	}
}

////////////////////////////////////////////////////////////

cOAL_Effect_Reverb* cOAL_EFXManager::CreateReverbEffect()
{
	cOAL_Effect_Reverb* pEffect = new cOAL_Effect_Reverb;

	if (pEffect && pEffect->GetStatus())
	{
		mplstEffectList->push_back(pEffect);
		return pEffect;
	}
	else
	{
        delete pEffect;
		pEffect = NULL;
		
		return NULL;
	}
	
}

////////////////////////////////////////////////////////////

void cOAL_EFXManager::DestroyFilter ( cOAL_Filter* apFilter )
{
	if (apFilter == NULL)
		return;

	mplstFilterList->remove(apFilter);
	delete apFilter;
}

////////////////////////////////////////////////////////////

void cOAL_EFXManager::DestroyEffect ( cOAL_Effect *apEffect )
{
	if (apEffect == NULL)
		return;

	mplstEffectList->remove(apEffect);
	delete apEffect;
}

////////////////////////////////////////////////////////////

int cOAL_EFXManager::UseEffect ( cOAL_Effect *apEffect )
{
	if (apEffect == NULL)
		return -1;

	cOAL_EffectSlot* pSlot = NULL;
	for (int i = 0; i < mlNumSlots; ++i)
	{
		pSlot = (*mpvSlots)[i];
		if (pSlot->IsFree())
		{
			if (pSlot->AttachEffect(apEffect))
            	return i;
			else
			{
				pSlot->Reset();
			}
		}
	}
	return -1;
}

void cOAL_EFXManager::UpdateSlots()
{
	cOAL_EffectSlot* pSlot;
	for (int i = 0; i < mlNumSlots; ++i)
	{
		pSlot = (*mpvSlots)[i];
		pSlot->Lock();
		pSlot->Update();
		pSlot->Unlock();
	}
}

inline int cOAL_EFXManager::GetThreadWaitTime()
{
	return mlThreadWaitTime;
}

int SlotUpdaterThread ( void* alUnusedArg )
{
	cOAL_EFXManager* pEFXManager = gpDevice->GetEFXManager();
	
	int lWaitTime = pEFXManager->GetThreadWaitTime();

	while(pEFXManager->IsThreadAlive())
	{
		//	While the thread lives, perform the update
        pEFXManager->UpdateSlots();
		//	And rest a bit
		SDL_Delay ( lWaitTime );			
	}
	return 0;
}

