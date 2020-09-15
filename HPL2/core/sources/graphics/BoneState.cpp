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

#include "graphics/BoneState.h"

#include "physics/PhysicsBody.h"

#include "math/Math.h"

#include "system/LowLevelSystem.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cBoneState::cBoneState(const tString &asName,bool abAutoDeleteChildren) : cNode3D(asName,abAutoDeleteChildren)
	{
		mpBody = NULL;
		mpColliderBody = NULL;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cBoneState::SetBody(iPhysicsBody *apBody)
	{
		mpBody = apBody;
	}
	iPhysicsBody* cBoneState::GetBody()
	{
		return mpBody;
	}

	//-----------------------------------------------------------------------

	void cBoneState::SetColliderBody(iPhysicsBody *apBody)
	{
		mpColliderBody = apBody;
	}
	iPhysicsBody* cBoneState::GetColliderBody()
	{
		return mpColliderBody;
	}
	
	//-----------------------------------------------------------------------

	void cBoneState::SetBodyMatrix(const cMatrixf &a_mtxBody)
	{
		m_mtxBody = a_mtxBody;
		m_mtxInvBody = cMath::MatrixInverse(m_mtxBody);
	}

	const cMatrixf& cBoneState::GetBodyMatrix()
	{
		return m_mtxBody;
	}

	const cMatrixf& cBoneState::GetInvBodyMatrix()
	{
		return m_mtxInvBody;
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	
	//-----------------------------------------------------------------------


}
