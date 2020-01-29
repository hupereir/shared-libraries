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
    QPixmap( size*qApp->devicePixelRatio() ),
    Counter( "CustomPixmap" )
{
    setDevicePixelRatio( qApp->devicePixelRatio() );
    if( flags&Flag::Transparent ) fill( Qt::transparent );
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

        const QSize iconSize( IconSize::get( IconSize::Maximum ) );
        CustomPixmap out( icon.pixmap( iconSize ) );
        *this = out;

    }
    #endif

}

//_________________________________________________
CustomPixmap& CustomPixmap::find( const QString& file )
{

    Debug::Throw( "CustomPixmap::find.\n" );
    *this = CustomPixmap( PixmapEngine::get( file ) );
    return *this;

}

//_________________________________________________
CustomPixmap CustomPixmap::rotated( const CustomPixmap::Rotation& rotation ) const
{
    if( rotation == Rotation::None ) return *this;

    const qreal dpiRatio( devicePixelRatio() );

    CustomPixmap out( QSize( height(), width() ), Flag::Transparent );
    out.setDevicePixelRatio( dpiRatio );
    QPainter painter( &out );

    // rotate the painter
    if( rotation == Rotation::CounterClockwise )
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

    CustomPixmap out( *this );
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
CustomPixmap CustomPixmap::desaturated() const
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
CustomPixmap CustomPixmap::colorized( const QColor& color ) const
{

    CustomPixmap out( desaturated() );
    QPainter painter( &out );
    painter.setCompositionMode(QPainter::CompositionMode_Screen);
    painter.fillRect(rect(), color);

    // copy alpha channel from source
    painter.setCompositionMode( QPainter::CompositionMode_DestinationIn );
    painter.drawPixmap( out.rect(), *this );

    painter.end();
    return out;
}


//_________________________________________________
CustomPixmap CustomPixmap::merged( const QPixmap& pixmap, Corner corner ) const
{
    if( isNull() ) return *this;

    QSize size( this->size()/devicePixelRatio() );
    QSize pixmapSize( pixmap.size()/pixmap.devicePixelRatio() );
    QImage source( toImage() );
    QPainter painter( &source );
    switch( corner )
    {

        case Corner::TopRight:
        painter.drawPixmap( QPoint( size.width()-pixmapSize.width(), 0 ), pixmap );
        break;

        case Corner::BottomLeft:
        painter.drawPixmap( QPoint( 0, size.height()-pixmapSize.height() ), pixmap );
        break;

        case Corner::BottomRight:
        painter.drawPixmap( QPoint( size.width()-pixmapSize.width(), size.height()-pixmapSize.height() ), pixmap );
        break;

        case Corner::Center:
        painter.drawPixmap(  QPoint( (size.width()-pixmapSize.width())/2, (size.height()-pixmapSize.height())/2 ), pixmap );
        break;

        case Corner::TopLeft:
        painter.drawPixmap( QPoint( 0, 0 ), pixmap );
        break;

    }

    painter.end();
    return CustomPixmap( fromImage( source ) );
}

//_________________________________________________
CustomPixmap CustomPixmap::highlighted( qreal opacity ) const
{

    Debug::Throw( "CustomPixmap::highlighted.\n" );
    if( opacity <= 0 ) return *this;
    opacity = qMin<qreal>( opacity, 1.0 );

    // compute mask
    CustomPixmap mask( *this );
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
    CustomPixmap out( *this );
    {
        QPainter painter( &out );
        painter.drawPixmap( QPoint(0,0), mask );
        painter.end();
    }

    return out;

}

//______________________________________________________________________________________
qreal CustomPixmap::devicePixelRatio() const
{ return QPixmap::devicePixelRatio(); }

//______________________________________________________________________________________
void CustomPixmap::setDevicePixelRatio( qreal value )
{ return QPixmap::setDevicePixelRatio( value ); }
