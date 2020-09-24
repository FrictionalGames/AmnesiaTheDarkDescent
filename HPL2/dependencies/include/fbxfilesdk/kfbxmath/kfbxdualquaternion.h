/*!  \file kfbxdualquaternion.h
 */

#ifndef FBXFILESDK_KFBXMATH_KFBXDUALQUATERNION_H
#define FBXFILESDK_KFBXMATH_KFBXDUALQUATERNION_H

/**************************************************************************************

 Copyright (C) 2010 Autodesk, Inc. and/or its licensors.
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

#include <fbxfilesdk/kfbxmath/kfbxvector4.h>
#include <fbxfilesdk/kfbxmath/kfbxquaternion.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

	/**	FBX SDK dual quaternion class to represent rigid transformation, which is combined by two quaternions.
	  * A transformation is said to be rigid if it preserves relative distances and angles.
	  * That means rotation and translation.
	  * \nosubgrouping
	  */
class KFBX_DLL KFbxDualQuaternion
	{

	public:

		/**
		  * \name Constructors and Destructor
		  */
		//@{

		//! Constructor.
		KFbxDualQuaternion();

		/** Constructor.
		* \param pV1 KFbxQuaternion object.
		* \param pV2 KFbxQuaternion object.
		*/
		KFbxDualQuaternion(const KFbxQuaternion& pV1, const KFbxQuaternion& pV2);

		/** Copy constructor.
		  * \param pV KFbxQuaternion object copied to this one.
		  */
		KFbxDualQuaternion(const KFbxDualQuaternion& pV);

		/** Constructor.
		* \param pRotation     The rotation the dual quaternion is going to represent.
		* \param pTranslation  The translation the dual quaternion is going to represent.
		*/
		KFbxDualQuaternion(const KFbxQuaternion& pRotation, const KFbxVector4& pTranslation);

		/** Constructor.
		  * \param pX1     The X component of the first quaternion.
		  * \param pY1     The Y component of the first quaternion.
		  * \param pZ1     The Z component of the first quaternion.
		  * \param pW1     The W component of the first quaternion.
		  * \param pX2     The X component of the second quaternion.
		  * \param pY2     The Y component of the second quaternion.
		  * \param pZ2     The Z component of the second quaternion.
		  * \param pW2     The W component of the second quaternion.
		  */
		KFbxDualQuaternion(double pX1, double pY1, double pZ1, double pW1, double pX2, double pY2, double pZ2, double pW2);
		
		//! Destructor.
		~KFbxDualQuaternion();

		//@}

		/**
		* \name Access
		*/
		//@{

		/** Assignment operation.
		* \param pDualQuaternion KFbxDualQuaternion object assigned to this one.
		*/
		KFbxDualQuaternion& operator=(const KFbxDualQuaternion& pDualQuaternion);

		/** Set vector.
		* \param pX1     The X component of the first quaternion.
		* \param pY1     The Y component of the first quaternion.
		* \param pZ1     The Z component of the first quaternion.
		* \param pW1     The W component of the first quaternion.
		* \param pX2     The X component of the second quaternion.
		* \param pY2     The Y component of the second quaternion.
		* \param pZ2     The Z component of the second quaternion.
		* \param pW2     The W component of the second quaternion.
		*/
		void Set(double pX1, double pY1, double pZ1, double pW1, double pX2, double pY2, double pZ2, double pW2);

		/** Get the first quaternion of the dual quaternion.
		* \return The first quaternion of the dual quaternion.
		*/
		KFbxQuaternion& GetFirstQuaternion();

		/** Get the second quaternion of the dual quaternion.
		* \return The second quaternion of the dual quaternion.
		*/
		KFbxQuaternion& GetSecondQuaternion();

		/** Get the first quaternion of the dual quaternion.
		* \return The first quaternion of the dual quaternion.
		*/
		const KFbxQuaternion& GetFirstQuaternion() const;

		/** Get the second quaternion of the dual quaternion.
		* \return The second quaternion of the dual quaternion.
		*/
		const KFbxQuaternion& GetSecondQuaternion() const;

		/** Get the rotation part from the dual quaternion.
		* \return KFbxQuaternion object to represent rotation.
		*/
		KFbxQuaternion GetRotation() const;

		/** Get the translation part from the dual quaternion.
		* \return KFbxVector4 object to represent translation.
		* \remarks A dual quaternion can represent rotation first then translation or translation first then rotation.
		* \Since in most DCC,rotation first then translation, so we extract translation in this order.
		*/
		KFbxVector4 GetTranslation() const;
		
		//@}

		/**
		  * \name Scalar Operations
		  */
		//@{
		
		/** Add a value to all vector components.
		  * \param pValue     The value to add to each component of the vector.
		  * \return           New vector.
		  * \remarks          The passed value is not checked.
		  */
		KFbxDualQuaternion operator+(double pValue) const;

		/** Subtract a value from all vector components.
		  * \param pValue     The value to subtract from each component of the vector.
		  * \return           New vector.
		  * \remarks          The passed value is not checked.
		  */
		KFbxDualQuaternion operator-(double pValue) const;

		/** Multiply all vector components by a value.
		  * \param pValue     The value multiplying each component of the vector.
		  * \return           New vector.
		  * \remarks          The passed value is not checked.
		  */
		KFbxDualQuaternion operator*(double pValue) const;

		/**	Divide all vector components by a value.
		  * \param pValue     The value dividing each component of the vector.
		  * \return           New vector.
		  * \remarks          The passed value is not checked.
		  */
		KFbxDualQuaternion operator/(double pValue) const;

		/** Add a value to all vector components.
		  * \param pValue     The value to add to each component of the vector.
		  * \return           The result of adding pValue to each component of the vector, replacing this dual quaternion.
		  * \remarks          The passed value is not checked.
		  */
		KFbxDualQuaternion& operator+=(double pValue);

		/** Subtract a value from all vector components.
		  * \param pValue     The value to subtract from each component of the vector.
		  * \return           The result of subtracting pValue from each component of the vector, replacing this dual quaternion.
		  * \remarks          The passed value is not checked.
		  */
		KFbxDualQuaternion& operator-=(double pValue);

		/** Multiply a value to all vector elements.
		  * \param pValue     The value multiplying each component of the vector.
		  * \return           The result of multiplying each component of the vector by pValue, replacing this dual quaternion.
		  * \remarks          The passed value is not checked.
		  */
		KFbxDualQuaternion& operator*=(double pValue);

		/**	Divide all vector elements by a value.
		  * \param pValue     The value dividing each component of the vector.
		  * \return           The result of dividing each component of the vector by pValue, replacing this dual quaternion.
		  * \remarks          The passed value is not checked.
		  */
		KFbxDualQuaternion& operator/=(double pValue);
		
		//@}

		/**
		  * \name Vector Operations
		  */
		//@{

		/**	Unary minus operator.
		  * \return      A dual quaternion where each component is multiplied by -1.
		  */
		KFbxDualQuaternion operator-() const;
		
		/** Add two vectors together.
		  * \param pDualQuaternion     Dual quaternion to add.
		  * \return                The dual quaternion v' = this + pDualQuaternion.
		  * \remarks               The values in pDualQuaternion are not checked.
		  */
		KFbxDualQuaternion operator+(const KFbxDualQuaternion& pDualQuaternion) const;

		/** Subtract a quaternion from another quaternion.
		  * \param pDualQuaternion     Dual quaternion to subtract.
		  * \return                The dual quaternion v' = this - pDualQuaternion.
		  * \remarks               The values in pDualQuaternion are not checked.
		  */
		KFbxDualQuaternion operator-(const KFbxDualQuaternion& pDualQuaternion) const;

		/** Memberwise multiplication of two vectors.
		  * \param pDualQuaternion     Multiplying dual quaternion.
		  * \return                The dual quaternion v' = this * pQuaternion.
		  * \remarks               The values in pDualQuaternion are not checked.
		  */
		KFbxDualQuaternion operator*(const KFbxDualQuaternion& pDualQuaternion) const;

		/** Memberwise division of a dual quaternion with another dual quaternion.
		  * \param pDualQuaternion     Dividing dual quaternion.
		  * \return                The dual quaternion v' = this / pQuaternion.
		  * \remarks               The values in pDualQuaternion are not checked.
		  */
		KFbxDualQuaternion operator/(const KFbxDualQuaternion& pDualQuaternion) const;

		/** Add two quaternions together.
		  * \param pDualQuaternion     Dual quaternion to add.
		  * \return                The dual quaternion v' = this + pQuaternion, replacing this dual quaternion.
		  * \remarks               The values in pDualQuaternion are not checked.
		  */
		KFbxDualQuaternion& operator+=(const KFbxDualQuaternion& pDualQuaternion);

		/** Subtract a dual quaternion from another vector.
		  * \param pDualQuaternion     Dual quaternion to subtract.
		  * \return                The dual quaternion v' = this - pQuaternion, replacing this dual quaternion.
		  * \remarks               The values in pDualQuaternion are not checked.
		  */
		KFbxDualQuaternion& operator-=(const KFbxDualQuaternion& pDualQuaternion);

		/** Memberwise multiplication of two quaternions.
		  * \param pDualQuaternion     Multiplying dual quaternion.
		  * \return                The dual quaternion v' = this * pQuaternion, replacing this dual quaternion.
		  * \remarks               The values in pDualQuaternion are not checked.
		  */
		KFbxDualQuaternion& operator*=(const KFbxDualQuaternion& pDualQuaternion);
		
		/** Memberwise division of a dual quaternion by another dual quaternion.
		  * \param pDualQuaternion     Dividing dual quaternion.
		  * \return                The dual quaternion v' = this / pQuaternion, replacing this dual quaternion.
		  * \remarks               The values in pDualQuaternion are not checked.
		  */
		KFbxDualQuaternion& operator/=(const KFbxDualQuaternion& pDualQuaternion);

		/** Multiplication of a dual quaternion by a KFbxVector4.
		* \param pVector     The KFbxVector4 to multiply with.
		* \return            The dual quaternion v' = KFbxDualQuaternion(mQ1, (mQ1 * pVector) + mQ2).
		* \remarks           The values in pDualQuaternion are not checked.
		*/
		KFbxDualQuaternion operator*(const KFbxVector4 pVector) const;

		/** Return dual quaternion product.
		* \param pDualQuaternion     product dual quaternion.
		* \return                This dual quaternion replace with the dual quaternion product.
		*/
		KFbxDualQuaternion Product(const KFbxDualQuaternion& pDualQuaternion) const;

		/** Normalize the dual quaternion, length set to 1.
		*/
		void           Normalize();

		/** Calculate the dual quaternion's inverse.
		* \return      The inverse of this dual quaternion. 
		*/
		void           Inverse();

		/** Deform a point by this dual quaternion.
		* \return      The inverse of this quaternion. 
		*/
		KFbxVector4    Deform(KFbxVector4& pPoint);

		//@}

		/**
		* \name Conjugate Operations
		* \brief Dual quaternion has three types of conjugate.
		*/
		//@{

		/** Conjugate both quaternions of this dual quaternion.
		*/
		void           Conjugate();

		/** Conjugate in dual space.
		*/
		void           Dual();

		/** Conjugate both quaternions of this dual quaternion in dual space.
		*/
		void           DualConjugate();

		//@}

		/**
		  * \name Boolean Operations
		  */
		//@{

		/**	Equivalence operator.
		  * \param pV     The quaternion to be compared to this quaternion.
		  * \return       \c true  if the two quaternions are equal (each element is within a 1.0e-6 tolerance), \c false  otherwise.
		  */
		bool operator==(const KFbxDualQuaternion & pV) const;
		
		/**	Non equivalence operator.
		  * \param pV     The quaternion to be compared to \e this.
		  * \return       \c  false if the two quaternions are equal (each element is within a 1.0e-6 tolerance), \c true  otherwise.
		  */
		bool operator!=(const KFbxDualQuaternion & pV) const;
		
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
		private:
			/**
			* \name Private data members
			*/
			//@{

			/**	The first quaternion of this dual quaternion.
			*/
			KFbxQuaternion mQ1;

			/**	The second quaternion of this dual quaternion.
			*/
			KFbxQuaternion mQ2;

			//@}
#endif //doxygen

	};

	//inline EFbxType FbxTypeOf( KFbxDualQuaternion const &pItem )	{ return eDOUBLE8; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXMATH_KFBXDUALQUATERNION_H

