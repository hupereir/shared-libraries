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

#include "BaseApplication.h"
#include "BaseIconNames.h"
#include "Command.h"
#include "CppUtil.h"
#include "CustomProcess.h"
#include "File.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "QtUtil.h"
#include "ServerSystemOptions.h"
#include "Util.h"
#include "XmlOptions.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QLayout>
#include <QLabel>
#include <QMessageBox>

#include <QSettings>

//____________________________________________
namespace Server
{

    //! event filter used to change appearance of QMessageBox before show, to match KDE layout
    class AppEventFilter: public QObject
    {
        Q_OBJECT

        public:
        virtual bool eventFilter( QObject*, QEvent* );
    };

}

//____________________________________________
bool Server::AppEventFilter::eventFilter( QObject* object, QEvent* event )
{

    switch( event->type() )
    {
        case QEvent::Show:
        {
            if( auto messageBox = qobject_cast<QMessageBox*>( object ) )
            {

                // try cast to message box and change buttons
                messageBox->setStandardButtons( QMessageBox::Close );

            } else if( auto buttonBox = qobject_cast<QDialogButtonBox*>( object ) ) {

                // insert separator
                if( auto gridLayout = qobject_cast<QGridLayout*>( buttonBox->parentWidget()->layout() ) )
                {
                    gridLayout->setVerticalSpacing( 5 );

                    // create separator
                    auto frame( new QFrame( buttonBox->parentWidget() ) );
                    frame->setFrameStyle( QFrame::HLine );
                    const int columnCount( gridLayout->columnCount() );
                    gridLayout->addWidget( frame, 2, 0, 1, columnCount );
                    gridLayout->addWidget( buttonBox, 3, 0, 1, columnCount );
                }
            }
            break;
        }

        default: break;

    }

    return false;
}

//____________________________________________
BaseApplication::BaseApplication( QObject* parent, const CommandLineArguments &arguments ) :
    BaseCoreApplication( parent, arguments )
{

    Debug::Throw( QStringLiteral("BaseApplication::BaseApplication.\n") );

    installServerSystemOptions();

    connect( this, &BaseApplication::configurationChanged, this, &BaseApplication::_updateConfiguration );
    connect( this, &BaseCoreApplication::configurationChanged, this, &BaseApplication::_updateFonts );
    connect( this, &BaseCoreApplication::configurationChanged, this, &BaseApplication::_updateIconTheme );

}

//____________________________________________
bool BaseApplication::initApplicationManager()
{
    Debug::Throw( QStringLiteral("BaseApplication::initApplicationManager.\n") );

    // assign application name to qapplication
    qApp->setWindowIcon( applicationIcon() );
    return BaseCoreApplication::initApplicationManager();
}

//____________________________________________
bool BaseApplication::realizeWidget()
{
    Debug::Throw( QStringLiteral("BaseApplication::realizeWidget.\n") );

    // check if the method has already been called.
    if( !BaseCoreApplication::realizeWidget() ) return false;

    // always enable high dpi
    qApp->setAttribute( Qt::AA_UseHighDpiPixmaps, true );

    // need to update icon theme upfront to make sure themed icons are loaded
    _updateIconTheme();

    // actions
    aboutAction_ = new QAction( applicationIcon(), tr( "About %1" ).arg( applicationName() ), this );
    connect( aboutAction_, &QAction::triggered, this, &BaseApplication::_about );

    aboutQtAction_ = new QAction( IconEngine::get( IconNames::AboutQt, Base::IconCacheItem::Flag::FromCache|Base::IconCacheItem::Flag::FromResource ), tr( "About Qt" ), this );
    connect( aboutQtAction_, &QAction::triggered, this, &BaseApplication::_aboutQt );

    closeAction_ = new QAction( IconEngine::get( IconNames::Exit ), tr( "Exit" ), this );
    closeAction_->setShortcut( QKeySequence::Quit );
    connect( closeAction_, &QAction::triggered, qApp, &QApplication::quit );

    configurationAction_ = new QAction( IconEngine::get( IconNames::Configure ), tr( "Configure %1..." ).arg( applicationName() ), this );
    connect( configurationAction_, &QAction::triggered, this, &BaseApplication::_configuration );

    return true;
}

