
//! File: <MotorModel.h>
//! Copyright (c) 1997-2007, Reachin Technologies AB.
//! All rights reserved.
//! The copyright to the computer program herein is the property of Reachin
//! Technologies AB, Sweden. The program may be used and/or copied only with the
//! written permission of Reachin Technologies AB or in accordance with the terms
//! stipulated in the agreement under which the program has been supplied.

#ifndef _HAPTX_MOTORMODEL_H_
#define _HAPTX_MOTORMODEL_H_

#include <iostream>

#include "HXSolver.h"
#include "MgMath.h"
#include "ValueError.h"
#include "LinearAlgebra.h"

namespace HaptX 
{
	HAPTX_VALUE_ERROR(const mgFloat, BadTemperatureFile);

	namespace GhostDriver 
	{
		class MotorModel
		{
		public:
			MotorModel(const mgFloat& core_temp, const mgFloat& case_temp, const mgFloat& room_temp,
					   const mgFloat& core_case_coupling, const mgFloat& case_room_coupling,
					   const mgFloat& core_mass, const mgFloat& case_mass, const mgFloat& exponent,
					   const mgFloat& soft_max_temp, const mgFloat& hard_max_temp);

			virtual void	Read(std::istream& i);
			virtual void	Write(std::ostream& o);
			mgFloat			SaturationVoltage(bool diag = false);
			virtual void	Simulate(const mgFloat& dt, const mgFloat& voltage);

		public:
			mgFloat	t1;
			mgFloat	t2;
			mgFloat	t3;
			mgFloat	r12;
			mgFloat	r23;
			mgFloat	m1i;
			mgFloat	m2i;
			mgFloat	vex;
			mgFloat	sat_t1;
			mgFloat	max_t1;
		};

		class ChassisMotorModel;

		class ChassisModel 
		{
		public:
			ChassisModel(const mgFloat& temp, const mgFloat& room_temp, const mgFloat& chassis_room_coupling, const mgFloat& heat_rate, const mgFloat& mass);

			virtual void	Read(std::istream& i);
			virtual void	Write(std::ostream& o);
			virtual void	Simulate(const mgFloat& dt, const bool& amps);

		public:
			mgFloat	t3;
			mgFloat	t4;
			mgFloat	r34;
			mgFloat	m4i;
			mgFloat	h;

			std::vector<ChassisMotorModel*> motors;
		};

		class ChassisMotorModel : public MotorModel 
		{
		public:
			ChassisMotorModel(const mgFloat& core_temp, const mgFloat& case_temp, const mgFloat& room_temp,
							  const mgFloat& core_case_coupling, const mgFloat& case_room_coupling,
							  const mgFloat& case_chassis_coupling, ChassisModel* _chassis,
							  const mgFloat& core_mass, const mgFloat& case_mass, const mgFloat& exponent,
							  const mgFloat& soft_max_temp, const mgFloat& hard_max_temp);

			virtual void	Simulate(const mgFloat& dt, const mgFloat& voltage);

		public:
			mgFloat			r24;
			ChassisModel*	chassis;
		};
	}
}

#endif
