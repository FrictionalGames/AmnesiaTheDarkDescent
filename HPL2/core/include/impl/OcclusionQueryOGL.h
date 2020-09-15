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

#ifndef HPL_OCCLUSION_QUERY_OGL_H
#define HPL_OCCLUSION_QUERY_OGL_H

#include "graphics/OcclusionQuery.h"

namespace hpl {

	class cOcclusionQueryOGL : public iOcclusionQuery
	{
	public:
		cOcclusionQueryOGL();
		~cOcclusionQueryOGL();

		void Begin();
		void End();
		bool FetchResults();
		unsigned int GetSampleCount();

	public:
		bool mbResultsAreFetched;
		int mlLastSampleCount;
		unsigned int mlQueryId;
	};

};
#endif // HPL_OCCLUSION_QUERY_H
