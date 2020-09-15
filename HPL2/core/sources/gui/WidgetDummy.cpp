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

#include "gui/WidgetDummy.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWidgetDummy::cWidgetDummy(cGuiSet *apSet, cGuiSkin *apSkin) : iWidget(eWidgetType_Window,apSet, apSkin)
	{
		mbEncloseChildren = true;
	}

	//-----------------------------------------------------------------------

	cWidgetDummy::~cWidgetDummy()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetDummy::SetEncloseChildren(bool abX)
	{
		mbEncloseChildren = abX;
		if(abX)
			OnChildUpdate(NULL);
	}
	
	//-----------------------------------------------------------------------

	void cWidgetDummy::OnChildUpdate(iWidget* apChild)
	{
		if(mbEncloseChildren==false)
			return;

		cVector3f vMinPos = cVector3f(9999.9f);
		cVector3f vMaxPos = cVector3f(-9999.9f);

		tWidgetListIt it = mlstChildren.begin();

		for(;it!=mlstChildren.end();++it)
		{
			iWidget* pWidget = *it;
			const cVector3f& vWidgetUpperLeftCorner = pWidget->GetLocalPosition();
			cVector3f vWidgetLowerRightCorner = vWidgetUpperLeftCorner + pWidget->GetSize();

			if(vWidgetUpperLeftCorner.x<vMinPos.x)
				vMinPos.x = vWidgetUpperLeftCorner.x;
			if(vWidgetUpperLeftCorner.y<vMinPos.y)
				vMinPos.y = vWidgetUpperLeftCorner.y;
			if(vWidgetLowerRightCorner.x>vMaxPos.x)
				vMaxPos.x = vWidgetLowerRightCorner.x;
			if(vWidgetLowerRightCorner.y>vMaxPos.y)
				vMaxPos.y = vWidgetLowerRightCorner.y;
		}
		cVector2f vNewSize = cVector2f(vMaxPos.x-vMinPos.x, vMaxPos.y-vMinPos.y);
		SetSize(vNewSize);
	}
}
