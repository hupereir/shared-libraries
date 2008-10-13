#ifndef UserSelectionFrame_h
#define UserSelectionFrame_h

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
  \file UserSelectionFrame.h
  \brief current user
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <set>
#include <string>
#include <QTimer>

#include "Counter.h"
#include "CustomComboBox.h"

class UserSelectionFrame: public QWidget, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT
    
  public:
   
  //! constructor
  UserSelectionFrame( QWidget* parent );
     
  //! set user
  void setUser( const QString& user );

  //! users
  std::set<std::string> users( void ) const;
  
  //! selected user
  std::string user( void ) const;  
  
  //! editor
  CustomComboBox& editor( void ) const
  { 
    assert( editor_ );
    return *editor_;
  }
  
  signals:
  
  //! emitted when user is changed
  void userChanged( std::string );
  
  public slots:
  
  //! update user list
  void updateUsers( std::set<std::string> );

  private slots:
  
  //! selected user changed
  void _userChanged( void );

  //! selected user changed
  void _delayedUserChanged( void )
  { _timer().start( delay_ ); }
  
  private:

  //! timer
  QTimer &_timer( void )
  { return timer_; }
  
  //! delay for userChanged signal emission
  static const unsigned int delay_;
  
  //! user changed timer
  QTimer timer_;
  
  //! user line_edit
  CustomComboBox* editor_;
      
  //! current user 
  /*! it is used to avoid emmitting signal when user was changed but fall back to the current one*/
  std::string user_;
  
};

#endif
