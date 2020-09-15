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

#include "graphics/Color.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cColor::cColor(float afR, float afG, float afB, float afA)
	{
		r = afR; g = afG; b = afB; a = afA;
	}
	
	//-----------------------------------------------------------------------

	cColor::cColor(float afR, float afG, float afB)
	{
		r = afR; g = afG; b = afB; a = 1;
	}

	//-----------------------------------------------------------------------

	cColor::cColor()
	{
		r = 0; g = 0; b = 0; a = 1;
	}
	
	//-----------------------------------------------------------------------

	cColor::cColor(float afVal)
	{
		r = afVal; g = afVal; b = afVal; a = 1;
	}
	
	//-----------------------------------------------------------------------

	cColor::cColor(float afVal, float afA)
	{
		r = afVal; g = afVal; b = afVal; a = afA;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void cColor::FromVec(float *apV)
	{
		r = apV[0];
		g = apV[1];
		b = apV[2];
		a = apV[3];
	}
	
	//-----------------------------------------------------------------------
	
	cColor cColor::operator*(float afVal) const
	{
		cColor col;
		col.r = r * afVal;
		col.g = g * afVal;
		col.b = b * afVal;
		col.a = a * afVal;
		return col;
	}

	cColor cColor::operator/(float afVal) const
	{
		cColor col;
		col.r = r / afVal;
		col.g = g / afVal;
		col.b = b / afVal;
		col.a = a / afVal;
		return col;
	}

	//-----------------------------------------------------------------------

	
	cColor cColor::operator+(const cColor &aCol) const
	{
		return cColor(
			r + aCol.r,
			g + aCol.g,
			b + aCol.b,
			a + aCol.a
			);
	}

	cColor cColor::operator-(const cColor &aCol) const
	{
		return cColor(
			r - aCol.r,
			g - aCol.g,
			b - aCol.b,
			a - aCol.a
			);
	}

	cColor cColor::operator*(const cColor &aCol) const
	{
		return cColor(
			r * aCol.r,
			g * aCol.g,
			b * aCol.b,
			a * aCol.a
			);
	}

	cColor cColor::operator/(const cColor &aCol) const
	{
		return cColor(
			r / aCol.r,
			g / aCol.g,
			b / aCol.b,
			a / aCol.a
			);
	}

	//-----------------------------------------------------------------------

	bool cColor::operator==(cColor aCol) const
	{
		if(r == aCol.r && g == aCol.g && b == aCol.b && a == aCol.a) 
			return true;
        else 
			return false;
	}

	//-----------------------------------------------------------------------
	
	tString cColor::ToString() const
	{
		char buf[512];
		sprintf(buf,"%f : %f : %f : %f",r,g,b,a);
		tString str = buf;
		return str;
	}


	//-----------------------------------------------------------------------

	tString cColor::ToFileString() const
	{
		char buf[512];
		sprintf(buf,"%g %g %g %g",r,g,b,a);
		tString str = buf;
		return str;
	}


	//-----------------------------------------------------------------------
}
