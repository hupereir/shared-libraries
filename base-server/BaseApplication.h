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
    explicit BaseApplication( QObject* parent, CommandLineArguments arguments = CommandLineArguments() );

    //* initialize application manager
    bool initApplicationManager() override;

    //* create all widgets
    bool realizeWidget() override;

    //* true if fixed fonts are used
    void setUseFixedFonts( bool value )
    {
        if( useFixedFonts_ == value ) return;
        useFixedFonts_ = value;
        _updateFonts();
    }

    //* true if fixed fonts are used
    bool useFixedFonts() const
    { return useFixedFonts_; }

    //*@name actions
    //@{

    //* about
    QAction& aboutAction() const
    { return *aboutAction_; }

    //* about
    QAction& aboutQtAction() const
    { return *aboutQtAction_; }

    //* configuration
    QAction& configurationAction() const
    { return *configurationAction_; }

    //* exit safely
    QAction& closeAction() const
    { return *closeAction_; }

    //@}

    //*@name application information
    //@{

    //* command line parser
    CommandLineParser commandLineParser( CommandLineArguments arguments = CommandLineArguments(), bool ignoreWarnings = true ) const override;

    // application icon
    virtual QIcon applicationIcon() const = 0;

    //@}

    public Q_SLOTS:

    //* set application busy
    void busy();

    //* set application idle
    void idle();

    protected Q_SLOTS:

    //* process request from application manager
    void _aboutQt();

    //* configuration
    virtual void _configuration() = 0;

    private Q_SLOTS:

    //* configuration
    void _updateConfiguration();

    //* fonts
    void _updateFonts();

    //* update icon path
    void _updateIconTheme();

    private Q_SLOTS:

    //* application 'about' dialog
    virtual void _about();

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
