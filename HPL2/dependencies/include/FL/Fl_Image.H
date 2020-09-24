//
// "$Id: Fl_Image.H 10379 2014-10-14 13:32:59Z AlbrechtS $"
//
// Image header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2014 by Bill Spitzak and others.
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
   Fl_Image, Fl_RGB_Image classes. */

#ifndef Fl_Image_H
#  define Fl_Image_H

#  include "Enumerations.H"
#include <stdlib.h>

class Fl_Widget;
class Fl_Pixmap;
struct Fl_Menu_Item;
struct Fl_Label;

/** \enum Fl_RGB_Scaling
    The scaling algorithm to use for RGB images.
*/
enum Fl_RGB_Scaling {
	FL_RGB_SCALING_NEAREST = 0, ///< default RGB image scaling algorithm
	FL_RGB_SCALING_BILINEAR ///< more accurate, but slower RGB image scaling algorithm
};

/**
  Fl_Image is the base class used for caching and
  drawing all kinds of images in FLTK. This class keeps track of
  common image data such as the pixels, colormap, width, height,
  and depth. Virtual methods are used to provide type-specific
  image handling.
  
  Since the Fl_Image class does not support image
  drawing by itself, calling the draw() method results in
  a box with an X in it being drawn instead.
*/
class FL_EXPORT Fl_Image {
  int w_, h_, d_, ld_, count_;
  const char * const *data_;
  static Fl_RGB_Scaling RGB_scaling_;

  // Forbid use of copy constructor and assign operator
  Fl_Image & operator=(const Fl_Image &);
  Fl_Image(const Fl_Image &);

  protected:

  /**
    Sets the current image width in pixels.
  */
  void w(int W) {w_ = W;}
  /**
    Sets the current image height in pixels.
  */
  void h(int H) {h_ = H;}
  /**
    Sets the current image depth.
  */
  void d(int D) {d_ = D;}
  /**
   Sets the current line data size in bytes.
   */
  void ld(int LD) {ld_ = LD;}
  /** 
   Sets the current array pointer and count of pointers in the array.
   */
  void data(const char * const *p, int c) {data_ = p; count_ = c;}
  void draw_empty(int X, int Y);

  static void labeltype(const Fl_Label *lo, int lx, int ly, int lw, int lh, Fl_Align la);
  static void measure(const Fl_Label *lo, int &lw, int &lh);

  public:

  /** 
   Returns the current image width in pixels.
  */
  int w() const {return w_;}
  /**     Returns the current image height in pixels.
   */
  int h() const {return h_;}
  /**
    Returns the current image depth. 
   The return value will be 0 for bitmaps, 1 for
    pixmaps, and 1 to 4 for color images.</P>
  */
  int d() const {return d_;}
  /**
    Returns the current line data size in bytes. 
    Line data is extra data that is included
    after each line of color image data and is normally not present.
  */
  int ld() const {return ld_;}
  /**
    The count() method returns the number of data values
    associated with the image. The value will be 0 for images with
    no associated data, 1 for bitmap and color images, and greater
    than 2 for pixmap images.
  */
  int count() const {return count_;}
  /**
    Returns a pointer to the current image data array. 
    Use the count() method to find the size of the data array.
  */
  const char * const *data() const {return data_;}
  
  /**
    The constructor creates an empty image with the specified
    width, height, and depth. The width and height are in pixels.
    The depth is 0 for bitmaps, 1 for pixmap (colormap) images, and
    1 to 4 for color images.
  */
  Fl_Image(int W, int H, int D) {w_ = W; h_ = H; d_ = D; ld_ = 0; count_ = 0; data_ = 0;}
  virtual ~Fl_Image();
  virtual Fl_Image *copy(int W, int H);
  /**
    The copy() method creates a copy of the specified
    image. If the width and height are provided, the image is
    resized to the specified size. The image should be deleted (or in
    the case of Fl_Shared_Image, released) when you are done
    with it.
  */
  Fl_Image *copy() { return copy(w(), h()); }
  virtual void color_average(Fl_Color c, float i);
  /**
    The inactive() method calls
    color_average(FL_BACKGROUND_COLOR, 0.33f) to produce 
    an image that appears grayed out. <I>This method does not 
    alter the original image data.</I>
  */
  void inactive() { color_average(FL_GRAY, .33f); }
  virtual void desaturate();
  virtual void label(Fl_Widget*w);
  virtual void label(Fl_Menu_Item*m);
  /**
    Draws the image with a bounding box. 
    This form specifies
    a bounding box for the image, with the origin
    (upper-lefthand corner) of the image offset by the cx
    and cy arguments.
  */
  virtual void draw(int X, int Y, int W, int H, int cx=0, int cy=0); // platform dependent
  /**
    Draws the image. 
    This form specifies the upper-lefthand corner of the image.
  */
  void draw(int X, int Y) {draw(X, Y, w(), h(), 0, 0);} // platform dependent
  virtual void uncache();

