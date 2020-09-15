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

#ifndef HPL_BONE_STATE_H
#define HPL_BONE_STATE_H

#include "scene/Node3D.h"

namespace hpl {

	class iPhysicsBody;

	class cBoneState : public cNode3D
	{
	public:
		cBoneState(const tString &asName, bool abAutoDeleteChildren);
		
		void SetBody(iPhysicsBody *apBody);
		iPhysicsBody* GetBody();

		void SetColliderBody(iPhysicsBody *apBody);
		iPhysicsBody* GetColliderBody();

		void SetBodyMatrix(const cMatrixf &a_mtxBody);
		const cMatrixf& GetBodyMatrix();
		const cMatrixf& GetInvBodyMatrix();

	private:
		iPhysicsBody *mpBody;
		cMatrixf m_mtxBody;
		cMatrixf m_mtxInvBody;

		iPhysicsBody *mpColliderBody;
	};

};
#endif // HPL_BONE_STATE_H
