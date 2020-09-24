//
// "$Id: Fl_Native_File_Chooser.H 10408 2014-10-29 20:05:05Z cand $"
//
// FLTK native OS file chooser widget
//
// Copyright 1998-2014 by Bill Spitzak and others.
// Copyright 2004 Greg Ercolano.
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

/** \file
   Fl_Native_File_Chooser widget. */

#ifndef FL_NATIVE_FILE_CHOOSER_H
#define FL_NATIVE_FILE_CHOOSER_H

// Use Windows' chooser
#ifdef WIN32
// #define _WIN32_WINNT	0x0501	// needed for OPENFILENAME's 'FlagsEx'
#  include <stdio.h>
#  include <stdlib.h>		// malloc
#  include <windows.h>
#  include <commdlg.h>		// OPENFILENAME, GetOpenFileName()
#  include <shlobj.h>		// BROWSEINFO, SHBrowseForFolder()
#endif

// Use Apple's chooser
#ifdef __APPLE__
#  define MAXFILTERS	80
#endif

// All else falls back to FLTK's own chooser
#if ! defined(__APPLE__) && !defined(WIN32)
#  include <FL/Fl_File_Chooser.H>
#  include <unistd.h>		// _POSIX_NAME_MAX
#else
#  include <FL/filename.H>	// FL_EXPORT
#endif

class Fl_FLTK_File_Chooser;
class Fl_GTK_File_Chooser;

/**
 This class lets an FLTK application easily and consistently access 
 the operating system's native file chooser. Some operating systems 
 have very complex and specific file choosers that many users want 
 access to specifically, instead of FLTK's default file chooser(s). 
 
 In cases where there is no native file browser, FLTK's own file browser
 is used instead.
 
 To use this widget, use the following include in your code:
 \code
 #include <FL/Fl_Native_File_Chooser.H>
 \endcode
 
 The following example shows how to pick a single file:
 \code
 // Create and post the local native file chooser
 #include <FL/Fl_Native_File_Chooser.H>
 [..]
 Fl_Native_File_Chooser fnfc;
 fnfc.title("Pick a file");
 fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
 fnfc.filter("Text\t*.txt\n"
             "C Files\t*.{cxx,h,c}");
 fnfc.directory("/var/tmp");           // default directory to use
 // Show native chooser
 switch ( fnfc.show() ) {
   case -1: printf("ERROR: %s\n", fnfc.errmsg());    break;  // ERROR
   case  1: printf("CANCEL\n");                      break;  // CANCEL
   default: printf("PICKED: %s\n", fnfc.filename()); break;  // FILE CHOSEN
 }
 \endcode
 
 The Fl_Native_File_Chooser widget transmits UTF-8 encoded filenames to its user. It is
 recommended to open files that may have non-ASCII names with the fl_fopen() or
 fl_open() utility functions that handle these names in a cross-platform way 
 (whereas the standard fopen()/open() functions fail on the MSWindows platform 
 to open files with a non-ASCII name).
 
 <B>Platform Specific Caveats</B>
 
 - Under X windows, and if Fl::OPTION_FNFC_USES_GTK has not been switched off,
 the widget attempts to use standard GTK file chooser dialogs if they are
 available at run-time on the platform, and falls back to use FLTK's Fl_File_Chooser if they are not.
 In the latter case, it's best if you call Fl_File_Icon::load_system_icons()
 at the start of main(), to enable the nicer looking file browser widgets.
 Use the static public attributes of class Fl_File_Chooser to localize
 the browser.
 - Some operating systems support certain OS specific options; see 
 Fl_Native_File_Chooser::options() for a list.
 
 \image html Fl_Native_File_Chooser.png "The Fl_Native_File_Chooser on different platforms."
 \image latex Fl_Native_File_Chooser.png "The Fl_Native_File_Chooser on different platforms" width=14cm
 
 */
