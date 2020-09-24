
//! File: <Surface.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_SURFACE_H_
#define _HAPTX_SURFACE_H_

#include <algorithm>
#include <vector>
#include "SimpleVector.h"
#include "PersistentAllocator.h"
#include "RefCounter.h"
#include "ILog.h"

#pragma warning(disable: 4251)	//! Disable warning bescause STL templates are exported implicitly.

namespace HaptX
{
	// <NO_DOC>
	namespace SurfaceInternals
	{
		// AutoVector is analagous to the auto_ptr template class.
		// Instances are sub-classes of a vector of pointers and the
		// following behaviour is provided:
		//   On destruction, the pointers in the vector are deleted.
		//   On assignment the pointers are moved (not copied) to the
		//   left hand side.
		// Note that responsibility for destruction of the pointers
		// resides with the AutoVector.

		template<class T>
		struct AutoVector : public std::vector<T*> 
		{
			inline AutoVector()						{}
			inline AutoVector(AutoVector<T>& rhs)	{ swap(rhs); }
			
			inline AutoVector<T>& operator = (AutoVector<T>& rhs) 
			{
				clear();
				swap(rhs);
				return *this;
			}

			inline void erase(typename std::vector<T*>::iterator position) 
			{
				delete (*position);
				std::vector<T*>::erase(position);
			}
			
			inline void erase(typename std::vector<T*>::iterator first, typename std::vector<T*>::iterator second)
			{
				for (iterator i = first; i != second; ++i)
				{
					delete (*i);
				}

				std::vector<T*>::erase(first,second);		
			}

			inline void append(AutoVector<T>& av) 
			{
				insert(end(),av.begin(),av.end());
				av.vector<T*>::clear();
			}

			inline void clear() 
			{
				for (iterator i = begin(); i != end(); ++i)
				{
					delete (*i);
				}

				std::vector<T*>::clear();
			}

			inline ~AutoVector()
			{
				clear();
			}
		};
	}
	// </NO_DOC>

	// forward declarations
	class CollisionState; class RealtimeGeometry; 
	class ExclusionPlane; class HapticsManager;
	class Lock;

	// A Surface defines the haptic material properties of a geometry
	// e.g. how the surface of the geometry should feel. Surfaces are referenced by HapticShapes. 
	// A single Surface instance may be referenced by several HapticShapes.

	// SimpleSurface is the most basic surface type. 
	// A SurfaceInfo object is associated with each Surface that is a sub-class of SimpleSurface. 
	// The SurfaceInfo object holds the parameters of the surface such as e.g. stiffness of the surface or 
	// friction values of the surface.

	// Each surface has an internal Contact object. Every time a new collision between the proxy and a geometry 
	// occurs a new Contact instance is created. That Contact instance is then responsible for calculating the forces
	// that is generated in that collision, this is done in the Evaluate functions of the Contact.
	class HAPTX_SOLVER_SPEC Surface : public RefCounter
	{
	public:
        // A Contact object is associated with every point of contact
        // between the proxy and geometries associated with this surface.
        // Sub-classes of Surface may sub-class Contact in order to store
        // state information associated with the contact.
        class HAPTX_SOLVER_SPEC Contact : public PersistentAllocator
        {
		public:
	        inline Contact(Surface* s);			// Constructor.
	        inline Contact();					// The void constructor is required in all sub-classes.
	        inline virtual ~Contact();			// Destructor.
        	
			// Evaluate is a virtual function defined by sub-classes to produce the force rendering of the surface. 
			// The device unconstrained position (finger position) is given in UVN space (surface coordinate system, U and V along the surface and N normal to  
			// the surface) as well as the texture coordinate transform in the plane. 
			// The Evaluate function sets the value of the resulting force vector and the desired movement of the proxy. Note that because of
	        // constraints with other surfaces, the proxy may not actually move according to proxy_movement.
	        // 
	        // The ds and dt arguments are the derivatives of the texture coordinates with respect to the u and v axes of
	        // the surface. Hence the s texture coordinate of the finger is given by
	        // st_origin.x + ds.x * finger.x + ds.y * finger.y
	        // and the t coordinate by
	        // st_origin.y + dt.x * finger.x + dt.y * finger.y
	        // Similarly the change in texture coordinate suggested by the proxy_movement will be
	        // Vec2f(proxy_movement * ds, proxy_movement * dt)
	        inline virtual void Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement);

