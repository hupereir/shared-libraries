#ifndef BaseApplication_h
#define BaseApplication_h

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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "ApplicationManager.h"
#include "BaseCoreApplication.h"
#include "CommandLineArguments.h"

#include <QAction>
#include <QObject>

//! Main Window singleton object
class BaseApplication: public BaseCoreApplication
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    BaseApplication( QObject* parent, CommandLineArguments arguments = CommandLineArguments() );

    //! destructor
    virtual ~BaseApplication( void );

    //! create all widgets
    virtual bool realizeWidget( void );

    //! true if fixed fonts are used
    virtual void setUseFixedFonts( bool value )
    {
        if( useFixedFonts_ == value ) return;
        useFixedFonts_ = value;
        _updateFonts();
    }

    //! true if fixed fonts are used
    virtual bool useFixedFonts( void ) const
    { return useFixedFonts_; }

    //!@name actions
    //@{

    //! about
    QAction& aboutAction( void ) const
    { return *aboutAction_; }

    //! about
    QAction& aboutQtAction( void ) const
    { return *aboutQtAction_; }

    //! configuration
    QAction& configurationAction( void ) const
    { return *configurationAction_; }

    //! exit safely
    QAction& closeAction( void ) const
    { return *closeAction_; }

    //@}

    public Q_SLOTS:

    //! set application busy
    virtual void busy( void );

    //! set application idle
    virtual void idle( void );

    protected Q_SLOTS:

    //! process request from application manager
    virtual void _about( void ) = 0;

    //! process request from application manager
    virtual void _aboutQt( void );

    //! configuration
    virtual void _configuration( void ) = 0;

    //! about to quit
    virtual void _aboutToQuit( void );

    protected:

    // application icon
    QIcon _applicationIcon( void ) const;

    private Q_SLOTS:

    //! configuration
    void _updateConfiguration( void );

    //! fonts
    void _updateFonts( void );

    //! update icon path
    void _updateIconTheme( void );

    protected:

    //! application 'about' dialog
    virtual void _about( const QString&, const QString& = QString(), const QString& = QString() );

    private:

    //! true if fixed fonts are used
    bool useFixedFonts_;

    //!@name actions
    //@{

    //! about action
    QAction* aboutAction_;

    //! about qt
    QAction* aboutQtAction_;

    //! configure
    QAction* configurationAction_;

    //! close
    QAction* closeAction_;

    //@}

};

#endif
