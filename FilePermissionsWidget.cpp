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

#include "FilePermissionsWidget.h"
#include "Debug.h"
#include "GridLayout.h"

#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QLayout>

//_____________________________________________________________________
FilePermissionsWidget::FilePermissionsWidget( QWidget* parent, QFile::Permissions permissions):
    QWidget( parent ),
    Counter( "FilePermissionsWidget" )
{

    Debug::Throw( "FilePermissionsWidget::FilePermissionsWidget\n" );

    GridLayout* gridLayout = new GridLayout();
    gridLayout->setMargin(0);
    gridLayout->setSpacing( 5 );
    gridLayout->setMaxCount( 4 );
    gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
    setLayout( gridLayout );

    gridLayout->addWidget( new QLabel( "<b>Permissions: </b>", this ) );
    gridLayout->addWidget( new QLabel( "Read", this ), Qt::AlignHCenter );
    gridLayout->addWidget( new QLabel( "Write", this ), Qt::AlignHCenter );
    gridLayout->addWidget( new QLabel( "Execute", this ), Qt::AlignHCenter );

    typedef QHash< QFile::Permission, QCheckBox* > CheckBoxMap;
    CheckBoxMap checkboxes;

    gridLayout->addWidget( new QLabel( "Owner:", this ) );
    gridLayout->addWidget( checkboxes[QFile::ReadOwner ]  = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes[QFile::WriteOwner ]  = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes[QFile::ExeOwner  ]  = new QCheckBox( this ), Qt::AlignHCenter );

    // on unix, right now, Qt does not return the current user permissions. Disable them from the dialog
    #if !defined(Q_WS_X11)
    gridLayout->addWidget( new QLabel( "User:", this ) );
    gridLayout->addWidget( checkboxes[QFile::ReadUser ]  = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes[QFile::WriteUser]  = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes[QFile::ExeUser  ]  = new QCheckBox( this ), Qt::AlignHCenter );
    #endif

    gridLayout->addWidget( new QLabel( "Group:", this ) );
    gridLayout->addWidget( checkboxes[QFile::ReadGroup  ] = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes[QFile::WriteGroup ] = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes[QFile::ExeGroup   ] = new QCheckBox( this ), Qt::AlignHCenter );

    gridLayout->addWidget( new QLabel( "Others:", this ) );
    gridLayout->addWidget( checkboxes[QFile::ReadOther  ] = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes[QFile::WriteOther ] = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes[QFile::ExeOther   ] = new QCheckBox( this ), Qt::AlignHCenter );

    // set checkboxes
    for( CheckBoxMap::iterator iter = checkboxes.begin(); iter != checkboxes.end(); ++iter )
    {
        iter.value()->setChecked( permissions & iter.key() );
        iter.value()->setEnabled( false );
    }


    gridLayout->setColumnStretch( 0, 0 );
    gridLayout->setColumnStretch( 1, 1 );
    gridLayout->setColumnStretch( 2, 1 );
    gridLayout->setColumnStretch( 3, 1 );
}
