
//! File: <RealtimeGeometry.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_REALTIMEGEOMETRY_H_
#define _HAPTX_REALTIMEGEOMETRY_H_

#include <list>

#include "HaptXDefines.h"
#include "ImpulseBase.h"
#include "Motion.h"
#include "Surface.h"

namespace HaptX
{
    //! forward declarations.
    class CollisionState; 
	class HapticsManager;
    class RealtimeGeometry;
    
	// <NO_DOC>
    // ExclusionPlanes are produced by the virtual Exclude function in order to keep 
	// the device outside of geometries. Since the geometry may be non-convex, there 
	// may be more than one ExclusionPlane produced by a single call to Exclude.
    class HAPTX_SOLVER_SPEC ExclusionPlane
    {
	public:
	    inline ExclusionPlane(const Vec3f& _p, const Vec3f& _n, RealtimeGeometry* _rg, const int _id = 0);	// Constructor.

		inline void					Evaluate(const Vec3f& finger, Vec3f& force, Vec3f& proxy_movement, const int& index, Surface::Contact* contact, const mgFloat& delta_t); // Shortcut to rg->Evaluate.
        inline void					AddImpulse(const Vec3f& force, const Time& time, const Vec3f& position);// Shortcut to rg->AddImpulse.
        inline bool					IsSticky();							// Shortcut to rg->IsSticky.
        inline Surface::Contact*	GetContact(int index);				// Shortcut to rg->surface->MatchContact with the EP normal and point given as arguments.
        
        inline bool					IsAbove(const Vec3f& p) const;		// Compare the given point in global coordinates to the exclusion plane.
        inline bool					IsBelow(const Vec3f& p) const;		// Compare the given point in global coordinates to the exclusion plane.
        inline bool					IsOn(const Vec3f& p) const;			// Compare the given point in global coordinates to the exclusion plane.

	public:
        
        Vec3f p;	// p is a point on the geometry in local coordinates at a local minimum distance from the proxy.
        Vec3f n;	// n is the normal of the surface at p (in local coordinates).
        Vec3f pg;	// p in global coordinates. 
		Vec3f ng;	// n in global coordinates. 

        // Used by HapticsManager for calls to Evaluate() if contact occurs.
        RealtimeGeometry* rg;

        // An id which is used by RealtimeGeometries to identify ExclusionPlanes produced in Exclude. 
        int id;
    };
	// </NO_DOC>

    // RealtimeGeometries are used to store HapticShape geometry data. The RealtimeGeometry 
	// has its own position and orientation with respect to the global coordinate space. 
	// This coordinate space is referred to as RG space. Both RG space and the global space 
	// are defined in metric units. The frw data member is a quaternion matrix describing 
	// the transformation from RG space to global space. The RealtimeGeometry can be moved 
	// within the global space by adjusting the frw matrix.
	//
	// RealtimeGeometry is and abstract class. Sub-classes must define the virtual Exclude() 
	// function. Exclude() produces the position and the surface normal of local distance 
	// minima on the surface with respect to a given point. These are used to exclude the 
	// Touch Controller from the interior of the geometry.
	//
	// We also define the mapping from RG space to texture coordinates.
    class HAPTX_SOLVER_SPEC RealtimeGeometry : public RefCounter
    {
    public:
        //! enumeration for available tex coord modes
        enum texCoordMode_e
		{
			NO_TEX_COORDS,
			TEX_COORDS_1D,
			TEX_COORDS_2D,
			TEX_COORDS_3D 
		};
	
        inline					RealtimeGeometry(texCoordMode_e tcm = TEX_COORDS_2D);	// Constructor.
        inline virtual			~RealtimeGeometry();									// Destructor.

        inline void				SetSurface(Surface* _surface);					// Set the surface instance.
        inline Surface*			GetSurface() const;								// Get the surface instance.
        inline void				SetImpulseBase(ImpulseBase* _impulse_base);		// Set the ImpulseBase instance, if any.

        inline void				SetTexCoordMode(const texCoordMode_e& m);		// Set the tex coord mode.
        inline texCoordMode_e	GetTexCoordMode();								// Get the tex coord mode.

