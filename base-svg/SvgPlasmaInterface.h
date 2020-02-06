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

#include "base_svg_export.h"
#include "File.h"

#include <QBasicTimer>
#include <QFileSystemWatcher>
#include <QObject>
#include <QPalette>
#include <QString>
#include <QTimerEvent>

namespace Svg
{

  //* kde specific interface to plasma to load matching Svg background
  class BASE_SVG_EXPORT SvgPlasmaInterface: public QObject
  {

    Q_OBJECT

    public:

    //* constructor
    explicit SvgPlasmaInterface( QObject* = nullptr );

    //*@name accessors
    //@{

    //* image path
    enum class ImagePath
    {
        DialogBackground,
        WidgetBackground,
        WidgetTranslucentBackground
    };

    //* validity
    bool isValid() const
    { return valid_; }

    //* filename
    QString fileName() const
    { return filename_; }

    //* theme palette
    bool hasThemePalette() const
    { return !themePaletteFileName_.isEmpty() && themePaletteFileName_.exists(); }

    //* theme palette
    QPalette themePalette() const;

    //@}}

    //*@name modifiers
    //@{

    //* image path
    bool setImagePath( ImagePath );

    //* get default theme from configuration file
    /** returns true if changed */
    bool loadTheme();

    //* load svg filename
    /** returns true if changed */
    bool loadFile();

    //@}

    Q_SIGNALS:

    //* emitted when svg file changed
    void themeChanged();

    protected:

    //* timer event
    void timerEvent( QTimerEvent* ) override;

    private:

    //* configuration file modified
    void _configurationFileChanged( const QString& );

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
    ImagePath imagePath_ = ImagePath::WidgetBackground;

    //* path to file name
    /** loaded from plasma configuration */
    File path_;

    //* theme color file name
    File themePaletteFileName_;

    //* filename
    File filename_;

    //* file system watcher
    /** needed to track modifications of kde configuration file */
    QFileSystemWatcher* fileSystemWatcher_ = nullptr;

    //* timer needed to prevent repeated calls to file change
    QBasicTimer timer_;

  };
};

#endif
