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

#ifndef HPL_VIEWPORT_H
#define HPL_VIEWPORT_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "gui/GuiTypes.h"
#include "scene/SceneTypes.h"

namespace hpl {

	//------------------------------------------
	
	class cScene;
	class cCamera;
	class iRenderer;
	class iFrameBuffer;
	class cRenderSettings;
	class cPostEffectComposite;
	class cWorld;
	class iViewportCallback;
	class iRendererCallback;
	class cGuiSet;
	
	//------------------------------------------
	
	class cViewport
	{
	public:
		cViewport(cScene *apScene);
		~cViewport();

		void SetActive(bool abX){ mbActive = abX;}
		void SetVisible(bool abX) { mbVisible = abX;}
		bool IsActive(){ return mbActive;}
		bool IsVisible(){ return mbVisible;}

		void SetIsListener(bool abX){ mbIsListener = abX;}
		bool IsListener(){ return mbIsListener;}

		void SetCamera(cCamera *apCamera){ mpCamera = apCamera;}
		cCamera* GetCamera(){ return mpCamera;}

		void SetWorld(cWorld *apWorld);
		cWorld* GetWorld(){ return mpWorld;}
		
		void SetRenderer(iRenderer *apRenderer){ mpRenderer = apRenderer;}
		iRenderer* GetRenderer(){ return mpRenderer;}

		cRenderSettings* GetRenderSettings(){ return mpRenderSettings;}

		void SetFrameBuffer(iFrameBuffer *apFrameBuffer){ mRenderTarget.mpFrameBuffer = apFrameBuffer;}
		iFrameBuffer* GetFrameBuffer(){ return mRenderTarget.mpFrameBuffer;}

		void SetPostEffectComposite(cPostEffectComposite *apPostEffectComposite){ mpPostEffectComposite = apPostEffectComposite;}
		cPostEffectComposite* GetPostEffectComposite(){ return mpPostEffectComposite;}

		void AddGuiSet(cGuiSet *apSet);
		void RemoveGuiSet(cGuiSet *apSet);
		cGuiSetListIterator GetGuiSetIterator();

		void SetPosition(const cVector2l& avPos){ mRenderTarget.mvPos = avPos;}
		void SetSize(const cVector2l& avSize){ mRenderTarget.mvSize = avSize;}
		
		const cVector2l& GetPosition(){ return  mRenderTarget.mvPos;}
		const cVector2l& GetSize(){ return mRenderTarget.mvSize;}

		cRenderTarget* GetRenderTarget(){ return &mRenderTarget; }
		
		void AddViewportCallback(iViewportCallback *apCallback);
		void RemoveViewportCallback(iViewportCallback *apCallback);
		void RunViewportCallbackMessage(eViewportMessage aMessage);

		void AddRendererCallback(iRendererCallback *apCallback);
		void RemoveRendererCallback(iRendererCallback *apCallback);
		tRendererCallbackList* GetRendererCallbackList(){ return &mlstRendererCallbacks;}
			
	private:
		cScene *mpScene;

		cCamera *mpCamera;
		cWorld *mpWorld;

        bool mbActive;
		bool mbVisible;

		bool mbIsListener;

		iRenderer *mpRenderer;
		cPostEffectComposite *mpPostEffectComposite;

		cRenderTarget mRenderTarget;
		
		tViewportCallbackList mlstCallbacks;
		tRendererCallbackList mlstRendererCallbacks;
		tGuiSetList mlstGuiSets;
		
		cRenderSettings *mpRenderSettings;
	};

	//------------------------------------------

};
#endif // HPL_VIEWPORT_H
