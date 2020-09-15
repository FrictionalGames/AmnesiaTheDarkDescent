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

#ifndef HPLEDITOR_ENTITY_WRAPPER
#define HPLEDITOR_ENTITY_WRAPPER

#include "../common/StdAfx.h"

using namespace hpl;

#include "EditorTypes.h"

#include "EditorUserClassDefinitionManager.h"

//-----------------------------------------------------------------------

class iPropVal;

class iEditorBase;

class cEntityIcon;
class iEngineEntity;

class iEditorWorld;
class iEditorAction;
class cEditorEditModeSelect;
class cEditorWindowViewport;

class iEntityWrapperDataCreator;

class cEditorWindowEntityEditBox;
class cEditorWindowEntityEditBoxGroup;

class cEntityWrapperCompoundObject;

class cEditorClipPlane;

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
// PROPERTY IDS
/////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

enum eObjInt
{
	eObjInt_ID,

	eObjInt_LastEnum,
};

enum eObjFloat
{
	eObjFloat_LastEnum,
};

enum eObjBool
{
	eObjBool_Active,

	eObjBool_LastEnum,
};

enum eObjStr
{
	eObjStr_Name,
	eObjStr_Tag,

	eObjStr_LastEnum,
};

enum eObjVec2f
{
	eObjVec2f_LastEnum,
};

enum eObjVec3f
{
	eObjVec3f_Position,
	eObjVec3f_Rotation,
	eObjVec3f_Scale,

	eObjVec3f_LastEnum,
};

enum eObjCol
{
	eObjCol_Color,


    eObjCol_LastEnum,
};

enum ePropCopyStep
{
	ePropCopyStep_PreEnt,
	ePropCopyStep_PostEnt,
	ePropCopyStep_PostDeployAll,

	ePropCopyStep_LastEnum,
};


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

enum eScaleType
{
	eScaleType_Normal,
	eScaleType_NonNeg,
	eScaleType_UniformOnly,
	eScaleType_NonNegUniformOnly,
	eScaleType_None,
};

//-----------------------------------------------------------------------

////////////////////////////////////
// iProp
//  Interface for entity properties. Used to automate entity creation and saving,
//  also helps in creating actions that modify these.
//	Each property has an ID number, a type, a name and a bool that says if it should be d.
//	Derived classes should also store default values
//
class iProp
{
public:
	iProp(int alID, eVariableType aType, const tString& asName, bool abSave) : mlID(alID), mType(aType), msName(asName), mbSave(abSave)
	{}

	void SetSaved(bool abX) { mbSave = abX; }

	int GetID() { return mlID; }
	eVariableType GetType() { return mType; }
	const tString& GetName() { return msName; }
	bool IsSaved() { return mbSave; }

	/**
	 * Creates a specific value. Each derived class must implement this method.
	 * \return 
	 */
	virtual iPropVal* CreateValue()=0;

protected:
	int mlID;
	eVariableType mType;
	tString msName;
	bool mbSave;
};

////////////////////////////////////
// iPropVal
//  Used to associate properties with their values in an instance.
//	Has methods to update an entity, or get its value from an entity, load from and save to a xml element
//	These will be stored in an EntityWrapperData object
//
class iPropVal
{
public:
	iPropVal(iProp*);
	iProp* GetProperty() { return mpProp; }

	/**
	 * Gets property value from EntityWrapper
	 * \param iEntityWrapper* 
	 */
	virtual void GetFromEntity(iEntityWrapper*)=0;
	/**
	 * Sets property value to EntityWrapper
	 * \param iEntityWrapper* 
	 */
	virtual void SetToEntity(iEntityWrapper*)=0;


	/**
	 * Load value from a XML element
	 * \param cXmlElement* 
	 */
	virtual void Load(cXmlElement*)=0;
	/**
	 * Save value to a XML element
	 * \param cXmlElement* 
	 */
	virtual void Save(cXmlElement*);
	/**
	 * Type specific saving routines
	 * \param cXmlElement* 
	 */
	virtual void SaveSpecific(cXmlElement*)=0;

protected:
	iProp* mpProp;
};

//-----------------------------------------------------------------------

typedef std::list<iProp*> tPropList;
typedef tPropList::iterator tPropListIt;

typedef std::list<iPropVal*> tPropValList;
typedef tPropValList::iterator tPropValListIt;

//-----------------------------------------------------------------------

////////////////////////////////////
// cPropInt
//  Represents an integer property
//
class cPropInt : public iProp
{
public:
	cPropInt(int alID, const tString& asName, int alX, bool abSave=true) : iProp(alID, eVariableType_Int, asName, abSave), mlVal(alX)
	{}

	iPropVal* CreateValue();

	int GetDefault() { return mlVal; }

private:
	int mlVal;
};

