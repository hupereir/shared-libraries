#ifndef SvgPlasmaInterface_h
#define SvgPlasmaInterface_h

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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "File.h"

#include <QObject>
#include <QString>
#include <QFileSystemWatcher>

namespace Svg
{

  //* kde specific interface to plasma to load matching Svg background
  class SvgPlasmaInterface: public QObject
  {

    Q_OBJECT

    public:

    //* constructor
    SvgPlasmaInterface( QObject* = nullptr );

    //* image path
    enum ImagePath
    {
        DialogBackground,
        WidgetBackground,
        WidgetTranslucentBackground
    };

    //* image path
    bool setImagePath( ImagePath );

    //* validity
    bool isValid( void ) const
    { return valid_; }

    //* filename
    const QString& fileName( void ) const
    { return filename_; }

    //* get default theme from configuration file
    /** returns true if changed */
    bool loadTheme( void );

    //* load svg filename
    /** returns true if changed */
    bool loadFile( void );

    Q_SIGNALS:

    //* emmited when svg file changed
    void themeChanged();

    private Q_SLOTS:

    //* connected to kde configuration file changed
    void _configurationFileChanged( const QString& );

    private:

    //* validity
    bool _setValid( bool value )
    {
      if( valid_ == value ) return false;
      valid_ = value;
      return true;
    }

    //* set theme
    bool _setTheme( const QString& );

    //* set path matching theme
    bool _setPath( const File& path )
    {
      if( path_ == path ) return false;
      path_ = path;
      return true;
    }

    //* set filename
    /** returns true when changed */
    bool _setFileName( const File& file )
    {
      if( filename_ == file ) return false;
      filename_ = file;
      return true;
    }

    //* return image path matching id
    File _imagePath( ImagePath ) const;

    //* return image path matching id
    File _findImage( const File&, ImagePath ) const;

    //* validity
    bool valid_ = false;

    //* path to file name
    /** loaded from plasma configuration */
    File path_;

    //* path to image (relative to path_)
    ImagePath imagePath_;

    //* filename
    File filename_;

    //* file system watcher
    /** needed to track modifications of kde configuration file */
    QFileSystemWatcher* fileSystemWatcher_ = nullptr;

  };
};

#endif
