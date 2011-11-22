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
* ANY WARRANTY;  without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.   See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA   02111-1307 USA
*
*
*******************************************************************************/
/*!
\file SystemEnvironmentDialog.cpp
\brief displays system environment
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "SystemEnvironmentDialog.h"
#include "AnimatedTreeView.h"
#include "Str.h"

#include <QtCore/QProcess>
#include <QtGui/QPushButton>
#include <QtGui/QLayout>

//__________________________________________________________________________
SystemEnvironmentDialog::SystemEnvironmentDialog( QWidget* parent ):
CustomDialog( parent, CloseButton )
{

    Debug::Throw( "SystemEnvironmentDialog::SystemEnvironmentDialog.\n" );
    setWindowTitle( "System Environment Variables" );
    setOptionName( "SYSTEM_ENVIRONMENT_DIALOG" );

    // tell dialog to delete when close
    setAttribute( Qt::WA_DeleteOnClose );

    // insert list
    AnimatedTreeView* list = new AnimatedTreeView( this );
    mainLayout().addWidget( list );
    list->setModel( &model_ );
    list->setRootIsDecorated( false );
    list->setMask( (1<<OptionModel::NAME)|(1<<OptionModel::VALUE) );

    // retrieve environment variables from QProcess
    QStringList env( QProcess::systemEnvironment() );
    OptionModel::Set options;
    for( QStringList::iterator iter = env.begin(); iter != env.end(); ++iter )
    {
        QStringList parsed( (iter)->split( "=" ) );
        if( parsed.empty() ) continue;

        options.insert( OptionPair( parsed[0], parsed.size() > 1 ? parsed[1]: "" ) );

    }

    model_.set( options );
    list->resizeColumns();

}
