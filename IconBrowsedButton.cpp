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
  \file IconBrowsedButton.cpp
  \brief icon browser. The icon is displayed as the button label
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <sstream>
#include <qbitmap.h>
#include <qimage.h>

#include "LineEditor.h"
#include "CustomPixmap.h"
#include "File.h"
#include "IconSize.h"
#include "IconBrowsedButton.h"
#include "IconFileDialog.h"
#include "InformationDialog.h"
#include "XmlOptions.h"
#include "QtUtil.h"

using namespace std;

//_____________________________________________
const std::string IconBrowsedButton::NO_ICON = "none";
    
//_____________________________________________
IconBrowsedButton::IconBrowsedButton( QWidget* parent, const File& file):
  CustomToolButton( parent ),
  file_( NO_ICON )
{ 
  setIconSize( IconSize( IconSize::HUGE ) );
  setAutoRaise( false );
  setFile( file, false ); 
  connect( this, SIGNAL( clicked() ), SLOT( _browse() ) );
}
  

//_____________________________________________
void IconBrowsedButton::setFile( const File& file, const bool& check )
{
  
  Debug::Throw() << "IconBrowsedButton::setFile - " << file << endl;
  
  CustomPixmap pixmap( file );
  
  // update file if pixmap is valid or current file is undefined
  if( !pixmap.isNull() || file_ == NO_ICON )
  { file_ = file; }
  
  // update pixmap if valid
  if( !pixmap.isNull() ) 
  {
    
    // resize pixmap
    if( pixmap.size() != IconSize( IconSize::HUGE ) )
    pixmap = pixmap.scaled( IconSize( IconSize::HUGE ), Qt::KeepAspectRatio, Qt::SmoothTransformation );
    
    setIcon( pixmap );
    return;
  }
  
  // popup dialog if invalid
  if( check )
  {
    ostringstream what;
    what << "invalid icon file " << file;  
    InformationDialog( this, what.str().c_str() ).exec();
  }
  
  // if file, set pixmap to empty
  if( no_icon_pixmap_.isNull() ) {
    no_icon_pixmap_ = CustomPixmap().empty( IconSize( IconSize::HUGE ) );
    setIcon( no_icon_pixmap_ );
  }
      
  return;
  
} 

//_____________________________________________
void IconBrowsedButton::_browse( void )
{
  Debug::Throw( "IconBrowsedButton::_Browse.\n" );
     
  IconFileDialog dialog( this );
  QtUtil::centerOnParent( &dialog );
  
  if( file_ != NO_ICON ) { 
    File path( file_.path() );
    if( path.exists() && path.isDirectory() ) dialog.setDirectory( file_.path().c_str() );
    dialog.selectFile( file_.c_str() ); 
  }
  
  if( dialog.exec() == QDialog::Rejected ) return;

  // retrieve selected files
  QStringList files( dialog.selectedFiles() );
    
  // check file size
  if( files.size() > 1 ) 
  {
    InformationDialog( this, "Too many files selected." ).exec();
    return;
  }
  
  if( files.size() < 1 )
  {
    InformationDialog( this, "No file selected." ).exec();
    return;
  }
    
  setFile( File( qPrintable( files.front() ) ), true );
  return; 
}
