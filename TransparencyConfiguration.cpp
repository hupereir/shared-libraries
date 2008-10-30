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
   \file    TransparencyConfiguration.cpp
   \brief   transparency common configuration
   \version $Revision$
   \date    $Date$
*/

#include <QGroupBox>
#include <QLabel>
#include <QLayout>

#include "GridLayout.h"
#include "Debug.h"
#include "OptionCheckBox.h"
#include "OptionColorDisplay.h"
#include "OptionSlider.h"
#include "OptionSpinBox.h"
#include "TransparencyConfiguration.h"

#include "CompositeEngine.h"

using namespace std;
using namespace TRANSPARENCY;

//________________________________________________________________________
TransparencyConfiguration::TransparencyConfiguration( QWidget* parent ):
  QWidget( parent ),
  Counter( "TransparencyConfiguration" )
{
  Debug::Throw( "TransparencyConfiguration::TransparencyConfiguration.\n" );
  
  setLayout( new QVBoxLayout() );
  layout()->setSpacing(5);
  layout()->setMargin(0);
   
  // generic objects
  QGroupBox *box;
  OptionColorDisplay* color_display;
  OptionSlider* slider;
  OptionSpinBox* spinbox;
  OptionCheckBox* checkbox;
  
  // general
  layout()->addWidget( box = new QGroupBox( "General", this ) );
  box->setLayout( new QVBoxLayout() );
  box->layout()->setSpacing(5);
  box->layout()->setMargin(5);
  
  // enable/disable transparency
  box->layout()->addWidget( checkbox = new OptionCheckBox( "use transparency", box, "TRANSPARENT" ) );
  checkbox->setToolTip( "enable/disable transparent background" );
  addOptionWidget( checkbox );

  // enable/disable compositing
  if( CompositeEngine::get().isAvailable() )
  {
    
    box->layout()->addWidget( checkbox = new OptionCheckBox( "use compositing", box, "TRANSPARENCY_USE_COMPOSITE" ) );
    checkbox->setToolTip( "enable/disable compositing" );
    addOptionWidget( checkbox );
    
  }
    
  QHBoxLayout* h_layout = new QHBoxLayout();
  h_layout->setSpacing(5);
  h_layout->setMargin(0);
  box->layout()->addItem( h_layout );
  
  // opacity
  h_layout->addWidget( new QLabel( "opacity (composite):", box ) );
  h_layout->addWidget( slider = new OptionSlider( this, "TRANSPARENCY_OPACITY" ) );
  slider->setScale( 1.0/2.55 );
  slider->slider().setMinimum( 0 );
  slider->slider().setMaximum( 100 );
  slider->setToolTip( "widget opacity" );
  addOptionWidget( slider );
  
  // colors
  layout()->addWidget( box = new QGroupBox( "Colors", this ) );
  
  GridLayout* grid_layout = new GridLayout();
  grid_layout->setSpacing(5);
  grid_layout->setMargin(5);
  grid_layout->setMaxCount( 2 );
  box->setLayout( grid_layout );

  // foreground color
  grid_layout->addWidget( new QLabel( "foreground color:", box ) );
  grid_layout->addWidget( color_display = new OptionColorDisplay( box, "TRANSPARENCY_FOREGROUND_COLOR" ) );
  color_display->setToolTip( "text/display foreground color" );
  addOptionWidget( color_display );
 
  // shadow color
  grid_layout->addWidget( new QLabel( "shadow color:", box ) );
  grid_layout->addWidget( color_display = new OptionColorDisplay( box, "TRANSPARENCY_SHADOW_COLOR" ) );
  color_display->setToolTip( "text/display shadow color" );
  addOptionWidget( color_display );
 
  // tint color
  grid_layout->addWidget( new QLabel( "tint color:", box ) );
  grid_layout->addWidget( color_display = new OptionColorDisplay( box, "TRANSPARENCY_TINT_COLOR" ) );
  color_display->setToolTip( "transparent background tint color" );
  addOptionWidget( color_display );

  // highlight color
  grid_layout->addWidget( new QLabel( "highlight color:", box ) );
  grid_layout->addWidget( color_display = new OptionColorDisplay( box, "TRANSPARENCY_HIGHLIGHT_COLOR" ) );
  color_display->setToolTip( "highlight color when entering widget" );
  addOptionWidget( color_display );

  // shadow offset
  grid_layout->addWidget( new QLabel( "shadow offset: ", box ) );
  grid_layout->addWidget( spinbox = new OptionSpinBox( box, "TRANSPARENCY_SHADOW_OFFSET" ) );
  spinbox->setMinimum( 0 );
  spinbox->setMaximum( 10 );
  spinbox->setToolTip( 
    "offset between text shadow and text.\n"
    "0 means no shadow." );
  addOptionWidget( spinbox );
  
  // foreground intensity
  grid_layout->addWidget( new QLabel( "foreground intensity:", box ) );
  grid_layout->addWidget( slider = new OptionSlider( box, "TRANSPARENCY_FOREGROUND_INTENSITY" ) );
  slider->setScale( 1.0/2.55 );
  slider->slider().setMinimum( 0 );
  slider->slider().setMaximum( 100 );
  slider->setToolTip( "foreground color intensity" );
  addOptionWidget( slider );
  
  // tint intensity
  grid_layout->addWidget( new QLabel( "tint intensity:", box ) );
  grid_layout->addWidget( slider = new OptionSlider( box, "TRANSPARENCY_TINT_INTENSITY" ) );
  slider->setScale( 1.0/2.55 );
  slider->slider().setMinimum( 0 );
  slider->slider().setMaximum( 100 );
  slider->setToolTip( "transparent background tint intensity" );
  addOptionWidget( slider );
  
  // tint intensity
  grid_layout->addWidget( new QLabel( "highlight intensity:", box ) );
  grid_layout->addWidget( slider = new OptionSlider( box, "TRANSPARENCY_HIGHLIGHT_INTENSITY" ) );
  slider->setScale( 1.0/2.55 );
  slider->slider().setMinimum( 0 );
  slider->slider().setMaximum( 100 );
  slider->setToolTip( "transparent background highlight intensity" );
  addOptionWidget( slider );
  
  return;
}
