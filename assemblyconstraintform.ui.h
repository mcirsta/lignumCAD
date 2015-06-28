/* -*- c++ -*-
 * assemblyconstraintform.ui.h
 *
 * Header for AssemblyConstraintForm class
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
/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/
#include <iostream>

void AssemblyConstraintForm::helpPushButton_clicked()
{
    QWhatsThis::display( tr( "<p>This dialog allows you to define the constraints by which a new model is located in space with respect to the assembly. A typical example would be the location of a table leg with respect to the skirts of the table.</p>\
<p>Defining the model location usually involves making three constraints. (It might help to think of the three constraints corresponding to the normal three spatial dimensions.) The constraints which can be made are:</p>\
<ul><li>Mate: The surfaces of two different models will touch and will face each other.</li>\
<li>Align: The surfaces of two different models will touch and face in the same direction.</li>\
<li>Mate Offset: The surfaces of two different models will face each other but will be separated by a given distance.</li>\
<li>Align Offset: The surfaces of two different models will face in the same direction but will be separated by a given distance</li></ul>\
<p>To create a constraint, select its button. When you click on it, the button will stay in the down position to indicate the type of constraint in progress. Next click on two surfaces: one on the new model and one on the completed assembly. If the two surfaces are compatible, the constraint will be recorded and the position of the new model will be updated to reflect the constraint.</p>\
<p>When the new model is completely constrained, usually after making three constraints, this dialog will disappear. If you decide not to place the new model, click <b>Cancel</b> (or select <b>Cancel Model Addition</b> from the context menu or type <b>Alt-C</b>) and the new model will be removed from the assembly.</p>" ) );
}
