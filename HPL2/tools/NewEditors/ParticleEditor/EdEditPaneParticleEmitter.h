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

#ifndef ED_EDIT_PANE_EMITTER_H
#define ED_EDIT_PANE_EMITTER_H

#include "../Common/EdEditPane.h"

class iEditorBase;
class cEdInputString;
class cEdInputBool;
class cEdInputNumber;
class cEdInputEnum;
class cEdInputVec2;
class cEdInputVec3;
class cEdInputColor;

class cEdObjParticleEmitter;

//-------------------------------------------------------------

class cEdEditPaneParticleEmitter : public iEdEditPane
{
public:
	cEdEditPaneParticleEmitter(iEdObject*);
	~cEdEditPaneParticleEmitter();

	void SetCurrentEmitter(cEdObjParticleEmitter* apEmitter);

protected:
	void OnCreateLayout();
	void OnUpdate();

	bool WindowSpecificInputCallback(iEdInput*);

	void OnSetActive(bool);

	////////////////////////////////////////////////////
	// Data
	cWidgetTabFrame* mpTFCategories;

	cEdInputText* mpInpName;
	cEdInputNumber* mpInpMaxParticles;
	cEdInputNumber* mpInpParticlesPerSec;
	cEdInputNumber* mpInpStartTimeOffset;
	cEdInputBool* mpInpRespawn;
	cEdInputNumber* mpInpWarmUpTime;
	cEdInputNumber* mpInpWarmUpStepsPerSec;

	cEdInputVec2* mpInpLength;
	cEdInputVec2* mpInpInterval;

	cEdInputVec3* mpInpOffsetPos;
	cEdInputVec3* mpInpOffsetAng;

	cEdInputEnum* mpInpDrawType;
	cEdInputBool* mpInpMulRGBWithAlpha;
	cEdInputVec2* mpInpMinSize;
	cEdInputVec2* mpInpMaxSize;

	cEdInputNumber* mpInpStartRelSize;
	cEdInputNumber* mpInpMiddleRelSizeStart;
	cEdInputNumber* mpInpMiddleRelSize;
	cEdInputNumber* mpInpMiddleRelSizeLength;
	cEdInputNumber* mpInpEndRelSize;

	cEdInputBool* mpInpUseCollision;
	cEdInputNumber* mpInpUpdateRate;
	cEdInputVec2* mpInpMaxCollisions;
	cEdInputVec2* mpInpBounceAmount;

	cEdInputBool* mpInpUsePartSpin;
	cEdInputEnum* mpInpSpinType;
	cEdInputVec2* mpInpSpinVelRange;
	cEdInputBool* mpInpUseRev;
	cEdInputVec3* mpInpMinRevVel;
	cEdInputVec3* mpInpMaxRevVel;

	cEdInputEnum* mpInpStartType;

	cEdInputVec3* mpInpBoxStartMinPos;
	cEdInputVec3* mpInpBoxStartMaxPos;
	cEdInputVec2* mpInpSphStartMinAng;
	cEdInputVec2* mpInpSphStartMaxAng;
	cEdInputVec2* mpInpSphStartRadius;

	cEdInputColor* mpInpMinCol;
	cEdInputColor* mpInpMaxCol;

	cEdInputColor* mpInpStartRelCol;
	cEdInputNumber* mpInpMiddleRelColStart;
	cEdInputColor* mpInpMiddleRelCol;
	cEdInputNumber* mpInpMiddleRelColLength;
	cEdInputColor* mpInpEndRelCol;

	cEdInputEnum* mpInpVelType;
	cEdInputEnum* mpInpCoordSys;
	cEdInputBool* mpInpUseDir;

	cEdInputEnum* mpInpGravType;
	cEdInputVec3* mpInpGravAcc;

	cEdInputVec3* mpInpMinVel;
	cEdInputVec3* mpInpMaxVel;

	cEdInputVec2* mpInpSphVelMinAng;
	cEdInputVec2* mpInpSphVelMaxAng;
	cEdInputVec2* mpInpSphVelSpeed;
    
	cEdInputVec3* mpInpMinAcc;
	cEdInputVec3* mpInpMaxAcc;
	cEdInputVec2* mpInpSpeedMul;
	cEdInputVec2* mpInpMaxSpeed;

	cEdInputVec2* mpInpLifeSpan;
	cEdInputEnum* mpInpDeathType;
	cEdInputFile* mpInpDeathPS;

	cEdInputFile* mpInpMaterial;
	cEdInputNumber* mpInpNumFiles;
	cEdInputNumber* mpInpAnimLen;
    
	cEdInputVec2* mpInpSubDAmount;
	cEdInputEnum* mpInpSubDType;


	static int mlLastEditedTab;
};

//-------------------------------------------------------------


#endif // ED_EDIT_PANE_EMITTER_H
