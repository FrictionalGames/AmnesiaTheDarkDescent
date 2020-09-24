/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/
/*!  \file kfbxbindingtable.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXBINDINGTABLE_H 
#define FBXFILESDK_KFBXPLUGINS_KFBXBINDINGTABLE_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxbindingtablebase.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** A binding table represents a collection of bindings
  * from source types such as KFbxObjects, or KFbxLayerElements
  * to corresponding destinations, usually a third party shader parameters.
  * Binding represents a link between internal object(e.g. KFbxObject) and 
  * external object(e.g. HLSL shader parameters).
  * \nosubgrouping
  * \see KFbxBindingOperator, KFbxBindingTableBase
  */
class KFBX_DLL KFbxBindingTable : public KFbxBindingTableBase
{
    KFBXOBJECT_DECLARE(KFbxBindingTable, KFbxBindingTableBase);

public:
    /** This property stores the name of target.
      *
      * Default value is "".
      */
    KFbxTypedProperty<fbxString>            TargetName;

    /** This property stores the type of target.
      *
      * Default value is "".
      */
    KFbxTypedProperty<fbxString>            TargetType;

    /** Relative URL of file containing the shader implementation description. 
      * e.g.: ./shader.mi
      * Default value is "".
      */
    KFbxTypedProperty<fbxString>            DescRelativeURL;

    /** Absolute URL of file containing the shader implementation description.
      * e.g.: file:///usr/tmp/shader.mi
      * Default value is "".
      */
    KFbxTypedProperty<fbxString>            DescAbsoluteURL;

    /** Identify the shader to use in previous description's URL.
      * e.g.: MyOwnShader
      * Default value is "".
      */
    KFbxTypedProperty<fbxString>            DescTAG;        

    /** Relative URL of file containing the shader implementation code.
      * e.g.: ./bin/shader.dll
      * Default value is "".
      */
    KFbxTypedProperty<fbxString>            CodeRelativeURL;

    /** Absolute URL of file containing the shader implementation code.
      * e.g.: file:///usr/tmp/bin/shader.dll
      * Default value is "".
      */
    KFbxTypedProperty<fbxString>            CodeAbsoluteURL;

    /** Identify the shader function entry to use in previous code's URL.
      * e.g.: MyOwnShaderFunc
      * Default value is "".
      */
    KFbxTypedProperty<fbxString>            CodeTAG;

    //////////////////////////////////////////////////////////////////////////
    // Static values
    //////////////////////////////////////////////////////////////////////////

    //! Target name.
    static const char* sTargetName;

    //! Target type.
    static const char* sTargetType;

    //! Relative URL for shader description. 
    static const char* sDescRelativeURL;

    //! Absolute URL for shader description.
    static const char* sDescAbsoluteURL;

    //! Identify the shader to use in previous description's URL.
    static const char* sDescTAG;

    //! Relative URL for shader code. 
    static const char* sCodeRelativeURL;

    //! Absolute URL for shader code.
    static const char* sCodeAbsoluteURL;

    //! Identify the shader function entry to use in previous code's URL.
    static const char* sCodeTAG;


    //! Default value for  target name.
    static const char* sDefaultTargetName;

    //! Default value for  target type.
    static const char* sDefaultTargetType;

    //! Default value for relative URL for shader description. 
    static const char* sDefaultDescRelativeURL;

    //! Default value for absolute URL for shader description. 
    static const char* sDefaultDescAbsoluteURL;

    //! Default value for identifying the shader to use in previous description's URL.
    static const char* sDefaultDescTAG;

    //! Default value for relative URL for shader code.
    static const char* sDefaultCodeRelativeURL;

    //! Default value for absolute URL for shader code. 
    static const char* sDefaultCodeAbsoluteURL;

    //! Default value for identifying the shader function entry to use in previous code's URL.
    static const char* sDefaultCodeTAG;

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//    Anything beyond these lines may not be documented accurately and is 
//     subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    KFbxBindingTable(KFbxSdkManager& pManager, char const* pName);

    virtual bool ConstructProperties(bool pForceSet);
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXBINDINGTABLE_H

