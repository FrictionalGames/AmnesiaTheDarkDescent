
//! File: <CollisionState.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_COLLISIONSTATE_H_
#define _HAPTX_COLLISIONSTATE_H_

#include "ForceModel.h"
#include "StateStack.h"
#include "mgTime.h"
//! TODO(ida): #include "TextureTransformBase.h"

namespace HaptX 
{
	// forward declaration.
	class Surface; 
	class ImpulseBase; 
	class HDynamic; 
	class HapticsManager; 
	class HapticsDevice;
	class RealtimeGeometry;

	// The CollisionState object is used to communicate data between the HaptX thread and the main thread.
	// Since each HapticsDevice has its own HaptX thread it also has its own CollisionState instance.
	// To register a Shape for HaptX rendering, a call to AddCollision in the CollisionState must be made.
	// Before adding a Shape the current state must be set, which is done by setting a Surface, an ImpulseBase 
	// and pushing a matrix onto the collision stack. To register a ForceModel for HaptX rendering, 
	// a call to AddAbsoluteGlobalFM or AddAbsoluteLocalFM must be made.
	class HAPTX_SOLVER_SPEC CollisionState 
	{
	public:
		inline CollisionState(): m_forceModels(-1){}
		// InitLayer sets up a proxy layer.
		void InitLayer(HapticsManager* _hm, const Vec3f& _proxy, const Vec3f& _new_proxy, const Vec3f& _drawn_proxy, const mgFloat& _radius);
		void Initialize(const Vec3f& _finger, const Time& _delta_t);				// Initialize sets up the state for a traversal.

		void AddCollision(RealtimeGeometry* rg);									// AddCollision registers a new realtime geometry with the CollisionState. 
		void SetAddCollisionEnabled(bool value);									// Disable/enable the AddCollision function. Enabled by default.
		bool GetAddCollisionEnabled();												// Get the state of the AddCollision function.

		// Adds a ForceModel which uses the global coordinate system as reference.
		void AddAbsoluteGlobalFM(ForceModel* fm);		
		void AddRelativeGlobalFM(ForceModel* fm);		// Currently not in use.

		// Adds a ForceModel which uses the matrix on top of the stack as reference, 
		// which includes camera transformation.
		void AddAbsoluteLocalFM(ForceModel* fm);		
		void AddRelativeLocalFM(ForceModel* fm);		// Currently not in use.

		// Get the current proxy layer. The first time a layer (apart from the zero layer) 
		// is used on a particular HapticsDevice, it is ignored until the following 
		// collision traversal. See also SetLayer.
		inline int GetLayer() const;

		// Set the current proxy layer. The first time a layer (apart from the zero layer) 
		// is used on a particular HapticsDevice, it is ignored until the following 
		// collision traversal. See also GetLayer.
		inline void SetLayer(const int& layer);

		// Get the number of proxy layers requested by the current traversal. 
		inline int GetRequestedLayers() const;

		// Get the accumulated forward matrix from the top of the state stack.
		// See also GetAccumulatedInverse GetLastAccumulatedForward GetLastAccumulatedInverse
		inline const Matrix4f& GetAccumulatedForward() const;

		// Get the accumulated inverse matrix from the top of the state stack.
		// See also GetAccumulatedForward GetLastAccumulatedForward GetLastAccumulatedInverse
		inline const Matrix4f& GetAccumulatedInverse() const;

		// Get the accumulated forward matrix from the previous	collision traversal. 
		// See also GetLastAccumulatedInverse GetAccumulatedForward GetAccumulatedInverse
		inline const Matrix4f& GetLastAccumulatedForward() const;

		// Get the accumulated inverse matrix from the previous collision traversal. 
		// See also GetLastAccumulatedForward GetAccumulatedForward GetAccumulatedInverse 
		inline const Matrix4f& GetLastAccumulatedInverse() const;

		// Push is called when we move into a nested coordinate space. m is the coordinate 
		// space transform associated with the nested space and m_inv should be its inverse. 
		// last_frw and last_inv are the accumulated forward and inverse transforms associated 
		// with this coordinate space at the previous CollisionState traversal.
		void Push(const Matrix4f& m, const Matrix4f& m_inv, const Matrix4f& last_frw, const Matrix4f& last_inv) throw(Error::EmptyStack);

		// Pop is called when we move out of a nested coordinate space. 
		void Pop() throw(Error::EmptyStack);

		inline const Vec3f&				GetLastProxy() const;						// Get the state values.
		inline const Vec3f&				GetProxy() const;							// Get the state values.
		inline const Vec3f&				GetNewProxy() const;						// Get the state values.
		inline Vec3f					GetDrawnProxy() const;						// Get the state values.
		inline const Vec3f&				GetFinger() const;							// Get the state values.
		inline mgFloat					GetRadius() const;							// Get the state values.
		std::vector<RealtimeGeometry*>&	GetRealtimeGeometries();					// Get the state values.
		std::vector<RealtimeGeometry*>&	GetRealtimeGeometries(const int &_layer);	// Get the state values.
		inline ForceModelSum&			GetForceModels();							// Get the state values.
		inline const Time&				GetDeltaT() const;							// Get the state values.
		inline int&						GetNumTriangles();							// Get the state values.
		
