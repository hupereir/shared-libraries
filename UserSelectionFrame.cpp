
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

/*!
\file UserSelectionFrame.cc
\brief current user
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <QLayout>
#include <QLabel>

#include "Util.h"
#include "UserSelectionFrame.h"




//____________________________________________________________
UserSelectionFrame::UserSelectionFrame( QWidget* parent ):
QWidget( parent ),
Counter( "UserSelectionFrame" ),
delay_( 500 )
{

    Debug::Throw( "UserSelectionFrame::UserSelectionFrame\n" );
    setLayout( new QHBoxLayout() );
    layout()->setMargin(0);
    layout()->setSpacing(5);
    layout()->addWidget( editor_ = new CustomComboBox( this ) );

    editor().setMinimumSize( QSize( 100, 0 ) );
    editor().setEditable( true );
    editor().setAutoCompletion( true );
    editor().setToolTip( "User selection" );

    // send appropriate signal directly
    connect( &editor(), SIGNAL( activated( const QString & ) ), SLOT( _userChanged() ) );

    // one must add a timer here so that the signal gets
    // emmitted only after some delay, to avoid to many signals are sent when typing
    connect( &editor(), SIGNAL( editTextChanged( const QString & ) ),  SLOT( _delayedUserChanged() ) );

}

//____________________________________________________________
std::set<QString> UserSelectionFrame::users( void ) const
{

    std::set<QString> out;
    for( int i=0; i< editor().QComboBox::count(); i++ )
    {
        const QString& user( editor().itemText( i ) );
        if( !user.isNull() ) out.insert( user );
    }

    return out;

}

//____________________________________________________________
void UserSelectionFrame::setUser( const QString& user )
{
    Debug::Throw() << "UserSelectionFrame::set - user: " << user << endl;
    if( user.isNull() || user.isEmpty() ) return;

    for( int i=0; i< editor().QComboBox::count(); i++ )
    {
        if( user == editor().itemText( i ) )
        {
            editor().setCurrentIndex(i);
            return;
        }
    }

    editor().addItem( user );
    editor().setEditText( user );

    return;
}

//________________________________________________________________
void UserSelectionFrame::updateUsers( std::set<QString> users )
{
    Debug::Throw( "UserSelectionFrame::updateUsers.\n" );

    // make sure default user and current user are in the list
    users.insert( user() );

    // remove items which are not in the set of users
    for( int i=0; i< editor().QComboBox::count(); )
    {

        const QString& user( editor().itemText( i ) );
        if( user.isNull() )
        {
            editor().removeItem( i );
            continue;
        }

        std::set<QString>::iterator iter( users.find( user ) );
        if( iter == users.end() ) editor().removeItem( i );
        else {

            users.erase( iter );
            i++;

        }
    }

    for( std::set<QString>::const_iterator iter =  users.begin(); iter != users.end(); ++iter )
    { if( !iter->isEmpty() ) editor().addItem( *iter ); }

}

//____________________________________________________________
QString UserSelectionFrame::user( void ) const
{
    Debug::Throw( "UserSelectionFrame::user.\n" );
    QString user(  editor().currentText() );
    if( user.isNull() || user.isEmpty() ) return "";
    return user.trimmed();
}

//____________________________________________________________
void UserSelectionFrame::_userChanged( void )
{
    QString new_user( user() );
    if( new_user == user_ ) return;
    user_ = new_user;
    emit userChanged( user_ );
}

