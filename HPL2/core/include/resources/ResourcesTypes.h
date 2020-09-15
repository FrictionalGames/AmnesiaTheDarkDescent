/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HPL_RESOURCES_TYPES_H
#define HPL_RESOURCES_TYPES_H

#include <stdio.h>

#include "system/SystemTypes.h"

namespace hpl {
	
	//change to own file type?
	typedef FILE tFile;
	
	//-------------------------------------------------------

	typedef tFlag tWorldLoadFlag;

	#define eWorldLoadFlag_NoLights					(0x00000001)
	#define eWorldLoadFlag_NoEntities				(0x00000002)
	#define eWorldLoadFlag_NoGameEntities			(0x00000004)
	#define eWorldLoadFlag_NoDynamicGameEntities	(0x00000008)
	#define eWorldLoadFlag_FastPhysicsLoad			(0x00000010)
	#define eWorldLoadFlag_FastStaticLoad			(0x00000020)
	#define eWorldLoadFlag_FastEntityLoad			(0x00000040)

	typedef tFlag tMeshLoadFlag;

	#define eMeshLoadFlag_NoGeometry		(0x00000001)
	#define eMeshLoadFlag_NoMaterial		(0x00000002)

	//-------------------------------------------------------

	typedef tFlag tBitmapLoadFlag;

	#define eBitmapLoadFlag_ForceNoCompression		(0x00000001)

	typedef tFlag tBitmapSaveFlag;

	//-------------------------------------------------------

	class cEFL_LightBillboardConnection
	{
	public:
		tString msLightName;
		int msBillboardID;
	};

	typedef std::list<cEFL_LightBillboardConnection> tEFL_LightBillboardConnectionList;
	typedef tEFL_LightBillboardConnectionList::iterator tEFL_LightBillboardConnectionListIt;

	//-------------------------------------------------------
	class iXmlDocument;
	class cBinaryBuffer;

	typedef std::list<iXmlDocument*> tXmlDocumentList;
	typedef tXmlDocumentList::iterator tXmlDocumentListIt;

	typedef std::list<cBinaryBuffer*> tBinaryBufferList;
	typedef tBinaryBufferList::iterator tBinaryBufferListIt;
	
	//-------------------------------------------------------

};
#endif // HPL_RESOURCES_TYPES_H
