//
// "$Id: win32.H 10312 2014-09-15 09:35:05Z ossman $"
//
// WIN32 header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2012 by Bill Spitzak and others.
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
// include this file if WIN32 is defined.  This is to encourage
// portability of even the system-specific code...

#ifndef FL_DOXYGEN
#ifndef Fl_X_H
#  error "Never use <FL/win32.H> directly; include <FL/x.H> instead."
#endif // !Fl_X_H

#include <windows.h>
typedef HRGN Fl_Region;
typedef HWND Window;
typedef POINT XPoint;

#include <FL/Fl_Window.H>

// this part is included only when compiling the FLTK library or if requested explicitly
#if defined(FL_LIBRARY) || defined(FL_INTERNALS) 

// In some of the distributions, the gcc header files are missing some stuff:
#ifndef LPMINMAXINFO
#define LPMINMAXINFO MINMAXINFO*
#endif
#ifndef VK_LWIN
#define VK_LWIN 0x5B
#define VK_RWIN 0x5C
#define VK_APPS 0x5D
#endif

// some random X equivalents
struct XRectangle {int x, y, width, height;};
extern Fl_Region XRectangleRegion(int x, int y, int w, int h);
inline void XDestroyRegion(Fl_Region r) {DeleteObject(r);}
inline void XClipBox(Fl_Region r,XRectangle* rect) {
    RECT win_rect; GetRgnBox(r,&win_rect);
    rect->x=win_rect.left;
    rect->y=win_rect.top;
    rect->width=win_rect.right-win_rect.left;
    rect->height=win_rect.bottom-win_rect.top;
}
#define XDestroyWindow(a,b) DestroyWindow(b)
#define XMapWindow(a,b) ShowWindow(b, SW_RESTORE)
#define XUnmapWindow(a,b) ShowWindow(b, SW_HIDE)

// this object contains all win32-specific stuff about a window:
// Warning: this object is highly subject to change!
class FL_EXPORT Fl_X {
public:
  // member variables - add new variables only at the end of this block
  Window xid;
  HBITMAP other_xid; // for double-buffered windows
  Fl_Window* w;
  Fl_Region region;
  Fl_X *next;
  int wait_for_expose;
  HDC private_dc; // used for OpenGL
  HCURSOR cursor;
  int custom_cursor;
  HDC saved_hdc;  // saves the handle of the DC currently loaded
  // static variables, static functions and member functions
  static Fl_X* first;
  static Fl_X* i(const Fl_Window* w) {return w->i;}
  static int fake_X_wm(const Fl_Window* w,int &X, int &Y,
		                 int &bt,int &bx,int &by);
  void make_fullscreen(int X, int Y, int W, int H);
  void setwindow(Fl_Window* wi) {w=wi; wi->i=this;}
  void flush() {w->flush();}
  void set_minmax(LPMINMAXINFO minmax);
  void mapraise();
  static void set_default_icons(const Fl_RGB_Image*[], int);
  static void set_default_icons(HICON, HICON);
  void set_icons();
  int set_cursor(Fl_Cursor);
  int set_cursor(const Fl_RGB_Image*, int, int);
  static Fl_X* make(Fl_Window*);
};
extern FL_EXPORT UINT fl_wake_msg;
extern FL_EXPORT char fl_override_redirect; // hack into Fl_Window::make_xid()
extern FL_EXPORT int fl_background_pixel;  // hack into Fl_Window::make_xid()
extern FL_EXPORT HPALETTE fl_palette; // non-zero only on 8-bit displays!
extern FL_EXPORT void fl_release_dc(HWND w, HDC dc);
extern FL_EXPORT void fl_save_dc( HWND w, HDC dc);

inline Window fl_xid(const Fl_Window* w) { Fl_X *temp = Fl_X::i(w); return temp ? temp->xid : 0; }

extern FL_EXPORT void fl_open_display();

#else
FL_EXPORT Window fl_xid_(const Fl_Window* w);
#define fl_xid(w) fl_xid_(w)
#endif // FL_LIBRARY || FL_INTERNALS

FL_EXPORT Fl_Window* fl_find(Window xid);
void fl_clip_region(Fl_Region);

// most recent fl_color() or fl_rgbcolor() points at one of these:
extern FL_EXPORT struct Fl_XMap {
  COLORREF rgb;	// this should be the type the RGB() macro returns
  HPEN pen;	// pen, 0 if none created yet
  int brush;	// ref to solid brush, 0 if none created yet
} *fl_current_xmap;
inline COLORREF fl_RGB() {return fl_current_xmap->rgb;}
inline HPEN fl_pen() {return fl_current_xmap->pen;}
FL_EXPORT HBRUSH fl_brush(); // allocates a brush if necessary
FL_EXPORT HBRUSH fl_brush_action(int); // now does the real work

extern FL_EXPORT HINSTANCE fl_display;
extern FL_EXPORT Window fl_window;
extern FL_EXPORT HDC fl_gc;
extern FL_EXPORT MSG fl_msg;
extern FL_EXPORT HDC fl_GetDC(Window);
extern FL_EXPORT HDC fl_makeDC(HBITMAP);

// off-screen pixmaps: create, destroy, draw into, copy to window
typedef HBITMAP Fl_Offscreen;
#define fl_create_offscreen(w, h) \
  CreateCompatibleBitmap( (fl_gc ? fl_gc : fl_GetDC(0) ) , w, h)

# define fl_begin_offscreen(b) \
   HDC _sgc=fl_gc; Window _sw=fl_window; \
   Fl_Surface_Device *_ss = Fl_Surface_Device::surface(); Fl_Display_Device::display_device()->set_current(); \
   fl_gc=fl_makeDC(b); int _savedc = SaveDC(fl_gc); fl_window=(HWND)b; fl_push_no_clip()

# define fl_end_offscreen() \
   fl_pop_clip(); RestoreDC(fl_gc, _savedc); DeleteDC(fl_gc); _ss->set_current(); fl_window=_sw; fl_gc = _sgc


FL_EXPORT void fl_copy_offscreen(int x,int y,int w,int h,HBITMAP pixmap,int srcx,int srcy);
#define fl_delete_offscreen(bitmap) DeleteObject(bitmap)

// Bitmap masks
typedef HBITMAP Fl_Bitmask;

extern FL_EXPORT Fl_Bitmask fl_create_bitmask(int w, int h, const uchar *data);
extern FL_EXPORT Fl_Bitmask fl_create_alphamask(int w, int h, int d, int ld, const uchar *data);
extern FL_EXPORT void fl_delete_bitmask(Fl_Bitmask bm);

// Dummy function to register a function for opening files via the window manager...
inline void fl_open_callback(void (*)(const char *)) {}

extern FL_EXPORT int fl_parse_color(const char* p, uchar& r, uchar& g, uchar& b);
#endif // FL_DOXYGEN
//
// End of "$Id: win32.H 10312 2014-09-15 09:35:05Z ossman $".
//
