/*!  \file kfbxiosettings.h
 */

#ifndef FBXFILESDK_KFBXIO_KFBXIOSETTINGS_H
#define FBXFILESDK_KFBXIO_KFBXIOSETTINGS_H

/**************************************************************************************

 Copyright (C) 2001 - 2010 Autodesk, Inc. and/or its licensors.
 All Rights Reserved.

 The coded instructions, statements, computer programs, and/or related material 
 (collectively the "Data") in these files contain unpublished information 
 proprietary to Autodesk, Inc. and/or its licensors, which is protected by 
 Canada and United States of America federal copyright law and by international 
 treaties. 
 
 The Data may not be disclosed or distributed to third parties, in whole or in
 part, without the prior written consent of Autodesk, Inc. ("Autodesk").

 THE DATA IS PROVIDED "AS IS" AND WITHOUT WARRANTY.
 ALL WARRANTIES ARE EXPRESSLY EXCLUDED AND DISCLAIMED. AUTODESK MAKES NO
 WARRANTY OF ANY KIND WITH RESPECT TO THE DATA, EXPRESS, IMPLIED OR ARISING
 BY CUSTOM OR TRADE USAGE, AND DISCLAIMS ANY IMPLIED WARRANTIES OF TITLE, 
 NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR USE. 
 WITHOUT LIMITING THE FOREGOING, AUTODESK DOES NOT WARRANT THAT THE OPERATION
 OF THE DATA WILL BE UNINTERRUPTED OR ERROR FREE. 
 
 IN NO EVENT SHALL AUTODESK, ITS AFFILIATES, PARENT COMPANIES, LICENSORS
 OR SUPPLIERS ("AUTODESK GROUP") BE LIABLE FOR ANY LOSSES, DAMAGES OR EXPENSES
 OF ANY KIND (INCLUDING WITHOUT LIMITATION PUNITIVE OR MULTIPLE DAMAGES OR OTHER
 SPECIAL, DIRECT, INDIRECT, EXEMPLARY, INCIDENTAL, LOSS OF PROFITS, REVENUE
 OR DATA, COST OF COVER OR CONSEQUENTIAL LOSSES OR DAMAGES OF ANY KIND),
 HOWEVER CAUSED, AND REGARDLESS OF THE THEORY OF LIABILITY, WHETHER DERIVED
 FROM CONTRACT, TORT (INCLUDING, BUT NOT LIMITED TO, NEGLIGENCE), OR OTHERWISE,
 ARISING OUT OF OR RELATING TO THE DATA OR ITS USE OR ANY OTHER PERFORMANCE,
 WHETHER OR NOT AUTODESK HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSS
 OR DAMAGE. 

**************************************************************************************/
#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/components/kbaselib/klib/kstring.h>

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>

#include <fbxfilesdk/components/libxml2-2.6.4/libxml.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/parser.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/tree.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/globals.h>


// undef the macro mkdir, since it conflict with function mkdir in Qt\4.2.3\src\corelib\io\qdir.h
#if (defined(_MSC_VER) || defined(__MINGW32__)) && defined(mkdir)
	#undef mkdir
#endif

#include <fbxfilesdk/kfbxplugins/kfbxtypes.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

#include <fbxfilesdk/kfbxio/kfbxiosettingspath.h>

#define IOSVisible    true
#define IOSHidden     false

#define IOSSavable    true
#define IOSNotSavable false

#define IOSEnabled    true
#define IOSDisabled   false

#define IOSBinary     0
#define IOSASCII      1

class KFbxIOSettings;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
	/** \internal usage only by Readers and Writers to handle information in a class removed ( StreamOptions )  
	*/
class KsoInfo
{

public: enum EIMPEXP
{
    eIMPORT = 0,
    eEXPORT
};

public:
    //! Default constructor.
    KsoInfo();

    //! Set values to default/
    void Reset(EIMPEXP pImpExp);
    
	//! If pTimeMode is set to KTime::eDEFAULT_MODE, pCustomFrameRate is used instead.
    void SetTimeMode(KTime::ETimeMode pTimeMode, double pCustomFrameRate = 0.0);

	//! A time mode set to KTime::eDEFAULT_MODE means a custom frame rate is set.
    KTime::ETimeMode GetTimeMode(){ return mTimeMode; }

	//! Get frame period associated with time mode or a custom frame period if time mode is set to KTime::eDEFAULT_MODE.
    KTime GetFramePeriod();
	
