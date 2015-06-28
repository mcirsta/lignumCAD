/* -*- c++ -*-
 * rectangleinfodialog.h
 *
 * Header for the RectangleInfoDialog classes
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

void RectangleInfoDialog::init ()
{
      // Create invisible button groups connecting the Left/Right, Above/Below
      // radio buttons, respectively.
      QButtonGroup* h = new QButtonGroup( dimDisplaySideGroupBox, "H" );
      h->insert( leftButton );
      h->insert( rightButton );
      h->hide();
      QButtonGroup* v = new QButtonGroup( dimDisplaySideGroupBox, "V" );
      v->insert( aboveButton );
      v->insert( belowButton );
      v->hide();
      
      // Populate the edge stipple combo box. Maybe make a Designer widget?
      edgeStyleComboBox->insertItem( lC::lookupPixmap( "edge_solid.png" ), tr( "Solid" ) );
      edgeStyleComboBox->insertItem( lC::lookupPixmap( "edge_dash.png" ), tr( "Dash" ) );
      edgeStyleComboBox->insertItem( lC::lookupPixmap( "edge_dot.png" ), tr( "Dot" ) );
      edgeStyleComboBox->insertItem( lC::lookupPixmap( "edge_dashdot.png"), tr( "Dash Dot" ) );
      edgeStyleComboBox->insertItem( lC::lookupPixmap( "edge_dashdotdot.png"), tr( "Dash Dot Dot" ) );
}

void RectangleInfoDialog::rectangleStyleSlider_valueChanged( int style )
{
    switch ( style ) {
    case lC::Render::PARENT:
      defaultStyleLabel->setEnabled( true );
      edgeStyleLabel->setEnabled( false );
      edgeStyleComboBox->setEnabled( false );
      wireframeStyleLabel->setEnabled( false );
      wireframeStyleColorChooser->setEnabled( false );
      solidStyleLabel->setEnabled( false );
      solidStyleColorChooser->setEnabled( false );
      textureStyleLabel->setEnabled( false );
      textureStyleFileChooser->setEnabled( false );
      break;
    case lC::Render::STIPPLE:
      defaultStyleLabel->setEnabled( false );
      edgeStyleLabel->setEnabled( true );
      edgeStyleComboBox->setEnabled( true );
      wireframeStyleLabel->setEnabled( false );
      wireframeStyleColorChooser->setEnabled( false );
      solidStyleLabel->setEnabled( false );
      solidStyleColorChooser->setEnabled( false );
      textureStyleLabel->setEnabled( false );
      textureStyleFileChooser->setEnabled( false );
      break;
    case lC::Render::WIREFRAME:
      defaultStyleLabel->setEnabled( false );
      edgeStyleLabel->setEnabled( true );
      edgeStyleComboBox->setEnabled( true );
      wireframeStyleLabel->setEnabled( true );
      wireframeStyleColorChooser->setEnabled( true );
      solidStyleLabel->setEnabled( false );
      solidStyleColorChooser->setEnabled( false );
      textureStyleLabel->setEnabled( false );
      textureStyleFileChooser->setEnabled( false );
      break;
    case lC::Render::SOLID:
      defaultStyleLabel->setEnabled( false );
      edgeStyleLabel->setEnabled( true );
      edgeStyleComboBox->setEnabled( true );
      wireframeStyleLabel->setEnabled( true );
      wireframeStyleColorChooser->setEnabled( true );
      solidStyleLabel->setEnabled( true );
      solidStyleColorChooser->setEnabled( true );
      textureStyleLabel->setEnabled( false );
      textureStyleFileChooser->setEnabled( false );
      break;
    case lC::Render::TEXTURED:
      defaultStyleLabel->setEnabled( false );
      edgeStyleLabel->setEnabled( true );
      edgeStyleComboBox->setEnabled( true );
      wireframeStyleLabel->setEnabled( true );
      wireframeStyleColorChooser->setEnabled( true );
      solidStyleLabel->setEnabled( true );
      solidStyleColorChooser->setEnabled( true );
      textureStyleLabel->setEnabled( true );
      textureStyleFileChooser->setEnabled( true );
      break;
    }
}


void RectangleInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::display( tr("<p><b>Rectangle Information</b></p>\
<p>This dialog allows you to edit selected properties of a \
rectangle. These include:\
<ul>\
<li>Name</li>\
<li>Width and height</li>\
<li>X and Y origin offsets</li>\
<li>The side on which the width and height dimensions are displayed</li>\
<li>The rendering style</li>\
</ul>\
</p>\
<p>After the properties are adjusted as desired, click on the \
<b>OK</b> button (or press <b>Enter</b> or <b>Alt+O</b>) \
to accept your changes. Clicking the <b>Cancel</b> button \
(or pressing <b>ESC</b> or <b>Alt+C</b>) will exit the \
dialog without applying the changes.</p>" ) );
}
