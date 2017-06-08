#ifndef DockWidget_h
#define DockWidget_h

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

#include "Counter.h"
#include "Debug.h"

#include <QDockWidget>

class DockWidget: public QDockWidget
{

    Q_OBJECT

    public:

    //* constructor
    explicit DockWidget( const QString&, QWidget* = nullptr, const QString& = QString() );

    //*@name accessors
    //@{

    //* lock
    virtual bool isLocked( void ) const
    { return locked_; }

    //* visibility action
    QAction& visibilityAction( void ) const
    { return *visibilityAction_; }

    //@}

    //*@name modifiers
    //@{

    //* use scroll area
    void setUseScrollArea( bool );

    //* lock
    virtual void setLocked( bool );

    //* set main widget
    void setMainWidget( QWidget* );

    //* title bar
    void setAutoHideTitleBar( bool );

    //@}

    protected Q_SLOTS:

    //* toggle visibility
    virtual void _toggleVisibility( bool );

    //* update title bar
    virtual void _updateTitleBarWidget( void );

    private Q_SLOTS:

    //* update configuration
    void _updateConfiguration( void );

    private:

    //* install actions
    void _installActions( void );

    //* assocated option name
    QString optionName_;

    //* visibility action
    QAction* visibilityAction_ = nullptr;

    //* use scroll area
    bool useScrollArea_ = false;

    //* lock
    bool locked_ = false;

    //* auto hide title bar
    bool autoHideTitleBar_ = false;

    //* container
    QWidget* container_ = nullptr;

    //* main widget
    QWidget* mainWidget_ = nullptr;

    //* title bar widget
    QWidget* titleBarWidget_ = nullptr;

};

#endif