		inline bool				IsSticky();										// IsSticky indicates if the surface of the RealtimeGeometry is sticky or not.

		inline void				GetTriangleID(int& id0, int& id1, int& id2);	// Get the triangle id's of the triangles that the Touch Controller currently is in contact with.
		inline void				SetTriangleID(int id0, int id1, int id2);		// Set the triangle id's of the triangles that the Touch Controller currently is in contact with.

        // Determines local distance minima to the given point. These minima 
		// must be registered with a call to AddExclusionPlane.
        inline virtual void Exclude(const Vec3f& p) = 0;			
	
        // Transform is used to move the geometry from a local coordinate system 
		// to a global one. The given transform (unlike the frw member) is generally 
		// affine and may include scaling and skewing. The inverse is also provided.
        inline virtual void Transform(const Matrix4f& frw, const Matrix4f& inv) = 0;

        // TransformTextures used to move the texture coordinates from a local coordinate system to a global one. 
        inline virtual void TransformTextures(const Matrix4f& xf) = 0;

        // Returns the texture coordinates for the given point p in RG space. It also defines 
		// the derivative with respect to the u and v axes, which are orthogonal unit vectors 
		// in RG space. The result must be stored in the tex_coord, ds, dt and dr data members.
        inline virtual void GetTextureCoords(const ExclusionPlane& ep, const Vec3f& u, const Vec3f& v) = 0;

        // AddImpulse notifies the m_impulseBase (if any) of an impulse (change in momentum) to be applied 
		// at the given position. The impulse is the force f applied for the duration t. Both vectors are 
		// defined in global coordinates.
        inline void AddImpulse(const Vec3f& force, const Time& time, const Vec3f& position);
	
	protected:
        // Evaluate is called by the HapticsManager object to extract a force, and proxy movement from 
		// the Surface at the given ExclusionPlane. The unconstrained position is provided in global 
		// coordinates and the desired force and proxy movement are returned in the same coordinate space.
        void Evaluate(const ExclusionPlane& ep, const Vec3f& finger, Vec3f& force,
				      Vec3f& proxy_movement, const int& index, Surface::Contact* contact,
				      const mgFloat& delta_t);
	
        // ExcludeGlobal is called by the HapticsManager in order to generate a number (possibly zero) 
		// of exclusion planes to exclude the given point. excludeGlobal will invoke the virtual
        // Exclude function which may call AddExclusionPlane several times.
        void ExcludeGlobal(const Vec3f& p, std::list<ExclusionPlane>* _exclusion_planes);

        // AddExclusionPlane is called by Exclude in order to register an ExclusionPlane instance with 
		// the HapticsManager during a call to ExcludeGlobal. The given point and normal must be in RG 
		// coordinates and the normal must be of unit length. The given id (if any) is associated with 
		// the ExclusionPlane. If the plane ends up needing evaluation, the texture coordinate information 
		// must be calculated. This id may be used to identify the exclusion plane at that point.
        inline void AddExclusionPlane(const Vec3f& p, const Vec3f& n, const int id = 0);

        inline Vec3f	ToRG(const Vec3f& p) const;				// Converts a point from global space to RG space.
        inline Vec3f	ToGlobal(const Vec3f& p) const;			// Converts a point from RG space to global space.
        inline Vec3f	ToRGVec(const Vec3f& v) const;			// Converts a Vec3f from global space to RG space (without translation).
        inline Vec3f	ToGlobalVec(const Vec3f& v) const;		// Converts a Vec3f from RG space to global space (without translation).
  
        void			Move(const mgFloat& delta_t);			// Move causes the frw matrix to be updated according to the motion member and the given change in time.

	protected:
        // Defines the forward transformation from RG space to global space. This transformation is
        // purely a rotation and translation from RG metres to global metres. The geometry is moved 
		// in global space by modifying this matrix.
        Matrix4f		frw;

        Vec2f			ds;					// Valid after calling GetTextureCoordinates.
        Vec2f			dt;					// Valid after calling GetTextureCoordinates.
        Vec2f			dr;					// Valid after calling GetTextureCoordinates.
        Vec3f			tex_coord;			// Valid after calling GetTextureCoordinates.

