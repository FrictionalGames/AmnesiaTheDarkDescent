//
// "$Id: Fl_Input_Choice.H 9740 2012-12-09 17:45:24Z manolo $"
//
// An input/chooser widget.
//            ______________  ____
//           |              || __ |
//           | input area   || \/ |
//           |______________||____|
//
// Copyright 1998-2010 by Bill Spitzak and others.
// Copyright 2004 by Greg Ercolano.
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
   Fl_Input_Choice widget . */

#ifndef Fl_Input_Choice_H
#define Fl_Input_Choice_H

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/fl_draw.H>
#include <string.h>

/**
  A combination of the input widget and a menu button.

  \image html input_choice.jpg
  \image latex input_choice.jpg "Fl_Input_Choice widget" width=6cm

  The user can either type into the input area, or use the
  menu button chooser on the right to choose an item which loads
  the input area with the selected text.
  <P>
  The application can directly access both the internal Fl_Input 
  and Fl_Menu_Button widgets respectively using the input() and menubutton()
  accessor methods.
*/
class FL_EXPORT Fl_Input_Choice : public Fl_Group {
  // Private class to handle slightly 'special' behavior of menu button
  class InputMenuButton : public Fl_Menu_Button {
    void draw() {
      draw_box(FL_UP_BOX, color());
      fl_color(active_r() ? labelcolor() : fl_inactive(labelcolor()));
      int xc = x()+w()/2, yc=y()+h()/2;
      fl_polygon(xc-5,yc-3,xc+5,yc-3,xc,yc+3);
      if (Fl::focus() == this) draw_focus();
    }
  public:
    InputMenuButton(int X,int Y,int W,int H,const char*L=0) : 
	Fl_Menu_Button(X, Y, W, H, L) { box(FL_UP_BOX); }
  };

  Fl_Input *inp_;
  InputMenuButton *menu_;

  static void menu_cb(Fl_Widget*, void *data) { 
    Fl_Input_Choice *o=(Fl_Input_Choice *)data;
    Fl_Widget_Tracker wp(o);
    const Fl_Menu_Item *item = o->menubutton()->mvalue();
    if (item && item->flags & (FL_SUBMENU|FL_SUBMENU_POINTER)) return;	// ignore submenus
    if (!strcmp(o->inp_->value(), o->menu_->text()))
    {
      o->Fl_Widget::clear_changed();
      if (o->when() & FL_WHEN_NOT_CHANGED)
	o->do_callback();
    }
    else
    {
      o->inp_->value(o->menu_->text());
      o->inp_->set_changed();
      o->Fl_Widget::set_changed();
      if (o->when() & (FL_WHEN_CHANGED|FL_WHEN_RELEASE))
	o->do_callback();
    }
    
    if (wp.deleted()) return;

    if (o->callback() != default_callback)
    {
      o->Fl_Widget::clear_changed();
      o->inp_->clear_changed();
    }
  }

  static void inp_cb(Fl_Widget*, void *data) { 
    Fl_Input_Choice *o=(Fl_Input_Choice *)data;
    Fl_Widget_Tracker wp(o);
    if (o->inp_->changed()) {
      o->Fl_Widget::set_changed();
      if (o->when() & (FL_WHEN_CHANGED|FL_WHEN_RELEASE))
	o->do_callback();
    } else {
      o->Fl_Widget::clear_changed();
      if (o->when() & FL_WHEN_NOT_CHANGED)
	o->do_callback();
    }
    
    if (wp.deleted()) return;

    if (o->callback() != default_callback)
      o->Fl_Widget::clear_changed();
  }

  // Custom resize behavior -- input stretches, menu button doesn't
  inline int inp_x() { return(x() + Fl::box_dx(box())); }
  inline int inp_y() { return(y() + Fl::box_dy(box())); }
  inline int inp_w() { return(w() - Fl::box_dw(box()) - 20); }
  inline int inp_h() { return(h() - Fl::box_dh(box())); }

  inline int menu_x() { return(x() + w() - 20 - Fl::box_dx(box())); }
  inline int menu_y() { return(y() + Fl::box_dy(box())); }
  inline int menu_w() { return(20); }
  inline int menu_h() { return(h() - Fl::box_dh(box())); }

public:
  /**
    Creates a new Fl_Input_Choice widget using the given position, size,
    and label string.
    Inherited destructor destroys the widget and any values associated with it.
  */
  Fl_Input_Choice(int X,int Y,int W,int H,const char*L=0);
  