////////////////////////////////////
// cPropValInt
//  Represents an integer value
//
class cPropValInt: public iPropVal
{
public:
	cPropValInt(iProp*);

	void Load(cXmlElement* apElement);
	void SaveSpecific(cXmlElement* apElement);

	void GetFromEntity(iEntityWrapper*);
	void SetToEntity(iEntityWrapper*);

	int& Get() { return mlVal; }
	void Set(int alX) { mlVal = alX; }

protected:
	int mlVal;
};

//-----------------------------------------------------------------------

////////////////////////////////////
// cPropFloat
//  Represents a float property
//
class cPropFloat : public iProp
{
public:
	cPropFloat(int alID, const tString& asName, float afX, bool abSave=true) : iProp(alID, eVariableType_Float, asName, abSave), mfVal(afX)
	{}

	iPropVal* CreateValue();

	float GetDefault() { return mfVal; }

protected:
	float mfVal;
};

////////////////////////////////////
// cPropValFloat
//  Represents a float value
//
class cPropValFloat : public iPropVal
{
public:
	cPropValFloat(iProp*);

	void GetFromEntity(iEntityWrapper*);
	void SetToEntity(iEntityWrapper*);

	void Load(cXmlElement* apElement);
	void SaveSpecific(cXmlElement* apElement);

	float& Get() { return mfVal; }
	void Set(float afX) { mfVal = afX; }

protected:
	float mfVal;
};

//-----------------------------------------------------------------------

////////////////////////////////////
// cPropBool
//  Represents a boolean property
//
class cPropBool : public iProp
{
public:
	cPropBool(int alID, const tString& asName, bool abX, bool abSave=true) : iProp(alID, eVariableType_Bool, asName, abSave), mbVal(abX)
	{}

	iPropVal* CreateValue();

	bool GetDefault() { return mbVal; }

private:
	bool mbVal;
};

////////////////////////////////////
// cPropValBool
//  Represents a boolean value
//
class cPropValBool : public iPropVal
{
public:
	cPropValBool(iProp*);

	void GetFromEntity(iEntityWrapper*);
	void SetToEntity(iEntityWrapper*);

	void Load(cXmlElement* apElement);
	void SaveSpecific(cXmlElement* apElement);

	bool& Get() { return mbVal; }
	void Set(bool abX) { mbVal = abX; }

private:
	bool mbVal;
};

//-----------------------------------------------------------------------

////////////////////////////////////
// cPropStr
//  Represents a string property
//
class cPropStr : public iProp
{
public:
	cPropStr(int alID, const tString& asName, const tString& asX, bool abSave=true) : iProp(alID, eVariableType_String, asName, abSave), msVal(asX)
	{}

	iPropVal* CreateValue();

	const tString& GetDefault() { return msVal; }

private:
	tString msVal;
};

////////////////////////////////////
// cPropValStr
//  Represents a string value
//
class cPropValStr : public iPropVal
{
public:
	cPropValStr(iProp*);

	void GetFromEntity(iEntityWrapper*);
	void SetToEntity(iEntityWrapper*);

	void Load(cXmlElement* apElement);
	void SaveSpecific(cXmlElement* apElement);

	tString& Get() { return msVal; }
	void Set(const tString& asX) { msVal = asX; }

private:
	tString msVal;
};

//-----------------------------------------------------------------------

////////////////////////////////////
// cPropVec2f
//  Represents a cVector2f property
//
class cPropVec2f : public iProp
{
public:
	cPropVec2f(int alID, const tString& asName, const cVector2f& avX, bool abSave=true) : iProp(alID, eVariableType_Vec2, asName, abSave), mvVal(avX)
	{}

	iPropVal* CreateValue();

	const cVector2f& GetDefault() { return mvVal; }

private:
	cVector2f mvVal;
};

////////////////////////////////////
// cPropValVec2f
//  Represents a cVector2f value
//
class cPropValVec2f : public iPropVal
{
public:
	cPropValVec2f(iProp*);

	void GetFromEntity(iEntityWrapper*);
	void SetToEntity(iEntityWrapper*);

	void Load(cXmlElement* apElement);
	void SaveSpecific(cXmlElement* apElement);

	cVector2f& Get() { return mvVal; }
	void Set(const cVector2f& avX) { mvVal = avX; }

private:
	cVector2f mvVal;
};

//-----------------------------------------------------------------------

////////////////////////////////////
// cPropVec3f
//  Represents a cVector3f property
//
class cPropVec3f : public iProp
{
public:
	cPropVec3f(int alID, const tString& asName, const cVector3f& avX, bool abSave=true) : iProp(alID, eVariableType_Vec3, asName, abSave), mvVal(avX)
	{}

	iPropVal* CreateValue();

	const cVector3f& GetDefault() { return mvVal; }

private:
	cVector3f mvVal;
};

