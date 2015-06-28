/*
 * EMF: A library for generating ECMA-234 Enhanced Metafiles
 * Copyright (C) 2002 lignum Computing, Inc. <libemf@lignumcomputing.com>
 * $Id: libemf.cpp,v 1.5 2002/03/28 17:14:43 allen Exp $
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "libemf.h"

namespace EMF {

  const char PADDING::padding_[4] = { 0, 0, 0, 0 };

  /*!
   * Very simple routine to determine endian-ness of the machine. Note:
   * this calls abort() if the results of the test are nonsensical, e.g. if
   * the byte swapping is not consistent between short's and int's.
   * \return true if big-endian.
   */
  bool DATASTREAM::bigEndian ( void )
  {
    bool be16, be32;
    short ns = 0x1234;
    int nl = 0x12345678;

    unsigned char* p = (unsigned char*)&ns;
    be16 = *p == 0x12;

    p = (unsigned char*)&nl;
    if ( p[0] == 0x12 && p[1] == 0x34 && p[2] == 0x56 && p[3] == 0x78 )
      be32 = true;
    else if ( p[0] == 0x78 && p[1] == 0x56 && p[2] == 0x34 && p[3] == 0x12 )
      be32 = false;
    else
      be32 = !be16;

    if ( be32 != be16 ) {
      cerr << "endian-ness not consistent between short's and int's!" << endl;
      ::abort();
    }

    // Should also check that the sizes of all the other data structures
    // are as expected as well...

    return be32;
  }

  /*!
   * The single instance of the GlobalObjects database. Must be
   * constructed when the library is loaded.
   */
  GLOBALOBJECTS globalObjects;

  GLOBALOBJECTS::GLOBALOBJECTS ( void )
  {
    // Create the STOCK objects
    LOGBRUSH lbrush = { BS_SOLID, RGB(0xff,0xff,0xff), HS_HORIZONTAL };
    add( new BRUSH( &lbrush ) );
    lbrush.lbColor = RGB(0xb0,0xb0,0xb0);
    add( new BRUSH( &lbrush ) );
    lbrush.lbColor = RGB(0x80,0x80,0x80);
    add( new BRUSH( &lbrush ) );
    lbrush.lbColor = RGB(0x40,0x40,0x40);
    add( new BRUSH( &lbrush ) );
    lbrush.lbColor = RGB(0,0,0);
    add( new BRUSH( &lbrush ) );
    lbrush.lbStyle = BS_NULL;
    add( new BRUSH( &lbrush ) );

    LOGPEN lpen = { PS_SOLID, {0,0}, RGB(0xff,0xff,0xff) };
    add( new PEN( &lpen ) );
    lpen.lopnColor = RGB(0,0,0);
    add( new PEN( &lpen ) );
    lpen.lopnStyle = PS_NULL;
    add( new PEN( &lpen ) );
    // There is no 9-th stock object!
    add( new PEN( &lpen ) );

    LOGFONTW lfont = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0} };
    add( new FONT( &lfont ) );
    add( new FONT( &lfont ) );
    add( new FONT( &lfont ) );
    add( new FONT( &lfont ) );
    add( new FONT( &lfont ) );

    LOGPALETTE lpalette = { 0, 0, { {0, 0, 0, 0 } } };
    PALETTE* default_palette = new PALETTE( &lpalette );
    add( default_palette );

    add( new FONT( &lfont ) );
    add( new FONT( &lfont ) );

    // Get ready for reading a metafile, too.
    new_records[EMR_EOF] = new_eof;
    new_records[EMR_SETVIEWPORTORGEX] = new_setviewportorgex;
    new_records[EMR_SETWINDOWORGEX] = new_setwindoworgex;
    new_records[EMR_SETVIEWPORTEXTEX] = new_setviewportextex;
    new_records[EMR_SETWINDOWEXTEX] = new_setwindowextex;
    new_records[EMR_SCALEVIEWPORTEXTEX] = new_scaleviewportextex;
    new_records[EMR_SCALEWINDOWEXTEX] = new_scalewindowextex;
    new_records[EMR_MODIFYWORLDTRANSFORM] = new_modifyworldtransform;
    new_records[EMR_SETWORLDTRANSFORM] = new_setworldtransform;
    new_records[EMR_SETTEXTALIGN] = new_settextalign;
    new_records[EMR_SETTEXTCOLOR] = new_settextcolor;
    new_records[EMR_SETBKCOLOR] = new_setbkcolor;
    new_records[EMR_SETBKMODE] = new_setbkmode;
    new_records[EMR_SETPOLYFILLMODE] = new_setpolyfillmode;
    new_records[EMR_SETMAPMODE] = new_setmapmode;
    new_records[EMR_SELECTOBJECT] = new_selectobject;
    new_records[EMR_DELETEOBJECT] = new_deleteobject;
    new_records[EMR_MOVETOEX] = new_movetoex;
    new_records[EMR_LINETO] = new_lineto;
    new_records[EMR_ARC] = new_arc;
    new_records[EMR_ARCTO] = new_arcto;
    new_records[EMR_RECTANGLE] = new_rectangle;
    new_records[EMR_ELLIPSE] = new_ellipse;
    new_records[EMR_POLYLINE] = new_polyline;
    new_records[EMR_POLYLINE16] = new_polyline16;
    new_records[EMR_POLYGON] = new_polygon;
    new_records[EMR_POLYGON16] = new_polygon16;
    new_records[EMR_POLYPOLYGON] = new_polypolygon;
    new_records[EMR_POLYPOLYGON16] = new_polypolygon16;
    new_records[EMR_POLYBEZIER] = new_polybezier;
    new_records[EMR_POLYBEZIER16] = new_polybezier16;
    new_records[EMR_POLYBEZIERTO] = new_polybezierto;
    new_records[EMR_POLYBEZIERTO16] = new_polybezierto16;
    new_records[EMR_POLYLINETO] = new_polylineto;
    new_records[EMR_POLYLINETO16] = new_polylineto16;
    new_records[EMR_EXTTEXTOUTA] = new_exttextouta;
    new_records[EMR_SETPIXELV] = new_setpixelv;
    new_records[EMR_CREATEPEN] = new_createpen;
    new_records[EMR_EXTCREATEPEN] = new_extcreatepen;
    new_records[EMR_CREATEBRUSHINDIRECT] = new_createbrushindirect;
    new_records[EMR_EXTCREATEFONTINDIRECTW] = new_extcreatefontindirectw;
    new_records[EMR_FILLPATH] = new_fillpath;
    new_records[EMR_STROKEPATH] = new_strokepath;
    new_records[EMR_STROKEANDFILLPATH] = new_strokeandfillpath;
    new_records[EMR_BEGINPATH] = new_beginpath;
    new_records[EMR_ENDPATH] = new_endpath;
    new_records[EMR_CLOSEFIGURE] = new_closefigure;
    new_records[EMR_SAVEDC] = new_savedc;
    new_records[EMR_RESTOREDC] = new_restoredc;
    new_records[EMR_SETMETARGN] = new_setmetargn;
  }

  GLOBALOBJECTS::~GLOBALOBJECTS ( void )
  {
    // Just clean up for memory checkers' sakes
    std::vector<OBJECT*>::const_iterator igo = objects.begin();
    for ( ; igo != objects.end(); igo++ )
      if ( *igo != 0 ) delete *igo;
    objects.clear();
    new_records.clear();
  }
  /*!
   * Add an object to the global vector. The object's handle is simply its
   * index in the global object vector, which is computed by the very interesting
   * "difference between two iterators" method.
   * \param object pointer to a real instance of an object, not its handle.
   */
  HGDIOBJ GLOBALOBJECTS::add ( OBJECT* object )
  {
    HGDIOBJ handle;
    std::vector<OBJECT*>::iterator igo;

    // See if there are any free slots
    igo = std::find( objects.begin(), objects.end(), (OBJECT*)0 );

    if ( igo != objects.end() ) {
      handle = igo - objects.begin();
      *igo = object;
    }
    else {
      handle = objects.end() - objects.begin();
      objects.push_back( object );
    }

    // Stock objects have their top bit set
    if ( handle <= STOCK_LAST )
      handle |= ENHMETA_STOCK_OBJECT;

    object->handle = handle;

    return handle;
  }

  /*!
   * Look up a object by handle in the global object vector. Note: Stock
   * objects (like a gray brush or the black pen) have their high order
   * bit set, so this has to be masked out when using their handles.
   * \param handle the object's handle.
   * \return pointer to object.
   */
  OBJECT* GLOBALOBJECTS::find ( const HGDIOBJ handle )
  {
    if ( handle & ENHMETA_STOCK_OBJECT )
      return objects[ handle & (~ENHMETA_STOCK_OBJECT) ];
    else
      return objects[ handle ];
  }

  /*!
   * A call to the metafile function DeleteObject() allows a particular
   * object's handle to be reused, so some care has to be taken to erase
   * it.
   * \param object pointer to object to delete.
   */
  void GLOBALOBJECTS::remove ( const OBJECT* object )
  {
    std::vector<OBJECT*>::iterator igo;

    igo = std::find( objects.begin(), objects.end(), object );

    if ( igo != objects.end() ) {
      delete *igo;
      *igo = 0;
    }
  }
  /*!
   * See if we have a constructor for a record of the given type.
   * \param iType metarecord type.
   * \return pointer to "virtual" constructor.
   */
  METARECORDCTOR GLOBALOBJECTS::newRecord ( DWORD iType ) const
  {
    std::map<DWORD,METARECORDCTOR>::const_iterator
      new_record = new_records.find( iType );

    if ( new_record != new_records.end() )
      return new_record->second;
    else
      return 0;
  }

  METARECORD* GLOBALOBJECTS::new_eof ( DATASTREAM& ds )
  {
    return new EMF::EMREOF( ds );
  }

  METARECORD* GLOBALOBJECTS::new_setviewportorgex ( DATASTREAM& ds )
  {
    return new EMF::EMRSETVIEWPORTORGEX( ds );
  }

  METARECORD* GLOBALOBJECTS::new_setwindoworgex ( DATASTREAM& ds )
  {
    return new EMF::EMRSETWINDOWORGEX( ds );
  }

  METARECORD* GLOBALOBJECTS::new_setviewportextex ( DATASTREAM& ds )
  {
    return new EMF::EMRSETVIEWPORTEXTEX( ds );
  }

  METARECORD* GLOBALOBJECTS::new_setwindowextex ( DATASTREAM& ds )
  {
    return new EMF::EMRSETWINDOWEXTEX( ds );
  }

  METARECORD* GLOBALOBJECTS::new_scaleviewportextex ( DATASTREAM& ds )
  {
    return new EMF::EMRSCALEVIEWPORTEXTEX( ds );
  }

  METARECORD* GLOBALOBJECTS::new_scalewindowextex ( DATASTREAM& ds )
  {
    return new EMF::EMRSCALEWINDOWEXTEX( ds );
  }

  METARECORD* GLOBALOBJECTS::new_modifyworldtransform ( DATASTREAM& ds )
  {
    return new EMF::EMRMODIFYWORLDTRANSFORM( ds );
  }

  METARECORD* GLOBALOBJECTS::new_setworldtransform ( DATASTREAM& ds )
  {
    return new EMF::EMRSETWORLDTRANSFORM( ds );
  }

  METARECORD* GLOBALOBJECTS::new_settextalign ( DATASTREAM& ds )
  {
    return new EMF::EMRSETTEXTALIGN( ds );
  }

  METARECORD* GLOBALOBJECTS::new_settextcolor ( DATASTREAM& ds )
  {
    return new EMF::EMRSETTEXTCOLOR( ds );
  }

  METARECORD* GLOBALOBJECTS::new_setbkcolor ( DATASTREAM& ds )
  {
    return new EMF::EMRSETBKCOLOR( ds );
  }

  METARECORD* GLOBALOBJECTS::new_setbkmode ( DATASTREAM& ds )
  {
    return new EMF::EMRSETBKMODE( ds );
  }

  METARECORD* GLOBALOBJECTS::new_setpolyfillmode ( DATASTREAM& ds )
  {
    return new EMF::EMRSETPOLYFILLMODE( ds );
  }

  METARECORD* GLOBALOBJECTS::new_setmapmode ( DATASTREAM& ds )
  {
    return new EMF::EMRSETMAPMODE( ds );
  }

  METARECORD* GLOBALOBJECTS::new_selectobject ( DATASTREAM& ds )
  {
    return new EMF::EMRSELECTOBJECT( ds );
  }

  METARECORD* GLOBALOBJECTS::new_deleteobject ( DATASTREAM& ds )
  {
    return new EMF::EMRDELETEOBJECT( ds );
  }

  METARECORD* GLOBALOBJECTS::new_movetoex ( DATASTREAM& ds )
  {
    return new EMF::EMRMOVETOEX( ds );
  }

  METARECORD* GLOBALOBJECTS::new_lineto ( DATASTREAM& ds )
  {
    return new EMF::EMRLINETO( ds );
  }

  METARECORD* GLOBALOBJECTS::new_arc ( DATASTREAM& ds )
  {
    return new EMF::EMRARC( ds );
  }

  METARECORD* GLOBALOBJECTS::new_arcto ( DATASTREAM& ds )
  {
    return new EMF::EMRARCTO( ds );
  }

  METARECORD* GLOBALOBJECTS::new_rectangle ( DATASTREAM& ds )
  {
    return new EMF::EMRRECTANGLE( ds );
  }

  METARECORD* GLOBALOBJECTS::new_ellipse ( DATASTREAM& ds )
  {
    return new EMF::EMRELLIPSE( ds );
  }

  METARECORD* GLOBALOBJECTS::new_polyline ( DATASTREAM& ds )
  {
    return new EMF::EMRPOLYLINE( ds );
  }

  METARECORD* GLOBALOBJECTS::new_polyline16 ( DATASTREAM& ds )
  {
    return new EMF::EMRPOLYLINE16( ds );
  }

  METARECORD* GLOBALOBJECTS::new_polygon ( DATASTREAM& ds )
  {
    return new EMF::EMRPOLYGON( ds );
  }

  METARECORD* GLOBALOBJECTS::new_polygon16 ( DATASTREAM& ds )
  {
    return new EMF::EMRPOLYGON16( ds );
  }

  METARECORD* GLOBALOBJECTS::new_polypolygon ( DATASTREAM& ds )
  {
    return new EMF::EMRPOLYPOLYGON( ds );
  }

  METARECORD* GLOBALOBJECTS::new_polypolygon16 ( DATASTREAM& ds )
  {
    return new EMF::EMRPOLYPOLYGON16( ds );
  }

  METARECORD* GLOBALOBJECTS::new_polybezier ( DATASTREAM& ds )
  {
    return new EMF::EMRPOLYBEZIER( ds );
  }

  METARECORD* GLOBALOBJECTS::new_polybezier16 ( DATASTREAM& ds )
  {
    return new EMF::EMRPOLYBEZIER16( ds );
  }

  METARECORD* GLOBALOBJECTS::new_polybezierto ( DATASTREAM& ds )
  {
    return new EMF::EMRPOLYBEZIERTO( ds );
  }

  METARECORD* GLOBALOBJECTS::new_polybezierto16 ( DATASTREAM& ds )
  {
    return new EMF::EMRPOLYBEZIERTO16( ds );
  }

  METARECORD* GLOBALOBJECTS::new_polylineto ( DATASTREAM& ds )
  {
    return new EMF::EMRPOLYLINETO( ds );
  }

  METARECORD* GLOBALOBJECTS::new_polylineto16 ( DATASTREAM& ds )
  {
    return new EMF::EMRPOLYLINETO16( ds );
  }

  METARECORD* GLOBALOBJECTS::new_exttextouta ( DATASTREAM& ds )
  {
    return new EMF::EMREXTTEXTOUTA( ds );
  }

  METARECORD* GLOBALOBJECTS::new_setpixelv ( DATASTREAM& ds )
  {
    return new EMF::EMRSETPIXELV( ds );
  }

  METARECORD* GLOBALOBJECTS::new_createpen ( DATASTREAM& ds )
  {
    return new EMF::EMRCREATEPEN( ds );
  }

  METARECORD* GLOBALOBJECTS::new_extcreatepen ( DATASTREAM& ds )
  {
    return new EMF::EMREXTCREATEPEN( ds );
  }

  METARECORD* GLOBALOBJECTS::new_createbrushindirect ( DATASTREAM& ds )
  {
    return new EMF::EMRCREATEBRUSHINDIRECT( ds );
  }

  METARECORD* GLOBALOBJECTS::new_extcreatefontindirectw ( DATASTREAM& ds )
  {
    return new EMF::EMREXTCREATEFONTINDIRECTW( ds );
  }

  METARECORD* GLOBALOBJECTS::new_fillpath ( DATASTREAM& ds )
  {
    return new EMF::EMRFILLPATH( ds );
  }

  METARECORD* GLOBALOBJECTS::new_strokepath ( DATASTREAM& ds )
  {
    return new EMF::EMRSTROKEPATH( ds );
  }

  METARECORD* GLOBALOBJECTS::new_strokeandfillpath ( DATASTREAM& ds )
  {
    return new EMF::EMRSTROKEANDFILLPATH( ds );
  }

  METARECORD* GLOBALOBJECTS::new_beginpath ( DATASTREAM& ds )
  {
    return new EMF::EMRBEGINPATH( ds );
  }

  METARECORD* GLOBALOBJECTS::new_endpath ( DATASTREAM& ds )
  {
    return new EMF::EMRENDPATH( ds );
  }

  METARECORD* GLOBALOBJECTS::new_closefigure ( DATASTREAM& ds )
  {
    return new EMF::EMRCLOSEFIGURE( ds );
  }

  METARECORD* GLOBALOBJECTS::new_savedc ( DATASTREAM& ds )
  {
    return new EMF::EMRSAVEDC( ds );
  }

  METARECORD* GLOBALOBJECTS::new_restoredc ( DATASTREAM& ds )
  {
    return new EMF::EMRRESTOREDC( ds );
  }

  METARECORD* GLOBALOBJECTS::new_setmetargn ( DATASTREAM& ds )
  {
    return new EMF::EMRSETMETARGN( ds );
  }

  EMRCREATEPEN::EMRCREATEPEN ( PEN* pen, HGDIOBJ handle )
  {
    emr.iType = EMR_CREATEPEN;
    emr.nSize = sizeof( ::EMRCREATEPEN );
    ihPen = handle;
    lopn = *pen;
  }

  void EMRSELECTOBJECT::execute ( METAFILEDEVICECONTEXT* source, HDC dc ) const
  {
    // The primary subtlety here is that the handle of an object
    // in the metafile is not the same as the global handle in memory.
    // So, the source context has to keep a map of metafile handles
    // to global object handles. It is this global handle which the
    // destination dc wants to see. emf_handles is manipulated when
    // a Create* object record is executed.

    if ( !( ihObject & ENHMETA_STOCK_OBJECT ) )
      SelectObject( dc, source->emf_handles[ihObject] );
    else
      SelectObject( dc, ihObject );
  }

  void EMRDELETEOBJECT::execute ( METAFILEDEVICECONTEXT* source, HDC /*dc*/ )
    const
  {
    // The primary subtlety here is that the handle of an object
    // in the metafile is not the same as the global handle in memory.
    // So, the source context has to keep a map of metafile handles
    // to global object handles. It is this global handle which the
    // destination dc wants to see. emf_handles is manipulated when
    // a Create* object record is executed.

    if ( !( ihObject & ENHMETA_STOCK_OBJECT ) )
      DeleteObject( source->emf_handles[ihObject] );
  }

  EMRCREATEPEN::EMRCREATEPEN ( DATASTREAM& ds )
  {
    ds >> emr >> ihPen >> lopn;
  }

  void EMRCREATEPEN::execute ( METAFILEDEVICECONTEXT* source, HDC /*dc*/ ) const
  {
    HPEN pen = CreatePenIndirect( &lopn );
    // The primary subtlety here is that the handle of an object
    // in the metafile is not the same as the global handle in memory.
    // So, the source context has to keep a map of metafile handles
    // to global object handles. It is this global handle which the
    // destination dc wants to see. Use the current records handle
    // as the index to store the real object's handle in the source
    // source context's map.
    source->emf_handles[ihPen] = pen;
  }

  EMREXTCREATEPEN::EMREXTCREATEPEN ( EXTPEN* ext_pen, HGDIOBJ handle )
  {
    emr.iType = EMR_EXTCREATEPEN;
    emr.nSize = sizeof( ::EMREXTCREATEPEN ) - sizeof(DWORD) ;
    ihPen = handle;
    offBmi = emr.nSize;
    cbBmi = 0;
    offBits = emr.nSize;
    cbBits = 0;
    elp = *ext_pen;
  }

  void EMREXTCREATEPEN::execute ( METAFILEDEVICECONTEXT* source, HDC /*dc*/ )
    const
  {
    LOGBRUSH brush;

    brush.lbStyle = elp.elpBrushStyle;
    brush.lbColor = elp.elpColor;
    brush.lbHatch = elp.elpHatch;

    HPEN pen = ExtCreatePen ( elp.elpPenStyle, elp.elpWidth, &brush, 0, 0 );
    // The primary subtlety here is that the handle of an object
    // in the metafile is not the same as the global handle in memory.
    // So, the source context has to keep a map of metafile handles
    // to global object handles. It is this global handle which the
    // destination dc wants to see. Use the current records handle
    // as the index to store the real object's handle in the source
    // source context's map.
    source->emf_handles[ihPen] = pen;
  }

  EMREXTCREATEPEN::EMREXTCREATEPEN ( DATASTREAM& ds )
  {
    ds >> emr >> ihPen >> offBmi >> cbBmi >> offBits >> cbBits >> elp;
  }

  EMRCREATEBRUSHINDIRECT::EMRCREATEBRUSHINDIRECT ( BRUSH* brush, HGDIOBJ handle )
  {
    emr.iType = EMR_CREATEBRUSHINDIRECT;
    emr.nSize = sizeof( ::EMRCREATEBRUSHINDIRECT );
    ihBrush = handle;
    lb = *brush;
  }

  void EMRCREATEBRUSHINDIRECT::execute ( METAFILEDEVICECONTEXT* source,
					 HDC /*dc*/ ) const
  {
    HBRUSH brush = CreateBrushIndirect( &lb );
    // The primary subtlety here is that the handle of an object
    // in the metafile is not the same as the global handle in memory.
    // So, the source context has to keep a map of metafile handles
    // to global object handles. It this global handle which the
    // destination dc wants to see. Use the current records handle
    // as the index to store the real object's handle in the source
    // source context's map.
    source->emf_handles[ihBrush] = brush;
  }

  EMRCREATEBRUSHINDIRECT::EMRCREATEBRUSHINDIRECT ( DATASTREAM& ds )
  {
    ds >> emr >> ihBrush >> lb;
  }

  EMREXTCREATEFONTINDIRECTW::EMREXTCREATEFONTINDIRECTW ( FONT* font,
							 HGDIOBJ handle )
  {
    emr.iType = EMR_EXTCREATEFONTINDIRECTW;
    emr.nSize = ROUND_TO_LONG( sizeof( ::EMREXTCREATEFONTINDIRECTW ) );
    ihFont = handle;
    elfw = *font;
  }

  EMREXTCREATEFONTINDIRECTW::EMREXTCREATEFONTINDIRECTW ( DATASTREAM& ds )
  {
    ds >> emr >> ihFont >> elfw;
  }

  void EMREXTCREATEFONTINDIRECTW::execute ( METAFILEDEVICECONTEXT* source,
					    HDC /*dc*/ ) const
  {
    HFONT font = CreateFontIndirectW( &elfw.elfLogFont );
    // The primary subtlety here is that the handle of an object
    // in the metafile is not the same as the global handle in memory.
    // So, the source context has to keep a map of metafile handles
    // to global object handles. It this global handle which the
    // destination dc wants to see. Use the current records handle
    // as the index to store the real object's handle in the source
    // source context's map.
    source->emf_handles[ihFont] = font;
  }

  EMRCREATEPALETTE::EMRCREATEPALETTE ( PALETTE* palette, HGDIOBJ handle )
  {
    emr.iType = EMR_CREATEPALETTE;
    emr.nSize = sizeof( ::EMRCREATEPALETTE );
    ihPal = handle;
    lgpl = *palette;
  }

  void EMRCREATEPALETTE::execute ( METAFILEDEVICECONTEXT* /*source*/,
				   HDC /*dc*/ ) const
  {
    // Does nothing for now...
  }
} // close EMF namespace

