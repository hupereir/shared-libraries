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

#include "SystemEnvironmentDialog.h"
#include "TreeView.h"

#if QT_VERSION >= 0x040800
#include <QProcessEnvironment>
#else
#include <QProcess>
#endif

#include <QPushButton>
#include <QLayout>

//__________________________________________________________________________
SystemEnvironmentDialog::SystemEnvironmentDialog( QWidget* parent ):
CustomDialog( parent, CloseButton )
{

    Debug::Throw( "SystemEnvironmentDialog::SystemEnvironmentDialog.\n" );
    setWindowTitle( tr( "System Environment Variables" ) );
    setOptionName( "SYSTEM_ENVIRONMENT_DIALOG" );

    layout()->setMargin(0);
    buttonLayout().setMargin(5);

    // tell dialog to delete when close
    setAttribute( Qt::WA_DeleteOnClose );

    // insert list
    auto list = new TreeView( this );
    mainLayout().addWidget( list );
    list->setModel( &model_ );
    list->setRootIsDecorated( false );
    list->setMask( (1<<OptionModel::Name)|(1<<OptionModel::Value) );

    // retrieve environment variables from QProcess
    OptionModel::List options;

    #if QT_VERSION >= 0x040800
    auto environment = QProcessEnvironment::systemEnvironment();
    for( const auto& key:environment.keys() )
    { options.append( { key, Option(environment.value( key )) } ); }
    #else
    for( const auto& line:QProcess::systemEnvironment())
    {
        auto parsed( line.split( "=" ) );
        if( parsed.empty() ) continue;
        options.append( { parsed[0], parsed.size() > 1 ? Option( parsed[1] ): Option() } );
    }
    #endif

    model_.set( options );
    list->resizeColumns();

}