////////////////////////////////////
// cPropValVec3f
//  Represents a cVector3f value
//
class cPropValVec3f : public iPropVal
{
public:
	cPropValVec3f(iProp*);

	void GetFromEntity(iEntityWrapper*);
	void SetToEntity(iEntityWrapper*);

	void Load(cXmlElement* apElement);
	void SaveSpecific(cXmlElement* apElement);

	cVector3f& Get() { return mvVal; }
	void Set(const cVector3f& avX) { mvVal = avX; }

private:
	cVector3f mvVal;
};


//-----------------------------------------------------------------------

////////////////////////////////////
// cPropCol
//  Represents a color property
//
class cPropCol : public iProp
{
public:
	cPropCol(int alID, const tString& asName, const cColor& aX, bool abSave=true) : iProp(alID, eVariableType_Color, asName, abSave), mVal(aX)
	{}

	iPropVal* CreateValue();

	const cColor& GetDefault() { return mVal; }

private:
	cColor mVal;
};

////////////////////////////////////
// cPropValCol
//  Represents a color value
//
class cPropValCol : public iPropVal
{
public:
	cPropValCol(iProp*);

	void GetFromEntity(iEntityWrapper*);
	void SetToEntity(iEntityWrapper*);

	void Load(cXmlElement*);
	void SaveSpecific(cXmlElement*);

	cColor& Get() { return mVal; }
	void Set(const cColor& aX) { mVal = aX; }

private:
	cColor mVal;
};

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// iEntityWrapperType
//	Base for Entity types. A type holds the properties that belong to a kind of entity.
//	Also states restrictions for edition of each kind.
//	This is responsible for creating Entity Data objects and set their default values.
//
class iEntityWrapperType
{
public:
	iEntityWrapperType(int, const tWString&, const tString& );
	virtual ~iEntityWrapperType();

	/**
	 * Creates an EntityWrapperData object
	 * \return 
	 */
	iEntityWrapperData* CreateData();
	/**
	 * Sets Data object to default values
	 * \param iEntityWrapperData* 
	 */
	void CreateValuesInData(iEntityWrapperData*);

	/////////////////////////////////////////////////
	// Own data
	void SetWorld(iEditorWorld* apWorld);
	iEditorWorld* GetWorld() { return mpWorld; }
	int GetID() { return mlID; }
	const tWString& GetName() { return msName; }
	const tString& GetXmlElementName() { return msXmlElementName; }

	virtual tString ToString();

	///////////////////////////////////////////////////
	// Used when loading an entity, check if data created by this type can load it
	virtual bool IsAppropriateType(cXmlElement*);
	virtual bool IsAppropriateDefaultType(cXmlElement*);

	///////////////////////////////////////////////////
	// Restrictions
	virtual bool AllowNullEngineEntity() { return false; }

	bool IsTranslateable() { return mbCanTranslate; }
	bool IsRotateable()		{ return mbCanRotate; }
	bool IsScalable()		{ return mScaleType!=eScaleType_None; }
	eScaleType GetScaleType() { return mScaleType; }
	bool IsRenameable()		{ return mbRenameable; }
	bool IsDeletable()		{ return mbDeletable; }
	bool IsCloneable()		{ return mbCloneable; }

	virtual bool IsTranslateableGrouped() { return true; }
	virtual bool IsRotateableGrouped() { return true; }
	virtual bool IsScalableGrouped() { return true; }

	//////////////////////////////////////////////////
	// Visiblity
	virtual bool IsVisible() { return mbVisible; }
	virtual void SetVisible(bool abX);

	//////////////////////////////////////////////////
	// Enabled
	virtual bool IsActive() { return mbActive; }
	virtual void SetActive(bool abX);

	//////////////////////////////////////////////////
	// Observers for properties
	cPropInt* GetPropInt(int);
	cPropFloat* GetPropFloat(int);
	cPropBool* GetPropBool(int);
	cPropStr*  GetPropString(int);
	cPropVec2f* GetPropVec2f(int);
	cPropVec3f* GetPropVec3f(int);
	cPropCol* GetPropColor(int);

	// Helpers for getting properties
	iProp* GetPropByTypeAndID(eVariableType, int);
	iProp* GetPropByName(const tString&);

	
	/**
	 * Gets a vector of integer IDs that represent the types that can be attached to 
	 * entities of this type, if any.
	 * \return 
	 */
	const tIntVec& GetAttachableTypes() { return mvAttachableTypes; }

protected:
	virtual void OnSetWorld(iEditorWorld*) {}

