//
// "$Id: fl_show_colormap.H 8864 2011-07-19 04:49:30Z greg.ercolano $"
//
// Colormap picker header file for the Fast Light Tool Kit (FLTK).
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

/** \file
   The fl_show_colormap() function hides the implementation classes used
   to provide the popup window and color selection mechanism.
*/

#ifndef fl_show_colormap_H
#define fl_show_colormap_H

/* doxygen comment here to avoid exposing ColorMenu in fl_show_colormap.cxx
*/

/** \addtogroup  fl_attributes
    @{ */

/**
  \brief Pops up a window to let the user pick a colormap entry.
  \image html fl_show_colormap.png
  \image latex fl_show_colormap.png "fl_show_colormap" height=10cm
  \param[in] oldcol color to be highlighted when grid is shown.
  \retval Fl_Color value of the chosen colormap entry.
  \see Fl_Color_Chooser
*/
FL_EXPORT Fl_Color fl_show_colormap(Fl_Color oldcol);

/** @} */

#endif

//
// End of "$Id: fl_show_colormap.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
