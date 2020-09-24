/*!  \file kfbxnurbscurve.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXNURBSCURVE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXNURBSCURVE_H

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

#include <fbxfilesdk/kfbxplugins/kfbxgeometry.h>
#include <fbxfilesdk/kfbxplugins/kfbxline.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/**
    A Non-Uniform Rational B-Spline (NURBS) curve is a type of parametric geometry. A NURBS
    curve is defined by the degree, form, knot vector and control points. 

	Let M be the degree of the curve.
	Let N be the number of control points of the curve.

	The form of the curve can be open, closed or periodic. A curve with end points
	that do not meet is defined as an open curve. The number of knots in an open curve
	is defined as N+(M+1). 
	
	A closed curve simply has its last control point equal to its first control point. 
	Note that this does not imply tangent continuity at the end point.  The curve may 
	have a kink at this point.  In FBX the last control point is not specified by the user
	in the InitControlPoints() method. For example, if there are to be 10 control points in
	total, and the curve is to be closed, than only 9 control points need to be passed 
	into the InitControlPoints() method. The last control point is implied to be equal
	to the first control point. Thus N represents the number of unique CVs. 

	A periodic curve has its last M control points equal to its first M control points. 
	A periodic curve is tangent continuous at the ends. Similar to a closed curve,
	when creating a periodic curve, only the unique control points need to be set. For
	example a periodic curve of degree 3 with 10 control points requires only 7 CVs to 
	be specified in the InitControlPoints() method. The last 3 CVs, which are the same as
	the first 3, are not included. 

	The calculation of the number of knots in closed and periodic curves is more complex. 
	Since we have excluded one CV in N in a closed curve, the number of knots is N+(M+1)+1. 
	Similarly, we excluded M CVs in periodic curves so the number of knots is N+(M+1)+M. 

	Note that FBX stores one extra knot at the beginning and and end of the knot vector,
	compared to some other graphics applications such as Maya. The two knots are not 
	used in calculation, but they are included so that no data is lost when converting
	from file formats that do store the extra knots.

  * \nosubgrouping
  */
class KFBX_DLL KFbxNurbsCurve : public KFbxGeometry 
{
	KFBXOBJECT_DECLARE(KFbxNurbsCurve,KFbxGeometry);
public:
	//! Returns the EAttributeType::eNURBS_CURVE node attribute type.
	virtual KFbxNodeAttribute::EAttributeType GetAttributeType() const;

	/** \enum EDimension        The dimension of the CVs.
	  * - \e e2D                The CVs are two dimensional points.
	  * - \e e3D                The CVs are three dimensional points.
	  */
	enum EDimension
	{
		e2D = 2,
		e3D,
		eDIMENSIONS_COUNT = 2
	};

	/** \enum EType             The curve's form.
	  * - \e eOPEN
	  * - \e eCLOSED
	  * - \e ePERIODIC
	  */
	enum EType
	{
		eOPEN,
		eCLOSED,
		ePERIODIC,
		eTYPE_COUNT
	}; 

	/** Allocates memory space for the control points array as well as for the knot 
	  * vector.
      * \param pCount           Number of control points.
      * \param pVType           NURBS type.
	  * \remarks                This function should always be called after KFbxNurb::SetOrder(). 
      */
	void InitControlPoints( int pCount, EType pVType );

	/** Returns the knot vector.
	  * \return                 Pointer to the knots array.
	  */
	inline double* GetKnotVector() const { return mKnotVector; }

	/** Returns the number of elements in the knot vector.
	  * \return                 The number of knots.
	  */
	int GetKnotCount() const;

	/** Sets the order of the curve.
      * \param pOrder           The curve order.
      * \remarks                The curve order must be set before InitControlPoints() is called. 
      */
	inline void SetOrder( int pOrder ) { mOrder = pOrder; }

	/** Returns the NURBS curve order.
	  * \return                 The NURBS curve order.
	  */
	inline int GetOrder() const { return mOrder; }

    /** Sets the step of the curve.
    * \param pOrder           The curve step.
    * \remarks                To tessellate curve, it denotes the evaluation frequency between two neighbor knots.
    */
    inline void SetStep( int pStep ) { mStep = pStep; }

