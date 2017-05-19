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


#include "WidgetMonitor.h"

#include <QDialog>

//* base dialog
class BaseDialog: public QDialog
{

    Q_OBJECT

    public:

    //* dialog centering enumeration
    enum Centering
    {

        CenterOnPointer,
        CenterOnParent,
        CenterOnDesktop

    };

    //* constructor
    BaseDialog( QWidget* = nullptr, Qt::WindowFlags = 0);

    //* window title
    virtual BaseDialog& setWindowTitle( const QString& );

    //* restore window size
    virtual QSize minimumSizeHint( void ) const;

    //* restore window size
    virtual QSize sizeHint( void ) const;

    //* center widget on pointer
    BaseDialog& centerOnPointer( void );

    //* center widget on argument widget
    BaseDialog& centerOnDesktop( void );

    //* center on parent widget
    BaseDialog& centerOnParent( void )
    { return centerOnWidget( parentWidget() ); }

    //* center widget on argument widget
    BaseDialog& centerOnWidget( QWidget* );

    //* size option name
    virtual void setOptionName( const QString& name )
    { monitor_.setOptionName( name ); }

    //* set icon size
    virtual void setIconSize( const QSize& size )
    { iconSize_ = size; }

    //* iconsize
    virtual QSize iconSize( void ) const
    { return iconSize_; }

    public Q_SLOTS:

    //* uniconify
    BaseDialog& uniconify( void );

    //* stickyness
    void toggleSticky( bool );

    protected:

    //* generic event
    virtual bool event( QEvent* );

//     //* maximize state prior to minimization
//     bool _wasMaximized( void ) const
//     { return wasMaximized_; }

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
