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

#include "scene/Node3D.h"
#include "scene/Entity3D.h"
#include "system/LowLevelSystem.h"
#include "math/Math.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cNode3D::cNode3D(const tString &asName, bool abAutoDeleteChildren)
	{
		msName = asName;

		mbActive = true;

		mbAutoDeleteChildren = abAutoDeleteChildren;

		m_mtxLocalTransform = cMatrixf::Identity;
		m_mtxWorldTransform = cMatrixf::Identity;

		mvWorldPosition = cVector3f(0,0,0);

		mbTransformUpdated = true;

		mbUsePreTransform = false;
		mbUsePostTransform = false;

		m_mtxPreTransform  = cMatrixf::Identity;
		m_mtxPostTransform = cMatrixf::Identity;

		mpParent = NULL;
		mpEntityParent = NULL;
		
		mqRotation = cQuaternion::Identity;
		mvScale = cVector3f(1,1,1);
		mvTranslation = cVector3f(0,0,0);

		mlCustomFlags =0;
	}

	//-----------------------------------------------------------------------

	cNode3D::~cNode3D()
	{
		if(mpEntityParent)
		{
			mpEntityParent->RemoveNodeChild(this);
		}
		else if(mpParent)
		{
			mpParent->RemoveChild(this);
		}

		
		for(tEntity3DListIt EIt = mlstEntity.begin(); EIt != mlstEntity.end();++EIt)
		{
			iEntity3D* pEntity = *EIt;
			pEntity->SetParent(NULL);
		}

		for(tNode3DListIt NIt = mlstNode.begin(); NIt != mlstNode.end();++NIt)
		{
			cNode3D* pNode = *NIt;
			pNode->mpParent = NULL;
		}
		
		if(mbAutoDeleteChildren)
		{
			STLDeleteAll(mlstNode);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	int cNode3D::SetVisible(bool abX, bool abCascade)
	{
		if(abCascade)
		{
			for(tNode3DListIt NIt = mlstNode.begin(); NIt != mlstNode.end(); NIt++)
			{
				(*NIt)->SetVisible(abX,abCascade);
			}
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	bool cNode3D::AddEntity(iEntity3D* apEntity)
	{
		if(apEntity->HasParent())return false;

		mlstEntity.push_back(apEntity);
		apEntity->SetParent(this);

		return true;
	}

	//-----------------------------------------------------------------------

	bool cNode3D::RemoveEntity(iEntity3D* apEntity)
	{
		for(tEntity3DListIt it = mlstEntity.begin();it != mlstEntity.end();it++)
		{
			if(*it == apEntity){
				apEntity->SetParent(NULL);
				mlstEntity.erase(it);
				return true;
			}
		}

		return false;
	}

	//-----------------------------------------------------------------------

	void cNode3D::ClearEntities()
	{
		mlstEntity.clear();
	}

	//-----------------------------------------------------------------------

	cNode3DIterator cNode3D::GetChildIterator()
	{
		return cNode3DIterator(&mlstNode);
	}

	cEntity3DIterator cNode3D::GetEntityIterator()
	{
		return cEntity3DIterator(&mlstEntity);
	}	
	

	//-----------------------------------------------------------------------

	cNode3D* cNode3D::CreateChild(const tString &asName, bool abAutoDeleteChildren)
	{
		cNode3D *pNode = hplNew( cNode3D, (asName,abAutoDeleteChildren) );

		pNode->mpParent = this;
		mlstNode.push_back(pNode);

		return pNode;
	}

	//-----------------------------------------------------------------------

	void cNode3D::RemoveChild(cNode3D* apNode)
	{
		for(tNode3DListIt NIt = mlstNode.begin(); NIt != mlstNode.end();++NIt)
		{
			if(*NIt == apNode)
			{
				mlstNode.erase(NIt);
				return;
			}
		}
	}

	//-----------------------------------------------------------------------

	cVector3f cNode3D::GetLocalPosition()
	{
		return m_mtxLocalTransform.GetTranslation();
	}
	
	//-----------------------------------------------------------------------

	cMatrixf& cNode3D::GetLocalMatrix()
	{
		return m_mtxLocalTransform;
	}
	
	//-----------------------------------------------------------------------

	cVector3f cNode3D::GetWorldPosition()
	{
		UpdateWorldTransform();

		return m_mtxWorldTransform.GetTranslation();
	}

	//-----------------------------------------------------------------------

	cMatrixf& cNode3D::GetWorldMatrix()
	{
		UpdateWorldTransform();

		return m_mtxWorldTransform;
	}

	//-----------------------------------------------------------------------


	void cNode3D::SetPosition(const cVector3f& avPos)
	{
		m_mtxLocalTransform.m[0][3] = avPos.x;
		m_mtxLocalTransform.m[1][3] = avPos.y;
		m_mtxLocalTransform.m[2][3] = avPos.z;

		SetWorldTransformUpdated();
	}

	void cNode3D::SetMatrix(const cMatrixf& a_mtxTransform, bool abSetChildrenUpdated)
	{
		m_mtxLocalTransform = a_mtxTransform;

		if(abSetChildrenUpdated)
			SetWorldTransformUpdated();
		else
			mbTransformUpdated = true;
	}

	//-----------------------------------------------------------------------

	void cNode3D::SetWorldPosition(const cVector3f& avWorldPos)
	{
		if(mpParent)
		{
			SetPosition(avWorldPos - mpParent->GetWorldPosition());
		}
		else
		{
			SetPosition(avWorldPos);
		}
	}
	
	//-----------------------------------------------------------------------


	void cNode3D::SetWorldMatrix(const cMatrixf& a_mtxWorldTransform)
	{
		if(mpParent)
		{
			SetMatrix(cMath::MatrixMul(cMath::MatrixInverse(mpParent->GetWorldMatrix()),
										a_mtxWorldTransform));
		}
		else
		{
			SetMatrix(a_mtxWorldTransform);
		}
	}

	//-----------------------------------------------------------------------
	
	const tString& cNode3D::GetName()
	{
		return msName;
	}

	//-----------------------------------------------------------------------

	cNode3D* cNode3D::GetParent()
	{
		return mpParent;
	}
	
	//-----------------------------------------------------------------------

	void cNode3D::SetParent(cNode3D* apNode)
	{
		mpParent = apNode;

		if(mpParent)
			mpParent->mlstNode.push_back(this);
	}

	void cNode3D::AddChild(cNode3D* apChild)
	{
		mlstNode.push_back(apChild);
	}

	//-----------------------------------------------------------------------

	void cNode3D::AddRotation(const cVector3f& avRot, eEulerRotationOrder aOrder)
	{
		cQuaternion qRot = cMath::MatrixToQuaternion(cMath::MatrixRotate(avRot, aOrder));
		AddRotation(qRot);
	}
	
	void cNode3D::AddRotation(const cQuaternion& aqRotation)
	{
		mqRotation = cMath::QuaternionMul(aqRotation, mqRotation);
	}

	void cNode3D::AddScale(const cVector3f& avScale)
	{
		mvScale *= avScale;
	}

	void cNode3D::AddTranslation(const cVector3f& avTrans)
	{
		mvTranslation += avTrans;
	}

	void cNode3D::UpdateMatrix(bool abSetChildrenUpdated)
	{
		cMatrixf mtxTransform = GetLocalMatrix();

		//Save the translation and set it to 0 so that only the rotation is altered.
		cVector3f vPos = mtxTransform.GetTranslation();
		mtxTransform.SetTranslation(cVector3f(0,0,0));

		//Log("Startpos: %s",vPos.ToString().c_str());
		//Log("World pos: %s\n",GetWorldMatrix().GetTranslation().ToString().c_str());
		
		//The animation rotation is applied before the local.
		mtxTransform = cMath::MatrixMul(mtxTransform, cMath::MatrixQuaternion(mqRotation));

		//Skip scale for now.
		//mtxTransform = cMath::MatrixMul(cMath::MatrixScale(mvScale), mtxTransform);
		
		mtxTransform.SetTranslation(vPos + mvTranslation);

		SetMatrix(mtxTransform,abSetChildrenUpdated);

		//Log("World pos: %s\n",GetWorldMatrix().GetTranslation().ToString().c_str());
		
		//Reset values
		mqRotation = cQuaternion::Identity;
		mvScale = cVector3f(1,1,1);
		mvTranslation = cVector3f(0,0,0);
	}

	//-----------------------------------------------------------------------

	void cNode3D::ApplyPreAnimTransform(bool abSetChildrenUpdated)
	{
		if(mbUsePreTransform==false) return;

		cMatrixf mtxTransform = GetLocalMatrix();
		mtxTransform = cMath::MatrixMul(mtxTransform, m_mtxPreTransform);

		SetMatrix(mtxTransform,abSetChildrenUpdated);
	}

	void cNode3D::ApplyPostAnimTransform(bool abSetChildrenUpdated)
	{
		if(mbUsePostTransform==false) return;

		cMatrixf mtxTransform = GetLocalMatrix();
		mtxTransform = cMath::MatrixMul(m_mtxPostTransform, mtxTransform);

		SetMatrix(mtxTransform,abSetChildrenUpdated);
	}
	
	//-----------------------------------------------------------------------

	void cNode3D::UpdateEntityChildren()
	{
		//Set all entities as updated
		tEntity3DListIt EIt = mlstEntity.begin();
		for(;EIt != mlstEntity.end();++EIt)
		{
			iEntity3D* pEntity = static_cast<iEntity3D*>(*EIt);

			pEntity->SetTransformUpdated();
		}
	}
	
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	void cNode3D::UpdateWorldTransform()
	{
		if(mbTransformUpdated)
		{
			//if(msName == "WeaponJoint") LogUpdate("  update world transform!\n");

			mbTransformUpdated = false;

			if(mpParent)
			{
				m_mtxWorldTransform = cMath::MatrixMul(mpParent->GetWorldMatrix(),m_mtxLocalTransform);
			}
			else if(mpEntityParent)
			{
				m_mtxWorldTransform = cMath::MatrixMul(mpEntityParent->GetWorldMatrix(),m_mtxLocalTransform);
			}
			else
			{
				m_mtxWorldTransform = m_mtxLocalTransform;
			}
		}
	}
	//-----------------------------------------------------------------------
	
	void cNode3D::SetWorldTransformUpdated()
	{
		//if(msName == "WeaponJoint") LogUpdate("  setworldtransform updated!\n");

		mbTransformUpdated = true;

		//Update entity childre
		UpdateEntityChildren();

		//Set all child nodes as updated
		tNode3DListIt NIt = mlstNode.begin();
		for(;NIt != mlstNode.end();++NIt)
		{
			cNode3D* pNode = static_cast<cNode3D*>(*NIt);

        	pNode->SetWorldTransformUpdated();
		}
	}

	//-----------------------------------------------------------------------
}
