/* -*- c++ -*-
 * modelinfodialog.h
 *
 * Header for the ModelInfoDialog classes
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

/*!
  * This is a little gloss on entering a new model name. The file name string is
  * updated as the model name is typed.
  * \param text current text of model name widget.
  */
void ModelInfoDialog::modelNameEdit_textChanged( const QString & text )
{
  modelFileChooser->setFileName( text + ".lcad" );
  modelFileChooser->setEdited( true );
}


void ModelInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::display( tr( "<p><b>Edit Model Information</b></p>\
<p>Modify the basic information about the model. \
(This is sometimes called the <i>metadata</i>.) \
The metadata includes:\
<ul>\
<li>Model name</li>\
<li>File to save the model in (defaults to <i>ModelName</i>.lcad)</li>\
<li>Version and revision numbers</li>\
<li>Optional description of the model</li>\
<li>Date and time of creation</li>\
<li>Date and time of last modification</li>\
</ul>\
The creation and last modification times \
are updated automatically.</p>\
<p>When the fields are modified to your satisfaction, \
click the <b>OK</b> button (or press <b>Enter</b> or \
<b>Alt+O</b>) to \
apply the changes and exit the dialog.</p>\
<p>If you click the <b>Cancel</b> button \
(or press <b>ESC</b> or <b>Alt+C</b>), \
no changes to the model will be made.</p>" ) );
 
}