	/////////////////////////////////////////////////
	// Used to set up properties in a type
	void AddInt(int, const tString&, int alDefault=-1, ePropCopyStep aStep=ePropCopyStep_PostEnt, bool abSave=true);
	void AddFloat(int, const tString&, float afDefault=0.0f, ePropCopyStep aStep=ePropCopyStep_PostEnt, bool abSave=true);
	void AddBool(int, const tString&, bool abDefault=true, ePropCopyStep aStep=ePropCopyStep_PostEnt, bool abSave=true);
	void AddString(int, const tString&, const tString& asDefault="", ePropCopyStep aStep=ePropCopyStep_PostEnt, bool abSave=true);
	void AddVec2f(int, const tString&, const cVector2f& avDefault=0, ePropCopyStep aStep=ePropCopyStep_PostEnt, bool abSave=true);
	void AddVec3f(int, const tString&, const cVector3f& avDefault=0, ePropCopyStep aStep=ePropCopyStep_PostEnt, bool abSave=true);
	void AddColor(int, const tString&, const cColor& aDefault=cColor(1), ePropCopyStep aStep=ePropCopyStep_PostEnt, bool abSave=true);

	void AddProperty(iProp*, ePropCopyStep);

	
	/**
	 * Creates Data objects of a specific type. This needs to be implemented by every child class
	 * \return 
	 */
	virtual iEntityWrapperData* CreateSpecificData()=0;

	/////////////////////////////////////////////
	// Data
	// Properties
	std::vector<tPropList> mvProperties;

	tIntVec mvAttachableTypes;

	iEditorWorld* mpWorld;
	int mlID;
	tWString msName;
	tString msXmlElementName;

	int mlEditFlags;

	bool mbCanTranslate;
	bool mbCanRotate;
	eScaleType mScaleType;
	bool mbRenameable;
	bool mbDeletable;
	bool mbCloneable;

	bool mbVisible;
	bool mbActive;
};

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// iEntityWrapperData
//	Base for Entity Data. EntityWrapperData holds all data that is used to set up an Entity in a map.
//	Used for loading, saving and undo delete operations.
//	This is responsible for creating EntityWrapper objects and set their values as stored by them.
//
class iEntityWrapperData
{
public:
	iEntityWrapperData(iEntityWrapperType*);
	virtual ~iEntityWrapperData();

	iEntityWrapperType* GetType() { return mpType; }

	/**
	 * Any setup that must be done after creating the data object must go here
	 * \return 
	 */
	virtual bool PostCreateSetUp(){ return true; }

	virtual void CopyFromEntity(iEntityWrapper*);
	virtual void CopyToEntity(iEntityWrapper*, int alCopyFlags);
	virtual bool Load(cXmlElement*);
	virtual bool Save(cXmlElement*);
	virtual bool SaveSpecific(cXmlElement*);

	/**
	 * Creates an EntityWrapper object
	 * \return 
	 */
	virtual iEntityWrapper* CreateEntity();

	/////////////////////////////////////////////////////
	// Generic property handlers
	void AddValue(ePropCopyStep aX, iPropVal* apVal);

	void SetInt(int, int);
	void SetFloat(int, float);
	void SetBool(int, bool);
	void SetString(int, const tString&);
	void SetVec2f(int, const cVector2f&);
	void SetVec3f(int, const cVector3f&);
	void SetColor(int, const cColor&);

	int GetInt(int);
	float GetFloat(int);
	bool GetBool(int);
	const tString& GetString(int);
	const cVector2f& GetVec2f(int);
	const cVector3f& GetVec3f(int);
	const cColor& GetColor(int);

	iProp* GetProperty(int, eVariableType);
	iPropVal* GetValue(int, eVariableType);

	//////////////////////////////////////////////////
	// Specific property handlers
	int GetID();
	const tString& GetName();

	void SetID(int alX);
	void SetName(const tString& asX);

	void SetCompoundID(int alX) { mlCompoundID = alX; }

	//////////////////////////////////////////////////
	// Extension Data (for Editor specifics)
	void SetExtData(void* apData) { mpExtData = apData; }
	void* GetExtData() { return mpExtData; }

protected:

	/**
	 * Helper to save children as xml child elements with an ID attribute
	 * \param cXmlElement* 
	 */
	void SaveChildren(cXmlElement*);
	/**
	 * Helper to load children in the format saved by SaveChildren
	 * \param cXmlElement* 
	 */
	void LoadChildren(cXmlElement*);

	/**
	 * Creates an EntityWrapper object of a specific type. All final derived classes must implement this.
	 * \return 
	 */
	virtual iEntityWrapper* CreateSpecificEntity()=0;

	/////////////////////////////////////////////
	// Data
	iEntityWrapperType* mpType;
	std::vector<tPropValList> mvValues;

	// Hierarchy
	int mlParentID;
	tIntVec mvChildIDs;

	int mlCompoundID;

