/* -*- c++ -*-
 * model.h
 *
 * Header for the Model class
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
#ifndef MODEL_H
#define MODEL_H

#include <qdatetime.h>
#include <qmap.h>

#include "modelitem.h"
#include "page.h"

#include <memory>

class QDomDocument;
class QDomElement;
class TopoDS_Shape;

class DelayedResolution {
  ModelItem* item_;
  const QString db_path_;
public:
  DelayedResolution ( ModelItem* item, const QString& db_path )
    : item_( item ), db_path_( db_path )
  {}
  ModelItem* item ( void ) const { return item_; }
  QString dbPath ( void ) const { return db_path_; }
};

/*!
 * This is the basic model (i.e., the piece of furniture) which we are
 * designing.
 */
class Model : public ModelItem {
  Q_OBJECT

  typedef uint VERSION_NO;

public:
  Model ( const QString name, const QString description );
  Model ( const QString file_name, const QDomElement& xml_rep );
  ~Model ( void );

  /*!
   * Determine the full hierarchical item id.
   * \return the hierarchical item id.
   */
  QVector<uint> ID ( void ) const;
  /*!
   * Determine the fully qualified item name. The subclass which
   * knows the parent of this object should implement this method.
   * \return the fully qualified item name.
   */
  QString path ( void ) const;
  /*!
   * Return the fully qualified item name given the ID path.
   * \param id_path full id path to object.
   * \return the fully qualified item name.
   */
  QString idPath ( const QVector<uint>& id_path ) const;
  /*!
   * Reciprocal to idPath(): return the id path given the DBURL of an
   * item.
   * \param db_url URL reference to item.
   * \return id path to item.
   */
  QVector<uint> pathID ( const DBURL& db_url ) const;
  /*!
   * Serialize the model. The object creates an XML representation of
   * itself and appends it to the argument.
   * \param xml_rep parent XML representation.
   */
  void write ( QDomElement& xml_rep ) const;

  QString readFileName ( void ) const { return read_file_name_; }
  void setReadFileName ( const QString file_name ) { read_file_name_ = file_name; }
 
  QString writeFileName ( void ) const { return write_file_name_; }
  void setWriteFileName ( const QString file_name ) { write_file_name_ = file_name;}

  QString description ( void ) const { return description_; }
  void setDescription ( const QString description )
  {
    if ( description_ != description ) {
      description_ = description;
      //      if ( !changed_ ) revision_++;
      changed_ = true;
      modified_ = QDateTime::currentDateTime();
    }
  }
 
  QDateTime created ( void ) const { return created_; }
 
  QDateTime modified ( void ) const { return modified_; }
 
  VERSION_NO version ( void ) const { return version_; }
  void setVersion ( VERSION_NO version )
  {
    if ( version_ != version ) {
      version_ = version;
      changed_ = true;
      modified_ = QDateTime::currentDateTime();
    }
  }
 
  VERSION_NO revision ( void ) const { return revision_; }
  void setRevision ( VERSION_NO revision )
  {
    if ( revision_ != revision ) {
      revision_ = revision;
      changed_ = true;
      modified_ = QDateTime::currentDateTime();
    }
  }
  void incrRevision ( void )
  {
    revision_++;
    changed_ = true;
    modified_ = QDateTime::currentDateTime();
  }

  bool changed ( void ) const { return changed_; }
  void setChanged ( bool changed )
  {
    changed_ = changed;
    modified_ = QDateTime::currentDateTime();
  }

  //! \return a unique id for the page
  uint newID ( void );

  void addPage ( PageBase* model_item );
  void removePage ( PageBase* model_item );
  /*!
   * Lookup the model item associated with this URL.
   * \param db_url DBURL of item.
   * \return model item of DBURL.
   */
  ModelItem* lookup ( const DBURL& db_url ) const;
  /*!
   * Lookup the model item associated with this ID path.
   * \param id_path ID path of item.
   * \return model item of ID path.
   */
  ModelItem* lookup ( const QVector<uint>& id_path ) const;

  /*!
   * Look up the geometry type of an OpenCASCADE shape.
   * \param db_url path to solid shape.
   */
  Handle(Standard_Type) lookupType ( const DBURL& db_url ) const;

  /*!
   * Look up the geometry type of an OpenCASCADE shape.
   * \param id_path id path to solid shape.
   */
  Handle(Standard_Type) lookupType ( const QVector<uint>& id_path ) const;
  /*!
   * Look up the topology of an OpenCASCADE shape. Should return
   * a shape which has the proper Location based on its traversal
   * of the assembly hierarchy.
   * \param db_url path to solid shape.
   */
  TopoDS_Shape lookupShape ( const DBURL& db_url ) const;
  /*!
   * Look up the topology of an OpenCASCADE shape. Should return
   * a shape which has the proper Location based on its traversal
   * of the assembly hierarchy.
   * \param id_path id path to solid shape.
   */
  TopoDS_Shape lookupShape ( const QVector<uint>& id_path ) const;
  /*!
   * Another lookup style routine: find all the pages which reference
   * the given item.
   * \param db_url path to page.
   */
  QList<std::shared_ptr<PageBase>> whereUsed( const DBURL& db_url ) const;

  void addDelayedResolution ( ModelItem* item, const QString db_path );
  void resolveNow ( void );

  const QMap<uint, PageBase*>& pages ( void ) const { return pages_; }

protected:
  void restoreMetadata ( const QDomElement& xml_rep );
  void restorePages ( const QDomElement& xml_rep );
  void setCreated ( const QDateTime& created ) { created_ = created; }
  void setModified ( const QDateTime& modified ) { modified_ = modified; }

private:
  //! \return a unique id for the next model.
  static uint uniqueID ( void );
  //! Models number themselves.
  static uint unique_id_;

  //! Unique page ids in the model.
  uint unique_page_id_;

  QString read_file_name_;
  QString write_file_name_;
  QString description_;
  QDateTime created_;
  QDateTime modified_;
  VERSION_NO version_;
  VERSION_NO revision_;

  bool changed_;

  //! The main collection of pages which constitute the model. The QMap
  //! keeps them sorted by id.
  QMap< uint, PageBase* > pages_;

  static const VERSION_NO INITIAL_VERSION = 1;
  static const VERSION_NO INITIAL_REVISION = 0;

  //! Transient list of items which need delayed resolution of references
  QList< std::shared_ptr<DelayedResolution> > delay_resolutions_;
};

#endif // MODEL_H
