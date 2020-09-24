/*!  \file kfbxsystemunit.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSYSTEMUNIT_H
#define FBXFILESDK_KFBXPLUGINS_KFBXSYSTEMUNIT_H

/**************************************************************************************

 Copyright (C) 2001 - 2009 Autodesk, Inc. and/or its licensors.
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

#include <fbxfilesdk/components/kbaselib/klib/kstring.h>
#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxGlobalSettings;
class KFbxAnimCurveNode;
class KFbxXMatrix;
class KFbxNode;
class KFbxScene;

/** \brief This class describes the units of measurement used within a particular scene.
  * \nosubgrouping
  */
class KFBX_DLL KFbxSystemUnit 
{
public:

    /** Struct to define various options that you can use to convert the system unit of a scene.
      * The default values are:
      *         mConvertRrsNodes = true
      *         mConvertLimits = true
      *         mConvertClusters = true
      *         mConvertLightIntensity = true
      *         mConvertPhotometricLProperties = true
      *         mConvertCameraClipPlanes = true
      *
      * The default configuration have been tested to give the best conversion results in the majority of the case. 
      * \remark Changing any of these values will have a direct impact on the whole scene behavior. 
      */
    struct KFbxUnitConversionOptions
    {
        //! This flag indicates whether or not to convert the nodes that do not inherit their parent's scale.
        bool mConvertRrsNodes;  

        //! This flag indicates whether or not to convert limits.
        bool mConvertLimits;

        //! This flag indicates whether or not to convert clusters.
        bool mConvertClusters;

        //! This flag indicates whether or not to convert the light intensity property.
        bool mConvertLightIntensity;	

        //! This flag indicates whether or not to convert photometric lights properties.
        bool mConvertPhotometricLProperties;

        //! This flag indicates whether or not to convert the cameras clip planes.
        bool mConvertCameraClipPlanes;
    };

    /** Constructor.
      * \param pScaleFactor The equivalent number of centimeters in the new system unit. 
      *                     For example, an inch unit uses a scale factor of 2.54.
      * \param pMultiplier  A multiplier factor of pScaleFactor.
      */
    explicit KFbxSystemUnit(double pScaleFactor, double pMultiplier = 1.0);

    /** Destructor.
      */
    ~KFbxSystemUnit();

    //! Predefined system unit for millimeters.
    static const KFbxSystemUnit mm;

    //! Predefined system unit for decimeters.
    static const KFbxSystemUnit dm;

    //! Predefined system unit for centimeters.
    static const KFbxSystemUnit cm;

    //! Predefined system unit for meters.
    static const KFbxSystemUnit m;

    //! Predefined system unit for kilometers.
    static const KFbxSystemUnit km;

    //! Predefined system unit for inches.
    static const KFbxSystemUnit Inch;

    //! Predefined system unit for feet.
    static const KFbxSystemUnit Foot;
    
    //! Predefined system unit for miles.
    static const KFbxSystemUnit Mile;

    //! Predefined system unit for yards.
    static const KFbxSystemUnit Yard;

    #define KFbxSystemUnit_sPredefinedUnitCount 9

    //! Points to a KFbxSystemUnit array to store the predefined system units. The array size is KFbxSystemUnit_sPredefinedUnitCount.
    static const KFbxSystemUnit *sPredefinedUnits;

    //! Stores the default conversion options.
    static const KFbxUnitConversionOptions DefaultConversionOptions;

    /** Converts a scene from its system units to this system unit.
      * \param pScene The scene to convert.
      * \param pOptions Conversion options, see:KFbxSystemUnit::KFbxUnitConversionOptions.
      */
    void ConvertScene( KFbxScene* pScene, const KFbxUnitConversionOptions& pOptions = DefaultConversionOptions ) const;

    /** Converts the child (or children) of the given node from the system unit to this system unit.
      * Unlike the ConvertScene() method, this method does not set the axis system 
      * of the scene to which the pRoot node belongs. It also does not adjust KFbxPoses
      * as they are not stored under the scene, and not under a particular node.
      * \param pRoot The given node.
      * \param pSrcUnit The source system unit.
      * \param pOptions Conversion options, see:KFbxSystemUnit::KFbxUnitConversionOptions.
      */
    void ConvertChildren( KFbxNode* pRoot, const KFbxSystemUnit& pSrcUnit, const KFbxUnitConversionOptions& pOptions = DefaultConversionOptions ) const;

