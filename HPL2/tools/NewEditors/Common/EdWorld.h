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

#ifndef ED_WORLD_H
#define ED_WORLD_H

//-----------------------------------------------------------------------

#include "../Common/EdModule.h"

using namespace std;
using namespace hpl;

//-----------------------------------------------------------------------

class iEditor;
class iEdWorld;

class iProp;
class iPropVal;

class iEdObjectType;
class iEdObjectData;
class iEdObject;
class iEdObjExtension;

class iEdAction;

class iEdEditPane;

//-----------------------------------------------------------------------

typedef std::vector<iEdObjectType*> tEdObjTypeVec;

typedef std::map<tWString, iEdObjectType*>	tEdObjTypeMap;
typedef tEdObjTypeMap::iterator			tEdObjTypeMapIt;

typedef std::map<int, int>			tIDRedirectMap;
typedef tIDRedirectMap::iterator	tIDRedirectMapIt;

typedef std::map<tWString, int>		tNameCountMap;
typedef tNameCountMap::iterator		tNameCountMapIt;

//-------------------------------------------------------------

typedef std::list<iEdObject*> tEdObjectList;
typedef tEdObjectList::iterator	tEdObjectListIt;

typedef std::map<int, iEdObject*>		tEdObjectMap;
typedef tEdObjectMap::iterator			tEdObjectMapIt;
typedef tEdObjectMap::const_iterator	tEdObjectMapConstIt;

typedef std::map<tWString, iEdObject*> tEdObjectNameMap;
typedef tEdObjectNameMap::iterator tEdObjectNameMapIt;

//-----------------------------------------------------------------------

enum eObjInt
{
	eObjInt_ID,
	eObjInt_LastEnum
};

enum eObjStr
{
	eObjStr_Name,
	eObjStr_LastEnum
};

enum eObjBool
{
	eObjBool_LastEnum
};

enum eObjFloat
{
	eObjFloat_LastEnum
};

enum eObjVec2f
{
	eObjVec2f_LastEnum
};

enum eObjVec3f
{
	eObjVec3f_LastEnum
};

enum eObjCol
{
	eObjCol_LastEnum
};

//-----------------------------------------------------------------------

class cIndexedFile;

//-----------------------------------------------------------------------

typedef std::list<cIndexedFile*> tIndexedFileList;
typedef tIndexedFileList::iterator tIndexedFileListIt;

//-----------------------------------------------------------------------

class cFileIndex
{
	friend class cIndexedFile;
public:
	cFileIndex(iEdWorld*, const tString&);

	cIndexedFile* CreateIndexedFile(const tString&);
	cIndexedFile* CreateIndexedFileFromElement(cXmlElement*);
	cIndexedFile* GetIndexedFile(const tString&);
	cIndexedFile* GetIndexedFile(int, bool abSkipRedirect=false);

	void Load(cXmlElement*);
	void Save(cXmlElement*);

	void Import(cXmlElement*);
	void PostImport();

protected:
	void CleanUpFiles();

	iEdWorld* mpWorld;
	tString msName;
	tIndexedFileList	mlstFiles;
	int mlIDCounter;

	tIDRedirectMap mmapRedirector;
};

//-----------------------------------------------------------------------

typedef std::map<tString, cFileIndex*>		tFileIndexMap;
typedef tFileIndexMap::iterator				tIndexedFileMapIt;

//-----------------------------------------------------------------------

class cIndexedFile
{
	friend class cFileIndex;
public:
	cIndexedFile(cFileIndex*, const tString&);
	cIndexedFile(cFileIndex*, cXmlElement*);

	void Assign() { ++mlRefCount; }
	void Release() { if(mlRefCount>0) --mlRefCount; }

	void Load(cXmlElement*);
	void Save(cXmlElement*);

	static tString GetIndexedFilePathFromNormalPath(const tString&);

	cFileIndex* mpIndex;
	int mlID;
	tString msRelativePath;
	int mlRefCount;
};

//-----------------------------------------------------------------------

/**
 * \ingroup Editors
 *
 *
 *
 */
class cPropIdentifier
{
public:
	int mlTypeID;
	int mlVarType;
	int mlPropID;
};

