//
// "$Id: mac.H 10305 2014-09-13 14:29:53Z manolo $"
//
// Mac header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2011 by Bill Spitzak and others.
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

// Do not directly include this file, instead use <FL/x.H>.  It will
// include this file if "__APPLE__" is defined.  This is to encourage
// portability of even the system-specific code...
#ifndef FL_DOXYGEN

#if !defined(Fl_X_H)
#  error "Never use <FL/mac.H> directly; include <FL/x.H> instead."
#endif // !Fl_X_H

#ifdef __OBJC__
@class FLWindow; // a subclass of the NSWindow Cocoa class
typedef FLWindow *Window;
#else
typedef class FLWindow_opaque *Window; // pointer to the FLWindow objective-c class
#endif // __OBJC__

#if !(defined(FL_LIBRARY) || defined(FL_INTERNALS)) // this part is used when compiling an application program
#  include <FL/Fl_Widget.H>

typedef struct flCocoaRegion* Fl_Region;
typedef struct CGContext* CGContextRef;
typedef struct OpaquePMPrintSettings*   PMPrintSettings;
typedef struct OpaquePMPageFormat*      PMPageFormat;
typedef struct OpaquePMPrintSession*    PMPrintSession;
typedef struct CGImage* CGImageRef;
typedef CGContextRef Fl_Offscreen;

#else // this part must be compiled when building the FLTK libraries

// Standard MacOS C/C++ includes...
#include <ApplicationServices/ApplicationServices.h>
#undef check // because of Fl::check()

#ifndef MAC_OS_X_VERSION_10_4
#define MAC_OS_X_VERSION_10_4 1040
#endif
#ifndef MAC_OS_X_VERSION_10_5
#define MAC_OS_X_VERSION_10_5 1050
#endif
#ifndef MAC_OS_X_VERSION_10_6
#define MAC_OS_X_VERSION_10_6 1060
#endif
#ifndef MAC_OS_X_VERSION_10_7
#define MAC_OS_X_VERSION_10_7 1070
#endif
#ifndef MAC_OS_X_VERSION_10_8
#define MAC_OS_X_VERSION_10_8 1080
#endif
#ifndef MAC_OS_X_VERSION_10_9
#define MAC_OS_X_VERSION_10_9 1090
#endif
#ifndef MAC_OS_X_VERSION_10_10
#define MAC_OS_X_VERSION_10_10 101000
#endif

#ifdef __OBJC__
@class NSCursor;
#else
class NSCursor;
#endif // __OBJC__

typedef CGContextRef Fl_Offscreen;

typedef struct flCocoaRegion {
  int count;
  CGRect *rects;
} *Fl_Region;  // a region is the union of a series of rectangles

#  include "Fl_Window.H"
#  include "../src/Fl_Font.H"

// Some random X equivalents
struct XPoint { int x, y; };
struct XRectangle {int x, y, width, height;};
#ifndef CGFLOAT_DEFINED //appears with 10.5 in CGBase.h
#if defined(__LP64__) && __LP64__
typedef double CGFloat;
#else
typedef float CGFloat;
#endif
#endif // CGFLOAT_DEFINED

extern CGRect fl_cgrectmake_cocoa(int x, int y, int w, int h);
inline Fl_Region XRectangleRegion(int x, int y, int w, int h) {
  Fl_Region R = (Fl_Region)malloc(sizeof(*R));
  R->count = 1;
  R->rects = (CGRect *)malloc(sizeof(CGRect));
  *(R->rects) = fl_cgrectmake_cocoa(x, y, w, h);
  return R;
}
inline void XDestroyRegion(Fl_Region r) {
  if(r) {
    free(r->rects);
    free(r);
  }
}
extern NSCursor *fl_default_cursor;