	        // Evaluate3D does the same as Evaluate except that the texture has three dimensions. See also Evaluate
	        inline virtual void Evaluate3D(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& dr, const Vec3f& str_origin, Vec3f& force, Vec2f& proxy_movement);

	        // see Evaluate
	        inline virtual void Evaluate2D(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement);

			// Evaluate1D does the same as Evaluate except that the texture only has one dimension. See also Evaluate
	        inline virtual void Evaluate1D(const Vec3f& finger, const Vec2f& ds, const mgFloat s_origin, Vec3f& force, Vec2f& proxy_movement);

			// Evaluate3D does the same as Evaluate except that no texture information is supplied. See also Evaluate
	        inline virtual void Evaluate0D(const Vec3f& finger, Vec3f& force, Vec2f& proxy_movement);

	        // PostEvaluate is called by the RealtimeGeometry immediately after the Evaluate function has been called. 
			// In this call, force and proxy_movement have been converted to global coordinates (instead of UVN coordinates,
			// see Evaluate). PostEvaluate can therefore be used to perform actions based on the global force, rather 
			// than a force relative to the realtime surface plane.
	        inline virtual void PostEvaluate(const Vec3f& finger, Vec3f& force, Vec3f& proxy_movement);

	        // PreEvaluate is called by the RealtimeGeometry immediately before the evaluate function is to be called. In this call,
	        // The axes of the UVN-space (see Evaluate) are given in global coordinates and PreEvaluate can therefore be used to project global vectors into UVN-space.
	        inline virtual void PreEvaluate(const Vec3f& ug, const Vec3f& vg, const Vec3f& ng);

		public:
	        Vec3f		pg;										// pg is the point of contact in global coordinates.
	        Vec3f		ng;										// ng is the normal in global coordinates.

			int			m_deviceID;								// The id of the device that is in contact with the Surface.
			mgFloat		m_deviceMaxStiffness;					// The maximum stiffness that the device can handle.
			mgFloat		m_deviceMaxForce;						// The maximum force that the device can handle.

			bool		m_touched;								// m_touched is set by contacts that get used and cleared every realtime-loop. Any left untouched are detroyed.
	        Surface*	m_surface;								// The associated surface instance.
	        mgFloat		m_deltaT;								// The time-step associated with a call to Evaluate. Only valid during such a call.


			// A boolean flag that specifies if a PostEvaluate call is needed for this Surface. 
			// False by default. If you want PostEvaluate to be called, this must be set to true in your Surface's constructor.
	        bool m_needsPostEvaluate;
        	        	
			// A boolean flag that specifies if a PreEvaluate call is needed for this Surface. 
			// False by default. If you want PreEvaluate to be called, you must set this to true in your Surface's constructor.
	        bool m_needsPreEvaluate;
	      
	        // Counter for releasing the Surface when in sticky mode. As long as the m_releaseCounter is greater than 0 no force is generated
	        // for this contact and the proxy movement is set to the finger position (unconstrained position). The m_releaseCounter is decremented each time the 
	        // contact is used and it is > 0.
	        int m_releaseCounter;
        };

		// A ContactInfo object is associated a with each HapticsDevice that are in contact with the Surface.
        struct HAPTX_SOLVER_SPEC ContactInfo
        {
		public:
	        inline ContactInfo();												// Constructor
        
		public:
			SurfaceInternals::AutoVector<Contact>	m_contacts;        			// The contact instances currently being touched by the proxy
        	int										m_numContacts;				// The current number of contacts for this surface.
        };	
	
	public:
		inline								Surface();									// Constructor
        inline virtual						~Surface();									// Destructor
        
        inline virtual void					UpdateFields();								// UpdateFields is called once per render loop. Sub-classes may update changes from the realtime state of the Surface into the variables of the Surface instance.

        inline int							GetNumContacts(const int& deviceID);		// Get the current number of contact points between a HapticsDevice and this Surface. See also GetNumContactsRealtime
        inline int							GetNumContactsRealtime(const int& deviceID);// Get the current number of contact points from the HaptX loop between a HapticsDevice and this Surface. See also GetNumContacts
        
        inline void							ReleaseAllDevices();						// Used for sticky surfaces when the proxies of all devices should be released from the Surface. Must be called by the render loop (typically from UpdateFields).
        inline void							ReleaseDevice(int deviceID);				// Used for a sticky surface when the proxies of a specific device should be released from the Surface. Must be called by the render loop (typically from UpdateFields).
		
