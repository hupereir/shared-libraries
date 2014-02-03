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

#include "BaseIconNames.h"
#include "CustomDialog.h"
#include "Debug.h"
#include "HelpDialog.h"
#include "HelpManager.h"
#include "HelpManager.moc"
#include "IconEngine.h"
#include "QtUtil.h"
#include "XmlOptions.h"

#include <QApplication>
#include <QAction>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>

namespace BASE
{
    //_________________________________________________________
    HelpManager::HelpManager( QObject* parent ):
        QObject( parent ),
        Counter( "HelpManager" ),
        windowTitle_( tr( "Reference Manual" ) )
    {

        Debug::Throw( "HelpManager::HelpManager.\n" );

        // actions
        displayAction_ = new QAction( IconEngine::get( IconNames::Help ), QString( tr( "%1 Handbook" ) ).arg( qApp->applicationName() ), this );
        displayAction_->setShortcut( QKeySequence::HelpContents );
        connect( displayAction_, SIGNAL(triggered()), SLOT(_display()) );

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
            items_ << HelpItem( label, text[i] );
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
        dialog->setItems( items_ );
        dialog->centerOnWidget( qApp->activeWindow() );
        dialog->show();
        return;

    }

}
