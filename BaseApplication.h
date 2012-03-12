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

/*!
\file BaseApplication.h
\brief application main object
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "ApplicationManager.h"
#include "BaseCoreApplication.h"
#include "CommandLineArguments.h"

#include <QtGui/QAction>
#include <QtCore/QObject>

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

    public slots:

    //! set application busy
    virtual void busy( void );

    //! set application idle
    virtual void idle( void );

    protected slots:

    //! process request from application manager
    virtual void _about( void ) = 0;

    //! process request from application manager
    virtual void _aboutQt( void );

    //! configuration
    virtual void _configuration( void ) = 0;

    //! about to quit
    virtual void _aboutToQuit( void );

    private slots:

    //! fonts
    void _updateFonts( void );

    //! configuration
    void _updateConfiguration( void );

    protected:

    //! application 'about' dialog
    virtual void _about( QString, QString version = "", QString stamp = "" );

    //! application 'about' dialog
    virtual void _about( QString name, double version, QString stamp = "" )
    { _about( name, QString().setNum( version ), stamp ); }

    private:

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
