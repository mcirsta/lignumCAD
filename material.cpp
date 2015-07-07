/*
 * material.cpp
 *
 * Material classes implementations.
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
#include <qdom.h>
#include <qdir.h>
#include <qfile.h>
#include <qsettings.h>
#include <qtextcodec.h>
#if 0
#include <zlib.h>
#endif
#include "constants.h"
#include "configuration.h"
#include "usersettings.h"
#include "material.h"

#include <QtGlobal>

MaterialDatabase MaterialDatabase::material_database_;

MaterialDatabase& MaterialDatabase::instance ( void )
{
  return material_database_;
}

MaterialDatabase::MaterialDatabase ( void )
{
  QSettings settings;
  bool ok;
  //TODO OK bool ( check exists )
  QString home = settings.value( lC::Setting::HOME, lC::STR::HOME).toString();
  QDir materials_dir( QString( "%1%2v%3.%4%5%6" ).
		      arg( home ).arg( QDir::separator() ).
		      arg( lC::STR::VERSION_MAJOR ).arg( lC::STR::VERSION_MINOR ).
		      arg( QDir::separator() ).arg( lC::STR::MATERIALS ) );

  if ( !materials_dir.exists() ) return;
  //TODO init materials here
//  QDir images_dir( materials_dir );
//  if ( images_dir.cd( lC::STR::IMAGES ) )
//    QMimeSourceFactory::defaultFactory()->addFilePath( images_dir.absolutePath() );

//  const QFileInfoList* files = materials_dir.entryInfoList(lC::STR::XML_FILE_PATERN,
//						      QDir::Files | QDir::Readable);
//  QFileInfoListIterator file( *files );
//  for ( ; file.current() != 0; ++file ) {
//    QFile material_file( file.current()->absFilePath() );
//    QDomDocument doc( lC::STR::LIGNUMCAD );
 
//    if ( material_file.open( IO_ReadOnly ) ) {
//      QString errorString;
//      int line, column;
//      doc.setContent( &material_file, false, &errorString, &line, &column );
//      QDomElement root = doc.documentElement();
//      if ( root.tagName() == lC::STR::MATERIAL ) {
//	new Material( root );
//      }
//      material_file.close();
//    }
//  }
}

void MaterialDatabase::insertMaterial ( Material& material )
{
  materials_.insert( material.name(), material );
}

const Material& MaterialDatabase::material ( const QString& name )
{
  if ( name.isEmpty() ) {
      qFatal("material %s not found", qUtf8Printable( name ));
      return materials_.begin().value();
  }

  return materials_[ name ];
}

const Material& MaterialDatabase::materialCommon ( const QString& common_name )
{
  if ( common_name.isEmpty() )
    return materials_.begin().value();

  QHashIterator<int,Material> material( materials_ );

  for ( ; material.current() != 0; ++material ) {
    if ( material.current()->commonName() == common_name )
      return material.current();
  }

  return materials_.begin().value();
}

Material::Material ( const QDomElement& xml_rep )
{
  // First, extact the language specific items.

  QDomNodeList language_node_list =
    xml_rep.ownerDocument().elementsByTagName( "language" );

  uint i;
  for ( i = 0; i < language_node_list.count(); ++i ) {
    QDomElement language_element = language_node_list.item(i).toElement();

    QString delimiters = "_.@";
    QString locale = QTextCodec::locale();

    while ( true ) {
      if ( language_element.attribute( "locale" ) == locale )
	goto done;

      // Incrementally strip off extra localizations.
      int rightmost = 0;
      for ( int j = 0; j < delimiters.length(); j++ ) {
	int k = locale.findRev( delimiters[j] );
	if ( k > rightmost )
	  rightmost = k;
      }
      if ( rightmost == 0 )
	break;

      locale.truncate( rightmost );
    }
  }
 done:
  // If this translation doesn't exist, use US English.
  if ( i == language_node_list.count() ) {
    for ( i = 0; i < language_node_list.count(); ++i ) {
      QDomElement language_element = language_node_list.item(i).toElement();
      if ( language_element.attribute( "locale" ) == "en_US" )
	break;
    }
    // And if this didn't show up, then we need to signal an error...
    if ( i == language_node_list.count() )
      return;
  }

  QDomNode n = language_node_list.item(i).toElement().firstChild();
  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == "common-name" ) {
	name_ = e.text();
      }
      else if ( e.tagName() == "class" ) {
	class_ = e.text();
      }
      else if ( e.tagName() == "other-names" ) {
	other_names_ = e.text();
      }
    }
    n = n.nextSibling();
  }

  // Grab the rest of the (language independent) data.
  QString botanical_name, author;

  n = xml_rep.firstChild();

  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == "common-name" ) {
	common_name_ = e.text();
      }
      else if ( e.tagName() == "botanical-name" ) {
	botanical_name = e.text();
      }
      else if ( e.tagName() == "author" ) {
	author = e.text();
      }
      else if ( e.tagName() == "specific-gravity" ) {
	specific_gravity_ = e.attribute( lC::STR::VALUE ).toFloat();
      }
      else if ( e.tagName() == "compression-strength" ) {
	QDomNode c = e.firstChild();
	while ( !c.isNull() ) {
	  QDomElement ce = c.toElement();
	  if ( !ce.isNull() ) {
	    if ( ce.tagName() == "fspl-parallel" )
	      cmp_fspl_l_ = ce.attribute( lC::STR::VALUE ).toFloat();
	    else if ( ce.tagName() == "mcs-parallel" )
	      cmp_mcs_l_ = ce.attribute( lC::STR::VALUE ).toFloat();
	    else if ( ce.tagName() == "fspl-perpendicular" )
	      cmp_fspl_r_ = ce.attribute( lC::STR::VALUE ).toFloat();
	  }
	  c = c.nextSibling();
	}
      }
      else if ( e.tagName() == "tension-strength" ) {
	QDomNode c = e.firstChild();
	while ( !c.isNull() ) {
	  QDomElement ce = c.toElement();
	  if ( !ce.isNull() ) {
	    if ( ce.tagName() == "mts-perpendicular" )
	      ten_mts_r_ = ce.attribute( lC::STR::VALUE ).toFloat();
	  }
	  c = c.nextSibling();
	}
      }
      else if ( e.tagName() == "shear-strength" ) {
	QDomNode c = e.firstChild();
	while ( !c.isNull() ) {
	  QDomElement ce = c.toElement();
	  if ( !ce.isNull() ) {
	    if ( ce.tagName() == "mss-parallel" )
	      shr_mss_l_ = ce.attribute( lC::STR::VALUE ).toFloat();
	  }
	  c = c.nextSibling();
	}
      }
      else if ( e.tagName() == "static-bending-strength" ) {
	QDomNode c = e.firstChild();
	while ( !c.isNull() ) {
	  QDomElement ce = c.toElement();
	  if ( !ce.isNull() ) {
	    if ( ce.tagName() == "fspl" )
	      bend_fspl_ = ce.attribute( lC::STR::VALUE ).toFloat();
	    else if ( ce.tagName() == "mr" )
	      bend_mr_ = ce.attribute( lC::STR::VALUE ).toFloat();
	    else if ( ce.tagName() == "e" )
	      bend_e_ = ce.attribute( lC::STR::VALUE ).toFloat();
	  }
	  c = c.nextSibling();
	}
      }
      else if ( e.tagName() == "cosmetic" ) {
	QDomNode c = e.firstChild();
	while ( !c.isNull() ) {
	  QDomElement ce = c.toElement();
	  if ( !ce.isNull() ) {
	    if ( ce.tagName() == "color" )
	      color_.setNamedColor( ce.attribute( lC::STR::VALUE ) );
	    else if ( ce.tagName() == "face-grain" )
	      face_grain_file_ = ce.attribute( "file" );
	    else if ( ce.tagName() == "end-grain" )
	      end_grain_file_ = ce.attribute( "file" );
	    else if ( ce.tagName() == "edge-grain" )
	      edge_grain_file_ = ce.attribute( "file" );
	  }
	  c = c.nextSibling();
	}
      }
    }
    n = n.nextSibling();
  }

  // Construct the HTML representation of the botanical name.
  if ( botanical_name != "-" ) {
    botanical_ = QString( "<i>%1</i> %2" ).arg( botanical_name ).arg( author );
  }
  else
    botanical_ = "-";

  // If there is no name, then don't bother adding this material to the
  // database. Seems like an error...
  if ( common_name_.isNull() )
    return;

  MaterialDatabase::instance().insertMaterial( this );
}

#if 0
QImage Material::getImage ( const QDomElement& xml_rep )
{
  // Convert from ASCII encoded hexadecimal to binary.
  QString image_text = xml_rep.text();

  QByteArray image_bytes_zip( image_text.length()/2 );

  QTextIStream tis( &image_text );

  for ( ulong i = 0; i < image_text.length()/2; i++ ) {
    char a, b;
    tis >> a >> b;

    uchar r;
    if ( a <= '9' ) r = a - '0';
    else r = a - 'a' + 10;
    r <<= 4;
    if ( b <= '9' ) r += b - '0';
    else r += b - 'a' + 10;

    image_bytes_zip[i] = r;
  }

  ulong length = xml_rep.attribute( "length" ).toInt();
  QByteArray image_bytes( length );

  ::uncompress( (uchar*)image_bytes.data(), &length,
		(uchar*)image_bytes_zip.data(), image_bytes_zip.size() );

  return QImage( image_bytes );
}
#endif