extern "C" {
  /*!
   * Delete a device context. (Have to check to see if there is any reason
   * to ever call this.)
   * \param context device context to delete.
   * \return true if context successfully deleted.
   */
  BOOL DeleteDC ( HDC context )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::globalObjects.remove( dc );

    return TRUE;
  }
  /*!
   * Create an enhanced metafile on disk.
   * \param context pseudo device context.
   * \param filename ASCII filename of metafile.
   * \param size the size (really the position on the paper) of the plot image.
   * \param description description string for metafile (see EMF::METAFILE
   * constructor for details).
   * \return handle to a metafile device context.
   */
  HDC CreateEnhMetaFileA ( HDC referenceContext, LPCSTR filename, const RECT* size,
			   LPCSTR description )
  {
    // In this version, the library handles the file operations itself.
    // Note: No effort is made to preserve the contents of an existing
    // metafile!

    FILE* fp = 0;

    if ( filename ) {
      fp = fopen( filename, "w" );
      if ( fp == 0 ) return 0;
    }

    LPWSTR description_w = 0;

    if ( description ) {
      int description1_count = ::strlen( description );
      int description2_count = ::strlen( description + (description1_count + 1) );

      description_w = new WCHAR[ description1_count + description2_count + 3 ];

      for ( int i=0; i<=(description1_count + description2_count + 2); i++ )
	description_w[i] = (WCHAR)*description++;
    }

    HDC dc = CreateEnhMetaFileWithFILEW ( referenceContext, fp, size,
					  description_w );

    if ( description_w ) delete[] description_w;

    return dc;
  }
  /*!
   * Create an enhanced metafile on disk. This differs from CreateEnhMetaFileA
   * only in that the filename and description are made of wide characters.
   *
   * \param context pseudo device context.
   * \param filename ASCII filename of metafile.
   * \param size the size (really the position on the paper) of the plot image.
   * \param description description string for metafile (see EMF::METAFILE
   * constructor for details).
   * \return handle to a metafile device context.
   */
  HDC CreateEnhMetaFileW ( HDC referenceContext, LPCWSTR filename,
			   const RECT* size, LPCWSTR description )
  {
    // Well, the ANSI C library doesn't have any routines for opening
    // a file with a wide character filename, so, we have to convert
    // it back to ASCII and hope for the best.

    FILE* fp = 0;
    char* filename_a = 0;

    if ( filename ) {
      int n_char_w = 0;
      LPCWSTR w_tmp = filename;
      while ( *w_tmp++ ) n_char_w++;

      filename_a = new char[n_char_w+1];
      for ( int i=0; i<=n_char_w; i++ )
	filename_a[i] = *filename++;

      fp = fopen( filename_a, "w" );

      if ( fp == 0 ) return 0;
    }

    HDC dc = CreateEnhMetaFileWithFILEW ( referenceContext, fp, size,
					  description );

    if ( filename_a ) delete[] filename_a;

    return dc;
  }

  /*!
   * Many graphics programs want to open and close output files themselves. At
   * least, gnuplot and Grace do. So, supply the FILE pointer when creating
   * a metafile and let the plotting program's driver handle it.
   *
   * \param context pseudo device context.
   * \param fp open FILE pointer.
   * \param size the size (really the position on the paper) of the plot image.
   * \param description description string for metafile (see EMF::METAFILE
   * constructor for details).
   * \return handle to a metafile device context.
   */
  HDC CreateEnhMetaFileWithFILEA ( HDC referenceContext, FILE* fp,
				   const RECT* size, LPCSTR description )
  {
    // All this does is promote the ASCII strings to UNICODE (after a fashion)
    // and then use CreateEnhMetaFileWithFILEW

    LPWSTR description_w;

    if ( description ) {
      int description1_count = ::strlen( description );
      int description2_count = ::strlen( description + (description1_count + 1) );

      description_w = new WCHAR[ description1_count + description2_count + 3 ];

      for ( int i=0; i<=(description1_count + description2_count + 2); i++ )
	description_w[i] = (WCHAR)*description++;
    }
    else
      description_w = 0;

    HDC dc = CreateEnhMetaFileWithFILEW ( referenceContext, fp, size, description_w );

    if ( description_w ) delete[] description_w;

    return dc;
  }
  /*!
   * Many graphics programs want to open and close output files themselves. At
   * least, gnuplot and Grace do. So, supply the FILE pointer when creating
   * a metafile and let the plotting program's driver handle it. This differs
   * from CreateEnhMetaFileWithFILEA only in that the description string uses
   * wide characters.
   *
   * \param context pseudo device context.
   * \param fp open FILE pointer.
   * \param size the size (really the position on the paper) of the plot image.
   * \param description description string for metafile (see EMF::METAFILE
   * constructor for details).
   * \return handle to a metafile device context.
   */
  HDC CreateEnhMetaFileWithFILEW ( HDC referenceContext, FILE* fp, const RECT* size,
				   LPCWSTR description )
  {
    (void)referenceContext;
    EMF::METAFILEDEVICECONTEXT* dc =
      new EMF::METAFILEDEVICECONTEXT ( fp, size, description );

    return dc->handle;
  }

  /*!
   * The old 16-bit metafile constructor. Actually creates an Enhanced Metafile
   * anyway.
   * \param filename ASCII filename of metafile.
   * \return a handle to the metafile
   */
  HMETAFILE CreateMetaFileA ( LPCSTR filename )
  {
    return CreateEnhMetaFileA( 0, filename, 0, 0 );
  }
  /*!
   * If the metafile was opened with CreateEnhMetaFileA or CreateEnhMetaFileW,
   * use this routine to close the metafile.
   * \param context metafile device context.
   * \return a handle to the metafile.
   */
  HENHMETAFILE CloseEnhMetaFile ( HDC context )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return 0;

    EMF::EMREOF* eof = new EMF::EMREOF;

    dc->appendRecord( eof );

    // If it's a disk-based metafile, actually write it to disk

    if ( dc->fp ) {

      std::for_each( dc->records.begin(), dc->records.end(),
		     std::bind2nd( std::mem_fun1( &EMF::METARECORD::serialize ),
				   dc->ds ) );
      fclose( dc->fp );

      dc->fp = 0;
    }

    // There's no particular reason to distinguish between the context and
    // the metafile

    return (HENHMETAFILE)context;
  }

  /*!
   * If the metafile was opened with CreateEnhMetaFileA or CreateEnhMetaFileW,
   * use this routine to close the metafile (same as CloseEnhMetaFile)
   * \param context metafile device context.
   * \return a handle to the metafile.
   */
  HMETAFILE CloseMetaFile ( HDC context )
  {
    return CloseEnhMetaFile( context );
  }

  /*!
   * Close a metafile which was created with an already open FILE pointer.
   *
   * \param context handle of metafile context
   * \return handle to a metafile. I don't think there's anything you can do with
   * it, though.
   */
  HENHMETAFILE CloseEnhMetaFileWithFILE ( HDC context )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return 0;

    EMF::EMREOF* eof = new EMF::EMREOF;

    dc->appendRecord( eof );

    // If it's a disk-based metafile, actually write it to disk

    if ( dc->fp ) {

      std::for_each( dc->records.begin(), dc->records.end(),
		     std::bind2nd( std::mem_fun1( &EMF::METARECORD::serialize ),
				   dc->ds ) );
    }

    // There's no particular reason to distinguish between the context and
    // the metafile

    return (HENHMETAFILE)context;
  }
  /*!
   * Delete the metafile. Well, you can call this to clear out the memory
   * used by the metafile, but it's already been written to disk by the
   * time this is callable.
   * \param metafile handle to the metafile.
   * \return true if metafile successfully deleted.
   */
  BOOL DeleteEnhMetaFile ( HENHMETAFILE metafile )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find(metafile));

    if ( dc == 0 ) return FALSE;

    dc->deleteMetafile( );

    return TRUE;
  }
  /*!
   * Delete the metafile. Well, you can call this to clear out the memory
   * used by the metafile, but it's already been written to disk by the
   * time this is callable. Just calls DeleteEnhMetafile.
   * \param metafile handle to the metafile.
   * \return true if metafile successfully deleted.
   */
  BOOL DeleteMetaFile ( HMETAFILE metafile )
  {
    return DeleteEnhMetaFile( metafile );
  }

  /*!
   * Read an enhanced metafile from the given file.
   * \param filename ASCII file name.
   * \return metafile handle.
   */
  HENHMETAFILE GetEnhMetaFileA ( LPCSTR filename )
  {
    // All this does is promote the ASCII strings to UNICODE (after a fashion)
    // and then use GetEnhMetaFileW

    if ( filename == 0 || *filename == '\0' ) return 0;

    LPWSTR filename_w;

    int filename_count = ::strlen( filename );

    filename_w = new WCHAR[ filename_count + 1 ];

    for ( int i=0; i<=filename_count; i++ )
      filename_w[i] = (WCHAR)*filename++;

    HENHMETAFILE handle =  GetEnhMetaFileW( filename_w );

    delete[] filename_w;

    return handle;
  }

  /*!
   * Read an enhanced metafile from the given file.
   * \param filename WCHAR file name.
   * \return metafile handle.
   */
  HENHMETAFILE GetEnhMetaFileW ( LPCWSTR filename )
  {
    if ( filename == 0 || *filename == 0 ) return 0;

    // Unfortunately, the ANSI C library doesn't have a "wide" character
    // file fopen call, so we convert the file name back to ASCII and
    // hope for the best.

    LPSTR filename_a;
    int n_char_w = 0;

    LPCWSTR w_tmp = filename;
    while ( *w_tmp++ ) n_char_w++;

    filename_a = new char[n_char_w+1];
    for ( int i=0; i<=n_char_w; i++ )
      filename_a[i] = *filename++;

    FILE* fp;

    fp = fopen( filename_a, "r" );

    delete[] filename_a;

    if ( fp == 0 )
      return 0;

    // Create an implicit device context for this metafile. This
    // also creates an implicit metafile header.

    EMF::METAFILEDEVICECONTEXT* dc =
      new EMF::METAFILEDEVICECONTEXT ( fp, 0, 0 );

    // Read the header a little bit at a time.

    ::EMR emr;

    dc->ds >> emr;

    if ( emr.iType != EMR_HEADER ) {
      DeleteDC( dc->handle );
      return 0;
    }

    rewind( fp );

    dc->header->unserialize( dc->ds );

    // Strictly, the records are reattached so these must be recomputed:
    dc->header->nBytes = dc->header->nSize;
    dc->header->nRecords = 1;

    fseek( fp, emr.nSize, SEEK_SET );

    while ( true ) {
      long position = ftell( fp );

      // Peek at this record.

      dc->ds >> emr;

      if ( feof( fp ) ) break;

      if ( emr.nSize == 0 ) {
	cerr << "GetEnhMetaFileW error: record size == 0. cannot continue" << endl;
	fclose( fp );
	return 0;
      }

      long next_position = position + emr.nSize;

      // Avoid a giant switch statement here by using a map<> to store
      // the "virtual" constructors for each of the record types.
      EMF::METARECORDCTOR new_record = EMF::globalObjects.newRecord( emr.iType );

      if ( new_record != 0 ) {
	fseek( fp, position, SEEK_SET );
	EMF::METARECORD* record = new_record( dc->ds );

	dc->appendRecord( record );
      }
      else
	cerr << "GetEnhMetaFileW warning: read unknown record type " << emr.iType
	     << " of size " << emr.nSize << endl;

      // Regardless, position ourselves at the next record.
      fseek( fp, next_position, SEEK_SET );
    }

    fclose( fp );

    return dc->handle;
  }

  /*!
   * "Display" the enhanced metafile in the given device context. For the
   * purposes of this library, this re-executes each graphics command
   * in the given context.
   * \param context The device context in which to display the metafile.
   * \param metafile The (loaded) metafile to display.
   * \param frame A bounding rectangle in context for the metafile.
   * \param true if successfully copied.
   */
  BOOL PlayEnhMetaFile ( HDC context, HENHMETAFILE metafile,
			 const RECT* /*frame*/ )
  {
    EMF::METAFILEDEVICECONTEXT* source =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find(metafile));

    if ( source == 0 ) return FALSE;

    source->emf_handles.clear();

    for ( std::vector<EMF::METARECORD*>::iterator r = source->records.begin();
	  r != source->records.end();
	  r++ ) {
      (*r)->execute( source, context );
    }

    return TRUE;
  }

  /*!
   * This is not a ECMA-234 standard function. Edit (print) the contents of the
   * metafile to stdout. Also, this function does nothing if editing
   * wasn't enabled at ./configure time.
   * \param metafile The handle of a metafile (which implies that
   * the metafile has been closed already even though it will work
   * on an open metafile).
   */
  void EditEnhMetaFile ( HENHMETAFILE metafile )
  {
#ifdef ENABLE_EDITING

    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find(metafile));

    if ( dc == 0 ) return;

    std::for_each( dc->records.begin(), dc->records.end(),
		   std::mem_fun( &EMF::METARECORD::edit ) );
