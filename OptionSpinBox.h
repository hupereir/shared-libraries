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

#ifndef OptionSpinBox_h
#define OptionSpinBox_h

/*!
   \file    OptionSpinBox.h
   \brief   QSpinBox associated to an option for configuration dialogs
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QSpinBox>
#include <cmath>
#include "OptionWidget.h"
#include "Options.h"

//! QSpinBox associated to an option for configuration dialogs
class OptionSpinBox: public QSpinBox, public OptionWidget
{
  
  public:
  
  //! constructor
  OptionSpinBox( QWidget* parent, const QString& option_name ):
      QSpinBox( parent ),
      OptionWidget( option_name ),
      scale_( 1 )
  {}
  
  //! scale (i.e. option = value()/scale)
  void setScale( const double& scale )
  { scale_ = scale; }
  
  //! read value from option
  void read( void )
  { setValue( static_cast<int>(round(scale_*XmlOptions::get().get<double>( optionName() )))); }
  
  //! write value to option
  void write( void ) const
  { XmlOptions::get().set<double>( optionName(), static_cast<double>(value())/scale_ ); }
  
  private:
      
  //! scale factor (default is 1)
  double scale_;
        
};
#endif
