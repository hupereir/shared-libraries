#ifndef BaseDialog_h
#define BaseDialog_h

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
#include "WidgetMonitor.h"

#include <QDialog>

//* base dialog
class BASE_QT_EXPORT BaseDialog: public QDialog
{

    Q_OBJECT

    public:

    //* constructor
    explicit BaseDialog( QWidget* = nullptr, Qt::WindowFlags = 0);

    //*@name accessors
    //@{

    //* restore window size
    QSize minimumSizeHint() const override;

    //* restore window size
    QSize sizeHint() const override;

    //* iconsize
    QSize iconSize() const
    { return iconSize_; }

    //@}

    //*@name modifiers
    //@{

    //* window title
    void setWindowTitle( const QString& );

    //* center widget on pointer
    BaseDialog& centerOnPointer();

    //* center widget on argument widget
    BaseDialog& centerOnDesktop();

    //* center on parent widget
    BaseDialog& centerOnParent()
    { return centerOnWidget( parentWidget() ); }

    //* center widget on argument widget
    BaseDialog& centerOnWidget( QWidget* );

    //* size option name
    void setOptionName( const QString& name )
    { monitor_.setOptionName( name ); }

    //* set icon size
    void setIconSize( QSize size )
    { iconSize_ = size; }

    //* uniconify
    void uniconify();

    //* stickyness
    void toggleSticky( bool );

    //@}

    protected:

    //* generic event
    bool event( QEvent* ) override;

    //* maximize state prior to minimization
    void _setWasMaximized( bool value )
    { wasMaximized_ = value; }

    private:

    //* size watch
    WidgetMonitor monitor_;

    //* icon size
    QSize iconSize_;

    //* window state prior to minimization
    bool wasMaximized_ = false;

};

#endif