		inline const Vec3f&				GetGlobalProxy() const;						// Get the global values.
		inline const Vec3f&				GetGlobalDrawnProxy() const;				// Get the global values.
		inline const Vec3f&				GetGlobalFinger() const;					// Get the global values.
		inline const mgFloat&			GetGlobalRadius() const;					// Get the global values.

		inline Surface*					GetSurface();								// Get the current Surface instance. Note: the surface pointer may be zero. 
		inline void						SetSurface(Surface* a);						// Set the current Surface instance. Note: the surface pointer may be zero.
		inline int						GetTextureDimension();						// Get the dimensions of the current texture.
		inline void						SetTextureDimension(int d);					// Set the dimensions of the current texture.
		inline ImpulseBase*				GetImpulseBase();							// Get the current ImpulseBase object. 
		inline void						SetImpulseBase(ImpulseBase *ib);			// Set the current ImpulseBase object. 

		// GetScale returns the scale component of the transform from
		// global to local space. If the scaling is non-uniform then 
		// the largest of the three scaling factors will be returned. 
		inline const mgFloat& GetScale() const;

		// GetUniformity returns the uniformity of the current accumulated inverse transform. 
		// Uniform transforms will have a uniformity close to 1. Non-uniform scaling changes 
		// the shape of objects whereas uniform scaling only changes the size. 
		inline mgFloat GetUniformity() const;
		
		// GetDevice returns the HapticsDevice being used for this collision traversal.
		// NOTE: If a dummy traversal is being performed, then the return value will be NULL.
		HapticsDevice* GetDevice();

		// GetDeviceIndex returns the device id of the HapticsDevice being used for this collision traversal.
		// NOTE: If a dummy traversal is being performed, then the device index will be -1. In this case, no actual 
		// collisions need to be added. 
		int GetDeviceIndex();

		//! get the current texture transform node. note: the pointer may be zero.
		//! TODO (lukas): inline TextureTransformBase *getTextureTransform();

		//! set the current texture transform node. note: the pointer may be zero. 
		//! TODO (lukas): inline void setTextureTransform( TextureTransformBase *a );
		
	protected:
		// StackElement is the type stored in the StateStack. The end-points and radius of 
		// the collision line segment are transformed as we move into different coordinate spaces.
		// The current accumulated forward and inverse transform matrices are stored 
		// as well as the accumulated and inverse matrices from the previous traversal. 
		// The forward matrix transforms from local space to global space.
		// Note that m_frw * m_lastInv represents the motion of the local
		// space within the global space and m_inv * m_lastFrw represents
		// the motion of the global space within the local space. 
		struct HAPTX_SOLVER_SPEC StackElement
		{
			Matrix4f	m_frw;				// The forward matrix.
			Matrix4f	m_inv;				// The inverse matrix.
			Matrix4f	m_lastFrw;			// The last forward matrix.
			Matrix4f	m_lastInv;			// The last inverse matrix.

			Time		m_timestamp;		// The time when the m_lastFrw and m_lastInv matrices were calculated. 

			Vec3f		m_lastProxy;		// The last position of the proxy.
			Vec3f		m_proxy;			// The position of the proxy.
			Vec3f		m_finger;			// The last position of the finger.

			mgFloat		m_scale; 			// The largest scaling factor of the inv matrix. 
			mgFloat		m_uniformity; 		// The ratio-squared of the smallest scaling component to the largest of the accumulated inverse transformation. 
			int			m_layer;			// The current proxy layer index.
		};

	protected:
		StateStack<StackElement>		m_stateStack;			// The coordinate space stack. 

		bool							m_addCollisionEnabled;  // A flag that can be used to enable or disable the possibility to add collision geometries.

		int								m_textureDimension;		// The number of dimensions of the current texture 
		int								m_maxLayers;			// The maximum number of allowed proxy layers in this traversal.
		int								m_reqLayers;			// The requested number of layers in this traversal. 
		
		std::vector<HapticsManager*>	m_hm;					// Data is stored per-layer.
		std::vector<Vec3f>				m_proxy;				// Data is stored per-layer.
		std::vector<Vec3f>				m_drawnProxy;			// Data is stored per-layer.
		std::vector<mgFloat>			m_radius;				// Data is stored per-layer.
		ForceModelSum					m_forceModels;			// Data is stored per-layer.

		Vec3f							m_finger;				// The unconstrained device position.

