/* This file is part of gPHPEdit, a GNOME2 PHP Editor.

   Copyright (C) 2003, 2004, 2005 Andy Jeffries <andy at gphpedit.org>
   Copyright (C) 2009 Anoop John <anoop dot john at zyxware.com>
   Copyright (C) 2009, 2010 José Rostagno (for vijona.com.ar) 

   For more information or to find the latest release, visit our 
   website at http://www.gphpedit.org/

   gPHPEdit is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   gPHPEdit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with gPHPEdit. If not, see <http://www.gnu.org/licenses/>.

   The GNU General Public License is contained in the file COPYING.
*/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "main_window.h"
#include "main_window_callbacks.h"
#include "pluginmenu.h"
#include <gdk/gdkkeysyms.h>

#include "menubar.h"

/*max reopen files shown */
#define NUM_REOPEN_MAX 20


#define MENUBAR_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), \
						GOBJECT_TYPE_MENUBAR,              \
						MenuBarPrivate))

struct _MenuBarPrivate 
{
  MainWindow *main_window;

  GtkAccelGroup *accel_group;
  GtkWidget *menubar;
  GtkWidget *menunew;
  GtkWidget *menuedit;
  GtkWidget *menuview;
  GtkWidget *menucode;
  GtkWidget *newi;
  GtkWidget *open;
  GtkWidget *menureciente;
  GtkWidget *opensel;
  GtkWidget *reload;
  GtkWidget *save;
  GtkWidget *saveas;
  GtkWidget *saveall;
  GtkWidget *close;
  GtkWidget *quit;

  GtkWidget *undo;
  GtkWidget *redo;
  GtkWidget *copy;
  GtkWidget *cut;
  GtkWidget *paste;
  GtkWidget *selectall;
  GtkWidget *find;
  GtkWidget *replace;
  GtkWidget *incfind;
  GtkWidget *gotoline;
  GtkWidget *indent;
  GtkWidget *unindent;
  GtkWidget *upper;
  GtkWidget *lower;

  GtkWidget *viewmaintoolbar;
  GtkWidget *viewstatusbar;
  GtkWidget *viewfullscreen;
  GtkWidget *tog_class;
  GtkWidget *zoomin;
  GtkWidget *zoomout;
  GtkWidget *zoom100;
  GtkWidget *preview;
  GtkWidget *preferences;

  GtkWidget *code;
  GtkWidget *syntax;
  GtkWidget *clearsyntax;
  GtkWidget *sep5;
  GtkWidget *record;
  GtkWidget *playback;
  GtkWidget *force;
  GtkWidget *menuforce;
  GtkWidget *forcephp;
  GtkWidget *forcecss;
  GtkWidget *forcecxx;
  GtkWidget *forcesql;
  GtkWidget *forceperl;
  GtkWidget *forcecobol;
  GtkWidget *forcepython;

  GtkWidget *plugin;
  GtkWidget *menuplugin;

  GtkWidget *menuhelp;
  GtkWidget *phphelp;
  #ifdef PACKAGE_BUGREPORT
  GtkWidget *bugreport;
  #endif
  #ifdef TRANSLATE_URL
  GtkWidget *translate;
  #endif
  GtkWidget *abouthelp;

};

static void MENUBAR_constructed (GObject *menubar);

G_DEFINE_TYPE(MenuBar, MENUBAR, GTK_TYPE_MENU_BAR)

enum
{
  PROP_0,
  PROP_MAIN_WINDOW
};

