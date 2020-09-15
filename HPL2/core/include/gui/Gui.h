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

#ifndef HPL_GUI_H
#define HPL_GUI_H

#include <map>

#include "engine/Updateable.h"

#include "gui/GuiTypes.h"
#include "system/SystemTypes.h"

namespace hpl {
	
	class cResources;
	class cGraphics;
	class cSound;
	class cScene;
	class cInput;
	
	class cGuiSet;
	class cGuiSkin;
	class cGuiGfxElement;

	class iGuiMaterial;
	class iTexture;

	//------------------------------------------------

	typedef std::map<tString, cGuiSkin*> tGuiSkinMap;
	typedef tGuiSkinMap::iterator tGuiSkinMapIt;
	
	//-------------------------------------

	typedef std::map<tString, eGuiSkinGfx> tGuiSkinGfxMap;
	typedef tGuiSkinGfxMap::iterator tGuiSkinGfxMapIt;

	typedef std::map<tString, eGuiSkinFont> tGuiSkinFontMap;
	typedef tGuiSkinFontMap::iterator tGuiSkinFontMapIt;

	typedef std::map<tString, eGuiSkinAttribute> tGuiSkinAttributeMap;
	typedef tGuiSkinAttributeMap::iterator tGuiSkinAttributeMapIt;


	//------------------------------------------------

	class cGui : public iUpdateable
	{
	public:
		cGui();
		~cGui();

		///////////////////////////////
		// General

		void Init(	cResources *apResources, cGraphics* apGraphics,
					cSound *apSound, cScene *apScene, cInput *apInput);

		void Update(float afTimeStep);
		void OnDraw(float afFrameTime);
		void OnPostBufferSwap();

		iGuiMaterial* GetMaterial(eGuiMaterial aType);

		///////////////////////////////
		// Skins
		cGuiSkin* CreateSkin(const tString& asFile);
		
		eGuiSkinGfx GetSkinGfxFromString(const tString& asType);
		eGuiSkinFont GetSkinFontFromString(const tString& asType);
		eGuiSkinAttribute GetSkinAttributeFromString(const tString& asType);

		tString GetSkinGfxString(eGuiSkinGfx aType);
		tString GetSkinFontString(eGuiSkinFont aType);
		tString GetSkinAttributeString(eGuiSkinAttribute aType);

		///////////////////////////////
		// Sets
		cGuiSet* CreateSet(const tString& asName, cGuiSkin *apSkin);
		cGuiSet* GetSetFromName(const tString& asName);
		void SetFocus(cGuiSet* apSet);
		void SetFocusByName(const tString& asSetName);
		cGuiSet* GetFocusedSet(){return mpSetInFocus;}
		void DestroySet(cGuiSet *apSet);

		///////////////////////////////
		// Graphics creation
		cGuiGfxElement* CreateGfxFilledRect(const cColor& aColor, eGuiMaterial aMaterial, bool abAddToList=true);
		
		cGuiGfxElement* CreateGfxImage(	const tString &asFile,eGuiMaterial aMaterial,
										const cColor& aColor=cColor(1,1), bool abAddToList=true);
		
		cGuiGfxElement* CreateGfxTexture(	const tString &asFile,eGuiMaterial aMaterial,
											eTextureType aTextureType = eTextureType_2D,
											const cColor& aColor=cColor(1,1), 
											bool abMipMaps=false,
											bool abAddToList=true);
		
		cGuiGfxElement* CreateGfxTexture(	iTexture *apTexture, bool abAutoDestroyTexture, 
											eGuiMaterial aMaterial,
											const cColor& aColor=cColor(1,1),bool abAddToList=true,
											const cVector2f& avStartUV=0, const cVector2f& avEndUV=1);
		
		
		/**
		 * Loads several images asFile+00, etc. Used for animations.Must have extension!
		 */
		cGuiGfxElement* CreateGfxImageBuffer(	const tString &asFile,eGuiMaterial aMaterial,
												bool abCreateAnimation=true,
												const cColor& aColor=cColor(1,1), bool abAddToList=true);
		
		void DestroyGfx(cGuiGfxElement* apGfx);
		
		///////////////////////////////
		// Input sending TODO (need some other way to pass key modifiers, but this will do now)
		bool SendMousePos(const cVector2l &avPos, const cVector2l &avRel);
		bool SendMouseClickDown(eGuiMouseButton aButton, int alKeyModifiers=0);
		bool SendMouseClickUp(eGuiMouseButton aButton, int alKeyModifiers=0);
		bool SendMouseDoubleClick(eGuiMouseButton aButton, int alKeyModifiers=0);

		bool SendKeyPress(const cKeyPress& keyPress);
		bool SendKeyRelease(const cKeyPress& keyPress);

		bool SendGamepadInput(const cGamepadInputData& aInput);
		
		bool SendUIArrowPress(eUIArrow aX);
		bool SendUIArrowRelease(eUIArrow aX);

		bool SendUIButtonPress(eUIButton aX);
		bool SendUIButtonRelease(eUIButton aX);
		bool SendUIButtonDoublePress(eUIButton aX);

		eMouseButton TranslateGuiMouseToMouse(eGuiMouseButton aButton);
		eGuiMouseButton TranslateMouseToGuiMouse(eMouseButton aButton);

		///////////////////////////////
		// Properties
		cResources* GetResources(){ return mpResources;}
		cInput*	GetInput() { return mpInput; }



		static cGuiGfxElement* mpGfxRect;

	private:
		void GenerateSkinTypeStrings();

		cResources *mpResources;
		cGraphics *mpGraphics;
		cSound *mpSound;
		cScene *mpScene;
		cInput *mpInput;

		cGuiSet *mpSetInFocus;

		tGuiSetMap m_mapSets;
		tGuiSkinMap m_mapSkins;

		iGuiMaterial *mvMaterials[eGuiMaterial_LastEnum];

		tGuiGfxElementList mlstGfxElements;
		tGuiGfxElementList mlstToBeDestroyedGfxElements;

		tGuiSkinGfxMap m_mapSkinGfxStrings;
		tGuiSkinFontMap m_mapSkinFontStrings;
		tGuiSkinAttributeMap m_mapSkinAttributeStrings;

		unsigned long mlLastRenderTime;
	};

};
#endif // HPL_GUI_H
