//
// "$Id: Fl_Tree.H 10275 2014-09-05 12:04:28Z cand $"
//

#ifndef FL_TREE_H
#define FL_TREE_H

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/fl_draw.H>

#include <FL/Fl_Tree_Item.H>
#include <FL/Fl_Tree_Prefs.H>

//////////////////////
// FL/Fl_Tree.H
//////////////////////
//
// Fl_Tree -- This file is part of the Fl_Tree widget for FLTK
// Copyright (C) 2009-2010 by Greg Ercolano.
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

///
/// \file
/// \brief This file contains the definitions of the Fl_Tree class
///

/// \class Fl_Tree
///
/// \brief Tree widget.
///
///     \image html tree-simple.png "Fl_Tree example program"
///     \image latex tree-simple.png "Fl_Tree example program" width=4cm
///
/// \code
///     Fl_Tree                                         // Top level widget
///        |--- Fl_Tree_Item                            // Items in the tree
///        |--- Fl_Tree_Prefs                           // Preferences for the tree
///                  |--- Fl_Tree_Connector (enum)      // Connection modes
///                  |--- Fl_Tree_Select (enum)         // Selection modes
///                  |--- Fl_Tree_Sort (enum)           // Sort behavior
/// \endcode
///
///     Similar to Fl_Browser, Fl_Tree is a browser of Fl_Tree_Item's arranged
///     in a parented hierarchy, or 'tree'. Subtrees can be expanded or closed.
///     Items can be added, deleted, inserted, sorted and re-ordered.
///
///     The tree items may also contain other FLTK widgets, like buttons, input fields,
///     or even "custom" widgets.
///
///     The callback() is invoked depending on the value of when():
///
///         - FL_WHEN_RELEASE -- callback invoked when left mouse button is released on an item
///         - FL_WHEN_CHANGED -- callback invoked when left mouse changes selection state
///
///     The simple way to define a tree:
/// \code
///    #include <FL/Fl_Tree.H>
///    [..]
///    Fl_Tree tree(X,Y,W,H);
///    tree.begin();
///      tree.add("Flintstones/Fred");
///      tree.add("Flintstones/Wilma");
///      tree.add("Flintstones/Pebbles");
///      tree.add("Simpsons/Homer");
///      tree.add("Simpsons/Marge");
///      tree.add("Simpsons/Bart");
///      tree.add("Simpsons/Lisa");
///    tree.end();
/// \endcode
///     
/// \par FEATURES
///     Items can be added with add(),<BR>
///     removed with remove(),<BR>
///     completely cleared with clear(),<BR>
///     inserted with insert() and insert_above(),<BR>
///     selected/deselected with select() and deselect(),<BR>
///     open/closed with open() and close(),<BR>
///     positioned on the screen with show_item_top(), show_item_middle() and
///     show_item_bottom(),<BR>
///     item children can be swapped around with Fl_Tree_Item::swap_children(),<BR>
///     sorting can be controlled when items are add()ed via sortorder().<BR>
///     You can walk the entire tree with first() and next().<BR>
///     You can walk visible items with first_visible_item()
///     and next_visible_item().<BR>
///     You can walk selected items with first_selected_item() and
///     next_selected_item().<BR>
///     Items can be found by their pathname using find_item(const char*),
///     and an item's pathname can be found with item_pathname().<BR>
///     The selected items' colors are controlled by selection_color()
///     (inherited from Fl_Widget).<BR>
///     A hook is provided to allow you to redefine how item's labels are drawn
///     via Fl_Tree::item_draw_callback().<BR>
///
/// \par SELECTION OF ITEMS
///     The tree can have different selection behaviors controlled by selectmode().
///     The background color used for selected items is the Fl_Tree::selection_color().
///     The foreground color for selected items is controlled internally with fl_contrast().
///
/// \par CHILD WIDGETS
///     FLTK widgets (including custom widgets) can be assigned to tree items via
///     Fl_Tree_Item::widget().
/// \par
///     When an Fl_Tree_Item::widget() is defined, the default behavior is for the
///     widget() to be shown in place of the item's label (if it has one).
///     Only the widget()'s width will be used; the widget()'s x() and y() position
///     will be managed by the tree, and the h() will track the item's height.
///     This default behavior can be altered (ABI 1.3.1): 
///     Setting Fl_Tree::item_draw_mode()'s FL_TREE_ITEM_DRAW_LABEL_AND_WIDGET flag
///     causes the label + widget to be displayed together in that order, and
///     adding the FL_TREE_ITEM_HEIGHT_FROM_WIDGET flag causes widget's height
///     to define the widget()'s height.
///
/// \par ICONS
///     The tree's open/close icons can be redefined with
///     Fl_Tree::openicon(), Fl_Tree::closeicon(). User icons
///     can either be changed globally with Fl_Tree::usericon(),
///     or on a per-item basis with Fl_Tree_Item::usericon().
/// \par
///     Various default preferences can be globally manipulated via Fl_Tree_Prefs, 
///     including colors, margins, icons, connection lines, etc. 
///
/// \par FONTS AND COLORS
///     When adding new items to the tree, the new items get the
///     defaults for fonts and colors from:
/// \par
///	- Fl_Tree::item_labelfont() -- The default item label font (default: FL_HELVETICA)
///     - Fl_Tree::item_labelsize() -- The default item label size (default: FL_NORMAL_SIZE)
///     - Fl_Tree::item_labelfgcolor() -- The default item label foreground color (default: FL_FOREGROUND_COLOR)
///     - Fl_Tree::item_labelbgcolor() -- The default item label background color (default: 0xffffffff, which tree uses as 'transparent')
/// \par
///     Each item (Fl_Tree_Item) inherits a copy of these font/color attributes when created,
///     and each item has its own methods to let the app change these values on a per-item basis
///	using methods of the same name:
/// \par
///	- Fl_Tree_Item::labelfont() -- The item's label font (default: FL_HELVETICA)
///     - Fl_Tree_Item::labelsize() -- The item's label size (default: FL_NORMAL_SIZE)
///     - Fl_Tree_Item::labelfgcolor() -- The item's label foreground color (default: FL_FOREGROUND_COLOR)
///     - Fl_Tree_Item::labelbgcolor() -- The item's label background color (default: 0xffffffff, which uses the tree's own bg color)
///
/// \par CALLBACKS
///     The tree's callback() will be invoked when items change state or are open/closed.
///     when() controls when mouse/keyboard events invoke the callback.
///     callback_item() and callback_reason() can be used to determine the cause of the callback. e.g.
/// \par
/// \code
/// void MyTreeCallback(Fl_Widget *w, void *data) {
///   Fl_Tree      *tree = (Fl_Tree*)w;
///   Fl_Tree_Item *item = (Fl_Tree_Item*)tree->callback_item();	// get selected item
///   switch ( tree->callback_reason() ) {
///     case FL_TREE_REASON_SELECTED: [..]
///     case FL_TREE_REASON_DESELECTED: [..]
///     case FL_TREE_REASON_RESELECTED: [..]
///     case FL_TREE_REASON_OPENED: [..]
///     case FL_TREE_REASON_CLOSED: [..]
///   }
/// \endcode
///
/// \par SIMPLE EXAMPLES
///     To find all the selected items:
/// \code
/// for ( Fl_Tree_Item *i=first_selected_item(); i; i=next_selected_item(i) )
///   printf("Item %s is selected\n", i->label());
/// \endcode
///     To get an item's full menu pathname, use Fl_Tree::item_pathname(), e.g.
/// \code
///   char pathname[256] = "???";
///   tree->item_pathname(pathname, sizeof(pathname), item);		// eg. "Parent/Child/Item"
/// \endcode
/// \par
///     To walk all the items of the tree from top to bottom:
/// \code
/// // Walk all the items in the tree, and print their labels
/// for ( Fl_Tree_Item *item = tree->first(); item; item = tree->next(item) ) {
///     printf("Item: %s\n", item->label());
/// }
/// \endcode
/// \par
///     To recursively walk all the children of a particular item, 
///     define a function that uses recursion:
///     \code
/// // Find all of the item's children and print an indented report of their labels
/// void my_print_all_children(Fl_Tree_Item *item, int indent=0) {
///     for ( int t=0; t<item->children(); t++ ) {
///         printf("%*s Item: %s\n", indent, "", item->child(t)->label());
///         my_print_all_children(item->child(t), indent+4);   // recurse
///     }
/// }
///     \endcode
/// \par
///     To change the default label font and color when creating new items:
/// \code
///  tree = new Fl_Tree(..);
///  tree->item_labelfont(FL_COURIER);	// Use Courier font for all new items
///  tree->item_labelfgcolor(FL_RED);	// Use red color for labels of all new items
///  [..]
///  // Now create the items in the tree using the above defaults.
///  tree->add("Aaa");
///  tree->add("Bbb");
///  [..]
/// \endcode
/// \par
///     To change the font and color of all existing items in the tree:
/// \code
/// // Change the font and color of all items currently in the tree
/// for ( Fl_Tree_Item *item = tree->first(); item; item = tree->next(item) ) {
///     item->labelfont(FL_COURIER);
///     item->labelcolor(FL_RED);
/// }
/// \endcode
///
/// \par DISPLAY DESCRIPTION
///     The following image shows the tree's various visual elements
///     and the methods that control them:
/// \par
///     \image html tree-elements.png
///     \image latex tree-elements.png "Fl_Tree elements" width=6cm
/// \par
///     The following shows the protected 'tree inner' (tix..)
///     and 'tree outer' (tox..) dimension variables:
///     \image html tree-dimensions.png "Fl_Tree inner/outer dimensions" width=6cm
///     \image latex tree-dimensions.png "Fl_Tree inner/outer dimensions" width=6cm
///
/// \par KEYBOARD BINDINGS
///     The following table lists keyboard bindings for navigating the tree:
///
///  <TABLE BORDER="1" SUMMARY="Fl_Tree keyboard bindings.">
///    <CAPTION ALIGN=TOP>Fl_Tree keyboard bindings.</CAPTION>
///  <TR>
///    <TD WIDTH=25% ALIGN=CENTER><B>Keyboard</B></TD>
///    <TD WIDTH=25% ALIGN=CENTER><B>FL_TREE_SELECT_MULTI</B></TD>
///    <TD WIDTH=25% ALIGN=CENTER><B>FL_TREE_SELECT_SINGLE</B></TD>
///    <TD WIDTH=25% ALIGN=CENTER><B>FL_TREE_SELECT_NONE</B></TD>
///
///  </TR><TR>
///    <TD ALIGN=CENTER><B>Ctrl-A</B> (Linux/Windows)<BR><B>Command-A</B> (Mac)</TD>
///    <TD ALIGN=CENTER>Select all items.</TD>
///    <TD ALIGN=CENTER>N/A</TD>
///    <TD ALIGN=CENTER>N/A</TD>
///
///  </TR><TR>
///    <TD ALIGN=CENTER><B>Space </B></TD>
///    <TD ALIGN=CENTER>Selects item.</TD>
///    <TD ALIGN=CENTER>Selects item.</TD>
///    <TD ALIGN=CENTER>N/A</TD>
///
///  </TR><TR>
///    <TD ALIGN=CENTER><B>Ctrl-Space </B></TD>
///    <TD ALIGN=CENTER>Toggle item.</TD>
///    <TD ALIGN=CENTER>Toggle item.</TD>
///    <TD ALIGN=CENTER>N/A</TD>
///
///  </TR><TR>
///    <TD ALIGN=CENTER><B>Shift-Space </B></TD>
///    <TD ALIGN=CENTER>Extends selection<BR>from last item.</TD>
///    <TD ALIGN=CENTER>Selects item.</TD>
///    <TD ALIGN=CENTER>N/A</TD>
///
///  </TR><TR>
///    <TD ALIGN=CENTER><B>Enter,<BR>Ctrl-Enter,<BR>Shift-Enter </B></TD>
///    <TD ALIGN=CENTER>Toggles open/close</TD>
///    <TD ALIGN=CENTER>Toggles open/close</TD>
///    <TD ALIGN=CENTER>Toggles open/close</TD>
///
///  </TR><TR>
///    <TD ALIGN=CENTER><B>Right / Left</B></TD>
///    <TD ALIGN=CENTER>Open/Close item.</TD>
///    <TD ALIGN=CENTER>Open/Close item.</TD>
///    <TD ALIGN=CENTER>Open/Close item.</TD>
///
///  </TR><TR>
///    <TD ALIGN=CENTER><B>Up / Down</B></TD>
///    <TD ALIGN=CENTER>Move focus box up/down.</TD>
///    <TD ALIGN=CENTER>Move focus box up/down.</TD>
///    <TD ALIGN=CENTER>N/A</TD>
///
///  </TR><TR>
///    <TD ALIGN=CENTER><B>Shift-Up / Shift-Down</B></TD>
///    <TD ALIGN=CENTER>Extend selection up/down.</TD>
///    <TD ALIGN=CENTER>Move focus up/down.</TD>
///    <TD ALIGN=CENTER>N/A</TD>
///
///  </TR><TR>
///    <TD ALIGN=CENTER><B>Home / End</B></TD>
///    <TD ALIGN=CENTER>Move to top/bottom of tree.</TD>
///    <TD ALIGN=CENTER>Move to top/bottom of tree.</TD>
///    <TD ALIGN=CENTER>Move to top/bottom of tree.</TD>
///
///  </TR><TR>
///    <TD ALIGN=CENTER><B>PageUp / PageDown</B></TD>
///    <TD ALIGN=CENTER>Page up/down.</TD>
///    <TD ALIGN=CENTER>Page up/down.</TD>
///    <TD ALIGN=CENTER>Page up/down.</TD>
///
///  </TD></TR></TABLE>
///

