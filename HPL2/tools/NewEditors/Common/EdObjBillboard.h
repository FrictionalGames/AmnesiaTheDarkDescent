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

#ifndef ED_OBJ_BILLBOARD_H
#define ED_OBJ_BILLBOARD_H

#include "../Common/EdScnObject.h"
#include "../Common/EdEngineObject.h"

#include "../Common/EdEditPaneBillboard.h"

//---------------------------------------------------------------------

class iEdWorld;
class cEdWindowViewport;
class iEdObjLight;

//---------------------------------------------------------------------

class cIconEntityBB : public iIconObject
{
public:
	cIconEntityBB(iEdScnObject* apParent);
	~cIconEntityBB();

	void Update();

	void Draw(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions, bool abIsSelected, bool abIsActive);

protected:
	bool ReCreateBB();
	void DestroyBB();

	bool mbSubs;
};

//---------------------------------------------------------------------

#define BillboardPropIdStart 20

enum eBillboardBool
{
	eBillboardBool_UseOffset = BillboardPropIdStart,
	eBillboardBool_IsHalo,

	eBillboardBool_LastEnum
};

enum eBillboardStr
{
	eBillboardStr_Type = BillboardPropIdStart,
	eBillboardStr_Material,
	eBillboardStr_ConnectedLight,

	eBillboardStr_LastEnum
};

enum eBillboardVec2f
{
	eBillboardVec2f_Size = BillboardPropIdStart,

	eBillboardVec2f_LastEnum
};

enum eBillboardVec3f
{
	eBillboardVec3f_HaloSourceSize = BillboardPropIdStart,

	eBillboardVec3f_LastEnum
};

enum eBillboardFloat
{
	eBillboardFloat_Offset = BillboardPropIdStart,

	eBillboardFloat_LastEnum
};

enum eBillboardCol
{
	eBillboardCol_Color = BillboardPropIdStart,

	eBillboardColor_LastEnum
};

//---------------------------------------------------------------------

class cTypeBillboard : public iEdScnObjType
{
public:
	cTypeBillboard();

	bool IsScalableGrouped() { return false; }

protected:
	iEdObjectData* CreateTypeSpecificData();
};

//---------------------------------------------------------------------

class cEdObjBillboardData : public iEdScnObjData
{
public:
	cEdObjBillboardData(iEdObjectType*);

protected:
	iEdObject* CreateTypeSpecificObject();
};

//---------------------------------------------------------------------

class cEdObjBillboard : public iEdScnObject
{
	friend class cIconEntityBB;
public:
	cEdObjBillboard(iEdObjectData*);
	virtual ~cEdObjBillboard();

	///////////////////////////////////////////////////////////
	// Property Interface
	bool GetProperty(int, cVector2f&);
	bool GetProperty(int, cVector3f&);
	bool GetProperty(int, bool&);
	bool GetProperty(int, float&);
	bool GetProperty(int, tString&);
	bool GetProperty(int, cColor&);

	bool SetProperty(int, const cVector2f&);
	bool SetProperty(int, const cVector3f&);
	bool SetProperty(int, const bool);
	bool SetProperty(int, const float);
	bool SetProperty(int, const tString&);
	bool SetProperty(int, const cColor&);

	void OnDraw(const cModuleDrawData&);

	void OnSetCulled(bool abX);

	void UpdateEntity();

	//void SaveToElement(cXmlElement* apElement);

	iEdEditPane* CreateEditPane();

	///////////////////////////////////////////
	// Own methods
	void SetBillboardOffset(float afX);
	float GetBillboardOffset() { return mfBillboardOffset; }

	void SetMaterialFile(const tString& asMatFile);
	const tString& GetMaterialFile() { return msMatFile; }

	void SetBillboardSize(const cVector2f& avSize);
	const cVector2f& GetBillboardSize() { return mvBillboardSize; }

	void SetBillboardType(const tString& asType);
	const tString& GetBillboardType() { return msType; }

	void SetBillboardColor(const cColor& aCol);
	cColor& GetBillboardColor() { return mCol; }

	void SetConnectedLightName(const tString& asLightName);
	const tString& GetConnectedLightName() { return msConnectedLightName; }
	void SetConnectedLight(iEdObjLight* apLight);

	void SetIsHalo(bool abX);
	bool GetIsHalo() { return mbIsHalo; }

	void SetHaloSourceSize(const cVector3f& avX);
	const cVector3f& GetHaloSourceSize() { return mvHaloSourceSize; }

	//void SetAbsScale(const cVector3f& avScale, int alAxis=-1);
	//const cVector3f& GetScale();

	void SetWorldMatrix(const cMatrixf& amtxX);

protected:
	static eBillboardType GetBillboardTypeFromString(const tString& asType);
	iEngineObject* CreateEngineObject();

	//////////////////////
	// Data
	//cVector3f mvAxis;

	tString msMatFile;
	tString msType;
	float  mfBillboardOffset;
	cVector2f mvBillboardSize;
	cColor mCol;

	bool mbIsHalo;
	cVector3f mvHaloSourceSize;

	tString msConnectedLightName;
	iEdObjLight* mpConnectedLight;

	bool mbBBUpdated;
	bool mbTypeUpdated;
	bool mbMatUpdated;
	bool mbColorUpdated;
	
	bool mbConnectLightUpdated;

	cVector3f mvBBScale;
};

//---------------------------------------------------------------------

#endif // ED_OBJ_BILLBOARD_H
