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

#include "impl/OcclusionQueryOGL.h"
#include "impl/LowLevelGraphicsSDL.h"

#include <GL/glew.h>


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cOcclusionQueryOGL::cOcclusionQueryOGL()
	{
		;

		glGenQueriesARB(1, (GLuint *)&mlQueryId);
		mlLastSampleCount =0;
		mbResultsAreFetched = true;
	}

	//-----------------------------------------------------------------------

	cOcclusionQueryOGL::~cOcclusionQueryOGL()
	{
		;

		glDeleteQueriesARB(1, (GLuint *)&mlQueryId);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cOcclusionQueryOGL::Begin()
	{
		;

		glBeginQueryARB(GL_SAMPLES_PASSED_ARB,mlQueryId);
		mbResultsAreFetched = false;
	}

	void cOcclusionQueryOGL::End()
	{
		;

		glEndQueryARB(GL_SAMPLES_PASSED_ARB);
	}

	bool cOcclusionQueryOGL::FetchResults()
	{
		if(mbResultsAreFetched) return true;

		;

		//if it is not a query, there are no samples to get!
		if(glIsQuery(mlQueryId) == GL_FALSE)
		{
			mbResultsAreFetched = true;
			mlLastSampleCount =0;
			return true;
		}

		GLint lAvailable=0;
		glGetQueryObjectivARB(mlQueryId,GL_QUERY_RESULT_AVAILABLE_ARB,&lAvailable);
		if(lAvailable==0) return false;

		glGetQueryObjectivARB(mlQueryId,GL_QUERY_RESULT_ARB,(GLint *)&mlLastSampleCount);

		mbResultsAreFetched = true;

		return true;
	}

	unsigned int cOcclusionQueryOGL::GetSampleCount()
	{
		return mlLastSampleCount;
	}

	//-----------------------------------------------------------------------
}