    void SetASFScene(KFbxObject* pASFScene, bool pASFSceneOwned = false);
    KFbxObject* GetASFScene(){ return mASFScene; }

	void Set_IOS(KFbxIOSettings *pios){ios = pios;}


private: 

    KTime::ETimeMode mTimeMode;
    KFbxObject * mASFScene;

    EIMPEXP mImpExp;

	KFbxIOSettings *ios;
};

#endif // DOXYGEN_SHOULD_SKIP_THIS

class KFbxSdkManager;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
	/** \internal usage only by plugin UI  
	*/

// http://www.microsoft.com/globaldev/reference/winxp/langtla.mspx

//  LOCALE                            TLA  LCID

//  English - United States           ENU  0409
//  German - Germany                  DEU  0407
//  French - France                   FRA  040c
//  Japanese - Japan                  JPN  0411
//  Korean(Extended Wansung) - Korea  KOR  0412
//  Chinese - PRC                     CHS  0804

enum FBXUILANGUAGE
{
    eENU = 0,			        // MAX 409 English - United States
	eDEU,           	        // MAX 407 German - Germany
	eFRA,            	        // MAX 40c French - France
	eJPN,           	        // MAX 411 Japanese - Japan
    eKOR,            	        // MAX 412 Korean(Extended Wansung) - Korea 
    eCHS,	                    // MAX 804 Chinese - PRC
    eFBXUILANGUAGE_COUNT
};

// this class is located in the UserDataPtr of each KFbxIOSettings properties
class PropInfo
{
public:
	PropInfo();
    ~PropInfo();

	void            * UIWidget;            // UI widget for showing the property
	void            * cbValueChanged;      // call back when value changed
    void            * cbDirty;             // call back when value changed
    KStringList       labels;              // list of labels in many languages
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** KFbxIOSettings is a collection of properties, arranged as a tree, that 
  * can be used by FBX file readers and writers to represent import and export
  * options. 
  * It is primarily used by FBX importers (KFbxImporter) and FBX exporter (KFbxExporter) 
  * when reading or writing data from or to a disk. 
  * The FBX plugins of some Autodesk products expose a UI representing the content of those options
  * to let users see and choose options when an import or export operation is about to be done.
  * The tree of options is extensible.
  * 
  * Options can be saved or loaded from an XML file using the functions: 
  * ReadXMLFile(), WriteXMLFile(), WriteXmlPropToFile(). This functionality can be useful 
  * for plugins that use preset files. 
  *
  * An instance of KFbxIOSettings must be created to be used before an import/export operation.
  * When a new KFbxIOSettings instance is created, all options are created with default values.
  * The new instance of KFbxIOSettings can be passed to the KFbxSdkManager, 
  * this way that instance will be used by all import/export operations.
  *
  * Ex: to set an instance of KFbxIOSettings to the KFbxSdkManager
  *
  * \code
  * // First create a new instance of KFbxIOSettings
  * KFbxIOSettings * ios = KFbxIOSettings::Create((KFbxSdkManager *) mKFbxSdkManager, IOSROOT);
  * // then set the KFbxSdkManager
  * mKFbxSdkManager->SetIOSettings(ios);
  * \endcode
  *
  * It's also possible for a developer to create another instance
  * of KFbxIOSettings, set particular options and use it for import/export operation.
  *
  * Ex: to set an instance of KFbxIOSettings to a KFbxImporter/KFbxExporter
  * \code
  * mKFbxImporter->SetIOSettings(ios); / mKFbxExporter->SetIOSettings(ios);
  * \endcode
  *
  * A schematic view of the KFbxIOSettings tree :
  *
  * \verbatim
  
                                         OPTION_GROUP_ROOT (IOSROOT)
                                         |
                                         |
                     ________________________________________
                     |                                      |     
                     -OPTION_GROUP_EXPORT (IOSN_EXPORT)     -OPTION_GROUP_IMPORT (IOSN_IMPORT)
                          |                                      |
                          -OPTION_GROUP_A                        -OPTION_GROUP_A
                          |     |                                |     |
                          |     -OPTION_A                        |     -OPTION_A
                          |     -OPTION_B                        |     -OPTION_B
                          |                                      |
                          -OPTION_GROUP_B                        -OPTION_GROUP_B
                          |     |                                |     |
                          |     -OPTION_GROUP_A                  |     -OPTION_GROUP_A
                          |     |     |                          |     |     |
                          |     |     -OPTION_A                  |     |     -OPTION_A
                          |     |     -OPTION_B                  |     |     -OPTION_B
                          |     |                                |     |
                          |     -OPTION_GROUP_B                  |     -OPTION_GROUP_B
                          |           |                          |           |
                          |           -OPTION_A                  |           -OPTION_A
                          |           -OPTION_B                  |           -OPTION_B 
                          |                                      |
                          -OPTION_GROUP_C                        -OPTION_GROUP_C
                                |                                      |
                                -OPTION_A                              -OPTION_A
  