#endif /* ENABLE_EDITING */
  }

  /*!
   * Return the number of the last error. Since libEMF doesn't really
   * do any graphics, this is always returns OK. Also, since this doesn't
   * take any arguments, there appears to be only one, global, error number.
   * \return the number of the last error.
   */
  DWORD GetLastError ( )
  {
    return NO_ERROR;
  }
  /*!
   * Move the current point to the given position.
   * \param context handle to metafile context
   * \param x new x position.
   * \param y new y position.
   * \param point returns a copy of the old current point.
   * \return true if position successfully changed (can this fail?)
   */
  BOOL MoveToEx ( HDC context, INT x, INT y, LPPOINT point )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRMOVETOEX* movetoex = new EMF::EMRMOVETOEX( x, y );

    dc->appendRecord( movetoex );

    if ( point ) {
      *point = dc->point;
    }

    // Update the graphics state

    dc->point.x = x;
    dc->point.y = y;

    dc->mergePoint( dc->point );

    return TRUE;
  }
  /*!
   * Draw a straight line using the current PEN from the current point to the given
   * position.
   * \param context handle to metafile context
   * \param x x position of line end.
   * \param y y position of line end.
   * \return true if line is drawn (can this fail?)
   */
  BOOL LineTo ( HDC context, INT x, INT y )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRLINETO* lineto = new EMF::EMRLINETO( x, y );

    dc->appendRecord( lineto );

    dc->point.x = x;
    dc->point.y = y;

    dc->mergePoint( dc->point );

    return TRUE;
  }
  /*!
   * Retrieve the handle for a predefined graphics object. Stock objects
   * include (at least) the following:
   * \li WHITE_BRUSH
   * \li LTGRAY_BRUSH
   * \li GRAY_BRUSH
   * \li DKGRAY_BRUSH
   * \li BLACK_BRUSH
   * \li NULL_BRUSH
   * \li HOLLOW_BRUSH
   * \li WHITE_PEN
   * \li BLACK_PEN
   * \li NULL_PEN
   * \li OEM_FIXED_FONT
   * \li ANSI_FIXED_FONT
   * \li ANSI_VAR_FONT
   * \li SYSTEM_FONT
   * \li DEVICE_DEFAULT_FONT
   * \li DEFAULT_PALETTE
   * \li SYSTEM_FIXED_FONT
   * \li DEFAULT_GUI_FONT
   *
   * \param obj number of stock object.
   * \return handle of stock graphics object.
   */
  HGDIOBJ GetStockObject ( INT obj )
  {
    if ( obj >= 0 && obj <= STOCK_LAST ) {
      return ENHMETA_STOCK_OBJECT | obj;
    }
    return 0;
  }
  /*!
   * Make the given graphics object current.
   * \param context handle to metafile context.
   * \param obj handle of graphics object to make current.
   * \return the handle of the current graphics object which obj replaces.
   */
  HGDIOBJ SelectObject ( HDC context, HGDIOBJ obj )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return 0;

    EMF::GRAPHICSOBJECT* gobj =
      dynamic_cast<EMF::GRAPHICSOBJECT*>( EMF::globalObjects.find( obj ) );

    if ( gobj == 0 ) return 0;

    // Note: when an object is created, it cannot be written to the metafile
    // since those graphics calls don't have a device context associated with
    // them (why?). So, it is up to SelectObject to add a record for the object
    // it is about to select (unless it is a stock object?)

    // Another note: StarOffice apparently must have objects serially numbered,
    // so now the GRAPHICSOBJECT handle is NOT the same as the one which is
    // written to the metafile.

    // If it is NOT a stock object and it hasn't been written to the metafile
    // yet, invent a new handle for it and add it to the records to be written
    // out later

    HGDIOBJ handle;

    if ( !( obj & ENHMETA_STOCK_OBJECT ) ) {
      // Has this object been written to the (metafile) device context before?
      std::map<HDC,HGDIOBJ>::const_iterator c = gobj->contexts.find( context );

      if ( c != gobj->contexts.end() ) {
	handle = c->second;
      }
      else {
	handle = dc->nextHandle();
	dc->appendHandle( gobj->newEMR( context, handle ) );
      }
    }
    else {
      handle = obj;
    }

    EMF::EMRSELECTOBJECT* selectobject = new EMF::EMRSELECTOBJECT( handle );

    dc->appendRecord( selectobject );

    // Supposed to return the current value of whatever kind of object is selected

    switch ( gobj->getType( ) ) {
    case EMF::O_BRUSH:
      handle = dc->brush->handle;
      dc->brush = dynamic_cast< EMF::BRUSH* >( gobj );
      return handle;
    case EMF::O_FONT:
      handle = dc->font->handle;
      dc->font = dynamic_cast< EMF::FONT* >( gobj );
      return handle;
    case EMF::O_PEN:
      handle = dc->pen->handle;
      dc->pen = dynamic_cast< EMF::PEN* >( gobj );
      return handle;
    case EMF::O_PALETTE:
      handle = dc->palette->handle;
      dc->palette = dynamic_cast< EMF::PALETTE* >( gobj );
      return handle;
    default:
      return 0;
    }
  }
  /*!
   * Return information about the object. What does it want?
   * \param obj handle of graphics object to query
   * \param size the size of the buffer to fill in
   * \param buffer memory to hold returned object
   * \return the actual size of the object copied into buffer
   */
  INT GetObjectA ( HGDIOBJ obj, INT size, LPVOID buffer )
  {
    EMF::GRAPHICSOBJECT* gobj =
      dynamic_cast<EMF::GRAPHICSOBJECT*>( EMF::globalObjects.find( obj ) );

    if ( gobj == 0 ) return 0;

    switch ( gobj->getType( ) ) {
    case EMF::O_BRUSH:
      {
	LPLOGBRUSH brush = dynamic_cast< LPLOGBRUSH >( gobj );
	if ( brush != 0 && UINT( size ) >= sizeof( LOGBRUSH ) ) {
	  *(LPLOGBRUSH)buffer = *brush;
	  return sizeof( LOGBRUSH );
	}
      }
    case EMF::O_FONT:
      {
	LPEXTLOGFONTW fontw = dynamic_cast< LPEXTLOGFONTW >( gobj );
	if ( fontw ) {
	  if ( UINT( size ) >= sizeof( LOGFONTA ) ) {
	    ((LPLOGFONTA)buffer)->lfHeight = fontw->elfLogFont.lfHeight;
	    ((LPLOGFONTA)buffer)->lfWidth = fontw->elfLogFont.lfWidth;
	    ((LPLOGFONTA)buffer)->lfEscapement = fontw->elfLogFont.lfEscapement;
	    ((LPLOGFONTA)buffer)->lfOrientation = fontw->elfLogFont.lfOrientation;
	    ((LPLOGFONTA)buffer)->lfWeight = fontw->elfLogFont.lfWeight;
	    ((LPLOGFONTA)buffer)->lfItalic = fontw->elfLogFont.lfItalic;
	    ((LPLOGFONTA)buffer)->lfUnderline = fontw->elfLogFont.lfUnderline;
	    ((LPLOGFONTA)buffer)->lfStrikeOut = fontw->elfLogFont.lfStrikeOut;
	    ((LPLOGFONTA)buffer)->lfCharSet = fontw->elfLogFont.lfCharSet;
	    ((LPLOGFONTA)buffer)->lfOutPrecision = fontw->elfLogFont.lfOutPrecision;
	    ((LPLOGFONTA)buffer)->lfClipPrecision =
	      fontw->elfLogFont.lfClipPrecision;
	    ((LPLOGFONTA)buffer)->lfQuality = fontw->elfLogFont.lfQuality;
	    ((LPLOGFONTA)buffer)->lfPitchAndFamily =
	      fontw->elfLogFont.lfPitchAndFamily;
	    for ( int i = 0; i < LF_FACESIZE; i++ )
	      ((LPLOGFONTA)buffer)->lfFaceName[i] =
		(CHAR)fontw->elfLogFont.lfFaceName[i];
	  }
	  if ( UINT( size ) >= sizeof( EXTLOGFONTA ) ) {
	    for ( int i = 0; i < LF_FULLFACESIZE; i++ )
	      ((LPEXTLOGFONTA)buffer)->elfFullName[i] =	(CHAR)fontw->elfFullName[i];
	    for ( int i = 0; i < LF_FACESIZE; i++ )
	      ((LPEXTLOGFONTA)buffer)->elfStyle[i] = (CHAR)fontw->elfStyle[i];
	    ((LPEXTLOGFONTA)buffer)->elfVersion = fontw->elfVersion;
	    ((LPEXTLOGFONTA)buffer)->elfStyleSize = fontw->elfStyleSize;
	    ((LPEXTLOGFONTA)buffer)->elfMatch = fontw->elfMatch;
	    for ( int i = 0; i < ELF_VENDOR_SIZE; i++ )
	      ((LPEXTLOGFONTA)buffer)->elfVendorId[i] = (CHAR)fontw->elfVendorId[i];
	    ((LPEXTLOGFONTA)buffer)->elfCulture = fontw->elfCulture;
	    ((LPEXTLOGFONTA)buffer)->elfPanose = fontw->elfPanose;

	    return sizeof( EXTLOGFONTA );
	  }
	  else
	    return sizeof( LOGFONTA );
	}
      }
    case EMF::O_PEN:
      {
	LPLOGPEN pen = dynamic_cast< LPLOGPEN >( gobj );
	if ( pen != 0 && UINT( size ) >= sizeof( LOGPEN ) ) {
	  *(LPLOGPEN)buffer = *pen;
	  return sizeof( LOGPEN );
	}
      }
    case EMF::O_PALETTE:
      {
	LPLOGPALETTE palette = dynamic_cast< LPLOGPALETTE >( gobj );
	if ( palette != 0 && UINT( size ) >= sizeof( WORD ) ) {
	  *(LPWORD)buffer = palette->palNumEntries;
	  return sizeof( WORD );
	}
      }
    default:
      break;
    }

    return 0;
  }
  /*!
   * Delete the given graphics object. Note that, now, only those
   * contexts into which the object has been selected get a delete object
   * records. (Makes a lot more sense to me now.)
   * \param obj handle of graphics object to make current.
   * \return true if the object was successfully deleted.
   */
  BOOL DeleteObject ( HGDIOBJ obj )
  {
    if ( obj & ENHMETA_STOCK_OBJECT ) return FALSE;

    EMF::GRAPHICSOBJECT* gobj =
      dynamic_cast<EMF::GRAPHICSOBJECT*>( EMF::globalObjects.find( obj ) );

    if ( gobj == 0 ) return FALSE;

    // Add a deletion record for this object to every device context
    // into which it has been selected(!) [Now this makes sense.]

//      for ( std::vector<EMF::OBJECT*>::const_iterator o = EMF::globalObjects.begin();
//  	  o != EMF::globalObjects.end();
//  	  o++ ) {
//        if ( *o != 0 && (*o)->getType() == EMF::O_METAFILEDEVICECONTEXT ) {
//  	EMF::METAFILEDEVICECONTEXT* dc = (EMF::METAFILEDEVICECONTEXT*)*o;

    for ( std::map<HDC,HGDIOBJ>::const_iterator c = gobj->contexts.begin();
	  c != gobj->contexts.end();
	  c++ ) {

      HDC context = c->first;

      EMF::METAFILEDEVICECONTEXT* dc =
	dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find(context));

      if ( dc == 0 ) continue;

      EMF::EMRDELETEOBJECT* deleteobject = new EMF::EMRDELETEOBJECT( c->second );
	
      dc->appendRecord( deleteobject );

      // Reuse its metafile handle
      dc->clearHandle( c->second );

      // If this graphics object is current, then restore the default!

      switch ( gobj->getType( ) ) {
      case EMF::O_BRUSH:
	if ( (EMF::BRUSH*)gobj == dc->brush )
	  dc->brush = (EMF::BRUSH*)EMF::globalObjects.
	    find( BLACK_BRUSH | ENHMETA_STOCK_OBJECT );
	break;
      case EMF::O_FONT:
	if ( (EMF::FONT*)gobj == dc->font )
	  dc->font = (EMF::FONT*)EMF::globalObjects.
	    find( DEVICE_DEFAULT_FONT | ENHMETA_STOCK_OBJECT );
	break;
      case EMF::O_PEN:
	if ( (EMF::PEN*)gobj == dc->pen )
	  dc->pen = (EMF::PEN*)EMF::globalObjects.
	    find( BLACK_PEN | ENHMETA_STOCK_OBJECT );
	break;
      case EMF::O_PALETTE:
	if ( (EMF::PALETTE*)gobj == dc->palette )
	  dc->palette = (EMF::PALETTE*)EMF::globalObjects.
	    find( DEFAULT_PALETTE | ENHMETA_STOCK_OBJECT );
	break;
      default:
	break;
      }
    }

    EMF::globalObjects.remove( gobj );

    return TRUE;
  }
  /*!
   * Set the origin of the viewport. (Not entirely sure if this is honored
   * by StarOffice.)
   * \param context handle of metafile context.
   * \param x new x position of the viewport origin.
   * \param y new y position of the viewport origin.
   * \param point returns the previous viewport origin.
   * \return true if viewport origin successfully set.
   */
  BOOL SetViewportOrgEx ( HDC context, INT x, INT y, LPPOINT point )
  {
    (void)point;

    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRSETVIEWPORTORGEX* setviewportorgex =
      new EMF::EMRSETVIEWPORTORGEX( x, y );

    dc->appendRecord( setviewportorgex );

    if ( point != 0 )
      *point = dc->viewport_org;

    dc->viewport_org.x = x;
    dc->viewport_org.y = y;

    return TRUE;
  }
  /*!
   * Get the origin of the viewport.
   * \param context handle of metafile context.
   * \param point returns the current viewport origin.
   * \return true if viewport origin successfully returned.
   */
  BOOL GetViewportOrgEx ( HDC context, LPPOINT point )
  {
    if ( point != 0 ) {
      EMF::METAFILEDEVICECONTEXT* dc =
	dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find(context));

      if ( dc == 0 ) return FALSE;

      *point = dc->viewport_org;

      return TRUE;
    }

    return FALSE;
  }
  /*!
   * Set the origin of the window. Evidently, this means that a point drawn
   * at the given coordinates will appear at the Viewport origin.
   * \param context handle of metafile context.
   * \param x new x position of the window origin.
   * \param y new y position of the window origin.
   * \param point returns the previous window  origin (currently unused).
   * \return true if window origin successfully set.
   */
  BOOL SetWindowOrgEx ( HDC context, INT x, INT y, LPPOINT point )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRSETWINDOWORGEX* setwindoworgex =
      new EMF::EMRSETWINDOWORGEX( x, y );

    dc->appendRecord( setwindoworgex );

    if ( point != 0 )
      *point = dc->window_org;

    dc->window_org.x = x;
    dc->window_org.y = y;

    return TRUE;
  }
  /*!
   * Get the origin of the window.
   * \param context handle of metafile context.
   * \param point returns the current window  origin.
   * \return true if window origin successfully returned.
   */
  BOOL GetWindowOrgEx ( HDC context, LPPOINT point )
  {
    if ( point != 0 ) {
      EMF::METAFILEDEVICECONTEXT* dc =
	dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find(context));

      if ( dc == 0 ) return FALSE;

      *point = dc->window_org;

      return TRUE;
    }
    return FALSE;
  }
  /*!
   * Set the dimensions of the viewport. (Not honored by StarOffice!)
   * \param context handle of metafile context.
   * \param cx new width of the viewport.
   * \param cy new height of the viewport.
   * \param size returns the previous size of the viewport (currently unused).
   * \return true if viewport size successfully set.
   */
  BOOL SetViewportExtEx ( HDC context, INT cx, INT cy, LPSIZE size )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRSETVIEWPORTEXTEX* setviewportextex =
      new EMF::EMRSETVIEWPORTEXTEX( cx, cy );

    dc->appendRecord( setviewportextex );

    if ( size != 0 )
      *size = dc->viewport_ext;

    dc->viewport_ext.cx = cx;
    dc->viewport_ext.cy = cy;

    return TRUE;
  }
  /*!
   * Scale the dimensions of the viewport.
   * \param context handle of metafile context.
   * \param x_num numerator of x scale
   * \param x_den denominator of x scale
   * \param y_num numerator of y scale
   * \param y_den denominator of y scale
   * \return true if viewport size successfully set.
   */
  BOOL ScaleViewportExtEx ( HDC context, INT x_num, INT x_den,
			    INT y_num, INT y_den, LPSIZE size )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRSCALEVIEWPORTEXTEX* scaleviewportextex =
      new EMF::EMRSCALEVIEWPORTEXTEX( x_num, x_den, y_num, y_den );

    dc->appendRecord( scaleviewportextex );

    if ( size != 0 )
      *size = dc->viewport_ext;

    dc->viewport_ext.cx = dc->viewport_ext.cx * x_num / x_den;
    dc->viewport_ext.cy = dc->viewport_ext.cy * y_num / y_den;

    return TRUE;
  }
  /*!
   * Get the dimensions of the viewport.
   * \param context handle of metafile context.
   * \param size returns the current size of the viewport.
   * \return true if viewport size successfully returned.
   */
  BOOL GetViewportExtEx ( HDC context, LPSIZE size )
  {
    if ( size != 0 ) {
      EMF::METAFILEDEVICECONTEXT* dc =
	dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find(context));

      if ( dc == 0 ) return FALSE;

      *size = dc->viewport_ext;

      return TRUE;
    }

    return FALSE;
  }
  /*!
   * Set the dimensions of the window. (Not entirely sure if this is honored
   * by StarOffice.)
   * \param context handle of metafile context.
   * \param cx new width of the window.
   * \param cy new height of the window.
   * \param size returns the previous size of the window (currently unused).
   * \return true if window size successfully set.
   */
  BOOL SetWindowExtEx ( HDC context, INT cx, INT cy, LPSIZE size )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRSETWINDOWEXTEX* setwindowextex =
      new EMF::EMRSETWINDOWEXTEX( cx, cy );

    dc->appendRecord( setwindowextex );

    if ( size != 0 )
      *size = dc->window_ext;

    dc->window_ext.cx = cx;
    dc->window_ext.cy = cy;

    return TRUE;
  }
  /*!
   * Scale the dimensions of the window.
   * \param context handle of metafile context.
   * \param x_num numerator of x scale
   * \param x_den denominator of x scale
   * \param y_num numerator of y scale
   * \param y_den denominator of y scale
   * \return true if window size successfully set.
   */
  BOOL ScaleWindowExtEx ( HDC context, INT x_num, INT x_den,
			  INT y_num, INT y_den, LPSIZE size )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRSCALEWINDOWEXTEX* scalewindowextex =
      new EMF::EMRSCALEWINDOWEXTEX( x_num, x_den, y_num, y_den );

    dc->appendRecord( scalewindowextex );

    if ( size != 0 )
      *size = dc->window_ext;

    dc->window_ext.cx = dc->window_ext.cx * x_num / x_den;
    dc->window_ext.cy = dc->window_ext.cy * y_num / y_den;

    return TRUE;
  }
  /*!
   * Get the dimensions of the window.
   * \param context handle of metafile context.
   * \param size returns the previous size of the window.
   * \return true if window size successfully returned.
   */
  BOOL GetWindowExtEx ( HDC context, LPSIZE size )
  {
    if ( size != 0 ) {
      EMF::METAFILEDEVICECONTEXT* dc =
	dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find(context));

      if ( dc == 0 ) return FALSE;

      *size = dc->window_ext;

      return TRUE;
    }
    return FALSE;
  }
  /*!
   * Multiply the given Affine transform times the current global transformation.
   * (Not handled correctly by StarOffice 5.2, as near as I can tell, anyway.)
   * \param context handle of metafile context.
   * \param transform transformation modification array.
   * \param mode indicate whether the transformation should be pre- or post-
   * multiplied into the current transformation.
   * \return true if global transformation was successfully modified.
   */
  BOOL ModifyWorldTransform(HDC context, const XFORM *transform, DWORD mode )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRMODIFYWORLDTRANSFORM* modifyworldtransform =
      new EMF::EMRMODIFYWORLDTRANSFORM( transform, mode );

    dc->appendRecord( modifyworldtransform );

    return TRUE;
  }
  /*!
   * Replace the global transformation with the given transformation.
   * (Not handled correctly by StarOffice 5.2, as near as I can tell, anyway.)
   * \param context handle of metafile context.
   * \param transform new global transformation.
   * \return true if global transformation was successfully replaced.
   */
  BOOL SetWorldTransform(HDC context, const XFORM *transform )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRSETWORLDTRANSFORM* setworldtransform =
      new EMF::EMRSETWORLDTRANSFORM( transform );

    dc->appendRecord( setworldtransform );

    return TRUE;
  }
  /*!
   * Create a PEN, used to draw lines. Styles have (at least) the following
   * values:
   * \li PS_SOLID
   * \li PS_DASH
   * \li PS_DOT
   * \li PS_DASHDOT
   * \li PS_DASHDOTDOT
   * \li PS_NULL
   * \li PS_INSIDEFRAME
   * \li PS_USERSTYLE
   * \li PS_ALTERNATE
   * \param style the style of the new PEN.
   * \param width the width of the new PEN.
   * \param color the color of the new PEN.
   * \return handle of new PEN graphics object.
   */
  HPEN CreatePen ( INT style, INT width, COLORREF color )
  {
    LOGPEN lpen;

    lpen.lopnStyle = style;
    lpen.lopnWidth.x = width;
    lpen.lopnWidth.y = 0;
    lpen.lopnColor = color;

    return CreatePenIndirect( &lpen );
  }
  /*!
   * Create a PEN. Use the color from brush. WINE does not do anything
   * with style_count and style_bits, so I'm not sure what to do here.
   * \param style the style of the new PEN.
   * \param width the width of the new PEN.
   * \param brush use the color of the brush.
   * \param style_count the number of styles in style_bits.
   * \param style_bits list of styles.
   * \return handle of new PEN graphics object.
   */
  HPEN ExtCreatePen ( DWORD style, DWORD width, const LOGBRUSH* brush,
		      DWORD /*style_count*/, const DWORD* /*style_bits*/ )
  {
    EXTLOGPEN lpen;

    lpen.elpPenStyle = style;
    lpen.elpWidth = width;
    lpen.elpBrushStyle = brush->lbStyle;
    lpen.elpColor = brush->lbColor;
    lpen.elpHatch = brush->lbHatch;
    lpen.elpNumEntries = 0;
    lpen.elpStyleEntry[0] = 0;

    EMF::EXTPEN* pen = new EMF::EXTPEN( &lpen );

    return EMF::globalObjects.add( pen );
  }
  /*!
   * Create a PEN, used to draw lines.
   * \param lpen use a structure to define the PEN attributes.
   * \return handle to PEN graphics object.
   */
  HPEN CreatePenIndirect ( const LOGPEN* lpen )
  {
    EMF::PEN* pen = new EMF::PEN( lpen );

    return EMF::globalObjects.add( pen );
  }
  /*!
   * Create a BRUSH, used to fill polygons.
   * \param lbrush use a structure to define the BRUSH attributes.
   * \return handle to BRUSH graphics object.
   */
  HBRUSH CreateBrushIndirect ( const LOGBRUSH* lbrush )
  {
    EMF::BRUSH* brush = new EMF::BRUSH( lbrush );

    return EMF::globalObjects.add( brush );
  }
  /*!
   * Create a solid BRUSH, used to fill polygons.
   * \param color the color of the solid BRUSH.
   * \return handle to BRUSH graphics object.
   */
  HBRUSH CreateSolidBrush ( COLORREF color )
  {
    LOGBRUSH lbrush;

    lbrush.lbStyle = BS_SOLID;
    lbrush.lbColor = color;
    lbrush.lbHatch = HS_HORIZONTAL;

    return CreateBrushIndirect( &lbrush );
  }
  /*!
   * Set the subsequent alignment of drawn text. You can also pass a flag
   * indicating whether or not to update the current point to the end of the
   * text. Alignment may have the (sum of) values:
   * \li TA_NOUPDATECP
   * \li TA_UPDATECP
   * \li TA_LEFT
   * \li TA_RIGHT
   * \li TA_CENTER
   * \li TA_TOP
   * \li TA_BOTTOM
   * \li TA_BASELINE
   * \li TA_RTLREADING
   * \param context handle of metafile context.
   * \param alignment new text alignment.
   * \return previous text alignment value.
   */
  UINT SetTextAlign ( HDC context, UINT alignment )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return 0;

    EMF::EMRSETTEXTALIGN* settextalign = new EMF::EMRSETTEXTALIGN( alignment );

    dc->appendRecord( settextalign );

    UINT old_alignment = dc->text_alignment;

    dc->text_alignment = alignment;

    return old_alignment;
  }
  /*!
   * Set the text foreground color.
   * \param context handle of metafile context.
   * \param color text foreground color.
   * \return previous text foreground color.
   */
  COLORREF SetTextColor ( HDC context, COLORREF color )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return 0;

    EMF::EMRSETTEXTCOLOR* settextcolor = new EMF::EMRSETTEXTCOLOR( color );

    dc->appendRecord( settextcolor );

    COLORREF old_color = dc->text_color;

    dc->text_color = color;

    return old_color;
  }
  /*!
   * Set the background color. (As near as I can tell, StarOffice only uses
   * this for text background.)
   * \param context handle of metafile context.
   * \param color background color.
   * \return previous background color.
   */
  COLORREF SetBkColor ( HDC context, COLORREF color )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return 0;

    EMF::EMRSETBKCOLOR* setbkcolor = new EMF::EMRSETBKCOLOR( color );

    dc->appendRecord( setbkcolor );

    COLORREF old_color = dc->bk_color;

    dc->bk_color = color;

    return old_color;
  }
  /*!
   * Set the background mode. (As near as I can tell, StarOffice ignores this value.)
   * The choices for mode are:
   * \li TRANSPARENT
   * \li OPAQUE
   * \param context handle of metafile context.
   * \param mode background mode.
   * \return previous background mode.
   */
  INT SetBkMode ( HDC context, INT mode )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return 0;

    EMF::EMRSETBKMODE* setbkmode = new EMF::EMRSETBKMODE( mode );

    dc->appendRecord( setbkmode );

    UINT old_bk_mode = dc->bk_mode;

    dc->bk_mode = mode;

    return old_bk_mode;
  }
  /*!
   * Set the window mapping mode. (Unfortunately, as near as I can tell, StarOffice
   * ignores this value.) The choices for mode are:
   * \li MM_TEXT
   * \li MM_LOMETRIC
   * \li MM_HIMETRIC
   * \li MM_LOENGLISH
   * \li MM_HIENGLISH
   * \li MM_TWIPS
   * \li MM_ISOTROPIC
   * \li MM_ANISOTROPIC
   * \param context handle of metafile context.
   * \param mode window mapping mode.
   * \return previous window mapping mode.
   */
  INT SetMapMode ( HDC context, INT mode )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return 0;

    EMF::EMRSETMAPMODE* setmapmode = new EMF::EMRSETMAPMODE( mode );

    dc->appendRecord( setmapmode );

    UINT old_map_mode = dc->map_mode;

    dc->map_mode = mode;

    return old_map_mode;
  }
  /*!
   * Create a new FONT object. Presumably, the system tries to find a reasonable
   * approximation to all the requested attributes.
   * \param height typical glyph height.
   * \param width typical glyph width.
   * \param escapement (what is this?)
   * \param orientation angle, in degrees*10, of rendered string rotation.
   * \param weight weight has (at least) the following values:
   * \li FW_DONTCARE
   * \li FW_THIN
   * \li FW_EXTRALIGHT
   * \li FW_ULTRALIGHT
   * \li FW_LIGHT
   * \li FW_NORMAL
   * \li FW_REGULAR
   * \li FW_MEDIUM
   * \li FW_SEMIBOLD
   * \li FW_DEMIBOLD
   * \li FW_BOLD
   * \li FW_EXTRABOLD
   * \li FW_ULTRABOLD
   * \li FW_HEAVY
   * \li FW_BLACK
   * \param italic non-zero means try to find an italic version of the face.
   * \param underline non-zero means to underline the glyphs.
   * \param strike_out non-zero means to strike-out the glyphs.
   * \param charset select the character set from the following list:
   * \li ANSI_CHARSET
   * \li DEFAULT_CHARSET
   * \li SYMBOL_CHARSET
   * \li SHIFTJIS_CHARSET
   * \li HANGEUL_CHARSET
   * \li HANGUL_CHARSET
   * \li GB2312_CHARSET
   * \li CHINESEBIG5_CHARSET
   * \li GREEK_CHARSET
   * \li TURKISH_CHARSET
   * \li HEBREW_CHARSET
   * \li ARABIC_CHARSET
   * \li BALTIC_CHARSET
   * \li RUSSIAN_CHARSET
   * \li EE_CHARSET
   * \li EASTEUROPE_CHARSET
   * \li THAI_CHARSET
   * \li JOHAB_CHARSET
   * \li MAC_CHARSET
   * \li OEM_CHARSET
   * \param out_precision the precision of the face may have on of the
   * following values:
   * \li OUT_DEFAULT_PRECIS
   * \li OUT_STRING_PRECIS
   * \li OUT_CHARACTER_PRECIS
   * \li OUT_STROKE_PRECIS
   * \li OUT_TT_PRECIS
   * \li OUT_DEVICE_PRECIS
   * \li OUT_RASTER_PRECIS
   * \li OUT_TT_ONLY_PRECIS
   * \li OUT_OUTLINE_PRECIS
   * \param clip_precision the precision of glyph clipping may have one of the
   * following values:
   * \li CLIP_DEFAULT_PRECIS
   * \li CLIP_CHARACTER_PRECIS
   * \li CLIP_STROKE_PRECIS
   * \li CLIP_MASK
   * \li CLIP_LH_ANGLES
   * \li CLIP_TT_ALWAYS
   * \li CLIP_EMBEDDED
   * \param quality (subjective) quality of the font. Choose from the following
   * values:
   * \li DEFAULT_QUALITY
   * \li DRAFT_QUALITY
   * \li PROOF_QUALITY
   * \li NONANTIALIASED_QUALITY
   * \li ANTIALIASED_QUALITY
   * \param pitch_family the pitch and family of the FONT face. Add up the
   * desired values from this list:
   * \li DEFAULT_PITCH
   * \li FIXED_PITCH
   * \li VARIABLE_PITCH
   * \li MONO_FONT
   * \li FF_DONTCARE
   * \li FF_ROMAN
   * \li FF_SWISS
   * \li FF_MODERN
   * \li FF_SCRIPT
   * \li FF_DECORATIVE
   * \param name ASCII string containing the name of the FONT face.
   * \return handle of font.
   */
  HFONT CreateFontA ( INT height, INT width, INT escapement, INT orientation,
		      INT weight, DWORD italic, DWORD underline, DWORD strike_out,
		      DWORD charset, DWORD out_precision, DWORD clip_precision,
		      DWORD quality, DWORD pitch_family, LPCSTR name )
  {
    LPWSTR name_w = new WCHAR[ strlen( name ) + 1 ];
    int i;

    // Convert ASCII to UNICODE

    for (i=0; *name; i++)
      name_w[i] = *name++;
    name_w[i] = 0;

    HFONT font = CreateFontW( height, width, escapement, orientation, weight,
			      italic, underline, strike_out, charset, out_precision,
			      clip_precision, quality, pitch_family, name_w );

    delete[] name_w;

    return font;
  }
  /*!
   * Create a new FONT object. Presumably, the system tries to find a reasonable
   * approximation to all the requested attributes.
   * \param height typical glyph height.
   * \param width typical glyph width.
   * \param escapement (what is this?)
   * \param orientation angle, in degrees*10, of rendered string rotation.
   * \param weight weight has (at least) the following values:
   * \li FW_DONTCARE
   * \li FW_THIN
   * \li FW_EXTRALIGHT
   * \li FW_ULTRALIGHT
   * \li FW_LIGHT
   * \li FW_NORMAL
   * \li FW_REGULAR
   * \li FW_MEDIUM
   * \li FW_SEMIBOLD
   * \li FW_DEMIBOLD
   * \li FW_BOLD
   * \li FW_EXTRABOLD
   * \li FW_ULTRABOLD
   * \li FW_HEAVY
   * \li FW_BLACK
   * \param italic non-zero means try to find an italic version of the face.
   * \param underline non-zero means to underline the glyphs.
   * \param strike_out non-zero means to strike-out the glyphs.
   * \param charset select the character set from the following list:
   * \li ANSI_CHARSET
   * \li DEFAULT_CHARSET
   * \li SYMBOL_CHARSET
   * \li SHIFTJIS_CHARSET
   * \li HANGEUL_CHARSET
   * \li HANGUL_CHARSET
   * \li GB2312_CHARSET
   * \li CHINESEBIG5_CHARSET
   * \li GREEK_CHARSET
   * \li TURKISH_CHARSET
   * \li HEBREW_CHARSET
   * \li ARABIC_CHARSET
   * \li BALTIC_CHARSET
   * \li RUSSIAN_CHARSET
   * \li EE_CHARSET
   * \li EASTEUROPE_CHARSET
   * \li THAI_CHARSET
   * \li JOHAB_CHARSET
   * \li MAC_CHARSET
   * \li OEM_CHARSET
   * \param out_precision the precision of the face may have on of the
   * following values:
   * \li OUT_DEFAULT_PRECIS
   * \li OUT_STRING_PRECIS
   * \li OUT_CHARACTER_PRECIS
   * \li OUT_STROKE_PRECIS
   * \li OUT_TT_PRECIS
   * \li OUT_DEVICE_PRECIS
   * \li OUT_RASTER_PRECIS
   * \li OUT_TT_ONLY_PRECIS
   * \li OUT_OUTLINE_PRECIS
   * \param clip_precision the precision of glyph clipping may have one of the
   * following values:
   * \li CLIP_DEFAULT_PRECIS
   * \li CLIP_CHARACTER_PRECIS
   * \li CLIP_STROKE_PRECIS
   * \li CLIP_MASK
   * \li CLIP_LH_ANGLES
   * \li CLIP_TT_ALWAYS
   * \li CLIP_EMBEDDED
   * \param quality (subjective) quality of the font. Choose from the following
   * values:
   * \li DEFAULT_QUALITY
   * \li DRAFT_QUALITY
   * \li PROOF_QUALITY
   * \li NONANTIALIASED_QUALITY
   * \li ANTIALIASED_QUALITY
   * \param pitch_family the pitch and family of the FONT face. Add up the
   * desired values from this list:
   * \li DEFAULT_PITCH
   * \li FIXED_PITCH
   * \li VARIABLE_PITCH
   * \li MONO_FONT
   * \li FF_DONTCARE
   * \li FF_ROMAN
   * \li FF_SWISS
   * \li FF_MODERN
   * \li FF_SCRIPT
   * \li FF_DECORATIVE
   * \param name wide character string containing the name of the FONT face.
   * \return handle of font.
   */
  HFONT CreateFontW ( INT height, INT width, INT escapement, INT orientation,
		      INT weight, DWORD italic, DWORD underline, DWORD strike_out,
		      DWORD charset, DWORD out_precision, DWORD clip_precision,
		      DWORD quality, DWORD pitch_family, LPCWSTR name )
  {
    LOGFONTW lfont;

    lfont.lfHeight = height;
    lfont.lfWidth = width;
    lfont.lfEscapement = escapement;
    lfont.lfOrientation = orientation;
    lfont.lfWeight = weight;
    lfont.lfItalic = italic;
    lfont.lfUnderline = underline;
    lfont.lfStrikeOut = strike_out;
    lfont.lfCharSet = charset;
    lfont.lfOutPrecision = out_precision;
    lfont.lfClipPrecision = clip_precision;
    lfont.lfQuality = quality;
    lfont.lfPitchAndFamily = pitch_family;

    WCHAR* face = &lfont.lfFaceName[0];

    memset( face, 0, sizeof lfont.lfFaceName );
    while ( *name )
      *face++ = *name++;

    return CreateFontIndirectW( &lfont );
  }
  /*!
   * Create a new FONT using the logical(?) font specification.
   * \param lfont pointer to logical font description.
   * \return handle of FONT object.
   */
  HFONT CreateFontIndirectW ( const LOGFONTW* lfont )
  {
    EMF::FONT* font = new EMF::FONT( lfont );

    return EMF::globalObjects.add( font );
  }
  /*!
   * Create a new FONT using the logical(?) font specification.
   * \param lfont pointer to logical font description.
   * \return handle of FONT object.
   */
  HFONT CreateFontIndirectA ( const LOGFONTA* lfonta )
  {
    LOGFONTW lfontw;
    memset( lfontw.lfFaceName, 0, sizeof lfontw.lfFaceName );

    // Convert ASCII to UNICODE

    int i;
    const char* name = lfonta->lfFaceName;
    for (i=0; *name; i++)
      lfontw.lfFaceName[i] = *name++;
    lfontw.lfFaceName[i] = 0;

    lfontw.lfHeight = lfonta->lfHeight;
    lfontw.lfWidth = lfonta->lfWidth;
    lfontw.lfEscapement = lfonta->lfEscapement;
    lfontw.lfOrientation = lfonta->lfOrientation;
    lfontw.lfWeight = lfonta->lfWeight;
    lfontw.lfItalic = lfonta->lfItalic;
    lfontw.lfUnderline = lfonta->lfUnderline;
    lfontw.lfStrikeOut = lfonta->lfStrikeOut;
    lfontw.lfCharSet = lfonta->lfCharSet;
    lfontw.lfOutPrecision = lfonta->lfOutPrecision;
    lfontw.lfClipPrecision = lfonta->lfClipPrecision;
    lfontw.lfQuality = lfonta->lfQuality;
    lfontw.lfPitchAndFamily = lfonta->lfPitchAndFamily;

    return CreateFontIndirectW( &lfontw );
  }
  /*!
   * Draw a string of text at the given position using the current FONT and
   * other text attributes.
   * \param context handle to metafile context.
   * \param x x position of text.
   * \param y y position of text.
   * \param string ASCII text string to render.
   * \param count number of characters in string to draw.
   * \return true of string successfully drawn.
   */
  BOOL TextOutA ( HDC context, INT x, INT y, LPCSTR string, INT count )
  {
    return ExtTextOutA( context, x, y, 0, 0, string, count, 0 );
  }
  /*!
   * Draw a string of text at the given position and using the additional
   * placement specifications.
   * \param context handle to metafile context.
   * \param x x position of text.
   * \param y y position of text.
   * \param flags additional rendering information, sum of the following attributes:
   * \li ETO_GRAYED
   * \li ETO_OPAQUE
   * \li ETO_CLIPPED
   * \li ETO_GLYPH_INDEX
   * \li ETO_RTLREADING
   * \li ETO_IGNORELANGUAGE
   * \param rect optional clipping rectangle for text(?)
   * \param string ASCII text string to render.
   * \param count number of characters in string to draw.
   * \param dx returns a list of where each glyph was drawn(?)
   * \return true if text was successfully rendered.
   */
  BOOL ExtTextOutA ( HDC context, INT x, INT y, UINT flags,
		     const RECT* rect, LPCSTR string, UINT count,
		     const INT* dx )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { 0, 0, -1, -1 };

    EMRTEXT text;
    text.ptlReference.x = x;
    text.ptlReference.y = y;
    text.nChars = count;
    text.fOptions = flags;
    text.rcl = bounds;

    if ( rect ) {
      bounds.left = rect->left;
      bounds.top = rect->top;
      bounds.right = rect->right;
      bounds.bottom = rect->bottom;
    }

    EMF::EMREXTTEXTOUTA* exttextouta =
      new EMF::EMREXTTEXTOUTA( &bounds, GM_COMPATIBLE, 1.0F, 1.0F, &text,
			       string, dx );

    dc->appendRecord( exttextouta );

    return TRUE;
  }