    /** Returns the NURBS curve step.
    * \return                 The NURBS curve step.
    * \remarks                To tessellate curve, it denotes the evaluation frequency between two neighbor knots.
    */
    inline int GetStep() const { return mStep; }

	/** Sets the dimension of the CVs.
	  * For 3D curves: control point = ( x, y, z, w ), where w is the weight.
	  * For 2D curves: control point = ( x, y, 0, w ), where the z component is unused, and w is the weight. 
	  * \param pDimension       The control points dimension(3D or 2D).
	  */
	inline void SetDimension( EDimension pDimension ) { mDimension = pDimension; }

	/** Returns the control points dimension.
	  * \return                 The curve dimension.
	  */
	inline EDimension GetDimension() const { return mDimension; }

	/** Determines if the curve is rational or not.
	  * \return                 \c True if the curve is rational, return \c false if not.
	  */
	bool IsRational(); 

	/** Calculates the number of curve spans with the following:
	  * Where
	  * S = Number of spans
	  * N = Number of CVs
	  * M = Order of the curve
	  *
	  * S = N - M + 1;
	  *
	  * In this calculation N includes the duplicate CVs for closed and periodic curves. 
	  * 
	  * \return                 The number of curve spans if the curve has been initialized, returns -1 if the curve has not been initialized.
	  */
	int GetSpanCount() const;

	/** Returns NURBS type.
	  * \return                 NURBS type identifier.
	  */
	inline EType GetType() const { return mNurbType; }

	/** Checks if the curve is a poly line. (A poly line is a 
	  * linear NURBS curve )
	  *
	  * \return                 \c True if curve is a poly line, return \c false if it is not a poly line.
	  */
	inline bool IsPolyline() const { return ( GetOrder() == 2 ); }

	/** This function determines if this NURBS curve is a Bezier curve.
	  * Bezier curves are a special case of NURBS curve. 
	  * \return                 \c True if curve is a Bezier curve. If it is not a Bezier curve return \c false.
	  */
	bool IsBezier() const;

    /** Evaluate the point on the curve. Save the result as a point array. Meanwhile, return the length of the point array.
    * \param pPointArray           Save the evaluate result as a point array.
    * \param pStep                 The evaluation frequency between two neighbor knots. Its default value is 16, which is same as Maya.
    * \return The length of the point array.
    */
    int TessellateCurve(KArrayTemplate<KFbxVector4>& pPointArray, int pStep = 16);

    /** Evaluate the point on the curve. Per the evaluation result, create a KFbxLine and return the pointer to the line.
    * \param pStep                 The evaluation frequency between two neighbor knots. Its default value is 16, which is same as Maya.
    * \return A line to hold the tessellate points.
    */
    KFbxLine* TessellateCurve(int pStep = 16);

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

    virtual KFbxObject& Copy(const KFbxObject& pObject);

	bool FullMultiplicity() const;

    // Error identifiers, these are only used internally.
	typedef enum 
	{
		eNurbCurveTypeUnknown,
		eWeightTooSmall,
		eKnotVectorError,
        eWrongNumberOfControlPoint,

		/*
        
        eUMultiplicityVectorError,
        eVMultiplicityVectorError,
        ,
        eVKnotVectorError, */
		eErrorCount
	} EError;

protected:
	KFbxNurbsCurve(KFbxSdkManager& pManager, char const* pName);
	
	void Reset();

	virtual void Destruct(bool pRecursive, bool pDependents);

public:
	//To maintain compatibility
	bool mIsRational;

private:
	double* mKnotVector;
	EType mNurbType;
	int mOrder;
	EDimension mDimension; 
    //To tessellate curve, it denotes the evaluation frequency between two neighbor knots.
    int mStep;
#endif
};

typedef KFbxNurbsCurve* HKFbxNurbsCurve;
typedef class KFBX_DLL KArrayTemplate< KFbxNurbsCurve* > KArrayKFbxNurbsCurve;


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXNURBSCURVE_H

