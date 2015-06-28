/* -*- c++ -*-
 * command.h
 *
 * Header for the Command class
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
#ifndef COMMAND_H
#define COMMAND_H

#include <qobject.h>
#include <QList>
#include <qdom.h>

#include "dburl.h"
#include "vectoralgebra.h"

class Model;
class ModelItem;
namespace Space2D {
  class ConstrainedLine;
}
class QFile;
class QTextStream;
/*!
 * Each user action can be represented by a command. This will serve
 * two (no...three...aieeee!) purposes (hopefully):
 * 1) The Undo/Redo mechanism can save these in the command history list,
 * 2) Provides a dribble mechanism for repeating sessions,
 * 3) The scripting interface should be able to invoke these operations.
 * (anything else?)
 */
class Command {
  QString name_;
public:
  Command ( const QString& name );
  virtual ~Command ( void );

  enum Type {
    Create,
    Delete,
    Rename,
    MoveLines,
    Reconstrain,
    MoveDimension,
    MoveRectangle,
    RestyleRectangle,
    MoveAnnotation,
    ChangeAnnotation,
    ChangeView,
    ModifySolid,
    CreateConstraint,
    DeleteConstraint,
    ChangeConstraintOffset,
    ChangeMaterial
  };

  QString name() const;

  virtual Type type() const = 0;

  virtual bool merge ( Command* command ) = 0;
  virtual void execute ( void ) = 0;
  virtual void unexecute ( void ) = 0;
  virtual void write ( QDomDocument* document ) const = 0;
};
/*!
 * Collect an ordered list of commands. This is a singleton object
 * shared by everything in the program.
 */
class CommandHistory : public QObject {
Q_OBJECT
  int current_;
  QList< Command > history_;
  QFile* file_;
  QTextStream* stream_;
  QDomDocument* document_; 

  static CommandHistory command_history_;

protected:
  CommandHistory ( void );
  ~CommandHistory ( void );

public:
  static CommandHistory& instance ( void );

  //! \return true if the command history is ready to accept commands.
  bool ready ( void ) const { return document_ != 0; }

  void addCommand ( Command* command );
  void undo ( void );
  void redo ( void );
  //! \return true if the history current command points before
  //! the beginning of the list (means model is unchanged, i.e., reverted
  //! to initial state).
  bool canUndo ( void ) const { return current_ < 0; }

  void flushOnSegV ( void );
private:
  void emitUndoRedo ( void );
signals:
  void undoRedoChanged ( bool undoAvailable, bool redoAvailable );
};

class RenameCommand;

/*!
 * CreateObject is an interface which objects can implement to
 * recreate or remove themselves. Kind of a memento. Also provides
 * a way to merge a rename command.
 */
class CreateObject {
public:
  virtual ~CreateObject ( void ) {}

  virtual void create ( void ) = 0;
  virtual void remove ( void ) = 0;
  virtual QDomDocument* representation ( void ) = 0;
  virtual void write ( QDomDocument* document ) = 0;
  virtual bool mergeRename ( RenameCommand* command ) = 0;
};
/*!
 * Create the object specified by the CreateObject memento.
 */
class CreateCommand : public Command {
  CreateObject* creator_;
public:
  CreateCommand ( const QString& name, CreateObject* creator );
  ~CreateCommand ( void );

  Type type ( void ) const { return Create; }

  bool merge ( Command* command );
  void execute ( void );
  void unexecute ( void );
  void write ( QDomDocument* document ) const;
};
/*!
 * Delete the object mentioned in the memento (inverse of Create, I guess)
 */
class DeleteCommand : public Command {
  CreateObject* creator_;
public:
  DeleteCommand ( const QString& name, CreateObject* creator_ );
  ~DeleteCommand ( void );

  Type type ( void ) const { return Delete; }

  bool merge ( Command* /*command*/ ) { return false; }
  void execute ( void );
  void unexecute ( void );
  CreateObject* creator ( void ) const { return creator_; }
  void write ( QDomDocument* document ) const;
};
/*!
 * Rename the object mentioned in the item.
 */