//_______________________________________________
CommandLineParser BaseApplication::commandLineParser( const CommandLineArguments &arguments, bool ignoreWarnings ) const
{

    Debug::Throw() << "BaseApplication::commandLineParser" << Qt::endl;

    CommandLineParser out( BaseCoreApplication::commandLineParser() );

    // these are additional flags recognized by Qt.
    // this may be system dependent, and vary from one Qt version to the other,
    // but is not very important. They are listed here only to avoid warnings from the application.
    out.setGroup( CommandLineParser::qtGroupName );
    out.registerOption( QStringLiteral("-style"), QStringLiteral("string"), QObject::tr( "Qt widget style" ) );
    out.registerOption( QStringLiteral("-graphicssystem"), QObject::tr( "string" ), QObject::tr( "Qt drawing backend (native|raster)" ) );
    out.registerOption( QStringLiteral("-platform"), QStringLiteral("string"), QObject::tr( "Qt platform" ) );

    if( !arguments.isEmpty() )
    { out.parse( arguments, ignoreWarnings ); }

    return out;

}

//____________________________________________
void BaseApplication::busy()
{ qApp->setOverrideCursor( Qt::WaitCursor ); }

//____________________________________________
void BaseApplication::idle()
{ qApp->restoreOverrideCursor(); }

//_______________________________________________
void BaseApplication::_aboutQt()
{
    Debug::Throw( QStringLiteral("BaseApplication::aboutQt.\n") );
    Server::AppEventFilter eventFilter;
    qApp->installEventFilter( &eventFilter );
    QMessageBox::aboutQt(nullptr);
}

//_______________________________________________
void BaseApplication::_about()
{

    Debug::Throw( QStringLiteral("BaseApplication::_about.\n") );

    // make sure name is all lower case and starts with upper case.
    auto name( applicationName() );
    if( !name.isEmpty() )
    {
        name = name.toLower();
        name[0] = name[0].toUpper();
    }

    // modify version to remove qt4 for version
    auto version( applicationVersion() );
    if( version.indexOf( QLatin1String("qt4_") ) >= 0 )
    { version = version.replace( QLatin1String("qt4_"), QLatin1String("") ) + " (qt4)"; }

    QString buffer;
    QTextStream in( &buffer, QIODevice::WriteOnly );
    if( !name.isEmpty() ) { in << "<h3>" << name << "</h3>"; }
    if( !version.isEmpty() ) { in << tr( "Version %1 " ).arg( version ); }

    in <<
        tr( "<p>Contact: <a href=\"mailto:hugo.pereira.da.costa@gmail.com\">hugo.pereira.da.costa@gmail.com</a><br/>"
        "License: <a href=\"http://www.gnu.org/licenses/gpl-2.0.html\">GNU General Public License Version 2</a><br/>");

    QMessageBox dialog;
    dialog.setWindowTitle( tr( "About %1" ).arg( name ) );
    auto icon( applicationIcon() );
    dialog.setWindowIcon( icon );

    const auto& size = IconSize::get( IconSize::Huge );
    dialog.setIconPixmap( icon.pixmap( size ) );
    dialog.setText( buffer );
    dialog.adjustSize();
    QtUtil::centerOnWidget( &dialog, qApp->activeWindow() );

    Server::AppEventFilter eventFilter;
    qApp->installEventFilter( &eventFilter );

    dialog.show();
    dialog.exec();

}

//_______________________________________________
void BaseApplication::_updateConfiguration()
{
    Debug::Throw( QStringLiteral("BaseApplication::_updateConfiguration.\n") );

    // application icon
    const QIcon icon( applicationIcon() );
    qApp->setWindowIcon( icon );

    // also assign to all existing top level widgets
    for( const auto& widget:qApp->topLevelWidgets() )
    { widget->setWindowIcon( icon ); }

    // reload IconEngine cache (in case of icon_path_list that changed)
    IconEngine::get().reload();

}