  \endverbatim
  *
  * Any group of options can contain sub options, or group of sub options.
  * To access an option value, we must pass the full path to the Get/Set functions
  * Ex: 
  * \code
  * ios->GetBoolProp("Import|IncludeGrp|Animation", true); // the root node name is not required
  * \endcode
  *
  * All options path are defined in the file kfbxiosettingspath.h to ease the access of any options.
  * Then "Import|IncludeGrp|Animation" == IMP_ANIMATION since IMP_ANIMATION is defined in kfbxiosettingspath.h
  * All options defined path start with "IMP_" for import branch or "EXP_" for export branch.
  *
  * We strongly encourage to use the defined path in kfbxiosettingspath.h, this way if the parent group of an option is changed
  * the change occur only in kfbxiosettingspath.h not in the code elsewhere.
  *
  * Ex: to get the boolean import "Animation" option
  * \code
  * bool anim = ios->GetBoolProp(IMP_ANIMATION, true); // will return true if not found, since we pass true as second param
  * \endcode
  *
  * Ex: to set the boolean import "Animation" option to false
  * \code
  * ios->SetBoolProp(IMP_ANIMATION, false);
  * \endcode
  *
  * Ex: to create a new option group under the "Import" branch
  * \code
  * // get the parent "Import" property
  * KFbxProperty import_Group = ios->GetProperty( IOSN_IMPORT ); // IOSN_IMPORT is defined as "Import" in kfbxiosettingspath.h
  * if(import_Group.IsValid()) // check if we have found the IOSN_IMPORT parent option
  * {
  *     // add a new group of options "myOptionGroup"
  *		KFbxProperty myOptionGrp = ios->AddPropertyGroup(import_Group, "myOptionGroup", DTString, "My Option Group UI Label");
  * }
  * \endcode
  *
  * Ex: to create a new boolean option under the "myOptionGroup"
  * \code
  * KFbxProperty myOptionGrp = ios->GetProperty( "Import|myOptionGroup" ); // can also use IOSN_IMPORT|"myOptionGroup"
  * if(myOptionGrp.IsValid()) // check if we have found the "myOptionGroup"
  * {
  *     bool defaultValue = true;
  *		KFbxProperty myOption = ios->AddProperty(myOptionGrp, "myOptionName", DTBool, "My Option UI label" , &defaultValue, eBOOL1);
  * }
  * \endcode
  *
  * Ex: to set some flags to myOption
  * \code
  * KFbxProperty myOption = ios->GetProperty( "Import|myOptionGroup|myOptionName" );
  * if(myOption.IsValid())
  * {
  *		myOPtion.ModifyFlag(KFbxProperty::eUI_HIDDEN, true);   // to make that option not visible to the UI
  *		myOPtion.ModifyFlag(KFbxProperty::eNOT_SAVABLE, true); // to avoid the read/save of that option in XML file
  * }
  * \endcode
  */
class KFBX_DLL KFbxIOSettings : public KFbxObject
{
	KFBXOBJECT_DECLARE(KFbxIOSettings,KFbxObject);

public:

        /** Destruct a property and optionally the children properties
		  * \param pRecursive
		  * \param pDependents
		  */
	virtual void Destruct(bool pRecursive, bool pDependents);

        /** Add a property group under the root prop to be a direct child of IOSROOT
		  * \param pName
		  * \param pDataType
		  * \param pLabel
		  * \return a new KFbxProperty created
		  */
	KFbxProperty AddPropertyGroup(char const* pName, KFbxDataType const &pDataType=KFbxDataType(), char const* pLabel="");

