
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

using namespace std;


//____________________________________________________________
const unsigned int UserSelectionFrame::delay_ = 500; 

//____________________________________________________________
UserSelectionFrame::UserSelectionFrame( QWidget* parent ):
  QWidget( parent ),
  Counter( "UserSelectionFrame" ),
  timer_( this )
{
  
  Debug::Throw( "UserSelectionFrame::UserSelectionFrame\n" );
  setLayout( new QHBoxLayout() );
  layout()->setMargin(0);
  layout()->setSpacing(5);
  layout()->addWidget( editor_ = new CustomComboBox( this ) );
  
  _editor().setMinimumSize( QSize( 100, 0 ) ); 
  _editor().setAutoCompletion( true );
  _editor().setEditable( true );
  _editor().setToolTip( "User selection" );
  
  _timer().setSingleShot( true );
  connect( &_timer(), SIGNAL( timeout() ), SLOT( _userChanged() ) );
  
  // send appropriate signal directly
  connect( &_editor(), SIGNAL( activated( const QString & ) ), SLOT( _userChanged() ) );
  
  // one must add a timer here so that the signal gets
  // emmitted only after some delay, to avoid to many signals are sent when typing
  connect( &_editor(), SIGNAL( editTextChanged( const QString & ) ),  SLOT( _delayedUserChanged() ) );

}

//____________________________________________________________
void UserSelectionFrame::setUser( const QString& user )
{ 
  Debug::Throw() << "UserSelectionFrame::set - user: " << qPrintable(user) << endl;
  if( user.isNull() || user.isEmpty() ) return;
  
  for( int i=0; i< _editor().QComboBox::count(); i++ )
  {
    if( user == _editor().itemText( i ) )
    { 
      _editor().setCurrentIndex(i);
      return;
    }
  }
  
  _editor().addItem( user );
  _editor().setEditText( user );

  return;
}

//________________________________________________________________
void UserSelectionFrame::updateUsers( set<string> users )
{
  Debug::Throw( "UserSelectionFrame::updateUsers.\n" );
    
  // make sure default user and current user are in the list
  users.insert( user() );

  // remove items which are not in the set of users
  for( int i=0; i< _editor().QComboBox::count(); )
  {
    
    const QString& user( _editor().itemText( i ) );
    if( user.isNull() ) 
    {
      _editor().removeItem( i );
      continue;
    }
        
    set<string>::iterator iter( users.find( qPrintable( user ) ) );
    if( iter == users.end() ) _editor().removeItem( i );
    else {
      
      users.erase( iter );
      i++;  
    
    }
  }

  for( set<string>::const_iterator iter =  users.begin(); iter != users.end(); iter++ )
  { if( !iter->empty() ) _editor().addItem( iter->c_str() ); }
  
}

//____________________________________________________________
string UserSelectionFrame::user( void ) const
{ 
  Debug::Throw( "UserSelectionFrame::user.\n" );
  QString user(  _editor().currentText() );
  if( user.isNull() || user.isEmpty() ) return "";
  return qPrintable( user.trimmed() ); 
}

//____________________________________________________________
void UserSelectionFrame::_userChanged( void )
{
  std::string new_user( user() );
  if( new_user == user_ ) return;
  user_ = new_user;
  emit userChanged( user_ ); 
}
  