static void
MENUBAR_set_property (GObject      *object,
			      guint         prop_id,
			      const GValue *value,
			      GParamSpec   *pspec)
{
  MenuBarPrivate *priv = MENUBAR_GET_PRIVATE(object);

  switch (prop_id)
  {
    case PROP_MAIN_WINDOW:
        priv->main_window = g_value_get_pointer(value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
MENUBAR_get_property (GObject    *object,
			      guint       prop_id,
			      GValue     *value,
			      GParamSpec *pspec)
{
  MenuBarPrivate *priv = MENUBAR_GET_PRIVATE(object);
  
  switch (prop_id)
  {
    case PROP_MAIN_WINDOW:
      g_value_set_pointer (value, priv->main_window);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
MENUBAR_class_init (MenuBarClass *klass)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = MENUBAR_set_property;
    object_class->get_property = MENUBAR_get_property;
    object_class->constructed = MENUBAR_constructed;

    g_object_class_install_property (object_class,
                              PROP_MAIN_WINDOW,
                              g_param_spec_pointer ("main_window",
                              NULL, NULL,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_type_class_add_private (klass, sizeof (MenuBarPrivate));
}

static void syntax_check(GtkWidget *widget, gpointer user_data)
{
    syntax_check_show((MainWindow *)user_data);
}

static void syntax_check_clear(GtkWidget *widget, gpointer user_data)
{
    syntax_check_hide((MainWindow *)user_data);
}

/*
 * tog_classbrowser
 * Show/hide side panel
*/
static void tog_classbrowser(GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
  side_panel_show_hide((MainWindow *) user_data);
}
/*
 * tog_statusbar
 * Show/hide application statusbar
*/
static void tog_statusbar(GtkWidget *widget, gpointer user_data)
{
    MainWindow *main_window = (MainWindow *) user_data;
    gboolean state = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
    statusbar_show_hide(main_window, state);
}

/*
 * tog_maintoolbar
 * Show/hide application maintoolbar
*/

static void tog_maintoolbar(GtkWidget *widget, gpointer user_data)
{
    MainWindow *main_window = (MainWindow *) user_data;
    gboolean state = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
    maintoolbar_show_hide(main_window, state);
}

/*
 * tog_fullscreen
 * Enable/disable fullscreen mode
*/
static void tog_fullscreen(GtkWidget *widget, gpointer user_data)
{
    MainWindow *main_window = (MainWindow *) user_data;
    gboolean state = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
    fullscreen_show_hide(main_window, state);
}

/*
 * ShowPreview
 * Preview Current Document
*/
static void showpreview (GtkWidget *widget, gpointer user_data)
{
    MainWindow *main_window = (MainWindow *) user_data;
    document_manager_get_document_preview(main_window->docmg);
}

#ifdef PACKAGE_BUGREPORT
/*
 * bugreport
 * launch default system browser with bug report page
*/
static void bugreport(GtkWidget *widget, gpointer user_data){
    MainWindow *main_window = (MainWindow *) user_data;
    GdkScreen *screen;
    screen = gtk_widget_get_screen (GTK_WIDGET (main_window->window));
    gtk_show_uri (screen, PACKAGE_BUGREPORT, GDK_CURRENT_TIME, NULL);
}
#endif
#ifdef TRANSLATE_URL
/*
 * translate
 * launch default system browser with tranlation page
*/
static void translate(GtkWidget *widget, gpointer user_data){
    MainWindow *main_window = (MainWindow *) user_data;
    GdkScreen *screen;
    screen = gtk_widget_get_screen (GTK_WIDGET (main_window->window));
    gtk_show_uri (screen, TRANSLATE_URL, GDK_CURRENT_TIME, NULL);
}
#endif

static void on_incfind_activate (GtkWidget *widget, gpointer user_data)
{
    MainWindow *main_window = (MainWindow *) user_data;
    Documentable *document = document_manager_get_current_documentable(main_window->docmg);
    documentable_activate_incremental_search (document);
}

static void on_gotoline_activate (GtkWidget *widget, gpointer user_data)
{
    MainWindow *main_window = (MainWindow *) user_data;
    Documentable *document = document_manager_get_current_documentable(main_window->docmg);
    if (OBJECT_IS_DOCUMENT_SCINTILLA(document)) {
        document_scintilla_activate_goto_line(DOCUMENT_SCINTILLA(document));
    }
}

/*
* create_menu
* create a new menu and insert it in menubar
*/
static inline void _create_menu(GtkWidget **menu, const gchar *caption, GtkWidget *menubar){
  *menu = gtk_menu_new();
  GtkWidget *menuitem = gtk_menu_item_new_with_mnemonic(caption);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), *menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);
}

/*
* _create_separator_item
* creates a new separator item, add it to menu and return the separator item
*/
static inline void _create_separator_item(GtkWidget *menu)
{
  GtkWidget *separator = gtk_separator_menu_item_new();
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), separator);
}

/*
 * create_stock_menu_item
 * creates a new stock menu item, append it to menu, add menu hint, optionally add accelerator and return the new menuitem
*/
static inline void create_stock_menu_item(GtkWidget **menuitem,GtkWidget *menu,const gchar *stock_id, GtkAccelGroup *accel_group, guint accel_key, GdkModifierType accel_mods){
  *menuitem = gtk_image_menu_item_new_from_stock(stock_id, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), *menuitem);
  if (!(accel_key==0 && accel_mods==0)){
  gtk_widget_add_accelerator(*menuitem, "activate", accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);
  }
}
/*
 * create_mnemonic_menu_item
 * creates a new mnemonic menu item, append it to menu, add menu hint, optionally add accelerator and return the new menuitem
*/
static inline void create_mnemonic_menu_item(GtkWidget **menuitem,GtkWidget *menu,gchar *mnemonic, GtkAccelGroup *accel_group, guint accel_key, GdkModifierType accel_mods){
  *menuitem = gtk_menu_item_new_with_mnemonic(mnemonic);
  if (!(accel_key==0 && accel_mods==0))
    gtk_widget_add_accelerator(*menuitem, "activate", accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), *menuitem);
}
/*
 * create_mnemonic_menu_item
 * creates a check menu item, append it to menu, add menu hint, optionally add accelerator, set default state and return the new menuitem
*/
static inline void create_check_menu_item(GtkWidget **menuitem,GtkWidget *menu,gchar *mnemonic, GtkAccelGroup *accel_group, guint accel_key, GdkModifierType accel_mods){
  *menuitem = gtk_check_menu_item_new_with_label(mnemonic);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(*menuitem), FALSE);
  if (!(accel_key==0 && accel_mods==0))
    gtk_widget_add_accelerator(*menuitem, "activate", accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu),*menuitem);
}

