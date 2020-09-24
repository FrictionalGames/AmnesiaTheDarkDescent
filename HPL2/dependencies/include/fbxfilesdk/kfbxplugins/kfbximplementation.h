/****************************************************************************************

Copyright (C) 2011 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/
/*!  \file kfbximplementation.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXIMPLEMENTATION_H 
#define FBXFILESDK_KFBXPLUGINS_KFBXIMPLEMENTATION_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxsdkmanager.h>
#include <fbxfilesdk/kfbxplugins/kfbxobject.h>

#include <fbxfilesdk/components/kbaselib/klib/kdynamicarray.h>
#include <fbxfilesdk/components/kbaselib/klib/kpair.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxBindingOperator;
class KFbxBindingTable;

/** This object represents the shading node implementation.
  * It defines basic information about the shader and the binding table(KFbxBindingTable).
  * For example, you can create a new KFbxImplementation like this:
  * \code
  * KFbxImplementation* lImpl = KFbxImplementation::Create( &pMyScene, "MyImplementation" );
  * pMyObject.AddImplementation( lImpl );
  * pMyObject.SetDefaultImplementation( lImpl );
  * lImpl->RenderAPI = sDirectX; //sDirectX, sOpenGL, sMentalRay or sPreview
  * lImpl->RenderAPIVersion = "9.0"; //API Version
  *
  * lImpl->Language = sHLSL; //sHLSL, sGLSL, sCGFX or sMentalRaySL
  * lImpl->LanguageVersion = "1.0";  //Language Version
  * \endcode
  *
  * After the new KFbxImplementation is created, you can access KFbxBindingTable like this:
  * \code
  * KFbxBindingTable* lTable = lImpl->GetTableByTargetName("root");
  * \endcode
  * Also, you can access the exist KFbxImplementation in KFbxObject by this:
  * \code
  * const KFbxImplementation* lImpl = GetImplementation( pMyObject, ImplementationCGFX ); // ImplementationPreview, ImplementationMentalRay, ImplementationCGFX, ImplementationHLSL, ImplementaitonOGS or ImplementationNone
  * \endcode
  * \nosubgrouping
  * \see KFbxImplementationFilter
  */
class KFBX_DLL KFbxImplementation : public KFbxObject
{
	KFBXOBJECT_DECLARE(KFbxImplementation, KFbxObject);

public:
    /**
      * \name Target Name.
      */
    //@{
	KString									RenderName;
    //@}

    /**
      * \name Shader Language and API descriptions.
      */
    //@{

	/** Shader Language.
      * \see sHLSL, sGLSL, sCGFX and sMentalRaySL in conventions.h
      */
	KFbxTypedProperty<fbxString>			Language;

	//! Shader Language version.
	KFbxTypedProperty<fbxString>			LanguageVersion;

	/** Render API.
      * \see sHLSL, sGLSL, sCGFX and sMentalRaySL in conventions.h
      */
	KFbxTypedProperty<fbxString>			RenderAPI;

	//! Render API version.
	KFbxTypedProperty<fbxString>			RenderAPIVersion;
    //@}


    /**
      * \name Binding description
      */
    //@{

	//! Name of root binding table.
	KFbxTypedProperty<fbxString>			RootBindingName;

	//! Property to store the shader parameters(constants) values in this implementation.
	KFbxProperty GetConstants() const;

	/** Add a new binding table to the table list.
	  * \param pTargetName The target name for the binding table.
	  * \param pTargetType The target type for the binding table.
	  * \return the new binding table.
	  */ 
	KFbxBindingTable* AddNewTable( char const* pTargetName, char const* pTargetType );

    /** Retrieves a handle on the root binding table.
    * \return A const pointer to the root table or NULL if it does not exist.
    */ 
    KFbxBindingTable const* GetRootTable() const;
    
	/** Retrieves a handle on the root binding table.
	* \return A pointer to the root table or NULL if it does not exist.
	*/
	KFbxBindingTable* GetRootTable();
    
