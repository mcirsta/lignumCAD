/* -*- c++ -*-
 * pagefactory.h
 *
 * Header for the PageFactory classes
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
#ifndef PAGEFACTORY_H
#define PAGEFACTORY_H

#include <qstring.h>
#include <qmap.h>

class Model;
class lignumCADMainWindow;
class DesignBookView;
class PageView;
class PageBase;
class QAction;
class QDomElement;

template<class T> class QValueVector;

/*!
 * The metadata defines the infomation presented to the
 * user at different times in the course of their interaction
 * with the program. Like a QAction, but constructable before
 * there is a QApplication object (hopefully...) You can also
 * create a new page and its view through this object.
 */
class PageMetadata {
public:
  /*!
   * Construct page metadata. Mostly like QAction's constructor.
   * \param order If a list of pages types is constructed, this is
   * the position of this item. Note that all of this text is in the
   * native language of the source code (i.e., English). It should
   * be translated before use in the UI.
   * \param type name of the page type.
   * \param icon name of icon file (found in the project image collection).
   * \param selection_text radio button text.
   * \param menu_text string for menu entries.
   * \param tool_tip tool tip and status line text.
   * \param whats_this text of what's this?.
   */
  PageMetadata ( const QString& type, const QString& icon,
		 const QString& selection_text,
		 const QString& menu_text, const QString& tool_tip,
		 const QString whats_this )
    : type_( type ), icon_( icon ),
      selection_text_( selection_text ), menu_text_( menu_text ),
      tool_tip_( tool_tip ),
      whats_this_( whats_this )
  {}
  //! Subclass may have something to do (but probably not).
  virtual ~PageMetadata ( void ) {}

  //! \return the page type's name.
  QString type ( void ) const { return type_; }

  //! \return the page type's icon file name.
  QString icon ( void ) const { return icon_; }

  //! \return the page type's selection (radio button) text.
  QString selection ( void ) const { return selection_text_; }

  //! \return the page type's menu text.
  QString menuText ( void ) const { return menu_text_; }

  //! \return the page type's tool tip text.
  QString toolTip ( void ) const { return tool_tip_; }

  //! \return the page type's what's this? text.
  QString whatsThis ( void ) const { return whats_this_; }

  /*!
   * Create a new page and view for this type.
   * \return view handle for page and its view.
   */
  virtual PageView* create ( DesignBookView* parent ) const = 0;

  /*!
   * Create a new page from its XML representation.
   * \return view handle for page.
   */
  virtual PageBase* create ( Model* parent, const QDomElement& xml_rep )
    const = 0;

  /*!
   * Create a new page view from its XML representation. (Note that
   * this does not create a page.)
   * \return view handle for page view.
   */
  virtual PageView* create ( DesignBookView* parent, const QDomElement& xml_rep )
    const = 0;

  /*!
   * Retrieve the QAction which can create this page.
   * \param lCMW pointer to the main window where the actions are stored.
   * \return the action associated with creating an instance of this page.
   */
  virtual QAction* action ( lignumCADMainWindow* lCMW ) const = 0;

private:
  //! Type name
  QString type_;
  //! Icon name
  QString icon_;
  //! Selection (radio button) text
  QString selection_text_;
  //! Menu text
  QString menu_text_;
  //! Tool tip and status line text
  QString tool_tip_;
  //! What's This? text
  QString whats_this_;
};

/*!
 * The Page factory constructs concrete pages and their views given
 * a concrete type name.
 */
class PageFactory {
public:
  //! \return the singleton instance of the pageview factory.
  static PageFactory* instance( void );

  void addPageMetadata ( uint id, PageMetadata* page_data );

  QValueVector<uint> pageIDs ( void ) const;

  QString type ( uint i ) const { return page_data_[i]->type(); }
  QString icon ( uint i ) const { return page_data_[i]->icon(); }
  QString selection ( uint i ) const { return page_data_[i]->selection(); }
  QString toolTip ( uint i ) const { return page_data_[i]->toolTip(); }
  QString whatsThis ( uint i ) const { return page_data_[i]->whatsThis(); }

  /*!
   * Create a page and its view from its factory.
   * \param id the page type id.
   * \param parent the parent of the view.
   * \return the pointer to the new page view.
   */
  PageView* create ( uint id, DesignBookView* parent ) const
  {
    return page_data_[id]->create( parent );
  }
  /*!
   * Create a page from its XML representation.
   * \param parent the parent of the page.
   * \param xml_rep XML representation of a page.
   * \return the pointer to the reconstructed page.
   */
  PageBase* create ( Model* parent, const QDomElement& xml_rep ) const;

  /*!
   * Create a page view from its XML representation.
   * \param parent the parent of the view.
   * \param xml_rep XML representation of a page view.
   * \return the pointer to the reconstructed page view.
   */
  PageView* create ( DesignBookView* parent, const QDomElement& xml_rep ) const;

  /*!
   * Find the QAction associated with creating this page.
   * \param id the page type id.
   * \param lCMW the widget within which the QAction was created.
   */
  QAction* action ( uint id, lignumCADMainWindow* lCMW )
  {
    return page_data_[id]->action( lCMW );
  }

protected:
  //! Per "Design Patterns": It can only construct itself. 
  PageFactory ( void );

private:
  //! The singleton instance of the pageview factory.
  static PageFactory* instance_;

  //! Page metadata indexed by their order.
  QMap<uint, PageMetadata*> page_data_;
};

#endif // PAGEFACTORY_H