/*
* create_file_menu
* create file menu widgets and fill file menu
*/
static void fill_menu_file(MenuBarPrivate *priv){
    create_stock_menu_item(&priv->newi, priv->menunew, GTK_STOCK_NEW, priv->accel_group, GDK_KEY_n, GDK_CONTROL_MASK);
    create_stock_menu_item(&priv->open,priv->menunew,GTK_STOCK_OPEN, priv->accel_group, GDK_KEY_o, GDK_CONTROL_MASK);
    create_mnemonic_menu_item(&priv->opensel ,priv->menunew,_("_Open selected file"), priv->accel_group,GDK_KEY_o, GDK_CONTROL_MASK);
    /* recent menu setup */
    GtkWidget *reciente = gtk_menu_item_new_with_mnemonic(_("_Recent Files"));
    gtk_container_add (GTK_CONTAINER (priv->menunew), reciente);
    priv->menureciente =  gtk_recent_chooser_menu_new (); /* create recent menu for default recent manager*/
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (reciente), priv->menureciente);
    gtk_recent_chooser_set_limit (GTK_RECENT_CHOOSER(priv->menureciente), NUM_REOPEN_MAX); /* set max files in menu */
    gtk_recent_chooser_set_sort_type (GTK_RECENT_CHOOSER(priv->menureciente),GTK_RECENT_SORT_MRU); /* Most recent first*/
    GtkRecentFilter *filter = gtk_recent_filter_new ();
    gtk_recent_filter_add_application (filter, "gPHPEdit"); /* only show our files */
    gtk_recent_chooser_add_filter (GTK_RECENT_CHOOSER(priv->menureciente), filter);

    create_stock_menu_item(&priv->reload, priv->menunew, GTK_STOCK_REVERT_TO_SAVED, priv->accel_group, GDK_KEY_r, GDK_SHIFT_MASK | GDK_CONTROL_MASK);
  /* separator */
  _create_separator_item(priv->menunew);

    create_stock_menu_item(&priv->save,priv->menunew, GTK_STOCK_SAVE, priv->accel_group, GDK_KEY_s, GDK_CONTROL_MASK);
    create_stock_menu_item(&priv->saveas,priv->menunew,GTK_STOCK_SAVE_AS, priv->accel_group, GDK_KEY_s, GDK_SHIFT_MASK | GDK_CONTROL_MASK);
    create_mnemonic_menu_item(&priv->saveall ,priv->menunew,_("Save A_ll"), priv->accel_group, GDK_KEY_a, GDK_SHIFT_MASK | GDK_MOD1_MASK);
    create_stock_menu_item(&priv->close,priv->menunew,GTK_STOCK_CLOSE, priv->accel_group, GDK_KEY_w, GDK_CONTROL_MASK);
    /* separator */
    _create_separator_item(priv->menunew);
    create_stock_menu_item(&priv->quit,priv->menunew, GTK_STOCK_QUIT, priv->accel_group, GDK_KEY_q, GDK_CONTROL_MASK);
}

/*
* create_file_menu
* create file menu widgets and fill file menu
*/
static void prepare_menu_file(MenuBarPrivate *priv){

    main_window_install_menu_hint(priv->main_window, priv->newi, _("Creates a new file"));
    main_window_install_menu_hint(priv->main_window, priv->open, _("Open a file"));
    main_window_install_menu_hint(priv->main_window, priv->opensel, _("Open a file with the name currently selected in the editor"));
    main_window_install_menu_hint(priv->main_window, priv->reload, _("Save the file currently selected in the editor"));

    main_window_install_menu_hint(priv->main_window, priv->save, _("Save the file currently selected in the editor"));
    main_window_install_menu_hint(priv->main_window, priv->saveas, _("Save the file currently selected in the editor"));
    main_window_install_menu_hint(priv->main_window, priv->saveall, _("Save all open unsaved files"));
    main_window_install_menu_hint(priv->main_window, priv->close, _("Close the current file"));
    main_window_install_menu_hint(priv->main_window, priv->quit, _("Quit the application"));

    g_signal_connect(G_OBJECT(priv->newi), "activate", G_CALLBACK(on_new1_activate), priv->main_window);
    g_signal_connect(G_OBJECT(priv->open), "activate", G_CALLBACK(on_open1_activate), priv->main_window);
    g_signal_connect(G_OBJECT(priv->opensel), "activate", G_CALLBACK(on_openselected1_activate), priv->main_window);
    g_signal_connect(G_OBJECT(priv->menureciente), "item-activated", G_CALLBACK(reopen_recent), priv->main_window);
    g_signal_connect(G_OBJECT(priv->reload), "activate", G_CALLBACK(on_reload1_activate), priv->main_window);

    g_signal_connect(G_OBJECT(priv->save), "activate", G_CALLBACK(on_save1_activate), priv->main_window);
    g_signal_connect(G_OBJECT(priv->saveas), "activate", G_CALLBACK(on_save_as1_activate), priv->main_window);
    g_signal_connect(G_OBJECT(priv->saveall), "activate", G_CALLBACK(on_saveall1_activate), priv->main_window);
    g_signal_connect(G_OBJECT(priv->close), "activate", G_CALLBACK(on_close1_activate), priv->main_window);
    g_signal_connect(G_OBJECT(priv->quit), "activate", G_CALLBACK(on_quit1_activate), priv->main_window);
}