	// Extra data
	void *mpExtData;
};

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// iEntityWrapper
//	Base for Entity objects. EntityWrapper represents an actual entity object in the map.
//	Can have an engine entity associated to it, or an Icon object.
//	All editor operations work on objects derived from this base class.
//	Has methods for transforming, interface for setting and getting Properties, cloning helpers, 
//	and basic action creators.
//
class iEntityWrapper
{
	friend class iEditorWorld;
public:
	iEntityWrapper(iEntityWrapperData* apData);
	virtual ~iEntityWrapper();

	////////////////////////////////////////////////////////////
	//
	virtual bool Create();
	virtual void OnPostDeployAll(bool);

	iEntityWrapperData* GetCreationData() { return mpData; }

	///////////////////////////
	// Link to type
	iEntityWrapperType* GetType()			{ return mpType; }
	int	GetTypeID()							{ return mpType->GetID(); }
	const tWString& GetTypeName()			{ return mpType->GetName(); }

	iEditorWorld* GetEditorWorld();

	////////////////////////////////////////////////////////////
	// Editor restrictions
	virtual bool CanTranslate() { return mpType->IsTranslateable(); }
	virtual bool CanRotate() { return mpType->IsRotateable(); }
	virtual bool CanScale() { return mpType->IsScalable(); }
	virtual eScaleType GetScaleType() { return mpType->GetScaleType(); }
	virtual bool IsRenameable() { return mpType->IsRenameable(); }
	virtual bool IsCloneable() { return mpType->IsCloneable(); }
	virtual bool IsDeletable() { return mpType->IsDeletable(); }

	virtual bool IsTranslateableGrouped() { return mpType->IsTranslateableGrouped(); }
	virtual bool IsRotateableGrouped() { return mpType->IsRotateableGrouped(); }
	virtual bool IsScalableGrouped() { return mpType->IsScalableGrouped(); }

	virtual bool IsAffectedByDecal(bool abAffectsStaticObject, bool abAffectsPrimitive, bool abAffectsEntity) { return false; }


	/////////////////////////////////////////////////////////////
	// Clone
	iEntityWrapperData* CreateCopyData();
	virtual tIntVec GetCloneIDs();
	virtual iEntityWrapper* Clone(const tIntVec& avIDs, bool abDestroyData=true);
	virtual void SetUpCloneData(iEntityWrapperData*, const tIntVec&) {}

	/////////////////////////////////////////////////////////////
	// Hierarchy stuff
	void SetParent(iEntityWrapper* apEntity, bool abDetachFromOld=true);
	virtual iEntityWrapper* GetParent() { return mpParent; }

	int GetParentID();

	void AttachChild(iEntityWrapper* apEntity);
	void DetachChild(iEntityWrapper* apEntity);
	bool HasChild(iEntityWrapper* apEntity);
	tEntityWrapperList& GetChildren() { return mlstChildren; }
	void ClearChildren();
	void SaveChildren(cXmlElement* apElement);

	const tIntVec& GetAttachableTypes() { return mpType->GetAttachableTypes(); }

	void Save(cXmlElement* apParentElement);

	///////////////////////////////////////////////
	// Transforms
	void SetLastTransformedAxis(int alX) { mlLastTransformedAxis = alX; }
	virtual void SetAbsPosition(const cVector3f& avPosition);
	virtual void SetAbsScale(const cVector3f& avScale, int alAxis=-1); 
	virtual void SetAbsRotation(const cVector3f& avRotation);

	virtual const cVector3f& GetPosition() { return mvPosition; }
	virtual const cVector3f& GetScale() { return mvScale; }
	virtual const cVector3f& GetRotation() { return mvRotation; }

	const cMatrixf& GetWorldMatrix();
	virtual void SetWorldMatrix(const cMatrixf& amtxX);

	const cMatrixf& GetTranslateMatrix() { return mmtxTranslate; }
	const cMatrixf& GetRotateMatrix() { return mmtxRotate; }
	const cMatrixf& GetScaleMatrix() { return mmtxScale; }
	
	void SnapToGrid();
	
	virtual void UpdateMatrix();
	virtual void UpdateEntity();

	/////////////////////////////////////////////////////////////////
	// Picking helpers
	virtual cMeshEntity* GetMeshEntity();
	virtual cBoundingVolume* GetPickBV(cEditorWindowViewport* apViewport);
	virtual bool EntitySpecificFilterCheck(bool abPassAll, bool abPassType);

	void SetSelected(bool abX);
	bool IsSelected() { return mbSelected; }

	bool IsInsideFrustum(cFrustum* apFrustum);
	
	virtual cRect2l GetClipRectangle(cEditorWindowViewport* apViewport);

	virtual bool CheckRayIntersect(cEditorWindowViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT=NULL);
	virtual bool Check2DBoxIntersect(cEditorWindowViewport* apViewport, const cRect2l& aBox, cVector3f* apPos);

