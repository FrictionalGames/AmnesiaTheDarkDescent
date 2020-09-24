/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxanimcurvenode.h
*/

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXANIMCURVENODE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXANIMCURVENODE_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>
class KFbxAnimStack;
class KFbxAnimCurve;
class KSet;

/** This class is an composite of animation curves and is called as animation curve node.
  * \nosubgrouping
  * Animation curve node is used as the connection point for animation curves and other animation curve nodes 
  * associated to a property.  KFbxAnimCurveNode can be connected to other KFbxAnimCurveNode, 
  * in this case, the destination animation curve node may be considered as "composite", \ref IsComposite().
  * remarks  When created, the KFbxAnimCurveNode has no valid channels unless it is created using the function CreateTypedCurveNode(). 
  * This function will add all the required channels to correctly match the number of values of the property.
  * For instance, when CreateTypedCurveNode(pNode.LclTranslation, pScene) is called, the resulting 
  * KFbxAnimCurveNode will automatically have 3 channels corresponding to the X,Y and Z components of the LclTranslation property. 
  * You can add and remove channels dynamically but can never remove the channels that have been added by the call to CreateTypedCurveNode(). 
  *
  * However, the FBX SDK animation system's default implementation is to consider only the first curve connected to 
  * the channel. Therefore, if the caller connects multiple animation curves to the same channel, then it becomes 
  * the caller's responsibility to handle and manipulate these extra curves in a meaningful manner.
  */
class KFBX_DLL KFbxAnimCurveNode : public KFbxObject
{
    KFBXOBJECT_DECLARE(KFbxAnimCurveNode, KFbxObject);

public:
    /**
      * \name Utility functions.
      *
      */
    //@{
        /** Check if the animation curve node contains any animation key.
          * \param pRecurse \c true to descend to the children if the animation curve node is composite.
          * \return \c true if at least one animation curve that contains one or more animation keys is found, 
          *         \c false otherwise.
		  * \remarks This method only considers the first animation curve connected to each channel.
		  *          To check multiple animation curves that are connected to the same channel, it is the caller's 
		  *          responsibility to write a new version of this method, and GetCurveCount() will be useful in this case.
          */
        bool IsAnimated(bool pRecurse=false) const;

        /** Find out start and end time of the animation.
          * This function retrieves the including time span for all animation curves of this animation curve node.
          * \param pStart Reference to receive start time.
          * \param pStop Reference to receive end time.
          * \return \c true on success, \c false otherwise.
          * \remarks \c false is also returned if this animation curve node has no animation.
		  * \remarks This method only considers the first animation curve connected to each channel.
		  *          To find time interval of multiple animation curves that are connected to the same channel, it is the caller's 
		  *          responsibility to write a new version of this method, and GetCurveCount() will be useful in this case.
          * \remarks This function is deprecated, please use GetAnimationInterval(KTimeSpan&) instead.
          */
        K_DEPRECATED bool GetAnimationInterval(KTime& pStart, KTime& pStop) const;

        /** Find out start and end time of the animation.
          * This function retrieves the including time span for all animation curves of this animation curve node.
          * \param pTimeInterval Reference to receive start time and end time.
          * \return \c true on success, \c false otherwise.
          * \remarks \c false is also returned if this animation curve node has no animation.
		  * \remarks This method only considers the first animation curve connected to each channel.
		  *          To find time interval of multiple animation curves that are connected to the same channel, it is the caller's 
		  *          responsibility to write a new version of this method, and GetCurveCount() will be useful in this case.
          */
        bool GetAnimationInterval(KTimeSpan& pTimeInterval) const;

        /** Test this object to see if it is a composite KFbxAnimCurveNode or a "leaf".
          * A composite KFbxAnimCurveNode is a KFbxAnimCurveNode whose all source connections are KFbxAnimCurveNode
          * and its property channels is totally empty. It is just a container to take other KFbxAnimCurveNode.
          * \return \c true if this object is a composite, \c false otherwise.
          */
        bool IsComposite() const;

        /** Recursively look for the KFbxAnimCurveNode matching the passed named argument.
          * \param pName Name of the KFbxAnimCurveNode we are looking for.
          * \return The found anim curve node or NULL.
          * \remarks If pName is an empty string, this function automatically return NULL.
          */
        KFbxAnimCurveNode* Find(const char* pName);