    /** Converts a scene from its system unit to this system unit, using the specified 
      * Fbx_Root node. This method is provided for backwards compatibility only
      * and instead you should use ConvertScene( KFbxScene* , const KFbxUnitConversionOptions&  ) whenever possible.
      * \param pScene The scene to convert.
      * \param pFbxRoot The Fbx_Root node to use for conversion.
      * \param pOptions Conversion options, see:KFbxSystemUnit::KFbxUnitConversionOptions
      */
    void ConvertScene( KFbxScene* pScene, KFbxNode* pFbxRoot, const KFbxUnitConversionOptions& pOptions = DefaultConversionOptions ) const;

    /** Returns the system unit's scale factor, relative to centimeters.
      * This factor scales system unit values to centimeters. If you want to scale values to centimeters, use this value.
      * Ignore the "multiplier" (returned by GetMultiplier()) value. 
      * \return The the system unit's scale factor, relative to centimeters.
      */
    double GetScaleFactor() const;

    /** Returns a unit label for the current scale factor.
      * \param pAbbreviated If \c true, returns abbreviated string. 
      * \return The unit label for the current scale factor.
      */
    KString GetScaleFactorAsString(bool pAbbreviated = true) const;

    /** Returns a unit label for the current scale factor. 
      * The first letter of the label is in upper case and the label should be pluralized. 
      * \return The unit label for the current scale factor.
      */
    KString GetScaleFactorAsString_Plurial() const;

    /** Returns the multiplier factor of the system unit.
      */
    double GetMultiplier() const;

    /** Equivalence operator.
      * \param pOther Another system unit compared with this system unit.
      * \return \c True if equal, \c false otherwise.
      */   
    bool operator==(const KFbxSystemUnit& pOther) const;

    /** Non-equivalence operator.
      * \param pOther Another system unit compared with this system unit.
      * \return \c True if unequal, \c false otherwise.
      */  
    bool operator!=(const KFbxSystemUnit& pOther) const;

    /** Returns the conversion factor from this system unit to the target system unit, excluding the multiplier factor.
      * \param pTarget The target system unit.
      */
    double GetConversionFactorTo( const KFbxSystemUnit& pTarget ) const;

    /** Returns the conversion factor from the source system unit to this system unit, excluding the multiplier factor.
      * \param pSource The source system unit.
      */
    double GetConversionFactorFrom( const KFbxSystemUnit& pSource ) const;

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

protected:
    double mScaleFactor;
    double mMultiplier;

    void ApplyMultiplier(KFbxNode* pRoot, bool pSubChildrenOnly) const;
    void ConvertSTProperties(KArrayTemplate<KFbxNode*>& pNodes, double pConversionFactor) const;
    void ConvertSProperty(KArrayTemplate<KFbxNode*>& pNodes, double pConversionFactor) const;
    void ConvertAnimCurveNode(KArrayTemplate<KFbxAnimCurveNode*>& pFCurves, double pConversionFactor) const;
    double GetConversionFactor( double pTargetScaleFactor, double pSourceScaleFactor) const;
    void AdjustPivots(KFbxNode* pNode, double pConversionFactor, KFbxXMatrix& pOriginalGlobalM ) const;
    void AdjustLimits(KFbxNode* pNode, double pConversionFactor) const;
    void AdjustPoses(KFbxScene* pScene, double pConversionFactor) const;

    void AdjustCluster(KFbxNode* pNode, double pConversionFactor) const;
    void AdjustLightIntensity(KFbxNode* pNode, const double pConversionFactor) const;
    void AdjustPhotometricLightProperties(KFbxNode* pNode, const double pConversionFactor) const;
    void AdjustCameraClipPlanes(KFbxNode* pNode, const double pConversionFactor) const;

    void ConvertChildren( KFbxNode* pRoot, const KFbxSystemUnit& pSrcUnit, bool pSubChildrenOnly, const KFbxUnitConversionOptions& pOptions ) const;

    friend class KFbxGlobalSettings;
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXSYSTEMUNIT_H