//_______________________________________________
void BaseApplication::_updateFonts()
{

    Debug::Throw( QStringLiteral("BaseApplication::_updateFonts.\n") );

    // default widget font
    if( !XmlOptions::get().get<bool>( QStringLiteral("USE_SYSTEM_FONT") ) )
    {

        QFont font;

        // generic font
        font.fromString( XmlOptions::get().raw( QStringLiteral("FONT_NAME") ) );
        qApp->setFont( font );

        // generic font
        if( useFixedFonts() ) font.fromString( XmlOptions::get().raw( QStringLiteral("FIXED_FONT_NAME") ) );
        qApp->setFont( font, "QTextEdit" );
        qApp->setFont( font, "QPlainTextEdit" );

    } else {

        File::List configurationFiles;
        if( XmlOptions::get().contains( QStringLiteral("KDE_CONFIG") ) )
        {

            // get kde4 config command and retrieve output
            const QString kde4ConfigCommand( XmlOptions::get().raw( QStringLiteral("KDE_CONFIG") ) );

            CustomProcess process( this );
            process.start( Base::Command( kde4ConfigCommand ) << "--path" << "config" );
            if( process.waitForFinished() && process.exitStatus() == QProcess::NormalExit )
            {
                auto configurationPath = QString( process.readAllStandardOutput() ).trimmed().split( QLatin1Char(':') );
                std::transform( configurationPath.begin(), configurationPath.end(), std::back_inserter( configurationFiles ),
                    []( const QString& path ) { return File( "kdeglobals" ).addPath( File( path ) ); } );
            }

        }

        if( configurationFiles.isEmpty() )
        {
            // add some files manually in case the above failed
            configurationFiles.append( File( ".config/kdeglobals" ).addPath( Util::home() ) );
            configurationFiles.append( File( ".kde/share/config/kdeglobals" ).addPath( Util::home() ) );
            configurationFiles.append( File( ".kde4/share/config/kdeglobals" ).addPath( Util::home() ) );
        }

        bool fontFound( false );
        bool fixedFontFound( !useFixedFonts() );
        for( const auto& file:configurationFiles )
        {

            // check file existence
            if( !file.exists() ) continue;

            Debug::Throw() << "BaseApplication::_updateFonts - file: " << file << Qt::endl;

            // load settings
            QSettings settings( file, QSettings::IniFormat );
            settings.sync();

            // generic font
            if( !fontFound && settings.contains( QStringLiteral("font") ) )
            {
                auto fontName( settings.value( QStringLiteral("font") ).toStringList().join( QStringLiteral(",") ) );
                QFont font;
                font.fromString( fontName );
                qApp->setFont( font );
                qApp->setFont( font, "QTextEdit" );
                qApp->setFont( font, "QPlainTextEdit" );
                fontFound = true;
                Debug::Throw() << "BaseApplication::_updateFonts - font: " << fontName << Qt::endl;
            }

            // fixed fonts
            if( useFixedFonts() && !fixedFontFound && settings.contains( QStringLiteral("fixed") ))
            {
                auto fixedFontName( settings.value( QStringLiteral("fixed") ).toStringList().join( QStringLiteral(",") ) );
                QFont font;
                font.fromString( fixedFontName );
                fixedFontFound = true;
                Debug::Throw() << "BaseApplication::_updateFonts - fixed: " << fixedFontName << Qt::endl;
                qApp->setFont( font, "QTextEdit" );
                qApp->setFont( font, "QPlainTextEdit" );
            }

            if( fontFound && fixedFontFound ) break;

        }

        if( !( fontFound && fixedFontFound ) )
        {
            qApp->setFont( QFont() );
            qApp->setFont( QFont(), "QTextEdit" );
            qApp->setFont( QFont(), "QPlainTextEdit" );
        }

    }

}

//_______________________________________________
void BaseApplication::_updateIconTheme()
{

    Debug::Throw( QStringLiteral("BaseApplication::_updateIconTheme.\n") );
    if( XmlOptions::get().get<bool>( QStringLiteral("USE_ICON_THEME") ) )
    {
        QIcon::setThemeSearchPaths( { XmlOptions::get().raw( QStringLiteral("ICON_THEME_PATH") ) } );
        QIcon::setThemeName( XmlOptions::get().raw( QStringLiteral("ICON_THEME") ) );
    }
}

#include "BaseApplication.moc"
