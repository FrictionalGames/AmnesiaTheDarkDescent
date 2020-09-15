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

#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include "../Common/EdWorld.h"

#include "../Common/EdUserClassDefinition.h"
#include "../Common/EdEditModeSelectTool.h"

//--------------------------------------------------------

class iEdScnObjType;
class iEdScnObjData;
class iEdScnObject;

class cUIPickFilter;

class cEdObjCompound;

class iScnObjManipulator;

class iEngineObject;
class cObjectIcon;

class iEdClass;
class cEdClassInstance;
class cEdUserClassDefinition;

class cEdVarInstance;

class cEdWindowViewport;

class iEdEditPane;

class cUIPickFilter;
class cUIPickMethod;

class cEdScnObjCategory;

class cEdSurfacePicker;

//--------------------------------------------------------

typedef std::vector<iEdScnObjType*> tScnObjTypeVec;

typedef std::list<iEdScnObject*> tScnObjList;
typedef tScnObjList::iterator	tScnObjListIt;

//--------------------------------------------------------

enum eScnVec3f
{
	eScnVec3f_Translation = eObjVec3f_LastEnum,
	eScnVec3f_Rotation,
	eScnVec3f_Scale,

	eScnVec3f_LastEnum
};

enum eScnBool
{
	eScnBool_Active = eObjBool_LastEnum,

	eScnBool_LastEnum
};

//--------------------------------------------------------

class iEdScnObjType : public iEdObjectType
{
public:
	iEdScnObjType(const tWString&, const tString&);

	bool Init();

	void SetCategory(cEdScnObjCategory*);
	cEdScnObjCategory* GetCategory();

	//////////////////////////////////////////////////
	// Visibility
	virtual bool IsVisible();
	virtual void SetVisible(bool);

	//////////////////////////////////////////////////
	// Enabled
	virtual bool IsEnabled();
	virtual void SetEnabled(bool);

	/**
	 * Gets a vector of types that can be attached to 
	 * entities of this type, if any.
	 * \return 
	 */
	const tScnObjTypeVec& GetAttachableTypes() { return mvAttachableTypes; }
	bool IsAttachable() { return mbAttachable; }

	///////////////////////////////////////////////////
	// Restrictions for objects of this type
	virtual bool AllowNullEngineObject()	{ return false; }
	virtual bool AllowNullIcon()			{ return true; }

	bool IsTranslateable() { return mbTranslateable; }
	bool IsRotateable()		{ return mbRotateable; }
	bool IsScalable()		{ return mbScalable; }

	virtual bool IsTranslateableGrouped() { return true; }
	virtual bool IsRotateableGrouped() { return true; }
	virtual bool IsScalableGrouped() { return true; }

	virtual void SetUpManipulators();
	virtual int GetNumManipulators();
	virtual iScnObjManipulator* GetManipulator(int);

	/**
	 * Helper for doing type specific creation stuff to Data
	 */
	virtual bool SetUpCreationData(iEdObjectData*) { return true; }

	
	/**
	 * Creates an edit pane for many objects
	 */
	virtual iEdEditPane* CreateGroupEditPane(iEdScnObject*);

	

	bool PostCreateSetUp();

protected:
	/**
	 * Register a type as attachable - should only be called in PostCreateSetUp
	 * \param tWString& Name of the type to declare as attachable
	 */
	void AddAttachableType(const tWString&);


	void AddManipulator(iScnObjManipulator*);
	////////////////////////////////////////////////////
	// Data
	bool mbVisible;
	bool mbEnabled;

	bool mbTranslateable;
	bool mbRotateable;
	bool mbScalable;

	tWStringList mlstAttachableTypeNames;
	tScnObjTypeVec mvAttachableTypes;
	bool mbAttachable;

	tManipulatorVec mvManipulators;

	cEdScnObjCategory* mpCategory;

	static tManipulatorMap mmapManipulatorPool;
};

//--------------------------------------------------------

class iEdScnObjData : public iEdObjectData
{
public:
	iEdScnObjData(iEdObjectType*);

	virtual void CopyFromObject(iEdObject*);
	virtual void CopyToObject(iEdObject*, ePropStep);

	bool mbSelected;

protected:
	virtual iEdObject* CreateTypeSpecificObject();

	virtual bool LoadSpecificData(cXmlElement*);
	virtual bool SaveSpecificData(cXmlElement*);

	///////////////////////////////////////////////////
	// Data
	int mlParentID;
	tIntList mlstChildIDs;
};

