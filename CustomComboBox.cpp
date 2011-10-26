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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
\file CustomComboBox.cpp
\brief Customized QCombobox to handle case sensitive auto completion
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <cassert>
#include <QAbstractItemView>
#include <QCompleter>

#include "CustomComboBox.h"
#include "TransitionWidget.h"


using namespace Qt;

//___________________________________________________
CustomComboBox::CustomComboBox( QWidget* parent ):
    QComboBox( parent ),
    Counter( "CustomComboBox" ),
    editor_( 0 )
{

    Debug::Throw( "CustomComboBox::CustomComboBox.\n" );
    setAutoCompletion( false );

    // size policy
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );

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
void CustomComboBox::setAutoCompletion( bool value, Qt::CaseSensitivity case_sensitivity )
{
    Debug::Throw( "CustomComboBox::setAutoCompletion.\n" );

    if( value )
    {
        assert( isEditable() );

        if( !completer() ) setCompleter( new QCompleter( this ) );

        completer()->setCaseSensitivity( case_sensitivity );

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