//-----------------------------------------------------------------------

////////////////////////////////////
// iProp
//  Interface for object properties. Used to automate object creation and saving,
//  also helps in creating actions that modify these.
//	Each property has an ID number, a type, a name and a bool that says if it should be saved.
//	Derived classes should also store default values
//
class iProp
{
public:
	iProp(iEdObjectType* apOwnerType, int alVarType, int alID, 
			const tString& asName, bool abSave) : mpOwnerType(apOwnerType), mlVarType(alVarType), mlID(alID), msName(asName), mbSave(abSave)
	{}

	void SetSaved(bool abX) { mbSave = abX; }

	iEdObjectType* GetOwnerType() { return mpOwnerType; }
	int GetID() { return mlID; }
	int		GetVarType() { return mlVarType; }
	tString	GetVarTypeName();
	const tString&	GetName() { return msName; }
	bool IsSaved() { return mbSave; }

	/**
	 * Creates a specific value. Each derived class must implement this method.
	 * \return 
	 */
	virtual iPropVal* CreateValue()=0;

protected:
	iEdObjectType* mpOwnerType;
	int mlVarType;
	int mlID;
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
	 * Gets property value from an Object
	 * \param iEdObject* 
	 */
	virtual void GetFromObject(iEdObject*)=0;
	/**
	 * Sets property value to an Object
	 * \param iEdObject* 
	 */
	virtual void SetToObject(iEdObject*)=0;


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

protected:
	/**
	 * Type specific saving routines
	 * \param cXmlElement* 
	 */
	virtual void SaveSpecific(cXmlElement*)=0;

	iProp* mpProp;
};

//-----------------------------------------------------------------------

typedef std::list<iProp*> tPropList;
typedef tPropList::iterator tPropListIt;

typedef std::vector<iProp*> tPropVec;
typedef tPropVec::iterator	tPropVecIt;

typedef std::list<iPropVal*> tPropValList;
typedef tPropValList::iterator tPropValListIt;

typedef std::vector<iPropVal*>	tPropValVec;
typedef tPropValVec::iterator	tPropValVecIt;

//-----------------------------------------------------------------------

////////////////////////////////////
// cPropInt
//  Represents an integer property
//
class cPropInt : public iProp
{
public:
	cPropInt(iEdObjectType* apType, int alID, const tString& asName, int alX, bool abSave=true) : iProp(apType, eVariableType_Int, alID, asName, abSave), mlVal(alX)
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

	void GetFromObject(iEdObject*);
	void SetToObject(iEdObject*);

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
	cPropFloat(iEdObjectType* apType, int alID, const tString& asName, float afX, bool abSave=true) : iProp(apType, eVariableType_Float, alID, asName, abSave), mfVal(afX)
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

	void GetFromObject(iEdObject*);
	void SetToObject(iEdObject*);

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
	cPropBool(iEdObjectType* apType, int alID, const tString& asName, bool abX, bool abSave=true) : iProp(apType, eVariableType_Bool, alID, asName, abSave), mbVal(abX)
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

	void GetFromObject(iEdObject*);
	void SetToObject(iEdObject*);

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
	cPropStr(iEdObjectType* apType, int alID, const tString& asName, const tString& asX, bool abSave=true) : iProp(apType, eVariableType_String, alID, asName, abSave), msVal(asX)
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

	void GetFromObject(iEdObject*);
	void SetToObject(iEdObject*);

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
	cPropVec2f(iEdObjectType* apType, int alID, const tString& asName, const cVector2f& avX, bool abSave=true) : iProp(apType, eVariableType_Vec2, alID, asName, abSave), mvVal(avX)
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

	void GetFromObject(iEdObject*);
	void SetToObject(iEdObject*);

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
	cPropVec3f(iEdObjectType* apType, int alID, const tString& asName, const cVector3f& avX, bool abSave=true) : iProp(apType, eVariableType_Vec3, alID, asName, abSave), mvVal(avX)
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

