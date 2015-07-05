/*
 * lccolorchooser.cpp
 *
 * Implementation of lCColorChooser class
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
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qcolordialog.h>

#include "constants.h"
#include "lccolorchooser.h"

lCColorChooser::lCColorChooser( QWidget *parent, const char *name )
    : QWidget( parent ), edited_( false )
{
    setObjectName( name );
    //TODO
    //  setFrameStyle( Panel | Sunken );
    //  setLineWidth( 2 );
    //  setMargin( 0 );
    //  setSpacing( 0 );

    color_label_ = new QLabel( this );
    color_label_->setObjectName( "label" );
    color_label_->setMinimumWidth( color_label_->fontMetrics().width( "COLOR" ) );
    color_label_->setFixedHeight( color_label_->fontMetrics().lineSpacing() );
    color_label_->setFrameStyle( QFrame::Box & QFrame::Plain );

    button_ = new QPushButton( this );
    button_->setObjectName( "..." );
    button_->setText( "button" );
    button_->setFixedWidth( button_->fontMetrics().width( "ABC" ) );

    default_ = new QToolButton( this );
    default_->setObjectName( "default" );

    default_->setToolTip( tr( "Click this button to restore the color to the default" ) );

    QIcon icon( lC::lookupPixmap( "default_active.png" ) );
    icon.addPixmap( lC::lookupPixmap( "default_inactive.png" ),
                    QIcon::Disabled, QIcon::Off );
    default_->setIcon( icon );

    default_->setFixedWidth( default_->sizeHint().width() );
    default_->setFixedHeight( button_->sizeHint().height()-2 );

    connect( button_, SIGNAL( clicked() ), this, SLOT( chooseColor() ) );
    connect( default_, SIGNAL( clicked() ), this, SLOT( chooseDefault() ) );

    setFocusProxy( button_ );
}

bool lCColorChooser::edited ( void ) const
{
    return edited_;
}

void lCColorChooser::setEdited ( bool edited )
{
    edited_ = edited;
}

void lCColorChooser::setColor( const QColor& color )
{
    color_ = color;
    //TODO
    //color_label_->setPaletteBackgroundColor( color_ );

    edited_ = false;

    if ( color_ == default_color_ )
        default_->setEnabled( false );
}

void lCColorChooser::setDefaultColor ( QColor default_color )
{
    default_color_ = default_color;
}

QColor lCColorChooser::color() const
{
    return color_;
}

void lCColorChooser::chooseColor()
{
    QColor color = QColorDialog::getColor( color_, this );

    if ( color.isValid() ) {
        color_ = color;

        //TODO
        //color_label_->setPaletteBackgroundColor( color_ );

        edited_ = true;

        if ( color_ != default_color_ )
            default_->setEnabled( true );
        else
            default_->setEnabled( false );

        emit colorChanged( color_ );
    }
}

void lCColorChooser::chooseDefault ( void )
{
    color_ = default_color_;

    //TODO
    //color_label_->setPaletteBackgroundColor( color_ );

    edited_ = true;

    default_->setEnabled( false );

    emit colorChanged( color_ );
}
