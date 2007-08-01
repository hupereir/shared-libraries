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
  \file IconProvider.cc
  \brief Icon provider based on configuration pixmap path
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "IconProvider.h"
#include "IconEngine.h"
#include "CustomPixmap.h"
#include "Debug.h"
#include "XmlOptions.h"

using namespace std;

//_____________________________________________
map< QFileIconProvider::IconType, QIcon > IconProvider::icons_;

//_____________________________________________
QIcon IconProvider::icon( QFileIconProvider::IconType type ) const
{
  
  Debug::Throw() << "IconProvider::icon - type: " << type << endl;
  
  std::map<IconType, QIcon>::iterator iter( icons_.find( type ) );
  if( iter != icons_.end() ) return iter->second;
  
  string filename;
  
  switch( type )
  {

//     case Computer:
//     case Desktop:
//     case Network:
//     case Drive:
    
    case Trashcan: 
    filename = "trashcan_empty.png";
    break;
    
    case Folder: 
    filename = "folder.png"; 
    break;
    
    case File:
    filename = "filenew.png";
    break;
    
    default: break;
  }

  QIcon out;
  if( !filename.empty() )
  {
    Debug::Throw() << "IconProvider::icon - file: " << filename << endl;
    
    // load icon path
    list<string> path_list( XmlOptions::get().specialOptions<string>( "PIXMAP_PATH" ) );
    out = QIcon( CustomPixmap().find( filename, path_list ) );
  }
  
  if( out.isNull() ) out = QFileIconProvider::icon( type );
  
  out = IconEngine::get( out );
  icons_.insert( make_pair( type, out ) );
  return out;
}


//____________________________________________________
QIcon IconProvider::icon( const QFileInfo& info ) const
{
  
  Debug::Throw( "IconProvider::icon (FileInfo).\n" );
  if( info.isDir() ) return icon( Folder );
  else if( info.isFile() ) return icon( File );
  else return IconEngine::get( QFileIconProvider::icon( info ) );

}
