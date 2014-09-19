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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "CustomPixmap.h"
#include "Debug.h"
#include "IconSize.h"
#include "File.h"
#include "PixmapEngine.h"
#include "QtUtil.h"

#include <QApplication>
#include <QImage>
#include <QIcon>
#include <QPainter>
#include <QFileIconProvider>

//_________________________________________________
CustomPixmap::CustomPixmap( const QSize& size, Flags flags ):
    #if QT_VERSION >= 0x050300
    QPixmap( flags&HighDpi ? size*qApp->devicePixelRatio() : size ),
    #else
    QPixmap( size ),
    #endif
    Counter( "CustomPixmap" )
{
    #if QT_VERSION >= 0x050300
    if( flags&HighDpi ) setDevicePixelRatio( qApp->devicePixelRatio() );
    #endif

    if( flags&Transparent ) fill( Qt::transparent );

}

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
    if( rotation == None ) return *this;

    const qreal dpiRatio( devicePixelRatio() );

    CustomPixmap out( QSize( height(), width() ), Transparent );
    out.setDevicePixelRatio( dpiRatio );
    QPainter painter( &out );

    // rotate the painter
    if( rotation == CounterClockwise )
    {
        painter.translate( 0, out.height()/dpiRatio );
        painter.rotate( -90 );
    } else {
        painter.translate( out.width()/dpiRatio, 0 );
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
CustomPixmap CustomPixmap::merge( const CustomPixmap& pixmap, Corner corner ) const
{
    if( isNull() ) return *this;

    QSize size( this->size()/devicePixelRatio() );
    QSize pixmapSize( pixmap.size()/pixmap.devicePixelRatio() );

    QImage source( toImage() );
    QPainter painter( &source );
    switch( corner )
    {

        case TOP_RIGHT:
        painter.drawPixmap( QPoint( size.width()-pixmapSize.width(), 0 ), pixmap );
        break;

        case BOTTOM_LEFT:
        painter.drawPixmap( QPoint( 0, size.height()-pixmapSize.height() ), pixmap );
        break;

        case BOTTOM_RIGHT:
        painter.drawPixmap( QPoint( size.width()-pixmapSize.width(), size.height()-pixmapSize.height() ), pixmap );
        break;

        case CENTER:
        painter.drawPixmap(  QPoint( (size.width()-pixmapSize.width())/2, (size.height()-pixmapSize.height())/2 ), pixmap );
        break;

        default:
        case TOP_LEFT:
        painter.drawPixmap( QPoint( 0, 0 ), pixmap );
        break;

    }

    painter.end();
    return CustomPixmap().fromImage( source );
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

//______________________________________________________________________________________
qreal CustomPixmap::devicePixelRatio( void ) const
{
    #if QT_VERSION >= 0x050300
    return QPixmap::devicePixelRatio();
    #else
    return 1;
    #endif
}

//______________________________________________________________________________________
void CustomPixmap::setDevicePixelRatio( qreal value )
{
    #if QT_VERSION >= 0x050300
    return QPixmap::setDevicePixelRatio( value );
    #else
    Q_UNUSED( value );
    #endif
}