	cVector3f GetViewSpacePosition(cEditorWindowViewport* apViewport);

	tString& GetFilename() { return msFilename; }
	virtual void SetFilename(const tString& asFilename) { msFilename = asFilename; }

	iEditorAction* CreateModifyPositionAction(cEditorEditModeSelect* apEditMode, const cVector3f& avPos);
	iEditorAction* CreateModifyRotationAction(cEditorEditModeSelect* apEditMode,const cVector3f& avRotation);
	iEditorAction* CreateModifyScaleAction(cEditorEditModeSelect* apEditMode,const cVector3f& avScale);

	///////////////////////////
	// EditBox setup methods
	virtual cEditorWindowEntityEditBox* CreateEditBox(cEditorEditModeSelect* apEditMode)=0;
	virtual cEditorWindowEntityEditBoxGroup* CreateGroupAllSameTypeEditBox(cEditorEditModeSelect* apEditMode, tEntityWrapperList& alstEntities) { return NULL; }

	bool IsActive();
	void SetActive(bool abX);

	///////////////////////////
	// Edition Helper stuff
	void UpdateVisibility();
	bool IsVisible();
	void SetVisible(bool abX) { mbVisible = abX; }

	///////////////////////////
	// Properties
	void SetID(int alX) { mlID = alX; }
	int GetID() { return mlID; }

	tString& GetName() { return msName; }
	void SetName(const tString& asName);

	void SetTag(const tString& asTag) { msTag = asTag; }
	tString& GetTag() { return msTag; }


	// Really ugly and quick hack to help on FileIndex sanitizing
	virtual int GetFileIndexForType(int alTypeID) { if(alTypeID==GetTypeID()) return GetFileIndex(); else return -1; }
	virtual int GetFileIndex() { return -1; }
	virtual void SetFileIndex(int alX) {}

	/////////////////////////////
	// Drawing
	//virtual void AddToDrawingList();

	virtual void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions,iEditorEditMode* apEditMode,
						bool abIsSelected, const cColor& aHighlightCol=cColor(1,1), const cColor& aDisabledCol=cColor(0.5f,1));
	virtual void DrawProgram(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iGpuProgram* apProg, const cColor& aCol);

	virtual bool IsCulledByFrustum(cCamera* apCamera);
	bool IsCulledByClipPlanes() { return mbCulledByPlane; }
	virtual void CheckCulledByClipPlanes(tEditorClipPlaneVec& avPlanes);
	virtual bool EntitySpecificCheckCulled(cEditorClipPlane* apPlane);

	virtual cBoundingVolume* GetRenderBV();


	///////////////////////////
	// Compound object stuff
	virtual void SetCompoundObject(cEntityWrapperCompoundObject* apObject, bool abDetachFromOld=true);
	cEntityWrapperCompoundObject* GetCompoundObject() { return mpCompoundObject; }
	bool BelongsToCompoundObject() { return mpCompoundObject!=NULL; }

	int GetCompoundObjectID();

	///////////////////////////
	// Icon management
	cEntityIcon* GetIcon() { return mpIcon; }
	virtual cEntityIcon* CreateIcon() { return NULL; }

	///////////////////////////
	// Engine entity management
	iEngineEntity* GetEngineEntity() { return mpEngineEntity; }

	virtual bool CreateEngineEntity();
	virtual iEngineEntity* CreateSpecificEngineEntity() { return NULL; }

	/////////////////////////////////////////////////////////////
	// Property interface
	virtual bool GetProperty(int, int&);
	virtual bool GetProperty(int, float&);
	virtual bool GetProperty(int, bool&);
	virtual bool GetProperty(int, tString&);
	virtual bool GetProperty(int, cVector2f&);
	virtual bool GetProperty(int, cVector3f&);
	virtual bool GetProperty(int, cColor&);

	virtual bool SetProperty(int, const int&);
	virtual bool SetProperty(int, const float&);
	virtual bool SetProperty(int, const bool&);
	virtual bool SetProperty(int, const tString&);
	virtual bool SetProperty(int, const cVector2f&);
	virtual bool SetProperty(int, const cVector3f&);
	virtual bool SetProperty(int, const cColor&);

	/////////////////////////////////////////////////////////////
	// Modifier action creation
    virtual iEditorAction* CreateSetPropertyActionInt(int alPropID, int alX);
	virtual iEditorAction* CreateSetPropertyActionFloat(int alPropID, float afX);
	virtual iEditorAction* CreateSetPropertyActionBool(int alPropID, bool abX);
	virtual iEditorAction* CreateSetPropertyActionString(int alPropID, const tString& asX);
	virtual iEditorAction* CreateSetPropertyActionVector2f(int alPropID, const cVector2f& avX);
	virtual iEditorAction* CreateSetPropertyActionVector3f(int alPropID, const cVector3f& avX);
	virtual iEditorAction* CreateSetPropertyActionColor(int alPropID, const cColor& aX);

	//////////////////////////////////////////////////////////
	// Extension Data
	void SetEntityExtData(void* apData) { mpExtData = apData; }
	void* GetEntityExtData() { return mpExtData; }

	virtual void OnDestroy() {}
