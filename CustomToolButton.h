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
  \file CustomToolButton.h
  \brief customized tool button to display tooltip in a dedicated label
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/
#ifndef CustomToolButton_h
#define CustomToolButton_h

#include <QIconSet>
#include <QLabel>
#include <QToolButton>

#include <string>

#include "Counter.h"  
#include "Debug.h"  

/*!
  \class CustomToolButton
  \brief customized tool button to display tooltip in a dedicated label
*/
class CustomToolButton: public QToolButton, public Counter
{  
  public:
  
  //! default creator
  CustomToolButton( QWidget* parent ):
    QToolButton( parent ),
    Counter( "CustomToolButton" ),
    tooltip_label_(0)
  {}
   
  //! creator with icon filename, tooltip and destination label
  CustomToolButton( 
    QWidget* parent,   
    const QPixmap& pixmap,
    const std::string& tooltip = "",
    QLabel* label = 0 );
   
  //! destructor
  virtual ~CustomToolButton( void ) 
  {}
  
  //! set destination label
  void SetToolTipLabel( QLabel* tooltip_label )
  { tooltip_label_ = tooltip_label; }
  
  protected:
      
  //! overloaded enter event handler
  void enterEvent( QEvent *event )
  {
    if( tooltip_label_ && toolTip().size() ) 
    { tooltip_label_->setText( toolTip() ); }
    QToolButton::enterEvent( event );
  }
  
  //! overloaded leave event handler
  void leaveEvent( QEvent *event )
  {
    if( tooltip_label_ && toolTip().size() ) 
    { tooltip_label_->clear(); }
    QToolButton::leaveEvent( event );
  }
  
  private:
  
  //! destination label for tooltip
  QLabel* tooltip_label_;
  
};

#endif
