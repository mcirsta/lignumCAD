/* -*- c++ -*-
 * materialdialog.h
 *
 * Header for the MaterialDialog class
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

void MaterialDialog::init()
{
  QDictIterator<Material> material = MaterialDatabase::instance().materials();

  for ( ; material.current() != 0; ++material ) {

    QListViewItem* class_item = MaterialList->firstChild();
    while ( class_item != 0 ) {
      if ( class_item->text( 0 ) == material.current()->materialClass() ) break;
      class_item = class_item->nextSibling();
    }
    if ( class_item == 0 ) {
      class_item = new QListViewItem( MaterialList,
				      material.current()->materialClass() );
      class_item->setSelectable( false );
      class_item->setOpen( true );
    }
    new QListViewItem( class_item, material.current()->name() );
  }
}


void MaterialDialog::MaterialList_selectionChanged( QListViewItem * item )
{
    if ( item == 0 ) {
	MaterialText->setText( QString::null );
	
	SolidColor->setPaletteBackgroundColor( colorGroup().background() );
    
	FaceGrainPixmap->setPixmap( QPixmap() );
	EndGrainPixmap->setPixmap( QPixmap() );
	EdgeGrainPixmap->setPixmap( QPixmap() );
	return;
    }
    
    Material* material = MaterialDatabase::instance().material( item->text( 0 ) );
    QString material_text;
    QString botanical_text = material->botanical() != "-" ? material->botanical() : tr( "none" );
    material_text = tr( "<p>Common name: %1</p><p>Botanical name: %2</p><p>Other names: %3</p><p>Specific gravity: %4" ).
		    arg( material->name() ).
		    arg( botanical_text ).
		    arg( material->otherNames() ).
		    arg( material->specificGravity() );
    
    QString compression_table;
    compression_table = tr( "<p align=\"center\">Compression Strengths</p><table border=\"1\"><tr><th>ll to Grain FSPL (PSI)</th><th>|| to Grain MCS (PSI)</th><th>T to Grain FSPL (PSI)</th></tr><tr><td>%1</td><td>%2</td><td>%3</td></tr></table>" ).
			arg( material->compStrengthFSPLPar() ).
			arg( material->compStrengthMCSPar() ).
			arg( material->compStrengthFSPLPerp() );
    QString tension_shear_table;
    tension_shear_table = tr( "<p align=\"center\">Tension and Shear Strengths</p><table border=\"1\"><tr><th>Tension T to Grain MTS (PSI)</th><th>Shear || to Grain MSS (PSI)</th></tr><tr><td>%1</td><td>%2</td></tr></table>" ).
			  arg( material->tenStrengthMTSPerp() ).
			  arg( material->shearStrengthMSSPar() );
    
    QString bend_table;
    bend_table = tr( "<p align=\"center\">Static Bending Strengths</p><table border=\"1\"><tr><th>FSPL (PSI)</th><th>MR (PSI)</th><th>E (10E6 PSI)</th></tr><tr><td>%1</td><td>%2</td><td>%3</td></tr></table>" ).
			arg( material->bendFSPL() ).
			arg( material->bendMR() ).
			arg( material->bendE() );
    
    QString annotations = tr( "<p>Notes:<br><font size=\"-1\">FSPL - fiber stress at proportional limit<br>MCS - maximum crushing strength<br>MTS - maximum tensile strength<br>MSS - maximum shear strength<br>MR - modulus of rupture<br>E - modulus of elasticity</font></p>" );
    
    material_text += compression_table + tension_shear_table + bend_table + annotations;
    
    MaterialText->setText( material_text );
    
    SolidColor->setPaletteBackgroundColor( material->color() );
    
    FaceGrainPixmap->setPixmap( lC::lookupPixmap( material->faceGrainFile() ) );
    EndGrainPixmap->setPixmap( lC::lookupPixmap( material->endGrainFile() ) );
    EdgeGrainPixmap->setPixmap( lC::lookupPixmap( material->edgeGrainFile() ) );
}


void MaterialDialog::setMaterial( const Material * material )
{
    if ( material == 0 ) {
	MaterialList->clearSelection(); // In single selection mode, the signal is not emitted.
	MaterialList_selectionChanged( 0 );
    }
    else {
	QListViewItem* class_item = MaterialList->firstChild();
	while ( class_item != 0 ) {
	    if ( class_item->text( 0 ) == material->materialClass() ) break;
	    class_item = class_item->nextSibling();
	}
	if ( class_item != 0 ) {
	    QListViewItem* material_item = class_item->firstChild();
	    while ( material_item != 0 ) {
		if ( material_item->text( 0 ) == material->name() ) {
		    MaterialList->setSelected( material_item, true );
		    return;
		}
		material_item = material_item->nextSibling();
	    }
	}
	MaterialList_selectionChanged( 0 );
    }
}


void MaterialDialog::buttonHelp_clicked()
{
    QWhatsThis::display( tr( "<p><b>Set Material for Part</b><p>\
<p>This dialog allows you to apply a material to the current part. Materials are \
grouped by type (e.g., solid wood, composites, and so on). Select a material from the \
list and some attributes of the material will be displayed in the Material Details box. \
Click the <b>OK</b> button (or press <b>Enter</b> or <b>Alt+O</b>) to apply the material \
to the part. If you click the \
<b>Cancel</b> button (or press <b>Escape</b> or <b>Alt+C</b>), the material currently \
assigned to the part will not be changed.</p>" ) );
}
