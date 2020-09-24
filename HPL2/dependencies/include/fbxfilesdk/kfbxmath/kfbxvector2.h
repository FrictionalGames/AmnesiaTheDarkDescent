/*!  \file kfbxvector2.h
 */

#ifndef FBXFILESDK_KFBXMATH_KFBXVECTOR2_H
#define FBXFILESDK_KFBXMATH_KFBXVECTOR2_H

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

#include <fbxfilesdk/kfbxplugins/kfbxtypes.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

	/**	FBX SDK 2-elements vector class.
  * \nosubgrouping
  */
	class KFBX_DLL KFbxVector2 : public fbxDouble2
	{

	public:

	/**
	  * \name Constructors and Destructor
	  */
	//@{

	//! Constructor.
	KFbxVector2();

	/** Copy constructor.
	  * \param pVector2 The vector copied to this one.
	  */
	KFbxVector2(const KFbxVector2& pVector2);

	/** Constructor.
	  *	\param pX X component.
	  *	\param pY Y component.
	  */
	KFbxVector2(double pX, double pY);
	
	//! Destructor.
	~KFbxVector2();

	//@}

	/**
	  * \name Access
	  */
	//@{
	
	/** Assignment operation.
	  * \param pVector2   The vector assigned to this one.
	  * \return           This vector after assignment. 
	  */
	KFbxVector2& operator=(const KFbxVector2& pVector2);
	
	/** Accessor.
	  * \param pIndex The index of the component to access.
	  * \return The reference to the indexed component.
	  * \remarks          The pIndex parameter is not checked for values out of bounds. The valid values are 0 and 1.
	  */
	double& operator[](int pIndex);

    /** Accessor.
    * \param pIndex The index of the component to access.
    * \return The const reference to the indexed component.
    * \remarks          The pIndex parameter is not checked for values out of bounds. The valid values are 0 and 1.
    */
    const double& operator[](int pIndex) const;

	/** Get a vector element.
	  * \param pIndex The index of the component to access.
	  * \return The value of the indexed component.
	  * \remarks          The pIndex parameter is not checked for values out of bounds. The valid values are 0 and 1.
	  */
	double GetAt(int pIndex) const;

	/** Set a vector element.
      * \param pIndex The index of the component to set.
	  * \param pValue The new value to set the component.
	  * \remarks          The pIndex parameter is not checked for values out of bounds. The valid values are 0 and 1.
	  */
	void SetAt(int pIndex, double pValue);

	/** Set vector.
	  * \param pX The X component value.
  	  * \param pY The Y component value.
	  */
	void Set(double pX, double pY);
	
	//@}

	/**
	  * \name Scalar Operations
	  */
	//@{

	/** Add a value to all vector components.
	  * \param pValue The value to add to each component of the vector.
	  * \return           A new vector with the result of adding pValue to each component of this vector.
	  * \remarks          The pValue parameter is not checked.
	  */
	KFbxVector2 operator+(double pValue) const;

	/** Subtract a value from all vector components.
	  * \param pValue The value to subtract from each component of the vector.
	  * \return           A new vector with the result of subtracting pValue from each component of this vector.
	  * \remarks          The pValue parameter is not checked.
	  */
	KFbxVector2 operator-(double pValue) const;

	/** Multiply a value to all vector components.
	  * \param pValue The value multiplying each component of the vector.
	  * \return           A new vector with the result of multiplying each component of this vector by pValue.
	  * \remarks          The pValue parameter is not checked.
	  */
	KFbxVector2 operator*(double pValue) const;

	/**	Divide all vector components by a value.
	  * \param pValue The value dividing each component of the vector.
	  * \return           A new vector with the result of dividing each component of this vector by pValue.
	  * \remarks          The pValue parameter is not checked.
	  */
	KFbxVector2 operator/(double pValue) const;

	/** Add a value to all vector components.
	  * \param pValue The value to add to each component of the vector.
	  * \return           The result of adding pValue to each component of this vector, replacing this vector.
	  * \remarks          The pValue parameter is not checked.
	  */
	KFbxVector2& operator+=(double pValue);

	/** Subtract a value from all vector components.
	  * \param pValue The value to subtract from each component of the vector.
	  * \return           The result of subtracting pValue from each component of this vector, replacing this vector.
  	  * \remarks          The pValue parameter is not checked.
	  */
	KFbxVector2& operator-=(double pValue);

	/** Multiply a value to all vector elements.
	  * \param pValue The value multiplying each component of the vector.
	  * \return           The result of multiplying each component of this vector by pValue, replacing this vector.
	  * \remarks          The pValue parameter is not checked.
	  */
	KFbxVector2& operator*=(double pValue);

	/**	Divide all vector elements by a value.
	  * \param pValue The value dividing each component of the vector.
	  * \return           The result of multiplying each component of this vector by pValue, replacing this vector.
	  * \remarks          The pValue parameter is not checked.
	  */
	KFbxVector2& operator/=(double pValue);
	
	//@}

	/**
	  * \name Vector Operations
	  */
	//@{

	/**	Unary minus operator.
	  * \return The vector that is the negation of \c this.
	  */
	KFbxVector2 operator-() const;
	
	/** Add two vectors together.
	  * \param pVector Vector to add.
	  * \return            The result of this vector + pVector.
	  * \remarks           The values in pVector are not checked.
	  */
	KFbxVector2 operator+(const KFbxVector2& pVector) const;

	/** Subtract a vector from another vector.
	  * \param pVector Vector to subtract.
	  * \return            The result of this vector - pVector.
	  * \remarks           The values in pVector are not checked.
	  */
	KFbxVector2 operator-(const KFbxVector2& pVector) const;

	/** Memberwise multiplication of two vectors.
	  * \param pVector      Multiplying vector.
	  * \return             The result of this vector * pVector.
	  * \remarks            The values in pVector are not checked.
	  */
	KFbxVector2 operator*(const KFbxVector2& pVector) const;

	/** Memberwise division of a vector with another vector.
	  * \param pVector     Dividing vector.
	  * \return            The result of this vector / pVector.
	  * \remarks           The values in pVector are not checked.
	  */
	KFbxVector2 operator/(const KFbxVector2& pVector) const;

	/** Add two vectors together.
	  * \param pVector Vector to add.
	  * \return            The result of this vector + pVector, replacing this vector.
	  * \remarks           The values in pVector are not checked.
	  */
	KFbxVector2& operator+=(const KFbxVector2& pVector);

	/** Subtract a vector from another vector.
	  * \param pVector Vector to subtract.
	  * \return            The result of this vector - pVector, replacing this vector.
	  * \remarks           The values in pVector are not checked.
	  */
	KFbxVector2& operator-=(const KFbxVector2& pVector);

	/** Memberwise multiplication of two vectors.
	  * \param pVector Multiplying vector.
	  * \return            The result of this vector * pVector, replacing this vector.
	  * \remarks           The values in pVector are not checked.
	  */
	KFbxVector2& operator*=(const KFbxVector2& pVector);
	
	/** Memberwise division of a vector with another vector.
	  * \param pVector Dividing vector.
	  * \remarks           The values in pVector are not checked.
	  * \return            The result of this vector / pVector, replacing this vector.
	  * \remarks           The values in pVector are not checked.
	  */
	KFbxVector2& operator/=(const KFbxVector2& pVector);

	/** Calculate the dot product of two vectors.
	  * \param pVector The second vector.
	  * \return The dot product value.
      * \remarks          pVector is considered a XYZ vector with fourth weight element, so only the first 3 elements are considered.
	  */
	double DotProduct(const KFbxVector2& pVector) const;
	
	//@}

	/**
	  * \name Boolean Operations
	  */
	//@{

	/**	Equivalence operator.
	  * \param pVector The vector to be compared to \e this.
	  * \return            \c true if the two vectors are equal (each element is within a 1.0e-6 tolerance), \c false otherwise.
	  */
	bool operator==(const KFbxVector2 & pVector) const;
	
	/**	Non-equivalence operator.
	  * \param pVector The vector to be compared to \e this.
	  * \return            \c false if the two vectors are equal (each element is within a 1.0e-6 tolerance), \c true otherwise.
	  */
	bool operator!=(const KFbxVector2 & pVector) const;
	
	//@}

	/**
	  * \name Length
	  */
	//@{

	/** Get the vector's length.
	  * \return The mathematical length of the vector.
	  */
	double Length() const;

	/** Get the vector's length squared.
	  * \return The mathematical square length of the vector.
	  */
	double SquareLength() const;

	/** Find the distance between 2 vectors.
	  * \param pVector The second vector.
	  * \return The mathematical distance between the two vectors.
	  */
	double Distance(const KFbxVector2& pVector) const;

	//! Normalize the vector, length set to 1.
	void Normalize();

	//@}

	/**
	  * \name Casting
	  */
	//@{

	//! Cast the vector in a double pointer.
	operator double* ();

    //! Cast the vector in a const double pointer.
    operator const double* () const;

	//@}

    // Fix value like 1.#IND, 1.#INF, nan, and inf
    void FixWeirdValue();

	};

	typedef  class KFBX_DLL KArrayTemplate<KFbxVector2>				KArrayKFbxVector2;

	inline EFbxType FbxTypeOf( KFbxVector2 const &pItem )		{ return eDOUBLE2;	}

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXMATH_KFBXVECTOR2_H

