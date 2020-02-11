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

#include "TextEncodingMenu.h"

#include "CppUtil.h"
#include "Debug.h"
#include "TextEncodingString.h"

#include <QActionGroup>
#include <QTextCodec>

//__________________________________________________________
TextEncodingMenu::TextEncodingMenu( QWidget* parent ):
    QMenu( parent ),
    Counter( QStringLiteral("TextEncodingMenu") )
{

    Debug::Throw( QStringLiteral("TextEncodingMenu::TextEncodingMenu.\n") );
    setTitle( tr( "Text Encoding" ) );

    auto group = new QActionGroup( this );
    connect( group, &QActionGroup::triggered, this, &TextEncodingMenu::_selected );

    // use MIBs to get rid of aliases
    QVector<TextEncodingString> codecStrings;
    for( const auto& codecId:QTextCodec::availableMibs() )
    {
        if( auto codec = QTextCodec::codecForMib( codecId ) )
        { codecStrings.append( TextEncodingString( codec->name() ) ); }
    }

    // store in menu
    std::sort( codecStrings.begin(), codecStrings.end() );
    for( const auto& codecString:codecStrings )
    {

        const auto value( qPrintable( codecString.get() ) );
        QAction* action = new QAction( value, this );
        addAction( action );
        action->setCheckable( true );
        actions_.insert( action, value );
        group->addAction( action );

    }

}

//_____________________________________________________________________________
void TextEncodingMenu::select( const QByteArray& constValue )
{

    Debug::Throw() << "TextEncodingMenu::select - encoding: " << constValue << endl;

    // find codec matching value and check
    auto codec( QTextCodec::codecForName( constValue ) );
    if( !codec ) return;

    // get 'standard name'
    const auto value( codec->name() );
    const auto iter = Base::findByValue( actions_, value );
    if( iter != actions_.cend() ) iter.key()->setChecked( true );

}

//_____________________________________________________________________________
void TextEncodingMenu::_selected( QAction* action )
{

    Debug::Throw( QStringLiteral("TextEncodingMenu::_selected.\n") );

    // find matching actions
    const auto iter = actions_.find( action );
    emit encodingChanged( iter.value() );

}
