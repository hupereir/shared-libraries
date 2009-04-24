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
  \file SvgPlasmaInterface.cpp
  \brief kde specific interface to plasma to load matching Svg background
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>
#include <QFile>
#include <QTextStream>

#include "Debug.h"
#include "SvgPlasmaInterface.h"
#include "Util.h"

namespace SVG
{
  
  //_________________________________________________
  SvgPlasmaInterface::SvgPlasmaInterface( QObject* parent ):
    QObject( parent ),
    Counter( "SvgPlasmaInterface" ),
    valid_( false ),
    transparent_( false ),
    file_system_watcher_(0)
  { Debug::Throw( "SvgPlasmaInterface::SvgPlasmaInterface.\n" ); }

  //_________________________________________________
  SvgPlasmaInterface::~SvgPlasmaInterface( void )
  { Debug::Throw( "SvgPlasmaInterface::~SvgPlasmaInterface.\n" ); }
  
  //_________________________________________________
  bool SvgPlasmaInterface::loadTheme( void )
  {
    
    Debug::Throw( "SvgPlasmaInterface::_loadTheme.\n" );
        
    // open file
    configuration_file_ = File( ".kde4/share/config/plasmarc" ).addPath( Util::home() );
    QFile in( configuration_file_ );
    if ( !in.open( QIODevice::ReadOnly ) )
    {
      Debug::Throw() << "SvgPlasmaInterface::_loadTheme - unable to load plasma configuration file " << configuration_file_ << endl;
      bool changed( false );
      changed |= _setValid( false );
      changed |= _setPath( File() );
      return changed;
    }
    
    // add file to file system watcher
    if( !_hasFileSystemWatcher() ) 
    {
      _initializeFileSystemWatcher();
      _fileSystemWatcher().addPath( configuration_file_ );
    }
    
    // get all lines inside QStringList
    QStringList lines = QString( in.readAll() ).split( '\n' );
    
    bool found_theme( false );
    QRegExp theme_tag( "^\\s*\\[Theme\\]\\s*$" );
    QRegExp section_tag( "^\\[\\S*\\]\\s*$" );
    QRegExp empty_line( "^\\s*$" );
    QRegExp name_tag( "^\\s*name\\s*=\\s*(\\S*)\\s*$" );
    
    QString theme;
    for( QStringList::const_iterator iter = lines.begin(); iter != lines.end(); iter++ )
    {
      if( !found_theme )
      {
       
        if( iter->indexOf( theme_tag ) >= 0 )
        {
          Debug::Throw( "SvgPlasmaInterface::_loadTheme - found theme tag.\n" );
          found_theme = true;
        }
        
        continue;
      
      } else if( iter->indexOf( section_tag ) >= 0 ) {
       
        // check for next tag
        Debug::Throw( "SvgPlasmaInterface::_loadTheme - found next tag.\n" );
        break;
        
      } else if( iter->indexOf( empty_line ) >= 0 ) {
       
        // check for empty lines
        continue;
        
      } else if( iter->indexOf( name_tag ) >= 0 ) {
        
        theme = name_tag.cap(1);
        Debug::Throw() << "SvgPlasmaInterface::_loadTheme - found theme: " << theme << endl;
        break;
        
      } else continue;
      
    }
    
    return _setTheme( (theme.isNull() || theme.isEmpty() ) ? "default":theme );
    
  }

  //_________________________________________________
  bool SvgPlasmaInterface::loadFile( void )
  { 
    
    Debug::Throw( "SvgPlasmaInterface::loadFile.\n" );
    const File background_transparent( "translucentbackground.svgz" );
    const File background_standard( "background.svgz" );

    // check path
    if( !_path().exists() ) return _setValid( false );
    
    // construct full filename base on theme and transparency setting
    File filename = (_transparent() ? background_transparent:background_standard ).addPath( _path() );    
    Debug::Throw() << "SvgPlasmaInterface::loadFile - filename: " << filename << " exists: " << filename.exists() << endl;

    bool changed( false );
    changed |= _setValid( filename.exists() );
    changed |= _setFileName( filename );
    return changed;
    
  }
  
  //_________________________________________________
  void SvgPlasmaInterface::_configurationFileChanged( const QString& file )
  {
    Debug::Throw(0) << "SvgPlasmaInterface::_configurationFileChanged - " << file << endl;
    assert( file == configuration_file_ );
    if( loadTheme() ) {
      loadFile();
      emit themeChanged();
    }
    
  }
    
  //_____________________________________________________________
  void SvgPlasmaInterface::_initializeFileSystemWatcher( void )
  {
    Debug::Throw( "SvgPlasmaInterface::_initializeFileSystemWatcher.\n" );
    assert( !_hasFileSystemWatcher() );
    file_system_watcher_ = new QFileSystemWatcher();
    connect( &_fileSystemWatcher(), SIGNAL( fileChanged( const QString& ) ), SLOT( _configurationFileChanged( const QString& ) ) );
  }

  //_________________________________________________
  bool SvgPlasmaInterface::_setTheme( const QString& theme )
  {
    Debug::Throw() << "SvgPlasmaInterface::_setTheme - theme: " << theme << endl;
    File path;
    QTextStream( &path ) << "/usr/share/apps/desktoptheme/" << theme << "/widgets";
    if( !path.exists() )
    {
      if( theme == "default" ) 
      {
        bool changed( false );
        changed |= _setPath( File() );
        changed |= _setValid( false );
        return false;
      } else return _setTheme( "default" );
    
    } else return _setPath( path );
    
  }
  
};
