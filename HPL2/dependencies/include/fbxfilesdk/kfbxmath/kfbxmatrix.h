/*!  \file kfbxmatrix.h
 */

#ifndef FBXFILESDK_KFBXMATH_KFBXMATRIX_H
#define FBXFILESDK_KFBXMATH_KFBXMATRIX_H

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

#include <fbxfilesdk/kfbxplugins/kfbxtypes.h>
#include <fbxfilesdk/kfbxmath/kfbxvector4.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxQuaternion;
class KFbxXMatrix;


/**	FBX SDK matrix class.
  * \nosubgrouping
  */
class KFBX_DLL KFbxMatrix : public fbxDouble44
{

public:

	/**
	  * \name Constructors and Destructor
	  */
	//@{

	//! Constructor. Constructs an identity matrix.
	KFbxMatrix ();

	/** Copy constructor.
	  * \param pM Another KFbxMatrix object copied to this one.
	  */
	KFbxMatrix (const KFbxMatrix& pM);

	/** Constructor.
	  *	\param pT     Translation vector.
	  *	\param pR     Euler rotation vector.
	  *	\param pS     Scale vector.
	  */
	KFbxMatrix (const KFbxVector4& pT,
		        const KFbxVector4& pR,
				const KFbxVector4& pS);

	/** Constructor.
	  *	\param pT     Translation vector.
	  *	\param pQ     Quaternion.
	  *	\param pS     Scale vector.
	  */
	KFbxMatrix (const KFbxVector4& pT,
		        const KFbxQuaternion& pQ,
				const KFbxVector4& pS);

	/** Constructor.
	  * \param pM     Affine matrix
	  */
	KFbxMatrix (const KFbxXMatrix& pM);
		
	//! Destructor.
	~KFbxMatrix ();
		
	//@}

	/**
	  * \name Access
	  */
	//@{

	/** Retrieve matrix element.
	  *	\param pY     Row index.
	  *	\param pX     Column index.
	  * \return       Value at element [ pX, pY ] of the matrix.
	  */
	double Get(int pY, int pX) const;

	/** Extract a row vector.
	  *	\param pY     Row index.
	  * \return       The row vector.
	  */
	KFbxVector4 GetRow(int pY) const;

	/** Extract a column vector.
	  *	\param pX      Column index.
	  * \return        The column vector.
	  */
	KFbxVector4 GetColumn(int pX) const;

	/** Set matrix element.
	  *	\param pY          Row index.
	  *	\param pX          Column index.
	  *	\param pValue      New component value.
	  */
	void Set(int pY, int pX, double pValue);

	//! Set matrix to identity.
	void SetIdentity();
	
	/** Set matrix.
	  *	\param pT     Translation vector.
	  *	\param pR     Euler rotation vector.
	  *	\param pS     Scale vector.
	  */
	void SetTRS(const KFbxVector4& pT,
		        const KFbxVector4& pR,
				const KFbxVector4& pS);

	/** Set matrix.
	  *	\param pT     Translation vector.
	  *	\param pQ     Quaternion.
	  *	\param pS     Scale vector.
	  */
	void SetTQS(const KFbxVector4& pT,
		        const KFbxQuaternion& pQ,
				const KFbxVector4& pS);

	/** Set a matrix row.
	  *	\param pY       Row index.
	  *	\param pRow	    Row vector.
	  */
	void SetRow(int pY, const KFbxVector4& pRow);

	/** Set a matrix column.
	  *	\param pX           Column index.
	  *	\param pColumn      Column vector.
	  */
	void SetColumn(int pX, const KFbxVector4& pColumn);

    /** Decompose the affine matrix into elements of translation, rotation,
      * shearing, scaling and sign of determinant.
      * \param pTranslation     Translation element.
      * \param pRotation        Rotation element.
      * \param pShearing        Shearing element.
      * \param pScaling         Scaling element.
      * \param pSign            Sign of determinant.
      */
    void GetElements(KFbxVector4 & pTranslation, KFbxQuaternion & pRotation,
        KFbxVector4 & pShearing, KFbxVector4 & pScaling, double & pSign) const;

