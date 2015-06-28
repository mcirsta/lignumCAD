/* -*- c++ -*-
 * aboutdialog.ui.h
 *
 * Header for aboutDialog class
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
 * Initialize the About dialog.
 */
void aboutDialog::init()
{
    programVersionLabel->setText( tr( "%1 Version %2.%3" ).
				  arg( lC::STR::LIGNUMCAD ).
				  arg( lC::STR::VERSION_MAJOR ).
				  arg( lC::STR::VERSION_MINOR ) );
    copyrightLabel->setText( tr( "Copyright © %1 %2" ).
			     arg( lC::STR::COPYRIGHT_DATES ).
			     arg( lC::STR::LIGNUMCOMPUTINGINC ) );
    contactLabel->setText( tr( "%1\ne-mail: %2" ).
			   arg( lC::STR::LIGNUMCOMPUTINGURL ).
			   arg( lC::STR::LIGNUMCOMPUTINGEMAIL ) );
    // Try to figure out where all these things come from:
    // OpenCASCADE
    openCASCADEVersionLabel->setText( tr( "Version: %1" ).
				 arg( OCC_VERSION_STRING ) );
    // FreeType (this interface is only available in very recent versions of FreeType)
    {
	FT_Library library = OGLFT::Library::instance();
	FT_Int major, minor, patch;
	FT_Library_Version( library, &major, &minor, &patch );
	freeTypeVersionLabel->setText( tr( "Version: %1.%2.%3" ).arg(major).arg(minor).arg(patch) );
    }
    // Qt
    qtVersionLabel->setText( tr( "Version: %1" ).arg( qVersion() ) );
  // SGI's  OpenGL GLU
    openGLGLUVersionLabel->setText( tr( "Version: %1" ).
				    arg( (char*)gluGetString( GLU_VERSION ) ) );
    // OpenGL driver
    openGLVersionLabel->setText( tr( "Vendor: %1 Version: %2" ).
				 arg( (char*)glGetString( GL_VENDOR ) ).
				 arg( (char*)glGetString( GL_VERSION ) ) );
  }