		/** Add a property group under another parent property
		  * \param pParentProperty
		  * \param pName
		  * \param pDataType
		  * \param pLabel   (optional, used by the UI as widget label)
		  * \param pVisible (used by the UI to show or not that property)
		  * \param pSavable (to enable a read & write to an XML file)
		  * \param pEnabled (used by the widget UI to show enabled or disabled)
		  * \return a new KFbxProperty created
		  */
	KFbxProperty AddPropertyGroup(	KFbxProperty const &pParentProperty, 
			                          	char const*		    pName, 
									  	KFbxDataType const &pDataType      = KFbxDataType(), 
									  	char const*         pLabel         = "",
									  	bool				pVisible	   = true,
									  	bool				pSavable	   = true,
									  	bool				pEnabled	   = true
									 );

		/** Add a property under another parent property with a value to set
		  * \param pParentProperty
		  * \param pName
		  * \param pDataType
		  * \param pLabel      (optional, used by the UI as widget label)
		  * \param pValue
		  * \param pValueType
		  * \param pVisible    (used by the UI to show or not that property)
		  * \param pSavable    (to enable a read & write to an XML file)
		  * \param pEnabled    (used by the widget UI to show enabled or disabled)
		  * \return a new KFbxProperty created
		  */		  
	KFbxProperty AddProperty(    KFbxProperty const     &pParentProperty, 
			                         char const*            pName,		
								     KFbxDataType const     &pDataType      = KFbxDataType(), 
								     char const*            pLabel          = "",
									 void const*            pValue          = NULL,
								     EFbxType               pValueType      = eUNIDENTIFIED,
									 bool					pVisible		= true,
									 bool					pSavable		= true,
									 bool					pEnabled		= true
								   );

		/** Add a property under another parent property with a value to set and a min max values
		  * \param pParentProperty
		  * \param pName
		  * \param pDataType
		  * \param pLabel     (optional, used by the UI as widget label)
		  * \param pValue
		  * \param pMinValue
		  * \param pMaxValue
		  * \param pValueType
		  * \param pVisible   (used by the UI to show or not that property)
		  * \param pSavable   (to enable a read & write to an XML file)
		  * \param pEnabled   (used by the widget UI to show enabled or disabled)
		  * \return a new KFbxProperty created
		  * \remarks Normally used with numeric properties Ex: integer, float, double, etc.
		  */		  		  
	KFbxProperty AddPropertyMinMax(    KFbxProperty const     &pParentProperty, 
			                         char const*            pName, 
								     KFbxDataType const     &pDataType      = KFbxDataType(), 
								     char const*            pLabel          = "",
									 void const*            pValue          = NULL,
									 double const*			pMinValue		= NULL,
									 double const*			pMaxValue		= NULL,
								     EFbxType               pValueType      = eUNIDENTIFIED,									 
									 bool					pVisible		= true,
									 bool					pSavable		= true,
									 bool					pEnabled		= true
								   );


		/** Get a property using the full path in the tree ex: "Export|IncludeGrp|Animation"
		  * \param pName
		  * \return a KFbxProperty found
		  * \remarks We strongly encourage to use the defined path in kfbxiosettingspath.h
		  * ex: EXP_ANIMATION == "Export|IncludeGrp|Animation"
		  */
	KFbxProperty GetProperty(char const* pName) const;

		/** Get a property using a short path found under the parent property.
		  * \param pParentProperty
		  * \param pName
		  * \return a KFbxProperty found
		  * \remarks This is a faster way to access a property when the parent is known
		  */
	KFbxProperty GetProperty(KFbxProperty const &pParentProperty, char const* pName) const;

        /** Get a bool property value using the full path
		  * \param pName
		  * \param pDefValue  Value returned if the property is not found
		  * \return true or false
		  */
	bool GetBoolProp(char const* pName, bool pDefValue) const;

        /** set a bool property value using the full path
		  * \param pName
		  * \param pValue
		  */
	void SetBoolProp(char const* pName, bool pValue);

        /** Get a double property value using the full path
		  * \param pName
		  * \param pDefValue Value returned if the property is not found
		  * \return a double
		  */
	double GetDoubleProp(char const* pName, double pDefValue) const;

        /** Set a double property using the full path
		  * \param pName
		  * \param pValue
		  */
	void   SetDoubleProp(char const* pName, double pValue);

        /** Get a int property value using the full path
		  * \param pName
		  * \param pDefValue Value returned if the property is not found
		  * \return a int
		  */
	int    GetIntProp(char const* pName, int pDefValue) const;

        /** Set a int property value using the full path
		  * \param pName
		  * \param pValue
		  */
	void   SetIntProp(char const* pName, int pValue);

