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
\file HelpManager.cpp
\brief reference manual help system
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "BaseIcons.h"
#include "CustomDialog.h"
#include "Debug.h"
#include "HelpDialog.h"
#include "HelpManager.h"
#include "IconEngine.h"
#include "QtUtil.h"
#include "XmlError.h"
#include "XmlOptions.h"

#include <QtGui/QApplication>
#include <QtGui/QAction>
#include <QtGui/QVBoxLayout>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

namespace BASE
{
    //_________________________________________________________
    HelpManager::HelpManager( QObject* parent ):
        QObject( parent ),
        Counter( "HelpManager" ),
        windowTitle_( "Reference Manual" ),
        modified_( false )
    {

        Debug::Throw( "HelpManager::HelpManager.\n" );

        // actions
        displayAction_ = new QAction( IconEngine::get( ICONS::HELP ), "Reference Manual", this );
        displayAction_->setShortcut( QKeySequence::HelpContents );
        connect( displayAction_, SIGNAL( triggered() ), SLOT( _display() ) );

        dumpAction_ = new QAction( "Dump Help", this );
        connect( dumpAction_, SIGNAL( triggered() ), SLOT( _dumpHelpString() ) );

        connect( qApp, SIGNAL( aboutToQuit() ), SLOT( _save() ) );

    }

    //_________________________________________________________
    void HelpManager::install( const QString text[], bool clear )
    {

        Debug::Throw( "HelpManager::install.\n" );

        // clear existing text
        if( clear ) HelpManager::clear();

        //! loop over help text
        for( unsigned int i=0; !text[i].isNull(); i++ ) {

            QString label( text[i] );
            i++;
            if( text[i].isNull() ) break;
            items_.push_back( HelpItem( label, text[i] ) );
        }

        return;

    }

    //_________________________________________________________
    void HelpManager::install( const QString& file )
    {

        Debug::Throw( "HelpManager::Install.\n" );

        // set file and check
        file_ = file;
        if( !QFileInfo( file_ ).exists() ) return;

        // parse the file
        QFile qtfile( file );
        if ( !qtfile.open( QIODevice::ReadOnly ) )
        {
            Debug::Throw( "HelpManager::install - cannot open file.\n" );
            return;
        }

        // dom document
        QDomDocument document;
        XmlError error( file );
        if ( !document.setContent( &qtfile, &error.error(), &error.line(), &error.column() ) ) {
            qtfile.close();
            return;
        }

        // clear existing help
        clear();

        // loop over dom elements
        QDomElement doc_element = document.documentElement();
        QDomNode node = doc_element.firstChild();
        for(QDomNode node = doc_element.firstChild(); !node.isNull(); node = node.nextSibling() )
        {
            QDomElement element = node.toElement();
            if( element.isNull() ) continue;

            // special options
            if( element.tagName() == XML_ITEM ) items_.push_back( HelpItem( element ) );

        }

        return;

    }

    //_____________________________________________________
    void HelpManager::setWindowTitle( const QString& value )
    {
        Debug::Throw( "HelpManager::setWindowTitle.\n" );
        windowTitle_ = value;
        displayAction().setText( value );
    }

    //_____________________________________________________
    void HelpManager::_display( void )
    {

        Debug::Throw( "HelpManager::_display.\n" );

        // create dialog
        HelpDialog* dialog( new HelpDialog( *this ) );
        dialog->setWindowTitle( windowTitle_ );
        dialog->setWindowIcon( QPixmap( File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).expand() ) );
        dialog->setItems( items_ );
        dialog->setEditEnabled( file_.size() );
        dialog->centerOnWidget( qApp->activeWindow() );
        dialog->show();
        return;

    }

    //_________________________________________________________
    void HelpManager::_dumpHelpString( void )
    {

        Debug::Throw( "HelpManager::_dumpHelpString.\n" );

        // write output to stream
        QString buffer;
        QTextStream out( &buffer );

        // retrieve all items from dialog
        out << "static const char* helpText[] = {\n";
        for( HelpItem::List::const_iterator iter = items_.begin(); iter != items_.end(); ++iter )
        {

            // dump label
            out << "  //_________________________________________________________\n";
            out << "  \"" << iter->label() << "\",\n";

            // dump text
            QString text( iter->text() );
            text = text.replace( "\"", "\\\"" );
            text = text.replace( "\n", "\\n\"\n  \"" );
            out << "  \"" << text << "\"";
            out << ",\n";
            out << "\n";
        }
        out << "  0\n";
        out << "};\n";

        CustomDialog* dialog = new CustomDialog( 0, CustomDialog::OkButton );
        TextEditor *editor = new TextEditor( dialog );
        dialog->layout()->setMargin(0);
        dialog->buttonLayout().setMargin(5);
        dialog->mainLayout().addWidget( editor );

        editor->setWrapFromOptions( false );
        editor->wrapModeAction().setChecked( false );
        editor->setPlainText( buffer );
        dialog->resize( 600, 500 );
        dialog->setOptionName( "DUMP_HELP_DIALOG" );
        dialog->centerOnWidget( qApp->activeWindow() );
        dialog->show();

    }

    //_____________________________________________________
    void HelpManager::_save( void )
    {

        Debug::Throw() << "HelpManager::_save - file: " << file_ << endl;

        if( file_.isEmpty() ) return;
        if( !modified() ) return;

        // output file
        QFile out( file_ );
        if( !out.open( QIODevice::WriteOnly ) ) return;

        // create document
        QDomDocument document;

        // top element
        QDomElement top = document.appendChild( document.createElement( XML_HELP ) ).toElement();
        for( HelpItem::List::const_iterator iter = items_.begin(); iter != items_.end(); ++iter )
        { top.appendChild( iter->domElement( document ) ); }

        out.write( document.toByteArray() );
        out.close();
        setModified( false );
        return;
    }
}