#if 0
  /*!
   * Draw a string of text at the given position and using the additional
   * placement specifications.
   * \param context handle to metafile context.
   * \param x x position of text.
   * \param y y position of text.
   * \param string Wide character text string to render.
   * \param count number of characters in string to draw.
   * \return true if text was successfully rendered.
   */
  BOOL TextOutW ( HDC context, INT x, INT y, LPCWSTR string, INT count )
  {
    return ExtTextOutW( context, x, y, string, count );
  }
#endif
  /*!
   * Draw an arc. Not sure what the specification here means, though.
   * \param left x position of left edge of arc box.
   * \param top y position of top edge of arc box.
   * \param right x position of right edge of arc box.
   * \param bottom y position bottom edge of arc box.
   * \param xstart x position of arc start.
   * \param ystart y position of arc start.
   * \param xend x position of arc end.
   * \param yend y position of arc end.
   * \return true if arc was successfully rendered.
   */
  BOOL Arc ( HDC context, INT left, INT top, INT right, INT bottom, INT xstart,
	     INT ystart, INT xend, INT yend )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRARC* arc = new EMF::EMRARC( left, top, right, bottom, xstart,
					ystart, xend, yend );

    dc->appendRecord( arc );

    // Update graphics state
    dc->mergePoint( left, top );
    dc->mergePoint( right, bottom );

    return TRUE;
  }
  /*!
   * Draw another arc. Not sure what the specification here means, though.
   * \param left x position of left edge of arc box.
   * \param top y position of top edge of arc box.
   * \param right x position of right edge of arc box.
   * \param bottom y position bottom edge of arc box.
   * \param xstart x position of arc start.
   * \param ystart y position of arc start.
   * \param xend x position of arc end.
   * \param yend y position of arc end.
   * \return true if arc was successfully rendered.
   */
  BOOL ArcTo ( HDC context, INT left, INT top, INT right, INT bottom, INT xstart,
	       INT ystart, INT xend, INT yend )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRARCTO* arcto = new EMF::EMRARCTO( left, top, right, bottom, xstart,
					      ystart, xend, yend );

    dc->appendRecord( arcto );

    // Update graphics state
    dc->mergePoint( left, top );
    dc->mergePoint( right, bottom );

    return TRUE;
  }
  /*!
   * Draw a rectangle using the current brush.
   * \param context handle to metafile context.
   * \param left x position of left side of rectangle.
   * \param top y position of top side of rectangle.
   * \param right x position of right edge of rectangle.
   * \param bottom y position of bottom edge of rectangle.
   * \return true if rectangle was successfully rendered.
   */
  BOOL Rectangle ( HDC context, INT left, INT top, INT right, INT bottom )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRRECTANGLE* rectangle = new EMF::EMRRECTANGLE( left, top, right, bottom);

    dc->appendRecord( rectangle );

    // Update graphics state
    dc->mergePoint( left, top );
    dc->mergePoint( right, bottom );

    return TRUE;
  }
  /*!
   * Draw an ellipse. (I have no idea how the ellipse is defined!)
   * \param context handle to metafile context.
   * \param left x position of left extrema of ellipse.
   * \param top y position of top extrema of ellipse.
   * \param right x position of right extrema of ellipse.
   * \param bottom y position of bottom extrema of ellipse.
   * \return true if the ellipse was successfully rendered.
   */
  BOOL Ellipse ( HDC context, INT left, INT top, INT right, INT bottom )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRELLIPSE* ellipse = new EMF::EMRELLIPSE( left, top, right, bottom );

    dc->appendRecord( ellipse );

    // Update graphics state
    dc->mergePoint( left, top );
    dc->mergePoint( right, bottom );

    return TRUE;
  }
  /*!
   * Draw a Bezier curve.
   * \param context handle to metafile context.
   * \param point array of points to draw.
   * \param n number of points in the array.
   * \return true if Bezier curve is successfully rendered.
   */
  BOOL PolyBezier ( HDC context, const POINT* points, DWORD n )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };

    // An optimization: if all the values in points are representable in
    // 16-bits, then we can use the smaller 16-bit POLYBEZIER16 structure.
    // So, as we update graphics state, try to determine if we've only
    // got short ints.
    const POINT* pnt_ptr = points;
    bool shorts_only = true;

    for ( DWORD i = 0; i < n; i++ ) {
      if ( pnt_ptr->x > SHRT_MAX || pnt_ptr->x < SHRT_MIN ||
	   pnt_ptr->y > SHRT_MAX || pnt_ptr->y < SHRT_MIN )
	shorts_only = false;

      if ( pnt_ptr->x < bounds.left ) bounds.left = pnt_ptr->x;
      if ( pnt_ptr->x > bounds.right ) bounds.right = pnt_ptr->x;
      if ( pnt_ptr->y < bounds.top ) bounds.top = pnt_ptr->y;
      if ( pnt_ptr->y > bounds.bottom ) bounds.bottom = pnt_ptr->y;

      dc->mergePoint( *pnt_ptr++ );
    }

    if ( shorts_only ) {
      EMF::EMRPOLYBEZIER16* polybezier16 =
	new EMF::EMRPOLYBEZIER16( &bounds, points, n );

      dc->appendRecord( polybezier16 );
    }
    else {
      EMF::EMRPOLYBEZIER* polybezier = new EMF::EMRPOLYBEZIER( &bounds, points, n );

      dc->appendRecord( polybezier );
    }

    return TRUE;
  }
  /*!
   * Draw a Bezier curve using 16-bit points.
   * \param context handle to metafile context.
   * \param point array of points to draw.
   * \param n number of points in the array.
   * \return true if Bezier curve is successfully rendered.
   */
  BOOL PolyBezier16 ( HDC16 context, const POINT16* points, INT16 n )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };
    const POINT16* pnt_ptr = points;

    for ( INT i = 0; i < n; i++, pnt_ptr++ ) {
      if ( pnt_ptr->x < bounds.left ) bounds.left = pnt_ptr->x;
      if ( pnt_ptr->x > bounds.right ) bounds.right = pnt_ptr->x;
      if ( pnt_ptr->y < bounds.top ) bounds.top = pnt_ptr->y;
      if ( pnt_ptr->y > bounds.bottom ) bounds.bottom = pnt_ptr->y;

      dc->mergePoint( pnt_ptr->x, pnt_ptr->y );
    }

    EMF::EMRPOLYBEZIER16* polybezier16 =
      new EMF::EMRPOLYBEZIER16( &bounds, points, n );

    dc->appendRecord( polybezier16 );

    // Update graphics state
    for ( INT16 i = 0; i < n; i++ )
      dc->mergePoint( points[i].x, points[i].y );

    return TRUE;
  }

  // Evidently, this is only valid for a closed metafile, because, otherwise,
  // you can't get an HENHMETAFILE
  /*!
   * Retrieve the handle of a *closed* metafile.
   * \param metafile metafile handle returned by CloseEnhMetaFile.
   * \param sizeof_enhmetaheader the size of the metafile file header structure
   * passed in.
   * \param metaheader pointer to memory where the metafile header is to be
   * stored
   * \return actual size of metafile header
   */
  UINT GetEnhMetaFileHeader( HENHMETAFILE metafile, const UINT sizeof_enhmetaheader,
			     LPENHMETAHEADER metaheader )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find(metafile));

    if ( dc == 0 ) return FALSE;

    if ( metaheader ) {
      UINT size = min( sizeof_enhmetaheader, sizeof(::ENHMETAHEADER) );

      memcpy( metaheader, dc->header, size );

      return size;
    }
    return 0;
  }
  /*!
   * Return various information about the "capabilities" of the device
   * context. This is wholly fabricated for the metafile (i.e., there is
   * no real device to which these attributes relate).
   * \param context handle to metafile context.
   * \param capability enumeration with the following options:
   * \li DRIVERVERSION
   * \li TECHNOLOGY
   * \li HORZSIZE
   * \li VERTSIZE
   * \li HORZRES
   * \li VERTRES
   * \li LOGPIXELSX
   * \li LOGPIXELSY
   * \return device capability
   */
  INT GetDeviceCaps ( HDC context, INT capability )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return -1;

    switch ( capability ) {
    case DRIVERVERSION:
      return 1;
    case TECHNOLOGY:
      return DT_METAFILE;
    case HORZSIZE:
      return dc->header->szlMillimeters.cx;
    case VERTSIZE:
      return dc->header->szlMillimeters.cy;
    case HORZRES:
      return dc->header->szlDevice.cx;
    case VERTRES:
      return dc->header->szlDevice.cy;
    case LOGPIXELSX:
      return dc->resolution.cx;
    case LOGPIXELSY:
      return dc->resolution.cy;
    default:
      return -1;
    }
  }
  /*!
   * Draw a sequence of connected lines.
   * \param context handle to metafile context.
   * \param point array of points to draw.
   * \param n number of points in the array.
   * \return true if polyline is successfully rendered.
   */
  BOOL Polyline ( HDC context, const POINT* points, INT n )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };

    // An optimization: if all the values in points are representable in
    // 16-bits, then we can use the smaller 16-bit POLYLINE16 structure.
    // So, as we update graphics state, try to determine if we've only
    // got short ints.
    const POINT* pnt_ptr = points;
    bool shorts_only = true;

    for ( INT i = 0; i < n; i++ ) {
      if ( pnt_ptr->x > SHRT_MAX || pnt_ptr->x < SHRT_MIN ||
	   pnt_ptr->y > SHRT_MAX || pnt_ptr->y < SHRT_MIN )
	shorts_only = false;

      if ( pnt_ptr->x < bounds.left ) bounds.left = pnt_ptr->x;
      if ( pnt_ptr->x > bounds.right ) bounds.right = pnt_ptr->x;
      if ( pnt_ptr->y < bounds.top ) bounds.top = pnt_ptr->y;
      if ( pnt_ptr->y > bounds.bottom ) bounds.bottom = pnt_ptr->y;

      dc->mergePoint( *pnt_ptr++ );
    }

    if ( shorts_only ) {
      EMF::EMRPOLYLINE16* polyline16 = new EMF::EMRPOLYLINE16( &bounds, points, n );

      dc->appendRecord( polyline16 );
    }
    else {
      EMF::EMRPOLYLINE* polyline = new EMF::EMRPOLYLINE( &bounds, points, n );

      dc->appendRecord( polyline );
    }

    return TRUE;
  }    

  /*!
   * Draw a sequence of connected lines using 16-bit points.
   * \param context handle to metafile context.
   * \param point array of points to draw.
   * \param n number of points in the array.
   * \return true if polyline is successfully rendered.
   */
  BOOL Polyline16 ( HDC context, const POINT16* points, INT16 n )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };
    const POINT16* pnt_ptr = points;

    for ( INT i = 0; i < n; i++, pnt_ptr++ ) {
      if ( pnt_ptr->x < bounds.left ) bounds.left = pnt_ptr->x;
      if ( pnt_ptr->x > bounds.right ) bounds.right = pnt_ptr->x;
      if ( pnt_ptr->y < bounds.top ) bounds.top = pnt_ptr->y;
      if ( pnt_ptr->y > bounds.bottom ) bounds.bottom = pnt_ptr->y;

      dc->mergePoint( pnt_ptr->x, pnt_ptr->y );
    }

    EMF::EMRPOLYLINE16* polyline16 = new EMF::EMRPOLYLINE16( &bounds, points, n );

    dc->appendRecord( polyline16 );

    return TRUE;
  }    
  /*!
   * Draw a sequence of connected straight line segments where the end
   * of the last line segment is connect to the beginning of the first
   * line segment.
   * \param context handle to metafile context.
   * \param points array of points to draw.
   * \param n number of points in the array.
   * \return true if polygon is successfully rendered.
   */
  BOOL Polygon ( HDC context, const POINT* points, INT n )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };

    // An optimization: if all the values in points are representable in
    // 16-bits, then we can use the smaller 16-bit POLYGON16 structure.
    // So, as we update graphics state, try to determine if we've only
    // got short ints.
    const POINT* pnt_ptr = points;
    bool shorts_only = true;

    for ( INT i = 0; i < n; i++ ) {
      if ( pnt_ptr->x > SHRT_MAX || pnt_ptr->x < SHRT_MIN ||
	   pnt_ptr->y > SHRT_MAX || pnt_ptr->y < SHRT_MIN )
	shorts_only = false;

      if ( pnt_ptr->x < bounds.left ) bounds.left = pnt_ptr->x;
      if ( pnt_ptr->x > bounds.right ) bounds.right = pnt_ptr->x;
      if ( pnt_ptr->y < bounds.top ) bounds.top = pnt_ptr->y;
      if ( pnt_ptr->y > bounds.bottom ) bounds.bottom = pnt_ptr->y;

      dc->mergePoint( *pnt_ptr++ );
    }

    if ( shorts_only ) {
      EMF::EMRPOLYGON16* polygon16 = new EMF::EMRPOLYGON16( &bounds, points, n );

      dc->appendRecord( polygon16 );
    }
    else {
      EMF::EMRPOLYGON* polygon = new EMF::EMRPOLYGON( &bounds, points, n );

      dc->appendRecord( polygon );
    }

    return TRUE;
  }
  /*!
   * Draw a sequence of connected straight line segments where the end
   * of the last line segment is connect to the beginning of the first
   * line segment.
   * \param context handle to metafile context.
   * \param points array of points to draw.
   * \param n number of points in the array.
   * \return true if polygon is successfully rendered.
   */
  BOOL Polygon16 ( HDC context, const POINT16* points, INT16 n )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };

    const POINT16* pnt_ptr = points;

    for ( INT i = 0; i < n; i++, pnt_ptr++ ) {

      if ( pnt_ptr->x < bounds.left ) bounds.left = pnt_ptr->x;
      if ( pnt_ptr->x > bounds.right ) bounds.right = pnt_ptr->x;
      if ( pnt_ptr->y < bounds.top ) bounds.top = pnt_ptr->y;
      if ( pnt_ptr->y > bounds.bottom ) bounds.bottom = pnt_ptr->y;

      dc->mergePoint( pnt_ptr->x, pnt_ptr->y );
    }

    EMF::EMRPOLYGON16* polygon16 = new EMF::EMRPOLYGON16( &bounds, points, n );

    dc->appendRecord( polygon16 );

    return TRUE;
  }
  /*!
   * Draw a series of sequences of connected straight line segments where
   * the end of the last line segment is connect to the beginning of the
   * first line segment.
   * \param context handle to metafile context.
   * \param points array of points to draw.
   * \param counts array of number of points in each polygon.
   * \param polygons number of polygons (i.e. number of values in counts array).
   * \return true if the polygons are successfully rendered.
   */
  BOOL PolyPolygon ( HDC context, const POINT* points, const INT* counts,
		     UINT polygons )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };

    // An optimization: if all the values in points are representable in
    // 16-bits, then we can use the smaller 16-bit POLYPOLYGON structure.
    // So, as we update graphics state, try to determine if we've only
    // got short ints.
    const POINT* pnt_ptr = points;
    bool shorts_only = true;

    for ( UINT i = 0; i < polygons; i++ )
      for ( INT j = 0; j < counts[i]; j++ ) {
	if ( pnt_ptr->x > SHRT_MAX || pnt_ptr->x < SHRT_MIN ||
	     pnt_ptr->y > SHRT_MAX || pnt_ptr->y < SHRT_MIN )
	  shorts_only = false;

	if ( pnt_ptr->x < bounds.left ) bounds.left = pnt_ptr->x;
	if ( pnt_ptr->x > bounds.right ) bounds.right = pnt_ptr->x;
	if ( pnt_ptr->y < bounds.top ) bounds.top = pnt_ptr->y;
	if ( pnt_ptr->y > bounds.bottom ) bounds.bottom = pnt_ptr->y;

	dc->mergePoint( *pnt_ptr++ );
      }

    if ( shorts_only ) {
      EMF::EMRPOLYPOLYGON16* polypolygon16 =
	new EMF::EMRPOLYPOLYGON16( &bounds, points, counts, polygons );

      dc->appendRecord( polypolygon16 );
    }
    else {
      EMF::EMRPOLYPOLYGON* polypolygon =
	new EMF::EMRPOLYPOLYGON( &bounds, points, counts, polygons );

      dc->appendRecord( polypolygon );
    }

    return TRUE;
  }
  /*!
   * Draw a series of sequences of connected straight line segments where
   * the end of the last line segment is connect to the beginning of the
   * first line segment. Uses the 16-bit interface (not directly callable
   * from a user-program, though).
   * \param context handle to metafile context.
   * \param points array of points to draw.
   * \param counts array of number of points in each polygon.
   * \param polygons number of polygons (i.e. number of values in counts array).
   * \return true if the polygons are successfully rendered.
   */
  BOOL PolyPolygon16 ( HDC context, const POINT16* points, const INT* counts,
		       UINT16 polygons )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };

    const POINT16* pnt_ptr = points;

    for ( UINT i = 0; i < polygons; i++ )
      for ( INT j = 0; j < counts[i]; j++, pnt_ptr++ ) {

	if ( pnt_ptr->x < bounds.left ) bounds.left = pnt_ptr->x;
	if ( pnt_ptr->x > bounds.right ) bounds.right = pnt_ptr->x;
	if ( pnt_ptr->y < bounds.top ) bounds.top = pnt_ptr->y;
	if ( pnt_ptr->y > bounds.bottom ) bounds.bottom = pnt_ptr->y;

	dc->mergePoint( pnt_ptr->x, pnt_ptr->y );
      }

    EMF::EMRPOLYPOLYGON16* polypolygon16 =
      new EMF::EMRPOLYPOLYGON16( &bounds, points, counts, polygons );

    dc->appendRecord( polypolygon16 );

    return TRUE;
  }

  /*!
   * Evidently returns the name of the current font.
   * \param context handle to metafile context.
   * \param count size of name buffer
   * \param name buffer to store returned name
   * \return real number of bytes in name
   */
  INT GetTextFaceA ( HDC /*context*/, INT /*count*/, LPSTR /*name*/ )
  {
    return 0;
  }

  /*!
   * Even after reading the documentation for this function, I still
   * don't understand what it does. This has no EMF implementation.
   * \param context handle to metafile context.
   * \param extra number of extra spaces.
   * \param breaks number of break characters.
   * \return true if successful.
   */
  BOOL SetTextJustification ( HDC /*context*/, INT /*extra*/, INT /*breaks*/ )
  {
    return FALSE;
  }
  /*!
   * Set the polygon fill mode.
   * \param polygon fill mode with the following options:
   * \li ALTERNATE
   * \li WINDING
   * \return previous fill mode.
   */
  INT SetPolyFillMode ( HDC context, INT mode )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return 0;

    EMF::EMRSETPOLYFILLMODE* setpolyfillmode = new EMF::EMRSETPOLYFILLMODE( mode );

    dc->appendRecord( setpolyfillmode );

    UINT old_polyfill_mode = dc->polyfill_mode;

    dc->polyfill_mode = mode;

    return old_polyfill_mode;
  }
  /*!
   * Fill the defined path.
   * \return true if successful.
   */
  BOOL FillPath ( HDC context )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { 0, 0, -1, -1 };

    EMF::EMRFILLPATH* fillpath = new EMF::EMRFILLPATH( &bounds );

    dc->appendRecord( fillpath );
    
    return TRUE;
  }
  /*!
   * Outline the defined path.
   * \return true if successful.
   */
  BOOL StrokePath ( HDC context )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { 0, 0, -1, -1 };

    EMF::EMRSTROKEPATH* strokepath = new EMF::EMRSTROKEPATH( &bounds );

    dc->appendRecord( strokepath );

    return TRUE;
  }
  /*!
   * Outline and Fill the defined path.
   * \return true if successful.
   */
  BOOL StrokeAndFillPath ( HDC context )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { 0, 0, -1, -1 };

    EMF::EMRSTROKEANDFILLPATH* strokeandfillpath =
      new EMF::EMRSTROKEANDFILLPATH( &bounds );

    dc->appendRecord( strokeandfillpath );

    return TRUE;
  }

  /*!
   * Draw a Bezier curve to (what?).
   * \param context handle to metafile context.
   * \param point array of points to draw.
   * \param n number of points in the array.
   * \return true if Bezier curve is successfully rendered.
   */
  BOOL PolyBezierTo ( HDC context, const POINT* points, DWORD n )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };

    // An optimization: if all the values in points are representable in
    // 16-bits, then we can use the smaller 16-bit POLYBEZIERTO16 structure.
    // So, as we update graphics state, try to determine if we've only
    // got short ints.
    const POINT* pnt_ptr = points;
    bool shorts_only = true;

    for ( DWORD i = 0; i < n; i++ ) {
      if ( pnt_ptr->x > SHRT_MAX || pnt_ptr->x < SHRT_MIN ||
	   pnt_ptr->y > SHRT_MAX || pnt_ptr->y < SHRT_MIN )
	shorts_only = false;

      if ( pnt_ptr->x < bounds.left ) bounds.left = pnt_ptr->x;
      if ( pnt_ptr->x > bounds.right ) bounds.right = pnt_ptr->x;
      if ( pnt_ptr->y < bounds.top ) bounds.top = pnt_ptr->y;
      if ( pnt_ptr->y > bounds.bottom ) bounds.bottom = pnt_ptr->y;

      dc->mergePoint( *pnt_ptr++ );
    }

    if ( shorts_only ) {
      EMF::EMRPOLYBEZIERTO16* polybezierto16 =
	new EMF::EMRPOLYBEZIERTO16( &bounds, points, n );

      dc->appendRecord( polybezierto16 );
    }
    else {
      EMF::EMRPOLYBEZIERTO* polybezierto =
	new EMF::EMRPOLYBEZIERTO( &bounds, points, n );

      dc->appendRecord( polybezierto );
    }

    return TRUE;
  }

  /*!
   * Draw a Bezier curve to (what?) using 16-bit points.
   * \param context handle to metafile context.
   * \param point array of points to draw.
   * \param n number of points in the array.
   * \return true if Bezier curve is successfully rendered.
   */
  BOOL PolyBezierTo16 ( HDC context, const POINT16* points, INT16 n )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };
    const POINT16* pnt_ptr = points;

    for ( INT i = 0; i < n; i++, pnt_ptr++ ) {
      if ( pnt_ptr->x < bounds.left ) bounds.left = pnt_ptr->x;
      if ( pnt_ptr->x > bounds.right ) bounds.right = pnt_ptr->x;
      if ( pnt_ptr->y < bounds.top ) bounds.top = pnt_ptr->y;
      if ( pnt_ptr->y > bounds.bottom ) bounds.bottom = pnt_ptr->y;

      dc->mergePoint( pnt_ptr->x, pnt_ptr->y );
    }

    EMF::EMRPOLYBEZIERTO16* polybezierto16 =
      new EMF::EMRPOLYBEZIERTO16( &bounds, points, n );

    dc->appendRecord( polybezierto16 );

    // Update graphics state
    for ( INT16 i = 0; i < n; i++ )
      dc->mergePoint( points[i].x, points[i].y );

    return TRUE;
  }

  /*!
   * Draw a Polyline to (what?).
   * \param context handle to metafile context.
   * \param point array of points to draw.
   * \param n number of points in the array.
   * \return true if Polyline is successfully rendered.
   */
  BOOL PolylineTo ( HDC context, const POINT* points, DWORD n )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };

    // An optimization: if all the values in points are representable in
    // 16-bits, then we can use the smaller 16-bit POLYLINETO16 structure.
    // So, as we update graphics state, try to determine if we've only
    // got short ints.
    const POINT* pnt_ptr = points;
    bool shorts_only = true;

    for ( DWORD i = 0; i < n; i++ ) {
      if ( pnt_ptr->x > SHRT_MAX || pnt_ptr->x < SHRT_MIN ||
	   pnt_ptr->y > SHRT_MAX || pnt_ptr->y < SHRT_MIN )
	shorts_only = false;

      if ( pnt_ptr->x < bounds.left ) bounds.left = pnt_ptr->x;
      if ( pnt_ptr->x > bounds.right ) bounds.right = pnt_ptr->x;
      if ( pnt_ptr->y < bounds.top ) bounds.top = pnt_ptr->y;
      if ( pnt_ptr->y > bounds.bottom ) bounds.bottom = pnt_ptr->y;

      dc->mergePoint( *pnt_ptr++ );
    }

    if ( shorts_only ) {
      EMF::EMRPOLYLINETO16* polylineto16 =
	new EMF::EMRPOLYLINETO16( &bounds, points, n );

      dc->appendRecord( polylineto16 );
    }
    else {
      EMF::EMRPOLYLINETO* polylineto = new EMF::EMRPOLYLINETO( &bounds, points, n );

      dc->appendRecord( polylineto );
    }

    return TRUE;
  }

  /*!
   * Draw a Polyline to (what?) using 16-bit points.
   * \param context handle to metafile context.
   * \param point array of points to draw.
   * \param n number of points in the array.
   * \return true if Polyline is successfully rendered.
   */
  BOOL PolylineTo16 ( HDC context, const POINT16* points, INT16 n )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    RECTL bounds = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };
    const POINT16* pnt_ptr = points;

    for ( INT i = 0; i < n; i++, pnt_ptr++ ) {
      if ( pnt_ptr->x < bounds.left ) bounds.left = pnt_ptr->x;
      if ( pnt_ptr->x > bounds.right ) bounds.right = pnt_ptr->x;
      if ( pnt_ptr->y < bounds.top ) bounds.top = pnt_ptr->y;
      if ( pnt_ptr->y > bounds.bottom ) bounds.bottom = pnt_ptr->y;

      dc->mergePoint( pnt_ptr->x, pnt_ptr->y );
    }

    EMF::EMRPOLYLINETO16* polylineto16 =
      new EMF::EMRPOLYLINETO16( &bounds, points, n );

    dc->appendRecord( polylineto16 );

    return TRUE;
  }

  /*!
   * Return information about the current font.
   * \param context handle to metafile context
   * \param textmetrics returns the font metrics.
   * \return true if successful.
   */
  BOOL GetTextMetricsA ( HDC /*context*/, LPTEXTMETRICA /*textmetrics*/ )
  {
    return FALSE;
  }

  /*!
   * Measure the extents of a string.
   * \param context handle to metafile context.
   * \param string ASCII string to measure.
   * \param count number of characters in string.
   * \param size size of text extents.
   * \return true if successful.
   */
  BOOL GetTextExtentPoint32A ( HDC /*context*/, LPCSTR /*string*/, INT /*count*/,
			       LPSIZE /*size*/ )
  {
    return FALSE;
  }

  /*!
   * Begin defining a path.
   * \param context handle to metafile context.
   * \return true if successful.
   */
  BOOL BeginPath ( HDC context )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRBEGINPATH* beginpath = new EMF::EMRBEGINPATH();

    dc->appendRecord( beginpath );

    return TRUE;
  }

  /*!
   * End the path definition.
   * \param context handle to metafile context.
   * \return true if successful.
   */
  BOOL EndPath ( HDC context )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRENDPATH* endpath = new EMF::EMRENDPATH();

    dc->appendRecord( endpath );

    return TRUE;
  }

  /*!
   * Close the figure (how is this different from EndPath?)
   * \param context handle to metafile context.
   * \return true if successful.
   */
  BOOL CloseFigure ( HDC context )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRCLOSEFIGURE* closefigure = new EMF::EMRCLOSEFIGURE();

    dc->appendRecord( closefigure );

    return TRUE;
  }

  /*!
   * Push the (contents of?) given Device Context on to a stack (?).
   * \param dc device context to save
   * \return number of save'd contexts?
   */
  INT SaveDC (HDC context )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRSAVEDC* savedc = new EMF::EMRSAVEDC();

    dc->appendRecord( savedc );

    return 0; // Not really implemented.
  }

  /*!
   * Get the (contents of?) given Device Context off a stack (?).
   * \param dc device context to restore into
   * \param n pushed context to restore
   * \return number of save'd contexts?
   */
  INT RestoreDC (HDC context, INT n )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRRESTOREDC* restoredc = new EMF::EMRRESTOREDC( n );

    dc->appendRecord( restoredc );

    return 0; // Not really implemented.
  }

  /*!
   * Uhm?
   * \param dc device context
   * \return number of somethings?
   */
  INT SetMetaRgn (HDC context )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return FALSE;

    EMF::EMRSETMETARGN* setmetargn = new EMF::EMRSETMETARGN();

    dc->appendRecord( setmetargn );

    return 0; // Not really implemented.
  }
  /*!
   * Adds extra space between each character drawn.
   * \param context handle to metafile context.
   * \param extra extra space to add.
   * \return previous value.
   */
  INT SetTextCharacterExtra ( HDC /*context*/, INT /*extra*/ )
  {
    return 0;
  }

  /*!
   * Returns the extra space between each character setting.
   * \param context handle to metafile context.
   * \return the current setting.
   */
  INT GetTextCharacterExtra ( HDC /*context*/ )
  {
    return 0;
  }

  /*!
   * Set the pixel to the given color.
   * \param context the metafile device context.
   * \param x the horizontal position of the pixel to color.
   * \param y the vertical position of the pixel to color.
   * \param color the color to color the pixel with.
   * \return the previous color of the pixel? (not really implemented, though).
   */
  COLORREF SetPixel ( HDC context, INT x, INT y, COLORREF color )
  {
    EMF::METAFILEDEVICECONTEXT* dc =
      dynamic_cast<EMF::METAFILEDEVICECONTEXT*>(EMF::globalObjects.find( context ));

    if ( dc == 0 ) return 0;

    EMF::EMRSETPIXELV* setpixelv = new EMF::EMRSETPIXELV( x, y, color );

    dc->appendRecord( setpixelv );

    dc->mergePoint( x, y );

    return RGB(0,0,0);
  }

  /*!
   * Return a dummy handle to the desktop window (graphics systems aren't
   * a requirement).
   * \return 0
   */
  HWND GetDesktopWindow ( void )
  {
    return 0;
  }

  /*!
   * Return a device context from the given window handle. Neither of these
   * exist here, actually.
   * \param window handle to a window.
   * \return device context of window.
   */
  HDC GetDC ( HWND /*window*/ )
  {
    return 0;
  }

  /*!
   * Surrender device context associated with the given window. Does nothing
   * here.
   * \param window handle to a window.
   * \param context device context associated with window.
   * \return 1 (determined from wine source code)
   */
  INT ReleaseDC ( HWND /*window*/, HDC /*context*/ )
  {
    return 1;
  }
}


