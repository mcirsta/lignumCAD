/* -*- c++ -*-
 * figureview.h
 *
 * Header for the FigureView classes
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
#ifndef FIGUREVIEW_H
#define FIGUREVIEW_H

#include <vector>

#include "graphics.h"
#include "curve.h"
#include "view.h"

class Model;
class OpenGLView;
class PageView;
class QDomElement;
class CreateObject;
class ListViewItem;
class FigureBase;

class FigureViewBase : public QObject, public GraphicsObject, public View {
  Q_OBJECT

  PageView* parent_;

  GLuint selection_name_;
  bool highlighted_;
  bool activated_;

public:
  FigureViewBase ( PageView* parent );
  virtual ~FigureViewBase ( void );
  PageView* parent ( void ) const { return parent_; }
  
  GLuint selectionName ( void ) const { return selection_name_; }
  virtual ListViewItem* listViewItem ( void ) const = 0;

  Model* model ( void ) const;
  OpenGLView* view ( void ) const;

  virtual uint id ( void ) const = 0;
  virtual QString name ( void ) const = 0;
  virtual QString type ( void ) const = 0;
  /*!
   * Generate some text to show in the main window status bar indicating
   * what is selected by the given selection name and entity. For example,
   * if entity is FIGURE, just give the name of the figure. If entity is
   * FACE, give "figure/face".
   * \param selection_name item's selection name.
   * \param entity type of entity currently being sought.
   * \return selection string representation.
   */
  virtual QString selectionText ( const std::vector<GLuint>& selection_name,
				  SelectionEntity entity ) const = 0;

  //  virtual FigureBase* figure ( void ) const = 0;
  virtual CreateObject* memento ( void ) = 0;

  virtual View* lookup ( QStringList& path_components ) const = 0;
  virtual void lookup ( QVector<uint>& id_path, std::vector<GLuint>& name_path ) const = 0;

  bool isHighlighted ( void ) const { return highlighted_; }
  void setHighlighted ( bool highlight );
  virtual void setHighlighted ( bool highlight, SelectionEntity entity,
				const std::vector<GLuint>& items ) = 0;

  bool isActivated ( void ) const { return activated_; }
  void setActivated ( bool activate );
  virtual void setActivated ( bool activate, SelectionEntity entity,
			      const std::vector<GLuint>& items ) = 0;

  virtual InputObject* createInput ( void ) = 0;
  virtual InputObject* modifyInput ( void ) = 0;

  virtual uint dimensionPickCount ( void ) const = 0;
  virtual void setDimensionView ( const QDomElement& xml_rep ) = 0;

  virtual void viewAttributeChanged ( void ) = 0;
  /*!
   * Create an XML representation of this view (only) suitable for saving
   * in a file with the rest of the model information.
   * \param xml_rep the XML representation of the model to append
   * this representation to.
   */
  virtual void write ( QDomElement& xml_rep ) const = 0;
  /*!
   * Create an XML representation for this view and its object suitable
   * for use as a clipboard selection.
   * \param xml_rep the XML representation of the selection to append
   * this representation to.
   */
  virtual void copyWrite ( QDomElement& xml_rep ) const = 0;

  virtual void editInformation ( void ) = 0;
};

namespace Space2D {
  class Curve;

  // This is the two dimensional figure view. Its purpose is solely to
  // add 2D geometry lookup to a figure view.

  class FigureView : public FigureViewBase {
    Q_OBJECT
  public:
    FigureView ( PageView* parent );
    virtual ~FigureView ( void ) {}

    Space space ( void ) const { return SPACE2D; }

    virtual Curve* geometry ( GLuint /*selection_name*/ ) const { return 0; }

    // Not implemented in 2D...
    void lookup ( QVector<uint>& /*id_path*/, std::vector<GLuint>& /*name_path*/ )
      const
    {}
  };
} // End of Space2D namespace

namespace Space3D {
  //  class Curve;

  // This is the three dimensional figure view. Its purpose is solely to
  // add 3D geometry lookup to a figure view.

  class FigureView : public FigureViewBase {
    Q_OBJECT
  public:
    FigureView ( PageView* parent );
    virtual ~FigureView ( void ) {}

    Space space ( void ) const { return SPACE3D; }

    virtual QString geometry ( GLuint /*selection_name*/ ) const
    { return QString::null; }
  };
} // End of Space3D namespace

#endif // FIGUREVIEW_H
