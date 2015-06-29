/* -*- c++ -*-
 * modelitem.h
 *
 * Header for the ModelItem class
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
#ifndef MODELITEM_H
#define MODELITEM_H

#include <qobject.h>

class QDomElement;
class DBURL;

/*!
 * This is the basic header inherited by all important entities in lignumCAD.
 * It would appear that it is necessary so that, when arbitrary entities
 * are lookup'ed, they can be effectively dynamically cast to the proper
 * type. Is this the best solution?
 *
 * The Id and Type of the model item are constants; only the name can be
 * changed after the object is created. Id is unique among the model item's
 * siblings and must reflect the order in which the object is created.
 *
 * The other job of the model item interface is to provide a means for
 * an item to be notified when one of its references becomes available
 * at de-serialization time. An item may reference another item which
 * is deserialized later in the input file. The file reader will call
 * the resolved() method if a callback has been registered by the item.
 */
class ModelItem : public QObject {
  Q_OBJECT

public:
  /*!
   * A ModelItem is a QObject, so almost every object in lignumCAD
   * can send and receive signals.
   * The type string is standardized and should be in constants.h.
   * For debugging purposes, the
   * constructor sets the QObject's "objname" to the latin1 version of the
   * item's UNICODE name (mostly so something shows up in connect() failure
   * messages).
   * \param id numerical id of the object.
   * \param name (UNICODE) name of the object.
   * \param type (string) type of the object.
   */
  ModelItem ( uint id, const QString& name, const QString& type )
    : QObject( 0 ), id_( id ), name_( name ), type_( type )
  {
      this->setName(name);
  }
  //! Destructor does do anything.
  virtual ~ModelItem ( void )
  {}
  //! \return the identifier of the object.
  uint id ( void ) const { return id_; }
  //! \return the (UNICODE) name of the object.
  QString name ( void ) const { return name_; }
  //! \return the type of the object.
  QString type ( void ) const { return type_; }
  /*!
   * Determine the full hierarchical object id. The subclass
   * which knows the parent of this object should implement this method.
   * Note: The hierarchical object id is a QValueVector (as opposed to a
   * regular STL vector<>) since the Qt version is a QShared object,
   * which should improve performance as a return value.
   * \return the hierarchical object id.
   */
  virtual QValueVector<uint> ID ( void ) const = 0;
  /*!
   * Determine the fully qualified object name. The subclass which
   * knows the parent of this object should implement this method.
   * \return the fully qualified object name.
   */
  virtual QString path ( void ) const = 0;
  /*!
   * \return the path name converted into a DB URL reference.
   */
  DBURL dbURL ( void ) const;
  /*!
   * If an unserialized geometry could not find a reference when it was
   * first created, it should have registered with the Model to have
   * a delayed resolution of the missing reference. When Model finds the
   * reference has been created, it invokes this method to report it to the
   * ModelItem. This is an optional routine.
   * \param db_path the path of the object which requested the resolution
   * (i.e., it should be this object!).
   * \param item reference's memory pointer.
   */
  virtual void resolved ( const QString& /*db_path*/, ModelItem* /*item*/ ) {}
  /*!
   * Serialize the object. The object creates an XML representation of
   * itself and appends it to the argument.
   * \param xml_rep parent XML representation.
   */
  virtual void write ( QDomElement& xml_rep ) const = 0;
  /*!
   * Translate a string from the constant context.
   * \param string constant string to translate.
   */
  const QString trC ( const QString& string ) const;

public slots:
  /*!
   * Generally, it's sufficent to just set the (UNICODE) name of the object
   * with this routine. However, some subclasses may need to manipulate the
   * names of dependent objects as well.
   * The QObject "objname" is changed as well.
   * This routine emits the nameChanged signal (evidently even if the
   * name is not changed?)
   * \param name new name of item.
   */
  virtual void setName ( const QString& name );

signals:
  /*!
   * Emitted when the name of the item is changed.
   * \param name new name of item.
   */
  void nameChanged ( const QString& name );

private:
  //! The permanent (within a session, anyway) identifier of the object.
  const uint id_;
  //! The (UNICODE) name of the object.
  QString name_;
  //! The (string) type of the object.
  const QString& type_;
};

#endif // MODELITEM_H