        /** Create a KFbxAnimCurveNode compatible with the specified property data type.
          * \param pProperty The property that needs a KFbxAnimCurveNode.
          * \param pScene The scene the created KFbxAnimCurveNode will belong to.
          * \return The pointer to the newly created KFbxAnimCurveNode. Returns NULL if an error occurred. 
          * \remarks This function does not connect the newly created KFbxAnimCurveNode to the property.
          * \remarks This function detects fbxDouble3, fbxDouble4 and fbxDouble44 properties DataTypes and
          *         automatically adds the required channels properties. Any other DataType is not 
          *         specifically processed and the channels properties are left empty and need to be filled
          *         using the AddChannel() function.
          */
        static KFbxAnimCurveNode* CreateTypedCurveNode(KFbxProperty& pProperty, KFbxScene* pScene);

        /** Get the total number of property channels defined in this animation curve node.
		  * For composite animation curve nodes, since they do not contain any channels, this function will always return 0.
          * \return The number of property channels.
          */
        unsigned int GetChannelsCount() const;

        /** Get the index of the named channel.
          * \param pChannelName Name of the channel for which we want the index.
          * \return the index of the named channel or -1 if no channel with this name is found.
          */
        int GetChannelIndex(const char* pChannelName) const;

        /** Get the name of the channel.
          * \param pChannelId Index of the channel for which we want the name.
          * \return the name of the indexed channel or "" if the index is invalid.
          */
        KString GetChannelName(int pChannelId) const;

        /** Empties the property channels of this animation curve node.
          * \remarks This function will remove all the channels added with the AddChannel() method
          *          regardless of their use and/or connections.
		  *          But it can not remove the channels that are added by the call to CreateTypedCurveNode().
          */
        void ResetChannels();

        /** Adds the specified channel property.
          * \param pChnlName Channel name.
          * \param pValue Default value of the channel.
          * \return \c true if successful, \c false otherwise.
          * \remarks It is an error to try to add a channel that already exists.
          */
        template <class T> bool AddChannel(const char* pChnlName, T const &pValue)
        {
            if (!pChnlName || strlen(pChnlName)==0) return false;
            KFbxProperty c = GetChannel(pChnlName);
            if (c.IsValid()) 
            {
                return false;
            }

            mChannels.BeginCreateOrFindProperty();
            KFbxDataType dt = GetFbxDataType(FbxTypeOf(pValue));
            c = KFbxProperty::Create(mChannels, dt, pChnlName);
            KFbxSet<T>(c, pValue);
            mChannels.EndCreateOrFindProperty();
            return true;
        }

        /** Set the default value of the channel.
          * \param pChnlName Channel name.
          * \param pValue    New default value of this channel.
          */
        template <class T> void SetChannelValue(const char* pChnlName, T pValue)
        {
            KFbxProperty c = GetChannel(pChnlName);
            if (c.IsValid()) KFbxSet<T>(c, pValue);
        }

        /** Set the default value of the channel.
          * \param pChnlId   Channel index.
          * \param pValue    New default value of this channel.
          */
        template <class T> void SetChannelValue(unsigned int pChnlId, T pValue)
        {
            KFbxProperty c = GetChannel(pChnlId);
            if (c.IsValid()) KFbxSet<T>(c, pValue);
        }

        /** Get the default value of the channel.
          * \param pChnlName Channel name.
          * \param pInitVal  Value returned if the specified channel is invalid.
          * \return The default value of this channel.
          */
        template <class T> T GetChannelValue(const char* pChnlName, T pInitVal)
        {
            T v = pInitVal;
            KFbxProperty c = GetChannel(pChnlName);
            if (c.IsValid()) KFbxGet<T>(c, v);
            return v;
        }

        /** Get the default value of the channel.
          * \param pChnlId Channel index.
		  * \param pInitVal  Value returned if the specified channel is invalid.
          * \return The default value of this channel.
          */
        template <class T> T GetChannelValue(unsigned int pChnlId, T pInitVal)
        {
            T v = pInitVal;
            KFbxProperty c = GetChannel(pChnlId);
            if (c.IsValid()) KFbxGet<T>(c, v);
            return v;
        }
    //@}

    /**
      * \name KFbxAnimCurve management.
      *
      */
    //@{
        /** Disconnect the animation curve from the channel.
          * \param pCurve       The curve to disconnect from the channel.
          * \param pChnlId      The channel index.
          * \return \c true if the disconnection was made, \c false if an error occurred.
          */
        bool DisconnectFromChannel(KFbxAnimCurve* pCurve, unsigned int pChnlId);

