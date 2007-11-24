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
  \file CustomSlider.cpp
  \brief customized QSlider associated to a CustomLineEdit
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QHBoxLayout>
#include <sstream>

#include "CustomSlider.h"
#include "Debug.h"
#include "QtUtil.h"
#include "Str.h"

using namespace std;

//_________________________________________________________
CustomSlider::CustomSlider( QWidget* parent ):
  QWidget( parent ),
  Counter( "CustomSlider" )
{  
  Debug::Throw( "CustomSlider::CustomSlider.\n" );
  
  setLayout( new QHBoxLayout() );
  layout()->setMargin(0);
  layout()->setSpacing(5);
  
  layout()->addWidget( slider_ = new QSlider( Qt::Horizontal, this ) );
  layout()->addWidget( line_edit_ = new CustomLineEdit( this ) );
  
  connect( line_edit_, SIGNAL( returnPressed() ),   SLOT( _updateSlider() ) );
  connect( slider_, SIGNAL( valueChanged( int ) ),  SLOT( _updateLineEdit( int ) ) );

  line_edit_->setMaximumSize( QSize( 
    line_edit_->fontMetrics().width("  -100  "), 
    line_edit_->height()) );

}

//_________________________________________________________
void CustomSlider::setValue( int value )
{
  if( value < slider_->minimum() || value > slider_->maximum() ) {
    QtUtil::infoDialog( this, "invalid value" );
    return;
  }
  
  slider_->setValue( value );
  _updateLineEdit( value );
}

//_________________________________________________________
void CustomSlider::_updateSlider( void )
{  
  Debug::Throw( "CustomSlider::_updateSlider.\n" );
  
  // retrieve/check text from line_edit_
  bool error( false );
  int value( Str( qPrintable( line_edit_->text() ) ).get<int>(&error) );
  if( error || value < slider_->minimum() || value > slider_->maximum() ) 
  {
    QtUtil::infoDialog( this, "invalid value" );
    return;
  }
  
  slider_->setValue( value );
  return;
}

//_________________________________________________________
void CustomSlider::_updateLineEdit( int value )
{
  Debug::Throw( "CustomSlider::_updateLineEdit.\n" );
  ostringstream what; 
  what << value;
  line_edit_->setText( what.str().c_str() );
  return;
}
