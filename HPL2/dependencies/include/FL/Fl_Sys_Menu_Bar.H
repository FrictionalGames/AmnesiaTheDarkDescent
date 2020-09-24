//
// "$Id: Fl_Sys_Menu_Bar.H 10102 2014-02-11 17:59:20Z manolo $"
//
// MacOS system menu bar header file for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Sys_Menu_Bar_H
#define Fl_Sys_Menu_Bar_H

#include "Fl_Menu_Bar.H"
#include "x.H"

#if defined(__APPLE__) || defined(FL_DOXYGEN)

/**
  A class to create, modify and delete menus that appear on Mac OS X in the menu bar at the top of the screen.
 
  On other than Mac OS X platforms, Fl_Sys_Menu_Bar is a synonym of class Fl_Menu_Bar.
 \n To use this class, just replace Fl_Menu_Bar by Fl_Sys_Menu_Bar, and, on the Mac platform,
 a system menu at the top of the screen will be available. This menu will match an array
 of Fl_Menu_Item's exactly as with standard FLTK menus.
  \n A few FLTK features are not supported by the Mac System menu:
  \li no symbolic labels
  \li no embossed labels
  \li no font sizes
 
  You can configure a callback for the 'About' menu item to invoke your own code with fl_mac_set_about().
 */
class FL_EXPORT Fl_Sys_Menu_Bar : public Fl_Menu_Bar {
protected:
  void update();
  void draw();
public:
  Fl_Sys_Menu_Bar(int x,int y,int w,int h,const char *l=0);
  ~Fl_Sys_Menu_Bar();
  /** Return the system menu's array of Fl_Menu_Item's 
   */
  const Fl_Menu_Item *menu() const {return Fl_Menu_::menu();}
  void menu(const Fl_Menu_Item *m);
  int add(const char* label, int shortcut, Fl_Callback*, void *user_data=0, int flags=0);
  /** Adds a new menu item. 
   \see Fl_Menu_::add(const char* label, int shortcut, Fl_Callback*, void *user_data=0, int flags=0)
   */
  int add(const char* label, const char* shortcut, Fl_Callback* cb, void *user_data=0, int flags=0) {
    return add(label, fl_old_shortcut(shortcut), cb, user_data, flags);
    }
  int add(const char* str);
  int insert(int index, const char* label, int shortcut, Fl_Callback *cb, void *user_data=0, int flags=0);
  /** Insert a new menu item. 
   \see Fl_Menu_::insert(int index, const char* label, const char* shortcut, Fl_Callback *cb, void *user_data=0, int flags=0)
   */
  int insert(int index, const char* label, const char* shortcut, Fl_Callback *cb, void *user_data=0, int flags=0) {
    return insert(index, label, fl_old_shortcut(shortcut), cb, user_data, flags);
    }
  void remove(int n);
  void replace(int rank, const char *name);
  /** Set the Fl_Menu_Item array pointer to null, indicating a zero-length menu. 
   \see Fl_Menu_::clear()
   */
  void clear();
  /** Clears the specified submenu pointed to by index of all menu items.
   \see Fl_Menu_::clear_submenu(int index)
   */
  int clear_submenu(int index);
  /** Make the shortcuts for this menu work no matter what window has the focus when you type it.
   */
  void global() {};
  /** Sets the flags of item i
   \see Fl_Menu_::mode(int i, int fl) */
  void 	mode (int i, int fl) {
    Fl_Menu_::mode(i, fl);
    update();
    }
  /** Gets the flags of item i.
   */
  int mode(int i) const { return Fl_Menu_::mode(i); }
  /** Changes the shortcut of item i to n.
   */
  void shortcut (int i, int s) { Fl_Menu_::shortcut(i, s); update(); };
};

#else

typedef Fl_Menu_Bar Fl_Sys_Menu_Bar;

#endif // defined(__APPLE__) || defined(FL_DOXYGEN)

#endif // Fl_Sys_Menu_Bar_H

//
// End of "$Id: Fl_Sys_Menu_Bar.H 10102 2014-02-11 17:59:20Z manolo $".
//
