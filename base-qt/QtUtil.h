#ifndef QtUtil_h
#define QtUtil_h

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

#include "base_qt_export.h"
#include <QAction>
#include <QDialog>
#include <QIcon>
#include <QMenu>
#include <QPalette>
#include <QString>

//* some convernience methods
class BASE_QT_EXPORT QtUtil
{

    public:

    //* desktop geometry
    static QRect desktopGeometry( QWidget* = nullptr );

    //* move menu to position taking proper care of the screen borders
    static void moveWidget( QWidget* widget, QPoint position );

    //* center widget on pointer
    static QPoint centerOnPointer( QSize size );

    //* return topleft point so that size is centered on widget
    static QPoint centerOnWidget( QSize size, QWidget* parent );

    //* return topleft point so that size is centered on pointer
    static QPoint centerOnDesktop( QSize size );

    //* center widget on pointer
    static QWidget* centerOnPointer( QWidget* widget );

    //* center widget on parent top level widget
    static QWidget* centerOnParent( QWidget* widget );

    //* center widget on parent top level widget
    static QWidget* centerOnWidget( QWidget* widget, QWidget* parent );

    //* center widget on desktop widget
    static QWidget* centerOnDesktop( QWidget* widget );

    //* uniconify a top level widget
    static QWidget* uniconify( QWidget* widget );

    //* title font point size
    static int titleFontPointSize( const QFont& );

    //* get 'title' font matching argument
    static QFont titleFont( QFont );

    //* light text color
    static QColor lightTextColor( const QPalette& palette,
        QPalette::ColorRole role )
    { return lightTextColor( palette.color( role ) ); }

    //* light text color
    static QColor lightTextColor( const QPalette& palette,
        QPalette::ColorGroup group = QPalette::Normal,
        QPalette::ColorRole role = QPalette::WindowText )
    { return lightTextColor( palette.color( group, role ) ); }

    //* light text color
    static QColor lightTextColor( const QColor& );

    //* menu section
    static QAction* addMenuSection( QMenu* menu, const QString& text )
    { return addMenuSection( menu, QIcon(), text ); }

    //* menu section
    static QAction* addMenuSection( QMenu*, const QIcon&, const QString& );

};

#endif
