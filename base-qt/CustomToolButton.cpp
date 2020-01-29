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


#include "CustomToolButton.h"

#include "IconSize.h"
#include "Singleton.h"
#include "XmlOptions.h"

#include <QIcon>
#include <QPixmap>
#include <QStylePainter>
#include <QStyleOptionToolButton>

//___________________________________________________________________
CustomToolButton::CustomToolButton( QWidget* parent ):
    QToolButton( parent ),
    Counter( "CustomToolButton" )
{

    Debug::Throw( "CustomToolButton::CustomToolButton.\n" );

    // auto-raise
    setAutoRaise( true );

    // configuration
    connect( Base::Singleton::get().application(), SIGNAL(configurationChanged()), SLOT(_updateConfiguration()) );
    _updateConfiguration();

}

//______________________________________________________________________
bool CustomToolButton::rotate( const CustomPixmap::Rotation& value )
{
    Debug::Throw( "CustomToolButton::rotate.\n" );
    if( rotation_ == value ) return false;

    const QIcon source( icon() );
    QIcon copy;

    // loop over available sizes, modes and states
    const auto modes = { QIcon::Normal, QIcon::Disabled, QIcon::Active, QIcon::Selected };
    const auto states = { QIcon::Off, QIcon::On };

    for( const auto& size:source.availableSizes() )
    for( const auto& mode:modes )
    for( const auto& state:states )
    {
        CustomPixmap pixmap( source.pixmap( size, mode, state ) );
        if( pixmap.isNull() ) continue;

        // clockwise rotations
        if(
            ( rotation_ == CustomPixmap::Rotation::None && value == CustomPixmap::Rotation::CounterClockwise ) ||
            ( rotation_ == CustomPixmap::Rotation::Clockwise && value == CustomPixmap::Rotation::None ) )
        {
            copy.addPixmap( pixmap.rotated( CustomPixmap::Rotation::Clockwise ), mode, state );
        } else if(
            ( rotation_ == CustomPixmap::Rotation::None && value == CustomPixmap::Rotation::Clockwise ) ||
            ( rotation_ == CustomPixmap::Rotation::CounterClockwise && value == CustomPixmap::Rotation::None ) )
        {
            copy.addPixmap( pixmap.rotated( CustomPixmap::Rotation::CounterClockwise ), mode, state );
        }

    }

    setIcon( copy );
    rotation_ = value;
    return true;
}

//______________________________________________________________________
QSize CustomToolButton::sizeHint() const
{
    QSize size( QToolButton::sizeHint() );
    if( rotation_ != CustomPixmap::Rotation::None ) size.transpose();
    return size;
}

//______________________________________________________________________
void CustomToolButton::paintEvent( QPaintEvent* event )
{

    // default implementation if not rotated
    if( rotation_ == CustomPixmap::Rotation::None ) return QToolButton::paintEvent( event );

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
    if( rotation_ == CustomPixmap::Rotation::CounterClockwise )
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
void CustomToolButton::_updateConfiguration()
{
    Debug::Throw( "CustomToolButton::_updateConfiguration.\n");

    if( !updateFromOptions_ ) return;

    int iconSize( XmlOptions::get().get<int>( "TOOLBUTTON_ICON_SIZE" ) );
    if( iconSize <= 0 ) iconSize = style()->pixelMetric( QStyle::PM_ToolBarIconSize );
    setIconSize( QSize( iconSize, iconSize ) );

    const int toolButtonTextPosition( XmlOptions::get().get<int>( "TOOLBUTTON_TEXT_POSITION" ) );
    if( toolButtonTextPosition < 0 ) setToolButtonStyle( static_cast<Qt::ToolButtonStyle>( style()->styleHint( QStyle::SH_ToolButtonStyle ) ) );
    else setToolButtonStyle( static_cast<Qt::ToolButtonStyle>( toolButtonTextPosition ) );

    adjustSize();

}
