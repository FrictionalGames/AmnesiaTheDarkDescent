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

#include "impl/CGProgram.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "impl/SDLTexture.h"
#include "impl/CGShader.h"

#include <GL/GLee.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>


namespace hpl{

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cCGProgram::cCGProgram() : iGpuProgram(eGpuProgramFormat_CG)
	{

	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cCGProgram::Link()
	{
		//Do nothing right?
		return true;
	}

	//-----------------------------------------------------------------------

	void cCGProgram::Bind()
	{
		for(int i=0; i<2; ++i) 
		{
			if(mpShader[i])
			{
				cCGShader *pCgShader = static_cast<cCGShader*>(mpShader[i]);
				pCgShader->Bind();
			}
		}
	}

	//-----------------------------------------------------------------------

	void cCGProgram::UnBind()
	{
		for(int i=0; i<2; ++i) 
		{
			if(mpShader[i])
			{
				cCGShader *pCgShader = static_cast<cCGShader*>(mpShader[i]);
				pCgShader->UnBind();
			}
		}
	}

		
	//-----------------------------------------------------------------------
	
	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	
	//-----------------------------------------------------------------------

}
