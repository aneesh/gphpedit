/* This file is part of gPHPEdit, a GNOME PHP Editor.

   Copyright (C) 2003, 2004, 2005 Andy Jeffries <andy at gphpedit.org>
   Copyright (C) 2009 Anoop John <anoop dot john at zyxware.com>
   Copyright (C) 2009, 2010, 2011 José Rostagno (for vijona.com.ar) 

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


#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>

#include "document_manager.h"
#include "menubar.h"
#include "toolbar.h"
#include "preferences_manager.h"
#include "templates.h"
#include "gtksourcestyleschememanager.h"
#include "symbol_manager.h"

typedef struct
{
  GtkBuilder *pbuilder;
  /* preferences manager */
  PreferencesManager *prefmg;
  TemplatesManager *tempmg;

  GtkSourceStyleSchemeManager *stylemg;

  SymbolManager *symbolmg;

  GtkWidget *window;
  /* menu */
  GtkWidget *pmenu;
  /* toolbars */
  GtkWidget *toolbar_main;

  GtkWidget *pmain_vertical_pane;
  GtkWidget *pmain_horizontal_pane;
  /* status bar */
  GtkWidget *appbar;

  GtkWidget *notebook_manager;
  
  GtkWidget *notebook_editor;
  /* syntax check widget */
  GtkWidget *pwin;

  DocumentManager *docmg;

  //widget for close side bar button
  GtkWidget *pclose_sidebar_button;
  GtkWidget *pclassbrowser;

  /*filebrowser widget */
  GtkWidget *pfolder;

  /*needed for menu hints*/
  guint context_id;
  guint message_id;
  GHashTable *pmenu_hints;

  gboolean is_app_closing;
}
MainWindow;

extern MainWindow main_window;

void main_window_create(char **argv, gint argc);
void update_app_title(MainWindow *main_window, Documentable *document);
void side_panel_show_hide(MainWindow *main_window);
void statusbar_show_hide(MainWindow *main_window, gboolean state);
void maintoolbar_show_hide(MainWindow *main_window, gboolean state);
void fullscreen_show_hide(MainWindow *main_window, gboolean state);
void syntax_check_show(MainWindow *main_window);
void syntax_check_hide(MainWindow *main_window);
void main_window_install_menu_hint(MainWindow *main_window, GtkWidget *widget, gchar *message);
gboolean main_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
void quit_application(MainWindow *main_window);
#endif
