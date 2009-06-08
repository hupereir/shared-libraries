#ifndef _ShadowLabel_h_
#define _ShadowLabel_h_
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
  \file    ShadowLabel.h
  \brief   transparent widget
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include <QColor>
#include <QLabel>
#include <QPaintDevice>

#include "Counter.h"

namespace TRANSPARENCY
{
  class ShadowLabel: public QLabel, public Counter
  {
        
    public:
    
    //! constructor
    ShadowLabel( QWidget* parent ):
      QLabel( parent ),
      Counter( "ShadowLabel" ),
      shadow_offset_( 0 )
    {}
     
    //! constructor
    ShadowLabel( const QString& text, QWidget* parent ):
      QLabel( text, parent ),
      Counter( "ShadowLabel" ),
      shadow_offset_( 0 )
    {}
   
    //! shadow
    void setShadow( const QColor& color, int offset )
    {
      setShadowColor( color );
      setShadowOffset( offset );
    }
    
    //! shadow offset
    void setShadowOffset( int value )
    { shadow_offset_ = value; }
    
    //! shadow color
    void setShadowColor( const QColor& color )
    { shadow_color_ = color; }
         
    protected:
    
    virtual void paintEvent( QPaintEvent* );
    
    private slots:
    
    //! configuration
    void _updateConfiguration( void )
    {}
    
    private:
    
    //! shadow offset
    const int& _shadowOffset( void ) const
    { return shadow_offset_; }
    
    //! shadow color
    const QColor& _shadowColor( void ) const
    { return shadow_color_; }
    
    //! shadow offset
    int shadow_offset_;
    
    //! shadow color
    QColor shadow_color_; 
    
    
  };
  
};

#endif