/// \enum Fl_Tree_Reason
/// The reason the callback was invoked.
///
enum Fl_Tree_Reason {
  FL_TREE_REASON_NONE=0,	///< unknown reason
  FL_TREE_REASON_SELECTED,	///< an item was selected
  FL_TREE_REASON_DESELECTED,	///< an item was de-selected
#if FLTK_ABI_VERSION >= 10301
  FL_TREE_REASON_RESELECTED,	///< an item was re-selected (e.g. double-clicked)
#endif /*FLTK_ABI_VERSION*/
  FL_TREE_REASON_OPENED,	///< an item was opened
  FL_TREE_REASON_CLOSED,	///< an item was closed
  FL_TREE_REASON_DRAGGED	///< an item was dragged into a new place
};

class FL_EXPORT Fl_Tree : public Fl_Group {
  friend class Fl_Tree_Item;
  Fl_Tree_Item  *_root;				// can be null!
  Fl_Tree_Item  *_item_focus;			// item that has focus box
  Fl_Tree_Item  *_callback_item;		// item invoked during callback (can be NULL)
  Fl_Tree_Reason _callback_reason;		// reason for the callback
  Fl_Tree_Prefs  _prefs;			// all the tree's settings
  int            _scrollbar_size;		// size of scrollbar trough
#if FLTK_ABI_VERSION >= 10301
  // NEW: 
  Fl_Tree_Item *_lastselect;
#else /*FLTK_ABI_VERSION*/
  // OLD: static data inside handle() method
#endif /*FLTK_ABI_VERSION*/
  void fix_scrollbar_order();

protected:
  Fl_Scrollbar *_vscroll;	///< Vertical scrollbar
#if FLTK_ABI_VERSION >= 10303
  Fl_Scrollbar *_hscroll;	///< Horizontal scrollbar
  int _tox,_toy,_tow,_toh;	///< Tree widget outer xywh dimension: outside scrollbars, inside widget border
  int _tix,_tiy,_tiw,_tih;	///< Tree widget inner xywh dimension: inside borders + scrollbars