        Motion			m_motion;			// Describes the motion of the RG space within the global space. In this sense it is the time-derivative of the frw matrix.
        Surface*		m_surface;			// The Surface instance responsible for generating forces.
        ImpulseBase*	m_impulseBase;		// The ImpulseBase object (if any) associated with this RealtimeGeometry.
        texCoordMode_e	m_texCoordMode;		// The texture coordinate mode used.
        
		iHapticDeviceID	m_deviceID;			// The device id of the device touching the geometry.
        mgFloat			m_radius;			// The radius of the proxy in metres. ExcludeGlobal uses this to move the exclusion point forward by the radius distance.

		int				m_triangleID0;		// The ID of the first triangle that the Touch Controller is in contact with. If there is no contact, m_triangleID0 is -1. 
		int				m_triangleID1;		// The ID of the second triangle that the Touch Controller is in contact with. If there is no contact or only contact with one triangle, m_triangleID1 is -1. 
		int				m_triangleID2;		// The ID of the third triangle that the Touch Controller is in contact with. If there is only contact with two or less triangles, m_triangleID2 is -1. 

        friend class CollisionState;
        friend class HapticsManager;
        friend class ExclusionPlane;

    private:
        std::list<ExclusionPlane>*	m_exclusionPlanes;			// A list of planes that constraines the proxy to stay outside the geometry. This member is valid only during a call to ExcludeGlobal.
	};

    
	typedef std::vector<RealtimeGeometry*> GeometryVec;		// A vector of pointers to RealtimeGeometry instances.


    //! Constructor.
    inline RealtimeGeometry::RealtimeGeometry(texCoordMode_e tcm) :
        m_impulseBase(0),
        m_surface(0),
        m_exclusionPlanes(0),
        m_deviceID(-1),
        m_texCoordMode(tcm),
		m_triangleID0(-1),
		m_triangleID1(-1),
		m_triangleID2(-1)
    {
        frw.SetToIdentity();
    }

    //! Destructor
	inline RealtimeGeometry::~RealtimeGeometry()
    {
    }

    //! AddImpulse
    inline void RealtimeGeometry::AddImpulse(const Vec3f& force, const Time& time, const Vec3f& position)
    {
	    if (m_impulseBase) m_impulseBase->AddImpulse(force, time, position);
    }

    //! IsSticky
    inline bool RealtimeGeometry::IsSticky()
    {
	    return m_surface->m_isSticky;
    }

	inline void RealtimeGeometry::GetTriangleID(int& id0, int& id1, int& id2)
	{ 
		id0 = m_triangleID0; 
		id1 = m_triangleID1; 
		id2 = m_triangleID2; 
	}

	inline void RealtimeGeometry::SetTriangleID(int id0, int id1, int id2 )
	{
		m_triangleID0 = id0; 
		m_triangleID1 = id1; 
		m_triangleID2 = id2; 
	}

    inline void RealtimeGeometry::ExcludeGlobal(const Vec3f& p, std::list<ExclusionPlane>* _exclusion_planes)
    {
	    m_exclusionPlanes = _exclusion_planes;
	    Exclude(ToRG(p));
    }
  
    inline void RealtimeGeometry::AddExclusionPlane(const Vec3f& p, const Vec3f& n, const int id)
    {
	    // we advance p by radius, if necessary.
	    if (m_radius > Util::epsilon)
	        m_exclusionPlanes->push_back(ExclusionPlane(p + m_radius * n,n,this,id));
	    else
	        m_exclusionPlanes->push_back(ExclusionPlane(p,n,this,id));
    }

    //! Set the surface instance.
    inline void RealtimeGeometry::SetSurface(Surface* _surface)
    {
	    m_surface = _surface;
    }

    //! Get the surface instance.
    inline Surface* RealtimeGeometry::GetSurface() const
    {
	    return m_surface;
    }

    //! Set the ImpulseBase instance, if any.
    inline void RealtimeGeometry::SetImpulseBase(ImpulseBase* _impulse_base)
    {
	    m_impulseBase = _impulse_base;
    }

    //! Set the m_texCoordMode
    inline void RealtimeGeometry::SetTexCoordMode(const texCoordMode_e& m)
    {
	    m_texCoordMode = m;
    }