/*
* create_help_menu
* create help menu widgets and fill help menu
*/
static void fill_help_menu(MenuBarPrivate *priv){
  /* help menu */
    create_stock_menu_item(&priv->phphelp,priv->menuhelp, GTK_STOCK_HELP,
                        priv->accel_group, GDK_KEY_F1, 0);
    /* set custom label */
    gtk_menu_item_set_label (GTK_MENU_ITEM(priv->phphelp), _("_PHP Help"));
  #ifdef PACKAGE_BUGREPORT
    create_mnemonic_menu_item(&priv->bugreport,priv->menuhelp,
                            _("_Report a bug in gPHPEdit"), priv->accel_group, 0, 0);
  #endif
  #ifdef TRANSLATE_URL
  create_mnemonic_menu_item(&priv->translate,priv->menuhelp,_("_Translate this application"),
                            priv->accel_group, 0, 0);
  #endif
    create_stock_menu_item(&priv->abouthelp,priv->menuhelp,GTK_STOCK_ABOUT,
                        priv->accel_group, 0, 0);
}

static void prepare_help_menu(MenuBarPrivate *priv){
    main_window_install_menu_hint(priv->main_window, priv->phphelp, _("Look for help on the currently selected function"));
    g_signal_connect(G_OBJECT(priv->phphelp), "activate", G_CALLBACK(context_help), priv->main_window);
    #ifdef PACKAGE_BUGREPORT
    g_signal_connect(G_OBJECT(priv->bugreport), "activate", G_CALLBACK(bugreport), priv->main_window);
    main_window_install_menu_hint(priv->main_window, priv->bugreport, _("Go to bug report page to report a bug"));
    #endif
    #ifdef TRANSLATE_URL
    g_signal_connect(G_OBJECT(priv->translate), "activate", G_CALLBACK(translate), priv->main_window);
    main_window_install_menu_hint(priv->main_window, priv->translate, _("Start translating this application"));
    #endif
    g_signal_connect(G_OBJECT(priv->abouthelp), "activate", G_CALLBACK(on_about1_activate), priv->main_window);
    main_window_install_menu_hint(priv->main_window, priv->abouthelp, _("Shows info about gPHPEdit"));
}

/*
* create_edit_menu
* create edit menu widgets and fill edit menu
*/
static void fill_menu_edit(MenuBarPrivate *priv)
{
    create_stock_menu_item(&priv->undo,priv->menuedit,GTK_STOCK_UNDO, priv->accel_group, GDK_KEY_z, GDK_CONTROL_MASK);
    create_stock_menu_item(&priv->redo,priv->menuedit,GTK_STOCK_REDO, priv->accel_group, GDK_KEY_z, GDK_SHIFT_MASK | GDK_CONTROL_MASK);
    /* separator */
    _create_separator_item(priv->menuedit);

    create_stock_menu_item(&priv->cut,priv->menuedit,GTK_STOCK_CUT, priv->accel_group, GDK_KEY_x, GDK_CONTROL_MASK);
    create_stock_menu_item(&priv->copy,priv->menuedit,GTK_STOCK_COPY, priv->accel_group, GDK_KEY_c, GDK_CONTROL_MASK);
    create_stock_menu_item(&priv->paste,priv->menuedit,GTK_STOCK_PASTE, priv->accel_group, GDK_KEY_v, GDK_CONTROL_MASK);
    create_stock_menu_item(&priv->selectall,priv->menuedit,GTK_STOCK_SELECT_ALL, priv->accel_group, GDK_KEY_a, GDK_CONTROL_MASK);
    /* separator */
    _create_separator_item(priv->menuedit);

    create_stock_menu_item(&priv->find,priv->menuedit,GTK_STOCK_FIND, priv->accel_group, GDK_KEY_f, GDK_CONTROL_MASK);
    create_stock_menu_item(&priv->replace,priv->menuedit,GTK_STOCK_FIND_AND_REPLACE, priv->accel_group, GDK_KEY_h, GDK_CONTROL_MASK);
    create_mnemonic_menu_item(&priv->incfind,priv->menuedit,_("Incremental search"), priv->accel_group, GDK_KEY_i, GDK_CONTROL_MASK);
    create_mnemonic_menu_item(&priv->gotoline,priv->menuedit,_("Go to line"), priv->accel_group, GDK_KEY_g, GDK_CONTROL_MASK);
    /* separator */
    _create_separator_item(priv->menuedit);

    create_stock_menu_item(&priv->indent,priv->menuedit,GTK_STOCK_INDENT, priv->accel_group, GDK_KEY_i, GDK_SHIFT_MASK | GDK_MOD1_MASK);
    create_stock_menu_item(&priv->unindent,priv->menuedit,GTK_STOCK_UNINDENT, priv->accel_group, GDK_KEY_i,  GDK_SHIFT_MASK | GDK_CONTROL_MASK |GDK_MOD1_MASK);
    /* separator */
    _create_separator_item(priv->menuedit);

    create_mnemonic_menu_item(&priv->upper ,priv->menuedit,_("_ToUpper"), priv->accel_group, GDK_KEY_u, GDK_CONTROL_MASK);
    create_mnemonic_menu_item(&priv->lower,priv->menuedit,_("_ToLower"),priv->accel_group, GDK_KEY_l, GDK_CONTROL_MASK);
    /* separator */
    _create_separator_item(priv->menuedit);

    create_stock_menu_item(&priv->preferences, priv->menuedit, GTK_STOCK_PREFERENCES, priv->accel_group, GDK_KEY_F5, 0);
}

