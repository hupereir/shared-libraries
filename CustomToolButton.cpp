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
  \file CustomToolButton.cpp
  \brief customized tool button to display tooltip in a dedicated label
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QPixmap>
#include <QPainter>
#include <sstream>

#include "CustomToolButton.h"
#include "Exception.h"
#include "XmlOptions.h"

using namespace std;

//___________________________________________________________________
const QSize CustomToolButton::BigIconSize = QSize( 32, 32 );
const QSize CustomToolButton::SmallIconSize = QSize( 24, 24 );

//___________________________________________________________________
CustomToolButton::CustomToolButton( 
  QWidget* parent,   
  QIcon icon,
  const string& tooltip,
  QLabel* label ):
  QToolButton( parent ),
  Counter( "CustomToolButton" ),
  tooltip_label_( label ),
  big_icon_size_( BigIconSize ),
  small_icon_size_( SmallIconSize )
{

  Debug::Throw( "CustomToolButton::CustomToolButton.\n" );
            
  // add tooltip
  setIcon( icon );
  if( tooltip.size() ) setToolTip( tooltip.c_str() );
    
  setAutoRaise( true );
  
  _updateConfiguration();
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  
}

//___________________________________________________________________
CustomToolButton::CustomToolButton( 
  QWidget* parent,   
  QAction* action,
  QLabel* label ):
  QToolButton( parent ),
  Counter( "CustomToolButton" ),
  tooltip_label_( label ),
  big_icon_size_( BigIconSize ),
  small_icon_size_( SmallIconSize )
{

  Debug::Throw( "CustomToolButton::CustomToolButton.\n" );
            
  // add action
  if( action ) setDefaultAction( action );
  
  // set configuration
  _updateConfiguration();
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  
}

//_________________________________________________________________
void CustomToolButton::_updateConfiguration( void )
{
  Debug::Throw( "CustomToolButton::_updateConfiguration.\n");

  // auto-raise
  setAutoRaise( true );

  // pixmap size
  QToolButton::setIconSize(  XmlOptions::get().get<bool>("USE_BIG_PIXMAP" ) ? bigIconSize():smallIconSize() );
  
  // text labels
  if( XmlOptions::get().get<bool>("USE_TEXT_LABEL" ) ) setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
  else setToolButtonStyle( Qt::ToolButtonIconOnly );
  
  adjustSize();
  
}