protected:
	iEditorAction* SetUpAction(iEditorAction* apAction);

	virtual void OnAddToWorld() {}
	virtual void OnSetActive(bool abX);
	virtual void OnSetVisible(bool abX);
	virtual void OnSetCulled(bool abX);
	virtual void OnSetSelected(bool abX) {}
	///////////////////////////
	// Helper
	/**
	 * Draws the texture in apGfx in a billboard style, ie always facing to the camera
	 * \param *apGfx 
	 * \param avWorldPosition 
	 * \param avSize 
	 * \param aColor 
	 * \param apViewport 
	 * \param apFunctions 
	 */
	void DrawBillboard(iTexture *apGfx, const cVector3f& avWorldPosition,const cVector2f& avSize,const cColor& aColor,  
						cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions);

	
	/**
	 * Draws an arrow pointing upwards using the amtxTransform reference system
	 * \param apViewport 
	 * \param apFunctions 
	 * \param amtxTransform 
	 * \param afLength 
	 * \param abKeepConstantSize 
	 * \param avHeadSizeRatio 
	 * \param aCol 
	 * \param afOrthoConstant 
	 * \param afPerspConstant 
	 */
	void DrawArrow(cEditorWindowViewport* apViewport, 
				   cRendererCallbackFunctions* apFunctions, 
				   const cMatrixf& amtxTransform, 
				   float afLength, 
				   bool abKeepConstantSize, 
				   const cVector2f& avHeadSizeRatio, 
				   const cColor& aCol, 
				   float afOrthoConstant=0.1f, float afPerspConstant=4);


	///////////////////////////
	// Data
	iEntityWrapperType* mpType;
	iEntityWrapperData* mpData;

	int mlID;
	tString msName;

	tString msFilename;
	
	cEntityIcon*	mpIcon;
	iEngineEntity*	mpEngineEntity;

	bool mbAllowNullEngineEntity;

	//Hierarchy
	iEntityWrapper* mpParent;
	tEntityWrapperList mlstChildren;

	bool mbEntityUpdated;
	
	bool mbTranslationUpdated;
	bool mbRotationUpdated;
	bool mbScaleUpdated;

	bool mbActive;
	bool mbVisible;
	bool mbCulledByPlane;

	bool mbSelected;

	int mlLastTransformedAxis;
	cVector3f mvPosition;
	cVector3f mvRotation;
	cVector3f mvScale;

	cMatrixf mmtxTranslate;
	cMatrixf mmtxRotate;
	cMatrixf mmtxScale;

	cMatrixf mmtxTransform;

	// User Data
	void* mpUserData;
    // Editor Helper
	tString msTag;
	unsigned int mlGroupID;

	// Compound
	cEntityWrapperCompoundObject* mpCompoundObject;

	////////////////////
	// Arrow drawing
	static tVertexVec mvArrowQuads[4];

	////////////////////
	// Extension data
	void* mpExtData;
};

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// iEntityWrapperTypeUserDefinedEntity
//	Base for User Defined Entity Types. User Defined Entities hold a number of parameters that 
//	are set by the user, and read from a config file.
//
class iEntityWrapperTypeUserDefinedEntity : public iEntityWrapperType
{
public:
	iEntityWrapperTypeUserDefinedEntity(int, const tWString&, const tString&, cEditorUserClassSubType*, const tString& asUserVarElement="UserVariables");

	cEditorUserClassSubType* GetUserType() { return mpUserType; }
	const tString& GetUserVarElementName() { return msVarElementName; }

protected:
	cEditorUserClassSubType* mpUserType;
	tString msVarElementName;
};

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// iEntityWrapperDataUserDefinedEntity
//	Base for User Defined Entity Data objects. 
//
class iEntityWrapperDataUserDefinedEntity : public iEntityWrapperData
{
public:
	iEntityWrapperDataUserDefinedEntity(iEntityWrapperType* apType, int alDefID);
	virtual ~iEntityWrapperDataUserDefinedEntity();

	virtual void CopyToEntity(iEntityWrapper* apEntity, int alX);
	virtual void CopyFromEntity(iEntityWrapper* apEntity);
	virtual bool Load(cXmlElement* apElement);

	bool SaveSpecific(cXmlElement* apElement);

protected:
	cEditorUserClassDefinitionManager* mpDefMgr;
	cEditorUserClassDefinition* mpDef;