	void GetFromObject(iEdObject*);
	void SetToObject(iEdObject*);

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
	cPropCol(iEdObjectType* apType, int alID, const tString& asName, const cColor& aX, bool abSave=true) : iProp(apType, eVariableType_Color, alID, asName, abSave), mVal(aX)
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

	void GetFromObject(iEdObject*);
	void SetToObject(iEdObject*);

	void Load(cXmlElement*);
	void SaveSpecific(cXmlElement*);

	cColor& Get() { return mVal; }
	void Set(const cColor& aX) { mVal = aX; }

private:
	cColor mVal;
};

//-----------------------------------------------------------------------

enum ePropStep
{
	ePropStep_PreCreate,
	ePropStep_PostCreate,
	ePropStep_PostDeployAll,

	ePropStep_LastEnum,
};

//-----------------------------------------------------------------------

/**
 * \ingroup ParticleEditor
 *
 *
 *
 * \version 1.0
 * first version
 *
 * \date 10-27-2010
 *
 * \author Luis
 *
 */
class iEdObjectType
{
	friend class iEdWorld;
	friend class iEdObjectData;
public:
	iEdObjectType(const tWString&, const tString&);
	virtual ~iEdObjectType();

	iEdWorld* GetWorld() { return mpWorld; }

	virtual bool Init() { return true; }
	virtual bool PostCreateSetUp();

	/**
	 * Returns the type's ID
	 */
	int GetTopID() { return mvID.front(); }
	const tIntVec& GetID() { return mvID; }
	/**
	 * Returns the type name
	 */
	const tWString& GetName() { return msName; }
	/**
	 * Returns the type's full name, in the form "supertype1name.supertype2name. ... supertypeNname.typename" 
	 */
	tWString GetFullName();

	/**
	 *
	 * \return 
	 */
	const tString& GetXmlElementName() { return msXmlElementName; }

	/**
	 *	Returns true if the current type or one of its supertypes matches the criteria.
	 */
	bool IsA(iEdObjectType*);
	bool IsA(int);
	bool IsA(const tWString&);

	/**
	 *
	 * \param iEdObjectType* 
	 * \return Most restrictive type between calling type and argument type.
	 */
	iEdObjectType* GetMostRestrictiveType(iEdObjectType*);

	/**
	 *	Gets the type matching the name given.
	 *  This should spread through the whole type hierarchy until the type is found.
	 */
	virtual iEdObjectType* GetTypeByName(const tWString&);
	/**
	 *	Gets the type matching the ID given. 
	 *  This should spread through the whole type hierarchy until the type is found.
	 */
	virtual iEdObjectType* GetTypeByID(const tIntVec&);
	/**
	 *	Gets the most appropriate type to load the given XML element. 
	 *  This should spread through the whole type hierarchy until the type is found.
	 */
	virtual iEdObjectType* GetTypeByXmlElement(cXmlElement*);

	iEdObjectType* GetMainType();

	void LogTypeHierarchyHelper(tWString&, int);

	/**
	 *
	 */
	bool AddChildType(iEdObjectType*);

	int GetNumChildren();
	iEdObjectType* GetChild(int);
	const tEdObjTypeVec& GetChildren();
	/**
	 *
	 * \return 
	 */
	virtual iEdObjectData* CreateData();
	virtual bool SetUpCreationData(iEdObjectData*) { return true; }

	virtual void AddObject(iEdObject*);
	virtual void RemoveObject(iEdObject*);

	int GetObjectNum();
	tEdObjectList GetObjects();
	void GetObjectsHelper(tEdObjectList&);

	void SetChildObjectsUpdated();

	tIntList GetObjectIDs();

	void ClearObjects();

	///////////////////////////////////////////////////
	// Restrictions for objects of this type
	bool IsRenameable()		{ return mbRenameable; }
	bool IsDeletable()		{ return mbDeletable; }
	bool IsCloneable()		{ return mbCloneable; }


	// Helpers for getting properties
	iProp* GetPropByTypeAndID(int, int);
	iProp* GetPropByName(const tString&);

