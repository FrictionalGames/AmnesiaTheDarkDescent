//
// "$Id: Fl_Tabs.H 10120 2014-03-23 17:36:59Z greg.ercolano $"
//
// Tab header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

/* \file
   Fl_Tabs widget . */

#ifndef Fl_Tabs_H
#define Fl_Tabs_H

#include "Fl_Group.H"

/**
  The Fl_Tabs widget is the "file card tabs"
  interface that allows you to put lots and lots of buttons and
  switches in a panel, as popularized by many toolkits.
  
  \image html  tabs.png
  \image latex tabs.png "Fl_Tabs" width=8cm
  
  Clicking the tab makes a child visible() by calling
  show() on it, and all other children are made invisible
  by calling hide() on them. Usually the children are Fl_Group widgets
  containing several widgets themselves.
  
  Each child makes a card, and its label() is printed
  on the card tab, including the label font and style.  The
  selection color of that child is used to color the tab, while
  the color of the child determines the background color of the pane.
  
  The size of the tabs is controlled by the bounding box of the
  children (there should be some space between the children and
  the edge of the Fl_Tabs), and the tabs may be placed
  "inverted" on the bottom - this is determined by which
  gap is larger. It is easiest to lay this out in fluid, using the
  fluid browser to select each child group and resize them until
  the tabs look the way you want them to.
  
  The background area behind and to the right of the tabs is
  "transparent", exposing the background detail of the parent. The
  value of Fl_Tabs::box() does not affect this area. So if Fl_Tabs is
  resized by itself without the parent, force the appropriate parent
  (visible behind the tabs) to redraw() to prevent artifacts.

  See "Resizing Caveats" below on how to keep tab heights constant.
  See "Callback's Use Of when()" on how to control the details
  of how clicks invoke the callback().

  A typical use of the Fl_Tabs widget:

  \par
  \code
      // Typical use of Fl_Tabs
      Fl_Tabs *tabs = new Fl_Tabs(10,10,300,200);
      {
          Fl_Group *grp1 = new Fl_Group(20,30,280,170,"Tab1");
	  {
	      ..widgets that go in tab#1..
          }
	  grp1->end();
          Fl_Group *grp2 = new Fl_Group(20,30,280,170,"Tab2");
	  {
	      ..widgets that go in tab#2..
	  }
	  grp2->end();
      }
      tabs->end();
  \endcode

  \b Default \b Appearance

  The appearance of each "tab" is taken from the label() and color() of the
  child group corresponding to that "tab" and panel. Where the "tabs" appear
  depends on the position and size of the child groups that make up the
  panels within the Fl_Tab, i.e. whether there is more space above or
  below them. The height of the "tabs" depends on how much free space
  is available.

  \image html  tabs_default.png "Fl_Tabs Default Appearance"
  \image latex tabs_default.png "Fl_Tabs Default Appearance" width=8cm

  \b Highlighting \b The \b Selected \b Tab

  The selected "tab" can be highlighted further by setting the
  selection_color() of the Fl_Tab itself, e.g.

  \par
  \code
  ..
  tabs = new Fl_Tabs(..);
  tabs->selection_color(FL_DARK3);
  ..
  \endcode

  The result of the above looks like:
  \image html  tabs_selection.png "Highlighting the selected tab"
  \image latex tabs_selection.png "Highlighting the selected tab" width=8cm

  \b Uniform \b Tab \b and \b Panel \b Appearance

  In order to have uniform tab and panel appearance, not only must the color()
  and selection_color() for each child group be set, but also the 
  selection_color() of the Fl_Tab itself any time a new "tab" is selected.
  This can be achieved within the Fl_Tab callback, e.g.

  \par
  \code
  void MyTabCallback(Fl_Widget *w, void*) {
    Fl_Tabs *tabs = (Fl_Tabs*)w;
    // When tab changed, make sure it has same color as its group
    tabs->selection_color( (tab->value())->color() );
  }
  ..
  int main(..) {
    // Define tabs widget
    tabs = new Fl_Tabs(..);
    tabs->callback(MyTabCallback);

    // Create three tabs each colored differently
    grp1 = new Fl_Group(.. "One");
     grp1->color(9);
     grp1->selection_color(9);
    grp1->end();

    grp2 = new Fl_Group(.. "Two");
     grp2->color(10);
     grp2->selection_color(10);
    grp2->end();

    grp3 = new Fl_Group(.. "Three");
     grp3->color(14);
     grp3->selection_color(14);
    grp3->end();
    ..
    // Make sure default tab has same color as its group
    tabs->selection_color( (tab->value())->color() );
    ..
    return Fl::run();
  }
  \endcode

  The result of the above looks like:
  \image html  tabs_uniform.png "Fl_Tabs with uniform colors"
  \image latex tabs_uniform.png "Fl_Tabs with uniform colors" width=8cm

  \b Resizing \b Caveats

  When Fl_Tabs is resized vertically, the default behavior scales the
  tab's height as well as its children.  To keep the tab height constant
  during resizing, set the tab widget's resizable() to one of the tab's
  child groups, i.e.

  \par
  \code
    tabs = new Fl_Tabs(..);
    grp1 = new Fl_Group(..);
    ..
    grp2 = new Fl_Group(..);
    ..
    tabs->end();
    tabs->resizable(grp1);	// keeps tab height constant
  \endcode

  \par Callback's Use Of when()

  As of FLTK 1.3.3, Fl_Tabs() supports the following flags for when():

    - \ref FL_WHEN_NEVER       -- callback never invoked (all flags off)
    - \ref FL_WHEN_CHANGED     -- if flag set, invokes callback when a tab has been changed (on click or keyboard navigation)
    - \ref FL_WHEN_NOT_CHANGED -- if flag set, invokes callback when the tabs remain unchanged (on click or keyboard navigation)
    - \ref FL_WHEN_RELEASE     -- if flag set, invokes callback on RELEASE of mouse button or keyboard navigation

  Notes:

    -#  The above flags can be logically OR-ed (|) or added (+) to combine behaviors.
    -#  The default value for when() is \ref FL_WHEN_RELEASE (inherited from Fl_Widget).
    -#  If \ref FL_WHEN_RELEASE is the \em only flag specified, 
        the behavior will be as if (\ref FL_WHEN_RELEASE|\ref FL_WHEN_CHANGED) was specified.
    -#  The value of changed() will be valid during the callback.
    -#  If both \ref FL_WHEN_CHANGED and \ref FL_WHEN_NOT_CHANGED are specified, 
        the callback is invoked whether the tab has been changed or not.
	The changed() method can be used to determine the cause.
    -#  \ref FL_WHEN_NOT_CHANGED can happen if someone clicks on an already selected tab,
        or if a keyboard navigation attempt results in no change to the tabs,
	such as using the arrow keys while at the left or right end of the tabs.
*/
class FL_EXPORT Fl_Tabs : public Fl_Group {
  Fl_Widget *value_;
  Fl_Widget *push_;
  int *tab_pos;		// array of x-offsets of tabs per child + 1
  int *tab_width;	// array of widths of tabs per child + 1
  int tab_count;	// array size
  int tab_positions();	// allocate and calculate tab positions
  void clear_tab_positions();
  int tab_height();
  void draw_tab(int x1, int x2, int W, int H, Fl_Widget* o, int sel=0);
protected:
  void redraw_tabs();
  void draw();

public:
  int handle(int);
  Fl_Widget *value();
  int value(Fl_Widget *);
  /**
    Returns the tab group for the tab the user has currently down-clicked on
    and remains over until FL_RELEASE. Otherwise, returns NULL.

    While the user is down-clicked on a tab, the return value is the tab group
    for that tab. But as soon as the user releases, or drags off the tab with
    the button still down, the return value will be NULL.

    \see push(Fl_Widget*).
  */
  Fl_Widget *push() const {return push_;}
  int push(Fl_Widget *);
  Fl_Tabs(int,int,int,int,const char * = 0);
  Fl_Widget *which(int event_x, int event_y);
  ~Fl_Tabs();
  void client_area(int &rx, int &ry, int &rw, int &rh, int tabh=0);
};

#endif

//
// End of "$Id: Fl_Tabs.H 10120 2014-03-23 17:36:59Z greg.ercolano $".
//
