#ifndef SvgEngine_h
#define SvgEngine_h

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
  \file SvgEngine.h
  \brief svg basic engine
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QSize>
#include <QObject>

#include <cassert>
#include <vector>

#include "Counter.h"
#include "SvgRenderer.h"
#include "SvgThread.h"

//! svg namespace
namespace SVG
{
  
  class SvgPlasmaInterface;
  
  //! customized Icon factory to provide better looking disabled icons
  class SvgEngine: public QObject, public Counter
  {
    
    Q_OBJECT;
    
    public:
    
    //! retrieve singleton
    static SvgEngine& get( void );
    
    //! create pixmap
    /*! the size is stored into a cache to avoid duplicate pixmaps manipulations */
    static const QPixmap& get( SvgId id )
    { return get()._get( id ); }
    
    //! destructor
    virtual ~SvgEngine( void );
    
    //! is valid
    bool isValid( void ) const
    { return svg_.isValid(); }
    
    //! preload sizes
    /*! uses a separate thread, in order not to slow down application */
    void preload( const SvgId::List& );

    signals:
    
    //! emmitted when svg files are changed
    void changed( void );
    
    public slots: 
    
    //! reload all icons set in cache from new path list
    /*! return true if changed */
    bool reload( const bool& forced = false );
    
    protected:
      
    //! custom event, used for preloading
    void customEvent( QEvent* );
    
    private:
    
    //! load svg
    bool _loadSvg( const bool& forced );
      
    //!@name non static methods are hidden
    //@{
    
    //! constructor
    SvgEngine( void );
    
    //! create icon
    /*! the file is stored into a cache to avoid all pixmaps manipulations */
    const QPixmap& _get( const SvgId&, bool from_cache = true );
    
    //@}
    
    //! initialise plasma interface
    void _initializePlasmaInterface( void );
        
    //! plasma interface
    bool _hasPlasmaInterface( void ) const
    { return plasma_interface_; }
    
    //! plasma interface
    SvgPlasmaInterface& _plasmaInterface( void ) const
    { return *plasma_interface_; }
    
    //! plasma interface
    SvgPlasmaInterface *plasma_interface_;
    
    //! svg file
    QString svg_file_;
    
    //! svg renderer
    SvgRenderer svg_;
    
    //! svg offest
    int svg_offset_;
       
    //! map size and pixmap
    PixmapCache cache_;
  
    //! thread preload sizes
    SvgThread thread_;
    
  };
};

#endif