        /** Connects the given animation curve to the specified channel.
          * \param pCurve   The curve to connect to the channel.
          * \param pChnl    The name of the channel the curve is to be connected to.
          * \param pInFront When \c true, all the current connections are moved after this one,
          *                 making this one the first. By default, the connection is the last one.
          * \return \c true if the connection was made, \c false if an error occurred.
          */
        bool ConnectToChannel(KFbxAnimCurve* pCurve, const char* pChnl, bool pInFront = false);

        /** Connects the given animation curve to the specified channel.
          * \param pCurve   The curve to connect to the channel.
          * \param pChnlId  Index of the channel the curve is to be connected to.
          * \param pInFront When \c true, all the current connections are moved after this one.
          *                 making this one the first. By default, the connection is the last one.
          * \return \c true if the connection was made, \c false if an error occurred.
          * \remarks The index is 0 based.
          */
        bool ConnectToChannel(KFbxAnimCurve* pCurve, unsigned int pChnlId, bool pInFront = false);

        /** Creates a new curve and connects it to the specified channel of the animation curve node named pCurveNodeName.
		  * If this animation curve node is composite, this function will try to search all children animation curve nodes
		  * recursively for the one named pCurveNodeName.
          * \param pCurveNodeName Name of the KFbxAnimCurveNode we are looking for.
          * \param pChannel Channel identifier.
          * \return Pointer to the KFbxAnimCurve or NULL if an error occurred.
          * \remarks pCurveNodeName cannot be empty.
          * \remarks If the pChannel identifier is left NULL, the first valid channel will be used to create curve.
          */
        KFbxAnimCurve* CreateCurve(const char* pCurveNodeName, const char* pChannel);

        /** Creates a new curve and connects it to the specified channel of the animation curve node named pCurveNodeName.
		  * If this animation curve node is composite, this function will try to search all children animation curve nodes
		  * recursively for the one named pCurveNodeName.
    	  * \param pCurveNodeName Name of the KFbxAnimCurveNode we are looking for.
          * \param pChannelId Channel index.
          * \return Pointer to the KFbxAnimCurve or NULL if an error occurred.
          * \remarks pCurveNodeName cannot be empty.
          *          If the pChannelId is not assigned, the first valid channel will be used to create curve.
          */
        KFbxAnimCurve* CreateCurve(const char* pCurveNodeName, unsigned int pChannelId = 0);
        
        /** Get the number of KFbxAnimCurve connected to the specified channel.
		  * If this animation curve node is composite, this function will try to search all children animation curve nodes
   		  * recursively for the one named pCurveNodeName.
          * \param pChannelId Channel index.
          * \param pCurveNodeName Name of the KFbxAnimCurveNode we are looking for.
          * \return The number of animation curves on the specified channel or 0 if an error occurred.
          * \remarks This method fails if the KFbxAnimCurveNode with name pCurveNodeName does not exist and return 0.
          *          If the specified channel cannot be found on the KFbxAnimCurveNode with name pCurveNodeName, return 0.
		  * \remarks If this animation curve node is composite, this function will try to search all 
		  *          children animation curve nodes recursively for the one named pCurveNodeName.
          *          If the pCurveNodeName is left NULL, then only look for the curves on this animation curve node 
		  *          even if it is composite.
          */
        int GetCurveCount(unsigned int pChannelId, const char* pCurveNodeName = NULL);

        /** Get the KFbxAnimCurve of the specified channel.
		  * If this animation curve node is composite, this function will try to search all children animation curve nodes
		  * recursively for the one named pCurveNodeName.
		  * \param pChannelId Channel index.
          * \param pId The index of the desired anim curve (in case there is more than one).
          * \param pCurveNodeName Name of the KFbxAnimCurveNode we are looking for (if this object is a composite).
          * \return Pointer to the KFbxAnimCurve that matches the criteria.
          * \remarks This method fails if the KFbxAnimCurveNode with name pCurveNodeName does not exist and return NULL.
          *          If the specified channel cannot be found in the KFbxAnimCurveNode with name pCurveNodeName, return NULL.
		  * \remarks If the pCurveNodeName is left NULL, then only search in the curves on this animation curve node 
		  *          even if it is composite.
          */
        KFbxAnimCurve* GetCurve(unsigned int pChannelId, unsigned int pId = 0, const char* pCurveNodeName = NULL);

     //@}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
	///////////////////////////////////////////////////////////////////////////////
	//  WARNING!
	//	Anything beyond these lines may not be documented accurately and is 
	// 	subject to change without notice.
	///////////////////////////////////////////////////////////////////////////////

