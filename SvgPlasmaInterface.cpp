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

#include <cassert>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QTextStream>

#include "SvgPlasmaInterface.h"

namespace SVG
{
  
  //_________________________________________________
  SvgPlasmaInterface::SvgPlasmaInterface( QObject* parent ):
    QObject( parent ),
    valid_( false ),
    image_path_( "widgets/background" ),
    file_system_watcher_(0)
  {}
  
  //_________________________________________________
  SvgPlasmaInterface::~SvgPlasmaInterface( void )
  {}
  
  //_________________________________________________
  bool SvgPlasmaInterface::loadTheme( void )
  {
    
    QStringList configuration_files;
    configuration_files 
      << ".kde/share/config/plasmarc"
      << ".kde4/share/config/plasmarc";
    
    // open file
    for( QStringList::const_iterator iter = configuration_files.begin(); iter != configuration_files.end(); iter++ )
    {
      QFileInfo info( QDir::home(), *iter );
      
      // QTextStream(stdout) << "SvgPlasmaInterface::loadTheme - trying " << info.absoluteFilePath() << endl;

      if( !info.exists() ) continue;
      
      configuration_file_ = info.absoluteFilePath();
      QFile in( configuration_file_ );
      if ( !in.open( QIODevice::ReadOnly ) ) continue;

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
          
          if( iter->indexOf( theme_tag ) >= 0 ) found_theme = true;
          continue;
          
        } else if( iter->indexOf( section_tag ) >= 0 ) break;
        else if( iter->indexOf( empty_line ) >= 0 ) continue;
        else if( iter->indexOf( name_tag ) >= 0 ) {
          
          theme = name_tag.cap(1);
          break;
          
        } else continue;
        
      }
    
      return _setTheme( (theme.isNull() || theme.isEmpty() ) ? "default":theme );
 
    }

    // configuration file not found.
    bool changed( false );
    changed |= _setValid( false );
    changed |= _setPath( QString() );
    return changed;
   
  }

  //_________________________________________________
  bool SvgPlasmaInterface::loadFile( void )
  { 
    
    // check path
    if( !QFileInfo( _path() ).exists() ) return _setValid( false );

    // generate possible file names
    QStringList files;
    files 
      << _imagePath()
      << _imagePath() + ".svg"
      << _imagePath() + ".svgz";
    
    bool found( false );
    QString filename;    
    for( QStringList::const_iterator iter = files.begin(); iter != files.end(); iter++ )
    { 
    
      QFileInfo info( QDir(_path()), *iter );
      if( info.exists() )
      {
        
        filename = info.absoluteFilePath();
        found = true;
        break;
      }
      
    }
    
    // construct full filename base on theme and transparency setting
    bool changed( false );
    changed |= _setValid( found );
    changed |= _setFileName( found ? filename:QString() );
    return changed;
    
  }
  
  //_________________________________________________
  void SvgPlasmaInterface::_configurationFileChanged( const QString& file )
  {
    assert( file == configuration_file_ );
    if( loadTheme() ) {
      loadFile();
      emit themeChanged();
    }
    
  }
    
  //_____________________________________________________________
  void SvgPlasmaInterface::_initializeFileSystemWatcher( void )
  {
    assert( !_hasFileSystemWatcher() );
    file_system_watcher_ = new QFileSystemWatcher();
    connect( &_fileSystemWatcher(), SIGNAL( fileChanged( const QString& ) ), SLOT( _configurationFileChanged( const QString& ) ) );
  }

  //_________________________________________________
  bool SvgPlasmaInterface::_setTheme( const QString& theme )
  {
    
    // try use user-scoped path
    QStringList local_path;
    local_path 
      << ".kde/share/apps/desktoptheme/"
      << ".kde4/share/apps/desktoptheme/";
    
    for( QStringList::const_iterator iter = local_path.begin(); iter != local_path.end(); iter++ )
    {
      QFileInfo info( QDir::home(), *iter + theme );
      // QTextStream(stdout) << "SvgPlasmaInterface::_setTheme - trying " << info.absoluteFilePath() << endl;
      if( info.exists() ) return _setPath( info.absoluteFilePath() );
      
    }

    // try use global path
    QFileInfo info( QDir("/usr/share/apps/desktoptheme/"), theme );
    // QTextStream(stdout) << "SvgPlasmaInterface::_setTheme - trying " << info.absoluteFilePath() << endl;
    if( info.exists() ) return _setPath( info.absoluteFilePath() );
      
    // try use default theme
    if( theme == "default" ) 
    {
      bool changed( false );
      changed |= _setPath( QString() );
      changed |= _setValid( false );
      return changed;
    } else return _setTheme( "default" );
    
  }
  
};