class FL_EXPORT Fl_Native_File_Chooser {
public:
  enum Type {
    BROWSE_FILE = 0,			///< browse files (lets user choose one file)
    BROWSE_DIRECTORY,			///< browse directories (lets user choose one directory)
    BROWSE_MULTI_FILE,			///< browse files (lets user choose multiple files)
    BROWSE_MULTI_DIRECTORY,		///< browse directories (lets user choose multiple directories)
    BROWSE_SAVE_FILE,			///< browse to save a file
    BROWSE_SAVE_DIRECTORY		///< browse to save a directory
  };
  enum Option {
    NO_OPTIONS     = 0x0000,		///< no options enabled
    SAVEAS_CONFIRM = 0x0001,		///< Show native 'Save As' overwrite confirm dialog
    NEW_FOLDER     = 0x0002,		///< Show 'New Folder' icon (if supported)
    PREVIEW        = 0x0004,		///< enable preview mode (if supported)
    USE_FILTER_EXT = 0x0008		///< Chooser filter pilots the output file extension (if supported)
  };
  /** Localizable message */
  static const char *file_exists_message;
  
public:
  Fl_Native_File_Chooser(int val=BROWSE_FILE);
  ~Fl_Native_File_Chooser();
  
  // Public methods
  void type(int t);
  int type() const ;
  void options(int o);
  int options() const;
  int count() const;
  const char *filename() const ;
  const char *filename(int i) const ;
  void directory(const char *val) ;
  const char *directory() const;
  void title(const char *t);
  const char* title() const;
  const char *filter() const ;
  void filter(const char *f);
  int filters() const ;
  void filter_value(int i) ;
  int filter_value() const ;
  void preset_file(const char*f) ;
  const char* preset_file() const;
  const char *errmsg() const ;
  int show() ;
  
#ifdef WIN32
private:
  int  _btype;			// kind-of browser to show()
  int  _options;		// general options
  OPENFILENAMEW _ofn;		// GetOpenFileName() & GetSaveFileName() struct
  BROWSEINFOW   _binf;		// SHBrowseForFolder() struct
  char  **_pathnames;		// array of pathnames
  int     _tpathnames;		// total pathnames
  char   *_directory;		// default pathname to use
  char   *_title;		// title for window
  char   *_filter;		// user-side search filter
  char   *_parsedfilt;		// filter parsed for Windows dialog
  int     _nfilters;		// number of filters parse_filter counted
  char   *_preset_file;		// the file to preselect
  char   *_errmsg;		// error message
  
  // Private methods
  void errmsg(const char *msg);
  
  void clear_pathnames();
  void set_single_pathname(const char *s);
  void add_pathname(const char *s);
  
  void FreePIDL(LPITEMIDLIST pidl);
  void ClearOFN();
  void ClearBINF();
  void Win2Unix(char *s);
  void Unix2Win(char *s);
  int showfile();
  static int CALLBACK Dir_CB(HWND win, UINT msg, LPARAM param, LPARAM data);
  int showdir();
  
  void parse_filter(const char *);
  void clear_filters();
  void add_filter(const char *, const char *);
#endif

#ifdef __APPLE__
private:
  int             _btype;		// kind-of browser to show()
  int             _options;		// general options
  void 	         *_panel;
  char          **_pathnames;		// array of pathnames
  int             _tpathnames;	        // total pathnames
  char           *_directory;		// default pathname to use
  char           *_title;		// title for window
  char           *_preset_file;	        // the 'save as' filename
  
  char           *_filter;		// user-side search filter, eg:
  // C Files\t*.[ch]\nText Files\t*.txt"
  
  char           *_filt_names;		// filter names (tab delimited)
  // eg. "C Files\tText Files"
  
  char           *_filt_patt[MAXFILTERS];
  // array of filter patterns, eg:
  //     _filt_patt[0]="*.{cxx,h}"
  //     _filt_patt[1]="*.txt"
  
  int             _filt_total;		// parse_filter() # of filters loaded
  int             _filt_value;		// index of the selected filter
  char           *_errmsg;		// error message
  
