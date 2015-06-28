/*
 * command.cpp
 *
 * Command classes: Execute lignumCAD commands (undo/redo, dribble)
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
#include <qapplication.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qregexp.h>

#if defined(Q_OS_UNIX)
extern "C" {
#include <stdlib.h>
#include <execinfo.h>
}
#endif

#include "constants.h"
#include "model.h"
#include "modelitem.h"
#include "line.h"
#include "constrainthistory.h"
#include "command.h"

Command::Command ( const QString& name )
  : name_( name )
{}

Command::~Command ( void )
{}

QString Command::name ( void ) const
{
  return name_;
}

CommandHistory CommandHistory::command_history_;

CommandHistory& CommandHistory::instance ( void )
{
  return command_history_;
}

CommandHistory::CommandHistory ( void )
  : QObject( 0, "command_history" ), current_( -1 ), file_( 0 ), stream_( 0 ),
    document_( 0 )
{
  history_.setAutoDelete( true );

  file_ = new QFile( "history.xml" );
  if ( file_->open( IO_WriteOnly ) ) {
    stream_ = new QTextStream( file_ );

    document_ = new QDomDocument( "LIGNUMCAD-HISTORY" );

    *stream_ << "<LIGNUMCAD-HISTORY version=\"1\">" << endl;
  }
}

CommandHistory::~CommandHistory ( void )
{
  if ( document_ != 0 )
    delete document_;

  if ( stream_ != 0 ) {
    *stream_ << "</LIGNUMCAD-HISTORY>" << endl;
    delete stream_;
  }

  if ( file_ != 0 ) {
    file_->close();
    delete file_;
  }
}

void CommandHistory::addCommand ( Command* command )
{
  // Write this command to the dribble file. Maybe later we'll try
  // to reduce the quantity of information written out (like compressing
  // view modifications).
  command->write( document_ );
  *stream_ << document_->firstChild();
  stream_->device()->flush();

  // If the current command is not the end of the list, then
  // this command basically truncates the remainder of the history
  // and we now take a new course into the future.
  if ( current_ < (int)history_.count() - 1 ) {
    QPtrList< Command > commands;

    for ( int i = 0; i <= current_; i++ ) {
      commands.append( history_.at( 0 ) );
      history_.take( 0 );
    }

    history_.clear();
    history_ = commands;
    history_.setAutoDelete( true );
  }

  // Can this command be merged into the current command?
  if ( history_.last() != 0 ) {
    if ( !history_.last()->merge( command ) ) {

      history_.append( command );

      current_++;
    }
    else
      delete command;
  }
  else {
    history_.append( command );

    current_++;
  }

  emitUndoRedo();
}

void CommandHistory::undo ( void )
{
  // I guess the main complication here is that allowing redo
  // means that you cannot discard commands after they are undone.
  // Therefore, the current command to be undone may be somewhere
  // in the middle of the list.

  if ( current_ > -1 ) {

    history_.at( current_ )->unexecute();

    *stream_ << "<undo command=\"" << history_.at( current_ )->name() << "\"/>"
	     << endl;
    stream_->device()->flush();

    current_--;
  }

  emitUndoRedo();
}

void CommandHistory::redo ( void )
{
  if ( current_ > -1 ) {
    // current command is somewhere in the middle of the list
    if ( current_ < (int)history_.count() - 1 ) {
      current_++;

      history_.at( current_ )->execute();

      *stream_ << "<redo command=\"" << history_.at( current_ )->name() << "\"/>"
	       << endl;
      stream_->device()->flush();
    }
  }
  else {
    // current command is at beginning of list (i.e., no more undoable
    // commands)
    if ( history_.count() > 0 ) {
      current_++;

      history_.at( current_ )->execute();

      *stream_ << "<redo command=\"" << history_.at( current_ )->name() << "\"/>"
	       << endl;
      stream_->device()->flush();
    }
  }

  emitUndoRedo();
}

void CommandHistory::emitUndoRedo ( void )
{
  bool undo_available = false;
  bool redo_available = false;

  if ( current_ >= 0 && current_ < (int)history_.count() )
    undo_available = true;

  if ( current_ + 1 >= 0 && current_ + 1 < (int)history_.count() )
    redo_available = true;

  emit undoRedoChanged( undo_available, redo_available );
}

void CommandHistory::flushOnSegV ( void )
{
  // In an emergency, write a stack trace to the history file and then
  // close the dribble file's output stream.
  cerr << "Got a segv" << endl;
  if ( stream_ != 0 ) {
    stream_->device()->flush();
#if defined(Q_OS_UNIX)
    {
      const int BTSIZE = 200;
      void* array[BTSIZE];
      size_t size;
      char** strings;
      size = backtrace( array, BTSIZE );
      strings = backtrace_symbols( array, size );
      *stream_ << "<backtrace>" << endl;
      for ( size_t i = 0; i < size; ++i )
	*stream_ << strings[i] << endl;
      *stream_ << "</backtrace>" << endl;
      free( strings );
    }
#endif
    stream_->device()->close();
  }
}

CreateCommand::CreateCommand ( const QString& name, CreateObject* creator )
  : Command( name ), creator_( creator )
{}

CreateCommand::~CreateCommand ( void )
{
  if ( creator_ != 0 ) delete creator_;
}

bool CreateCommand::merge ( Command* command )
{
  if ( command->type() == Rename )
    return creator_->mergeRename( dynamic_cast<RenameCommand*>( command ) );

  return false;
}

void CreateCommand::execute ( void )
{
  creator_->create();
}

void CreateCommand::unexecute ( void )
{
  creator_->remove();
}

void CreateCommand::write ( QDomDocument* document ) const
{
  QDomElement create_element = document->createElement( lC::STR::CREATE );
  QDomNode root_node = document->firstChild();
  if ( !root_node.isNull() )
    document->replaceChild( create_element, root_node );
  else
    document->appendChild( create_element );
  creator_->write( document );
}

DeleteCommand::DeleteCommand ( const QString& name, CreateObject* creator )
  : Command( name ), creator_( creator )
{}

DeleteCommand::~DeleteCommand ( void )
{
  if ( creator_ != 0 ) delete creator_;
}

void DeleteCommand::execute ( void )
{
  creator_->remove();
}

void DeleteCommand::unexecute ( void )
{
  creator_->create();
}

void DeleteCommand::write ( QDomDocument* document ) const
{
  QDomElement delete_element = document->createElement( lC::STR::DELETE );
  QDomNode root_node = document->firstChild();
  if ( !root_node.isNull() )
    document->replaceChild( delete_element, root_node );
  else
    document->appendChild( delete_element );
  creator_->write( document );
}

RenameCommand::RenameCommand ( const QString& name, Model* model,
			       const DBURL& old_db_url, const DBURL& new_db_url)
  : Command( name ), model_( model ),
    old_db_url_( old_db_url ), new_db_url_( new_db_url )
{}

void RenameCommand::execute ( void )
{
  ModelItem* item = model_->lookup( old_db_url_ );
  if ( item == 0 ) {
    cerr << "Yikes, item did not exist!" << endl;
    return;
  }

  item->setName( new_db_url_.name() );
}

void RenameCommand::unexecute ( void )
{
  ModelItem* item = model_->lookup( new_db_url_ );
  if ( item == 0 ) {
    cerr << "Yikes, item did not exist!" << endl;
    return;
  }

  item->setName( old_db_url_.name() );
}

void RenameCommand::write ( QDomDocument* document ) const
{
  QDomElement rename_element = document->createElement( lC::STR::RENAME );
  QDomNode root_node = document->firstChild();
  if ( !root_node.isNull() )
    document->replaceChild( rename_element, root_node );
  else
    document->appendChild( rename_element );

  rename_element.setAttribute( lC::STR::OLD_URL, old_db_url_.toString(true) );
  rename_element.setAttribute( lC::STR::NEW_URL, new_db_url_.toString(true) );
}

MoveLinesCommand::MoveLinesCommand ( const QString& name,
				     Model* model )
  : Command( name ), model_( model ), xml_rep_( 0 )
{
  lines_.setAutoDelete( true );
}

MoveLinesCommand::~MoveLinesCommand ( void )
{
  if ( xml_rep_ != 0 ) delete xml_rep_;
}

bool MoveLinesCommand::merge ( Command* /*command*/ ) { return false; }

