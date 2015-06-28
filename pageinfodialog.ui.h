/* -*- c++ -*-
 * pageinfodialog.h
 *
 * Header for the PageInfoDialog classes
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


void PageInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::display( tr( "<p><b>Rename Page</b></p>\
<p>This dialog lets you rename a page. In the current \
version of <i>lignumCAD</i>, there is no distinction between \
a page and its contents. For example, if you rename a Part \
page, you are also effectively renaming the Part itself. \
<font color=\"red\">(This is likely to change in a future \
version of the code.)</font></p>\
<p>Click the <b>OK</b> button \
(or press <b>Enter</b> or <b>Alt+O</b>) to apply the change. \
If you enter the same name as an existing page of the same type, \
<i>lignumCAD</i> will reject the change.</p>\
<p>You can cancel the rename by clicking the <b>Cancel</b> \
button (or by pressing <b>ESC</b> or <b>Alt+C</b>).</p>" ) );
}
