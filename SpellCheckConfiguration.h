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

#ifndef _SpellCheckConfiguration_h_
#define _SpellCheckConfiguration_h_

/*!
   \file    SpellCheckConfiguration.h
   \brief   Speller common configuration
   \version $Revision$
   \date    $Date$
*/

#include <QGroupBox>

#include "Counter.h"
#include "OptionWidget.h"

class OptionBrowsedLineEdit;
class OptionComboBox;

//! Spell common configuration
class SpellCheckConfiguration: public QGroupBox, public OptionWidget, public Counter
{
  public:
      
  //! constructor
  SpellCheckConfiguration( QWidget* parent );
  
  //! set widget value from option value
  virtual void read( void );
  
  //! set option value from widget value
  virtual void write( void ) const;
  
  private:
  
  //! aspell browsed line editor
  OptionBrowsedLineEdit* aspell_;
  
  //! dictionary combo box
  OptionComboBox* dictionary_;
  
  //! filter combo box
  OptionComboBox* filter_;
  
};

#endif
