/* -*- c++ -*-
 * dimensioninfodialog.ui.h
 *
 * Header for DimensionInfoDialog class
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


void DimensionInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::display( tr("<p><b>Dimension Information</b></p>\
<p>This dialog allows you to edit selected properties of a \
dimension. These include:\
<ul>\
<li>Name</li>\
<li>Offset from its reference</li>\
<li>The side of the references on which the dimension is drawn (annotation side)</li>\
</ul>\
</p>\
<p>After the properties are adjusted as desired, click on the \
<b>OK</b> button (or press <b>Enter</b> or <b>Alt+O</b>) \
to accept your changes. Clicking the <b>Cancel</b> button \
(or pressing <b>ESC</b> or <b>Alt+C</b>) will exit the \
dialog without applying the changes.</p>" ) );
}
