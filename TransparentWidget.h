#ifndef _TransparentWidget_h_
#define _TransparentWidget_h_
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
  \file    TransparentWidget.h
  \brief   transparent widget
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include <QWidget>
#include <QMoveEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QShowEvent>

#include "Counter.h"

//! clock widgets
namespace TRANSPARENCY {
  
  //! transparent widget
  class TransparentWidget: public QWidget, public Counter
  {
    
    public:
    
    //! constructor
    TransparentWidget( QWidget *parent = 0, Qt::WindowFlags flags = 0 ):
      QWidget( parent, flags ),
      Counter( "TransparentWidget" ),
      background_changed_( true ),
      highlighted_( false )
    { Debug::Throw( "TransparentWidget::TransparentWidget.\n" ); }
    
    //! tint
    virtual void setTint( const QColor& color = QColor() );
    
    //! highlight
    virtual void setHighlight( const QColor& color = QColor() );
        
    protected:
       
    //! background pixmap
    virtual QPixmap& _backgroundPixmap( void )
    { return background_pixmap_; }
       
    //! background pixmap
    virtual const QPixmap& _backgroundPixmap( void ) const
    { return background_pixmap_; }
    
    //! move
    virtual void moveEvent( QMoveEvent* event )
    { 
      background_changed_ = true;
      update(); 
      return QWidget::moveEvent( event );
    }
    
    //! resize
    virtual void resizeEvent( QResizeEvent* event )
    { 
      background_changed_ = true;
      return QWidget::resizeEvent( event );
    }
    
    //! show
    virtual void showEvent( QShowEvent* event )
    { 
      background_changed_ = true;
      return QWidget::showEvent( event );
    }
    
    //! enter event
    virtual void enterEvent( QEvent *event )
    {
      if( !highlighted_ && highlight_color_.isValid() ) 
      {
        highlighted_ = true;
        update();
      }
    }
    
    //! leave event
    virtual void leaveEvent( QEvent *event )
    {
      if( highlighted_ && highlight_color_.isValid() ) 
      {
        highlighted_ = false;
        update();
      }
    }

    //! paint
    virtual void paintEvent( QPaintEvent* event );
        
    //! update background pixmap
    virtual void _updateBackgroundPixmap( void );
    
    private:

    //! true when background needs to be reloaded
    bool background_changed_;
    
    //! tint color
    QColor tint_color_;
    
    //! true when pointer is in window
    bool highlighted_;
    
    //! tint color
    QColor highlight_color_;
    
    //! store background pixmap
    QPixmap background_pixmap_;
    
  };
  
  
  
};

#endif
