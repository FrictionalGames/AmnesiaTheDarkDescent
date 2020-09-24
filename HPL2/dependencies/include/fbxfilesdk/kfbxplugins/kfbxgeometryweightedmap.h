/*!  \file kfbxgeometryweightedmap.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXGEOMETRYWEIGHTEDMAP_H
#define FBXFILESDK_KFBXPLUGINS_KFBXGEOMETRYWEIGHTEDMAP_H

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

#include <fbxfilesdk/kfbxplugins/kfbxweightedmapping.h>

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/kfbxplugins/kfbxsdkmanager.h>
#include <fbxfilesdk/kfbxplugins/kfbxgroupname.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxGeometry;

/** \brief This class provides the structure to build a correspondence between 2 geometries.
  *
  * This correspondence is done at the vertex level. Which means that for each vertex in the
  * source geometry, you can have from 0 to N corresponding vertices in the destination
  * geometry. Each corresponding vertex is weighted.
  *
  * For example, if the source geometry is a NURB and the destination geometry is a mesh,
  * the correspondence object will express the correspondence between the NURB's control vertices
  * and the mesh's vertices.
  *
  * If the mesh corresponds to a tesselation of the NURB, the correspondence object can be used
  * to transfer any deformation that affect the NURB's control vertices to the mesh's vertices.
  *
  * See KFbxWeightedMapping for more details.
  */
class KFBX_DLL KFbxGeometryWeightedMap : public KFbxObject
{
    KFBXOBJECT_DECLARE(KFbxGeometryWeightedMap,KFbxObject);
public:
    /**
    * \name Error Management
    */
    //@{

    /** Retrieve error object.
    * \return     Reference to error object.
    */
    KError& GetError();

    /** \enum EError Error identifiers.
    * - \e eERROR
    * - \e eERROR_COUNT
    */
    typedef enum
    {
        eERROR,
        eERROR_COUNT
    } EError;

    /** Get last error code.
    * \return     Last error code.
    */
    EError GetLastErrorID() const;

    /** Get last error string.
    * \return     Textual description of the last error.
    */
    const char* GetLastErrorString() const;

    //@}

    /** Set correspondence values.
      * \param pWeightedMappingTable     Pointer to the table containing values
      * \return                          Pointer to previous correspondence values table.
      */
    KFbxWeightedMapping* SetValues(KFbxWeightedMapping* pWeightedMappingTable);

    /** Return correspondence values.
      * \return     Pointer to the correspondence values table.
      */
    KFbxWeightedMapping* GetValues() const;

    /** Return source geometry.
      * \return     Pointer to the source geometry, or \c NULL if there is no connected source geometry
      */
    KFbxGeometry* GetSourceGeometry();

    /** Return destination geometry.
      * \return     Pointer to the destination geometry, or \c NULL if there is no connected destination geometry
      */
    KFbxGeometry* GetDestinationGeometry();

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

    virtual KFbxObject& Copy(const KFbxObject& pObject);

protected:
    KFbxGeometryWeightedMap(KFbxSdkManager& pManager, char const* pName);
    virtual void Destruct(bool pRecursive, bool pDependents);

    // Real weigths table
    KFbxWeightedMapping* mWeightedMapping;

private:
    // Error management object
    KError mError;

    friend class KFbxGeometry;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

typedef KFbxGeometryWeightedMap* HKFbxGeometryWeightedMap;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXGEOMETRYWEIGHTEDMAP_H