	bool HasProp(const tString&);

protected:
	/////////////////////////////////////////////////
	// Interface for adding properties to a type / parameters yet to be decided
	/*
	void AddTexture();
	void AddMaterial();
	void AddSound();
	void AddMesh();
	*/
	/////////////////////////////////////////////////
	// Used to set up properties in a type
	void AddInt		(int, const tString&, int alDefault=-1,					ePropStep aOrder=ePropStep_PostCreate, bool abSave=true);
	void AddFloat	(int, const tString&, float afDefault=0.0f,				ePropStep aOrder=ePropStep_PostCreate, bool abSave=true);
	void AddBool	(int, const tString&, bool abDefault=true,				ePropStep aOrder=ePropStep_PostCreate, bool abSave=true);
	void AddString	(int, const tString&, const tString& asDefault="",		ePropStep aOrder=ePropStep_PostCreate, bool abSave=true);
	void AddVec2f	(int, const tString&, const cVector2f& avDefault=0,		ePropStep aOrder=ePropStep_PostCreate, bool abSave=true);
	void AddVec3f	(int, const tString&, const cVector3f& avDefault=0,		ePropStep aOrder=ePropStep_PostCreate, bool abSave=true);
	void AddColor	(int, const tString&, const cColor& aDefault=cColor(1), ePropStep aOrder=ePropStep_PostCreate, bool abSave=true);


	/**
	 *
	 * \return 
	 */
	virtual iEdObjectData* CreateTypeSpecificData()=0; 


	////////////////////////////////////////////
	// Data
	iEdWorld* mpWorld;
	int mlID;
	tIntVec mvID;
	tWString msName;
	tWString msFullName;
	tString msXmlElementName;

	iEdObjectType* mpParentType;
	tEdObjTypeVec mvChildTypes;
	tEdObjTypeMap mmapChildTypes;

	// Properties
	std::vector<tPropVec> mvProperties;

	bool mbRenameable;
	bool mbDeletable;
	bool mbCloneable;

private:
	void SetWorld(iEdWorld* apWorld) { mpWorld = apWorld; }
	void SetParent(iEdObjectType*, int);

	void AddProperty(iProp*, ePropStep);

	/**
	 * Sets Data object to default values
	 * \param iEdObjectData* 
	 */
	void CreateValuesInData(iEdObjectData*);

	bool mbClearingObjects;
	tEdObjectList mlstObjects;
};

//-------------------------------------------------------------

class iEdObjectData
{
public:
	iEdObjectData(iEdObjectType*, bool abSaveID=true, bool abSaveName=true);
	virtual ~iEdObjectData();

	iEdObjectType* GetType() { return mpType; }

	iEdObjExtension* GetExtension() { return mpExtension; }

	virtual void CopyFromObject(iEdObject*);
	virtual void CopyToObject(iEdObject*, ePropStep);

	virtual bool Load(cXmlElement*);
	virtual bool Save(cXmlElement*);

	virtual iEdObject* CreateObject();

	void SetID(int alX)	{ mlID = alX; }
	int GetID()			{ return mlID; }

	void SetName(const tWString& asX)	{ msName = asX; }
	const tWString& GetName()			{ return msName; }

	/////////////////////////////////////////////////////
	// Generic property handlers
	void AddValue(ePropStep aOrder, iPropVal* apVal);

	iProp* GetProperty(int, int);
	iPropVal* GetValue(int, int);

	void SetInt(int, int);
	void SetFloat(int, float);
	void SetBool(int, bool);
	void SetString(int, const tString&);
	void SetColor(int, const cColor&);
	void SetVec2f(int, const cVector2f&);
	void SetVec3f(int, const cVector3f&);

	int		GetInt(int);
	float	GetFloat(int);
	bool	GetBool(int);
	const tString&	GetString(int);
	const cColor&	GetColor(int);
	const cVector2f& GetVec2f(int);
	const cVector3f& GetVec3f(int);

protected:
	virtual bool LoadSpecificData(cXmlElement*)=0;
	virtual bool SaveSpecificData(cXmlElement*)=0;

	virtual iEdObject* CreateTypeSpecificObject()=0;

	///////////////////////////////////////////////////
	// Data;
	iEdObjectType* mpType;

	bool mbSaveID;
	int mlID;

	bool mbSaveName;
	tWString msName;

