/* -*- c++ -*-
 * assemblyconfigdialog.ui.h
 *
 * Header for AssemblyConfigDialog class
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

void AssemblyConfigDialog::init()
{
    modelListView->setSorting( -1 );
}

void AssemblyConfigDialog::modelListView_selectionChanged( QListViewItem* item )
{
    if ( item != 0 )
	buttonOk->setEnabled( true );
    else
	buttonOk->setEnabled( false );
}


void AssemblyConfigDialog::buttonHelp_clicked()
{
   QWhatsThis::display( tr( "<p>Set the name for this assembly and then select the model (either a Part or an Assembly) to form the basis for the assembly. Its origin will coincide with the origin of the assembly.</p>\
<p>Note that the dialog cannot be completed until a model is selected. Canceling the dialog (either by clicking the <b>Cancel</b> button or pressing <b>ESC</b> or <b>Alt+C</b>) will cancel the creation of the assembly as well.</p>" ) );
}
