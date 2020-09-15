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

#ifndef ED_HPL_MATERIAL_H
#define ED_HPL_MATERIAL_H

//----------------------------------------------------------

#include "../Common/EdWorld.h"
#include "../Common/EdVar.h"

class cEdWindowMaterialEditor;
class cHPLMaterial;

//----------------------------------------------------------

class cEdDummyRoot : public iEdObjectType
{
public:
	cEdDummyRoot();

protected:
	iEdObjectData* CreateTypeSpecificData();
};

//----------------------------------------------------------

class cEdMaterialClass : public iEdClass
{
public:
	cEdMaterialClass(cEdWindowMaterialEditor* apWindow);
	~cEdMaterialClass();

	bool Create(iMaterialType*);
	bool Create();

protected:
	void AddVariablesToInstance(cEdClassInstance*);

	iMaterialType* mpMatType;

	cEdWindowMaterialEditor* mpWindow;
	tEdVarVec mvVariables;
};

//----------------------------------------------------------

class cHplTexture
{
	friend class cHPLMaterial;
public:
	cHplTexture(cHPLMaterial*, eMaterialTexture);
	~cHplTexture();

	iTexture* GetEngTexture() { return mpEngTexture; }

	void Reset();

	void Load(cXmlElement* apElement);
	void Save(cXmlElement* apElement);

	void Reload();
	void Update();

	void CreateFromTexture(iTexture* apTexture);

	bool CheckFileIsUpdated();

	void SetEnabled(bool abX);
	void SetNeedsReload()			{ mbNeedsReload = true; }
	void SetUpdated()				{ mbUpdated = true; }

	bool IsValid()					{ return mbValid; }
	bool IsEnabled()				{ return mbEnabled; }

	const tWString& GetFile()		{ return msFile; }
	const tString& GetWrap()		{ return msWrap; }
	const tString& GetAnimMode()	{ return msAnimMode; }
	float GetFrameTime()			{ return mfFrameTime; }
	const tString& GetType()		{ return msType; }

	bool GetCompressed()			{ return mbCompressed; }
	bool GetUseMipMaps()			{ return mbMipMaps; }

	void SetFile(const tWString& asX);
	void SetWrap(const tWString& asX);
	void SetAnimMode(const tWString& asX);
	void SetFrameTime(float afX);

	void SetCompressed(bool abX);
	void SetUseMipMaps(bool abX);

protected:
	static eTextureType GetTextureTypeFromBitmap(cBitmap* apBmp);

	cHPLMaterial* mpMat;
	eMaterialTexture mUnit;

	bool mbValid;
	bool mbEnabled;
	bool mbNeedsReload;
	bool mbUpdated;

	tWString msFile;
	bool mbMipMaps;
	bool mbCompressed;
	tString msWrap;
	tString msType;
	tString msAnimMode;
	float mfFrameTime;

	iTexture* mpEngTexture;
	cDate mTimeStamp;
};

//----------------------------------------------------------

class cHPLMaterial : public iEdWorld
{
	friend class cHplTexture;
public:
	cHPLMaterial(iEditor*);
	~cHPLMaterial();

	void SetMatEditorWindow(cEdWindowMaterialEditor* apWin) { mpMatEditor = apWin; }

	cHplTexture* GetTextureWrapper(eMaterialTexture aUnit) { return mvTextures[aUnit]; }

	cEdClassInstance* GetClassInstance() { return mpClassInstance; }

	void SetType(const tWString& asType);
	void SetPhysicsMaterial(const tWString& asMat);
	void SetDepthTest(bool abX);
	void SetUseAlpha(bool abX);
	void SetBlendMode(const tWString& asMode);
	void SetHeightMapScale(float afX);
	void SetHeightMapBias(float afX);

	void AddUVAnim(eMaterialUvAnimation aType, float afSpeed, float afAmp, eMaterialAnimationAxis aAxis);
	void RemoveUVAnim(int alIdx);
	cMaterialUvAnimation* GetUVAnim(int alIdx);
	int GetUVAnimNum() { return (int)mvUVAnimations.size(); }
	void ClearUVAnims();

	const tWString& GetType() { return msType; }
	const tWString& GetPhysicsMat() { return msPhysicsMat; }
	bool GetDepthTest() { return mbDepthTest; }
	bool GetUseAlpha() { return mbUseAlpha; }
	const tWString& GetBlendMode() { return msBlendMode; }

	iMaterialType* GetTypePointer();
	void SetTextureUnitEnabled(eMaterialTexture aIdx, bool abX) { mvTextures[aIdx]->mbEnabled = abX; }
	void SetTextureUnitUpdated(eMaterialTexture aIdx)			{ mvTextures[aIdx]->mbUpdated = true; }
	void SetTextureUnitNeedsReload(eMaterialTexture aIdx)		{ mvTextures[aIdx]->mbNeedsReload = true; }

	cMaterial* GetPreviewMaterial();

	bool IsSaveable();


	void UpdateMaterialInMemory(const tString& asName);

	bool ReloadTextures();

	void SetUpdated() { mbPreviewUpdated = true; }

	bool LoadFromFile(const tWString&);
	bool SaveToFile(const tWString&);

	static tString gsTextureTypeStrings[];
	static tString gsWrapTypeStrings[];
	static tString gsAnimModeStrings[];
	static tString gsBlendModeStrings[];

protected:
	void OnInit();
	void OnReset();

	void OnWorldLoad();
	void OnWorldSave();
	void OnWorldParamsChange();

	void UpdatePreview();
	bool CheckDataIsValid();

	cEdWindowMaterialEditor* mpMatEditor;

	cMaterial*			mpEngMaterial;
	bool				mbUpdateMemoryMat;

	tWString msType;
	tWString msPhysicsMat;
	bool mbDepthTest;
	bool mbUseAlpha;
	tWString msBlendMode;

	std::vector<cHplTexture*>	mvTextures;
	std::vector<iTexture*>		mvDefaultTextures;

	std::vector<cMaterialUvAnimation> mvUVAnimations;

	cEdMaterialClass* mpMatClass;
	cEdClassInstance* mpClassInstance;

	bool mbPreviewUpdated;
	bool mbTypeChanged;
};

//----------------------------------------------------------

#endif // ED_HPL_MATERIAL_H
