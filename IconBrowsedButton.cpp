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
  \file IconBrowsedButton.cc
  \brief icon browser. The icon is displayed as the button label
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <sstream>
#include <qbitmap.h>
#include <qimage.h>

#include "CustomPixmap.h"
#include "File.h"
#include "IconBrowsedButton.h"
#include "CustomFileDialog.h"
#include "XmlOptions.h"
#include "QtUtil.h"

using namespace std;

//_____________________________________________
const std::string IconBrowsedButton::NO_ICON = "none";
const unsigned int IconBrowsedButton::icon_size_ = 48;
    
//_____________________________________________
IconBrowsedButton::IconBrowsedButton( QWidget* parent, const File& file):
  QPushButton( parent ),
  Counter( "IconBrowsedButton" ),
  icon_file_( NO_ICON )
{ 
  setIconSize( QSize(icon_size_,icon_size_) );
  setIconFile( file, false ); 
  setFixedSize( QSize( icon_size_+8, icon_size_+8 ) );
  connect( this, SIGNAL( clicked() ), SLOT( _browse() ) );
}
  

//_____________________________________________
void IconBrowsedButton::setIconFile( const File& file, const bool& check )
{
  
  Debug::Throw() << "IconBrowsedButton::setIconFile - " << file << endl;
  
  CustomPixmap pixmap( file );
  if( !pixmap.isNull() ) 
  {
    icon_file_ = file;
    
    // resize pixmap
    if( pixmap.size() != QSize( icon_size_, icon_size_ ) )
    pixmap = pixmap.scale( QSize( icon_size_, icon_size_ ) );
    
    setIcon( pixmap );
    return;
  }
  
  if( check )
  {
    ostringstream what;
    what << "invalid icon file " << file;  
    QtUtil::infoDialog( this, what.str() );
  }
  
  if( no_icon_pixmap_.isNull() ) {
    no_icon_pixmap_ = CustomPixmap().empty( QSize( icon_size_, icon_size_ ) );
    setIcon( no_icon_pixmap_ );
  }
      
  return;
  
} 

//_____________________________________________
void IconBrowsedButton::_browse( void )
{
  Debug::Throw( "IconBrowsedButton::_Browse.\n" );
     
  // create file dialog
  CustomFileDialog dialog( this );
  dialog.setFileMode( QFileDialog::AnyFile );
  
  // set dialog working directory from file, if valid
  if( icon_file_.path().exists() ) 
  { dialog.setDirectory( icon_file_.path().c_str() ); }

  // run dialog
  if( dialog.exec() != QDialog::Accepted ) return; 
    
  QStringList files( dialog.selectedFiles() );
  if( files.size() < 1 ) return;
  
  setIconFile( File( qPrintable( files.front() ) ), true );
 
  return; 
}