//--------------------------------------------------------

class iEdScnObject : public iEdObject
{
public:
	iEdScnObject(iEdObjectData* apData);
	virtual ~iEdScnObject();

	cEdScnObjCategory* GetCategory() { return static_cast<iEdScnObjType*>(mpType)->GetCategory(); }

	const tScnObjTypeVec& GetAttachableTypes() { return static_cast<iEdScnObjType*>(mpType)->GetAttachableTypes(); }
	bool IsAttachable() { return static_cast<iEdScnObjType*>(mpType)->IsAttachable(); }

	bool IsEnabled() { return static_cast<iEdScnObjType*>(mpType)->IsEnabled(); }
	bool IsVisible() { return static_cast<iEdScnObjType*>(mpType)->IsVisible(); }

	bool IsTranslateable() { return static_cast<iEdScnObjType*>(mpType)->IsTranslateable(); }
	bool IsRotateable() { return static_cast<iEdScnObjType*>(mpType)->IsRotateable(); }
	bool IsScalable() { return static_cast<iEdScnObjType*>(mpType)->IsScalable(); }

	bool IsCulledByClipPlanes() { return false; }

	virtual bool CheckPicked(cUIPickFilter*, cUIPickMethod*, iEdScnObject**);
	virtual bool PassesFilter(cUIPickFilter*);

	virtual bool CheckRayIntersect(iEdViewport*, cVector3f*, tVector3fVec*, float* apT=NULL);
	virtual bool Check2DBoxIntersect(iEdViewport*, const cRect2l& aBox, cVector3f*);

	virtual void SetTranslation(const cVector3f&);
	virtual void SetRotation(const cVector3f&);
	virtual void SetScale(const cVector3f&);

	void SetActive(bool abX) { mbActive = abX; }
	bool IsActive() { return mbActive; }

	void SetTransformUpdated() { mbTransformUpdated = true; }
	
	const cVector3f& GetTranslation()	{ return mvTranslation; }
	const cVector3f& GetRotation()		{ return mvRotation; }
	const cVector3f& GetScale()			{ return mvScale; }

	virtual void SetTranslationMatrix(const cMatrixf&);
	virtual void SetRotationMatrix(const cMatrixf&);
	virtual void SetScaleMatrix(const cMatrixf&);

	const cMatrixf&	GetTranslationMatrix(bool abForceUpdate=false);
	const cMatrixf& GetRotationMatrix(bool abForceUpdate=false);
	const cMatrixf& GetScaleMatrix(bool abForceUpdate=false);

	const cMatrixf& GetWorldMatrix(bool abForceUpdate=false);
	virtual void SetWorldMatrix(const cMatrixf&);

	cMatrixf GetRelativeTranslationMatrix(const cMatrixf&);
	cMatrixf GetRelativeRotationMatrix(const cMatrixf&);
	cMatrixf GetRelativeScaleMatrix(const cMatrixf&);

	cMatrixf GetRelativeWorldMatrix(const cMatrixf&);

	//////////////////////////////////////////////////
	// Compounds
	void SetCompoundObject(cEdObjCompound*, bool abDetachFromOld=true);
	cEdObjCompound* GetCompoundObject() { return mpCompound; }

	virtual bool IsAffectedByDecal(cEdSurfacePicker* apPicker);

	/////////////////////////////////////////////////////////////
	// Property get/set interface
	virtual bool SetProperty(int, const int)			{ return false; }
	virtual bool SetProperty(int, const float)			{ return false; }
	virtual bool SetProperty(int, const bool);
	virtual bool SetProperty(int, const tString&)		{ return false; }
	virtual bool SetProperty(int, const cColor&)		{ return false; }
	virtual bool SetProperty(int, const cVector2f&)		{ return false; }
	virtual bool SetProperty(int, const cVector3f&);

	virtual bool GetProperty(int, int&)					{ return false; }
	virtual bool GetProperty(int, float&)				{ return false; }
	virtual bool GetProperty(int, bool&);
	virtual bool GetProperty(int, tString&)				{ return false; }
	virtual bool GetProperty(int, cColor&)				{ return false; }
	virtual bool GetProperty(int, cVector2f&)			{ return false; }
	virtual bool GetProperty(int, cVector3f&);


	virtual iEdAction* CreateSetTranslationMatrixAction(const cMatrixf&);
	virtual iEdAction* CreateSetRotationMatrixAction(const cMatrixf&);
	virtual iEdAction* CreateSetScaleMatrixAction(const cMatrixf&);
	virtual iEdAction* CreateSetWorldMatrixAction(const cMatrixf&);

