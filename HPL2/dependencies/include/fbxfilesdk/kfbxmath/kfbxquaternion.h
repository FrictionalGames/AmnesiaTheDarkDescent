/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxquaternion.h
 */

#ifndef FBXFILESDK_KFBXMATH_KFBXQUATERNION_H
#define FBXFILESDK_KFBXMATH_KFBXQUATERNION_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxmath/kfbxvector4.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/**	FBX SDK quaternion class.
 * \nosubgrouping
 * Quaternions form a four-dimensional normed division algebra over the real numbers. 
 * It is for calculations involving three-dimensional rotations. 
 */
class KFBX_DLL KFbxQuaternion : public fbxDouble4
	{

	public:

		/**
		  * \name Constructors and Destructor
		  */
		//@{

		//! Constructor.
		KFbxQuaternion();

		/** Copy constructor.
		  * \param pV KFbxQuaternion object copied to this one.
		  */
		KFbxQuaternion(const KFbxQuaternion& pV);

		/** Constructor.
		  * \param pX     The X component.
		  * \param pY     The Y component.
		  * \param pZ     The Z component.
		  * \param pW     The W component.
		  */
		KFbxQuaternion(double pX, double pY, double pZ, double pW = 1.0);
		
		//! Destructor.
		~KFbxQuaternion();

		//@}

		/**
		  * \name Access
		  */
		//@{
		
		/** Assignment operation.
		  * \param pQuaternion KFbxQuaternion object assigned to this one.
		  */
		KFbxQuaternion& operator=(const KFbxQuaternion& pQuaternion);
		
		/** Accessor.
		  * \param pIndex     The index of the component to access.
		  * \return           The reference to the indexed component.
		  * \remarks          The index parameter is not checked for values out of bounds. The valid range is [0,3].
		  */
		double& operator[](int pIndex);

        /** Accessor.
        * \param pIndex     The index of the component to access.
        * \return           The const reference to the indexed component.
        * \remarks          The index parameter is not checked for values out of bounds. The valid range is [0,3].
        */
        const double& operator[](int pIndex) const;

		/** Get a vector element.
		  * \param pIndex     The index of the component to access.
		  * \return           The value of the indexed component.
		  * \remarks          The index parameter is not checked for values out of bounds. The valid range is [0,3].
		  */
		double GetAt(int pIndex) const;

		/** Set a vector element.
		  * \param pIndex     The index of the component to set.
		  * \param pValue     The new value to set the component.
		  * \remarks          The index parameter is not checked for values out of bounds. The valid range is [0,3].
		  */
		void SetAt(int pIndex, double pValue);

		/** Set vector.
		  * \param pX     The X component value.
  		  * \param pY     The Y component value.
		  * \param pZ     The Z component value.
		  * \param pW     The W component value.
		  */
		void Set(double pX, double pY, double pZ, double pW = 1.0);
		
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
		KFbxQuaternion operator+(double pValue) const;

		/** Subtract a value from all vector components.
		  * \param pValue     The value to subtract from each component of the vector.
		  * \return           New vector.
		  * \remarks          The passed value is not checked.
		  */
		KFbxQuaternion operator-(double pValue) const;

		/** Multiply all vector components by a value.
		  * \param pValue     The value multiplying each component of the vector.
		  * \return           New vector.
		  * \remarks          The passed value is not checked.
		  */
		KFbxQuaternion operator*(double pValue) const;

		/**	Divide all vector components by a value.
		  * \param pValue     The value dividing each component of the vector.
		  * \return           New vector.
		  * \remarks          The passed value is not checked.
		  */
		KFbxQuaternion operator/(double pValue) const;

		/** Add a value to all vector components.
		  * \param pValue     The value to add to each component of the vector.
		  * \return           The result of adding pValue to each component of the vector, replacing this quaternion.
		  * \remarks          The passed value is not checked.
		  */
		KFbxQuaternion& operator+=(double pValue);

		/** Subtract a value from all vector components.
		  * \param pValue     The value to subtract from each component of the vector.
		  * \return           The result of subtracting pValue from each component of the vector, replacing this quaternion.
		  * \remarks          The passed value is not checked.
		  */
		KFbxQuaternion& operator-=(double pValue);

		/** Multiply a value to all vector elements.
		  * \param pValue     The value multiplying each component of the vector.
		  * \return           The result of multiplying each component of the vector by pValue, replacing this quaternion.
		  * \remarks          The passed value is not checked.
		  */
		KFbxQuaternion& operator*=(double pValue);

		/**	Divide all vector elements by a value.
		  * \param pValue     The value dividing each component of the vector.
		  * \return           The result of dividing each component of the vector by pValue, replacing this quaternion.
		  * \remarks          The passed value is not checked.
		  */
		KFbxQuaternion& operator/=(double pValue);
		
		//@}

		/**
		  * \name Vector Operations
		  */
		//@{

		/**	Unary minus operator.
		  * \return      A quaternion where each component is multiplied by -1.
		  */
		KFbxQuaternion operator-() const;
		
		/** Add two vectors together.
		  * \param pQuaternion     Quaternion to add.
		  * \return                The quaternion v' = this + pQuaternion.
		  * \remarks               The values in pQuaternion are not checked.
		  */
		KFbxQuaternion operator+(const KFbxQuaternion& pQuaternion) const;

		/** Subtract a quaternion from another quaternion.
		  * \param pQuaternion     Quaternion to subtract.
		  * \return                The quaternion v' = this - pQuaternion.
		  * \remarks               The values in pQuaternion are not checked.
		  */
		KFbxQuaternion operator-(const KFbxQuaternion& pQuaternion) const;

		/** Memberwise multiplication of two vectors.
		  * \param pQuaternion     Multiplying quaternion.
		  * \return                The quaternion v' = this * pQuaternion.
		  * \remarks               The values in pQuaternion are not checked.
		  */
		KFbxQuaternion operator*(const KFbxQuaternion& pQuaternion) const;

		/** Memberwise division of a quaternion with another quaternion.
		  * \param pQuaternion     Dividing quaternion.
		  * \return                The quaternion v' = this / pQuaternion.
		  * \remarks               The values in pQuaternion are not checked.
		  */
		KFbxQuaternion operator/(const KFbxQuaternion& pQuaternion) const;

		/** Add two quaternions together.
		  * \param pQuaternion     Quaternion to add.
		  * \return                The quaternion v' = this + pQuaternion, replacing this quaternion.
		  * \remarks               The values in pQuaternion are not checked.
		  */
		KFbxQuaternion& operator+=(const KFbxQuaternion& pQuaternion);

		/** Subtract a quaternion from another vector.
		  * \param pQuaternion     Quaternion to subtract.
		  * \return                The quaternion v' = this - pQuaternion, replacing this quaternion.
		  * \remarks               The values in pQuaternion are not checked.
		  */
		KFbxQuaternion& operator-=(const KFbxQuaternion& pQuaternion);

		/** Memberwise multiplication of two quaternions.
		  * \param pQuaternion     Multiplying quaternion.
		  * \return                The quaternion v' = this * pQuaternion, replacing this quaternion.
		  * \remarks               The values in pQuaternion are not checked.
		  */
		KFbxQuaternion& operator*=(const KFbxQuaternion& pQuaternion);
		
		/** Memberwise division of a quaternion by another quaternion.
		  * \param pQuaternion     Dividing quaternion.
		  * \return                The quaternion v' = this / pQuaternion, replacing this quaternion.
		  * \remarks               The values in pQuaternion are not checked.
		  */
		KFbxQuaternion& operator/=(const KFbxQuaternion& pQuaternion);

		/** Return quaternion product.
		  * \param pQuaternion     product quaternion.
		  * \return                This quaternion replace with the quaternion product.
		  */
		KFbxQuaternion Product(const KFbxQuaternion& pQuaternion) const;

		/** Return quaternion dot product.
		* \param pQuaternion     Dot product quaternion.
		* \return                The dot product of this quaternion and pQuaternion.
		*/
		double DotProduct(const KFbxQuaternion& pQuaternion) const;

		/** Normalize the quaternion, length set to 1.
		  */
		void Normalize();

		/** Calculate the quaternion conjugate.
		  * \return      The conjugate of this quaternion. Set XYZ as -XYZ.
		  */
		void Conjugate();

		/** Calculate the length of the quaternion.
		* \return The norm of the quaternion.
		*/
		double Length();

		/** Calculate the inverse of the quaternion.
		  * \return      The inverse of this quaternion. 
		  * \remarks     If the quaternion is normalized, then its inverse is equal to its conjugate.
		  */
		void Inverse();

        /** Create a Quaternion equivalent to the supplied Euler XYZ in spherical coordinate.
          * \param pEuler   The Euler XYZ angle.
          */
		void ComposeSphericalXYZ(const KFbxVector4 pEuler);

        /** Create an Euler XYZ equivalent to the current quaternion.
          * \return     The Euler XYZ equivalent to the current quaternion in spherical coordinate.
          */
		KFbxVector4 DecomposeSphericalXYZ() const;
		
		//@}

		/**
		  * \name Boolean Operations
		  */
		//@{

		/**	Equivalence operator.
		  * \param pV     The quaternion to be compared to this quaternion.
		  * \return       \c true  if the two quaternions are equal (each element is within a 1.0e-6 tolerance), \c false  otherwise.
		  */
		bool operator==(const KFbxQuaternion & pV) const;
		
		/**	Non equivalence operator.
		  * \param pV     The quaternion to be compared to \e this.
		  * \return       \c  false if the two quaternions are equal (each element is within a 1.0e-6 tolerance), \c true  otherwise.
		  */
		bool operator!=(const KFbxQuaternion & pV) const;
		
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


	///////////////////////////////////////////////////////////////////////////////
	//
	//  WARNING!
	//
	//	Anything beyond these lines may not be documented accurately and is 
	// 	subject to change without notice.
	//
	///////////////////////////////////////////////////////////////////////////////

	#ifndef DOXYGEN_SHOULD_SKIP_THIS	
		void GetKFbxQuaternionFromPositionToPosition(const KFbxVector4 &pP0, const KFbxVector4 &pP1);
	#endif //doxygen

	};

	inline EFbxType FbxTypeOf( KFbxQuaternion const &pItem )	{ return eDOUBLE4; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXMATH_KFBXQUATERNION_H