  // Private methods
  void errmsg(const char *msg);
  void clear_pathnames();
  void set_single_pathname(const char *s);
  int get_saveas_basename(void);
  void clear_filters();
  void add_filter(const char *, const char *);
  void parse_filter(const char *from);
  int post();
  int runmodal();
#endif

#if ! defined(__APPLE__) && !defined(WIN32)
private:
#if FLTK_ABI_VERSION <= 10302
  int   _btype;			// kind-of browser to show()
  int   _options;		// general options
  int   _nfilters;
  char *_filter;		// user supplied filter
  char *_parsedfilt;		// parsed filter
  int   _filtvalue;		// selected filter
  char *_preset_file;
  char *_prevvalue;		// Returned filename
  char *_directory;
  char *_errmsg;		// error message
#endif
  static int have_looked_for_GTK_libs;
  union {
    Fl_FLTK_File_Chooser *_x11_file_chooser;
    Fl_GTK_File_Chooser *_gtk_file_chooser;
  };
#endif
};

#if !defined(__APPLE__) && !defined(WIN32)
class FL_EXPORT Fl_FLTK_File_Chooser {
  friend class Fl_Native_File_Chooser;
protected:
  int   _btype;			// kind-of browser to show()
  int   _options;		// general options
  int   _nfilters;
  char *_filter;		// user supplied filter
  char *_parsedfilt;		// parsed filter
  int   _filtvalue;		// selected filter
  char *_preset_file;
  char *_prevvalue;		// Returned filename
  char *_directory;
  char *_errmsg;		// error message
  Fl_FLTK_File_Chooser(int val);
  virtual ~Fl_FLTK_File_Chooser();
  void errmsg(const char *msg);
  int type_fl_file(int);
  void parse_filter();
  int exist_dialog();
  Fl_File_Chooser *_file_chooser;
  virtual void type(int);
  int type() const;
  void options(int);
  int options() const;
  virtual int count() const;
  virtual const char *filename() const;
  virtual const char *filename(int i) const;
  void directory(const char *val);
  const char *directory() const;
  virtual void title(const char *);
  virtual const char* title() const;
  const char *filter() const;
  void filter(const char *);
  int filters() const;
  void filter_value(int i);
  int filter_value() const;
  void preset_file(const char*);
  const char* preset_file() const;
  const char *errmsg() const;
  virtual int show();
};


class FL_EXPORT Fl_GTK_File_Chooser : public Fl_FLTK_File_Chooser {
  friend class Fl_Native_File_Chooser;
private:
  typedef struct _GtkWidget GtkWidget;
  typedef struct _GtkFileFilterInfo GtkFileFilterInfo;
  struct pair {
    Fl_GTK_File_Chooser* running; // the running Fl_GTK_File_Chooser
    const char *filter; // a filter string of the chooser
    pair(Fl_GTK_File_Chooser* c, const char *f) {
      running = c; 
      filter = strdup(f);
    };
    ~pair() {
      free((char*)filter);
    };
  };
  GtkWidget *gtkw_ptr; // used to hold a GtkWidget* without pulling GTK into everything...
  void *gtkw_slist; // used to hold a GLib GSList...
  unsigned gtkw_count; // number of files read back - if any
  mutable char *gtkw_filename; // last name we read back
  char *gtkw_title; // the title to be applied to the dialog
  const char *previous_filter;
  
  int fl_gtk_chooser_wrapper(); // method that wraps the GTK widget
  Fl_GTK_File_Chooser(int val);
  virtual ~Fl_GTK_File_Chooser();
  static int did_find_GTK_libs;
  static void probe_for_GTK_libs(void);
  virtual void type(int);
  virtual int count() const;
  virtual const char *filename() const;
  virtual const char *filename(int i) const;
  virtual void title(const char *);
  virtual const char* title() const;
  virtual int show();
  void changed_output_type(const char *filter);
  
  static int custom_gtk_filter_function(const GtkFileFilterInfo*, Fl_GTK_File_Chooser::pair*);
  static void free_pair(pair *p);
};
#endif // !defined(__APPLE__) && !defined(WIN32)

#endif /*FL_NATIVE_FILE_CHOOSER_H*/

//
// End of "$Id: Fl_Native_File_Chooser.H 10408 2014-10-29 20:05:05Z cand $".
//
