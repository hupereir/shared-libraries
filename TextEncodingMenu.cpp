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

#include "TextEncodingMenu.h"

#include "Debug.h"

#include <QActionGroup>
#include <QTextCodec>

//__________________________________________________________
TextEncodingMenu::TextEncodingMenu( QWidget* parent ):
    QMenu( parent ),
    Counter( "TextEncodingMenu" )
{

    Debug::Throw( "TextEncodingMenu::TextEncodingMenu.\n" );
    setTitle( tr( "Text Encoding" ) );

    QActionGroup *group = new QActionGroup( this );
    connect( group, SIGNAL(triggered(QAction*)), SLOT(_selected(QAction*)) );

    QList<QByteArray> codecs( QTextCodec::availableCodecs() );
    _removeAliases( codecs );

    foreach( const QByteArray& value, codecs )
    {
        QAction* action = new QAction( value, this );
        addAction( action );
        action->setCheckable( true );
        actions_.insert( action, value );
        group->addAction( action );
    }

}

//_____________________________________________________________________________
void TextEncodingMenu::select( const QByteArray& value )
{

    Debug::Throw() << "TextEncodingMenu::select - encoding: " << value << endl;
    for( ActionMap::const_iterator iter = actions_.begin(); iter != actions_.end(); ++iter )
    {
        if( iter.value() == value )
        {
            iter.key()->setChecked( true );
            return;
        }
    }

}

//_____________________________________________________________________________
void TextEncodingMenu::_selected( QAction* action )
{

    Debug::Throw( "TextEncodingMenu::_selected.\n" );

    // find matching actions
    ActionMap::const_iterator iter = actions_.find( action );
    Q_ASSERT( iter != actions_.end() );
    emit encodingChanged( iter.value() );

}

//_____________________________________________________________________________
void TextEncodingMenu::_removeAliases( QList<QByteArray>& codecs ) const
{
    bool done( false );
    while( !done )
    {
        done = true;
        foreach( const QByteArray& codecName, codecs )
        {

            // create codec
            QTextCodec* codec = QTextCodec::codecForName( codecName );
            foreach( const QByteArray& aliasName,  codec->aliases()  )
            {
                if( aliasName == codecName ) continue;
                if( codecs.contains( aliasName ) )
                {
                    done = false;
                    codecs.removeAll( aliasName );
                }
            }

            if( !done ) break;

        }

    }

}
