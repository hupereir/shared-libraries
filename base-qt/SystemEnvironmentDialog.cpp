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

#include <QProcess>
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
    TreeView* list = new TreeView( this );
    mainLayout().addWidget( list );
    list->setModel( &model_ );
    list->setRootIsDecorated( false );
    list->setMask( (1<<OptionModel::Name)|(1<<OptionModel::Value) );

    // retrieve environment variables from QProcess
    OptionModel::List options;
    for( auto line:QProcess::systemEnvironment())
    {
        QStringList parsed( line.split( "=" ) );
        if( parsed.empty() ) continue;

        options << OptionPair( parsed[0], parsed.size() > 1 ? parsed[1]: "" );

    }

    model_.set( options );
    list->resizeColumns();

}