    inline RealtimeGeometry::texCoordMode_e RealtimeGeometry::GetTexCoordMode()
    {
	    return m_texCoordMode;
    }

    //! ToRG converts a point from global space to RG space.
    inline Vec3f RealtimeGeometry::ToRG(const Vec3f& p) const
    {
        mgFloat x = p.x - frw[0][3];
        mgFloat y = p.y - frw[1][3];
        mgFloat z = p.z - frw[2][3];

	    return Vec3f(frw[0][0]*x + frw[1][0]*y + frw[2][0]*z,
		    		 frw[0][1]*x + frw[1][1]*y + frw[2][1]*z,
			    	 frw[0][2]*x + frw[1][2]*y + frw[2][2]*z);
    }

    //! ToGlobal converts a point from RG space to global space.
    inline Vec3f RealtimeGeometry::ToGlobal(const Vec3f& p) const
    {
	    return Vec3f(frw[0][0]*p.x + frw[0][1]*p.y + frw[0][2]*p.z + frw[0][3],
                     frw[1][0]*p.x + frw[1][1]*p.y + frw[1][2]*p.z + frw[1][3],
                     frw[2][0]*p.x + frw[2][1]*p.y + frw[2][2]*p.z + frw[2][3]);
    }

    //! Transform a Vec3f from global space to RG space (without translation).
    inline Vec3f RealtimeGeometry::ToRGVec(const Vec3f& v) const
    {
	    return Vec3f(frw[0][0]*v.x + frw[1][0]*v.y + frw[2][0]*v.z,
	    			 frw[0][1]*v.x + frw[1][1]*v.y + frw[2][1]*v.z,
		    		 frw[0][2]*v.x + frw[1][2]*v.y + frw[2][2]*v.z);
    }

	//! Transform a Vec3f from RG space to global space (without translation). 
    inline Vec3f RealtimeGeometry::ToGlobalVec(const Vec3f& v) const
    {
	    return Vec3f(frw[0][0]*v.x + frw[0][1]*v.y + frw[0][2]*v.z,
		    		 frw[1][0]*v.x + frw[1][1]*v.y + frw[1][2]*v.z,
			    	 frw[2][0]*v.x + frw[2][1]*v.y + frw[2][2]*v.z);
    }

    //! ExclusionPlane constructor.
    inline ExclusionPlane::ExclusionPlane(const Vec3f& _p, const Vec3f& _n, RealtimeGeometry* _rg, const int _id) :
	    p(_p), n(_n), rg(_rg), pg(_rg->frw * _p), ng(_rg->frw.ScaleRotate(_n)), id(_id)
    {
    }

    //! Shortcut to rg->Evaluate().
    inline void ExclusionPlane::Evaluate(const Vec3f& finger, Vec3f& force, Vec3f& proxy_movement, const int& index, Surface::Contact* contact, const mgFloat& delta_t)
    {
	    rg->Evaluate(*this, finger, force, proxy_movement, index, contact, delta_t);
    }
  
    //! Shortcut to rg->AddImpulse().
    inline void ExclusionPlane::AddImpulse(const Vec3f& force, const Time& time, const Vec3f& position)
    {
	    rg->AddImpulse(force, time, position);
    }

    //! Shortcut to rg->IsSticky().
    inline bool ExclusionPlane::IsSticky()
    {
	    return rg->IsSticky();
    }

    //! Shortcut to rg->surface->MatchContact with the EP normal and point as arguments
    inline Surface::Contact* ExclusionPlane::GetContact(int index)
    {
	    return rg->m_surface->MatchContact(pg, ng, index);
    } 

    inline bool ExclusionPlane::IsAbove(const Vec3f& p) const
    {
	    Vec3f d = p - pg;
	    return d * ng > Util::epsilon;
    }

    inline bool ExclusionPlane::IsBelow(const Vec3f& p) const
    {
	    Vec3f d = p - pg;
	    return d * ng < -Util::epsilon;
    }

    inline bool ExclusionPlane::IsOn(const Vec3f& p) const
    {
	    Vec3f d = p - pg;
	    return mgAbs( d * ng ) < Util::epsilon;
    }
}

#endif
