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


#include "CustomToolButton.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "Singleton.h"
#include "XmlOptions.h"

#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <QtGui/QStylePainter>
#include <QtGui/QStyleOptionToolButton>

//___________________________________________________________________
CustomToolButton::CustomToolButton( QWidget* parent ):
QToolButton( parent ),
Counter( "CustomToolButton" ),
updateFromOptions_( true ),
rotation_( NONE )
{

    Debug::Throw( "CustomToolButton::CustomToolButton.\n" );

    // auto-raise
    setAutoRaise( true );

    // configuration
    connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
    _updateConfiguration();

}

//______________________________________________________________________
bool CustomToolButton::rotate( const CustomToolButton::Rotation& value )
{
    Debug::Throw( "CustomToolButton::rotate.\n" );
    if( rotation_ == value ) return false;

    // rotate icon if any
    CustomPixmap pixmap( icon().pixmap( IconSize::Large, QIcon::Normal ) );
    if( !pixmap.isNull() )
    {

        // clockwise rotations
        if( ( rotation_ == NONE && value == COUNTERCLOCKWISE ) || ( rotation_ == CLOCKWISE && value == NONE ) )
        { setIcon( IconEngine::get( pixmap.rotate( CustomPixmap::CLOCKWISE ) ) ); }

        if( ( rotation_ == NONE && value == CLOCKWISE ) || ( rotation_ == COUNTERCLOCKWISE && value == NONE ) )
        { setIcon( IconEngine::get( pixmap.rotate( CustomPixmap::COUNTERCLOCKWISE ) ) ); }

    } else { Debug::Throw(0) << "CustomToolButton::rotate - null pixmap." << endl; }

    rotation_ = value;
    return true;
}

//______________________________________________________________________
QSize CustomToolButton::sizeHint( void ) const
{  QSize size( QToolButton::sizeHint() );
if( _rotation() != NONE ) size.transpose();
return size;
}

//______________________________________________________________________
void CustomToolButton::paintEvent( QPaintEvent* event )
{

    // default implementation if not rotated
    if( _rotation() == NONE ) return QToolButton::paintEvent( event );

    // rotated paint
    QStylePainter painter(this);
    QStyleOptionToolButton option;
    initStyleOption(&option);

    // first draw normal frame and not text/icon
    option.text = QString();
    option.icon = QIcon();
    painter.drawComplexControl(QStyle::CC_ToolButton, option);

    // rotate the options
    QSize size( option.rect.size() );
    size.transpose();
    option.rect.setSize( size );

    // rotate the painter
    if( _rotation() == COUNTERCLOCKWISE )
    {
        painter.translate( 0, height() );
        painter.rotate( -90 );
    } else {
        painter.translate( width(), 0 );
        painter.rotate( 90 );
    }

    // paint text and icon
    option.text = text();
    option.icon = icon();
    painter.drawControl(QStyle::CE_ToolButtonLabel, option);
    painter.end();

    return;
}

//_________________________________________________________________
void CustomToolButton::_updateConfiguration( void )
{
    Debug::Throw( "CustomToolButton::_updateConfiguration.\n");

    if( !_updateFromOptions() ) return;

    int iconSize( XmlOptions::get().get<int>( "TOOLBUTTON_ICON_SIZE" ) );
    if( iconSize <= 0 ) iconSize = style()->pixelMetric( QStyle::PM_ToolBarIconSize );
    setIconSize( QSize( iconSize, iconSize ) );

    const int toolButtonTextPosition( XmlOptions::get().get<int>( "TOOLBUTTON_TEXT_POSITION" ) );
    if( toolButtonTextPosition < 0 ) setToolButtonStyle( (Qt::ToolButtonStyle) style()->styleHint( QStyle::SH_ToolButtonStyle ) );
    else setToolButtonStyle( (Qt::ToolButtonStyle) toolButtonTextPosition );

    adjustSize();

}