  /// the calculated width of the entire tree hierarchy. See calc_tree()
  int _tree_w;
  /// the calculated height of the entire tree hierarchy. See calc_tree()
  int _tree_h;
#endif
  void item_clicked(Fl_Tree_Item* val);
  void do_callback_for_item(Fl_Tree_Item* item, Fl_Tree_Reason reason);
#if FLTK_ABI_VERSION >= 10303
// next_visible_item() and extend_selection() moved to 'public' in ABI 1.3.3
// undocmented draw_tree() dropped -- draw() does all the work now
#else
  Fl_Tree_Item *next_visible_item(Fl_Tree_Item *start, int dir);
  void extend_selection(Fl_Tree_Item *from, Fl_Tree_Item *to);
  int draw_tree();
#endif

public:
  Fl_Tree(int X, int Y, int W, int H, const char *L=0);
  ~Fl_Tree();
  int handle(int e);
  void draw();
  void show_self();
  void resize(int,int,int,int);

  ///////////////////////
  // root methods
  ///////////////////////
  void root_label(const char *new_label);
  Fl_Tree_Item* root();
  void root(Fl_Tree_Item *newitem);
  const Fl_Tree_Prefs& prefs() const { return _prefs; }

  ////////////////////////////////
  // Item creation/removal methods
  ////////////////////////////////
#if FLTK_ABI_VERSION >= 10303
  Fl_Tree_Item *add(const char *path, Fl_Tree_Item *newitem=0);
#else
  Fl_Tree_Item *add(const char *path);
  Fl_Tree_Item *add(const char *path, Fl_Tree_Item *newitem);
#endif
  Fl_Tree_Item* add(Fl_Tree_Item *parent_item, const char *name);
  Fl_Tree_Item *insert_above(Fl_Tree_Item *above, const char *name);
  Fl_Tree_Item* insert(Fl_Tree_Item *item, const char *name, int pos);
  int remove(Fl_Tree_Item *item);
  void clear();
  void clear_children(Fl_Tree_Item *item);

