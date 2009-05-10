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
  \file BaseDialog.cpp
  \brief customized QDialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QTextStream>
#include <QWindowStateChangeEvent>

#include "BaseDialog.h"
#include "Debug.h"
#include "QtUtil.h"
#include "Util.h"

using namespace std;

//__________________________________________________
BaseDialog::BaseDialog( QWidget* parent, Qt::WFlags flags ):
  QDialog( parent, flags ),
  size_watcher_( this ),
  was_maximized_( false )
{ 
  Debug::Throw( "BaseDialog::BaseDialog.\n" );
  setSizeGripEnabled ( true );
}

//__________________________________________________
BaseDialog::~BaseDialog( void )
{ Debug::Throw( "BaseDialog::~BaseDialog.\n" ); }

//__________________________________________________
void BaseDialog::setWindowTitle( const QString& title )
{
  
  Debug::Throw( "BaseDialog::setWindowTitle.\n" );
  QString host( Util::host() );
  if( host == "localhost" ) QDialog::setWindowTitle( title );
  else {
    QString buffer;
    QTextStream( &buffer ) << title << " [" << Util::host() << "]";
    QDialog::setWindowTitle( buffer );
  }
  
}

//__________________________________________________
QSize BaseDialog::minimumSizeHint( void ) const
{
  QSize out( size_watcher_.sizeHint() );
  return out.isValid() ? out:QDialog::minimumSizeHint();
}

//__________________________________________________
QSize BaseDialog::sizeHint( void ) const
{
  QSize out( size_watcher_.sizeHint() );
  return out.isValid() ? out:QDialog::sizeHint();
}

//________________________________________________________________
BaseDialog& BaseDialog::centerOnPointer( void )
{
  
  Debug::Throw( "BaseDialog::centerOnPointer" );
  move( QtUtil::centerOnPointer( sizeHint() ) );
  return *this;
}

//________________________________________________________________
BaseDialog& BaseDialog::centerOnDesktop( void )
{
  
  Debug::Throw( "BaseDialog::centerOnDesktop" );
  move( QtUtil::centerOnDesktop( sizeHint() ) );
  return *this;
  
}

//________________________________________________________________
BaseDialog& BaseDialog::centerOnWidget( QWidget* parent )
{
  
  Debug::Throw( "BaseDialog::centerOnWidget" );
  move( QtUtil::centerOnWidget( sizeHint(), parent ) );
  return *this;
  
}

//________________________________________________________________
BaseDialog& BaseDialog::uniconify( void )
{ 

  Debug::Throw( "BaseDialog::uniconify" );
  if( isMinimized() ) 
  {
    
    if( _wasMaximized() ) showMaximized();
    else showNormal();
    
  } else if( isHidden() ) show();
  
  activateWindow();
  raise();
  return *this;
  
}

//_______________________________________________________
bool BaseDialog::event( QEvent* event )
{
    
  // check that all needed widgets/actions are valid and checked.
  switch (event->type()) 
  {
          
    case QEvent::WindowStateChange:
    {
      // cast
      QWindowStateChangeEvent *state_event( static_cast<QWindowStateChangeEvent*>(event) );
      
      if( windowState() & Qt::WindowMinimized )
      { _setWasMaximized( state_event->oldState() & Qt::WindowMaximized ); }
      
    }
    break;
        
    default: break;
  }
  
  return QDialog::event( event );
  
}

//____________________________________________________________
void BaseDialog::resizeEvent( QResizeEvent* event )
{
  size_watcher_.restart(); 
  return QDialog::resizeEvent( event );
}