	virtual KFbxObject& Copy(const KFbxObject& pObject);

    void Evaluate(double* pData, KTime pTime);

    /**
      * \name Internal use only.
      * This section is for internal use and is subject to change without notice.
      */
    //@{
        
    /** Create the compatible KFCurveNode structure.
      * This method will create a temporary KFCurveNode object and all the necessary
      * children to represent all the connections of this KFbxAnimCurveNode.
      * Each call to this method will clear the previous content of the KFCurveNode but does not
      * destroy it. An explicit call to ReleaseKFCurveNode has to be made.
      *
      * \remarks The = operator WILL NOT copy the KFCurveNode
      *
      */
    KFCurveNode* GetKFCurveNode(bool pNoCreate=false);

    /** Destroy the KFCurveNode structure.
      * This method will destroy the KFCurveNode that has been created by the call to GetKFCurveNode.
      * \remarks See the source code for the specific behavior for the deletion of the KFCurves.
      */
    void ReleaseKFCurveNode();

    //! Sync the each channel property with the KFCurve's mValue stored in the KFCurveNode
    void SyncChannelsWithKFCurve();

    inline bool UseQuaternionInterpolation() {return mQuaternionInterpolation != 0;}; 

    //! Returns false if \c pVal is an invalid value.
	bool SetQuaternionInterpolation(unsigned short pVal); 
    unsigned short GetQuaternionInterpolation() { return mQuaternionInterpolation; };

    // sync the KFCurveNodeLayerType based on the property Datatype (see the source code for details)
    void SetKFCurveNodeLayerType(KFbxProperty& pProp);

    static const char* CurveNodeNameFrom(const char* pName);
    //@}

private:
    /** Access the specified channel property.
      * \param pChnl Name of the desired channel.
      * \return The corresponding channel property if found or an empty one.
      * \remarks The returned property must always be tested for its validity.
      * \remarks If pChnl is NULL this method returns the first channel property.
      */
    KFbxProperty GetChannel(const char* pChnl);

    /** Access the specified channel property.
      * \param pChnlId Index of the desired channel.
      * \return The corresponding channel property if found or an empty one.
      * \remarks The returned property must always be tested for its validity.
      * \remarks The index is 0 based.
      */
    KFbxProperty GetChannel(unsigned int pChnlId);

protected:
    KFbxAnimCurveNode(KFbxSdkManager& pManager, char const* pName);
    KFbxAnimCurveNode* Find(KFbxAnimCurveNode* pRoot, const KString& pName);
    virtual bool ConstructProperties( bool pForceSet );
    virtual void Destruct(bool pRecursive, bool pDependents);

private:
	friend class KFbxObject;
	friend void CollectAnimFromCurveNode(KFCurve **lSrc, KFCurveNode *fcn, unsigned int nbCrvs, KFbxAnimCurveNode *cn, KSet* pNickToAnimCurveTimeWarpsSet, KSet& pNickToKFCurveNodeWarpSet);
    unsigned char	mNonRemovableChannels;
    KFbxProperty	mChannels;
    KFbxProperty*	mCurrentlyProcessed;
    KFCurveNode*	mFCurveNode;
    bool*			mOwnedKFCurve;
    int				mKFCurveNodeLayerType;
    unsigned short  mQuaternionInterpolation;
	int*			mDirectIndexes;
	int				mDirectIndexesSize;

    KFbxAnimCurve* GetCurve(unsigned int pChannelId, unsigned int pId, KFbxAnimCurveNode* pCurveNode);
    bool ConnectToChannel(KFbxProperty& p, KFbxAnimCurve* pCurve, bool pInFront);
    void ResetKFCurveNode();
    void SyncKFCurveValue(KFbxAnimCurve* pCurve, double pVal);
	void ReleaseOwnershipOfKFCurve(int pIndex);

	template <class T> KFbxAnimCurve* CreateCurveGeneral(const char* pCurveNodeName, T pChannel);

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};
typedef KFbxAnimCurveNode* HKKFbxAnimCurveNode;

KFBX_DLL void GetAllAnimCurves(KFbxAnimStack* pAnimStack, KArrayTemplate<KFbxAnimCurve*>& pCurves);
KFBX_DLL void GetAllAnimCurves(KFbxObject* pObj, KFbxAnimStack* pAnimStack, KArrayTemplate<KFbxAnimCurve*>& pCurves);

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXANIMCURVENODE_H