  ////////////////////////
  // Item lookup methods
  ////////////////////////
  Fl_Tree_Item *find_item(const char *path);
  const Fl_Tree_Item *find_item(const char *path) const;
  int item_pathname(char *pathname, int pathnamelen, const Fl_Tree_Item *item) const;
#if FLTK_ABI_VERSION >= 10303
  const Fl_Tree_Item* find_clicked(int yonly=0) const;
  Fl_Tree_Item* find_clicked(int yonly=0);
#else
  const Fl_Tree_Item *find_clicked() const;
  Fl_Tree_Item *find_clicked();
#endif
  Fl_Tree_Item *item_clicked();
  Fl_Tree_Item *first();
  Fl_Tree_Item *first_visible();		// deprecated in ABI 10303
  Fl_Tree_Item *first_visible_item();
  Fl_Tree_Item *next(Fl_Tree_Item *item=0);
  Fl_Tree_Item *prev(Fl_Tree_Item *item=0);
  Fl_Tree_Item *last();
  Fl_Tree_Item *last_visible();			// deprecated in ABI 10303
  Fl_Tree_Item *last_visible_item();
#if FLTK_ABI_VERSION >= 10303
  Fl_Tree_Item *next_visible_item(Fl_Tree_Item *start, int dir);	// made public in 1.3.3 ABI
#endif
  Fl_Tree_Item *first_selected_item();
  Fl_Tree_Item *last_selected_item();
  Fl_Tree_Item *next_item(Fl_Tree_Item *item, int dir=FL_Down, bool visible=false);
#if FLTK_ABI_VERSION >= 10303
  Fl_Tree_Item *next_selected_item(Fl_Tree_Item *item=0, int dir=FL_Down);
  int get_selected_items(Fl_Tree_Item_Array &items);
#else
  Fl_Tree_Item *next_selected_item(Fl_Tree_Item *item=0);
  Fl_Tree_Item *next_selected_item(Fl_Tree_Item *item, int dir);
#endif

