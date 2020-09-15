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

#ifndef LUX_MAP_HELPER_H
#define LUX_MAP_HELPER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class iLuxEntity;

//----------------------------------------------

class cLuxLineOfSightCallback : public iPhysicsRayCallback
{
public:
	void Reset();

	bool BeforeIntersect(iPhysicsBody *pBody);
	bool OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams);

	bool GetIntersected(){ return mbIntersected;}
	void SetCheckShadow(bool abX){ mbCheckShadow = abX;}

private:
	bool mbIntersected;
	bool mbCheckShadow;
};

//----------------------------------------------

class cLuxClosestEntityCallback : public iPhysicsRayCallback
{
public:
	void Reset();

	bool BeforeIntersect(iPhysicsBody *apBody);
	bool OnIntersect(iPhysicsBody *apBody,cPhysicsRayParams *apParams);
	
	float mfClosestDist;
	iPhysicsBody *mpClosestBody;
	int mlCheckCount;
};

//----------------------------------------------

class cLuxClosestCharColliderCallback : public iPhysicsRayCallback
{
public:
	void Reset();

	bool BeforeIntersect(iPhysicsBody *apBody);
	bool OnIntersect(iPhysicsBody *apBody,cPhysicsRayParams *apParams);

	float mfClosestDist;
	cVector3f mvClosestNormal;
	iPhysicsBody *mpClosestBody;
	int mlCheckCount;
};

//----------------------------------------------

class cLuxAttackRayCallback : public iPhysicsRayCallback
{
public:
	void Setup(iPhysicsBody *apSkipBody);

	bool BeforeIntersect(iPhysicsBody *apBody);
	bool OnIntersect(iPhysicsBody *apBody,cPhysicsRayParams *apParams);

	iPhysicsBody *mpSkipBody;
	bool mbIntersection;
};

//----------------------------------------------

class cLuxMapHelper : public iLuxUpdateable
{
public:	
	cLuxMapHelper();
	~cLuxMapHelper();
	
	void OnStart();
	void Update(float afTimeStep);
	void Reset();

	bool ShapeDamage(	iCollideShape *apShape, const cMatrixf& a_mtxTransform, const cVector3f &avOrigin,
						float afMinDamage, float afMaxDamage, float afForce, float afMaxImpulse,
						int alStrength, float afHitSpeed, eLuxDamageType aDamageType,eLuxWeaponHitType aWeaponHitType,
						bool abCheckEnemies, bool abCheckPlayer, bool abCheckProps, bool abLethalForPlayer,
						bool *apHitPlayer=NULL);

	bool CheckLineOfSight(const cVector3f& avStart, const cVector3f& avEnd, bool abCheckShadows);

	bool GetClosestEntity(	const cVector3f& avStart,const cVector3f& avDir, float afRayLength,
							float *afDistance, iPhysicsBody** apBody, iLuxEntity **apEntity);

	bool GetClosestCharCollider(const cVector3f& avStart,const cVector3f& avDir, float afRayLength,
								float *afDistance, cVector3f *avNormal, iPhysicsBody** apBody);

	float GetLightLevelAtPos(const cVector3f& avPos, std::vector<iLight*>* apSkipLightsVec=NULL, float afRadiusAdd=0);

private:
	void GetLightsAtNode(iRenderableContainerNode *apNode, tLightList &alstLights, const cVector3f& avPos);	

	cLuxLineOfSightCallback mLineOfSightCallback;
	cLuxClosestEntityCallback mClosestEntityCallback;
	cLuxClosestCharColliderCallback mClosestharColliderCallback;
	cLuxAttackRayCallback mAttackRayCallback;
	
};

//----------------------------------------------


#endif // LUX_MAP_HELPER_H