	/** Assignment operator.
	  *	\param pMatrix     Source matrix.
	  */
	KFbxMatrix& operator=(const KFbxMatrix& pMatrix);
	
	//@}

	/**
	  * \name Matrix Operations
	  */
	//@{	

	/**	Unary minus operator.
	  * \return     A matrix where each element is multiplied by -1.
	  */
	KFbxMatrix operator-() const;
	
	/** Add two matrices together.
	  * \param pMatrix    A matrix.
	  * \return           The result of this matrix + pMatrix.
	  */
	KFbxMatrix operator+(const KFbxMatrix& pMatrix) const;

	/** Subtract a matrix from another matrix.
	  * \param pMatrix     A matrix.
	  * \return            The result of this matrix - pMatrix.
	  */
	KFbxMatrix operator-(const KFbxMatrix& pMatrix) const;

	/** Multiply two matrices.
	  * \param pMatrix     A matrix.
	  * \return            The result of this matrix * pMatrix.
	  */
	KFbxMatrix operator*(const KFbxMatrix& pMatrix) const;

	/** Add two matrices together.
	  * \param pMatrix     A matrix.
	  * \return            The result of this matrix + pMatrix, replacing this matrix.
	  */
	KFbxMatrix& operator+=(const KFbxMatrix& pMatrix);

	/** Subtract a matrix from another matrix.
	  * \param pMatrix     A matrix.
	  * \return            The result of this matrix - pMatrix, replacing this matrix.
	  */
	KFbxMatrix& operator-=(const KFbxMatrix& pMatrix);

	/** Multiply two matrices.
	  * \param pMatrix     A matrix.
	  * \return            The result of this matrix * pMatrix, replacing this matrix.
	  */
	KFbxMatrix& operator*=(const KFbxMatrix& pMatrix);

    /** Calculate the matrix inverse.
    * \return     The inverse matrix.
    */
    KFbxMatrix Inverse() const;

	/** Calculate the matrix transpose.
	  * \return     This matrix transposed.
	  */
	KFbxMatrix Transpose() const;

	//@}

	/**
	  * \name Vector Operations
	  */
	//@{	

    /** Multiply this matrix by pVector, the w component is normalized to 1.
    * \param pVector     A vector.
    * \return            The result of this matrix * pVector.
    */
    KFbxVector4 MultNormalize(const KFbxVector4& pVector) const;

    //@}

	/**
	  * \name Boolean Operations
	  */
	//@{

	/**	Equivalence operator.
	  * \param pM     The matrix to be compared against this matrix.
	  * \return       \c true if the two matrices are equal (each element is within a 1.0e-6 tolerance), \c false otherwise.
	  */
	bool operator==(const KFbxMatrix & pM) const;

	/**	Equivalence operator.
	  * \param pM     The affine matrix to be compared against this matrix.
	  * \return       \c true if the two matrices are equal (each element is within a 1.0e-6 tolerance), \c false otherwise.
	  */
	bool operator==(const KFbxXMatrix & pM) const;

	/**	Non-equivalence operator.
	  * \param pM     The matrix to be compared against this matrix.
	  * \return       \c false if the two matrices are equal (each element is within a 1.0e-6 tolerance), \c true otherwise.
	  */
	bool operator!=(const KFbxMatrix & pM) const;

	/**	Non-equivalence operator.
	  * \param pM     The affine matrix to be compared against this matrix.
	  * \return       \c false if the two matrices are equal (each element is within a 1.0e-6 tolerance), \c true otherwise.
	  */
	bool operator!=(const KFbxXMatrix & pM) const;

	
	//@}

	/**
	  * \name Casting
	  */
	//@{

    //! Cast the vector in a double pointer.
    operator double* ();

    //! Cast the vector in a const double pointer.
    operator const double* () const;

	//! Define 4*4 array as a new type.
	typedef const double(kDouble44)[4][4] ;

	//! Cast the matrix in a reference to a 4*4 array.
	inline kDouble44 & Double44() const { return *((kDouble44 *)&mData); }

	//@}

	// Matrix data.
//	double mData[4][4];

};

inline EFbxType FbxTypeOf( KFbxMatrix	const &pItem )	{ return eDOUBLE44; }


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXMATH_KFBXMATRIX_H