        /** Get a KTime property value using the full path
		  * \param pName 
		  * \param pDefValue  Value returned if the property is not found
		  */
	KTime  GetKTimeProp(char const* pName, KTime pDefValue) const;

        /** Set a KTime property value using the full path
		  * \param pName 
		  * \param pValue
		  * \return a KTime
		  */
	void   SetKTimeProp(char const* pName, KTime pValue);

		/** \name Enum Properties 
          * An enum property is a list of KString and integer pairs.
		  * A current index value is available to get the selected pair
		  * of KString+integer
		  *
		  * Ex: Content of an enum property
          * \code
		  *    0 -> (14, "Bird")
		  *    1 -> (17, "Horse")
		  *    2 -> (93, "Cat")
		  *    3 -> (45, "Dog")
		  * \endcode
          * 
		  *    If current index is 2: the current int value is 93, 
		  *    and the current KString value is "Cat"
		  */    		
    
    //@{
	
        /** Get the KString at current index of an enum property using the full path.
		  * \param pName
		  * \param pDefValue Value returned if the property is not found
		  * \return a KString
		  */
	KString GetEnumProp(char const* pName, KString pDefValue) const;

        /** Get the integer at current index of an enum property using the full path.
		  * \param pName
		  * \param pDefValue Value returned if the property is not found
		  * \return a int
		  */
	int     GetEnumProp(char const* pName, int pDefValue) const;

        /** Get the index of a KString from the enum property using the full path.
		  * \param pName
		  * \param pValue Return -1 if the KString is not found
		  * \return a int
		  */
	int     GetEnumIndex(char const* pName, KString pValue) const;

		/** Set the current index using an existing KString of an enum property using the full path.
		  * \param pName
		  * \param pValue
		  * \remarks The current index will not change if the KString is not found
		  */
	void    SetEnumProp(char const* pName, KString pValue);

		/** Set the current index of an enum property using the full path.
		  * \param pName
		  * \param pValue
		  * \remarks The current index will not change if the pValue is out of bound
		  */
	void    SetEnumProp(char const* pName, int pValue);

		/** Remove a pair of KString+integer from an enum property.
		  * \param pName
		  * \param pValue The KString to find
		  * \remarks The first KString found from 0 index will be removed only even 
		  * if the same KString exist in other index, if the current index was on the KString found
		  * the current index will be set to 0
		  */
	void    RemoveEnumPropValue(char const* pName, KString pValue);

		/** Empty all the KString+integer pair of the enum property
          * \param pName
		  */
	void    EmptyEnumProp(char const* pName);

		/** Check if a KString is present in the enum property.
		  * \param &pProp a ref to an enum prop
		  * \param &enumString ref to a KString to find
		  * \return \c true if found, \c false otherwise. 
		  */
	bool	IsEnumExist(KFbxProperty& pProp, KString& enumString) const;

		/** Get the enum index of a KString
		  * \param &pProp a ref to an enum prop
		  * \param &enumString ref to string to find
		  * \param pNoCase To match case sensitive or not
		  * \return the index found or -1 if not found
		  */
	int		GetEnumIndex(KFbxProperty& pProp, KString& enumString, bool pNoCase = false) const;
    //@}

		/** Set a specific flag value on a property using the full path
		  * \param pName
		  * \param propFlag
		  * \param pValue
		  * \return Always true
		  */
	bool    SetFlag(char const* pName, FbxPropertyFlags::eFbxPropertyFlags propFlag, bool pValue);

        /** Get a KString property value using the full path.
		  * \param pName
		  * \param pDefValue  Value returned if the property is not found
		  * \return The KString value
		  */
	KString GetStringProp(char const* pName, KString pDefValue) const;

        /** Set a KString property value using the full path
		  * \param pName
		  * \param pValue
		  */
	void    SetStringProp(char const* pName, KString pValue);

    /** \name XML Serialization Functions */
    //@{
     
		/** Load the settings values from an XML file.
		  * \param path The path of the XML file.
		  * \return \c True on success, \c false otherwise.
		  */
	virtual bool ReadXMLFile(KString& path);

		/** Write the settings values to an XML file.
		  * \param path The path of the XML file.
          * \return \c True on success, \c false otherwise.
		  * \remarks The flag of the property must be eNOT_SAVABLE == false
		  */
	virtual bool WriteXMLFile(KString& path);

