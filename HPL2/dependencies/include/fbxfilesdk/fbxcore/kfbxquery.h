/*!  \file kfbxquery.h
 */

#ifndef FBXFILESDK_FBXCORE_KFBXQUERY_H
#define FBXFILESDK_FBXCORE_KFBXQUERY_H

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

#include <fbxfilesdk/components/kbaselib/klib/kmap.h>
#include <fbxfilesdk/fbxcore/kfbxpropertydef.h>
#include <fbxfilesdk/kfbxplugins/kfbxproperty.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

    /***********************************************
      KFbxQuery
    ************************************************/

/** Base class to manage query. A query contains a filter and reference ID, which will be used to search and retrieve objects. 
* The derived query classes are used to create KFbxCriteria.
* \nosubgrouping
* \see KFbxQueryOperator KFbxUnaryQueryOperator KFbxQueryClassId KFbxQueryIsA KFbxQueryProperty KFbxQueryConnectionType KFbxCriteria
*/
    class KFBX_DLL KFbxQuery {
        // Overridable Test functions
        public:
            KFBXNEW_DECLARE_FRIEND
			//! Get unique filter Id
            virtual kFbxFilterId                    GetUniqueId() const     { return 0x1452e230; }
            
			/** Judge if the given property is valid.
			  * \param pProperty The given property.
			  * \return \c True always, not implemented.
			  */
			virtual bool IsValid(KFbxProperty const &pProperty) const;

			/** Judge if the given property and connection type are valid.
			  * \param pProperty The given property.
			  * \param pType The given connection type.
              * \return \c True always, not implemented.
			  */
            virtual bool IsValid(KFbxProperty const &pProperty,kFbxConnectionType pType) const;

            /**This compares whether two KFbxQuery are the same, NOT whether the query
              * matches or not.  It's strictly the equivalent of an operator==, but virtual.
			  * \param pOtherQuery The given KFbxQuery
             */
            virtual bool IsEqual(KFbxQuery *pOtherQuery)    const;

        public:
			//! Add one to ref count .
            void                                    Ref();
			//! Minus one to ref count, if ref count is zero, delete this query object.
            void                                    Unref();
        protected:
            KFbxQuery();
            virtual ~KFbxQuery();
        private:
            class KFbxInternalFilter : public KFbxConnectionPointFilter{
                public:
                    KFbxInternalFilter(KFbxQuery *pQuery);
                    ~KFbxInternalFilter();

                // internal functions
                public:
                    KFbxConnectionPointFilter*      Ref();
                    void                            Unref();
                    kFbxFilterId                    GetUniqueId() const { return mQuery->GetUniqueId(); }
                    bool                            IsValid             (KFbxConnectionPoint*   pConnect) const;
                    bool                            IsValidConnection   (KFbxConnectionPoint*   pConnect,kFbxConnectionType pType) const;
                    bool                            IsEqual             (KFbxConnectionPointFilter* pConnectFilter) const;
                public:
                    KFbxQuery*      mQuery;

            };

            KFbxInternalFilter  mFilter;
            int                 mRefCount;

            friend class KFbxProperty;
            friend class KFbxInternalFilter;
        };

    /***********************************************
      KFbxQueryOperator (binary operators)
    ************************************************/
    enum eFbxQueryOperator { eFbxAnd,eFbxOr } ;

	/** Class to manage query operator. A query operator contains two KFbxQuery and an operator flag(eFbxAnd or eFbxOr).
    * The class is used to create criteria operator. For more details, please see KFbxCriteria::operator=(), KFbxCriteria::operator &&(), etc.
	* \nosubgrouping
    * \see KFbxQuery KFbxUnaryQueryOperator KFbxQueryClassId KFbxQueryIsA KFbxQueryProperty KFbxQueryConnectionType KFbxCriteria
	*/
    class KFBX_DLL KFbxQueryOperator : public KFbxQuery {
        //
        public:
            KFBXNEW_DECLARE_FRIEND
			/** Create new query operator.
			  * \param pA
			  * \param pOperator
			  * \param pB
			  */
            static KFbxQueryOperator* Create(KFbxQuery* pA,eFbxQueryOperator pOperator,KFbxQuery* pB);

        protected:
            KFbxQueryOperator(KFbxQuery* pA,eFbxQueryOperator pOperator,KFbxQuery* pB);
            virtual ~KFbxQueryOperator();

        // Test functions
        public:

			//! Get unique filter Id
            virtual kFbxFilterId    GetUniqueId() const{ return 0x945874; }

			/** Test if the given property is valid for this query operator.
			  * \param pProperty The given property.
			  * \return \c True if valid, \c false otherwise.
			  */
            virtual bool IsValid(KFbxProperty const &pProperty) const;

			/** Test if the given property and connection type are valid for this query operator.
			  * \param pProperty The given property.
			  * \param pType The given connection type.
			  * \return \c True if valid, \c false otherwise.
			  */
            virtual bool IsValid(KFbxProperty const &pProperty,kFbxConnectionType pType) const;
			
			/** Test if this query operator is equal with the given query operator.
			  * \param pOtherQuery The given query operator
			  * \return \c True if equal, \c false otherwise.
			  */
            virtual bool IsEqual(KFbxQuery *pOtherQuery)    const;
        private:
            KFbxQuery           *mA,*mB;
            eFbxQueryOperator   mOperator;
    };

    /***********************************************
      KFbxUnaryQueryOperator
    ************************************************/
    enum eFbxUnaryQueryOperator { eFbxNot };

	/** Class to manage unary query operator. A query operator contains one KFbxQuery and an operator flag(eFbxNot).
    * The class is used to create KFbxCriteria::operator !().
	* \nosubgrouping
    * \see KFbxQuery KFbxQueryOperator KFbxQueryClassId KFbxQueryIsA KFbxQueryProperty KFbxQueryConnectionType KFbxCriteria
	*/
    class KFBX_DLL KFbxUnaryQueryOperator : public KFbxQuery {
        //
        public:
            KFBXNEW_DECLARE_FRIEND
			/** Create new unary query operator.
			  * \param pA
			  * \param pOperator
			  */
            static KFbxUnaryQueryOperator* Create(KFbxQuery* pA,eFbxUnaryQueryOperator pOperator);

        protected:
            KFbxUnaryQueryOperator(KFbxQuery* pA,eFbxUnaryQueryOperator pOperator);
            virtual ~KFbxUnaryQueryOperator();

        // Test functions
        public:

			//! Get unique filter Id
            virtual kFbxFilterId    GetUniqueId() const{ return 0x945874BB; }

			/** Test if the given property is valid for this unary query operator.
			  * \param pProperty
			  * \return \c True if valid, \c false otherwise.
			  */
            virtual bool IsValid(KFbxProperty const &pProperty) const;

            /** Test if the given property and connection type are valid for this unary query operator.
			  * \param pProperty The given property.
			  * \param pType The given connection type.
			  * \return \c True if valid, \c false otherwise.
			  */
            virtual bool IsValid(KFbxProperty const &pProperty,kFbxConnectionType pType) const;

			/** Test if this unary query operator is equal with the given unary query operator.
			  * \param pOtherQuery The given unary query operator.
			  * \return \c True if equal, \c false otherwise.
			  */
            virtual bool IsEqual(KFbxQuery *pOtherQuery)    const;
        private:
            KFbxQuery                *mA;
            eFbxUnaryQueryOperator   mOperator;
    };

    /***********************************************
      KFbxQueryClassId -- match anywhere in the hierarchy of an object.
    ************************************************/
	/** Class to manage query class Id. It contains a class ID.
    * The class is used to create KFbxCriteria. Please see KFbxCriteria::ObjectType() for more details.
	* \nosubgrouping
    * \see KFbxQuery KFbxQueryOperator KFbxUnaryQueryOperator KFbxQueryIsA KFbxQueryProperty KFbxQueryConnectionType KFbxCriteria
	*/
    class KFBX_DLL KFbxQueryClassId : public KFbxQuery {
        //
        public:
            KFBXNEW_DECLARE_FRIEND
			/** Create a new query class Id.
			  * \param pClassId Used to create a new query class Id.
			  */
            static KFbxQueryClassId* Create(kFbxClassId pClassId);

        protected:
            KFbxQueryClassId(kFbxClassId pClassId);

        // Test functions
        public:

			//! Get unique filter Id
            virtual kFbxFilterId    GetUniqueId() const{ return 0x14572785; }

			/** Test if the given property is valid for this query class Id.
			  * \param pProperty The given property.
			  * \return \c True if valid, \c false if invalid.
			  */
            virtual bool IsValid(KFbxProperty const &pProperty) const;

			/** Test if the given property is valid for this query class Id.
			  * \param pProperty The given property.
			  * \param pType 
			  * \return \c True if valid, \c false if invalid. 
			  */
            virtual bool IsValid(KFbxProperty const &pProperty,kFbxConnectionType pType) const;

            /** Test if this query class Id is equal with the given query class Id.
			  * \param pOtherQuery The given query class Id.
			  * \return \c True if equal, \c false otherwise.
			  */
            virtual bool IsEqual(KFbxQuery *pOtherQuery)    const;
        private:
            kFbxClassId             mClassId;
    };

    /***********************************************
      KFbxQueryIsA -- Exact match.
    ************************************************/

	/** Class to manage query property. It contains a class ID.
    * The class is used to create KFbxCriteria. Please see KFbxCriteria::ObjectIsA() for more details.
	* \nosubgrouping
    * \see KFbxQuery KFbxQueryOperator KFbxUnaryQueryOperator KFbxQueryClassId KFbxQueryProperty KFbxQueryConnectionType KFbxCriteria
	*/
    class KFBX_DLL KFbxQueryIsA : public KFbxQuery {
        //
        public:
            KFBXNEW_DECLARE_FRIEND
			/** Create a new query IsA object
			  * \param pClassId 
			  */
            static KFbxQueryIsA* Create(kFbxClassId pClassId);

        protected:

			/** Constructor!
			  * \param pClassId
			  */
            KFbxQueryIsA(kFbxClassId pClassId);

        // Test functions
        public:

			//! Get unique filter Id
            virtual kFbxFilterId    GetUniqueId() const{ return 0x1457278F; }

			/** Test if the given property is valid for this query IsA.
			  * \param pProperty The given property.
			  * \return \c True if valid, \c false otherwise.
			  */
            virtual bool IsValid(KFbxProperty const &pProperty) const;

			/** Test if the given property is valid for this query IsA.
			  * \param pProperty The given property.
			  * \param pType
			  * \return \c True if valid, \c false otherwise.
			  */
            virtual bool IsValid(KFbxProperty const &pProperty,kFbxConnectionType pType) const;

			/** Test if this query is equal with the given query .
			  * \param pOtherQuery
			  * \return \c True if equal, \c false otherwise.
			  */
            virtual bool IsEqual(KFbxQuery *pOtherQuery)    const;
        private:
            kFbxClassId             mClassId;
    };

    /***********************************************
      KFbxQueryProperty
    ************************************************/

	/** Class to manage query property. It's used to create KFbxCriteria. Please see KFbxCriteria::Property() for more details.
	* \nosubgrouping
    * \see KFbxQuery KFbxQueryOperator KFbxUnaryQueryOperator KFbxQueryClassId KFbxQueryIsA KFbxQueryConnectionType KFbxCriteria
	*/
    class KFBX_DLL KFbxQueryProperty : public KFbxQuery {
        //
        public:
            KFBXNEW_DECLARE_FRIEND
			//! Create new query property
            static KFbxQueryProperty* Create();

        protected:
            KFbxQueryProperty();

        // Test functions
        public:

			//! Get unique filter Id
            virtual kFbxFilterId    GetUniqueId() const{ return 0x9348203; }

			/** Test if this query for given property is valid.
			  * \param pProperty The given property.
			  * \return \c True if valid, \c false otherwise.
			  */
            virtual bool IsValid(KFbxProperty const &pProperty) const;

			/** Test if this query for given property is valid.
			  * \param pProperty The given property.
			  * \param pType 
			  * \return \c True if valid, \c false otherwise.
			  */
            virtual bool IsValid(KFbxProperty const &pProperty,kFbxConnectionType pType) const;

			/** Return true.
			  * \param pOtherQuery
			  */
            virtual bool IsEqual(KFbxQuery *pOtherQuery)    const;
        private:
    };

    /***********************************************
      KFbxQueryConnectionType
    ************************************************/

	/** Class to manage query connection type. It contains a connection type to query. 
    * The class is used to create KFbxCriteria. Please see KFbxCriteria::ConnectionType() for more details.
	* \nosubgrouping
    * \see KFbxQuery KFbxQueryOperator KFbxUnaryQueryOperator KFbxQueryClassId KFbxQueryIsA KFbxQueryProperty KFbxQueryConnectionType KFbxCriteria
	*/
    class KFBX_DLL KFbxQueryConnectionType : public KFbxQuery {
        //
        public:
            KFBXNEW_DECLARE_FRIEND

			/** Create a new query connection type
			  * \param pConnectionType  
			  */
            static KFbxQueryConnectionType* Create(kFbxConnectionType pConnectionType);

        protected:
            KFbxQueryConnectionType(kFbxConnectionType pConnectionType);

        // Test functions
        public:

			//! Get unique filter Id
            virtual kFbxFilterId    GetUniqueId() const{ return 0x14587275; }

			/** Return true, not implemented.
			  * \param pProperty
			  * \return \c True.
			  */
            virtual bool IsValid(KFbxProperty const &pProperty) const;

			/** Test if the given connection type is valid.
			  * \param pProperty 
			  * \param pType The given connection type.
			  * \return \c True if valid, \c false otherwise.
			  */
            virtual bool IsValid(KFbxProperty const &pProperty,kFbxConnectionType pType) const;

			/** Test if this query connection type is equal with the given query connection type.
			  * \param pOtherQuery The given connection type.
			  * \return \c True, \c false otherwise.
			  */
            virtual bool IsEqual(KFbxQuery *pOtherQuery)    const;
        private:
            kFbxConnectionType              mConnectionType;
    };

    /***********************************************
      KFbxCriteria
    ************************************************/

	/** Defines a filtering criteria for a query of objects, connections
    * and properties, so that only those satisfying the criteria are
    * affected by the query. Some examples of kinds of criteria are
    * object type, connection type, or property. Criteria can be combined
    * using logical operators such as "and" and "or"
    * \note 
    * Objects are basic elements in FBX. Each of them has a hierarchy type and some properties.
    * Objects and properties can be connected through a connection to represent a relationship
    * between them. (e.g. child-parent, container membership, reference, etc.,). In a query,
    * you could select object or properties based on these criteria.
    * Here are some examples:
    * \code
    * KFbxObject* lObject = KFbxObject::Create(lManager, "Object");
    * int lSrcReferencedObjectCount = lObject->RootProperty.GetSrcObjectCount(KFbxCriteria::ConnectionType(eFbxConnectionReference));
    * int lSrcLightCount = lObject->RootProperty.GetSrcObjectCount(KFbxCriteria::ObjectType(KFbxLight::ClassId));
    * int lSrcDeformerCount = lObject->RootProperty.GetSrcObjectCount(KFbxCriteria::ObjectIsA(KFbxDeformer::ClassId));
    * int lSrcPropertyCount = lObject->RootProperty.GetSrcCount(KFbxCriteria::Property());
    * \endcode
    * \see KFbxQuery
    * \see KFbxProperty::GetSrcObjectCount(KFbxCriteria const &) const
    * \see KFbxCollection::GetMemberCount(KFbxCriteria) const
    * \see KFbxLibrary::ImportAssets(KFbxLibrary*, const KFbxCriteria&)
	* \nosubgrouping
	*/
    class KFBX_DLL KFbxCriteria {
        public:
            /** Creates a new query criteria that only selects the specific connection type.
            * \param pConnectionType The connection type to query
            */
            static KFbxCriteria ConnectionType(kFbxConnectionType pConnectionType)
            {
                return KFbxCriteria(KFbxQueryConnectionType::Create(pConnectionType));
            }

            /** Creates a new query criteria that only selects objects which have a specific
            * class ID or derive from a class with a specific class ID.
            * \param pClassId The base type class ID
            */
            static KFbxCriteria ObjectType(kFbxClassId pClassId)
            {
                return KFbxCriteria(*sCache.GetObjectType(pClassId));
            }

            /** Creates a new query criteria that only selects objects which have a specific class ID.
            * \param pClassId The type class ID
            */
            static KFbxCriteria ObjectIsA(kFbxClassId pClassId)
            {
                return KFbxCriteria(KFbxQueryIsA::Create(pClassId));
            }

            //! Creates a new query criteria that only selects properties.
            static KFbxCriteria Property()
            {
                return KFbxCriteria(KFbxQueryProperty::Create());
            }

            //! Default constructor.
            inline KFbxCriteria()
                : mQuery(0)
            {
            }

            /** Copy constructor.
            * \param pCriteria The criteria to be copied
            */
            inline KFbxCriteria(KFbxCriteria const &pCriteria)
                : mQuery(pCriteria.mQuery)
            {
                if( mQuery )
                    mQuery->Ref();
            }

            //! Destructor.
            inline ~KFbxCriteria()
            {
                if( mQuery )
                    mQuery->Unref();
            }

            /** Assignment operator.
            * \param pCriteria The criteria to be copied
            */
            inline KFbxCriteria &operator=(KFbxCriteria const &pCriteria)
            {
                if( this != &pCriteria )
                {
                    KFbxQuery* lQuery = mQuery;
                    mQuery = pCriteria.mQuery;

                    if( mQuery )
                        mQuery->Ref();

                    if( lQuery )
                        lQuery->Unref();
                }

                return *this;
            }
            /** Gets a logical conjunction (and) criteria from this and the specified criteria.
            * \param pCriteria The specified criteria
            */
            inline KFbxCriteria operator && (KFbxCriteria const &pCriteria) const
            {
                return KFbxCriteria(KFbxQueryOperator::Create(GetQuery(),eFbxAnd,pCriteria.GetQuery()));
            }

            /** Gets a logical disjunction (or) criteria from this and the specified criteria.
            * \param pCriteria The specified criteria
            */
            inline KFbxCriteria operator || (KFbxCriteria const &pCriteria) const
            {
                return KFbxCriteria(KFbxQueryOperator::Create(GetQuery(),eFbxOr,pCriteria.GetQuery()));
            }

            //! Returns a negated version of the criteria.
            inline KFbxCriteria operator !() const
            {
                return KFbxCriteria(KFbxUnaryQueryOperator::Create(GetQuery(), eFbxNot));
            }
            /** Retrieves the query.
            * \return The query of this criteria
            */
            inline KFbxQuery*   GetQuery() const { return mQuery; }

            ///////////////////////////////////////////////////////////////////////////////
            //
            //  WARNING!
            //
            //  Anything beyond these lines may not be documented accurately and is
            //  subject to change without notice.
            //
            ///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

        private:
            KFbxQuery*  mQuery;
            inline KFbxCriteria(KFbxQuery* pQuery)
                : mQuery(pQuery)
            {
            }

            // KFbxCriteriaCache caches KFbxCriteria instances for reuse
            KFBXNEW_DECLARE_FRIEND
            class KFbxCriteriaClassIdCompare
            {
            public:
                inline int operator() (kFbxClassId const& pKeyA, kFbxClassId const& pKeyB) const
                {
                    const KFbxClassIdInfo* lKeyA = pKeyA.GetClassIdInfo();
                    const KFbxClassIdInfo* lKeyB = pKeyB.GetClassIdInfo();
                    return (lKeyA > lKeyB) ? 1 : (lKeyA < lKeyB) ? -1 : 0;
                }
            };

            class KFbxCriteriaCache
            {
            public:
                KFbxCriteriaCache();
                ~KFbxCriteriaCache();

                inline KFbxCriteria* GetObjectType(kFbxClassId pClassId)
                {
                    KFbxCriteriaCacheMap::RecordType* lRecord = mObjectTypeCriteria.Find(pClassId);
                    if (lRecord == NULL)
                    {
                        KFbxCriteria* lCriteria = FbxSdkNew<KFbxCriteria>(KFbxQueryClassId::Create(pClassId));
                        mObjectTypeCriteria.Insert(pClassId, lCriteria);
                        return lCriteria;
                    }
                    return lRecord->GetValue();
                }

            private:
                typedef KMap<kFbxClassId, KFbxCriteria*, KFbxCriteriaClassIdCompare> KFbxCriteriaCacheMap;
                KFbxCriteriaCacheMap mObjectTypeCriteria;
            };
            static KFbxCriteriaCache sCache;
#endif
    };

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_FBXCORE_KFBXQUERY_H