  // set RGB image scaling method
  static void RGB_scaling(Fl_RGB_Scaling);

  // get RGB image scaling method
  static Fl_RGB_Scaling RGB_scaling();
};

/**
  The Fl_RGB_Image class supports caching and drawing
  of full-color images with 1 to 4 channels of color information.
  Images with an even number of channels are assumed to contain
  alpha information, which is used to blend the image with the
  contents of the screen.

  Fl_RGB_Image is defined in
  &lt;FL/Fl_Image.H&gt;, however for compatibility reasons
  &lt;FL/Fl_RGB_Image.H&gt; should be included.
*/
class FL_EXPORT Fl_RGB_Image : public Fl_Image {
  friend class Fl_Quartz_Graphics_Driver;
  friend class Fl_GDI_Graphics_Driver;
  friend class Fl_Xlib_Graphics_Driver;
  static size_t max_size_;
public:

  const uchar *array;
  int alloc_array; // Non-zero if array was allocated

  private:

#if defined(__APPLE__) || defined(WIN32)
  void *id_; // for internal use
  void *mask_; // for internal use (mask bitmap)
#else
  unsigned id_; // for internal use
  unsigned mask_; // for internal use (mask bitmap)
#endif // __APPLE__ || WIN32

  public:

/**
  The constructor creates a new image from the specified data.
  \param[in] bits   The image data array.
  \param[in] W      The width of the image in pixels
  \param[in] H      The height of the image in pixels
  \param[in] D      The image depth, or 'number of channels'. Default=3<br>
                    If D=1, each uchar in bits[] is a grayscale pixel value.<br>
		    If D=2, each uchar pair in bits[] is a grayscale + alpha pixel value.<br>
		    If D=3, each uchar triplet in bits[] is an R/G/B pixel value<br>
		    If D=4, each uchar quad in bits[] is an R/G/B/A pixel value.
  \param[in] LD     Line data size (default=0).<br>
                    Line data is extra data that is included after each line
		    of color image data and is normally not present. 
  \see Fl_Image::data(), Fl_Image::w(), Fl_Image::h(), Fl_Image::d(), Fl_Image::ld()
  */
  Fl_RGB_Image(const uchar *bits, int W, int H, int D=3, int LD=0) :
    Fl_Image(W,H,D), array(bits), alloc_array(0), id_(0), mask_(0) {data((const char **)&array, 1); ld(LD);}
  Fl_RGB_Image(const Fl_Pixmap *pxm, Fl_Color bg=FL_GRAY);
  virtual ~Fl_RGB_Image();
  virtual Fl_Image *copy(int W, int H);
  Fl_Image *copy() { return copy(w(), h()); }
  virtual void color_average(Fl_Color c, float i);
  virtual void desaturate();
  virtual void draw(int X, int Y, int W, int H, int cx=0, int cy=0);
  void draw(int X, int Y) {draw(X, Y, w(), h(), 0, 0);}
  virtual void label(Fl_Widget*w);
  virtual void label(Fl_Menu_Item*m);
  virtual void uncache();
  /** Sets the maximum allowed image size in bytes when creating an Fl_RGB_Image object.
   
   The image size in bytes of an Fl_RGB_Image object is the value of the product w() * h() * d().
   If this product exceeds size, the created object of a derived class of Fl_RGB_Image 
   won't be loaded with the image data.
   This does not apply to direct RGB image creation with 
   Fl_RGB_Image::Fl_RGB_Image(const uchar *bits, int W, int H, int D, int LD).
   The default max_size() value is essentially infinite. 
   */
  static void max_size(size_t size) { max_size_ = size;}
  /** Returns the maximum allowed image size in bytes when creating an Fl_RGB_Image object.
   
   \sa  void Fl_RGB_Image::max_size(size_t)
   */
  static size_t max_size() {return max_size_;}
};

#endif // !Fl_Image_H

//
// End of "$Id: Fl_Image.H 10379 2014-10-14 13:32:59Z AlbrechtS $".
//
