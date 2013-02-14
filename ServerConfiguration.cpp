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

#include "GridLayout.h"
#include "Debug.h"
#include "OptionLineEditor.h"
#include "OptionSpinBox.h"
#include "ServerConfiguration.h"

#include <QGroupBox>
#include <QLabel>
#include <QLayout>

namespace SERVER
{
    //________________________________________________________________________
    ServerConfiguration::ServerConfiguration( QWidget* parent, QString title ):
        QWidget( parent ),
        Counter( "ServerConfiguration" )
    {
        Debug::Throw( "ServerConfiguration::ServerConfiguration.\n" );

        setLayout( new QVBoxLayout() );
        layout()->setSpacing(5);
        layout()->setMargin(0);

        // generic objects
        QWidget *box;
        OptionLineEditor* line_editor;
        OptionSpinBox* spinBox;

        // general
        if( title.isEmpty() ) layout()->addWidget( box = new QWidget( this ) );
        else layout()->addWidget( box = new QGroupBox( title, this ) );

        GridLayout* gridLayout = new GridLayout();
        gridLayout->setSpacing(5);
        gridLayout->setMargin(5);
        gridLayout->setMaxCount( 2 );
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        box->setLayout( gridLayout );

        QString tooltip( "The application server configuration refers to the host name\n"
            "and port used for inter-process communication. It notably ensures\n"
            "that only one instance of each application runs at a time." );

        // host
        gridLayout->addWidget( new QLabel( "Host:", box ) );
        gridLayout->addWidget( line_editor = new OptionLineEditor( box, "SERVER_HOST" ) );
        line_editor->setToolTip( tooltip );
        addOptionWidget( line_editor );

        // shadow color
        gridLayout->addWidget( new QLabel( "Port:", box ) );
        gridLayout->addWidget( spinBox = new OptionSpinBox( box, "SERVER_PORT" ) );
        spinBox->setMinimum(0);
        spinBox->setMaximum(10000);
        spinBox->setToolTip( tooltip );
        addOptionWidget( spinBox );

        return;
    }

}