// This object contains all mac-specific stuff about a window:
// WARNING: this object is highly subject to change!
class Fl_X {
  
public:
  Window xid;              // pointer to the Cocoa window object (FLWindow*)
  Fl_Offscreen other_xid;  // pointer for offscreen bitmaps (overlay window)
  Fl_Window *w;            // FLTK window for 
  Fl_Region region;
  Fl_Region subRegion;     // region for this specific subwindow
  Fl_X *next;              // linked tree to support subwindows
  Fl_X *xidChildren, *xidNext; // more subwindow tree
  int wait_for_expose;
  NSCursor *cursor;
  static Fl_X* first;
  static Fl_X* i(const Fl_Window* w) {return w->i;}
  static int fake_X_wm(const Fl_Window*,int&,int&,int&,int&,int&);
  static void make(Fl_Window*);
  void flush();
  // Quartz additions:
  CGContextRef gc;                 // graphics context (NULL when using QD)
  static void q_fill_context();    // fill a Quartz context with current FLTK state
  static void q_clear_clipping();  // remove all clipping from a Quartz context
  static void q_release_context(Fl_X *x=0); // free all resources associated with fl_gc
  static void q_begin_image(CGRect&, int x, int y, int w, int h);
  static void q_end_image();
  // Cocoa additions
  void destroy(void);
  void map(void);
  void unmap(void);
  int unlink(Fl_X* start = NULL);
  void collapse(void);
  WindowRef window_ref(void);
  void set_key_window(void);
  // OS X doesn't have per window icons
  static void set_default_icons(const Fl_RGB_Image*[], int) {};
  void set_icons() {};
  int set_cursor(Fl_Cursor);
  int set_cursor(const Fl_RGB_Image*, int, int);
  static CGImageRef CGImage_from_window_rect(Fl_Window *win, int x, int y, int w, int h);
  static unsigned char *bitmap_from_window_rect(Fl_Window *win, int x, int y, int w, int h, int *bytesPerPixel);
  static CFDataRef CGBitmapContextToTIFF(CGContextRef c);
  static Fl_Region intersect_region_and_rect(Fl_Region current, int x,int y,int w, int h);
  static CGContextRef watch_cursor_image(void);
  static CGContextRef help_cursor_image(void);
  static CGContextRef nesw_cursor_image(void);
  static CGContextRef nwse_cursor_image(void);
  static CGContextRef none_cursor_image(void);
  static void *get_carbon_function(const char *name);
  static void screen_work_area(int &X, int &Y, int &W, int &H, int n); // compute work area of a given screen
  static int next_marked_length; // next length of marked text after current marked text will have been replaced
  static int insertion_point_location(int *px, int *py, int *pheight); // computes window coordinates & height of insertion point
  static const int CoreText_threshold;  // Mac OS version from which the Core Text API is used to display text
  static Fl_Fontdesc* calc_fl_fonts(void); // computes the fl_fonts global variable
private:
  static void relink(Fl_Window*, Fl_Window*);
  bool subwindow;
};

extern Window fl_window;

#endif // FL_LIBRARY || FL_INTERNALS

typedef CGImageRef Fl_Bitmask;

extern CGContextRef fl_gc;

extern Window fl_xid(const Fl_Window*);
extern Fl_Window* fl_find(Window xid);
void fl_clip_region(Fl_Region);

extern Fl_Bitmask fl_create_bitmask(int w, int h, const uchar *data);
extern Fl_Bitmask fl_create_alphamask(int w, int h, int d, int ld, const uchar *data);
extern void fl_delete_bitmask(Fl_Bitmask bm);
extern Fl_Offscreen fl_create_offscreen(int w, int h);
extern void fl_copy_offscreen(int x,int y,int w,int h, Fl_Offscreen gWorld, int srcx,int srcy);
extern void fl_delete_offscreen(Fl_Offscreen gWorld);
extern void fl_begin_offscreen(Fl_Offscreen gWorld);
extern void fl_end_offscreen();

extern int fl_parse_color(const char* p, uchar& r, uchar& g, uchar& b);
extern void fl_open_display();

#endif // FL_DOXYGEN
/** \file
 Mac OS X-specific symbols.
 */

/** \defgroup group_macosx Mac OS X-specific symbols
 Mac OS X-specific symbols declared in <FL/x.H> or <FL/gl.h>
 \sa \ref osissues_macos
 @{ */

/** @brief Register a function called for each file dropped onto an application icon.
 \e cb will be called with a single Unix-style file name and path.
 If multiple files were dropped, \e cb will be called multiple times.
 */
extern void fl_open_callback(void (*cb)(const char *));

/** 
 * \brief Attaches a callback to the "About myprog" item of the system application menu.
 *
 * \param cb   a callback that will be called by "About myprog" menu item
 *		   with NULL 1st argument.
 * \param user_data   a pointer transmitted as 2nd argument to the callback.
 * \param shortcut    optional shortcut to attach to the "About myprog" menu item (e.g., FL_META+'a')
 */
extern void fl_mac_set_about( Fl_Callback *cb, void *user_data, int shortcut = 0);

/** \brief The version number of the running Mac OS X (e.g., 100604 for 10.6.4)
 */
extern int fl_mac_os_version;

/** The system menu bar.
 */
extern class Fl_Sys_Menu_Bar *fl_sys_menu_bar;


class Fl_Mac_App_Menu {
public:
  /** Localizable text for the "About xxx" application menu item */
  static const char *about;
  /** Localizable text for the "Print Front Window" application menu item.
   This menu item won't be displayed if Fl_Mac_App_Menu::print 
   is set to an empty string.
   */
  static const char *print;
  /** Localizable text for the "Services" application menu item */
  static const char *services;
  /** Localizable text for the "Hide xxx" application menu item */
  static const char *hide;
  /** Localizable text for the "Hide Others" application menu item */
  static const char *hide_others;
  /** Localizable text for the "Show All" application menu item */
  static const char *show;
  /** Localizable text for the "Quit xxx" application menu item */
  static const char *quit;
};

/** @} */

//
// End of "$Id: mac.H 10305 2014-09-13 14:29:53Z manolo $".
//