	std::vector<tPropValVec> mvValues;

	iEdObjExtension* mpExtension;
};

//-------------------------------------------------------------

typedef std::list<iEdObjectData*>	tEdObjDataList;
typedef tEdObjDataList::iterator	tEdObjDataListIt;

typedef std::vector<iEdObjectData*> tEdObjDataVec;

//-------------------------------------------------------------

class iEdObject
{
	friend class iEdWorld;
public:
	iEdObject(iEdObjectData*);
	virtual ~iEdObject();

	iEdObjectData*	GetCreatorData(){ return mpData; }
	iEdObjExtension* GetExtension() { return mpExtension; }

	iEdObjectType*	GetType()		{ return mpType; }
	int				GetTypeID()		{ return mpType->GetTopID(); }
	const tWString& GetTypeName()	{ return mpType->GetName(); }
	iEdObjectType*	GetMainType()	{ return mpType->GetMainType(); }

	bool IsA(int alTypeID)			{ return mpType->IsA(alTypeID); }
	bool IsA(iEdObjectType* apType) { return mpType->IsA(apType); }
	bool IsA(const tWString& asName){ return mpType->IsA(asName); }

	iEdWorld*		GetWorld()		{ return mpType->GetWorld(); }

	virtual bool Create();
	virtual void PostDeployAll(bool);

	virtual bool Update(bool abForce=false);

	int GetID() { return mlID; }
	const tWString& GetName() { return msName; }

	void SetID(int alX)					{ mlID = alX; }
	void SetName(const tWString& asX)	{ msName = asX; }

	bool IsRenameable()				{ return mpType->IsRenameable(); }
	bool IsCloneable()				{ return mpType->IsCloneable(); }
	bool IsDeletable()				{ return mpType->IsDeletable(); }

	void SetExtension(iEdObjExtension*);

	/////////////////////////////////////////////////////////////
	// Clone
	iEdObjectData* CreateCopyData();
	virtual tIntVec GetCloneIDs();
	virtual iEdObject* Clone(const tIntVec& avIDs, bool abCallPostDeploy=true, bool abDestroyData=true, 
								const tWString& asNamePrefix=_W(""), const tWString& asNameSuffix=_W(""));
	virtual void SetUpCloneData(iEdObjectData*, const tIntVec&) {}


	virtual bool Save(cXmlElement*);

	/////////////////////////////////////////////////////////////
	// Property management
	bool HasProperty(const tString&);

	virtual bool SetProperty(int, const int)				{ return false; }
	virtual bool SetProperty(int, const float)				{ return false; }
	virtual bool SetProperty(int, const bool)				{ return false; }			
	virtual bool SetProperty(int, const tString&)			{ return false; }
	virtual bool SetProperty(int, const cColor&)			{ return false; }
	virtual bool SetProperty(int, const cVector2f&)			{ return false; }
	virtual bool SetProperty(int, const cVector3f&)			{ return false; }

	virtual bool GetProperty(int, int&)						{ return false; }
	virtual bool GetProperty(int, float&)					{ return false; }
	virtual bool GetProperty(int, bool&)					{ return false; }
	virtual bool GetProperty(int, tString&)					{ return false; }
	virtual bool GetProperty(int, cColor&)					{ return false; }
	virtual bool GetProperty(int, cVector2f&)				{ return false; }
	virtual bool GetProperty(int, cVector3f&)				{ return false; }

	void SetUpdated(bool abX=true);

	/////////////////////////////////////////////////////////////
	// Modifier action creation
    virtual iEdAction* CreateSetIntAction(int, int);
	virtual iEdAction* CreateSetFloatAction(int, float);
	virtual iEdAction* CreateSetBoolAction(int, bool);
	virtual iEdAction* CreateSetStringAction(int , const tString&);
	virtual iEdAction* CreateSetVector2fAction(int, const cVector2f&);
	virtual iEdAction* CreateSetVector3fAction(int, const cVector3f&);
	virtual iEdAction* CreateSetColorAction(int, const cColor&);

