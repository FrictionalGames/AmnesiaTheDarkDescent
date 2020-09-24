/*!  \file kfbxxmatrix.h
 */
 
#ifndef FBXFILESDK_KFBXMATH_KFBXXMATRIX_H
#define FBXFILESDK_KFBXMATH_KFBXXMATRIX_H

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

#include <fbxfilesdk/kfbxplugins/kfbxtypes.h>
#include <fbxfilesdk/kfbxmath/kfbxquaternion.h>
#include <fbxfilesdk/kfbxmath/kfbxvector4.h>
#include <fbxfilesdk/kfbxmath/kfbxtransformation.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KgeAMatrix;

	/**	FBX SDK affine matrix class.
	  * \nosubgrouping
	  * Matrices are defined using the Column Major scheme. When a KFbxXMatrix represents a transformation (translation, rotation and scale), 
	  * the last row of the matrix represents the translation part of the transformation.
      *
      * \remarks It is important to realize that an affine matrix must respect a certain structure.  To be sure the structure is respected,
      * use SetT, SetR, SetS, SetQ, SetTRS or SetTQS.  If by mistake bad data is entered in this affine matrix, some functions such as 
      * Inverse() will yield wrong results.  If a matrix is needed to hold values that aren't associate with an affine matrix, please use KFbxMatrix instead.
	  */
	class KFBX_DLL KFbxXMatrix : public fbxDouble44
	{
	public:

		/**
		  * \name Constructors and Destructor
		  */
		//@{

		//! Constructor.
		KFbxXMatrix();

		/** Copy constructor.
		  * \param pXMatrix KFbxXMatrix copied to this one.
		  */
		KFbxXMatrix(const KFbxXMatrix& pXMatrix);

		/** Constructor.
		  *	\param pT     Translation vector.
		  *	\param pR     Euler rotation vector.
		  *	\param pS     Scale vector.
		  */
		KFbxXMatrix(const KFbxVector4& pT,
					const KFbxVector4& pR,
					const KFbxVector4& pS);
			
		//! Destructor.
		~KFbxXMatrix();
			
		//@}

		/**
		  * \name Access
		  */
		//@{

		/** Retrieve matrix element.
		  *	\param pY     Row index.
		  *	\param pX     Column index.
		  * \return       Cell [ pX, pY ] value.
		  */
		double Get(int pY, int pX) const;

		/** Extract translation vector.
		  * \return     Translation vector.
		  */
		KFbxVector4 GetT() const;

		/** Extract rotation vector.
		  * \return     Rotation vector.
		  * \remarks    The returned rotation vector is in Euler angle and the rotation order is XYZ.
		  */
		KFbxVector4 GetR() const;

		/** Extract quaternion vector.
		  * \return     Quaternion vector.
		  */
		KFbxQuaternion GetQ() const;

		/** Extract scale vector.
		  * \return     Scale vector.
		  */
		KFbxVector4 GetS() const;

		/** Extract a row vector.
		  *	\param pY     Row index.
		  * \return       The row vector.
		  */
		KFbxVector4 GetRow(int pY) const;

		/** Extract a column vector.
		  *	\param pX     Column index.
		  * \return       The column vector.
		  */
		KFbxVector4 GetColumn(int pX) const;

		//! Set matrix to identity.
		void SetIdentity();
		
		/** Set matrix's translation.
		  * \param pT     Translation vector.
		  */
		void SetT(const KFbxVector4& pT);

		/** Set matrix's Euler rotation.
		  * \param pR     X, Y and Z rotation values expressed as a vector.
		  * \remarks      The rotation transform is constructed in rotation order XYZ.
		  */
		void SetR(const KFbxVector4& pR);

		/** Set matrix's quaternion.
		  * \param pQ     The new quaternion.
		  */
		void SetQ(const KFbxQuaternion& pQ);

		/** Set matrix's scale.
		  * \param pS     X, Y and Z scaling factors expressed as a vector.
		  */
		void SetS(const KFbxVector4& pS);

		/** Set matrix.
		  *	\param pT     Translation vector.
		  *	\param pR     Rotation vector.
		  *	\param pS     Scale vector.
		  */
		void SetTRS(const KFbxVector4& pT,
					const KFbxVector4& pR,
					const KFbxVector4& pS);

		/** Set matrix.
		  *	\param pT     Translation vector.
		  *	\param pQ     Quaternion vector.
		  *	\param pS     Scale vector.
		  */
		void SetTQS(const KFbxVector4& pT,
					const KFbxQuaternion& pQ,
					const KFbxVector4& pS);

		/** Assignment operator.
		  * \param pM KFbxXMatrix assigned to this one.
		  */
		KFbxXMatrix& operator=(const KFbxXMatrix& pM);
		
		//@}

		/**
		  * \name Scalar Operations
		  */
		//@{

		/** Multiply matrix by a scalar value.
		  * \param pValue     Scalar value.
		  * \return           The scaled matrix.
		  * \remarks          The passed value is not checked. 
		  *                   This operator operates on the first three rows and columns of the matrix. 
		  *                   So only the rotation and scaling are scaled, not the translation part.
		  *                   After operation, the translation vector will be set as (0,0,0,1);
		  */
		KFbxXMatrix operator*(double pValue) const;

		/** Divide matrix by a scalar value.
		  * \param pValue     Scalar value.
		  * \return           The divided matrix.
		  * \remarks          The passed value is not checked.
		  *                   This operator operates on the first three rows and columns of the matrix. 
		  *                   So only the rotation and scaling are scaled, not the translation part. 
		  *                   After operation, the translation vector will be set as (0,0,0,1);
		  */
		KFbxXMatrix operator/(double pValue) const;

		/** Multiply matrix by a scalar value.
		  * \param pValue     Scalar value.
		  * \return           \e this updated with the result of the multiplication.
		  * \remarks          The passed value is not checked.
		  *                   This operator operates on the first three rows and columns of the matrix. 
		  *                   So only the rotation and scaling are scaled, not the translation part. 
		  *                   After operation, the translation vector will keep original value.
		  */
		KFbxXMatrix& operator*=(double pValue);

		/** Divide matrix by a scalar value.
		  * \param pValue     Scalar value.
		  * \return           \e this updated with the result of the division.
		  * \remarks          The passed value is not checked.
		  *                   This operator operates on the first three rows and columns of the matrix. 
		  *                   So only the rotation and scaling are scaled, not the translation part. 
		  *                   After operation, the translation vector will keep original value.
		  */
		KFbxXMatrix& operator/=(double pValue);

		//@}

		/**
		  * \name Vector Operations
		  */
		//@{

		/** Multiply matrix by a translation vector.
		  * \param pVector4     Translation vector.
		  * \return             t' = M * t
		  */
		KFbxVector4 MultT(const KFbxVector4& pVector4) const;

		/** Multiply matrix by an Euler rotation vector.
		  * \param pVector4     Euler Rotation vector.
		  * \return             r' = M * r
		  */
		KFbxVector4 MultR(const KFbxVector4& pVector4) const;
		
		/** Multiply matrix by a quaternion.
		  * \param pQuaternion     Rotation value.
		  * \return                q' = M * q
		  */
		KFbxQuaternion MultQ(const KFbxQuaternion& pQuaternion) const;

		/** Multiply matrix by a scale vector.
		  * \param pVector4     Scaling vector.
		  * \return             s' = M * s
		  */
		KFbxVector4 MultS(const KFbxVector4& pVector4) const;
			
		//@}

		/**
		  * \name Matrix Operations
		  */
		//@{	

		/**	Unary minus operator.
		  * \return     A matrix where each element is multiplied by -1.
		  */
		KFbxXMatrix operator-() const;
		
		/** Multiply two matrices together.
		  * \param pXMatrix     A Matrix.
		  * \return             this * pMatrix.
		  * \remarks            Transformations are pre-multiplied.
		  *  That means to scale, then rotate, and then translate a vector V, the transform should be T * R * S * V. \n
		  *  Below is an example of code that shows how to construct rotation transform in XYZ rotation order.
		  *  \code
		  *  KFbxXMatrix lRotateXM, lRotateYM, lRotateZM, lRotateXYZM, lRotateM;
		  *  // Construct rotation matrix around X, Y and Z axises separately and then combine them.
		  *  KFbxVector4 lRotateX(10, 0, 0);
		  *  KFbxVector4 lRotateY(0, 10, 0);
		  *  KFbxVector4 lRotateZ(0, 0, 10);
		  *  lRotateXM.SetR(lRotateX);
		  *  lRotateYM.SetR(lRotateY);
		  *  lRotateZM.SetR(lRotateZ);
		  *  lRotateXYZM = lRotateZM * lRotateYM * lRotateXM;
          *
		  *  // Alternatively, we can use SetR() directly.
		  *  // lRotateXYZM and lRotateM will be the same.
		  *  KFbxVector4 lRotateXYZ (10, 10, 10);
		  *  lRotateM.SetR(lRotateXYZ);
		  *  \endcode
		  * \note                Please refer to the FBX SDK programmers guide for more details.
		  */
		KFbxXMatrix operator*(const KFbxXMatrix& pXMatrix) const;

		/** Multiply two matrices together.
		  * \param pXMatrix     A Matrix.
		  * \return             \e this updated with the result of the multiplication.
		  */
		KFbxXMatrix& operator*=(const KFbxXMatrix& pXMatrix);

		/** Calculate the matrix inverse.
		  * \return     The inverse matrix of \e this.
		  */
		KFbxXMatrix Inverse() const;

		/** Calculate the matrix transpose.
		  * \return     The transposed matrix of \e this.
		  */
		KFbxXMatrix Transpose() const;

		//@}

		/**
		  * \name Boolean Operations
		  */
		//@{

		/**	Equivalence operator.
		  * \param pXMatrix     The matrix to be compared to \e this.
		  * \return             \c true if the two matrices are equal (each element is within a 1.0e-6 tolerance) and \c false otherwise.
		  */
		bool operator==(const KFbxXMatrix & pXMatrix) const;

		/**	Non-equivalence operator.
		  * \param pXMatrix     The matrix to be compared to \e this.
		  * \return            \c false if the two matrices are equal (each element is within a 1.0e-6 tolerance) and \c true otherwise.
		  */
		bool operator!=(const KFbxXMatrix & pXMatrix) const;
		
		//@}

		/**
		  * \name Casting
		  */
		//@{
		
		//! Cast the matrix in a double pointer.
		operator double* ();
        //! Cast the matrix in a const double pointer.
        operator const double* () const;
        //! Define 4*4 array as a new type 
		typedef const double(kDouble44)[4][4] ;
        //! Cast the matrix in a reference to a 4*4 array.
		inline kDouble44 & Double44() const { return *((kDouble44 *)&mData); }
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
		void CreateKFbxXMatrixRotation(double pX, double pY, double pZ);
		void V2M(KFbxXMatrix &pMatrix, const KFbxVector4 &pVector, ERotationOrder pRotationOrder);
		void M2V(KFbxVector4 &pVector, const KFbxXMatrix &pMatrix, ERotationOrder pRotationOrder);

		/**
		  * \name Internal Casting
		  */
		//@{

		KFbxXMatrix& operator=(const KgeAMatrix& pAMatrix);
		operator KgeAMatrix& ();

		//@}

	#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

	};

	inline EFbxType FbxTypeOf( KFbxXMatrix	const &pItem )	{ return eDOUBLE44; }
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXMATH_KFBXXMATRIX_H

