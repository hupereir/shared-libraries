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

#include "GridLayout.h"
#include "Debug.h"
#include "OptionLineEditor.h"
#include "OptionSpinBox.h"
#include "ServerConfiguration.h"
#include "ServerConfiguration.moc"

#include <QGroupBox>
#include <QLabel>
#include <QLayout>

namespace Server
{
    //________________________________________________________________________
    ServerConfiguration::ServerConfiguration( QWidget* parent, QString title ):
        QWidget( parent ),
        OptionWidgetList( this ),
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

        QString tooltip(
            tr( "The application server configuration refers to the host name\n"
            "and port used for inter-process communication. It notably ensures\n"
            "that only one instance of each application runs at a time." ) );

        // host
        gridLayout->addWidget( new QLabel( tr( "Host:" ), box ) );
        gridLayout->addWidget( line_editor = new OptionLineEditor( box, "SERVER_HOST" ) );
        line_editor->setToolTip( tooltip );
        addOptionWidget( line_editor );

        // shadow color
        gridLayout->addWidget( new QLabel( tr( "Port:" ), box ) );
        gridLayout->addWidget( spinBox = new OptionSpinBox( box, "SERVER_PORT" ) );
        spinBox->setMinimum(0);
        spinBox->setMaximum(10000);
        spinBox->setToolTip( tooltip );
        addOptionWidget( spinBox );

        return;
    }

}