	/////////////////////////////////////////////////////////////
	// Selection
	void SetSelected(bool);
	bool IsSelected() { return mbSelected; }

	/////////////////////////////////////////////////////////////
	// EditBox
	virtual iEdEditPane* CreateEditPane() { return NULL; }

	/////////////////////////////////////////////////////////////
	// Hierarchy
	void AddChild(iEdScnObject*);
	void RemChild(iEdScnObject*);

	void ClearChildren();

	bool HasChild(iEdScnObject*);

	iEdScnObject* GetParent() { return mpParent; }
	const tScnObjList& GetChildren() { return mlstChildren; }

	void SetParent(iEdScnObject*, bool abRemoveFromOld=true);

	iEdAction* CreateAddChildrenAction(tScnObjList& alstChildren);
	iEdAction* CreateRemChildrenAction(tScnObjList& alstChildren);

	/////////////////////////////////////////////////////////////
	// Outline helpers
	virtual bool SkipAddingToOutline(int alLevel, const tScnObjList& alstAddedChildren);
	virtual void AddToOutline(cWidgetListBox* apList, int alLevel, tScnObjList& alstAddedChildren);
	virtual void AddToOutlineSpecific(cWidgetListBox* apList, int alLevel, tScnObjList& alstAddedChildren){}

	iEngineObject* GetEngObject() { return mpEngObject; }

	virtual void DrawObject(const cModuleDrawData& aData, bool abHighlight, const cColor& aHighlightCol);
	virtual void DrawLinks(const cModuleDrawData& aData);

protected:

	virtual bool OnCreate();
	virtual void OnPostDeployAll(bool);
	virtual bool OnUpdate(bool);
	virtual void OnDestroy();

	virtual void OnDraw(const cModuleDrawData&);

	virtual void OnSetSelected(bool) {}

	virtual bool SetUpEngineObject();
	virtual bool SetUpIcon();

	virtual iEngineObject* CreateEngineObject()		{ return NULL; }
	virtual cObjectIcon* CreateIcon()				{ return NULL; }

	//////////////////////////////////////////////////////////
	// Data
	cVector3f	mvTranslation;
	bool		mbTranslationUpdated;
	cVector3f	mvRotation;
	bool		mbRotationUpdated;
	cVector3f	mvScale;
	bool		mbScaleUpdated;

	cMatrixf	mmtxTranslation;
	cMatrixf	mmtxRotation;
	cMatrixf	mmtxScale;

	bool		mbTransformUpdated;
	cMatrixf	mmtxTransform;

	iEngineObject* mpEngObject;
	cObjectIcon* mpIcon;

	bool mbSelected;

	bool mbEnabled;
	bool mbVisible;

	bool mbActive;

	cEdObjCompound* mpCompound;
	iEdScnObject*	mpParent;
	tScnObjList		mlstChildren;
};

//--------------------------------------------------------

///////////////////////////////////////////////////////////
// USER TYPE BASE
///////////////////////////////////////////////////////////

//--------------------------------------------------------

class iUserTypeNode;

//--------------------------------------------------------

class iUserTypeRoot : public iEdScnObjType
{
	friend class iUserTypeNode;
public:
	iUserTypeRoot(const tWString&, const tString&, const tString&);
	~iUserTypeRoot();

	void RegisterVarCatName(eEdVarCategory, const tString&);

	virtual bool Init();

	iEdObjectType* GetTypeByName(const tWString&);
	iUserTypeNode* GetDefaultType();

	cEdUserClassDefinition* GetUserClassDefinition() { return mpUserClassDef; }
protected:
	virtual iUserTypeNode* CreateSpecificUserTypeNode(iEdClass*)=0;

	iUserTypeNode* mpDefaultType;

	cEdUserClassDefinition* mpUserClassDef;
};

class iUserTypeNode : public iEdScnObjType
{
public:
	iUserTypeNode(iUserTypeRoot*, iEdClass*);

	bool Init();

	iEdObjectType* GetTypeByName(const tWString&);
	iUserTypeNode* GetDefaultType();

	cEdClassInstance* CreateClassInstance();

	iUserTypeRoot* GetUserTypeRoot() { return mpRoot; }

protected:
	iUserTypeRoot* mpRoot;
	iEdClass* mpClass;

	iUserTypeNode* mpDefaultType;
};