		/** Write the settings values to an XML file.
		  * \param pFullPath The path of the XML file.
		  * \param propPath a prop Path
          * \return \c True on success, \c false otherwise.
		  * \remarks To save only a branch of the settings ex: Import branch only
		  */
	bool WriteXmlPropToFile(KString& pFullPath, KString& propPath);
    //@}

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
public:
	PropInfo * GetPropInfo(KFbxProperty &pProp);

	FBXUILANGUAGE UILanguage;
	KString GetLanguageLabel(KFbxProperty &pProp);
	void SetLanguageLabel(KFbxProperty &pProp, KString &pLabel);
	FBXUILANGUAGE Get_Max_Runtime_Language(KString regLocation);

	KsoInfo impInfo;
	KsoInfo expInfo;

	static KString GetUserMyDocumentDir();
	void SetPropVisible(KFbxProperty &pProp, bool pWithChildren, bool pVisible);

	virtual bool ConstructProperties(bool pForceSet);

			// Read an XML file from MyDocument dir
	bool ReadXmlPropFromMyDocument(KString& subDir, KString& filename);

			// Write property branch to an XML file in MyDocument dir
	bool WriteXmlPropToMyDocument(KString& subDir, KString& filename, KString& propPath);

    static const char * GetFileMergeDescription(int pIndex);

    enum ELoadMode         
    { 
		eCREATE,          /*!< Add to scene(duplicate the ones with the same name)    */  
		eMERGE,           /*!< Add to scene and update animation                      */
        eEXCLUSIVE_MERGE  /*!< Update animation                                       */
    };


	enum EQuaternionMode   { eAS_QUATERNION, eAS_EULER, eRESAMPLE };
	enum EObjectDerivation { eBY_LAYER, eBY_ENTITY, eBY_BLOCK }; 

	enum ESysUnits
	{
		kUNITS_USER,
		kUNITS_INCHES, 
		kUNITS_FEET,
		kUNITS_YARDS,
		kUNITS_MILES,
		kUNITS_MILLIMETERS,
		kUNITS_CENTIMETERS,
		kUNITS_METERS,
		kUNITS_KILOMETERS
	};

	enum ESysFrameRate  
	{
		kFRAMERATE_USER,
		kFRAMERATE_HOURS,
		kFRAMERATE_MINUTES,
		kFRAMERATE_SECONDS,
		kFRAMERATE_MILLISECONDS,
		kFRAMERATE_GAMES_15,
		kFRAMERATE_FILM_24,
		kFRAMERATE_PAL_25,
		kFRAMERATE_NTSC_30,
		kFRAMERATE_SHOWSCAN_48,
		kFRAMERATE_PALFIELD_50,
		kFRAMERATE_NTSCFIELD_60
	};
    
// Max
	enum EEnveloppeSystem
	{
		eSKIN_MODIFIER,
		ePHYSIQUE,
		eBONESPRO,
		eENVELOPPE_SYSTEM_COUNT
	};

// Max
	enum EGeometryType
	{
		eTRIANGLE,
		eSIMPLIFIED_POLY,
		ePOLY,
		eNURB,
		ePATCH,
		eGEOMETRY_TYPE_COUNT
	};

// Maya IK type
	enum EIKType
	{
		eNONE,
		eFBIK,		
		eHUMANIK
	};

protected:
	KFbxIOSettings(KFbxSdkManager& pManager,char const* pName);

private:
	// the caller must delete the KString returned
	KString* GetAttributeFromXmlNode(xmlNode* node, KString* attName);

	// the caller must delete the KString returned
	KString* GetInnerTextXmlNode(xmlNode* node);

	void CreatePropRecursive(xmlNode* pNode, KFbxProperty& pPropParent);

	xmlNode* GetChildXmlNode(xmlNode* parent, KString* nodeName);
	xmlNode* AddChildXmlNode(xmlNode* parent, KString* nodeName, KString* nodeValue = NULL);
	xmlAttr* AddAttributeToXmlNode(xmlNode* node, KString* attName, KString* attValue = NULL);

	void AddNewPropInfo(KFbxProperty &pProp);
	void DeletePropInfo(KFbxProperty &pProp);
	void DeleteAllPropInfo(KFbxProperty &pProp);

	// add recurively a xml node equivalent for a KFbxProperty and all child KFbxProperties
	void AddXMLProp(KFbxProperty& pProp, xmlNodePtr pParent, xmlDocPtr pXmlDoc);

	// create a new xmlNode with all KFbxProperty info
	xmlNodePtr GetXMLNode(KFbxProperty& pProp);
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXIO_KFBXIOSETTINGS_H

