//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// One and only header file.
//********************************************************************

#ifndef NEWTON_H__736495_6495_076__INCLUDED_
#define NEWTON_H__736495_6495_076__INCLUDED_


#define NEWTON_MAJOR_VERSION 2 
#define NEWTON_MINOR_VERSION 00 

// force all not visual studio platform to be in library form
#ifndef _MSC_VER
	#ifndef _NEWTON_USE_LIB
		#define _NEWTON_USE_LIB
	#endif
#endif

#ifdef _NEWTON_USE_LIB
	#define NEWTON_API
#else
	#ifdef _NEWTON_BUILD_DLL
		#define NEWTON_API __declspec(dllexport)
	#else
		#define NEWTON_API __declspec(dllimport)
	#endif
#endif

//#define __USE_DOUBLE_PRECISION__
#ifdef __USE_DOUBLE_PRECISION__
	#define dFloat double
#else
	#define dFloat float
#endif



#ifdef __cplusplus 
extern "C" {
#endif


	#define NEWTON_PROFILER_WORLD_UPDATE					0

	#define NEWTON_PROFILER_COLLISION_UPDATE				1
	#define NEWTON_PROFILER_FORCE_CALLBACK_UPDATE			2
	#define NEWTON_PROFILER_COLLISION_UPDATE_BROAD_PHASE	3
	#define NEWTON_PROFILER_COLLISION_UPDATE_NARROW_PHASE	4

	#define NEWTON_PROFILER_DYNAMICS_UPDATE					5
	#define NEWTON_PROFILER_DYNAMICS_CONSTRAINT_GRAPH		6
	#define NEWTON_PROFILER_DYNAMICS_SOLVE_CONSTRAINT_GRAPH	7

	typedef struct NewtonMesh{} NewtonMesh;
	typedef struct NewtonBody{} NewtonBody;
	typedef struct NewtonWorld{} NewtonWorld;
	typedef struct NewtonJoint{} NewtonJoint;
	typedef struct NewtonMaterial{} NewtonMaterial;
	typedef struct NewtonCollision{} NewtonCollision;
	typedef struct NewtonSceneProxy{} NewtonSceneProxy;

//	typedef struct NewtonRagDoll{} NewtonRagDoll;
//	typedef struct NewtonRagDollBone{} NewtonRagDollBone;

	#define SERIALIZE_ID_BOX					0	
	#define SERIALIZE_ID_CONE					1
	#define SERIALIZE_ID_SPHERE					2
	#define SERIALIZE_ID_CAPSULE				3
	#define SERIALIZE_ID_CYLINDER				4
	#define SERIALIZE_ID_COMPOUND				5
	#define SERIALIZE_ID_CONVEXHULL				6
	#define SERIALIZE_ID_CONVEXMODIFIER			7
	#define SERIALIZE_ID_CHAMFERCYLINDER		8
	#define SERIALIZE_ID_TREE					9
	#define SERIALIZE_ID_NULL					10
	#define SERIALIZE_ID_HEIGHTFIELD			11
	#define SERIALIZE_ID_USERMESH				12
	#define SERIALIZE_ID_SCENE					13

	struct NewtonCollisionInfoRecord
	{
		dFloat m_offsetMatrix[4][4];
		int m_collisionType;				// tag id to identify the collision primitive
		int m_referenceCount;				// the current reference count for this collision		
		int m_collisionUserID;				

		struct NewtonBoxParam
		{
			dFloat m_x;
			dFloat m_y;
			dFloat m_z;
		};

		struct NewtonSphereParam
		{
			dFloat m_r0;
			dFloat m_r1;
			dFloat m_r2;
		};

		struct NewtonCylinderParam
		{
			dFloat m_r0;
			dFloat m_r1;
			dFloat m_height;
		};

		struct NewtonCapsuleParam
		{
			dFloat m_r0;
			dFloat m_r1;
			dFloat m_height;
		};

		struct NewtonConeParam
		{
			dFloat m_r;
			dFloat m_height;
		};

		struct NewtonChamferCylinderParam
		{
			dFloat m_r;
			dFloat m_height;
		};

		struct NewtonConvexHullParam
		{
			int m_vertexCount;
			int m_vertexStrideInBytes;
			int m_faceCount;
			dFloat* m_vertex;
		};

		struct NewtonConvexHullModifierParam
		{
			NewtonCollision* m_chidren;
		};


		struct NewtonCompoundCollisionParam
		{
			int m_chidrenCount;
			NewtonCollision** m_chidren;
		};

		struct NewtonCollisionTreeParam
		{
			int m_vertexCount;
			int m_indexCount;
		};

		struct NewtonHeightFieldCollisionParam
		{
			int m_width;
			int m_height;
			int m_gridsDiagonals;
			dFloat m_horizonalScale;
			dFloat m_verticalScale;
			unsigned short *m_elevation;
			char *m_atributes;
		};

		struct NewtonSceneCollisionParam
		{
			int m_childrenProxyCount;
		};

		union {
			NewtonBoxParam m_box;									
			NewtonConeParam m_cone;
			NewtonSphereParam m_sphere;
			NewtonCapsuleParam m_capsule;
			NewtonCylinderParam m_cylinder;
			NewtonChamferCylinderParam m_chamferCylinder;
			NewtonConvexHullParam m_convexHull;
			NewtonCompoundCollisionParam m_compoundCollision;
			NewtonConvexHullModifierParam m_convexHullModifier;
			NewtonCollisionTreeParam m_collisionTree;
			NewtonHeightFieldCollisionParam m_heightField;
			NewtonSceneCollisionParam m_sceneCollision;
			dFloat m_paramArray[64];		    // user define collision can use this to store information
		};
	};

	struct NewtonJointRecord
	{
		dFloat m_attachmenMatrix_0[4][4];
		dFloat m_attachmenMatrix_1[4][4];
		dFloat m_minLinearDof[3];
		dFloat m_maxLinearDof[3];
		dFloat m_minAngularDof[3];
		dFloat m_maxAngularDof[3];
		const NewtonBody* m_attachBody_0;
		const NewtonBody* m_attachBody_1;
		dFloat m_extraParameters[16];
		int	m_bodiesCollisionOn;
		char m_descriptionType[32];
	} ;


	
	struct NewtonUserMeshCollisionCollideDesc
	{
		dFloat m_boxP0[4];						// lower bounding box of intersection query in local space
		dFloat m_boxP1[4];						// upper bounding box of intersection query in local space
		int m_threadNumber;						// current thread executing this query
		int	m_faceCount;                        // the application should set here how many polygons intersect the query box
		int m_vertexStrideInBytes;              // the application should set here the size of each vertex
		void* m_userData;                       // user data passed to the collision geometry at creation time
		dFloat* m_vertex;                       // the application should the pointer to the vertex array. 
	 	int* m_userAttribute;                   // the application should set here the pointer to the user data, one for each face
		int* m_faceIndexCount;                  // the application should set here the pointer to the vertex count of each face.
		int* m_faceVertexIndex;                 // the application should set here the pointer index array for each vertex on a face.
		NewtonBody* m_objBody;                  // pointer to the colliding body
		NewtonBody* m_polySoupBody;             // pointer to the rigid body owner of this collision tree 
	};

	struct NewtonWorldConvexCastReturnInfo
	{
		dFloat m_point[4];						// collision point in global space
		dFloat m_normal[4];						// surface normal at collision point in global space
		dFloat m_normalOnHitPoint[4];           // surface normal at the surface of the hit body, 
												// is the same as the normal calculated by a ray cast hitting the body at the hit point
		dFloat m_penetration;                   // contact penetration at collision point
		int    m_contactID;	                    // collision ID at contact point
		const  NewtonBody* m_hitBody;			// body hit at contact point
	};
	
	struct NewtonUserMeshCollisionRayHitDesc
	{
		dFloat m_p0[4];							// ray origin in collision local space
		dFloat m_p1[4];                         // ray destination in collision local space   
		dFloat m_normalOut[4];					// copy here the normal at the ray intersection
		int m_userIdOut;                        // copy here a user defined id for further feedback  
		void* m_userData;                       // user data passed to the collision geometry at creation time
	};

	struct NewtonHingeSliderUpdateDesc
	{
		dFloat m_accel;
		dFloat m_minFriction;
		dFloat m_maxFriction;
		dFloat m_timestep;
	} ;


	// Newton callback functions
	typedef void* (*NewtonAllocMemory) (int sizeInBytes);
	typedef void (*NewtonFreeMemory) (void *ptr, int sizeInBytes);
	typedef void (*NewtonDestroyWorld) (const NewtonWorld* newtonWorld);

	typedef unsigned (*NewtonGetTicksCountCallback) ();

	typedef void (*NewtonSerialize) (void* serializeHandle, const void* buffer, int size);
	typedef void (*NewtonDeserialize) (void* serializeHandle, void* buffer, int size);
	
	// user collision callbacks	
	typedef void (*NewtonUserMeshCollisionDestroyCallback) (void* userData);
	typedef void (*NewtonUserMeshCollisionCollideCallback) (NewtonUserMeshCollisionCollideDesc* collideDescData);
	typedef dFloat (*NewtonUserMeshCollisionRayHitCallback) (NewtonUserMeshCollisionRayHitDesc* lineDescData);
	typedef void (*NewtonUserMeshCollisionGetCollisionInfo) (void* userData, NewtonCollisionInfoRecord* infoRecord);
	typedef int (*NewtonUserMeshCollisionGetFacesInAABB) (void* userData, const dFloat* p0, const dFloat* p1,
														   const dFloat** vertexArray, int* vertexCount, int* vertexStrideInBytes, 
		                                                   const int* indexList, int maxIndexCount, const int* userDataList);

	typedef dFloat (*NewtonCollisionTreeRayCastCallback) (dFloat interception, dFloat* normal, int faceId, void* usedData);

	
	// collision tree call back (obsoleted no recommended)
	typedef void (*NewtonTreeCollisionCallback) (const NewtonBody* bodyWithTreeCollision, const NewtonBody* body, int faceID, 
												 int vertexCount, const dFloat* vertex, int vertexStrideInBytes); 

	typedef void (*NewtonBodyDestructor) (const NewtonBody* body);
	typedef void (*NewtonApplyForceAndTorque) (const NewtonBody* body, dFloat timestep, int threadIndex);
	typedef void (*NewtonSetTransform) (const NewtonBody* body, const dFloat* matrix, int threadIndex);

	typedef void (*NewtonCollisionDestructor) (const NewtonCollision* collision);
	typedef int (*NewtonIslandUpdate) (const void* islandHandle, int bodyCount);
	typedef void (*NewtonBodyLeaveWorld) (const NewtonBody* body, int threadIndex);

	typedef int (*NewtonGetBuoyancyPlane) (const int collisionID, void *context, const dFloat* globalSpaceMatrix, dFloat* globalSpacePlane);
	typedef unsigned (*NewtonWorldRayPrefilterCallback)(const NewtonBody* body, const NewtonCollision* collision, void* userData);
	typedef dFloat (*NewtonWorldRayFilterCallback)(const NewtonBody* body, const dFloat* hitNormal, int collisionID, void* userData, dFloat intersectParam);
	

	typedef int  (*NewtonOnAABBOverlap) (const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int threadIndex);
	typedef void (*NewtonContactsProcess) (const NewtonJoint* contact, dFloat timestep, int threadIndex);

	typedef void (*NewtonBodyIterator) (const NewtonBody* body);
	typedef void (*NewtonJointIterator) (const NewtonJoint* joint);
	typedef void (*NewtonCollisionIterator) (void* userData, int vertexCount, const dFloat* faceArray, int faceId);

	typedef void (*NewtonBallCallBack) (const NewtonJoint* ball, dFloat timestep);
	typedef unsigned (*NewtonHingeCallBack) (const NewtonJoint* hinge, NewtonHingeSliderUpdateDesc* desc);
	typedef unsigned (*NewtonSliderCallBack) (const NewtonJoint* slider, NewtonHingeSliderUpdateDesc* desc);
	typedef unsigned (*NewtonUniversalCallBack) (const NewtonJoint* universal, NewtonHingeSliderUpdateDesc* desc);
	typedef unsigned (*NewtonCorkscrewCallBack) (const NewtonJoint* corkscrew, NewtonHingeSliderUpdateDesc* desc);

	typedef void (*NewtonUserBilateralCallBack) (const NewtonJoint* userJoint, dFloat timestep, int threadIndex);
	typedef void (*NewtonUserBilateralGetInfoCallBack) (const NewtonJoint* userJoint, NewtonJointRecord* info);

	typedef void (*NewtonConstraintDestructor) (const NewtonJoint* me);



//	typedef void (*NewtonSetRagDollTransform) (const NewtonRagDollBone* bone);
//	typedef void (*NewtonBodyActivationState) (const NewtonBody* body, unsigned state);
//	typedef void (*NewtonVehicleTireUpdate) (const NewtonJoint* vehicle, dFloat timestep);


	// **********************************************************************************************
	//
	// world control functions
	//
	// **********************************************************************************************
	NEWTON_API NewtonWorld* NewtonCreate (NewtonAllocMemory malloc, NewtonFreeMemory mfree);
	NEWTON_API void NewtonDestroy (const NewtonWorld* newtonWorld);
	NEWTON_API void NewtonDestroyAllBodies (const NewtonWorld* newtonWorld);
	NEWTON_API int NewtonGetMemoryUsed ();

	NEWTON_API void NewtonUpdate (const NewtonWorld* newtonWorld, dFloat timestep);
	NEWTON_API void NewtonInvalidateCache (const NewtonWorld* newtonWorld);
	NEWTON_API void NewtonCollisionUpdate (const NewtonWorld* newtonWorld);

	NEWTON_API void NewtonSetSolverModel (const NewtonWorld* newtonWorld, int model);
	NEWTON_API void NewtonSetPlatformArchitecture (const NewtonWorld* newtonWorld, int mode);
	NEWTON_API int NewtonGetPlatformArchitecture(const NewtonWorld* newtonWorld, char* description);
	NEWTON_API void NewtonSetMultiThreadSolverOnSingleIsland (const NewtonWorld* newtonWorld, int mode);
	NEWTON_API int NewtonGetMultiThreadSolverOnSingleIsland (const NewtonWorld* newtonWorld);

	NEWTON_API void NewtonSetPerformanceClock (const NewtonWorld* newtonWorld, NewtonGetTicksCountCallback callback);
	NEWTON_API unsigned NewtonReadPerformanceTicks (const NewtonWorld* newtonWorld, unsigned performanceEntry);

	
	NEWTON_API void NewtonWorldCriticalSectionLock (const NewtonWorld* newtonWorld);
	NEWTON_API void NewtonWorldCriticalSectionUnlock (const NewtonWorld* newtonWorld);
	NEWTON_API void NewtonSetThreadsCount (const NewtonWorld* newtonWorld, int threads);
	NEWTON_API int NewtonGetThreadsCount(const NewtonWorld* newtonWorld);

	NEWTON_API void NewtonSetFrictionModel (const NewtonWorld* newtonWorld, int model);
	NEWTON_API void NewtonSetMinimumFrameRate (const NewtonWorld* newtonWorld, dFloat frameRate);
	NEWTON_API void NewtonSetBodyLeaveWorldEvent (const NewtonWorld* newtonWorld, NewtonBodyLeaveWorld callback); 
	NEWTON_API void NewtonSetWorldSize (const NewtonWorld* newtonWorld, const dFloat* minPoint, const dFloat* maxPoint); 
	NEWTON_API void NewtonSetIslandUpdateEvent (const NewtonWorld* newtonWorld, NewtonIslandUpdate islandUpdate); 
	NEWTON_API void NewtonSetCollisionDestructor (const NewtonWorld* newtonWorld, NewtonCollisionDestructor callback); 
	

//	NEWTON_API void NewtonWorldForEachBodyDo (const NewtonWorld* newtonWorld, NewtonBodyIterator callback);
	NEWTON_API void NewtonWorldForEachJointDo (const NewtonWorld* newtonWorld, NewtonJointIterator callback);
	NEWTON_API void NewtonWorldForEachBodyInAABBDo (const NewtonWorld* newtonWorld, const dFloat* p0, const dFloat* p1, NewtonBodyIterator callback);

	NEWTON_API int NewtonWorldGetVersion (const NewtonWorld* newtonWorld);
	NEWTON_API void NewtonWorldSetUserData (const NewtonWorld* newtonWorld, void* userData);
	NEWTON_API void* NewtonWorldGetUserData (const NewtonWorld* newtonWorld);
	NEWTON_API void NewtonWorldSetDestructorCallBack (const NewtonWorld* newtonWorld, NewtonDestroyWorld destructor);
	NEWTON_API NewtonDestroyWorld NewtonWorldGetDestructorCallBack (const NewtonWorld* newtonWorld);

	NEWTON_API void NewtonWorldRayCast (const NewtonWorld* newtonWorld, const dFloat* p0, const dFloat* p1, NewtonWorldRayFilterCallback filter, void* userData, 
										NewtonWorldRayPrefilterCallback prefilter);
	NEWTON_API int NewtonWorldConvexCast (const NewtonWorld* newtonWorld, const dFloat* matrix, const dFloat* target, const NewtonCollision* shape, dFloat* hitParam, void* userData,  
										  NewtonWorldRayPrefilterCallback prefilter, NewtonWorldConvexCastReturnInfo* info, int maxContactsCount, int threadIndex);
									   

	// world utility functions
	NEWTON_API int NewtonWorldGetBodyCount(const NewtonWorld* newtonWorld);
	NEWTON_API int NewtonWorldGetConstraintCount(const NewtonWorld* newtonWorld);

	// NEWTON_API int NewtonGetActiveBodiesCount();
	// NEWTON_API int NewtonGetActiveConstraintsCount();
	// NEWTON_API dFloat NewtonGetGlobalScale (const NewtonWorld* newtonWorld);



	// **********************************************************************************************
	//
	// Simulation islands 
	//
	// **********************************************************************************************
	NEWTON_API NewtonBody* NewtonIslandGetBody (const void* island, int bodyIndex);
	NEWTON_API void NewtonIslandGetBodyAABB (const void* island, int bodyIndex, const dFloat* p0, const dFloat* p1);

	// **********************************************************************************************
	//
	// Physics Material Section
	//
	// **********************************************************************************************
	NEWTON_API int NewtonMaterialCreateGroupID(const NewtonWorld* newtonWorld);
	NEWTON_API int NewtonMaterialGetDefaultGroupID(const NewtonWorld* newtonWorld);
	NEWTON_API void NewtonMaterialDestroyAllGroupID(const NewtonWorld* newtonWorld);

	// material definitions that can not be overwritten in function callback
	NEWTON_API void* NewtonMaterialGetUserData (const NewtonWorld* newtonWorld, int id0, int id1);
	NEWTON_API void NewtonMaterialSetSurfaceThickness (const NewtonWorld* newtonWorld, int id0, int id1, dFloat thickness);
	NEWTON_API void NewtonMaterialSetContinuousCollisionMode (const NewtonWorld* newtonWorld, int id0, int id1, int state);
	NEWTON_API void NewtonMaterialSetCollisionCallback (const NewtonWorld* newtonWorld, int id0, int id1, void* userData,
														NewtonOnAABBOverlap aabbOverlap, NewtonContactsProcess process);

	NEWTON_API void NewtonMaterialSetDefaultSoftness (const NewtonWorld* newtonWorld, int id0, int id1, dFloat value);
	NEWTON_API void NewtonMaterialSetDefaultElasticity (const NewtonWorld* newtonWorld, int id0, int id1, dFloat elasticCoef);
	NEWTON_API void NewtonMaterialSetDefaultCollidable (const NewtonWorld* newtonWorld, int id0, int id1, int state);
	NEWTON_API void NewtonMaterialSetDefaultFriction (const NewtonWorld* newtonWorld, int id0, int id1, 
													  dFloat staticFriction, dFloat kineticFriction);

	NEWTON_API NewtonMaterial* NewtonWorldGetFirstMaterial (const NewtonWorld* world);
	NEWTON_API NewtonMaterial* NewtonWorldGetNextMaterial (const NewtonWorld* world, const NewtonMaterial* material);

	NEWTON_API NewtonBody* NewtonWorldGetFirstBody (const NewtonWorld* world);
	NEWTON_API NewtonBody* NewtonWorldGetNextBody (const NewtonWorld* world, const NewtonBody* curBody);


	// **********************************************************************************************
	//
	// Physics Contact control functions
	//
	// **********************************************************************************************
	NEWTON_API void *NewtonMaterialGetMaterialPairUserData (const NewtonMaterial* material);
	NEWTON_API unsigned NewtonMaterialGetContactFaceAttribute (const NewtonMaterial* material);
	NEWTON_API unsigned NewtonMaterialGetBodyCollisionID (const NewtonMaterial* material, const NewtonBody* body);
	NEWTON_API dFloat NewtonMaterialGetContactNormalSpeed (const NewtonMaterial* material);
	NEWTON_API void NewtonMaterialGetContactForce (const NewtonMaterial* material, dFloat* force);
	NEWTON_API void NewtonMaterialGetContactPositionAndNormal (const NewtonMaterial* material, dFloat* posit, dFloat* normal);
	NEWTON_API void NewtonMaterialGetContactTangentDirections (const NewtonMaterial* material, dFloat* dir0, dFloat* dir1);
	NEWTON_API dFloat NewtonMaterialGetContactTangentSpeed (const NewtonMaterial* material, int index);
	
	NEWTON_API void NewtonMaterialSetContactSoftness (const NewtonMaterial* material, dFloat softness);
	NEWTON_API void NewtonMaterialSetContactElasticity (const NewtonMaterial* material, dFloat restitution);
	NEWTON_API void NewtonMaterialSetContactFrictionState (const NewtonMaterial* material, int state, int index);
	NEWTON_API void NewtonMaterialSetContactFrictionCoef (const NewtonMaterial* material, dFloat staticFrictionCoef, dFloat kineticFrictionCoef, int index);
	
	NEWTON_API void NewtonMaterialSetContactNormalAcceleration (const NewtonMaterial* material, dFloat accel);
	NEWTON_API void NewtonMaterialSetContactNormalDirection (const NewtonMaterial* material, const dFloat* directionVector);

	NEWTON_API void NewtonMaterialSetContactTangentAcceleration (const NewtonMaterial* material, dFloat accel, int index);
	NEWTON_API void NewtonMaterialContactRotateTangentDirections (const NewtonMaterial* material, const dFloat* directionVector);

	

	// **********************************************************************************************
	//
	// convex collision primitives creation functions
	//
	// **********************************************************************************************
	NEWTON_API NewtonCollision* NewtonCreateNull (const NewtonWorld* newtonWorld);
	NEWTON_API NewtonCollision* NewtonCreateSphere (const NewtonWorld* newtonWorld, dFloat radiusX, dFloat radiusY, dFloat radiusZ, int shapeID, const dFloat *offsetMatrix);
	NEWTON_API NewtonCollision* NewtonCreateBox (const NewtonWorld* newtonWorld, dFloat dx, dFloat dy, dFloat dz, int shapeID, const dFloat *offsetMatrix);
	NEWTON_API NewtonCollision* NewtonCreateCone (const NewtonWorld* newtonWorld, dFloat radius, dFloat height, int shapeID, const dFloat *offsetMatrix);
	NEWTON_API NewtonCollision* NewtonCreateCapsule (const NewtonWorld* newtonWorld, dFloat radius, dFloat height, int shapeID, const dFloat *offsetMatrix);
	NEWTON_API NewtonCollision* NewtonCreateCylinder (const NewtonWorld* newtonWorld, dFloat radius, dFloat height, int shapeID, const dFloat *offsetMatrix);
	NEWTON_API NewtonCollision* NewtonCreateChamferCylinder (const NewtonWorld* newtonWorld, dFloat radius, dFloat height, int shapeID, const dFloat *offsetMatrix);
	NEWTON_API NewtonCollision* NewtonCreateConvexHull (const NewtonWorld* newtonWorld, int count, const dFloat* vertexCloud, int strideInBytes, dFloat tolerance, int shapeID, const dFloat *offsetMatrix);
	NEWTON_API NewtonCollision* NewtonCreateConvexHullFromMesh (const NewtonWorld* newtonWorld, const NewtonMesh* mesh, dFloat tolerance, int shapeID);

	NEWTON_API NewtonCollision* NewtonCreateConvexHullModifier (const NewtonWorld* newtonWorld, const NewtonCollision* convexHullCollision, int shapeID);
	NEWTON_API void NewtonConvexHullModifierGetMatrix (const NewtonCollision* convexHullCollision, dFloat* matrix);
	NEWTON_API void NewtonConvexHullModifierSetMatrix (const NewtonCollision* convexHullCollision, const dFloat* matrix);

	NEWTON_API int NewtonCollisionIsTriggerVolume(const NewtonCollision* convexCollision);
	NEWTON_API void NewtonCollisionSetAsTriggerVolume(const NewtonCollision* convexCollision, int trigger);
	NEWTON_API void NewtonCollisionSetUserID (const NewtonCollision* convexCollision, unsigned id);
	NEWTON_API unsigned NewtonCollisionGetUserID (const NewtonCollision* convexCollision);

	NEWTON_API int NewtonConvexHullGetFaceIndices (const NewtonCollision* convexHullCollision, int face, int* faceIndices);
	NEWTON_API dFloat NewtonConvexCollisionCalculateVolume (const NewtonCollision* convexCollision);
	NEWTON_API void NewtonConvexCollisionCalculateInertialMatrix (const NewtonCollision* convexCollision, dFloat* inertia, dFloat* origin);	


	NEWTON_API void NewtonCollisionMakeUnique (const NewtonWorld* newtonWorld, const NewtonCollision* collision);
	NEWTON_API void NewtonReleaseCollision (const NewtonWorld* newtonWorld, const NewtonCollision* collision);
	NEWTON_API int NewtonAddCollisionReference (const NewtonCollision* collision);

	
	// **********************************************************************************************
	//
	// mass/spring/damper collision shape
	//
	// **********************************************************************************************
//	NEWTON_API NewtonCollision* NewtonCreateSoftShape (const NewtonWorld* newtonWorld);
//	NEWTON_API void NewtonSoftBodySetMassCount (const NewtonCollision* convexCollision, int count);
//	NEWTON_API void NewtonSoftBodySetSpringCount (const NewtonCollision* convexCollision, int count);

//	NEWTON_API void NewtonSoftBodySetMass (const NewtonCollision* convexCollision, int index, dFloat mass, dFloat* position);
//	NEWTON_API int NewtonSoftBodySetSpring (const NewtonCollision* convexCollision, int index, int mass0, int mass1, dFloat stiffness, dFloat damper);
//	NEWTON_API int NewtonSoftBodyGetMassArray (const NewtonCollision* convexCollision, dFloat* masses, dFloat** positions);	


	// **********************************************************************************************
	//
	// complex collision primitives creation functions
	//
	// **********************************************************************************************
	NEWTON_API NewtonCollision* NewtonCreateCompoundCollision (const NewtonWorld* newtonWorld, int count, NewtonCollision* const collisionPrimitiveArray[], int shapeID);
	NEWTON_API NewtonCollision* NewtonCreateCompoundCollisionFromMesh (const NewtonWorld* newtonWorld, const NewtonMesh* mesh, dFloat concavity, int maxShapeCount, int shapeID);

	NEWTON_API NewtonCollision* NewtonCreateUserMeshCollision (const NewtonWorld* newtonWorld, const dFloat *minBox, 
		const dFloat *maxBox, void *userData, NewtonUserMeshCollisionCollideCallback collideCallback, 
		NewtonUserMeshCollisionRayHitCallback rayHitCallback, NewtonUserMeshCollisionDestroyCallback destroyCallback,
		NewtonUserMeshCollisionGetCollisionInfo getInfoCallback, NewtonUserMeshCollisionGetFacesInAABB facesInAABBCallback, int shapeID);

	NEWTON_API NewtonCollision* NewtonCreateSceneCollision (const NewtonWorld* newtonWorld, int shapeID);
	NEWTON_API NewtonSceneProxy* NewtonSceneCollisionCreateProxy (NewtonCollision* scene, NewtonCollision* collision);

	NEWTON_API void NewtonSceneCollisionDestroyProxy (NewtonCollision* scene, NewtonSceneProxy* Proxy);
	NEWTON_API void NewtonSceneProxySetMatrix (NewtonSceneProxy* proxy, const dFloat* matrix);
	NEWTON_API void NewtonSceneProxyGetMatrix (NewtonSceneProxy* proxy, dFloat* matrix);
	
	NEWTON_API void NewtonSceneCollisionOptimize (NewtonCollision* scene);

	//  ***********************************************************************************************************
	//
	//	Collision serialization functions
	//
	// ***********************************************************************************************************
	NEWTON_API NewtonCollision* NewtonCreateCollisionFromSerialization (const NewtonWorld* newtonWorld, NewtonDeserialize deserializeFunction, void* serializeHandle);
	NEWTON_API void NewtonCollisionSerialize (const NewtonWorld* newtonWorld, const NewtonCollision* collision, NewtonSerialize serializeFunction, void* serializeHandle);
	NEWTON_API void NewtonCollisionGetInfo (const NewtonCollision* collision, NewtonCollisionInfoRecord* collisionInfo);

	// **********************************************************************************************
	//
	// Static collision shapes functions
	//
	// **********************************************************************************************
	NEWTON_API NewtonCollision* NewtonCreateHeightFieldCollision (const NewtonWorld* newtonWorld, int width, int height, int gridsDiagonals,
																  unsigned short* elevationMap, char* attributeMap,
																  dFloat horizontalScale, dFloat verticalScale, int shapeID);
	
	NEWTON_API NewtonCollision* NewtonCreateTreeCollision (const NewtonWorld* newtonWorld, int shapeID);
	NEWTON_API void NewtonTreeCollisionSetUserRayCastCallback (const NewtonCollision* treeCollision, NewtonCollisionTreeRayCastCallback rayHitCallback);

	NEWTON_API void NewtonTreeCollisionBeginBuild (const NewtonCollision* treeCollision);
	NEWTON_API void NewtonTreeCollisionAddFace (const NewtonCollision* treeCollision, int vertexCount, const dFloat* vertexPtr, int strideInBytes, int faceAttribute);
	NEWTON_API void NewtonTreeCollisionEndBuild (const NewtonCollision* treeCollision, int optimize);

	NEWTON_API int NewtonTreeCollisionGetFaceAtribute (const NewtonCollision* treeCollision, const int* faceIndexArray); 
	NEWTON_API void NewtonTreeCollisionSetFaceAtribute (const NewtonCollision* treeCollision, const int* faceIndexArray, int attribute); 
	NEWTON_API int NewtonTreeCollisionGetVertexListIndexListInAABB (const NewtonCollision* treeCollision, const dFloat* p0, const dFloat* p1, const dFloat** vertexArray, int* vertexCount, int* vertexStrideInBytes, const int* indexList, int maxIndexCount, const int* faceAttribute); 


	NEWTON_API void NewtonStaticCollisionSetDebugCallback (const NewtonCollision* staticCollision, NewtonTreeCollisionCallback userCallback);

	// **********************************************************************************************
	//
	// General purpose collision library functions
	//
	// **********************************************************************************************
	NEWTON_API int NewtonCollisionPointDistance (const NewtonWorld* newtonWorld, const dFloat *point,
		const NewtonCollision* collision, const dFloat* matrix, dFloat* contact, dFloat* normal, int threadIndex);

	NEWTON_API int NewtonCollisionClosestPoint (const NewtonWorld* newtonWorld, 
		const NewtonCollision* collisionA, const dFloat* matrixA, const NewtonCollision* collisionB, const dFloat* matrixB,
		dFloat* contactA, dFloat* contactB, dFloat* normalAB, int threadIndex);
	
	NEWTON_API int NewtonCollisionCollide (const NewtonWorld* newtonWorld, int maxSize,
		const NewtonCollision* collisionA, const dFloat* matrixA, 
		const NewtonCollision* collisionB, const dFloat* matrixB,
		dFloat* contacts, dFloat* normals, dFloat* penetration, int threadIndex);

	NEWTON_API int NewtonCollisionCollideContinue (const NewtonWorld* newtonWorld, int maxSize, const dFloat timestep, 
		const NewtonCollision* collisionA, const dFloat* matrixA, const dFloat* velocA, const dFloat* omegaA, 
		const NewtonCollision* collisionB, const dFloat* matrixB, const dFloat* velocB, const dFloat* omegaB, 
		dFloat* timeOfImpact, dFloat* contacts, dFloat* normals, dFloat* penetration, int threadIndex);

	NEWTON_API void NewtonCollisionSupportVertex (const NewtonCollision* collision, const dFloat* dir, dFloat* vertex);
	NEWTON_API dFloat NewtonCollisionRayCast (const NewtonCollision* collision, const dFloat* p0, const dFloat* p1, dFloat* normals, int* attribute);
	NEWTON_API void NewtonCollisionCalculateAABB (const NewtonCollision* collision, const dFloat *matrix, dFloat* p0, dFloat* p1);
	NEWTON_API void NewtonCollisionForEachPolygonDo (const NewtonCollision* collision, const dFloat* matrix, NewtonCollisionIterator callback, void* userData);


	
	
	// **********************************************************************************************
	//
	// transforms utility functions
	//
	// **********************************************************************************************
	NEWTON_API void NewtonGetEulerAngle (const dFloat* matrix, dFloat* eulersAngles);
	NEWTON_API void NewtonSetEulerAngle (const dFloat* eulersAngles, dFloat* matrix);
	NEWTON_API dFloat NewtonCalculateSpringDamperAcceleration (dFloat dt, dFloat ks, dFloat x, dFloat kd, dFloat s);

	// **********************************************************************************************
	//
	// body manipulation functions
	//
	// **********************************************************************************************
	NEWTON_API NewtonBody* NewtonCreateBody (const NewtonWorld* newtonWorld, const NewtonCollision* collision);
	NEWTON_API void  NewtonDestroyBody(const NewtonWorld* newtonWorld, const NewtonBody* body);

	NEWTON_API void  NewtonBodyAddForce (const NewtonBody* body, const dFloat* force);
	NEWTON_API void  NewtonBodyAddTorque (const NewtonBody* body, const dFloat* torque);
	NEWTON_API void  NewtonBodyCalculateInverseDynamicsForce (const NewtonBody* body, dFloat timestep, const dFloat* desiredVeloc, dFloat* forceOut);

	NEWTON_API void  NewtonBodySetMatrix (const NewtonBody* body, const dFloat* matrix);
	NEWTON_API void  NewtonBodySetMatrixRecursive (const NewtonBody* body, const dFloat* matrix);
	NEWTON_API void  NewtonBodySetMassMatrix (const NewtonBody* body, dFloat mass, dFloat Ixx, dFloat Iyy, dFloat Izz);
	NEWTON_API void  NewtonBodySetMaterialGroupID (const NewtonBody* body, int id);
	NEWTON_API void  NewtonBodySetContinuousCollisionMode (const NewtonBody* body, unsigned state);
	NEWTON_API void  NewtonBodySetJointRecursiveCollision (const NewtonBody* body, unsigned state);
	NEWTON_API void  NewtonBodySetOmega (const NewtonBody* body, const dFloat* omega);
	NEWTON_API void  NewtonBodySetVelocity (const NewtonBody* body, const dFloat* velocity);
	NEWTON_API void  NewtonBodySetForce (const NewtonBody* body, const dFloat* force);
	NEWTON_API void  NewtonBodySetTorque (const NewtonBody* body, const dFloat* torque);

	NEWTON_API void  NewtonBodySetCentreOfMass  (const NewtonBody* body, const dFloat* com);
	NEWTON_API void  NewtonBodySetLinearDamping (const NewtonBody* body, dFloat linearDamp);
	NEWTON_API void  NewtonBodySetAngularDamping (const NewtonBody* body, const dFloat* angularDamp);
	NEWTON_API void  NewtonBodySetUserData (const NewtonBody* body, void* userData);
	NEWTON_API void  NewtonBodySetCollision (const NewtonBody* body, const NewtonCollision* collision);


	
	NEWTON_API int  NewtonBodyGetSleepState (const NewtonBody* body);
	NEWTON_API int  NewtonBodyGetAutoSleep (const NewtonBody* body);
	NEWTON_API void NewtonBodySetAutoSleep (const NewtonBody* body, int state);

	NEWTON_API int  NewtonBodyGetFreezeState(const NewtonBody* body);
	NEWTON_API void NewtonBodySetFreezeState (const NewtonBody* body, int state);
	

//	NEWTON_API void NewtonBodySetAutoFreeze(const NewtonBody* body, int state);
//	NEWTON_API void NewtonBodyCoriolisForcesMode (const NewtonBody* body, int mode);
//	NEWTON_API void NewtonBodySetGyroscopicForcesMode (const NewtonBody* body, int mode);
//	NEWTON_API int  NewtonBodyGetGyroscopicForcesMode (const NewtonBody* body);
//	NEWTON_API int  NewtonBodyGetFreezeState (const NewtonBody* body);
//	NEWTON_API void NewtonBodySetFreezeState  (const NewtonBody* body, int state);
//	NEWTON_API void NewtonBodyGetFreezeTreshold (const NewtonBody* body, dFloat* freezeSpeed2, dFloat* freezeOmega2);
//	NEWTON_API void NewtonBodySetFreezeTreshold (const NewtonBody* body, dFloat freezeSpeed2, dFloat freezeOmega2, int framesCount);
//	NEWTON_API void NewtonBodySetAutoactiveCallback (const NewtonBody* body, NewtonBodyActivationState callback);
	

	NEWTON_API void  NewtonBodySetDestructorCallback (const NewtonBody* body, NewtonBodyDestructor callback);

	NEWTON_API void  NewtonBodySetTransformCallback (const NewtonBody* body, NewtonSetTransform callback);
	NEWTON_API NewtonSetTransform NewtonBodyGetTransformCallback (const NewtonBody* body);
	
	NEWTON_API void  NewtonBodySetForceAndTorqueCallback (const NewtonBody* body, NewtonApplyForceAndTorque callback);
	NEWTON_API NewtonApplyForceAndTorque NewtonBodyGetForceAndTorqueCallback (const NewtonBody* body);
	NEWTON_API void* NewtonBodyGetUserData (const NewtonBody* body);
	NEWTON_API NewtonWorld* NewtonBodyGetWorld (const NewtonBody* body);
	NEWTON_API NewtonCollision* NewtonBodyGetCollision (const NewtonBody* body);
	NEWTON_API int   NewtonBodyGetMaterialGroupID (const NewtonBody* body);

	NEWTON_API int   NewtonBodyGetContinuousCollisionMode (const NewtonBody* body);
	NEWTON_API int   NewtonBodyGetJointRecursiveCollision (const NewtonBody* body);

	NEWTON_API void  NewtonBodyGetMatrix(const NewtonBody* body, dFloat* matrix);
	NEWTON_API void  NewtonBodyGetRotation(const NewtonBody* body, dFloat* rotation);
	NEWTON_API void  NewtonBodyGetMassMatrix (const NewtonBody* body, dFloat* mass, dFloat* Ixx, dFloat* Iyy, dFloat* Izz);
	NEWTON_API void  NewtonBodyGetInvMass(const NewtonBody* body, dFloat* invMass, dFloat* invIxx, dFloat* invIyy, dFloat* invIzz);
	NEWTON_API void  NewtonBodyGetOmega(const NewtonBody* body, dFloat* vector);
	NEWTON_API void  NewtonBodyGetVelocity(const NewtonBody* body, dFloat* vector);
	NEWTON_API void  NewtonBodyGetForce(const NewtonBody* body, dFloat* vector);
	NEWTON_API void  NewtonBodyGetTorque(const NewtonBody* body, dFloat* vector);
	NEWTON_API void  NewtonBodyGetForceAcc(const NewtonBody* body, dFloat* vector);
	NEWTON_API void  NewtonBodyGetTorqueAcc(const NewtonBody* body, dFloat* vector);
	NEWTON_API void  NewtonBodyGetCentreOfMass (const NewtonBody* body, dFloat* com);

	NEWTON_API dFloat NewtonBodyGetLinearDamping (const NewtonBody* body);
	NEWTON_API void  NewtonBodyGetAngularDamping (const NewtonBody* body, dFloat* vector);
	NEWTON_API void  NewtonBodyGetAABB (const NewtonBody* body, dFloat* p0, dFloat* p1);
	NEWTON_API NewtonJoint* NewtonBodyGetFirstJoint (const NewtonBody* body);
	NEWTON_API NewtonJoint* NewtonBodyGetNextJoint (const NewtonBody* body, const NewtonJoint* joint);
	NEWTON_API NewtonJoint* NewtonBodyGetFirstContactJoint (const NewtonBody* body);
	NEWTON_API NewtonJoint* NewtonBodyGetNextContactJoint (const NewtonBody* body, const NewtonJoint* contactJoint);

	NEWTON_API void* NewtonContactJointGetFirstContact (const NewtonJoint* contactJoint);
	NEWTON_API void* NewtonContactJointGetNextContact (const NewtonJoint* contactJoint, void* contact);

	NEWTON_API int NewtonContactJointGetContactCount(const NewtonJoint* contactJoint);
	NEWTON_API void NewtonContactJointRemoveContact(const NewtonJoint* contactJoint, void* contact); 

	NEWTON_API NewtonMaterial* NewtonContactGetMaterial (const void* contact);




	
	NEWTON_API void  NewtonBodyAddBuoyancyForce (const NewtonBody* body, dFloat fluidDensity, 
		dFloat fluidLinearViscosity, dFloat fluidAngularViscosity, 
		const dFloat* gravityVector, NewtonGetBuoyancyPlane buoyancyPlane, void *context);

//	NEWTON_API void NewtonBodyForEachPolygonDo (const NewtonBody* body, NewtonCollisionIterator callback);
	NEWTON_API void NewtonBodyAddImpulse (const NewtonBody* body, const dFloat* pointDeltaVeloc, const dFloat* pointPosit);


	// **********************************************************************************************
	//
	// Common joint functions
	//
	// **********************************************************************************************
	NEWTON_API void* NewtonJointGetUserData (const NewtonJoint* joint);
	NEWTON_API void NewtonJointSetUserData (const NewtonJoint* joint, void* userData);

	NEWTON_API NewtonBody* NewtonJointGetBody0 (const NewtonJoint* joint);
	NEWTON_API NewtonBody* NewtonJointGetBody1 (const NewtonJoint* joint);

	NEWTON_API void NewtonJointGetInfo  (const NewtonJoint* joint, NewtonJointRecord* info);
	NEWTON_API int NewtonJointGetCollisionState (const NewtonJoint* joint);
	NEWTON_API void NewtonJointSetCollisionState (const NewtonJoint* joint, int state);

	NEWTON_API dFloat NewtonJointGetStiffness (const NewtonJoint* joint);
	NEWTON_API void NewtonJointSetStiffness (const NewtonJoint* joint, dFloat state);

	NEWTON_API void NewtonDestroyJoint(const NewtonWorld* newtonWorld, const NewtonJoint* joint);
	NEWTON_API void NewtonJointSetDestructor (const NewtonJoint* joint, NewtonConstraintDestructor destructor);

	


	// **********************************************************************************************
	//
	// Ball and Socket joint functions
	//
	// **********************************************************************************************
	NEWTON_API NewtonJoint* NewtonConstraintCreateBall (const NewtonWorld* newtonWorld, const dFloat* pivotPoint, 
														const NewtonBody* childBody, const NewtonBody* parentBody);
	NEWTON_API void NewtonBallSetUserCallback (const NewtonJoint* ball, NewtonBallCallBack callback);
	NEWTON_API void NewtonBallGetJointAngle (const NewtonJoint* ball, dFloat* angle);
	NEWTON_API void NewtonBallGetJointOmega (const NewtonJoint* ball, dFloat* omega);
	NEWTON_API void NewtonBallGetJointForce (const NewtonJoint* ball, dFloat* force);
	NEWTON_API void NewtonBallSetConeLimits (const NewtonJoint* ball, const dFloat* pin, dFloat maxConeAngle, dFloat maxTwistAngle);

	// **********************************************************************************************
	//
	// Hinge joint functions
	//
	// **********************************************************************************************
	NEWTON_API NewtonJoint* NewtonConstraintCreateHinge (const NewtonWorld* newtonWorld, 
		const dFloat* pivotPoint, const dFloat* pinDir, 
		const NewtonBody* childBody, const NewtonBody* parentBody);

	NEWTON_API void NewtonHingeSetUserCallback (const NewtonJoint* hinge, NewtonHingeCallBack callback);
	NEWTON_API dFloat NewtonHingeGetJointAngle (const NewtonJoint* hinge);
	NEWTON_API dFloat NewtonHingeGetJointOmega (const NewtonJoint* hinge);
	NEWTON_API void NewtonHingeGetJointForce (const NewtonJoint* hinge, dFloat* force);
	NEWTON_API dFloat NewtonHingeCalculateStopAlpha (const NewtonJoint* hinge, const NewtonHingeSliderUpdateDesc* desc, dFloat angle);

	// **********************************************************************************************
	//
	// Slider joint functions
	//
	// **********************************************************************************************
	NEWTON_API NewtonJoint* NewtonConstraintCreateSlider (const NewtonWorld* newtonWorld, 
		const dFloat* pivotPoint, const dFloat* pinDir, 
		const NewtonBody* childBody, const NewtonBody* parentBody);
	NEWTON_API void NewtonSliderSetUserCallback (const NewtonJoint* slider, NewtonSliderCallBack callback);
	NEWTON_API dFloat NewtonSliderGetJointPosit (const NewtonJoint* slider);
	NEWTON_API dFloat NewtonSliderGetJointVeloc (const NewtonJoint* slider);
	NEWTON_API void NewtonSliderGetJointForce (const NewtonJoint* slider, dFloat* force);
	NEWTON_API dFloat NewtonSliderCalculateStopAccel (const NewtonJoint* slider, const NewtonHingeSliderUpdateDesc* desc, dFloat position);


	// **********************************************************************************************
	//
	// Corkscrew joint functions
	//
	// **********************************************************************************************
	NEWTON_API NewtonJoint* NewtonConstraintCreateCorkscrew (const NewtonWorld* newtonWorld, 
		const dFloat* pivotPoint, const dFloat* pinDir, 
		const NewtonBody* childBody, const NewtonBody* parentBody);
	NEWTON_API void NewtonCorkscrewSetUserCallback (const NewtonJoint* corkscrew, NewtonCorkscrewCallBack callback);
	NEWTON_API dFloat NewtonCorkscrewGetJointPosit (const NewtonJoint* corkscrew);
	NEWTON_API dFloat NewtonCorkscrewGetJointAngle (const NewtonJoint* corkscrew);
	NEWTON_API dFloat NewtonCorkscrewGetJointVeloc (const NewtonJoint* corkscrew);
	NEWTON_API dFloat NewtonCorkscrewGetJointOmega (const NewtonJoint* corkscrew);
	NEWTON_API void NewtonCorkscrewGetJointForce (const NewtonJoint* corkscrew, dFloat* force);
	NEWTON_API dFloat NewtonCorkscrewCalculateStopAlpha (const NewtonJoint* corkscrew, const NewtonHingeSliderUpdateDesc* desc, dFloat angle);
	NEWTON_API dFloat NewtonCorkscrewCalculateStopAccel (const NewtonJoint* corkscrew, const NewtonHingeSliderUpdateDesc* desc, dFloat position);


	// **********************************************************************************************
	//
	// Universal joint functions
	//
	// **********************************************************************************************
	NEWTON_API NewtonJoint* NewtonConstraintCreateUniversal (const NewtonWorld* newtonWorld, 
		const dFloat* pivotPoint, const dFloat* pinDir0, const dFloat* pinDir1, 
		const NewtonBody* childBody, const NewtonBody* parentBody);
	NEWTON_API void NewtonUniversalSetUserCallback (const NewtonJoint* universal, NewtonUniversalCallBack callback);
	NEWTON_API dFloat NewtonUniversalGetJointAngle0 (const NewtonJoint* universal);
	NEWTON_API dFloat NewtonUniversalGetJointAngle1 (const NewtonJoint* universal);
	NEWTON_API dFloat NewtonUniversalGetJointOmega0 (const NewtonJoint* universal);
	NEWTON_API dFloat NewtonUniversalGetJointOmega1 (const NewtonJoint* universal);
	NEWTON_API void NewtonUniversalGetJointForce (const NewtonJoint* universal, dFloat* force);
	NEWTON_API dFloat NewtonUniversalCalculateStopAlpha0 (const NewtonJoint* universal, const NewtonHingeSliderUpdateDesc* desc, dFloat angle);
	NEWTON_API dFloat NewtonUniversalCalculateStopAlpha1 (const NewtonJoint* universal, const NewtonHingeSliderUpdateDesc* desc, dFloat angle);


	// **********************************************************************************************
	//
	// Up vector joint functions
	//
	// **********************************************************************************************
	NEWTON_API NewtonJoint* NewtonConstraintCreateUpVector (const NewtonWorld* newtonWorld, const dFloat* pinDir, const NewtonBody* body); 
	NEWTON_API void NewtonUpVectorGetPin (const NewtonJoint* upVector, dFloat *pin);
	NEWTON_API void NewtonUpVectorSetPin (const NewtonJoint* upVector, const dFloat *pin);


	// **********************************************************************************************
	//
	// User defined bilateral Joint
	//
	// **********************************************************************************************
	NEWTON_API NewtonJoint* NewtonConstraintCreateUserJoint (const NewtonWorld* newtonWorld, int maxDOF, 
															 NewtonUserBilateralCallBack callback,
															 NewtonUserBilateralGetInfoCallBack getInfo,
															 const NewtonBody* childBody, const NewtonBody* parentBody) ; 
	NEWTON_API void NewtonUserJointSetFeedbackCollectorCallback (const NewtonJoint* joint, NewtonUserBilateralCallBack getFeedback);
	NEWTON_API void NewtonUserJointAddLinearRow (const NewtonJoint* joint, const dFloat *pivot0, const dFloat *pivot1, const dFloat *dir);
	NEWTON_API void NewtonUserJointAddAngularRow (const NewtonJoint* joint, dFloat relativeAngle, const dFloat *dir);
	NEWTON_API void NewtonUserJointAddGeneralRow (const NewtonJoint* joint, const dFloat *jacobian0, const dFloat *jacobian1);
	NEWTON_API void NewtonUserJointSetRowMinimumFriction (const NewtonJoint* joint, dFloat friction);
	NEWTON_API void NewtonUserJointSetRowMaximumFriction (const NewtonJoint* joint, dFloat friction);
	NEWTON_API void NewtonUserJointSetRowAcceleration (const NewtonJoint* joint, dFloat acceleration);
	NEWTON_API void NewtonUserJointSetRowSpringDamperAcceleration (const NewtonJoint* joint, dFloat springK, dFloat springD);
	NEWTON_API void NewtonUserJointSetRowStiffness (const NewtonJoint* joint, dFloat stiffness);
	NEWTON_API dFloat NewtonUserJointGetRowForce (const NewtonJoint* joint, int row);
	

	// **********************************************************************************************
	//
	// Mesh joint functions
	//
	// **********************************************************************************************
	NEWTON_API NewtonMesh* NewtonMeshCreate();
	NEWTON_API NewtonMesh* NewtonMeshCreateFromCollision(const NewtonCollision* collision);
	NEWTON_API void NewtonMeshDestroy(const NewtonMesh*mesh);

	NEWTON_API void NewtonMeshCalculateVertexNormals(const NewtonMesh* mesh, dFloat angleInRadians);
	NEWTON_API void NewtonMeshApplySphericalMapping(const NewtonMesh* mesh, int material);
	NEWTON_API void NewtonMeshApplyBoxMapping(const NewtonMesh* mesh, int front, int side, int top);
	NEWTON_API void NewtonMeshApplyCylindricalMapping(const NewtonMesh* mesh, int cylinderMaterial, int capMaterial);
	

	NEWTON_API int NewtonMeshPlaneClip (const NewtonMesh* mesh, 
									    const dFloat* plane, const dFloat* textureProjectionMatrix, 
										NewtonMesh** meshOut, int maxMeshCount, int capMaterial);

	NEWTON_API void NewtonMeshBeginFace(const NewtonMesh *mesh);
	NEWTON_API void NewtonMeshAddFace(const NewtonMesh *mesh, int vertexCount, const dFloat* vertex, int strideInBytes, int materialIndex);
	NEWTON_API void NewtonMeshEndFace(const NewtonMesh *mesh);

	NEWTON_API int NewtonMeshGetVertexCount (const NewtonMesh *mesh); 
	NEWTON_API void NewtonMeshGetVertexStreams (const NewtonMesh *mesh, 
												int vertexStrideInByte, dFloat* vertex,
												int normalStrideInByte, dFloat* normal,
												int uvStrideInByte, dFloat* uv);
	NEWTON_API void NewtonMeshGetIndirectVertexStreams(const NewtonMesh *mesh, 
													   int vertexStrideInByte, dFloat* vertex, int* vertexIndices, int* vertexCount,
													   int normalStrideInByte, dFloat* normal, int* normalIndices, int* normalCount,
													   int uvStrideInByte, dFloat* uv, int* uvIndices, int* uvCount);

	NEWTON_API int NewtonMeshFirstMaterial (const NewtonMesh *mesh); 
	NEWTON_API int NewtonMeshNextMaterial (const NewtonMesh *mesh, int materialHandle); 
	NEWTON_API int NewtonMeshMaterialGetMaterial (const NewtonMesh *mesh, int materialHandle); 
	NEWTON_API int NewtonMeshMaterialGetIndexCount (const NewtonMesh *mesh, int materialHandle); 
	NEWTON_API void NewtonMeshMaterialGetIndexStream (const NewtonMesh *mesh, int materialHandle, int* index); 
	NEWTON_API void NewtonMeshMaterialGetIndexStreamShort (const NewtonMesh *mesh, int materialHandle, short int* index); 


	// **********************************************************************************************
	//
	// Rag doll joint container functions
	//
	// **********************************************************************************************
	//	NEWTON_API NewtonRagDoll* NewtonCreateRagDoll (const NewtonWorld* newtonWorld);
	//	NEWTON_API void NewtonDestroyRagDoll (const NewtonWorld* newtonWorld, const NewtonRagDoll* ragDoll);
	//	NEWTON_API void NewtonRagDollBegin (const NewtonRagDoll* ragDoll);
	//	NEWTON_API void NewtonRagDollEnd (const NewtonRagDoll* ragDoll);
	//	NEWTON_API NewtonRagDollBone* NewtonRagDollFindBone (const NewtonRagDoll* ragDoll, int id);
	//	NEWTON_API void NewtonRagDollSetForceAndTorqueCallback (const NewtonRagDoll* ragDoll, NewtonApplyForceAndTorque callback);
	//	NEWTON_API void NewtonRagDollSetTransformCallback (const NewtonRagDoll* ragDoll, NewtonSetRagDollTransform callback);
	//	NEWTON_API NewtonRagDollBone* NewtonRagDollAddBone (const NewtonRagDoll* ragDoll, const NewtonRagDollBone* parent, 
	//		void *userData, dFloat mass, const dFloat* matrix, 
	//		const NewtonCollision* boneCollision, const dFloat* size);
	//	NEWTON_API void* NewtonRagDollBoneGetUserData (const NewtonRagDollBone* bone);
	//	NEWTON_API NewtonBody* NewtonRagDollBoneGetBody (const NewtonRagDollBone* bone);
	//	NEWTON_API void NewtonRagDollBoneSetID (const NewtonRagDollBone* bone, int id);
	//	NEWTON_API void NewtonRagDollBoneSetLimits (const NewtonRagDollBone* bone, 
	//		const dFloat* coneDir, dFloat minConeAngle, dFloat maxConeAngle, dFloat maxTwistAngle,
	//		const dFloat* bilateralConeDir, dFloat negativeBilateralConeAngle, dFloat positiveBilateralConeAngle);

	//	NEWTON_API void NewtonRagDollBoneGetLocalMatrix (const NewtonRagDollBone* bone, dFloat* matrix);
	//	NEWTON_API void NewtonRagDollBoneGetGlobalMatrix (const NewtonRagDollBone* bone, dFloat* matrix);


	// **********************************************************************************************
	//
	// Vehicle joint functions
	//
	// **********************************************************************************************
	//	NEWTON_API NewtonJoint* NewtonConstraintCreateVehicle (const NewtonWorld* newtonWorld, const dFloat* upDir, const NewtonBody* body); 
	//	NEWTON_API void NewtonVehicleReset (const NewtonJoint* vehicle); 
	//	NEWTON_API void NewtonVehicleSetTireCallback (const NewtonJoint* vehicle, NewtonVehicleTireUpdate update);
	//	NEWTON_API void* NewtonVehicleAddTire (const NewtonJoint* vehicle, const dFloat* localMatrix, const dFloat* pin, dFloat mass, dFloat width, dFloat radius, dFloat suspesionShock, dFloat suspesionSpring, dFloat suspesionLength, void* userData, int collisionID);
	//	NEWTON_API void NewtonVehicleRemoveTire (const NewtonJoint* vehicle, void* tireId);
	//	NEWTON_API void* NewtonVehicleGetFirstTireID (const NewtonJoint* vehicle);
	//	NEWTON_API void* NewtonVehicleGetNextTireID (const NewtonJoint* vehicle, void* tireId);
	//	NEWTON_API int NewtonVehicleTireIsAirBorne (const NewtonJoint* vehicle, void* tireId);
	//	NEWTON_API int NewtonVehicleTireLostSideGrip (const NewtonJoint* vehicle, void* tireId);
	//	NEWTON_API int NewtonVehicleTireLostTraction (const NewtonJoint* vehicle, void* tireId);
	//	NEWTON_API void* NewtonVehicleGetTireUserData (const NewtonJoint* vehicle, void* tireId);
	//	NEWTON_API dFloat NewtonVehicleGetTireOmega (const NewtonJoint* vehicle, void* tireId);
	//	NEWTON_API dFloat NewtonVehicleGetTireNormalLoad (const NewtonJoint* vehicle, void* tireId);
	//	NEWTON_API dFloat NewtonVehicleGetTireSteerAngle (const NewtonJoint* vehicle, void* tireId);
	//	NEWTON_API dFloat NewtonVehicleGetTireLateralSpeed (const NewtonJoint* vehicle, void* tireId);
	//	NEWTON_API dFloat NewtonVehicleGetTireLongitudinalSpeed (const NewtonJoint* vehicle, void* tireId);
	//	NEWTON_API void NewtonVehicleGetTireMatrix (const NewtonJoint* vehicle, void* tireId, dFloat* matrix);
	//	NEWTON_API void NewtonVehicleSetTireTorque (const NewtonJoint* vehicle, void* tireId, dFloat torque);
	//	NEWTON_API void NewtonVehicleSetTireSteerAngle (const NewtonJoint* vehicle, void* tireId, dFloat angle);
	//	NEWTON_API void NewtonVehicleSetTireMaxSideSleepSpeed (const NewtonJoint* vehicle, void* tireId, dFloat speed);
	//	NEWTON_API void NewtonVehicleSetTireSideSleepCoeficient (const NewtonJoint* vehicle, void* tireId, dFloat coeficient);
	//	NEWTON_API void NewtonVehicleSetTireMaxLongitudinalSlideSpeed (const NewtonJoint* vehicle, void* tireId, dFloat speed);
	//	NEWTON_API void NewtonVehicleSetTireLongitudinalSlideCoeficient (const NewtonJoint* vehicle, void* tireId, dFloat coeficient);
	//	NEWTON_API dFloat NewtonVehicleTireCalculateMaxBrakeAcceleration (const NewtonJoint* vehicle, void* tireId);
	//	NEWTON_API void NewtonVehicleTireSetBrakeAcceleration (const NewtonJoint* vehicle, void* tireId, dFloat accelaration, dFloat torqueLimit);



#ifdef __cplusplus 
}
#endif
#endif


