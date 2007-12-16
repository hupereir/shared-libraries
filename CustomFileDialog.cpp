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
  \file CustomFileDialog.cpp
  \brief customized file selection dialog, to store last selected directory
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

using namespace std;

#include "CustomFileDialog.h"
#include "File.h"  
#include "Debug.h"  
#include "Util.h"  

//______________________________________________________________________
File CustomFileDialog::working_directory_( Util::workingDirectory() );

//______________________________________________________________________
CustomFileDialog::CustomFileDialog( QWidget* parent ):
  QFileDialog( parent ),
  Counter( "CustomFileDialog" )
{
  
  Debug::Throw() <<  "CustomFileDialog::CustomFileDialog - working directory: " << working_directory_ << endl;
  if( !working_directory_.empty() && File( working_directory_ ).isDirectory() )
  { setDirectory( QDir( working_directory_.c_str() ) ); }
  
  // set default view mode to list rather than detail because detail sometime has problems
  // with column sizes.
  setViewMode( List );
  connect( this, SIGNAL( currentChanged( const QString& ) ), SLOT( _saveWorkingDirectory( const QString& ) ) );

}  

//______________________________________________________________________
void CustomFileDialog::_saveWorkingDirectory( const QString& directory )
{ 
  Debug::Throw( "CustomFileDialog::_saveWorkingDirectory.\n" );
  working_directory_ = File( qPrintable( directory ) ).path();
}
