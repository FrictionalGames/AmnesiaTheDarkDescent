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

#ifndef LUX_HAND_OBJECT_MELEE_H
#define LUX_HAND_OBJECT_MELEE_H

//----------------------------------------------

#include "LuxHandObject.h"


//----------------------------------------------

class cLuxHandObject_Melee : public iLuxHandObject
{
public:	
	cLuxHandObject_Melee(const tString& asName, cLuxPlayerHands *apHands);
	~cLuxHandObject_Melee();
	
	void LoadImplementedVars(cXmlElement *apVarsElem);

	void ImplementedCreateEntity(cLuxMap *apMap);
	void ImplementedDestroyEntity(cLuxMap *apMap);

	void RenderSolid(cRendererCallbackFunctions* apFunctions);
	void RenderTrans(cRendererCallbackFunctions* apFunctions);
	
	void ImplementedReset();

	void Update(float afTimeStep);

	bool DoAction(eLuxPlayerAction aAction, bool abPressed);
	bool AnimationIsOver();

private:
	void UpdateCharge(float afTimeStep);
	void UpdateCheckDamageEvent(float afTimeStep);
	
	
	void Charge();
	void Swing();

	tString msHandsAnim_Swing;
	tString msHandsAnim_SwingCharge;
	tString msHandsAnim_Charge;
	tString msSwingSound;
	tString msSwingChargeSound;
	tString msChargeSound;
	tString msChargeDoneSound;
	tString msHitSound;

	cVector3f mvCollideShapeSize;
	cVector3f mvCollideShapeOffset;
	iCollideShape *mpCollideShape;

	float mfChargePlayerSpeedMul;
	float mfChargeTime;
	float mfRelativeDamageTime;

	float mfAttackForce;
	float mfMaxAttackForce;

	float mfAttackMinDamage;
	float mfAttackMaxDamage;
	float mfAttackHitSpeed;
	int mlAttackStrength;
};

//----------------------------------------------



#endif // LUX_HAND_OBJECT_MELEE_H
