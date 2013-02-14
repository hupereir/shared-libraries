#ifndef DockWidget_h
#define DockWidget_h

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

#include "Counter.h"
#include "Debug.h"

#include <QDockWidget>

class DockWidget: public QDockWidget
{

    Q_OBJECT

    public:

    //! constructor
    DockWidget( const QString&, QWidget* = 0x0, const QString& = QString() );

    //! destructor
    virtual ~DockWidget( void )
    {}

    //!@name modifiers
    //@{

    //! use scroll area
    void setUseScrollArea( bool );

    //! lock
    virtual void setLocked( bool );

    //! set main widget
    void setMainWidget( QWidget* );

    //@}

    //!@name accessors
    //@{

    //! lock
    virtual bool isLocked( void ) const
    { return locked_; }

    //! visibility action
    QAction& visibilityAction( void ) const
    { return *visibilityAction_; }

    //@}

    protected slots:

    //! toggle visibility
    virtual void _toggleVisibility( bool );

    private slots:

    //! update configuration
    void _updateConfiguration( void );

    private:

    //! install actions
    void _installActions( void );

    //! assocated option name
    QString optionName_;

    //! visibility action
    QAction* visibilityAction_;

    //! use scroll area
    bool useScrollArea_;

    //! lock
    bool locked_;

    //! container
    QWidget* container_;

    //! main widget
    QWidget* mainWidget_;

};

#endif
