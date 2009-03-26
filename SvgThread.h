#ifndef SvgThread_h
#define SvgThread_h

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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License        
* for more details.                     
*                          
* You should have received a copy of the GNU General Public License along with 
* software; if not, write to the Free Software Foundation, Inc., 59 Temple     
* Place, Suite 330, Boston, MA 02111-1307 USA                           
*                         
*                         
*******************************************************************************/

/*!
  \file SvgThread.h
  \brief check validity of a set of files
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QEvent>
#include <QSvgRenderer>
#include <QThread>
#include <vector>

#include "Counter.h"
#include "Debug.h" 
#include "SvgImage.h"

//! used to post a new grid when ready
namespace SVG
{
  
  class SvgEvent: public QEvent, public Counter
  {
    
    public:
    
    //! constructor
    SvgEvent( const SvgImage::Cache& cache ):
      QEvent( eventType() ),
      Counter( "SvgEvent" ),
      cache_( cache )
    {}
    
    //! destructor
    ~SvgEvent( void )
    {}
    
    //! static event type
    static QEvent::Type eventType( void );
    
    //! records
    const SvgImage::Cache& cache() const
    { return cache_; }
        
    private:
    
    //! Svg cache
    SvgImage::Cache cache_;
        
  };
  
  //! independent thread used to automatically save file
  class SvgThread: public QThread, public Counter
  {
    
    public:
    
    //! constructor
    SvgThread( QObject* reciever ):
      Counter( "SvgThread" ),
      reciever_( reciever ),
      svg_offset_( 0 )
    {}
    
    //! svg file
    void setSvgFile( const QString& file )
    { svg_.load( file ); }
    
    //! offset
    void setSvgOffset( const double& value )
    { svg_offset_ = value; }
    
    //! set file
    void setSizes( const std::vector<QSize>& sizes )
    { sizes_ = sizes; }
        
    //! Check files validity. Post a SvgEvent when finished
    void run( void );
    
    private:

    //! reciever object for posted events
    QObject* reciever_;
    
    //! svg renderer
    QSvgRenderer svg_;
    
    //! svg offset
    double svg_offset_;
    
    //! requested sizes
    std::vector<QSize> sizes_;
    
  };
  
}

#endif
