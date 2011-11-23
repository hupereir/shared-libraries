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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
\file Application.cc
\brief application main object
\author  Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "BaseApplication.h"
#include "BaseIcons.h"
#include "FlatStyle.h"
#include "IconEngine.h"
#include "QtUtil.h"
#include "XmlOptions.h"

#include <QtGui/QApplication>
#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QMessageBox>

//____________________________________________
BaseApplication::BaseApplication( QObject* parent, CommandLineArguments arguments ) :
    BaseCoreApplication( parent, arguments )
{

    Debug::Throw( "BaseApplication::BaseApplication.\n" );
    connect( this, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
    if( XmlOptions::get().get<bool>( "USE_FLAT_THEME" ) ) qApp->setStyle( new FlatStyle() );

}

//____________________________________________
BaseApplication::~BaseApplication( void )
{ Debug::Throw( "BaseApplication::~BaseApplication.\n" ); }

//____________________________________________
bool BaseApplication::realizeWidget( void )
{
    Debug::Throw( "BaseApplication::realizeWidget.\n" );

    // check if the method has already been called.
    if( !BaseCoreApplication::realizeWidget() ) return false;

    // connections
    connect( qApp, SIGNAL( aboutToQuit( void ) ), SLOT( _aboutToQuit() ) );

    // actions
    aboutAction_ = new QAction( QPixmap( XmlOptions::get().raw( "ICON_PIXMAP" ) ), "About this &Application", this );
    connect( aboutAction_, SIGNAL( triggered() ), SLOT( _about() ) );

    aboutQtAction_ = new QAction( IconEngine::get( ICONS::ABOUT_QT ), "About Qt", this );
    connect( aboutQtAction_, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );

    closeAction_ = new QAction( IconEngine::get( ICONS::EXIT ), "Exit", this );
    closeAction_->setShortcut( Qt::CTRL + Qt::Key_Q );
    connect( closeAction_, SIGNAL( triggered() ), qApp, SLOT( quit() ) );

    configurationAction_ = new QAction( IconEngine::get( ICONS::CONFIGURE ), "Default Configuration", this );
    connect( configurationAction_, SIGNAL( triggered() ), SLOT( _configuration() ) );

    return true;
}


//____________________________________________
void BaseApplication::busy( void )
{
    qApp->setOverrideCursor( Qt::WaitCursor );
    qApp->processEvents();
}

//____________________________________________
void BaseApplication::idle( void )
{ qApp->restoreOverrideCursor(); }

//_______________________________________________
void BaseApplication::_aboutToQuit( void )
{ Debug::Throw( "BaseApplication::_aboutToQuit.\n" ); }

//_______________________________________________
void BaseApplication::_about( QString name, QString version, QString stamp )
{

    Debug::Throw( "BaseApplication::about.\n" );

    // modify version to remvoe qt4 for version
    if( version.indexOf( "qt4_" ) >= 0 )
    { version = version.replace( "qt4_", "" ) + " (qt4)"; }

    QString buffer;
    QTextStream in( &buffer, QIODevice::WriteOnly );
    if( !name.isEmpty() ) { in << "<h3>" << name << "</h3>"; }
    if( !version.isEmpty() ) { in << "version " << version; }
    if( !stamp.isEmpty() ) { in << " (" << stamp << ")"; }

    in
        << "<p>This application was written for personal use only. "
        << "It is not meant to be bug free, although all efforts "
        << "are made so that it remains/becomes so. "

        << "<p>Suggestions, comments and bug reports are welcome. "
        << "Please use the following e-mail address:"

        << "<p><a href=\"mailto:hugo.pereira@free.fr\">hugo.pereira@free.fr</a>";

    QMessageBox dialog;
    dialog.setWindowTitle( QString( "About ")+name );

    QPixmap pixmap( XmlOptions::get().raw( "ICON_PIXMAP" ) );
    dialog.setWindowIcon( pixmap );
    dialog.setIconPixmap( pixmap );
    dialog.setText( buffer );
    dialog.adjustSize();
    QtUtil::centerOnWidget( &dialog, qApp->activeWindow() );
    dialog.exec();

}

//_______________________________________________
void BaseApplication::_updateConfiguration( void )
{
    Debug::Throw( "BaseApplication::_updateConfiguration.\n" );

    // application icon
    qApp->setWindowIcon( QPixmap( XmlOptions::get().raw( "ICON_PIXMAP" ) ) );

    // set fonts
    QFont font;
//     font.fromString( XmlOptions::get().raw( "FONT_NAME" ) );
//     qApp->setFont( font );

    font.fromString( XmlOptions::get().raw( "FIXED_FONT_NAME" ) );
    qApp->setFont( font, "QTextEdit" );

    // reload IconEngine cache (in case of icon_path_list that changed)
    IconEngine::get().reload();

    // emit signal to propagate changes to other widgets
    Debug::Throw( "BaseApplication::_updateConfiguration - done.\n" );

}