		Surface*						m_surface;				// The current Surface instance. this may be 0. 
		ImpulseBase*					m_impulseBase;			// The current ImpulseBase object. this may be 0. 
		//! TODO (ida): TextureTransformBase* m_texture_transform; //! the current TextureTransform. this may be 0. 
		
		Time							m_deltaT;				// The change in time since the last traversal of the scene. 
	};

	//! Get the current proxy layer. The first time a layer (apart
	//! from the zero layer) is used on a particular HapticsDevice, it
	//! is ignored until the following collision traversal. 
	inline int CollisionState::GetLayer() const 
	{
		return m_stateStack.Top().m_layer;
	}

	//! Get the number of proxy layers requested by the current traversal. 
	inline int CollisionState::GetRequestedLayers() const 
	{
		return m_reqLayers;
	}

	//! Get the accumulated forward matrix from the top of the state stack.
	inline const Matrix4f& CollisionState::GetAccumulatedForward() const 
	{
		return m_stateStack.Top().m_frw;
	}

	//! Get the accumulated inverse matrix from the top of the state stack.
	inline const Matrix4f& CollisionState::GetAccumulatedInverse() const 
	{
		return m_stateStack.Top().m_inv;
	}

	//! Get the accumulated forward matrix from the previous collision traversal.
	inline const Matrix4f& CollisionState::GetLastAccumulatedForward() const 
	{
		return m_stateStack.Top().m_lastFrw;
	}
	
	//! Get the accumulated inverse matrix from the previous collision traversal.
	inline const Matrix4f& CollisionState::GetLastAccumulatedInverse() const 
	{
		return m_stateStack.Top().m_lastInv;
	}

	//! Get the state values
	inline const Vec3f& CollisionState::GetLastProxy() const 
	{
		return m_stateStack.Top().m_lastProxy;
	}

	inline const Vec3f& CollisionState::GetProxy() const 
	{
		return m_stateStack.Top().m_proxy;
	}

	inline Vec3f CollisionState::GetDrawnProxy() const 
	{
		return m_stateStack.Top().m_inv * m_drawnProxy[GetLayer()];
	}

	inline const Vec3f& CollisionState::GetFinger() const 
	{
		return m_stateStack.Top().m_finger;
	}

	inline mgFloat CollisionState::GetRadius() const 
	{
		return m_radius[GetLayer()] * m_stateStack.Top().m_scale;
	}

	inline const mgFloat& CollisionState::GetScale() const 
	{
		return m_stateStack.Top().m_scale;
	}

	inline mgFloat CollisionState::GetUniformity() const 
	{
		return m_stateStack.Top().m_uniformity;
	}

	inline ForceModelSum& CollisionState::GetForceModels() 
	{
		return m_forceModels;
	}

	//! Get the elapsed time since last traversal 
	inline const Time& CollisionState::GetDeltaT() const 
	{
		return m_deltaT;
	}  

	//! Get the global values
	inline const Vec3f& CollisionState::GetGlobalFinger() const 
	{
		return m_finger;
	}

	inline const Vec3f& CollisionState::GetGlobalProxy() const 
	{
		return m_proxy[GetLayer()];
	}

	inline const Vec3f& CollisionState::GetGlobalDrawnProxy() const 
	{
		return m_drawnProxy[GetLayer()];
	}

	inline const mgFloat& CollisionState::GetGlobalRadius() const 
	{
		return m_radius[GetLayer()];
	}

	//! Get a pointer to the current surface instance. Note: the surface pointer may be 0.
	inline Surface* CollisionState::GetSurface() 
	{
		return m_surface;
	}

	//! Set the current surface instance. Note: the surface pointer may be 0.
	inline void CollisionState::SetSurface(Surface* a) 
	{
		m_surface = a;
	}

	//! Get the current texture transform. Note: the pointer may be 0.
	//! TODO (ida): 
	//! inline TextureTransformBase *CollisionState::getTextureTransform() 
	//! {
	//! 	return texture_transform;
	//! } 

	//! Set the current texture transform. Note: the pointer may be zero.
	//! TODO (ida): 
	//! inline void CollisionState::setTextureTransform( TextureTransformBase *a ) 
	//! {
	//! 	texture_transform = a;
	//! }

	//! Fet the nomber of dimensions of the current texture 
	inline int CollisionState::GetTextureDimension() 
	{
		return m_textureDimension;
	}

	//! Set the number of dimensions of the current texture 
	inline void CollisionState::SetTextureDimension(int d)
	{
		m_textureDimension = d;
	}

	//! Get the current ImpulseBase object.
	inline ImpulseBase* CollisionState::GetImpulseBase() 
	{
		return m_impulseBase;
	}

	//! Set the current ImpulseBase object.
	inline void CollisionState::SetImpulseBase(ImpulseBase* ib) 
	{
		m_impulseBase = ib;
	}
}

#endif
