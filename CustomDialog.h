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

#include <QDialog>
#include <QPushButton>
#include <QBoxLayout>
#include <QLayout>
#include <string>

#include "Counter.h"  
#include "Debug.h"  
#include "Exception.h"  
 
/*!
  \class CustomDialog
  \brief customized QDialog
*/
class CustomDialog: public QDialog, public Counter
{

  public:
  
  //! mask used to define number of buttons in customized dialogs  
  enum Flag 
  {
    //! no flags
    NONE = 0,
    
    //! dialog is exclusive
    EXCLUSIVE = 1,
      
    //! dialog has OK button
    OK_BUTTON = 2,
    
    //! dialog has CANCEL button
    CANCEL_BUTTON = 4 
  };
  
  //! constructor
  CustomDialog( 
    QWidget *parent, 
    const unsigned int& flags = EXCLUSIVE | OK_BUTTON | CANCEL_BUTTON, 
    Qt::WFlags wflags = 0);  
  
  //! destructor
  virtual ~CustomDialog( void ) 
  {}
    
  //! retrieve main vbox  
  QWidget& mainWidget( void ) const
  {
    Exception::checkPointer( main_widget_, DESCRIPTION( "box_ not initialized" ) );
    return *main_widget_;
  }

  //! retrieve button layout
  QBoxLayout& buttonLayout( void ) const
  {
    Exception::checkPointer( button_layout_, DESCRIPTION( "button_layout_ not initialized" ) );
    return *button_layout_;
  }
 
  //! returns true if OK button is valid
  bool hasOkButton( void )
  { return ok_button_; }
   
  //! retrieve OK button
  QPushButton& okButton( void ) const
  {
    Exception::checkPointer( ok_button_, DESCRIPTION( "ok_button_ not initialized" ) );
    return *ok_button_;
  }
  
  //! returns true if Cancel button is valid
  bool hasCancelButton( void )
  { return cancel_button_; }
  
  //! retrieve CANCEL button
  QPushButton& cancelButton( void ) const
  {
    Exception::checkPointer( cancel_button_, DESCRIPTION( "cancel_button_ not initialized" ) );
    return *cancel_button_;
  }
  
  //! set main vbox style as raised panel
  void raiseMainWidget( int spacing = 2, int margin = 2 ) 
  {
    mainWidget().layout()->setMargin( margin );
    mainWidget().layout()->setSpacing( spacing );
  }
    
  private:
  
  //! main QVBox for further display
  QWidget *main_widget_;
  
  //! button hbox
  QBoxLayout *button_layout_;
  
  //! ok QPushButton, if any
  QPushButton *ok_button_;
  
  //! cancel QPushButton, if any
  QPushButton *cancel_button_;
};

#endif 
    
