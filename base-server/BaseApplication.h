#ifndef BaseApplication_h
#define BaseApplication_h

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

//* Main Window singleton object
class BaseApplication: public BaseCoreApplication
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    BaseApplication( QObject* parent, CommandLineArguments arguments = CommandLineArguments() );

    //* initialize application manager
    virtual bool initApplicationManager( void );

    //* create all widgets
    virtual bool realizeWidget( void );

    //* true if fixed fonts are used
    virtual void setUseFixedFonts( bool value )
    {
        if( useFixedFonts_ == value ) return;
        useFixedFonts_ = value;
        _updateFonts();
    }

    //* true if fixed fonts are used
    virtual bool useFixedFonts( void ) const
    { return useFixedFonts_; }

    //*@name actions
    //@{

    //* about
    QAction& aboutAction( void ) const
    { return *aboutAction_; }

    //* about
    QAction& aboutQtAction( void ) const
    { return *aboutQtAction_; }

    //* configuration
    QAction& configurationAction( void ) const
    { return *configurationAction_; }

    //* exit safely
    QAction& closeAction( void ) const
    { return *closeAction_; }

    //@}

    //*@name application information
    //@{

    //* command line parser
    virtual CommandLineParser commandLineParser( CommandLineArguments arguments = CommandLineArguments(), bool ignoreWarnings = true ) const;

    // application icon
    virtual QIcon applicationIcon( void ) const = 0;

    //@}

    public Q_SLOTS:

    //* set application busy
    virtual void busy( void );

    //* set application idle
    virtual void idle( void );

    protected Q_SLOTS:

    //* process request from application manager
    virtual void _aboutQt( void );

    //* configuration
    virtual void _configuration( void ) = 0;

    //* about to quit
    virtual void _aboutToQuit( void );

    private Q_SLOTS:

    //* configuration
    void _updateConfiguration( void );

    //* fonts
    void _updateFonts( void );

    //* update icon path
    void _updateIconTheme( void );

    private Q_SLOTS:

    //* application 'about' dialog
    virtual void _about( void );

    private:

    //* true if fixed fonts are used
    bool useFixedFonts_ = false;

    //*@name actions
    //@{

    //* about action
    QAction* aboutAction_ = nullptr;

    //* about qt
    QAction* aboutQtAction_ = nullptr;

    //* configure
    QAction* configurationAction_ = nullptr;

    //* close
    QAction* closeAction_ = nullptr;

    //@}

};

#endif
