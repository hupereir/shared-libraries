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

#include "CustomComboBox.h"
#include "TransitionWidget.h"

#include <QAbstractItemView>
#include <QCompleter>

//___________________________________________________
CustomComboBox::CustomComboBox( QWidget* parent ):
    QComboBox( parent ),
    Counter( "CustomComboBox" ),
    editor_( 0 )
{

    Debug::Throw( "CustomComboBox::CustomComboBox.\n" );
    setAutoCompletion( false );

    // size policy
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

}

//____________________________________________________
void CustomComboBox::setEditable( bool value )
{
    Debug::Throw( "CustomComboBox::setEditable.\n" );
    QComboBox::setEditable( value );

    if( !value )
    {
        if( editor_ ) delete editor_;
        return;
    }

    if( !editor_ )
    {
        editor_ = new ComboLineEdit( this );
        editor_->setFrame( false );
        setLineEdit( editor_ );
        connect( view(), SIGNAL( pressed( QModelIndex ) ), editor_, SLOT( startAnimation( void ) ) );
        connect( view(), SIGNAL( activated( QModelIndex ) ), editor_, SLOT( startAnimation( void ) ) );
    }

}

//____________________________________________________
void CustomComboBox::setAutoCompletion( bool value, Qt::CaseSensitivity caseSensitivity )
{
    Debug::Throw( "CustomComboBox::setAutoCompletion.\n" );

    if( value )
    {
        Q_ASSERT( isEditable() );

        if( !completer() ) setCompleter( new QCompleter( this ) );

        completer()->setCaseSensitivity( caseSensitivity );

    } else {

        setCompleter(0);

    }

    return;

}

//____________________________________________________
void ComboLineEdit::startAnimation( void )
{
    Debug::Throw( "ComboLineEdit::startAnimation.\n" );
    if( !( _transitionWidget().isEnabled() && isVisible() ) ) return;
    _transitionWidget().initialize();
    _transitionWidget().start();
}