static void prepare_menu_edit(MenuBarPrivate *priv)
{
    main_window_install_menu_hint(priv->main_window, priv->undo, _("Undo last action"));
    main_window_install_menu_hint(priv->main_window, priv->redo, _("Redo last action"));
    main_window_install_menu_hint(priv->main_window, priv->cut, _("Cut Selected Text"));
    main_window_install_menu_hint(priv->main_window, priv->copy, _("Copy Selected Text"));
    main_window_install_menu_hint(priv->main_window, priv->paste, _("Paste Text from clipboard"));
    main_window_install_menu_hint(priv->main_window, priv->selectall, _("Select all Text in current file"));
    main_window_install_menu_hint(priv->main_window, priv->find, _("Find text in current file"));
    main_window_install_menu_hint(priv->main_window, priv->replace, _("Find and replace text in current file"));
    main_window_install_menu_hint(priv->main_window, priv->incfind, _("Search as you type"));
    main_window_install_menu_hint(priv->main_window, priv->gotoline, _("Go to line"));
    main_window_install_menu_hint(priv->main_window, priv->indent, _("Indent the currently selected block"));
    main_window_install_menu_hint(priv->main_window, priv->unindent, _("Unindent the currently selected block"));
    main_window_install_menu_hint(priv->main_window, priv->upper, _("Convert the current selection text to upper case"));
    main_window_install_menu_hint(priv->main_window, priv->lower, _("Convert the current selection text to lower case"));
    main_window_install_menu_hint(priv->main_window, priv->preferences, _("Application Config"));
    g_signal_connect(G_OBJECT(priv->undo), "activate", G_CALLBACK(on_undo1_activate), priv->main_window);
    g_signal_connect(G_OBJECT(priv->redo), "activate", G_CALLBACK(on_redo1_activate), priv->main_window);

    g_signal_connect(G_OBJECT(priv->cut), "activate", G_CALLBACK(on_cut1_activate), priv->main_window);
    g_signal_connect(G_OBJECT(priv->copy), "activate", G_CALLBACK(on_copy1_activate), priv->main_window);
    g_signal_connect(G_OBJECT(priv->paste), "activate", G_CALLBACK(on_paste1_activate), priv->main_window);
    g_signal_connect(G_OBJECT(priv->selectall), "activate", G_CALLBACK(on_selectall1_activate), priv->main_window);

    g_signal_connect(G_OBJECT(priv->find), "activate", G_CALLBACK(on_find1_activate), priv->main_window);
    g_signal_connect(G_OBJECT(priv->replace), "activate", G_CALLBACK(on_replace1_activate), priv->main_window);

    g_signal_connect(G_OBJECT(priv->incfind), "activate", G_CALLBACK(on_incfind_activate), priv->main_window);
    g_signal_connect(G_OBJECT(priv->gotoline), "activate", G_CALLBACK(on_gotoline_activate), priv->main_window);
  
    g_signal_connect(G_OBJECT(priv->indent), "activate", G_CALLBACK(block_indent), priv->main_window);
    g_signal_connect(G_OBJECT(priv->unindent), "activate", G_CALLBACK(block_unindent), priv->main_window);

    g_signal_connect(G_OBJECT(priv->upper), "activate", G_CALLBACK(selectiontoupper), priv->main_window);
    g_signal_connect(G_OBJECT(priv->lower), "activate", G_CALLBACK(selectiontolower), priv->main_window);
    g_signal_connect(G_OBJECT(priv->preferences), "activate", G_CALLBACK(on_preferences1_activate), priv->main_window);
}

