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

#include "ColorMenu.h"

#include "BaseIcons.h"
#include "IconEngine.h"
#include "IconSize.h"

#include <QColorDialog>
#include <QPainter>

//________________________________________________
const QString ColorMenu::NONE = "None";

//_______________________________________________
ColorMenu::ColorMenu( QWidget* parent ):
    QMenu( parent ),
    Counter( "ColorMenu" )
{
    Debug::Throw( "ColorMenu::ColorMenu.\n" );
    connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _selected( QAction* ) ) );
    connect( this, SIGNAL( aboutToShow() ), SLOT( _display() ) );
}

//_______________________________________________
void ColorMenu::add( const QString& colorname )
{
    Debug::Throw() << "ColorMenu::add - name: " << colorname << endl;
    if( colorname.compare( NONE, Qt::CaseInsensitive ) != 0 ) _add( QColor( colorname ) );
}

//_______________________________________________
ColorMenu::ColorSet ColorMenu::colors( void ) const
{

    ColorSet out;
    for( ColorMap::const_iterator iter = colors_.begin(); iter != colors_.end(); ++iter )
    { out.insert( iter.key() ); }

    return out;
}

//_______________________________________________
void ColorMenu::paintEvent( QPaintEvent* event )
{

    static const int margin = 5;

    // default paint
    QMenu::paintEvent( event );

    // loop over actions associated to existing colors
    QPainter painter( this );
    painter.setClipRect( event->rect() );

    painter.setPen( Qt::NoPen );
    for( ActionMap::iterator iter = actions_.begin(); iter != actions_.end(); ++iter )
    {
        QRect action_rect( actionGeometry( iter.key() ) );
        if( !event->rect().intersects( action_rect ) ) continue;
        action_rect.adjust( 2*margin, margin+1, -2*margin-1, -margin );
        painter.setBrush( colors_[iter.value().name()] );
        painter.setRenderHints(QPainter::Antialiasing );
        painter.drawRoundedRect( action_rect, 4, 4 );
    }

    painter.end();

}

//_______________________________________________
void ColorMenu::_display( void )
{

    Debug::Throw( "ColorMenu::_display.\n" );

    // clear menu
    clear();

    // new color action
    addAction( IconEngine::get( ICONS::ADD ), "&New", this, SLOT( _new() ) );

    // default color action
    addAction( "&Default", this, SLOT( _default() ) );

    // clear actions
    actions_.clear();

    for( ColorMap::iterator iter = colors_.begin(); iter != colors_.end(); ++iter )
    {

        // create pixmap if not done already
        if( iter.value() == Qt::NoBrush ) iter.value() = QColor( iter.key() );

        // create action
        QAction* action = new QAction( this );
        actions_.insert( action, iter.key() );
        addAction( action );

    };

    return;

}

//_______________________________________________
void ColorMenu::_new( void )
{

    Debug::Throw( "ColorMenu::_new.\n" );
    QColor color( QColorDialog::getColor( Qt::white, this ) );
    if( color.isValid() )
    {
        _add( color );
        lastColor_ = color;
        emit selected( color );
    }

}

//_______________________________________________
void ColorMenu::_default( void )
{

    Debug::Throw( "ColorMenu::_default.\n" );
    lastColor_ = QColor();
    emit selected( QColor() );

}

//_______________________________________________
void ColorMenu::_selected( QAction* action )
{
    Debug::Throw( "ColorMenu::_selected.\n" );
    ActionMap::const_iterator iter = actions_.find( action );
    if( iter != actions_.end() )
    {
        lastColor_ = iter.value();
        emit selected( iter.value() );
    }
}

//_______________________________________________
void ColorMenu::_add( const QColor& color )
{

    if( color.isValid() && !colors_.contains( color.name() ) )
    { colors_.insert( color.name(), QBrush() ); }

    return;
}
