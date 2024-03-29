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


#include "ToolButton.h"
#include "IconSize.h"
#include "Singleton.h"
#include "XmlOptions.h"


#include <QIcon>
#include <QPixmap>
#include <QStylePainter>
#include <QStyleOptionToolButton>

//___________________________________________________________________
ToolButton::ToolButton( QWidget* parent ):
    QToolButton( parent ),
    Counter( QStringLiteral("ToolButton") )
{

    Debug::Throw( QStringLiteral("ToolButton::ToolButton.\n") );

    // auto-raise
    setAutoRaise( true );

    // configuration
    connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &ToolButton::_updateConfiguration );
    _updateConfiguration();

}

//______________________________________________________________________
bool ToolButton::rotate( Pixmap::Rotation value )
{
    Debug::Throw( QStringLiteral("ToolButton::rotate.\n") );
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
        Pixmap pixmap( source.pixmap( size, mode, state ) );
        if( pixmap.isNull() ) continue;

        // clockwise rotations
        if(
            ( rotation_ == Pixmap::Rotation::None && value == Pixmap::Rotation::CounterClockwise ) ||
            ( rotation_ == Pixmap::Rotation::Clockwise && value == Pixmap::Rotation::None ) )
        {
            copy.addPixmap( pixmap.rotated( Pixmap::Rotation::Clockwise ), mode, state );
        } else if(
            ( rotation_ == Pixmap::Rotation::None && value == Pixmap::Rotation::Clockwise ) ||
            ( rotation_ == Pixmap::Rotation::CounterClockwise && value == Pixmap::Rotation::None ) )
        {
            copy.addPixmap( pixmap.rotated( Pixmap::Rotation::CounterClockwise ), mode, state );
        }

    }

    setIcon( copy );
    rotation_ = value;
    return true;
}

//______________________________________________________________________
QSize ToolButton::sizeHint() const
{
    QSize size( QToolButton::sizeHint() );
    if( rotation_ != Pixmap::Rotation::None ) size.transpose();
    return size;
}

//______________________________________________________________________
void ToolButton::paintEvent( QPaintEvent* event )
{

    // default implementation if not rotated
    if( rotation_ == Pixmap::Rotation::None )
    {
        QToolButton::paintEvent( event );
        return;
    }

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
    if( rotation_ == Pixmap::Rotation::CounterClockwise )
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
}

//_________________________________________________________________
void ToolButton::_updateConfiguration()
{
    Debug::Throw( QStringLiteral("ToolButton::_updateConfiguration.\n"));
    if( !updateFromOptions_ ) return;
    int iconSize( XmlOptions::get().get<int>( QStringLiteral("TOOLBUTTON_ICON_SIZE") ) );
    if( iconSize <= 0 ) iconSize = style()->pixelMetric( QStyle::PM_ToolBarIconSize );
    setIconSize( QSize( iconSize, iconSize ) );

    const int toolButtonTextPosition( XmlOptions::get().get<int>( QStringLiteral("TOOLBUTTON_TEXT_POSITION") ) );
    if( toolButtonTextPosition < 0 ) setToolButtonStyle( static_cast<Qt::ToolButtonStyle>( style()->styleHint( QStyle::SH_ToolButtonStyle ) ) );
    else setToolButtonStyle( static_cast<Qt::ToolButtonStyle>( toolButtonTextPosition ) );

    adjustSize();
}