/*
* create_view_menu
* create view menu widgets and fill view menu
*/
static void fill_menu_view(MenuBarPrivate *priv)
{
    create_check_menu_item(&priv->viewstatusbar,priv->menuview,_("Statusbar"), priv->accel_group, 0, 0);
    create_check_menu_item(&priv->viewmaintoolbar,priv->menuview,_("Main Toolbar"), priv->accel_group, 0, 0);
    /* separator */
    _create_separator_item(priv->menuview);

    create_check_menu_item(&priv->tog_class,priv->menuview,_("Show Side Panel"), priv->accel_group, GDK_KEY_F8, 0);
    create_check_menu_item(&priv->viewfullscreen,priv->menuview,_("Fullscreen"), priv->accel_group, GDK_KEY_F11, 0);
    /* separator */
    _create_separator_item(priv->menuview);

    create_stock_menu_item(&priv->zoomin,priv->menuview,GTK_STOCK_ZOOM_IN, priv->accel_group, GDK_KEY_plus, GDK_CONTROL_MASK);
    create_stock_menu_item(&priv->zoomout,priv->menuview,GTK_STOCK_ZOOM_OUT, priv->accel_group, GDK_KEY_minus, GDK_CONTROL_MASK);
    create_stock_menu_item(&priv->zoom100,priv->menuview,GTK_STOCK_ZOOM_100, priv->accel_group, GDK_KEY_0, GDK_CONTROL_MASK);
    /* separator */
    _create_separator_item(priv->menuview);
    create_mnemonic_menu_item(&priv->preview ,priv->menuview,_("_Show Preview"), priv->accel_group, 0, 0);
}

static void prepare_menu_view(MenuBarPrivate *priv)
{
    main_window_install_menu_hint(priv->main_window, priv->viewstatusbar, _("Show/Hide Application Statusbar"));
    main_window_install_menu_hint(priv->main_window, priv->viewmaintoolbar, _("Show/Hide Application Main Toolbar"));
    main_window_install_menu_hint(priv->main_window, priv->tog_class, _("Show/Hide Application Side Panel"));
    main_window_install_menu_hint(priv->main_window, priv->viewfullscreen, _("Enable/Disable Fullscreen mode"));
    main_window_install_menu_hint(priv->main_window, priv->zoomin, _("Increases zoom in 10%"));
    main_window_install_menu_hint(priv->main_window, priv->zoomout, _("Decreases zoom in 10%"));
    main_window_install_menu_hint(priv->main_window, priv->zoom100, _("Restores normal zoom level"));
    main_window_install_menu_hint(priv->main_window, priv->preview, _("Preview the Document"));

    gboolean showstatus = get_preferences_manager_show_statusbar(priv->main_window->prefmg);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(priv->viewstatusbar), showstatus);
    g_signal_connect(G_OBJECT(priv->viewstatusbar), "activate", G_CALLBACK(tog_statusbar), priv->main_window);
    gboolean showmainbar = get_preferences_manager_show_maintoolbar(priv->main_window->prefmg);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(priv->viewmaintoolbar), showmainbar);
    g_signal_connect(G_OBJECT(priv->viewmaintoolbar), "activate", G_CALLBACK(tog_maintoolbar), priv->main_window);

    gboolean status;
    g_object_get(priv->main_window->prefmg, "side_panel_hidden", &status, NULL);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(priv->tog_class), status);
    g_signal_connect(G_OBJECT(priv->tog_class), "activate", G_CALLBACK(tog_classbrowser), priv->main_window);

    g_signal_connect(G_OBJECT(priv->viewfullscreen), "activate", G_CALLBACK(tog_fullscreen), priv->main_window);
    g_signal_connect(G_OBJECT(priv->zoomin), "activate", G_CALLBACK(zoom_in), priv->main_window);
    g_signal_connect(G_OBJECT(priv->zoomout), "activate", G_CALLBACK(zoom_out), priv->main_window);
    g_signal_connect(G_OBJECT(priv->zoom100), "activate", G_CALLBACK(zoom_100), priv->main_window);
    g_signal_connect(G_OBJECT(priv->preview), "activate", G_CALLBACK(showpreview), priv->main_window);
}