void MoveLinesCommand::add ( Space2D::ConstrainedLine* line, double old_offset )
{
  lines_.append( new MoveLine( line->dbURL(), old_offset, line->offset() ) );
}

void MoveLinesCommand::add ( QDomDocument* xml_rep )
{
  xml_rep_ = xml_rep;
}

void MoveLinesCommand::execute ( void )
{
  QPtrListIterator< MoveLine > i( lines_ );
  for ( ; i.current() != 0; ++i ) {
    Space2D::ConstrainedLine* line =
      dynamic_cast<Space2D::ConstrainedLine*>( model_->lookup( i.current()->db_url_ ) );
    if ( line == 0 ) {
      cerr << "Yikes, line did not exist!" << endl;
      continue;
    }
    line->setOffset( i.current()->new_offset_ );
  }

  if ( xml_rep_ != 0 ) {
    QDomElement reconstraints = xml_rep_->firstChild().toElement();
      
    QDomNode old_constraints = reconstraints.firstChild();
    QDomElement new_constraints = old_constraints.nextSibling().toElement();
      
    QDomNode n = new_constraints.firstChild();

    while ( !n.isNull() ) {
      QDomElement constrained_line = n.toElement();

      DBURL db_url = constrained_line.attribute( lC::STR::URL );

      Space2D::ConstrainedLine* line =
	dynamic_cast<Space2D::ConstrainedLine*>( model_->lookup( db_url ) );

      if ( line == 0 ) {
	cerr << "Yikes, line did not exist!" << endl;
	return;
      }

      QDomElement new_constraint = constrained_line.firstChild().toElement();

      line->setConstraint( new_constraint );

      n = n.nextSibling();
    }
  }
}

