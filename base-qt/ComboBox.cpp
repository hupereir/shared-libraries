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

#include "ComboBox.h"
#include "LineEditor.h"


#include <QAbstractItemView>
#include <QCompleter>

//___________________________________________________
ComboBox::ComboBox( QWidget* parent ):
    QComboBox( parent ),
    Counter( QStringLiteral("ComboBox") )
{

    Debug::Throw( QStringLiteral("ComboBox::ComboBox.\n") );
    setAutoCompletion( false );

    // size policy
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

}

//____________________________________________________
ComboBox::~ComboBox() = default;

//____________________________________________________
void ComboBox::setEditable( bool value )
{
    Debug::Throw( QStringLiteral("ComboBox::setEditable.\n") );
    QComboBox::setEditable( value );
    if( !value ) editor_.reset();
    else if( !editor_ ) {
        editor_.reset( new LineEditor( this ) );
        editor_->setFrame( false );
        setLineEdit( editor_.get() );
    }

}

//____________________________________________________
void ComboBox::setAutoCompletion( bool value, Qt::CaseSensitivity caseSensitivity )
{
    Debug::Throw( QStringLiteral("ComboBox::setAutoCompletion.\n") );

    if( isEditable() )
    {
        if( value )
        {
            if( !completer() ) setCompleter( new QCompleter( this ) );
            completer()->setCaseSensitivity( caseSensitivity );
        } else setCompleter( nullptr );
    }
    return;

}

//___________________________________________________
void ComboBox::keyPressEvent( QKeyEvent* event )
{
    if( !navigationEnabled_ )
    {
        switch( event->key() )
        {
            case Qt::Key_Up:
            case Qt::Key_PageUp:
            case Qt::Key_Down:
            case Qt::Key_PageDown:
            if( event->modifiers() ) break;
            else return;

            default: break;

        }
    }

    QComboBox::keyPressEvent( event );
}