class iUserObjData : public iEdScnObjData
{
public:
	iUserObjData(iEdObjectType*);

	void CopyFromObject(iEdObject*);
	void CopyToObject(iEdObject*, ePropStep);

	bool LoadSpecificData(cXmlElement* apElem);
	bool SaveSpecificData(cXmlElement* apElem);

protected:
	cEdClassInstance* mpClassInstance;
};

class iUserObject : public iEdScnObject
{
public:
	iUserObject(iEdObjectData*);

	cEdClassInstance* GetClassInstance() { return mpClassInstance; }

	void SetType(iUserTypeNode*, bool abForce=false);
	void SetTypeChanged(bool abX=true) { mbTypeChanged = abX; }
	bool HasChangedType() { return mbTypeChanged; }

	cEdVarInstance* GetVar(const tWString&);
	void SetVar(const tWString&, const tWString&);

	virtual iEdAction* CreateSetUserTypeAction(iUserTypeNode*);
	virtual iEdAction* CreateSetUserValueAction(const tWString&, const tWString&);
protected:
	virtual void OnSetVar(const tWString&, const tWString&)=0;
    cEdClassInstance* mpClassInstance;

	bool mbTypeChanged;
};

//--------------------------------------------------------

///////////////////////////////////////////////////////////
// AGGREGATE BASE
///////////////////////////////////////////////////////////

//--------------------------------------------------------

class iBaseAggregateType : public iEdScnObjType
{
public:
	iBaseAggregateType(const tWString&, const tString&, const tString&);

	const tString& GetComponentElementName() { return msXmlComponentName; }

protected:
	tString msXmlComponentName;
};

class iBaseAggregateObjData : public iEdScnObjData
{
public:
	iBaseAggregateObjData(iEdObjectType*);

	virtual void CopyFromObject(iEdObject*);
	virtual void CopyToObject(iEdObject*, ePropStep);

	bool LoadSpecificData(cXmlElement*);
	bool SaveSpecificData(cXmlElement*);

protected:
	tEdObjDataVec	mvComponentData;
	tIntVec			mvComponentIDs;
};

class iBaseAggregateObject : public iEdScnObject
{
public:
	iBaseAggregateObject(iEdObjectData*);
	virtual ~iBaseAggregateObject();

	virtual void SetTranslation(const cVector3f&);
	virtual void SetRotation(const cVector3f&);
	virtual void SetScale(const cVector3f&);

	void SetUpCloneData(iEdObjectData*, const tIntVec&);
	tIntVec GetCloneIDs();

	const tScnObjList& GetComponents() { return mlstComponents; }

	virtual bool IsCulledByFrustum(cCamera*);

	virtual void AddComponent(iEdScnObject*);
	virtual void RemoveComponent(iEdScnObject*);

	bool HasComponents();
	iEdScnObject* GetComponentByID(int);
	iEdScnObject* GetComponentByName(const tWString&);

	int GetNumComponents();
	iEdScnObject* GetComponent(int);

	bool CheckRayIntersect(iEdViewport*, cVector3f*, tVector3fVec*, float*);
	bool Check2DBoxIntersect(iEdViewport*, const cRect2l&, cVector3f*);

	virtual iEdAction* CreateAddComponentsAction(const tScnObjList&) { return NULL; }
	virtual iEdAction* CreateRemoveComponentsAction(const tScnObjList&) { return NULL; }
	
protected:
	virtual bool OnUpdate(bool);
	virtual void OnAddComponent(iEdScnObject*)=0;
	virtual void OnRemoveComponent(iEdScnObject*)=0;
	virtual void UpdateComponents()=0;

	virtual void OnDestroy();

	virtual void ApplyTransformToComponents();

	void SetComponentsUpdated() { mbComponentsUpdated = true; }

	bool mbBypassComponentsTransform;
	bool mbComponentsUpdated;
	bool mbUpdatingComponents;

	tScnObjList mlstComponents;

	cMatrixf	mmtxInvWorldMatrix;
	tMatrixfList mlstRelWorldMatrices;
};

//--------------------------------------------------------

///////////////////////////////////////////////////////////
// ROOT OBJECT
///////////////////////////////////////////////////////////

//--------------------------------------------------------

class cEdScnObjRoot : public iEdScnObjType
{
public:
	cEdScnObjRoot();

protected:
	iEdObjectData* CreateTypeSpecificData() { return NULL; }
	void SetUpManipulators() {}
};

//--------------------------------------------------------

#endif // BASE_TYPES_H
