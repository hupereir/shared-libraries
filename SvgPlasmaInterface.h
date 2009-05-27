#ifndef SvgPlasmaInterface_h
#define SvgPlasmaInterface_h

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
  \file SvgPlasmaInterface.h
  \brief kde specific interface to plasma to load matching Svg background
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <cassert>
#include <QObject>
#include <QString>
#include <QFileSystemWatcher>

#include "Counter.h"
#include "File.h"

//! svg namespace
namespace SVG
{

  //! kde specific interface to plasma to load matching Svg background
  class SvgPlasmaInterface: public QObject, public Counter
  {
    
    Q_OBJECT
    
    public:
    
    //! constructor
    SvgPlasmaInterface( QObject* parent = 0 );
    
    //! destructor
    virtual ~SvgPlasmaInterface( void );
    
    //! validity
    const bool& isValid( void ) const
    { return valid_; }
    
    //! transparent background 
    /*! returns true when changed */
    bool setTransparent( const bool& value )
    { 
      if( transparent_ == value ) return false;
      transparent_ = value; 
      return true; 
    }
    
    //! filename
    const File& fileName( void ) const
    { return filename_; }
    
    //! get default theme from configuration file
    /*! returns true if changed */
    bool loadTheme( void );

    //! load svg filename
    /*! returns true if changed */
    bool loadFile( void );
    
    signals:
    
    //! emmited when svg file changed
    void themeChanged();
    
    private slots:
    
    //! connected to kde configuration file changed
    void _configurationFileChanged( const QString& );
    
    private:
    
    //! validity
    bool _setValid( const bool& value )
    { 
      if( valid_ == value ) return false;
      valid_ = value;
      return true;
    }
    
    //! transparent background
    const bool& _transparent( void ) const
    { return transparent_; }

    //! set theme
    bool _setTheme( const QString& );
    
    //! set path matching theme
    bool _setPath( const File& path )
    { 
      if( path_ == path ) return false;
      path_ = path;
      return true;
    }
    
    //! get path matching theme
    const File& _path( void ) const
    { return path_; }
   
    //! set filename
    /*! returns true when changed */
    bool _setFileName( const File& file )
    {
      if( filename_ == file ) return false;
      filename_ = file;
      return false;
    }
      
    //! file system watcher
    bool _hasFileSystemWatcher( void ) const
    { return file_system_watcher_; }
    
    //! initialize file system watcher
    void _initializeFileSystemWatcher( void );
    
    //! file system watcher
    QFileSystemWatcher& _fileSystemWatcher( void )
    { 
      assert( file_system_watcher_ );
      return *file_system_watcher_; 
    }
    
    //! validity
    bool valid_;
    
    //! transparency
    bool transparent_;
    
    //! configuration file
    File configuration_file_;

    //! path to file name 
    /*! loaded from plasma configuration */
    File path_;
    
    //! filename
    File filename_;

    //! file system watcher
    /*! needed to track modifications of kde configuration file */
    QFileSystemWatcher* file_system_watcher_;

  };
};

#endif