    /** Gets the number of binding tables.
	  * \return the number of binding tables.
	  */ 
	int GetTableCount() const;

	/** Retrieves a handle on the (pIndex)th binding table.
	  * \param pIndex The index of the table to retrieve. Valid values are [ 0, GetTableCount() ).
	  * \return A const pointer to the pIndex-th table or NULL if pIndex is out of range.
	  */ 
	KFbxBindingTable const* GetTable( int pIndex ) const;
    /** Retrieves a handle on the (pIndex)th binding table.
	  * \param pIndex The index of the table to retrieve. Valid values are [ 0, GetTableCount() ).
	  * \return A const pointer to the pIndex-th table or NULL if pIndex is out of range.
	  */ 
	KFbxBindingTable* GetTable( int pIndex );

	/** Returns the binding table that has the given target name.
	* \param pName The target name of the table to look for.
	* \return A const pointer to the binding table with the given target name, or NULL if there is no such binding table.
	*/ 
	KFbxBindingTable const* GetTableByTargetName( char const* pName ) const;
	
	/** Returns the binding table that has the given target name.
	* \param pName The target name of the table to look for.
	* \return A pointer to the binding table with the given target name, or NULL if there is no such binding table.
	*/ 
	KFbxBindingTable* GetTableByTargetName( char const* pName );

  	/** Returns the binding table that has the given target type.
	* \param pTargetType The target type to look for.
	* \return A const pointer to the binding table with the given target type, or NULL if there is no such binding table.
	*/
	KFbxBindingTable const* GetTableByTargetType( char const* pTargetName ) const;
	
	/** Returns the binding table that has the given target type.
	* \param pTargetType The target type to look for.
	* \return A pointer to the binding table with the given target type, or NULL if there is no such binding table.
	*/
	KFbxBindingTable* GetTableByTargetType( char const* pTargetName );

	
	/** Add a new binding operator to the operator list.
	* \param pTargetName The target name for the binding operator.
	* \param pFunctionName The function name for the binding operator.
	* \return The new operator.
	*/ 
	KFbxBindingOperator* AddNewBindingOperator( char const* pTargetName, char const* pFunctionName );

	/** Gets the number of binding operators.
	  * \return the number of binding operators.
	  */ 
	int GetBindingOperatorCount() const;

	/** Returns the binding operator that has the given name.
	* \param pTargetName The target name of the binding operator to look for.
	* \return A const pointer to the binding operator with the given name, or NULL if there is no such binding table.
	*/
	KFbxBindingOperator const* GetOperatorByTargetName( char const* pTargetName ) const;
    //@}


    /**
      * \name Static values
      */
    //@{

	// property names

	/** Shader Language name.
      * \see Language
      */
	static const char* sLanguage;

	/** Shader Language version.
      * \see LanguageVersion
      */
	static const char* sLanguageVersion;

	/** Shader render API.
      * \see RenderAPI
      */
	static const char* sRenderAPI;

	/** Shader render API version.
      * \see RenderAPIVersion
      */
	static const char* sRenderAPIVersion;

	/** Name of root binding table.
      * \see RootBindingName
      */
	static const char* sRootBindingName;

	/** Name of property to store the shader parameters(constants) values in this implementation.
      * \see GetConstants
      */
	static const char* sConstants;

    //! default value for implementation type.
	static const char* sDefaultType;

    //! default value for shader language.
	static const char* sDefaultLanguage;

    //! default value for shader language version.
	static const char* sDefaultLanguageVersion;

    //! default value for shader render API.
	static const char* sDefaultRenderAPI;

    //! default value for shader render API version.
	static const char* sDefaultRenderAPIVersion;

    //! default value for root binding table name.
	static const char* sDefaultRootBindingName;
    //@}

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
	// Constructor / Destructor
	KFbxImplementation(KFbxSdkManager& pManager, char const* pName);
	virtual bool ConstructProperties(bool pForceSet);
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXIMPLEMENTATION_H