	virtual iEdEditPane* CreateEditPane()=0;

protected:
	virtual bool OnCreate()=0;
	virtual void OnPostDeployAll(bool)=0;
	virtual bool OnUpdate(bool)=0;
	virtual void OnDestroy()=0;

	virtual void OnDraw(const cModuleDrawData&) {}

	virtual iEdAction* SetUpAction(iEdAction*);

	iEdObjectType*		mpType;
	iEdObjectData*	mpData;

	int mlID;
	tWString msName;

	bool mbUpdated;

	iEdObjExtension* mpExtension;
};

//-------------------------------------------------------------

/**
 *
 */
class iEdSessionData
{
public:
	iEdSessionData(iEdWorld* apWorld) : mpWorld(apWorld)
	{}

	virtual bool Save(cXmlElement*) { return true; }
	virtual bool Load(cXmlElement*) { return true; }
protected:
	virtual void OnLoad(cXmlElement*) {}

	iEdWorld* mpWorld;
};

/**
 *
 */
class iEdObjExtension
{
public:
	iEdObjExtension(iEdWorld*, iEdObject*);
	virtual ~iEdObjExtension();

	virtual bool Save(cXmlElement*)=0;
	virtual bool Load(cXmlElement*)=0;

	iEdObjExtension* CreateCopy(iEdObject*);
	virtual void OnPostDeployAll()=0;

protected:
	virtual void Copy(iEdObjExtension*)=0;

	static iEdWorld* mpWorld;
	iEdObject* mpObject;
};

/**
 *
 *
 *
 * \version 1.0
 * first version
 *
 * \date 10-26-2010
 *
 * \author Luis
 *
 */
class iEdWorld : public iEdModule
{
public:
	iEdWorld(iEditor* apEditor, const tWString& asName, const tWString& asFileExt, const tString& asXmlElementName, iEdObjectType* apRootType);
	virtual ~iEdWorld();

	virtual void Compile();

	void SetBGColor(const cColor& aCol);
	const cColor& GetBGColor() { return mBGColor; }
	void SetAmbientLightEnabled(bool);
	void SetPointLightEnabled(bool);

	bool GetAmbientLightEnabled();
	bool GetPointLightEnabled();

	/**
	 * Adds a type to the World, as a child of the root type
	 * \param iEdObjectType* 
	 */
	bool AddType(iEdObjectType*);

	/**
	 *
	 * \return 
	 */
	iEdObjectType* GetRootType() { return mpRootType; }

	/**
	 *
	 * \param tWString& 
	 * \return 
	 */
	iEdObjectType* GetTypeByName(const tWString&);
	/**
	 *
	 * \param int 
	 * \return 
	 */
	iEdObjectType* GetTypeByID(const tIntVec&);
	/**
	 *
	 * \param cXmlElement* 
	 * \return 
	 */
	iEdObjectType* GetTypeByXmlElement(cXmlElement*);

	void LogTypeHierarchy();

	/**
	 *
	 * \param iEdObjectType* 
	 * \param apParent 
	 * \return 
	 */
	bool IsIDAvailable(int);
	bool IsNameAvailable(const tWString&);

	tWString GenerateValidName(const tWString&, bool);

	cWorld* GetEngWorld() { return mpWorld; }
	cWorld* GetTempEngWorld() { return mpTempWorld; }

	/**
	 *
	 * \param iEdObjectData* 
	 * \return 
	 */
	iEdObject* CreateObjFromData(iEdObjectData*);
	/**
	 *
	 * \param cXmlElement* 
	 * \return 
	 */
	iEdObject* CreateObjFromXmlElement(cXmlElement*);

	/**
	 *
	 * \param iEdObject* 
	 */
	void DestroyObj(iEdObject*);

	bool AddObject(iEdObject*);
	bool RemoveObject(iEdObject*);
	void ClearObjects();

	bool IsClearing() { return mbClearing; }


	void CreateLoadWorldDialog();
	void CreateSaveWorldDialog();

	/**
	 *
	 * \param tWString& 
	 * \return 
	 */
	virtual bool LoadFromFile(const tWString&);
	/**
	 *
	 * \param tWString& 
	 * \return 
	 */
	virtual bool SaveToFile(const tWString&);
	virtual bool Save();