  /** Adds an item to the menu.
   You can access the more complex Fl_Menu_Button::add() methods 
   (setting callbacks, userdata, etc), via menubutton(). Example:
   \code
   Fl_Input_Choice *choice = new Fl_Input_Choice(100,10,120,25,"Fonts");
   Fl_Menu_Button *mb = choice->menubutton();             // use Fl_Input_Choice's Fl_Menu_Button
   mb->add("Helvetica", 0, MyFont_CB,     (void*)mydata); // use Fl_Menu_Button's add() methods
   mb->add("Courier",   0, MyFont_CB,     (void*)mydata);
   mb->add("More..",    0, FontDialog_CB, (void*)mydata);
   \endcode
   */
  void add(const char *s) { menu_->add(s);  }
  /** Returns the combined changed() state of the input and menu button widget. */
  int changed() const { return inp_->changed() | Fl_Widget::changed(); }
  /** Clears the changed() state of both input and menu button widgets. */
  void clear_changed() { 
    inp_->clear_changed();
    Fl_Widget::clear_changed();
  }
  /** Sets the changed() state of both input and menu button widgets
   to the specfied value.*/
  void set_changed() { 
    inp_->set_changed();
    // no need to call Fl_Widget::set_changed()
  }
  /** Removes all items from the menu. */
  void clear() { menu_->clear(); }
  /** Gets the box type of the menu button */
  Fl_Boxtype down_box() const { return (menu_->down_box()); }
  /** Sets the box type of the menu button */
  void down_box(Fl_Boxtype b) { menu_->down_box(b); }
  /** Gets the Fl_Menu_Item array used for the menu. */
  const Fl_Menu_Item *menu() { return (menu_->menu()); }
  /** Sets the Fl_Menu_Item array used for the menu. */
  void menu(const Fl_Menu_Item *m) { menu_->menu(m); }
  void resize(int X, int Y, int W, int H) {
    Fl_Group::resize(X,Y,W,H);
    inp_->resize(inp_x(), inp_y(), inp_w(), inp_h());
    menu_->resize(menu_x(), menu_y(), menu_w(), menu_h());
  }
  /// Gets the Fl_Input text field's text color.
  Fl_Color textcolor() const { return (inp_->textcolor());}
  /// Sets the Fl_Input text field's text color to \p c.
  void textcolor(Fl_Color c) { inp_->textcolor(c);}
  /// Gets the Fl_Input text field's font style.
  Fl_Font textfont() const { return (inp_->textfont());}
  /// Sets the Fl_Input text field's font style to \p f.
  void textfont(Fl_Font f) { inp_->textfont(f);}
  /// Gets the Fl_Input text field's font size
  Fl_Fontsize textsize() const { return (inp_->textsize()); }
  /// Sets the Fl_Input text field's font size to \p s.
  void textsize(Fl_Fontsize s) { inp_->textsize(s); }
  /// Returns the Fl_Input text field's current contents.
  const char* value() const { return (inp_->value()); }
  /** Sets the Fl_Input text field's contents to \p val.
   Does not affect the menu selection.*/
  void value(const char *val) { inp_->value(val); }
  /** Chooses item# \p val in the menu, and sets the Fl_Input text field
   to that value. Any previous text is cleared.*/
  void value(int val) {
    menu_->value(val);
    inp_->value(menu_->text(val));
  }
  /** Returns a pointer to the internal Fl_Menu_Button widget.
   This can be used to access any of the methods of the menu button, e.g.
   \code
   Fl_Input_Choice *choice = new Fl_Input_Choice(100,10,120,25,"Choice:");
   [..]
   // Print all the items in the choice menu
   for ( int t=0; t<choice->menubutton()->size(); t++ ) {
       const Fl_Menu_Item &item = choice->menubutton()->menu()[t];
       printf("item %d -- label=%s\n", t, item.label() ? item.label() : "(Null)");
   }
   \endcode
  */
  Fl_Menu_Button *menubutton() { return menu_; }
  /** Returns a pointer to the internal Fl_Input widget.
   This can be used to directly access all of the Fl_Input widget's
   methods.*/
  Fl_Input *input() { return inp_; }
};

#endif // !Fl_Input_Choice_H

//
// End of "$Id: Fl_Input_Choice.H 9740 2012-12-09 17:45:24Z manolo $".
//
