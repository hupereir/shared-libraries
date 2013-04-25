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

#include <QImage>
#include <QIcon>
#include <QPainter>
#include <QFileIconProvider>

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
    #if defined(Q_OS_WIN)
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
CustomPixmap CustomPixmap::transparent( qreal intensity ) const
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
CustomPixmap CustomPixmap::desaturate( void ) const
{
    if( isNull() ) return *this;

    QImage image( toImage() );
    if( image.format() != QImage::Format_ARGB32_Premultiplied )
    {
        Debug::Throw(0) << "CustomPixmap::desaturate - wrong format." << endl;
        return *this;
    }

    for( int row = 0; row < image.height(); ++row )
    {

        // alter pixmaps
        QRgb* pixels = reinterpret_cast<QRgb*>( image.scanLine( row ) );

        for( int column = 0; column < image.width(); ++column )
        {
            const int gray( qGray( pixels[column] ) );
            pixels[column] = qRgba( gray, gray, gray, qAlpha(pixels[column]) );
        }
    }

    return CustomPixmap( image );
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
CustomPixmap CustomPixmap::highlighted( qreal opacity ) const
{

    Debug::Throw( "CustomPixmap::highlighted.\n" );
    if( opacity <= 0 ) return *this;
    opacity = qMin( opacity, 1.0 );

    // compute mask
    QPixmap mask( *this );
    {
        QPainter painter( &mask );

        painter.setRenderHints(QPainter::SmoothPixmapTransform);
        painter.setPen( Qt::NoPen );

        QColor color( Qt::white );
        color.setAlphaF( opacity );
        painter.setBrush( color );

        painter.setCompositionMode( QPainter::CompositionMode_SourceIn );

        painter.drawRect( mask.rect() );
        painter.end();
    }

    if( opacity == 1.0 ) return mask;

    // apply highlight
    QPixmap out( *this );
    {
        QPainter painter( &out );
        painter.drawPixmap( QPoint(0,0), mask );
        painter.end();
    }

    return out;

}
