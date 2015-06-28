/* -*- c++ -*-
 * constrainthistory.h
 *
 * Header for the ConstraintHistory class
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
#ifndef CONSTRAINTHISTORY_H
#define CONSTRAINTHISTORY_H

class QDomDocument;
class QDomElement;
/*!
 * As the user breaks constraints (generally by deleting a figure),
 * there can be a cascading effect if another figure's geometries' references
 * are deleted. This class captures all of the changes of constraints which are
 * caused by Cut (or Delete) so that they can be undone (by Undo),
 * or redone by Redo (and possibly by Paste (we'll see...))
 *
 * This class is a singleton like the Command History. "Taking" the
 * current set of ConstraintHistorys empties the list, readying it for the
 * next round.
 *
 * The QDomDocument member, reconstraints_, is used as a flag to determine
 * if there is anything of interest in this class. If the history()
 * method returns 0, then there is nothing of interest.
 */
class ConstraintHistory {
  QDomDocument* reconstraints_;

  static ConstraintHistory constraint_history_;

protected:
  ConstraintHistory ( void );
  ~ConstraintHistory ( void );

public:
  static ConstraintHistory& instance ( void );
  QDomDocument* history ( void ) { return reconstraints_; }

  void init ( void );
  void appendContraint ( const QDomElement& xml_rep );
  void appendUnconstraint ( const QDomElement& xml_rep );
  void takeConstraints ( QDomDocument* xml_doc );
};

#endif /* CONSTRAINTHISTORY_H */
