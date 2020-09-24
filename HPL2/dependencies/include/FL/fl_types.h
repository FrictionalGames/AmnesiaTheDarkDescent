/*
 * "$Id: fl_types.h 8864 2011-07-19 04:49:30Z greg.ercolano $"
 *
 * Simple "C"-style types for the Fast Light Tool Kit (FLTK).
 *
 * Copyright 1998-2010 by Bill Spitzak and others.
 *
 * This library is free software. Distribution and use rights are outlined in
 * the file "COPYING" which should have been included with this file.  If this
 * file is missing or damaged, see the license at:
 *
 *     http://www.fltk.org/COPYING.php
 *
 * Please report all bugs and problems on the following page:
 *
 *     http://www.fltk.org/str.php
 */

/** \file 
 *  This file contains simple "C"-style type definitions.
 */

#ifndef FL_TYPES_H
#define FL_TYPES_H

/** \name	Miscellaneous */
/*@{*/	/* group: Miscellaneous */

/** unsigned char */
typedef unsigned char uchar;
/** unsigned long */
typedef unsigned long ulong;

/** Flexible length utf8 Unicode text
 *
 *  \todo FIXME: temporary (?) typedef to mark UTF8 and Unicode conversions
 */
typedef char *Fl_String;

/** Flexible length utf8 Unicode read-only string
 *  \sa Fl_String
 */
typedef const char *Fl_CString;

/** 24-bit Unicode character + 8-bit indicator for keyboard flags */
typedef unsigned int Fl_Shortcut;

/** 24-bit Unicode character - upper 8-bits are unused */
typedef unsigned int Fl_Char;
 
/*@}*/	/* group: Miscellaneous */

#endif

/*
 * End of "$Id: fl_types.h 8864 2011-07-19 04:49:30Z greg.ercolano $".
 */
