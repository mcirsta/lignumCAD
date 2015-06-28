/* -*- c++ -*-
 * designbookview.h
 *
 * Header for the DesignBookView class
 * Copyright (C) 2002 lignum Computing, Inc. <lignumcad@lignumcomputing.com>
 * $Id$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef DESIGNBOOKVIEW_H
#define DESIGNBOOKVIEW_H

#include <vector>

#include <qobject.h>
#include <qptrlist.h>
#include <qptrdict.h>
#include <qvbox.h>
#include <qpalette.h>

#include "constants.h"
#include "view.h"

class QDomElement;
class QLabel;
class QWidget;
class DBURL;
class QPainter;
class QListBoxItem;
class QListViewItem;
class ListViewItem;
class OpenGLView;
class OpenGLExample;
class OpenGLPrinter;
class TabBarContext;
class lignumCADMainWindow;
class PageView;
class NewModelWizard;
class ModelInfoDialog;
class PageInfoDialog;
class PreferencesDialog;
class Model;
class Ratio;
class Printer;

/*!
 * Encapsulate the OpenGL view and the TabBar which constitute the main user
 * interface of lignumCAD. This is Q_OBJECT so it can receive the signals from
 * it's GUI children.
 */
class DesignBookView : public QVBox, public View {
Q_OBJECT
  lignumCADMainWindow* lCMW_;

  QColorGroup app_palette_;

  OpenGLView* opengl_view_;
  TabBarContext* page_tabbar_;

  QPtrList< PageView > page_views_;
  QPtrDict< PageView > page_tabs_;

  NewModelWizard* new_model_wizard_;
  ModelInfoDialog* model_info_dialog_;
  PageInfoDialog* page_info_dialog_;

  QWidget* old_central_widget_;
  bool gui_visible_;

  Model* model_;

  ListViewItem* model_list_item_;

  bool printing_;
  static Printer* printer_;
  static OpenGLPrinter* opengl_printer_;

  static PreferencesDialog* preferences_dialog_;
  static OpenGLExample* preferences_example_;

public:
  DesignBookView ( lignumCADMainWindow* lCMW );
  DesignBookView ( lignumCADMainWindow* lCMW, const QString file_name );
  ~DesignBookView ( );

  OpenGLView* view ( void ) const;
  Model* model ( void ) const { return model_; }
  lignumCADMainWindow* lCMW ( void ) const { return lCMW_; }

  ListViewItem* modelListItem ( void ) const { return model_list_item_; }

  QColorGroup appPalette ( void ) const { return app_palette_; }

  bool aboutToExit ( void );

  void addPageView ( PageView* page_view );
  void showPageView ( PageView* page_view );
  void removePageView ( PageView* page_view );
  PageView* lastPageView ( void );
  QListViewItem* previousItem ( uint id ) const;
  void deletePage ( PageView* page_view );
  View* lookup ( const DBURL& db_url );
  vector<GLuint> lookup ( const QValueVector<uint>& id_path ) const;

  void modelChanged ( bool status );

  void clear ( void );

  void undo ( void );
  void redo ( void );
  void cut ( void );
  void copy ( void );
  void paste ( void );
  /*!
   * Make up a unique name for this new page.
   * \param newName pointer to function returning a possible name.
   * \param type type of page being named.
   * \return unique name for page.
   */
  QString uniqueName ( QString (*newName)( void ), const QString& type );
  /*!
   * Check to make sure the new name is not used by another page
   * of the same type. Displays a modal dialog if the name is already used.
   * \param page_view page being changed (so we can tell which page view to ignore).
   * \param name new name of page.
   * \param type type of page being renamed.
   * \return flag indicating status: OK = rename can proceed, Rejected =
   * name exists and user doesn't want to apply changes after all, Redo =
   * go back to previous editing step.
   */
  lC::RenameStatus uniquePageName ( const PageView* page_view,
				    const QString& name,
				    const QString& type ) const;

  // Implementation of View interface
  QString name ( void ) const;
  DBURL dbURL ( void ) const;

public slots:
  void editPreferences ( void );
  void pageChanged ( int id );
  void renamePage ( void );
  void deletePage ( void );
  void newModel ( void );
  bool save ( void );
  bool saveAs ( void );
  void print ( void );
  void exportPage ( void );
  void open ( void );
  void editModelInfo ( void );
  void updateCutCopy ( bool );

signals:
  void setCaption ( const QString& caption );
  void pageChanged ( const QString& name );

private:
  bool uniquePageName ( const QString name ) const;
  void showView ( void );
  void hideView ( void );
  void init ( void );
  bool write ( void );
  bool read ( const QString file_name );
  void restoreViews ( const QDomElement& xml_rep );
  bool newModelWizard ( Model* model, uint& initial_page_id );

private slots:
  void setName ( const QString& );
  void updateName ( const QString& );
  void updateUndoRedo ( bool, bool );
};

/*!
 * There are an unknowable number of types of Pages. So, rather
 * than statically create a method for each type, this helper
 * class can invoke the creation routine for a given type
 * when its QAction command is invoked.
 */
class CreatePage : public QObject {
  Q_OBJECT
public:
  /*!
   * This object consists almost entirely of just the slot which
   * is invoked when the action is activated.
   * \param view the design book view in which to insert the page.
   * (Stored as the parent of this object.)
   * \param id type of page to create.
   */
  CreatePage ( DesignBookView* view, uint id )
    : QObject( view ), id_( id )
  {
    //cout << "constructing createpage for " << id_ << endl;
  }
  ~CreatePage ( void )
  {
    //cout << "destructing createpage for " << id_ << endl;
  }
public slots:
  /*!
   * The parent design book view connects the create action to this slot.
   */
  void createPage ( void );

private:
  //! Type of page to create.
  uint id_;
};

#endif // DESIGNBOOKVIEW_H
