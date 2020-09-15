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

#ifndef HPLEDITOR_PARTICLE_EDITOR_WINDOW_EMITTER_PARAMS_H
#define HPLEDITOR_PARTICLE_EDITOR_WINDOW_EMITTER_PARAMS_H

#include "../common/EditorWindow.h"

class iEditorBase;
class cEditorInputString;
class cEditorInputBool;
class cEditorInputNumber;
class cEditorInputEnum;
class cEditorInputVec2;
class cEditorInputVec3;
class cEditorInputColorFrame;

class cEntityWrapperParticleEmitter;

//-------------------------------------------------------------

class cParticleEditorWindowEmitterParams : public iEditorWindow
{
public:
	cParticleEditorWindowEmitterParams(iEditorBase* apEditor);
	~cParticleEditorWindowEmitterParams();

	void SetCurrentEmitter(cEntityWrapperParticleEmitter* apEmitter);

protected:
	void OnInitLayout();
	void OnUpdate(float afTimeStep);

	bool WindowSpecificInputCallback(iEditorInput* apInput);

	cWidgetTabFrame* mpTFCategories;

	cEditorInputText* mpInpName;
	cEditorInputNumber* mpInpMaxParticles;
	cEditorInputNumber* mpInpParticlesPerSec;
	cEditorInputNumber* mpInpStartTimeOffset;
	cEditorInputBool* mpInpRespawn;
	cEditorInputNumber* mpInpWarmUpTime;
	cEditorInputNumber* mpInpWarmUpStepsPerSec;

	cEditorInputVec2* mpInpLength;
	cEditorInputVec2* mpInpInterval;

	cEditorInputVec3* mpInpOffsetPos;
	cEditorInputVec3* mpInpOffsetAng;

	cEditorInputEnum* mpInpDrawType;
	cEditorInputBool* mpInpMulRGBWithAlpha;
	cEditorInputVec2* mpInpMinSize;
	cEditorInputVec2* mpInpMaxSize;

	cEditorInputNumber* mpInpStartRelSize;
	cEditorInputNumber* mpInpMiddleRelSizeStart;
	cEditorInputNumber* mpInpMiddleRelSize;
	cEditorInputNumber* mpInpMiddleRelSizeLength;
	cEditorInputNumber* mpInpEndRelSize;

	cEditorInputBool* mpInpUseCollision;
	cEditorInputNumber* mpInpUpdateRate;
	cEditorInputVec2* mpInpMaxCollisions;
	cEditorInputVec2* mpInpBounceAmount;

	cEditorInputBool* mpInpUsePartSpin;
	cEditorInputEnum* mpInpSpinType;
	cEditorInputVec2* mpInpSpinVelRange;
	cEditorInputBool* mpInpUseRev;
	cEditorInputVec3* mpInpMinRevVel;
	cEditorInputVec3* mpInpMaxRevVel;

	cEditorInputEnum* mpInpStartType;

	cEditorInputVec3* mpInpBoxStartMinPos;
	cEditorInputVec3* mpInpBoxStartMaxPos;
	cEditorInputVec2* mpInpSphStartMinAng;
	cEditorInputVec2* mpInpSphStartMaxAng;
	cEditorInputVec2* mpInpSphStartRadius;

	cEditorInputColorFrame* mpInpMinCol;
	cEditorInputColorFrame* mpInpMaxCol;

	cEditorInputColorFrame* mpInpStartRelCol;
	cEditorInputNumber* mpInpMiddleRelColStart;
	cEditorInputColorFrame* mpInpMiddleRelCol;
	cEditorInputNumber* mpInpMiddleRelColLength;
	cEditorInputColorFrame* mpInpEndRelCol;

	cEditorInputEnum* mpInpVelType;
	cEditorInputEnum* mpInpCoordSys;
	cEditorInputBool* mpInpUseDir;

	cEditorInputEnum* mpInpGravType;
	cEditorInputVec3* mpInpGravAcc;

	cEditorInputVec3* mpInpMinVel;
	cEditorInputVec3* mpInpMaxVel;

	cEditorInputVec2* mpInpSphVelMinAng;
	cEditorInputVec2* mpInpSphVelMaxAng;
	cEditorInputVec2* mpInpSphVelSpeed;
    
	cEditorInputVec3* mpInpMinAcc;
	cEditorInputVec3* mpInpMaxAcc;
	cEditorInputVec2* mpInpSpeedMul;
	cEditorInputVec2* mpInpMaxSpeed;

	cEditorInputVec2* mpInpLifeSpan;
	cEditorInputEnum* mpInpDeathType;
	cEditorInputFile* mpInpDeathPS;

	cEditorInputFile* mpInpMaterial;
	cEditorInputNumber* mpInpNumFiles;
	cEditorInputNumber* mpInpAnimLen;
    
	cEditorInputVec2* mpInpSubDAmount;
	cEditorInputEnum* mpInpSubDType;


	cEntityWrapperParticleEmitter* mpEmitter;
};

//-------------------------------------------------------------


#endif // HPLEDITOR_PARTICLE_EDITOR_WINDOW_EMITTER_PARAMS_H
