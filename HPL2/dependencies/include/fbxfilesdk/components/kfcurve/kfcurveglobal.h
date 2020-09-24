/*!  \file kfcurveglobal.h
 */

#ifndef FBXFILESDK_COMPONENTS_KFCURVE_KFCURVEGLOBAL_H
#define FBXFILESDK_COMPONENTS_KFCURVE_KFCURVEGLOBAL_H

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
#include <fbxfilesdk/components/kbaselib/klib/ktime.h>

#include <fbxfilesdk/components/kfcurve/kfcurve.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

	class KSet;

	KFBX_FORWARD_HI(KFCurve);
	KFBX_FORWARD_HI(KFCurveNode);

	#define KFCURVE_INTEPOLATION_INFINITE 1000
	#define KFCURVE_OVERSHOOT_INTERPOLATION 500.0 

    /**KSmartPlotOptions
    */
	KFBX_FORWARD(KSmartPlotOptions)

	class KFBX_DLL KSmartPlotOptions
	{

	public: 

		//! Enable the SmartPlot
		bool	mUseSmartPlot;

		//! Enable the Break Tangent post plot operation (these settings will also be used by the fidelity check)
		bool	mUseBreakTangent;
		double	mBreakTangentThreshold;
		KTime	mEpsilonTime;

		//! Enable the Increase Fidelity post plot operation
		bool	mUseIncreaseFidelity;
		double	mFidelityTolerance;
		bool	mFidelityUseBreakTangent;
		int		mFidelitySegmentCount;
		bool	mFidelitySyncAddedKeys;

		//! Use the reference curve as the result instead of the smart plot curve. Used for debug. Is also the most exact plot we can have in MB.
		bool	mShowReferenceCurve;

		//! Constructor. All values will be set to default.
		KSmartPlotOptions();
	};

	/**KMergeLayerOptions
	  */
	KFBX_FORWARD(KMergeLayerOptions)

	class KFBX_DLL KMergeLayerOptions
	{

	public: 

		//! Start time. Merge in this interval.
		KTime mStart;

        //! Stop time. Merge in this interval.
		KTime mStop;
		
		//! Plot period. Used only when not smart plotting.
		KTime mPlotPeriod;

		//! Smart Merge options
		KSmartPlotOptions mSmartPlotOptions;

		//! Constructor. All values will be set to default.
		KMergeLayerOptions();
	};

	// TimeWarp Utility

	//!
	KFBX_DLL void KFCurve_SetTWSet (KSet* pSet);

	//!
	KFBX_DLL HKFCurveNode KFCurve_FindTWFromNickNumber (int pNickNumber);

	//!
	KFBX_DLL bool KFCurve_FindNickNumberFromTW (int& pNickNumber, HKFCurveNode pTWNode);

	// ControlCurve Utility

	enum 
	{	
	//	eKFCURVE_PLOT_SourceCurvesKeep					= 1<<0 ,
		eKFCURVE_PLOT_SourceCurvesDestroy				= 1<<1 ,
		eKFCURVE_PLOT_SourceCurvesZeroDuringPlotTime	= 1<<2 ,
		eKFCURVE_PLOT_DestinationCurveMergeWithResult	= 1<<3 ,
		eKFCURVE_PLOT_DestinationCurveReplaceByResult	= 1<<4 ,
		eKFCURVE_PLOT_ForCache							= 1<<5 ,
	};

    /** Plots inside a curve node. 
    *
    *	Plot is risky for rotations because they are not commutative, so make sure
    *	layers to be plot are successive !!!
    *
    *	\param pParentNode				The node containing all layers.
    *	\param pSourceLayers			Layers to be plotted.
    *	\param pDestLayer				Layers to be plotted on this layer.
    *	\param pOptions				Flags  to set the behavior.
    *	\param pStart					Start time of the plot.
    *	\param pStop					Stop  time of the plot.
    *   \param pPlotPeriod
    *   \param pDestinationNode
    *   \param pTotalNode
    *   \param pUseTimeWarp
    *   \param pSmartPlotOptions
    */
    KFBX_DLL void KFCurve_PlotInLayers 
	(	
		HKFCurveNode pParentNode, 
		KArrayTemplate<int> *pSourceLayers, // if pointer is NULL, all layers are considered
		int pDestLayer,
		kUInt pOptions, 
		KTime pStart, 
		KTime pStop, 
		KTime pPlotPeriod,
		HKFCurveNode pDestinationNode,
		HKFCurveNode pTotalNode,	// If this node is set, function calculates layer to 
		bool pUseTimeWarp,
		HKSmartPlotOptions pSmartPlotOptions = NULL
	);

	/******************************************************************/
	//
	//	Helpers
	//
	/******************************************************************/

	// Vectorial conversions tools

	/** Calculate the euler rotation required to align axis pAB-pA on pAB-pB.
	*	\param pAB		The intersection of the 2 axis.
	*	\param pA		A point on axis to be aligned.
	*	\param pB		A point on reference axis.
	*	\param pAngles Resulting euler angles.
	*	\return how the operation succeed.
	*/
	KFBX_DLL bool AxisAlignmentInEulerAngle(double pAB[3], double pA[3], double pB[3], double pAngles[3]);

	// bob 23-05-2001 - old call found in kmodelutility are forwarded to those one
	KFBX_DLL KTime	KFCURVE_GetPlotPeriod();
	KFBX_DLL void	KFCURVE_SetPlotPeriod(KTime pPlotPeriod);

	//
	//	For plotting to be on frame or free... i.e. start at the beginning of the take or on frame.
	//
	KFBX_DLL bool	KFCURVE_GetPlotOnFrame();
	KFBX_DLL void	KFCURVE_SetPlotOnFrame( bool pPlotOnFrame );

	//
	// bob 23-05-2001 - the rotation correction filter that should be applied after a plot
	//
	enum { KFCurveApplyNone = 0, KFCurveApplyGimbleKiller, KFCurveApplyUnroll };

	KFBX_DLL int		KFCURVE_GetRotationFilterToApply();
	KFBX_DLL void	KFCURVE_SetRotationFilterToApply(int pRotationFilterToApply);
	KFBX_DLL void	KFCURVE_ApplyRotationFilter( HKFCurveNode pCurveNode, KTime pStart	= KTIME_MINUS_INFINITE, KTime pStop	= KTIME_INFINITE );

	KFBX_DLL double	KFCURVE_GetRotationFilterUnrollQuality();
	KFBX_DLL void	KFCURVE_SetRotationFilterUnrollQuality( double pQuality );

	KFBX_DLL bool	KFCURVE_GetRotationFilterUnrollTestForPath();
	KFBX_DLL void	KFCURVE_SetRotationFilterUnrollTestForPath( bool pTestForPath );

	KFBX_DLL double	KFCURVE_GetButterworthFilterInternalRate();
	KFBX_DLL void	KFCURVE_SetButterworthFilterInternalRate( double pButterworthFilterInternalRate );

	//
	// the constant key reducer to be applied on Plot
	//
	KFBX_DLL bool	KFCURVE_GetUseConstantKeyReducer();
	KFBX_DLL void	KFCURVE_SetUseConstantKeyReducer( bool pUseConstantKeyReducer );
    KFBX_DLL void   KFCURVE_ApplyConstantKeyReducer( HKFCurveNode pCurveNode, KTime pStart	= KTIME_MINUS_INFINITE, KTime pStop	= KTIME_INFINITE );
	//
	// keep at least one key after applying the filter (else, there will only be a candidate on the curve)
	//
	KFBX_DLL bool	KFCURVE_GetConstantKeyReducerKeepOneKey();
	KFBX_DLL void	KFCURVE_SetConstantKeyReducerKeepOneKey( bool pConstantKeyReducerKeepOneKey );

	//
	// the constant key reducer to be applied on Device record
	//
	KFBX_DLL bool	KFCURVE_GetUseConstantKeyReducerOnDevices();
	KFBX_DLL void	KFCURVE_SetUseConstantKeyReducerOnDevices( bool pUseConstantKeyReducerOnDevices );

	//
	KFBX_DLL double	KFCURVE_GetUseConstantKeyReducerTranslationThreshold();
	KFBX_DLL void	KFCURVE_SetUseConstantKeyReducerTranslationThreshold( double pConstantKeyReducerTranslationThreshold );

	//
	KFBX_DLL double	KFCURVE_GetUseConstantKeyReducerRotationThreshold();
	KFBX_DLL void	KFCURVE_SetUseConstantKeyReducerRotationThreshold( double pConstantKeyReducerRotationThreshold );

	//
	KFBX_DLL double	KFCURVE_GetUseConstantKeyReducerScalingThreshold();
	KFBX_DLL void	KFCURVE_SetUseConstantKeyReducerScalingThreshold( double pConstantKeyReducerScalingThreshold );

	//
	KFBX_DLL double	KFCURVE_GetUseConstantKeyReducerDefaultThreshold();
	KFBX_DLL void	KFCURVE_SetUseConstantKeyReducerDefaultThreshold( double pConstantKeyReducerDefaultThreshold );

	//
	// tell if we should "Plot Translation On Root Only" or not (Antoine 2003-05-02)
	//
	KFBX_DLL bool	KFCURVE_GetPlotTranslationOnRootOnly();
	KFBX_DLL void	KFCURVE_SetPlotTranslationOnRootOnly( bool pPlotTranslationOnRootOnly );

	//
	KFBX_DLL bool KFCURVE_GetPreciseTimeDiscontinuities();
	KFBX_DLL void KFCURVE_SetPreciseTimeDiscontinuities( bool pPreciseTimeDiscontinuities );

	//
	KFBX_DLL bool KFCURVE_GetTCB4051EvalMode();
	KFBX_DLL void KFCURVE_SetTCB4051EvalMode( bool pTCB4051EvalMode );

	// Functions to convert bezier tan to the new automatic parametrisation
	KFBX_DLL void KFCURVE_ComputeAutoFromBezier(double &pLeftParam, double &pRightParam, double &pLeftTan,   double &pRightTan,   double &pToLeftKey, double &pToRightKey);
	KFBX_DLL void KFCURVE_ComputeBezierFromAuto(double &pLeftTan,   double &pRightTan,   double &pLeftParam, double &pRightParam, double &pToLeftKey, double &pToRightKey);

	KFBX_DLL void KFCURVE_ComputeLeftAutoFromBezier(double &pLeftParam, double &pLeftTan,   double &pToLeftKey, double &pToRightKey);
	KFBX_DLL void KFCURVE_ComputeLeftBezierFromAuto(double &pLeftTan,   double &pLeftParam, double &pToLeftKey, double &pToRightKey);

	KFBX_DLL void KFCURVE_ComputeRightAutoFromBezier(double &pRightParam, double &pRightTan,   double &pToLeftKey, double &pToRightKey);
	KFBX_DLL void KFCURVE_ComputeRightBezierFromAuto(double &pRightTan,	  double &pRightParam, double &pToLeftKey, double &pToRightKey);

	KFBX_DLL void KFCURVE_ComputeAutoFromBezierForKey(double &pLeftParam, double &pRightParam, double &pLeftTan,   double &pRightTan,   HKFCurve pCurve, int pIndex);
	KFBX_DLL void KFCURVE_ComputeBezierFromAutoForKey(double &pLeftTan,   double &pRightTan  , double &pLeftParam, double &pRightParam, HKFCurve pCurve, int pIndex);

	KFBX_DLL void KFCURVE_ComputeLeftAutoFromBezierForKey(double &pLeftParam, double &pLeftTan,   HKFCurve pCurve, int pIndex);
	KFBX_DLL void KFCURVE_ComputeLeftBezierFromAutoForKey(double &pLeftTan,   double &pLeftParam, HKFCurve pCurve, int pIndex);

	KFBX_DLL void KFCURVE_ComputeRightAutoFromBezierForKey(double &pRightParam, double &pRightTan,   HKFCurve pCurve, int pIndex);
	KFBX_DLL void KFCURVE_ComputeRightBezierFromAutoForKey(double &pRightTan  , double &pRightParam, HKFCurve pCurve, int pIndex);

	KFBX_DLL void KFCURVE_ComputeToLeftAndToRight(double &pToLeft, double &pToRight, HKFCurve pCurve , int pIndex);
    KFBX_DLL void KFCURVE_ComputeProgressiveClampedTangents(double& pLeftTangent, double& pRightTangent, HKFCurve pCurve, int pIndex);

	/** Compare the FCurve with the reference FCurve and break the tangents when necessary so that both FCurves match.
	* \param pFCurveNode			FCurveNode to modify.
	* \param pReferenceFCurveNode	FCurveNode we use as the reference.
	* \param pEpsilonTime			Epsilon time used to evaluate the tangents.
	* \param pTangentThreshold	Threshold used when comparing tangents. If the difference is over the threshold, we break the tangent.
	*/
	KFBX_DLL void KFCURVE_BreakTangents( HKFCurveNode pFCurveNode, HKFCurveNode pReferenceFCurveNode, KTime pEpsilonTime, double pTangentThreshold );

