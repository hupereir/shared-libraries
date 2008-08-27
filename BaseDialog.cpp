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

#include "BaseDialog.h"
#include "Debug.h"
#include "QtUtil.h"

using namespace std;

BaseDialog::BaseDialog( QWidget* parent, Qt::WFlags flags ):
  QDialog( parent, flags ),
  size_watcher_( this )
{ 
  Debug::Throw( "BaseDialog::BaseDialog.\n" );
  setSizeGripEnabled ( true );
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
void BaseDialog::centerOnDesktop( void )
{
  
  Debug::Throw( "BaseDialog::centerOnDesktop" );
  move( QtUtil::centerOnDesktop( sizeHint() ) );
  
}

//________________________________________________________________
void BaseDialog::centerOnWidget( QWidget* parent )
{
  
  Debug::Throw( "BaseDialog::centerOnWidget" );
  move( QtUtil::centerOnWidget( sizeHint(), parent ) );
  
}

//____________________________________________________________
void BaseDialog::resizeEvent( QResizeEvent* event )
{
  size_watcher_.restart(); 
  return QDialog::resizeEvent( event );
}
