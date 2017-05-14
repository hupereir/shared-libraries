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
#include <QPalette>
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

    //*@name accessors
    //@{

    //* image path
    enum ImagePath
    {
        DialogBackground,
        WidgetBackground,
        WidgetTranslucentBackground
    };

    //* validity
    bool isValid( void ) const
    { return valid_; }

    //* filename
    QString fileName( void ) const
    { return filename_; }

    //* theme palette
    bool hasThemePalette( void ) const
    { return !themePaletteFilename_.isEmpty() && themePaletteFilename_.exists(); }

    //* theme palette
    QPalette themePalette( void ) const;

    //@}}

    //*@name modifiers
    //@{

    //* image path
    bool setImagePath( ImagePath );

    //* get default theme from configuration file
    /** returns true if changed */
    bool loadTheme( void );

    //* load svg filename
    /** returns true if changed */
    bool loadFile( void );

    //@}

    Q_SIGNALS:

    //* emitted when svg file changed
    void themeChanged();

    protected Q_SLOTS:

    //* configuration file modified
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
    bool _setPath( const File& );

    //* set filename
    /** returns true when changed */
    bool _setFileName( const File& );

    //* return image path matching id
    File _imagePath( ImagePath ) const;

    //* return image path matching id
    File _findImage( const File&, ImagePath ) const;

    //* validity
    bool valid_ = false;

    //* type of svg to be loaded
    ImagePath imagePath_ = WidgetBackground;

    //* path to file name
    /** loaded from plasma configuration */
    File path_;

    //* theme color file name
    File themePaletteFilename_;

    //* filename
    File filename_;

    //* file system watcher
    /** needed to track modifications of kde configuration file */
    QFileSystemWatcher* fileSystemWatcher_ = nullptr;

  };
};

#endif
