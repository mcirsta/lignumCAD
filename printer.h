/* -*- c++ -*-
 * printer.h
 *
 * Header for our Printer class
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
#ifndef PRINTER_H
#define PRINTER_H

#include <qprinter.h>

class Printer : public QPrinter
{
public:
  Printer ( PrinterMode m = HighResolution )
    : QPrinter( m )
  {
    setColorMode( Color );
    setOrientation( Landscape );
    setPageSize( Letter );
    setFullPage( true );
  }
};
#endif // PRINTER_H
