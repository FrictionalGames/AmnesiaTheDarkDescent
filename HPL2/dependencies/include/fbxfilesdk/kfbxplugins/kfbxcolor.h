/*!  \file kfbxcolor.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXCOLOR_H
#define FBXFILESDK_KFBXPLUGINS_KFBXCOLOR_H

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

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** Class to represent colors in RGBA format using doubles.
  * \nosubgrouping
  */
class KFBX_DLL KFbxColor
{
public:

    //! Constructor.
    KFbxColor();

	/** Constructor.
	  * \param pRed       The Red component value.
	  * \param pGreen     The Green component value.
	  * \param pBlue      The Blue component value.
	  * \param pAlpha     The alpha value of the color.
	  */
	KFbxColor(double pRed, double pGreen, double pBlue, double pAlpha = 1.0);

    //! Destructor.
    ~KFbxColor();

	/** Re-initialize the color object with their new values.
	  * \param pRed       The Red component value.
	  * \param pGreen     The Green component value.
	  * \param pBlue      The Blue component value.
	  * \param pAlpha     The alpha value of the color.
	  */
	void Set(double pRed, double pGreen, double pBlue, double pAlpha = 1.0);

	/** Indicate if all the members in the color objects are within their valid range.
	  * \return     \c true if all the members are within their valid range.
	  */
	bool IsValid() const;

    /** Accessors.
      * \param pIndex The index of the component to access.
      * \return The reference to the indexed component.
      * \remarks The pIndex parameter is not checked for values out of bounds.
      */
    double & operator[](int pIndex);

    /** Accessors.
      * \param pIndex The index of the component to access.
      * \return The reference to the indexed component.
      * \remarks The pIndex parameter is not checked for values out of bounds.
      */
    const double & operator[](int pIndex) const;

	/**
	  * \name Operators
	  */
	//@{

	/** Assignment operator.
	  * \param pColor KFbxColor to be copied.
	  */
    KFbxColor& operator=(const KFbxColor& pColor);

	/** Equality operator.
	  * \param pColor KFbxColor compared with this one.
	  * \return \c true if equal, \c false if unequal.
	  */
    bool operator==(const KFbxColor& pColor) const;

	/** Inequality operator.
	  * \param pColor KFbxColor compared with this one.
	  * \return \c true if unequal, \c false if equal.
	  */
    bool operator!=(const KFbxColor& pColor) const;
	//@}

	/**
	  * name Public Members
	  */
	//@{

	//! Valid range is from 0.0 to 1.0.
    double mRed;

	//! Valid range is from 0.0 to 1.0.
    double mGreen;

	//! Valid range is from 0.0 to 1.0.
    double mBlue;

	//! Valid range is from 0.0 to 1.0.
    double mAlpha;

	//@}
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXCOLOR_H

