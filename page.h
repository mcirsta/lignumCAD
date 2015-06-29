/* -*- c++ -*-
 * page.h
 *
 * Header for the Page class
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
#ifndef PAGE_H
#define PAGE_H

#include <qstringlist.h>

#include "modelitem.h"
#include "figure.h"

class Model;
class TopoDS_Shape;
/*!
 * This is the basic page. A model is constructed out of a series of
 * pages, which come in the usual 2D and 3D varieties. A page contains
 * a set of Figures.
 */
class PageBase : public ModelItem {
  Q_OBJECT
public:
  PageBase ( uint id, const QString& name, const QString& type, Model* parent );

  virtual ModelItem* lookup ( QStringList& path_components ) const = 0;
  virtual ModelItem* lookup ( QValueVector<uint>& id_path ) const = 0;
  virtual Handle(Standard_Type) lookupType ( QStringList& path_components )
    const = 0;
  virtual Handle(Standard_Type) lookupType ( QValueVector<uint>& id_path )
    const = 0;
  virtual TopoDS_Shape lookupShape ( QStringList& path_components )
    const = 0;
  virtual TopoDS_Shape lookupShape ( QValueVector<uint>& id_path )
    const = 0;

  virtual bool used ( const PageBase* page ) const = 0;

  QValueVector<uint> ID ( void ) const;
  QString path ( void ) const;

  virtual QString idPath ( QValueVector<uint> id_path ) const = 0;
  virtual void pathID ( QStringList& path_components, QValueVector<uint>& id_path ) const = 0;
  virtual void dumpInfo ( void ) const = 0;

protected:
  Model* parent_;
};

namespace Space2D {

  class Page : public PageBase {
    Q_OBJECT

  public:
    Page ( uint id, const QString& name, const QString& type, Model* parent );
    virtual ~Page ( void );

    Model* model ( void ) const { return parent_; }

    //! \return a unique id for the next figure.
    uint newID ( void );

    void addFigure ( Figure* figure );
    void removeFigure ( Figure* figure );
#if 0
    QPtrListIterator< Figure > figures ( void ) const;
#endif
    ModelItem* lookup ( QStringList& path_components ) const;
    ModelItem* lookup ( QValueVector<uint>& /*id_path*/ ) const
    {
      return 0; // Not needed (currently) in 2D context.
    }

    Handle(Standard_Type) lookupType ( QStringList& /*path_components*/ ) const
    {
      return Handle(Standard_Type)(); // Not needed (currently) in 2D context
    }
    Handle(Standard_Type) lookupType ( QValueVector<uint>& /*path_components*/ ) const
    {
      return Handle(Standard_Type)(); // Not needed (currently) in 2D context
    }
    TopoDS_Shape lookupShape ( QStringList& /*path_components*/ ) const;
    TopoDS_Shape lookupShape ( QValueVector<uint>& /*path_components*/ ) const;

    bool used ( const PageBase* /*page*/ ) const { return false; }

    QString idPath ( QValueVector<uint> id_path ) const;
    void pathID ( QStringList& path_components, QValueVector<uint>& id_path ) const;

    void dumpInfo ( void ) const;

  public slots:
    void modelRenamed ( const QString& name );

  protected:
    QMap< uint, Figure* > figures_;

  private:
    //! Unique number for each figure on this page.
    uint unique_figure_id_;
  };
} // End of Space2D namespace

namespace Space3D {

  /*!
   * In 3D, this represents a closed solid consisting of one or more parts.
   * It also includes pointers to any construction datums. These are all
   * represented as Space3D::Figures.
   */
  class Page : public PageBase {
    Q_OBJECT
  public:
    Page ( uint id, const QString& name, const QString& type, Model* parent );
    virtual ~Page ( void );

    Model* model ( void ) const { return parent_; }

    //! \return a unique id for the next figure.
    uint newID ( void );

    void addFigure ( Figure* figure );
    void removeFigure ( Figure* figure );

    const QMap< uint, Figure* >& figures ( void ) const { return figures_; }

    //! \return the representation of this solid as an OpenCASCADE shape.
    //    virtual const TopoDS_Shape& shape ( void ) const = 0;

    ModelItem* lookup ( QStringList& path_components ) const;
    ModelItem* lookup ( QValueVector<uint>& id_path ) const;

    void dumpInfo ( void ) const;

  protected:
    QMap< uint, Figure* > figures_;

  private:
    //! Unique number for each figure on this page.
    uint unique_figure_id_;
  };
} // End of Space3D namespace

#endif // PAGE_H