/*
* create_code_menu
* create code menu widgets and fill code menu
*/
static void fill_menu_code (MenuBarPrivate *priv)
{
  create_stock_menu_item(&priv->syntax,priv->menucode, GTK_STOCK_SPELL_CHECK, priv->accel_group, GDK_KEY_F9, 0);
  /* set custom label */
  gtk_menu_item_set_label (GTK_MENU_ITEM(priv->syntax), _("_Syntax check"));

  create_stock_menu_item(&priv->clearsyntax,priv->menucode, GTK_STOCK_CLEAR, priv->accel_group, GDK_KEY_F9, GDK_CONTROL_MASK);
  /* set custom label */
  gtk_menu_item_set_label (GTK_MENU_ITEM(priv->clearsyntax), _("_Clear Syntax check"));
  /* separator */
  _create_separator_item(priv->menucode);

  create_stock_menu_item(&priv->record,priv->menucode, GTK_STOCK_MEDIA_RECORD, priv->accel_group, GDK_KEY_k, GDK_MOD1_MASK);
  /* set custom label */
  gtk_menu_item_set_label (GTK_MENU_ITEM(priv->record), _("_Record keyboard macro start/stop"));

  create_stock_menu_item(&priv->playback,priv->menucode, GTK_STOCK_MEDIA_PLAY, priv->accel_group, GDK_KEY_k, GDK_CONTROL_MASK);
  /* set custom label */
  gtk_menu_item_set_label (GTK_MENU_ITEM(priv->playback), _("_Playback keyboard macro"));

  priv->force = gtk_menu_item_new_with_mnemonic(_("_Force"));
  gtk_container_add (GTK_CONTAINER (priv->menucode), priv->force);
  priv->menuforce = gtk_menu_new();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (priv->force), priv->menuforce);

  create_mnemonic_menu_item(&priv->forcephp,priv->menuforce,_("_PHP/HTML/XML"), priv->accel_group, 0, 0);
  create_mnemonic_menu_item(&priv->forcecss,priv->menuforce,_("_CSS"), priv->accel_group, 0, 0);
  create_mnemonic_menu_item(&priv->forcecxx,priv->menuforce,_("C/C_++"), priv->accel_group, 0, 0);
  create_mnemonic_menu_item(&priv->forcesql,priv->menuforce,_("_SQL"), priv->accel_group, 0, 0);
  create_mnemonic_menu_item(&priv->forceperl,priv->menuforce,_("_Perl"), priv->accel_group, 0, 0);
  create_mnemonic_menu_item(&priv->forcecobol,priv->menuforce,_("_Cobol"), priv->accel_group, 0, 0);
  create_mnemonic_menu_item(&priv->forcepython,priv->menuforce,_("P_ython"), priv->accel_group, 0, 0);
}

static void prepare_menu_code (MenuBarPrivate *priv)
{
    main_window_install_menu_hint(priv->main_window, priv->syntax, _("Check the syntax using the PHP command line binary"));
    main_window_install_menu_hint(priv->main_window, priv->clearsyntax, _("Remove the syntax check window"));
    main_window_install_menu_hint(priv->main_window, priv->record, _("Record keyboard actions"));
    main_window_install_menu_hint(priv->main_window, priv->playback, _("Playback the stored keyboard macro"));
    main_window_install_menu_hint(priv->main_window, priv->forcephp, _("Force syntax highlighting to PHP/HTML/XML mode"));
    main_window_install_menu_hint(priv->main_window, priv->forcecss, _("Force syntax highlighting to CSS mode"));
    main_window_install_menu_hint(priv->main_window, priv->forcecxx, _("Force syntax highlighting to C/C++ mode"));
    main_window_install_menu_hint(priv->main_window, priv->forcesql, _("Force syntax highlighting to SQL mode"));
    main_window_install_menu_hint(priv->main_window, priv->forceperl, _("Force syntax highlighting to Perl mode"));
    main_window_install_menu_hint(priv->main_window, priv->forcecobol, _("Force syntax highlighting to Cobol mode"));
    main_window_install_menu_hint(priv->main_window, priv->forcepython, _("Force syntax highlighting to Python mode"));

    g_signal_connect(G_OBJECT(priv->syntax), "activate", G_CALLBACK(syntax_check), priv->main_window);
    g_signal_connect(G_OBJECT(priv->clearsyntax), "activate", G_CALLBACK(syntax_check_clear), priv->main_window);
    g_signal_connect(G_OBJECT(priv->record), "activate", G_CALLBACK(keyboard_macro_startstop), priv->main_window);
    g_signal_connect(G_OBJECT(priv->playback), "activate", G_CALLBACK(keyboard_macro_playback), priv->main_window);

    g_signal_connect(G_OBJECT(priv->forcephp), "activate", G_CALLBACK(force_php), priv->main_window);
    g_signal_connect(G_OBJECT(priv->forcecss), "activate", G_CALLBACK(force_css), priv->main_window);
    g_signal_connect(G_OBJECT(priv->forcecxx), "activate", G_CALLBACK(force_cxx), priv->main_window);
    g_signal_connect(G_OBJECT(priv->forcesql), "activate", G_CALLBACK(force_sql), priv->main_window);
    g_signal_connect(G_OBJECT(priv->forceperl), "activate", G_CALLBACK(force_perl), priv->main_window);
    g_signal_connect(G_OBJECT(priv->forcecobol), "activate", G_CALLBACK(force_cobol), priv->main_window);
    g_signal_connect(G_OBJECT(priv->forcepython), "activate", G_CALLBACK(force_python), priv->main_window);
}

static void
MENUBAR_init (MenuBar *menubar)
{
  MenuBarPrivate *priv = MENUBAR_GET_PRIVATE(menubar);
  /* create menu accel group */
  priv->accel_group = gtk_accel_group_new();
  /*create menus*/
  /*Menu file*/
  _create_menu(&priv->menunew, _("_File"), GTK_WIDGET(menubar));
  fill_menu_file(priv);
  /*Menu edit*/
  _create_menu(&priv->menuedit, _("_Edit"), GTK_WIDGET(menubar));
  fill_menu_edit(priv);
  /* view menu */
  _create_menu(&priv->menuview, _("_View"),GTK_WIDGET(menubar));
  fill_menu_view(priv);
  /*menu code*/
  priv->menucode = gtk_menu_new();
  priv->code = gtk_menu_item_new_with_mnemonic(_("_Code"));
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(priv->code), priv->menucode);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), priv->code);

  fill_menu_code(priv);
}


