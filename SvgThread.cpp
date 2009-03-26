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
* Place, Suite 330, Boston, MA 02111-1307 USA                           
*                         
*                         
*******************************************************************************/

/*!
  \file SvgThread.cpp
  \brief check validity of a set of files
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <algorithm>
#include <QApplication>
#include "SvgThread.h"
#include "File.h" 

using namespace std;

namespace SVG
{
  
  //______________________________________________________
  QEvent::Type SvgEvent::eventType( void )
  {
    
    #if QT_VERSION >= 0x040400
    static QEvent::Type event_type = (QEvent::Type) QEvent::registerEventType();
    #else
    static QEvent::Type event_type = QEvent::User;
    #endif
    
    return event_type;
    
  }
  
  //______________________________________________________
  void SvgThread::run( void )
  {
    
    if( !svg_.isValid() ) return;
    if( sizes_.empty() ) return;
    
    SvgImage::Cache cache;
    for( std::vector<QSize>::const_iterator iter = sizes_.begin(); iter != sizes_.end(); iter++ )
    {

      SvgImage image( *iter );
      image.render( svg_, svg_offset_ );
      cache.insert( make_pair( *iter, image ) );
      
    }
    
    qApp->postEvent( reciever_, new SvgEvent( cache ) );  
    return;
    
  }
  
};