  //////////////////////////
  // Item open/close methods
  //////////////////////////
  int open(Fl_Tree_Item *item, int docallback=1);
  int open(const char *path, int docallback=1);
  void open_toggle(Fl_Tree_Item *item, int docallback=1);
  int close(Fl_Tree_Item *item, int docallback=1);
  int close(const char *path, int docallback=1);
  int is_open(Fl_Tree_Item *item) const;
  int is_open(const char *path) const;
  int is_close(Fl_Tree_Item *item) const;
  int is_close(const char *path) const;

  /////////////////////////
  // Item selection methods
  /////////////////////////
  int select(Fl_Tree_Item *item, int docallback=1);
  int select(const char *path, int docallback=1);
  void select_toggle(Fl_Tree_Item *item, int docallback=1);
  int deselect(Fl_Tree_Item *item, int docallback=1);
  int deselect(const char *path, int docallback=1);
  int deselect_all(Fl_Tree_Item *item=0, int docallback=1);
  int select_only(Fl_Tree_Item *selitem, int docallback=1);
  int select_all(Fl_Tree_Item *item=0, int docallback=1);
  int extend_selection_dir(Fl_Tree_Item *from,
                           Fl_Tree_Item *to,
			   int dir,
			   int val,
			   bool visible);
#if FLTK_ABI_VERSION >= 10303
  int extend_selection(Fl_Tree_Item *from,
  		       Fl_Tree_Item *to,
		       int val=1,
		       bool visible=false);
#else
private:
  // Adding overload if not at least one overload breaks ABI, so avoid
  // by keeping private until we can break ABI. ref: http://www.ros.org/reps/rep-0009.html
  int extend_selection__(Fl_Tree_Item *from,
  			 Fl_Tree_Item *to,
			 int val,
			 bool visible);
public:
#endif
  void set_item_focus(Fl_Tree_Item *item);
  Fl_Tree_Item *get_item_focus() const;
  int is_selected(Fl_Tree_Item *item) const;
  int is_selected(const char *path);

