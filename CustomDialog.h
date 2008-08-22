#ifndef CustomDialog_h
#define CustomDialog_h

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
  \file CustomDialog.h
  \brief customized QDialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>
#include <QPushButton>
#include <QBoxLayout>
#include <QLayout>
#include <string>

#include "BaseDialog.h"
#include "Counter.h"  
#include "Debug.h"  
 
/*!
  \class CustomDialog
  \brief customized QDialog
*/
class CustomDialog: public BaseDialog, public Counter
{

  public:
  
  //! mask used to define number of buttons in customized dialogs  
  enum Flag 
  {
    //! no flags
    NONE = 0,
    
    //! dialog has OK button
    OK_BUTTON = 1<<0,
    
    //! dialog has CANCEL button
    CANCEL_BUTTON = 1<<1 
  };
  
  //! constructor
  CustomDialog( 
    QWidget *parent, 
    const unsigned int& flags = OK_BUTTON | CANCEL_BUTTON, 
    Qt::WFlags wflags = 0);  
  
  //! destructor
  virtual ~CustomDialog( void ) 
  {}
    
  //! retrieve main vbox  
  QBoxLayout& mainLayout( void ) const
  {
    assert( main_layout_ );
    return *main_layout_;
  }

  //! retrieve button layout
  QBoxLayout& buttonLayout( void ) const
  {
    assert( button_layout_ );
    return *button_layout_;
  }
 
  //! returns true if OK button is valid
  bool hasOkButton( void )
  { return ok_button_; }
   
  //! retrieve OK button
  QPushButton& okButton( void ) const
  {
    assert( ok_button_ );
    return *ok_button_;
  }
  
  //! returns true if Cancel button is valid
  bool hasCancelButton( void )
  { return cancel_button_; }
  
  //! retrieve CANCEL button
  QPushButton& cancelButton( void ) const
  {
    assert( cancel_button_ );
    return *cancel_button_;
  }
  
    
  private:
  
  //! main layout
  QBoxLayout *main_layout_;
  
  //! button hbox
  QBoxLayout *button_layout_;
  
  //! ok QPushButton, if any
  QPushButton *ok_button_;
  
  //! cancel QPushButton, if any
  QPushButton *cancel_button_;
  
};

#endif 
    