class RenameCommand : public Command {
  Model* model_;
  DBURL old_db_url_;
  DBURL new_db_url_;
public:
  RenameCommand ( const QString& name, Model* model, const DBURL& old_db_url,
		  const DBURL& new_db_url );

  Type type ( void ) const { return Rename; }

  bool merge ( Command* /*command*/ ) { return false; }
  void execute ( void );
  void unexecute ( void );
  void write ( QDomDocument* document ) const;

  DBURL oldDBURL ( void ) const { return old_db_url_; }
  DBURL newDBURL ( void ) const { return new_db_url_; }

  friend class CreateCommand;
};

/*!
 * Two things can happen when you adjust the constrainted lines of a figure:
 * they can just be moved in space or they can acquire a coincident constraints.
 * Both actions can be recorded in this structure.
 */
class MoveLinesCommand : public Command {
  //! Need to lookup the various URLs for moved or reconstained lines.
  Model* model_;
  //! Simple structure to hold the key data for moved lines.
  struct MoveLine {
    //! URL of moved line.
    DBURL db_url_;
    //! Old offset from constraint reference.
    double old_offset_;
    //! New offset from constraint reference.
    double new_offset_;
    /*!
     * Constructor just collects the data.
     * \param db_url URL of moved line.
     * \param old_offset old offset from constraint reference.
     * \param new_offset new offset from constraint reference.
     */
    MoveLine ( DBURL db_url, double old_offset, double new_offset )
      : db_url_( db_url ), old_offset_( old_offset ), new_offset_( new_offset )
    {}
  };
  //! List of lines which were moved by this command.
  QPtrList< MoveLine > lines_;
  //! (Optional) XML document containing the details of any reconstraints
  //! which were caused by the creation of coincindent constraints.
  QDomDocument* xml_rep_;

public:
  /*!
   * Constructor only wants a reference to the model.
   * \param name Command name.
   * \param model model which contains the lines to be moved.
   */
  MoveLinesCommand ( const QString& name, Model* model );
  /*!
   * Destructor deletes the XML representation if one is present.
   */
  ~MoveLinesCommand ( void );
  //! \return type of command (MoveLines).
  Type type ( void ) const { return MoveLines; }
  /*!
   * If possible, merge the command argument into this command. Currently,
   * there are no commands which are merged (though, conceivably, two
   * plain moves might be effectively merged...).
   * \param command command to merge.
   * \return true if the command was merged.
   */
  bool merge ( Command* command );
  /*!
   * Add a line to be moved. (Note: the line has already been moved.
   * The new_offset comes directly from the line.)
   * \param line constrained line which was moved.
   * \param old_offset the offset before the line was moved.
   */
  void add ( Space2D::ConstrainedLine* line, double old_offset );
  /*!
   * If coincident reconstraints occured, pass a pointer to the XML
   * representation of the old and new constraints. The command assumes
   * responsiblity for the memory.
   * \param xml_rep XML representation of the old and new constraints.
   */
  void add ( QDomDocument* xml_rep );
  /*!
   * Redo the action the user tried before (i.e., put the lines in thier
   * new places again).
   */
  void execute ( void );
  /*!
   * Undo the action the user attempted earlier (i.e., put the lines back
   * where they were).
   */
  void unexecute ( void );
  /*!
   * Append the XML representation of this command to the argument
   * XML document.
   * \param document usually CommandHistory's global XML document.
   */
  void write ( QDomDocument* document ) const;
};
/*!
 * Reconstrain the object (can only be a constrained line for now).
 */
class ReconstrainCommand : public Command {
  Model* model_;
  QDomDocument* xml_rep_;
public:
  ReconstrainCommand ( const QString& name, Model* model,
		       QDomDocument* constraints );
  ~ReconstrainCommand ( void );

  Type type ( void ) const { return Reconstrain; }

  bool merge ( Command* /*command*/ ) { return false; }
  void execute ( void );
  void unexecute ( void );
  void write ( QDomDocument* document ) const;
};
#endif /* COMMAND_H */