void MoveLinesCommand::unexecute ( void )
{
  if ( xml_rep_ != 0 ) {
    QDomElement reconstraints = xml_rep_->firstChild().toElement();
    QDomElement old_constraints = reconstraints.firstChild().toElement();
    QDomNode n = old_constraints.firstChild();

    while ( !n.isNull() ) {
      QDomElement constrained_line = n.toElement();

      DBURL db_url = constrained_line.attribute( lC::STR::URL );

      Space2D::ConstrainedLine* line =
	dynamic_cast<Space2D::ConstrainedLine*>( model_->lookup( db_url ) );

      if ( line == 0 ) {
	cerr << "Yikes, line did not exist!" << endl;
	return;
      }

      QDomElement old_constraint = constrained_line.firstChild().toElement();

      line->setConstraint( old_constraint );
	  
      n = n.nextSibling();
    }
  }

  QPtrListIterator< MoveLine > i( lines_ );
  for ( ; i.current() != 0; ++i ) {
    Space2D::ConstrainedLine* line =
      dynamic_cast<Space2D::ConstrainedLine*>( model_->lookup( i.current()->db_url_ ) );
    if ( line == 0 ) {
      cerr << "Yikes, line did not exist!" << endl;
      continue;
    }

    line->setOffset( i.current()->old_offset_ );
  }
}

void MoveLinesCommand::write ( QDomDocument* document ) const
{
  QDomElement move_element = document->createElement( lC::STR::MOVE_LINES );
  QDomNode root_node = document->firstChild();
  if ( !root_node.isNull() )
    document->replaceChild( move_element, root_node );
  else
    document->appendChild( move_element );

  QPtrListIterator< MoveLine > i( lines_ );
  for ( ; i.current() != 0; ++i ) {
    QDomElement line_element = document->createElement( lC::STR::MOVE_LINE );

    line_element.setAttribute( lC::STR::URL, i.current()->db_url_ );
    line_element.setAttribute( lC::STR::OLD_OFFSET,
			       lC::format( i.current()->old_offset_ ) );
    line_element.setAttribute( lC::STR::NEW_OFFSET,
			       lC::format( i.current()->new_offset_ ) );

    move_element.appendChild( line_element );
  }

  if ( xml_rep_ != 0 ) {
    QDomNode reconstraints = document->importNode( xml_rep_->firstChild(),true);
    move_element.appendChild( reconstraints );
  }
}

ReconstrainCommand::ReconstrainCommand ( const QString& name, Model* model,
					 QDomDocument* constraints )
  : Command( name ), model_( model ), xml_rep_( constraints )
{}

ReconstrainCommand::~ReconstrainCommand ( void )
{
  if ( xml_rep_ != 0 ) delete xml_rep_;
}

void ReconstrainCommand::execute ( void )
{
  QDomElement reconstraints = xml_rep_->firstChild().toElement();

  QDomNode old_constraints = reconstraints.firstChild();
  QDomElement new_constraints = old_constraints.nextSibling().toElement();

  QDomNode n = new_constraints.firstChild();

  while ( !n.isNull() ) {
    QDomElement constrained_line = n.toElement();

    DBURL db_url = constrained_line.attribute( lC::STR::URL );

    Space2D::ConstrainedLine* line =
      dynamic_cast<Space2D::ConstrainedLine*>( model_->lookup( db_url ) );

    if ( line == 0 ) {
      cerr << "Yikes, line did not exist!" << endl;
      return;
    }

    QDomElement new_constraint = constrained_line.firstChild().toElement();

    line->setConstraint( new_constraint );

    n = n.nextSibling();
  }
}

void ReconstrainCommand::unexecute ( void )
{
  QDomElement reconstraints = xml_rep_->firstChild().toElement();

  QDomElement old_constraints = reconstraints.firstChild().toElement();

  QDomNode n = old_constraints.firstChild();

  while ( !n.isNull() ) {
    QDomElement constrained_line = n.toElement();

    DBURL db_url = constrained_line.attribute( lC::STR::URL );

    Space2D::ConstrainedLine* line =
      dynamic_cast<Space2D::ConstrainedLine*>( model_->lookup( db_url ) );

    if ( line == 0 ) {
      cerr << "Yikes, line did not exist!" << endl;
      return;
    }

    QDomElement old_constraint = constrained_line.firstChild().toElement();

    line->setConstraint( old_constraint );

    n = n.nextSibling();
  }
}

void ReconstrainCommand::write ( QDomDocument* document ) const
{
  // This is suggestive a pattern which might be followed by the
  // other commands...
  *document = xml_rep_->cloneNode().toDocument();
}