        inline ContactInfo&					GetContactInfo(const int& deviceID);		// Get the ContactInfo object associated with the given device id.

		inline virtual Surface::Contact*	NewContact();								// Sub-classes should contain exactly the following in order to register their new Contact sub-classes. See also CopyContact()
        inline virtual Surface::Contact*	CopyContact();								// Sub-classes should contain exactly the following in order to register their new Contact sub-classes. See also NewContact()

        
        template<class C>
        Surface::Contact* CopyContactT(C* c)											// CopyContactT is used by the virtual CopyContact function and its overriding definitions.
        {
	        (*c) = *static_cast<C*>(m_contactTemplate);
	        return c;
        }
	
	public:
        Contact*							m_contactTemplate;							// Created during the render loop and copied off during the realtime loop. 
		SimpleVector<ContactInfo>			m_contactInfos;								// The contact information for each HapticsDevice.
		bool								m_isSticky;									// Indicates if the Surface is sticky or not. In a sticky Surface the proxy will stay in contact with the surface when it has been touched until ReleaseDevice is called.
        
	protected:
        inline void							UpdateContactTemplate();					// Called from the render loop to clear the m_contactTemplate.
        inline void							ClearTouchedFlags(const int& index);		// Clears the m_touched members of all our Contact instances.
		inline void							RemoveUntouchedContacts(const int& index);	// Destroys any Contact instances which have not been touched since the last call to ClearTouchedFlags().

        Contact*							MatchContact(const Vec3f& pg, const Vec3f& ng, const int& index);// Returns the Contact instance matching the given contact point and normal in global coordinates. If no matching contact exists, a new contact is returned.
        
		friend class RealtimeGeometry;
        friend class ExclusionPlane;
        friend class CollisionState;
        friend class HapticsManager;
		friend class HapticsDevice;

        Lock								m_contactLock;								// lock
        