static void MENUBAR_constructed (GObject *menubar)
{
    MenuBarPrivate *priv = MENUBAR_GET_PRIVATE(menubar);

    gtk_window_add_accel_group(GTK_WINDOW(priv->main_window->window), priv->accel_group);

  /*plugin menu*/
  priv->menuplugin= gtk_plugin_manager_menu_new (priv->accel_group, priv->main_window);
  priv->plugin = gtk_menu_item_new_with_mnemonic(_("_Plugin"));
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(priv->plugin), priv->menuplugin);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), priv->plugin);

  /* help menu */
  _create_menu(&priv->menuhelp, _("_Help"), GTK_WIDGET(menubar));
  fill_help_menu(priv);

    prepare_menu_file(priv);
    prepare_help_menu(priv);
    prepare_menu_edit(priv);
    prepare_menu_view(priv);
    prepare_menu_code(priv);
}

GtkWidget *
menubar_new (gpointer main_window)
{
    MainWindow *main_win = (MainWindow *) main_window;
	return GTK_WIDGET(g_object_new (GOBJECT_TYPE_MENUBAR, "main_window", main_win, NULL));
}

GtkWidget *menubar_get_menu_plugin(MenuBar *menubar)
{
  if (!menubar) return NULL;
  MenuBarPrivate *priv = MENUBAR_GET_PRIVATE(menubar);
  return priv->menuplugin;
}

void menubar_set_classbrowser_status(MenuBar *menubar, gboolean value)
{
  if (!menubar) return;
  MenuBarPrivate *priv = MENUBAR_GET_PRIVATE(menubar);
  gtk_check_menu_item_set_active ((GtkCheckMenuItem *) priv->tog_class,value);
}

void menubar_update_controls(MenuBar *menubar, gboolean is_scintilla, gboolean can_preview, gboolean isreadonly)
{
  if (!menubar) return ;
  MenuBarPrivate *priv = MENUBAR_GET_PRIVATE(menubar);
  if (!priv) return ;
  if (is_scintilla){
    //activate menu items
    gtk_widget_set_sensitive (priv->code, TRUE);
    gtk_widget_set_sensitive (priv->cut, TRUE);
    gtk_widget_set_sensitive (priv->paste, TRUE);
    if (isreadonly){
      gtk_widget_set_sensitive (priv->save, FALSE);
    } else {
      gtk_widget_set_sensitive (priv->save, TRUE);
    }
    gtk_widget_set_sensitive (priv->saveas, TRUE);
    gtk_widget_set_sensitive (priv->reload, TRUE);
    gtk_widget_set_sensitive (priv->indent, TRUE);
    gtk_widget_set_sensitive (priv->unindent, TRUE);
    gtk_widget_set_sensitive (priv->replace, TRUE);
    gtk_widget_set_sensitive (priv->plugin, TRUE);
    gtk_widget_set_sensitive (priv->undo, TRUE);
    gtk_widget_set_sensitive (priv->redo, TRUE);
    gtk_widget_set_sensitive (priv->phphelp, TRUE);
    gtk_widget_set_sensitive (priv->upper, TRUE);
    gtk_widget_set_sensitive (priv->lower, TRUE);
    gtk_widget_set_sensitive (priv->gotoline, TRUE);
    /* only show preview in html files */
      if (can_preview){
        gtk_widget_set_sensitive (priv->preview, TRUE);
      } else {
        gtk_widget_set_sensitive (priv->preview, FALSE);
      }
  }else{
    //deactivate menu items
    gtk_widget_set_sensitive (priv->code, FALSE);
    gtk_widget_set_sensitive (priv->cut, FALSE);
    gtk_widget_set_sensitive (priv->paste, FALSE);
    gtk_widget_set_sensitive (priv->save, FALSE);
    gtk_widget_set_sensitive (priv->saveas, FALSE);
    gtk_widget_set_sensitive (priv->reload, FALSE);
    gtk_widget_set_sensitive (priv->indent, FALSE);
    gtk_widget_set_sensitive (priv->unindent, FALSE);
    gtk_widget_set_sensitive (priv->replace, FALSE);
    gtk_widget_set_sensitive (priv->plugin, FALSE);
    gtk_widget_set_sensitive (priv->undo, FALSE);
    gtk_widget_set_sensitive (priv->redo, FALSE);
    gtk_widget_set_sensitive (priv->phphelp, FALSE);
    gtk_widget_set_sensitive (priv->upper, FALSE);
    gtk_widget_set_sensitive (priv->lower, FALSE);
    gtk_widget_set_sensitive (priv->preview, FALSE);
    gtk_widget_set_sensitive (priv->gotoline, FALSE);
  }
}
