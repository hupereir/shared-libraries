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

#include "HelpManager.h"
#include "BaseIconNames.h"
#include "Debug.h"
#include "Dialog.h"
#include "HelpDialog.h"
#include "IconEngine.h"
#include "QtUtil.h"
#include "XmlOptions.h"

#include <QApplication>
#include <QAction>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>

namespace Base
{
    //_________________________________________________________
    HelpManager::HelpManager( QObject* parent ):
        QObject( parent ),
        Counter( QStringLiteral("HelpManager") ),
        windowTitle_( tr( "Reference Manual" ) )
    {

        Debug::Throw( QStringLiteral("HelpManager::HelpManager.\n") );

        // actions
        displayAction_ = new QAction( IconEngine::get( IconNames::Help ), tr( "%1 Handbook" ).arg( qApp->applicationName() ), this );
        displayAction_->setShortcut( QKeySequence::HelpContents );
        connect( displayAction_, &QAction::triggered, this, &HelpManager::_display );

    }

    //_________________________________________________________
    void HelpManager::install( const QString text[], bool clear )
    {

        Debug::Throw( QStringLiteral("HelpManager::install.\n") );

        // clear existing text
        if( clear ) HelpManager::clear();

        //! loop over help text
        for( int i=0; !text[i].isNull(); i++ ) {

            QString label( text[i] );
            i++;
            if( text[i].isNull() ) break;
            items_.append( HelpItem( label, text[i] ) );
        }

        return;

    }

    //_____________________________________________________
    void HelpManager::setWindowTitle( const QString& value )
    {
        Debug::Throw( QStringLiteral("HelpManager::setWindowTitle.\n") );
        windowTitle_ = value;
        displayAction().setText( value );
    }

    //_____________________________________________________
    void HelpManager::_display()
    {

        Debug::Throw( QStringLiteral("HelpManager::_display.\n") );

        // create dialog
        auto dialog = new HelpDialog;
        dialog->setAttribute( Qt::WA_DeleteOnClose );
        dialog->setWindowTitle( windowTitle_ );
        dialog->setItems( items_ );
        dialog->centerOnWidget( qApp->activeWindow() );
        dialog->show();
        return;

    }

}