	const tWString& GetCurrentFilename() { return msFilename; }

	virtual void AddRecentFile(const tWString&);
	virtual void RemRecentFile(const tWString&);
	const tWStringList& GetRecentFiles();

	void SetFileToLoadOnUpdate(const tWString& asX) { msFileToLoadOnUpdate = asX; }

	bool LoadFileIndices(cXmlElement*);
	bool SaveFileIndices(cXmlElement*);

	/**
	 * Modification tracking
	 */
	int Modify();
	bool Unmodify(int);

	bool IsModified();

	/**
	 *
	 * \param alID 
	 * \return 
	 */
	iEdObject* GetObject(int);
	iEdObject* GetObject(const tWString&);

	const tEdObjectMap& GetObjects() { return mmapObjects; }

	bool HasObject(int);
	bool HasObject(const tWString&);
	bool HasObject(iEdObject*);

	void AddUpdatedObject(iEdObject*);
	void RemUpdatedObject(iEdObject*);

	cFileIndex* CreateFileIndex(const tString& asName);
	cFileIndex* GetFileIndex(const tString& asName);

	//cIndexedFile* CreateIndexedFile(const tString& asIndexName, const tString& asFilename);
	//cIndexedFile* GetIndexedFile(const tString& asIndexName, int alIdx);

	int GetFreeID();

	
	////////////////////////////////////////////////////////////////////////
	// Object management actions
	virtual iEdAction* CreateActionCreateObject(iEdObjectData*);
	virtual iEdAction* CreateActionDeleteObjects(const tEdObjectList&);
	iEdAction* CreateActionDeleteObject(iEdObject*);

	virtual iEdAction* CreateActionCloneObjects(const tEdObjectList&, const tWString& asPrefix=_W(""), const tWString& asSuffix=_W(""));
	iEdAction* CreateActionCloneObject(iEdObject*, const tWString& asPrefix=_W(""), const tWString& asSuffix=_W(""));

	iEdObject* GetLastAddedObject() { return mpLastAddedObject; }

	virtual iEdObjExtension* CreateObjExtension(iEdObject*) { return NULL; }

protected:
	bool LoadWorldCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(LoadWorldCallback);
	bool SaveWorldCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(SaveWorldCallback);

	virtual iEdSessionData* CreateSessionData() { return NULL; }

	virtual bool LoadContentsFromXmlDoc(iXmlDocument*);
	virtual bool SaveContentsToXmlDoc(iXmlDocument*);

	virtual void OnLoadLocalConfig(cConfigFile*);

	virtual void OnSaveLocalConfig(cConfigFile*);

	virtual void OnInit();
	virtual void OnReset();
	virtual void OnUpdate();

	virtual void OnDraw(const cModuleDrawData&);

	virtual void OnWorldLoad();
	virtual void OnWorldSave();
	virtual void OnWorldParamsChange();
	virtual void OnWorldObjModify();

	virtual void PostDeployObjects(bool);

	////////////////////////////////////////////////////
	// Data
	tWString msFileExt;
	tWString msFilename;
	tString msXmlValue;

	tWString msLastUsedPath;

	tWString msFileToLoadOnUpdate;

	tWStringVec mvDestFilenames;

	iEdSessionData* mpSessionData;

	cColor mBGColor;

	iEdObjectType* mpRootType;

	cWorld* mpWorld;
	cLightBox* mpAmbientLight;
	cLightPoint* mpPointLight;

	cWorld* mpTempWorld;

	bool mbClearing;

	int mlObjIDCounter;
	tEdObjectMap		mmapObjects;
	tEdObjectNameMap	mmapObjectNames;

	tIDRedirectMap		mmapIDRedirections;
	
	tNameCountMap		mmapNameCount;

	tEdObjectList		mlstUpdatedObjects;

	iEdObject*			mpLastAddedObject;

	int mlModificationStamp;
	int mlLastSavedModificationStamp;

	tWStringList mlstRecentFiles;

	tFileIndexMap mmapFileIndices;

private:
	void SetRootType(iEdObjectType*);
};

//-------------------------------------------------------------

#endif // ED_WORLD_H
