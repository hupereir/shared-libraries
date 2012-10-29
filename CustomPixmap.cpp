// $Id$

/******************************************************************************
*
* Copyright (C) 2002 Hugo PEREIRA <mailto: hugo.pereira@free.fr>
*
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; either version 2 of the License, or (at your option) any later
* version.
*
* This software is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "CustomPixmap.h"
#include "Debug.h"
#include "IconSize.h"
#include "File.h"
#include "PixmapEngine.h"

#include <QtCore/QBitmap>
#include <QtGui/QImage>
#include <QtGui/QIcon>
#include <QtGui/QPainter>
#include <QtCore/QFileIconProvider>

//_________________________________________________
CustomPixmap::CustomPixmap( const QString& file ):
    QPixmap( file ),
    Counter( "CustomPixmap" )
{
    Debug::Throw( "CustomPixmap::CustomPixmap.\n" );

    /*
    under windows, if pixmap is null, and file is an executable of a link
    try load the icon using FileIncoProvider
    */
    #if defined(Q_WS_WIN)
    if( !isNull() ) return;
    QFileInfo info( file );
    if( info.isExecutable() || info.isSymLink() )
    {

        QIcon icon( QFileIconProvider().icon( file ) );
        if( icon.isNull() ) return;

        const QSize iconSize( IconSize( IconSize::Maximum ) );
        QPixmap out( icon.pixmap( iconSize ) );
        *this = out;

    }
    #endif

}

//_________________________________________________
CustomPixmap CustomPixmap::find( const QString& file )
{

    Debug::Throw( "CustomPixmap::find.\n" );
    *this = CustomPixmap( PixmapEngine::get( file ) );
    return *this;

}

//_________________________________________________
CustomPixmap CustomPixmap::rotate( const CustomPixmap::Rotation& rotation )
{
    if( rotation == NONE ) return *this;

    CustomPixmap out = CustomPixmap().empty( QSize( height(), width() ) );
    QPainter painter( &out );

    // rotate the painter
    if( rotation == COUNTERCLOCKWISE )
    {
        painter.translate( 0, out.height() );
        painter.rotate( -90 );
    } else {
        painter.translate( out.width(), 0 );
        painter.rotate( 90 );
    }

    painter.drawPixmap( QPoint(0,0), *this );
    painter.end();
    return out;
}

//_________________________________________________
CustomPixmap CustomPixmap::tint( const QColor& base_color, const double& intensity ) const
{
    if( isNull() ) return *this;

    QPixmap out( *this );
    QPainter painter( &out );
    QColor color( base_color );
    color.setAlphaF( intensity );

    painter.setPen( Qt::NoPen );
    painter.setBrush( color );
    painter.drawRect( out.rect() );
    painter.end();

    out.setAlphaChannel( alphaChannel() );
    return out;
}

//_________________________________________________
CustomPixmap CustomPixmap::transparent( const double& intensity ) const
{
    if( isNull() ) return *this;

    QPixmap out( *this );
    QPainter painter( &out );

    QColor color( Qt::black );
    color.setAlphaF( intensity );

    painter.setCompositionMode( QPainter::CompositionMode_DestinationIn );
    painter.setPen( Qt::NoPen );
    painter.setBrush( color );
    painter.drawRect( out.rect() );
    painter.end();

    return out;
}

//_________________________________________________
CustomPixmap CustomPixmap::merge( const QPixmap& pixmap, Corner corner ) const
{
    if( isNull() ) return *this;

    QImage source_image( toImage() );
    QPainter painter( &source_image );
    switch( corner )
    {

        case TOP_RIGHT:
        painter.drawPixmap( QPoint( width()-pixmap.width(), 0 ), pixmap );
        break;

        case BOTTOM_LEFT:
        painter.drawPixmap( QPoint( 0, height()-pixmap.height() ), pixmap );
        break;

        case BOTTOM_RIGHT:
        painter.drawPixmap( QPoint( width()-pixmap.width(), height()-pixmap.height() ), pixmap );
        break;

        case CENTER:
        painter.drawPixmap(  QPoint( (width()-pixmap.width())/2, (height()-pixmap.height())/2 ), pixmap );
        break;

        default:
        case TOP_LEFT:
        painter.drawPixmap( QPoint( 0, 0 ), pixmap );
        break;

    }

    painter.end();
    return CustomPixmap().fromImage( source_image );
}

//_________________________________________________
CustomPixmap CustomPixmap::empty( const QSize& size, const QColor& color ) const
{

    Debug::Throw( "CustomPixmap::empty.\n" );
    CustomPixmap out( size );
    if( color.isValid() ) out.fill( color );
    else out.fill( Qt::transparent );
    return out;

}

//_________________________________________________
CustomPixmap CustomPixmap::disabled( void ) const
{

    Debug::Throw( "CustomPixmap::disabled.\n" );
    QImage image( toImage() );

    // retrieve dimensions
    int width( image.width() );
    int height( image.height() );

    QColor mergedColor;
    for( int x = 0; x < width; x++ )
    {
        for( int y = 0; y < height; y++ )
        {
            QColor color( image.pixel( x, y ) );
            int gray( 128 + qGray( color.rgb() )/2 );
            mergedColor.setRgb( gray, gray, gray );

            image.setPixel( x, y, mergedColor.rgb() );
        }
    }

    CustomPixmap out( image );
    out.setAlphaChannel( alphaChannel() );
    return out;

}

//_________________________________________________
CustomPixmap CustomPixmap::highlighted( qreal opacity ) const
{

    Debug::Throw( "CustomPixmap::highlighted.\n" );
    if( opacity <= 0 ) return *this;
    opacity = qMin( opacity, 1.0 );

    // apply highlight
    QPixmap out( *this );
    QPixmap alphaChannel( out.alphaChannel() );
    QPainter painter( &out );

    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    painter.setPen( Qt::NoPen );

    QColor color( Qt::white );
    color.setAlphaF( opacity );
    painter.setBrush( color );

    painter.drawRect( out.rect() );
    painter.end();

    out.setAlphaChannel( alphaChannel );
    return out;

}