	tString msType;
	tString msSubType;
	cEditorClassInstance* mpClass;
};

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// iEntityWrapperUserDefinedEntity
//	Base for User Defined Entity Objects. These hold an instance of the defined type (values)
//	(New) Can change user type on the fly
class iEntityWrapperUserDefinedEntity : public iEntityWrapper
{
public:
	iEntityWrapperUserDefinedEntity(iEntityWrapperData*);
	virtual ~iEntityWrapperUserDefinedEntity();

	cEditorClassInstance* GetClass() { return mpClass; }
	void SetClass(cEditorClassInstance*);

	cEditorVarInstance* GetVar(const tWString&);
	void SetVar(const tWString&, const tWString&);

	virtual iEditorAction* CreateActionSetUserType();
	virtual iEditorAction* CreateActionSetUserVariable(const tWString&, const tWString&);

protected:
	virtual void OnSetVar(const tWString& asName, const tWString& asValue)=0;

	cEditorClassInstance* mpClass;
};

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// iEntityWrapperTypeAggregate
//	Base for Aggregate Types. Configures some values useful to manage Aggregates
//
class iEntityWrapperTypeAggregate : public iEntityWrapperType
{
public:
	iEntityWrapperTypeAggregate(int alType, const tWString& asName, 
									 const tString& asElementName, const tString& asComponentElementName,
									 bool abDestroyComponentsOnDeletion=true);

	const tString& GetComponentElementName() { return msComponentElementName; }

	bool GetDestroyComponentsOnDeletion() { return mbDestroyComponentsOnDeletion; }

protected:
	tString msComponentElementName;
	bool mbDestroyComponentsOnDeletion;
};

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// iEntityWrapperDataAggregate
//	Base for Aggregate Data objects. Hold data for component entities
//
class iEntityWrapperDataAggregate : public iEntityWrapperData
{
public:
	iEntityWrapperDataAggregate(iEntityWrapperType* apType);
	~iEntityWrapperDataAggregate();

	void CopyFromEntity(iEntityWrapper*);
	void CopyToEntity(iEntityWrapper*, int);
	
	bool Load(cXmlElement*);
	bool SaveSpecific(cXmlElement*);

	void SetComponentsData(const tEntityDataVec& avData) { mvComponentsData = avData; }
	tEntityDataVec& GetComponentsData() { return mvComponentsData; }

protected:

	tEntityDataVec mvComponentsData;
};

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
// iEntityWrapperAggregate
//	Base for Aggregate objects. These group a number of entities and treat them as
//	they were one.
//
class iEntityWrapperAggregate : public iEntityWrapper
{
public:
	iEntityWrapperAggregate(iEntityWrapperData* apData);
	~iEntityWrapperAggregate();

	void SetUpCloneData(iEntityWrapperData*, const tIntVec&);
	tIntVec GetCloneIDs();

	virtual bool IsCulledByFrustum(cCamera* apCamera);

	bool CheckRayIntersect(cEditorWindowViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT);
	bool Check2DBoxIntersect(cEditorWindowViewport* apViewport, const cRect2l& aBox, cVector3f* apPos);

	virtual void AddComponent(iEntityWrapper* apEntity);
	virtual void RemoveComponent(iEntityWrapper* apEntity);

	void UpdateRelativeTransforms();

	//void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected);
	//cEditorWindowEntityEditBox* CreateEditBox(cEditorEditModeSelect* apEditMode);

	virtual void UpdateEntity();

	void SetComponentsUpdated() { mbComponentsUpdated=true; }

	void SetAbsPosition(const cVector3f& avPosition);
	void SetAbsRotation(const cVector3f& avRotation);
	void SetAbsScale(const cVector3f& avScale, int alAxis=-1);

	const tEntityWrapperList& GetComponents() { return mlstComponents; }

	void OnDestroy();

	bool CanTranslate() { return mbCanTranslate; }
	bool CanRotate() { return mbCanRotate; }
	bool CanScale() { return mScaleType!=eScaleType_None; }
	eScaleType GetScaleType() { return mScaleType; }
	bool IsCloneable() { return mbCloneable; }
	bool IsDeletable() { return mbDeletable; }

protected:
	virtual void OnAddComponent(iEntityWrapper*) {}
	virtual void OnRemoveComponent(iEntityWrapper*) {}
	virtual void OnUpdateRelativeTransform(iEntityWrapper*, const cMatrixf&) {}

	bool mbComponentsUpdated;
	tEntityWrapperList mlstComponents;
	tMatrixfList mlstRelMatrices;

	bool mbCanTranslate;
	bool mbCanRotate;
	eScaleType mScaleType;
	bool mbCloneable;
	bool mbDeletable;

};

//-----------------------------------------------------------------------


#endif //HPLEDITOR_ENTITY_WRAPPER
