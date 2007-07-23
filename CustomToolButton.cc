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
  \file CustomToolButton.cc
  \brief customized tool button to display tooltip in a dedicated label
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "CustomToolButton.h"
#include "Exception.h"
#include "XmlOptions.h"

#include <QApplication>
#include <qpixmap.h>
#include <qiconset.h>
#include <qtooltip.h>
#include <qpainter.h>
#include <qstyle.h>
#include <sstream>

using namespace std;

//___________________________________________________________________
CustomToolButton::CustomToolButton( 
  QWidget* parent,   
  QIcon icon,
  const string& tooltip,
  QLabel* label ):
  QToolButton( parent ),
  Counter( "CustomToolButton" ),
  tooltip_label_( label )
{

  Debug::Throw( "CustomToolButton::CustomToolButton.\n" );
            
  // add tooltip
  setIcon( icon );
  if( tooltip.size() ) setToolTip( tooltip.c_str() );
    
  setAutoRaise( true );
  
  _updateConfiguration();
  connect( qApp, SIGNAL( configurationChanged() ), this, SLOT( _updateConfiguration() ) );
  
}

//_________________________________________________________________
void CustomToolButton::_updateConfiguration( void )
{
  Debug::Throw( "CustomToolButton::_updateConfiguration.\n");
  
  // pixmap size
  if( XmlOptions::get().get<bool>("USE_BIG_PIXMAP" ) ) setIconSize( QSize( 32, 32 ) );
  else setIconSize( QSize( 24, 24 ) );
  
  // text labels
  if( XmlOptions::get().get<bool>("USE_TEXT_LABEL" ) ) setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
  else setToolButtonStyle( Qt::ToolButtonIconOnly );
  
  adjustSize();
  
}
