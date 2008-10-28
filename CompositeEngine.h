#ifndef CompositeEngine_h
#define CompositeEngine_h
 
// $Id$

/***************************************************************************
 *   Copyright (C) 2006 by Helio Chissini de Castro   *
 *   helio@mandriva.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*!
  \file    CompositeEngine.h
  \brief   initialize compositing, if available
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include <Qt>

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif

namespace TRANSPARENCY
{
  
  //! initialize compositing, if available
  class CompositeEngine
  {
    public:
      
    //! singleton
    static CompositeEngine& get( void );
    
    //! display
    Display* display( void ) const
    { return display_; }
    
    //! availability
    bool isAvailable( void ) const
    { return available_; }
    
    //! enability
    bool isEnabled( void ) const
    { return available_ && enabled_; }
    
    //! enability
    bool setEnabled( const bool& value ) 
    { 
      if( enabled_ == value ) return false;
      enabled_ = value;
      return true;
    }
    
    //! initialize
    void initialize( void );
    
    //! visual
    Qt::HANDLE visual( void ) const
    { return Qt::HANDLE( visual_ ); }
    
    //! colormar
    Qt::HANDLE colormap( void ) const
    { return Qt::HANDLE( colormap_ ); }
    
    private:
    
    //! constructor
    CompositeEngine( void );
        
    //! returns true if composition is enabled
    bool _compositingEnabled( void ) const;
    
    //! returns true if composition is enabled
    bool _compositingEnabled( Display* ) const;

    //! singleton
    static CompositeEngine singleton_;
    
    //! validity
    bool available_;
    
    //! enability
    bool enabled_;
    
    //! initialized
    bool initialized_;
    
    //! display
    Display* display_;
    
    //! visual
    Visual* visual_;
    
    //! colormap
    Colormap colormap_;
    
  };
  
};

#endif
