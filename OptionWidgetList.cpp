
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

#include "OptionWidgetList.h"

#include <QApplication>
#include <QStyle>
#include <QStyleOption>

//______________________________________________________________________
void OptionWidgetList::addOptionWidget( OptionWidget* widget )
{

    Debug::Throw()
        << "OptionWidgetList::addOptionWidget -"
        << " buddy: " << ( widget->hasBuddy() ? widget->buddy().metaObject()->className():"none" )
        << endl;

    optionWidgets_ << widget;

    //! connect signals
    if( _connected() && hasBuddy() && widget->hasBuddy() )
    {
        QObject::connect( &widget->buddy(), SIGNAL(modified()), &buddy(), SIGNAL(modified()));

        if( Debug::level() > 0 )
        { QObject::connect( &widget->buddy(), SIGNAL(modified()), &buddy(), SLOT(debugModification())); }
    }

}

//______________________________________________________________________
void OptionWidgetList::read( const Options& options )
{

    Debug::Throw( "OptionWidgetList::read.\n" );
    foreach( OptionWidget* widget, optionWidgets_ )
    {
        widget->read( options );
        if( !_connected() && hasBuddy() && widget->hasBuddy() )
        {
            QObject::connect( &widget->buddy(), SIGNAL(modified()), &buddy(), SIGNAL(modified()));

            if( Debug::level() > 0 )
            { QObject::connect( &widget->buddy(), SIGNAL(modified()), &buddy(), SLOT(debugModification())); }
        }
    }

    _setConnected();

}

//______________________________________________________________________
void OptionWidgetList::write( Options& options ) const
{
    Debug::Throw( "OptionWidgetList::write\n" );
    foreach( OptionWidget* widget, optionWidgets_ )
    { widget->write( options ); }
}

//__________________________________________________
int OptionWidgetList::_checkBoxSpacing( void ) const
{
    QStyleOption option;
    option.rect = QRect( 0, 0, 50, 50 );
    QRect adjusted( qApp->style()->subElementRect( QStyle::SE_CheckBoxContents, &option, 0x0 ) );
    return adjusted.left()-option.rect.left();
}