KFBX_DLL void KFCURVE_ComputeTCBFromBezierForKey( double &pT, double &pC ,double &pB , double pLTan, double pRTan, HKFCurve pCurve, int pIndex );

	/** Compare the FCurve with the reference FCurve and add keys when necessary so that both FCurves match.
	* \param pFCurveNode					FCurveNode to modify.
	* \param pReferenceFCurveNode			FCurveNode we use as the reference.
	* \param pFidelityThreshold			Threshold used when comparing values. If the difference is over the threshold, we add a new key.
	* \param pSegmentCount				Number of segments we divide the curve between every pairs of keys.
	* \param pBreakTangents				True if we should break the tangents when they don't match the reference.
	* \param pEpsilonTime					Epsilon time used to evaluate the tangents.
	* \param pTangentThreshold			Threshold used when comparing tangents. If the difference is over the threshold, we break the tangent.
	* \param pSyncAddedKeys				True if fidelity keys should be added to all FCurves of a vector.
	* \param pSyncWithFCurves				For internal use, leave to NULL.
	* \param pSyncWithReferenceFCurves	For internal use, leave to NULL.
	*/
	KFBX_DLL void KFCURVE_IncreaseFidelity( HKFCurveNode pFCurveNode, HKFCurveNode pReferenceFCurveNode, double pFidelityThreshold, int pSegmentCount, bool pBreakTangents, KTime pEpsilonTime, double pTangentThreshold, bool pSyncAddedKeys = false, KArrayTemplate<HKFCurve>* pSyncWithFCurves = NULL, KArrayTemplate<HKFCurve>* pSyncWithReferenceFCurves = NULL );


	/**  Determined if all the keys in the underlying KFCurveNode have the same interpolation type
	* \param pFCurveNode		FCurveNode to check.
	* \param pInterpolation	The interpolation type we are querying for.
	* \param pAllLayer		True if check is in all layers.
	* \return					True if the all all have pInterpolation type.
	*/
	KFBX_DLL bool KFCURVE_IsAllSameInterpolation( HKFCurveNode pFCurveNode, kFCurveInterpolation pInterpolation, bool pAllLayer);

	/** Smart merge version for curves that have only constant keys. The current implementation of SmartMerge does not work very well with
	* different types of constant keys.
	* \param pDstNode		The Destination KFCurveNode in which all layers will be merged.
	* \param pSrcNode		Multi-layered KFCurveNode that has to be merged
	* \param pStart		Merge start time
	* \param pStop		Merge stop time
	*/
	KFBX_DLL void KFCURVE_ConstantCurveMerge(HKFCurveNode pDstNode,HKFCurveNode pSrcNode, KTime pStart, KTime pStop);

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KFCURVE_KFCURVEGLOBAL_H

