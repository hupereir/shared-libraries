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

#include "BaseApplication.h"
#include "BaseIcons.h"
#include "File.h"
#include "FlatStyle.h"
#include "IconEngine.h"
#include "QtUtil.h"
#include "Util.h"
#include "XmlOptions.h"

#include <QtGui/QApplication>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QMessageBox>

#include <QtCore/QSettings>

#ifndef QT_NO_DBUS
#include <QtDBus/QDBusConnection>
#endif

//____________________________________________
namespace SERVER
{
    //! event filter used to change appearance of QMessageBox before show, to match KDE layout
    class AppEventFilter: public QObject
    {
        public:
        virtual bool eventFilter( QObject*, QEvent* );
    };

}

//____________________________________________
bool SERVER::AppEventFilter::eventFilter( QObject* object, QEvent* event )
{
    Q_UNUSED( object );

    switch( event->type() )
    {

        case QEvent::Show:

        if( QMessageBox* messageBox = qobject_cast<QMessageBox*>( object ) )
        {

            // try cast to message box and change buttons
            messageBox->setStandardButtons( QMessageBox::Close );

        } else if( QDialogButtonBox* buttonBox = qobject_cast<QDialogButtonBox*>( object ) ) {

            // insert separator
            if( QGridLayout* gridLayout = qobject_cast<QGridLayout*>( buttonBox->parentWidget()->layout() ) )
            {

                // create separator
                QFrame* frame( new QFrame( buttonBox->parentWidget() ) );
                frame->setFrameStyle( QFrame::HLine );
                gridLayout->addWidget( frame, 2, 0, 1, 2 );

                gridLayout->addWidget( buttonBox, 3, 0, 1, 2 );
            }

        }
        break;

        default: break;

    }

    return false;
}

//____________________________________________
BaseApplication::BaseApplication( QObject* parent, CommandLineArguments arguments ) :
    BaseCoreApplication( parent, arguments ),
    useFixedFonts_( false )
{

    Debug::Throw( "BaseApplication::BaseApplication.\n" );
    connect( this, SIGNAL( configurationChanged( void ) ), SLOT( _updateConfiguration( void ) ) );
    connect( this, SIGNAL( configurationChanged( void ) ), SLOT( _updateFonts( void ) ) );
    if( XmlOptions::get().get<bool>( "USE_FLAT_THEME" ) ) qApp->setStyle( new FlatStyle() );

    // use DBus connection to update on oxygen configuration change
    #ifndef QT_NO_DBUS
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.connect( QString(), "/OxygenStyle", "org.kde.Oxygen.Style", "reparseConfiguration", this, SLOT(_updateFonts( void ) ) );
    dbus.connect( QString(), "/KGlobalSettings", "org.kde.KGlobalSettings", "notifyChange", this, SLOT(_updateFonts( void ) ) );
    #endif

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
    connect( aboutQtAction_, SIGNAL( triggered() ), this, SLOT( _aboutQt() ) );

    closeAction_ = new QAction( IconEngine::get( ICONS::EXIT ), "Exit", this );
    closeAction_->setShortcut( QKeySequence::Quit );
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
void BaseApplication::_aboutQt( void )
{
    Debug::Throw( "BaseApplication::aboutQt.\n" );
    SERVER::AppEventFilter eventFilter;
    qApp->installEventFilter( &eventFilter );
    QMessageBox::aboutQt(0);
}

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
    QPixmap pixmap( XmlOptions::get().raw( XmlOptions::get().contains( "LARGE_ICON_PIXMAP" ) ? "LARGE_ICON_PIXMAP":"ICON_PIXMAP" ) );
    dialog.setWindowIcon( pixmap );
    dialog.setIconPixmap( pixmap );
    dialog.setText( buffer );
    dialog.adjustSize();
    QtUtil::centerOnWidget( &dialog, qApp->activeWindow() );

    SERVER::AppEventFilter eventFilter;
    qApp->installEventFilter( &eventFilter );

    dialog.show();
    dialog.exec();

}

//_______________________________________________
void BaseApplication::_updateConfiguration( void )
{
    Debug::Throw( "BaseApplication::_updateConfiguration.\n" );

    // application icon
    qApp->setWindowIcon( QPixmap( XmlOptions::get().raw( "ICON_PIXMAP" ) ) );

    // reload IconEngine cache (in case of icon_path_list that changed)
    IconEngine::get().reload();

}

//_______________________________________________
void BaseApplication::_updateFonts( void )
{

    Debug::Throw( "BaseApplication::_updateFonts.\n" );

    // default widget font
    if( !XmlOptions::get().get<bool>( "USE_SYSTEM_FONT" ) )
    {

        QFont font;

        // generic font
        font.fromString( XmlOptions::get().raw( "FONT_NAME" ) );
        qApp->setFont( font );

        // generic font
        if( useFixedFonts() ) font.fromString( XmlOptions::get().raw( "FIXED_FONT_NAME" ) );
        qApp->setFont( font, "QTextEdit" );

    } else {

        QStringList files;
        files
            << Util::home() + "/.kde4/share/config/kdeglobals"
            << Util::home() + "/.kde/share/config/kdeglobals";

        bool found( false );
        foreach( const QString& file, files )
        {

            // check file existence
            if( !File( file ).exists() ) continue;
            found = true;

            // load settings
            QSettings settings( file, QSettings::IniFormat );
            settings.sync();

            // debug
            Debug::Throw() << "BaseApplication::_updateFonts - font: " << settings.value( "font" ).toStringList().join( "," ) << endl;
            Debug::Throw() << "BaseApplication::_updateFonts - fixed: " << settings.value( "fixed" ).toStringList().join( "," ) << endl;

            // generic font
            QFont font;
            font.fromString( settings.value( "font" ).toStringList().join( "," ) );
            qApp->setFont( font );

            // fixed fonts
            if( useFixedFonts() ) font.fromString( settings.value( "fixed" ).toStringList().join( "," ) );
            qApp->setFont( font, "QTextEdit" );

        }

        if( !found )
        {
            qApp->setFont( QFont() );
            qApp->setFont( QFont(), "QTextEdit" );
        }

    }

}