/* W. Glunz */
/* not important */ INT EnumFontFamiliesA(HDC,LPCSTR,FONTENUMPROCA,LPARAM) { return 1; }
/* not important */ INT EnumFontFamiliesW(HDC,LPCWSTR,FONTENUMPROCW,LPARAM) { return 1; }
/* not important */ INT EnumFontFamiliesExA(HDC,LPLOGFONTA,FONTENUMPROCEXA,LPARAM,DWORD) { return 1; }
/* not important */ INT EnumFontFamiliesExW(HDC,LPLOGFONTW,FONTENUMPROCEXW,LPARAM,DWORD) { return 1; }
/* not important */ INT EnumFontsA(HDC,LPCSTR,FONTENUMPROCA,LPARAM) { return 1; }
/* not important */ INT EnumFontsW(HDC,LPCWSTR,FONTENUMPROCW,LPARAM) { return 1; }

/* important only for embedding bitmaps but could be avoided */ INT StretchDIBits(HDC,INT,INT,INT,INT,INT,INT, INT,INT,const void*,const BITMAPINFO*,UINT,DWORD) { return 1; }
INT SetDIBitsToDevice(HDC,INT,INT,DWORD,DWORD,INT,
  INT,UINT,UINT,LPCVOID,const BITMAPINFO*,UINT) { return 1; }