  /////////////////////////////////
  // Item attribute related methods
  /////////////////////////////////
  Fl_Font     item_labelfont() const;
  void        item_labelfont(Fl_Font val);
  Fl_Fontsize item_labelsize() const;
  void        item_labelsize(Fl_Fontsize val);
  Fl_Color    item_labelfgcolor(void) const;
  void        item_labelfgcolor(Fl_Color val);
  Fl_Color    item_labelbgcolor(void) const;
  void        item_labelbgcolor(Fl_Color val);
  Fl_Color connectorcolor() const;
  void connectorcolor(Fl_Color val);
  int marginleft() const;
  void marginleft(int val);
  int margintop() const;
  void margintop(int val);
#if FLTK_ABI_VERSION >= 10301
  int marginbottom() const;
  void marginbottom(int val);
#endif /*FLTK_ABI_VERSION*/
  int linespacing() const;
  void linespacing(int val);
  int openchild_marginbottom() const;
  void openchild_marginbottom(int val);
  int usericonmarginleft() const;
  void usericonmarginleft(int val);
  int labelmarginleft() const;
  void labelmarginleft(int val);
#if FLTK_ABI_VERSION >= 10301
  int widgetmarginleft() const;
  void widgetmarginleft(int val);
#endif /*FLTK_ABI_VERSION*/
  int connectorwidth() const;
  void connectorwidth(int val);
  Fl_Image* usericon() const;
  void usericon(Fl_Image *val);
  Fl_Image* openicon() const;
  void openicon(Fl_Image *val);
  Fl_Image* closeicon() const;
  void closeicon(Fl_Image *val);
  int showcollapse() const;
  void showcollapse(int val);
  int showroot() const;
  void showroot(int val);
  Fl_Tree_Connector connectorstyle() const;
  void connectorstyle(Fl_Tree_Connector val);
  Fl_Tree_Sort sortorder() const;
  void sortorder(Fl_Tree_Sort val);
  Fl_Boxtype selectbox() const;
  void selectbox(Fl_Boxtype val);
  Fl_Tree_Select selectmode() const;
  void selectmode(Fl_Tree_Select val);
#if FLTK_ABI_VERSION >= 10301
  Fl_Tree_Item_Reselect_Mode item_reselect_mode() const;
  void item_reselect_mode(Fl_Tree_Item_Reselect_Mode mode);
  Fl_Tree_Item_Draw_Mode item_draw_mode() const;
  void item_draw_mode(Fl_Tree_Item_Draw_Mode mode);
  void item_draw_mode(int mode);
#endif
#if FLTK_ABI_VERSION >= 10303
  void calc_dimensions();
  void calc_tree();
#endif
  void recalc_tree();
  int displayed(Fl_Tree_Item *item);
  void show_item(Fl_Tree_Item *item, int yoff);
  void show_item(Fl_Tree_Item *item);
  void show_item_top(Fl_Tree_Item *item);
  void show_item_middle(Fl_Tree_Item *item);
  void show_item_bottom(Fl_Tree_Item *item);
  void display(Fl_Tree_Item *item);
  int  vposition() const;
  void vposition(int pos);
  int  hposition() const;
  void hposition(int pos);

  int is_scrollbar(Fl_Widget *w);
  int scrollbar_size() const;
  void scrollbar_size(int size);
  int is_vscroll_visible() const;
  int is_hscroll_visible() const;

  ///////////////////////
  // callback related
  ///////////////////////
  void callback_item(Fl_Tree_Item* item);
  Fl_Tree_Item* callback_item();
  void callback_reason(Fl_Tree_Reason reason);
  Fl_Tree_Reason callback_reason() const;

  /// Load FLTK preferences
  void load(class Fl_Preferences&);
};

#endif /*FL_TREE_H*/

//
// End of "$Id: Fl_Tree.H 10275 2014-09-05 12:04:28Z cand $".
//