        bool								m_supportsTex0D : 1;						// flags that indicates what different texture coordinate modes this surface supports, see also m_supportsTex1D, m_supportsTex2D, m_supportsTex3D.
        bool								m_supportsTex1D : 1;						// flags that indicates what different texture coordinate modes this surface supports, see also m_supportsTex0D, m_supportsTex2D, m_supportsTex3D.
        bool								m_supportsTex2D : 1;						// flags that indicates what different texture coordinate modes this surface supports, see also m_supportsTex0D, m_supportsTex1D, m_supportsTex3D.
		bool								m_supportsTex3D : 1;						// flags that indicates what different texture coordinate modes this surface supports, see also m_supportsTex0D, m_supportsTex1D, m_supportsTex2D.
    };


    //! Inline member methods

    inline Surface::Surface() : m_contactInfos(4), m_contactTemplate(0), m_isSticky(false), m_supportsTex0D(false), m_supportsTex1D(false), m_supportsTex2D(true), m_supportsTex3D(false)
    {
    }

    inline Surface::~Surface()
    {
	    if (m_contactTemplate) delete m_contactTemplate;
    }

    inline void Surface::UpdateFields()
    {
    }

    inline Surface::Contact::Contact(Surface* s) : m_surface(s), m_needsPostEvaluate(false), m_needsPreEvaluate(false), m_releaseCounter(0), m_deviceID(-1), m_deviceMaxStiffness(0.0), m_deviceMaxForce(0.0)
    {
    }

    inline Surface::Contact::Contact() : m_needsPostEvaluate(false), m_needsPreEvaluate(false), m_releaseCounter(0), m_deviceID(-1), m_deviceMaxStiffness(0.0), m_deviceMaxForce(0.0)
    {
    }

    inline Surface::Contact::~Contact()
    {
    }

    inline Surface::ContactInfo::ContactInfo() : m_numContacts(0)
    {
    }

    //! sub-classes should contain exactly the following in order to 
    //! register their new Contact sub-classes.
    inline Surface::Contact* Surface::NewContact()
    {
	    return new Contact(this);
    }

    inline Surface::Contact* Surface::CopyContact()
    {
	    return CopyContactT(new Contact);
    }

    //! ReleaseDevice and ReleaseAllDevices must be called by the
    //! render loop (typically from UpdateFields).
    inline void Surface::ReleaseDevice(int device_index)
    {
	    m_contactLock.Enter();
	    ContactInfo& ci = GetContactInfo(device_index);
	    m_contactLock.Leave();

	    for (std::vector<Contact*>::iterator i = ci.m_contacts.begin(); i != ci.m_contacts.end(); i++)
	    {
		    (*i)->m_releaseCounter = 2;
	    }
    }

    inline void Surface::ReleaseAllDevices()
    {
	    m_contactLock.Enter();

		const int num_devices = (int)m_contactInfos.size();

		for (int i = 0; i < num_devices; ++i)
	    {
			ContactInfo& ci = m_contactInfos[i];

		    for (std::vector<Contact*>::iterator j = ci.m_contacts.begin(); j != ci.m_contacts.end(); j++)
		    {
			    (*j)->m_releaseCounter = 2;
		    }
	    }

	    m_contactLock.Leave();
    }

    inline void Surface::UpdateContactTemplate()
    {
	    Contact* save = m_contactTemplate;
	    m_contactTemplate = NewContact();
	    if (save) delete save;
    }

    inline void Surface::ClearTouchedFlags(const int& index)
    {
	    m_contactLock.Enter();  // called from realtime loop
	    ContactInfo& ci = GetContactInfo(index);
	    m_contactLock.Leave();

	    for (std::vector<Contact*>::iterator i = ci.m_contacts.begin(); i != ci.m_contacts.end(); ++i)
	    {
		    (*i)->m_touched = false;
	    }
    }

    inline void Surface::RemoveUntouchedContacts(const int& index)
    {
	    m_contactLock.Enter();  // called from realtime loop
	    ContactInfo& ci = GetContactInfo(index);
	    m_contactLock.Leave();

	    std::vector<Contact*>::iterator j = ci.m_contacts.end();

	    for (std::vector<Contact*>::iterator i = ci.m_contacts.begin(); i != j;)
	    {
		    if (!(*i)->m_touched)
		    {
				std::iter_swap(i,--j);
		    }
		    else
		    {
			    ++i;
		    }
	    }

	    ci.m_contacts.erase(j,ci.m_contacts.end());
	    ci.m_numContacts = (int)ci.m_contacts.size();
    }

    inline int Surface::GetNumContacts(const int& index)
    {
	    m_contactLock.Enter();  // called from render loop
	    int num_contacts = GetContactInfo(index).m_numContacts;
	    m_contactLock.Leave();
	    return num_contacts;
    }

    inline int Surface::GetNumContactsRealtime(const int& index)
    {
	    m_contactLock.Enter();  // called from realtime loop
	    int num_contacts = GetContactInfo(index).m_numContacts;
	    m_contactLock.Leave();
	    return num_contacts;
    }

    inline Surface::ContactInfo& Surface::GetContactInfo(const int& index)
    {
	    if (index >= (int)m_contactInfos.size())
	    {
		    m_contactInfos.resize(index + 1);
	    }

	    return m_contactInfos[index];
    }

    inline void Surface::Contact::Evaluate3D(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& dr, const Vec3f& str_origin, Vec3f& force, Vec2f& proxy_movement)
    {				
		HAPTX_LOG_ERROR("Surface::Contact::evaluate3D(). UnsupportedTexCoordMode 3d tex coords.");
    }

    inline void Surface::Contact::Evaluate(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement)
    {
	    force = Vec3f(0.0,0.0,0.0);
	    proxy_movement = Vec2f(0.0,0.0);
    }

    inline void Surface::Contact::Evaluate2D(const Vec3f& finger, const Vec2f& ds, const Vec2f& dt, const Vec2f& st_origin, Vec3f& force, Vec2f& proxy_movement)
    {
	    Evaluate(finger,ds,dt,st_origin,force,proxy_movement);
    }

    inline void Surface::Contact::Evaluate1D(const Vec3f& finger, const Vec2f& ds, const mgFloat s_origin, Vec3f& force, Vec2f& proxy_movement)
    {
		HAPTX_LOG_ERROR("Surface::Contact::Evaluate1D(). UnsupportedTexCoordMode 1d tex coords.");
    }

    inline void Surface::Contact::Evaluate0D(const Vec3f& finger, Vec3f& force, Vec2f& proxy_movement)
    {
   		HAPTX_LOG_ERROR("Surface::Contact::Evaluate0D(). UnsupportedTexCoordMode No tex coords.");
    }

    inline void Surface::Contact::PostEvaluate(const Vec3f& finger, Vec3f& force, Vec3f& proxy_movement)
    {
    }

    inline void Surface::Contact::PreEvaluate(const Vec3f& ug, const Vec3f& vg, const Vec3f& ng)
    {
    } 
}

#endif
