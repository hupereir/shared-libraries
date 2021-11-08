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

#include "Debug.h"
#include "FilePermissionsWidget.h"
#include "GridLayout.h"
#include "QtUtil.h"

#include <QHash>
#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QHash>

//_____________________________________________________________________
FilePermissionsWidget::FilePermissionsWidget( QWidget* parent, QFile::Permissions permissions):
    QWidget( parent ),
    Counter( QStringLiteral("FilePermissionsWidget") )
{

    Debug::Throw( QStringLiteral("FilePermissionsWidget::FilePermissionsWidget\n") );

    GridLayout* gridLayout = new GridLayout;
    QtUtil::setMargin(gridLayout, 0);
    gridLayout->setMaxCount( 4 );
    gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
    setLayout( gridLayout );

    gridLayout->addWidget( new QLabel( tr( "Permissions: " ), this ) );
    gridLayout->addWidget( new QLabel( tr( "Read" ), this ), Qt::AlignHCenter );
    gridLayout->addWidget( new QLabel( tr( "Write" ), this ), Qt::AlignHCenter );
    gridLayout->addWidget( new QLabel( tr( "Execute" ), this ), Qt::AlignHCenter );

    gridLayout->addWidget( new QLabel( tr( "Owner:" ), this ) );
    gridLayout->addWidget( checkboxes_[QFile::ReadOwner]  = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes_[QFile::WriteOwner]  = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes_[QFile::ExeOwner]  = new QCheckBox( this ), Qt::AlignHCenter );

    // on unix, right now, Qt does not return the current user permissions. Disable them from the dialog
    #if !defined(Q_OS_UNIX)
    gridLayout->addWidget( new QLabel( tr( "User:" ), this ) );
    gridLayout->addWidget( checkboxes_[QFile::ReadUser]  = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes_[QFile::WriteUser]  = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes_[QFile::ExeUser]  = new QCheckBox( this ), Qt::AlignHCenter );
    #endif

    gridLayout->addWidget( new QLabel( tr( "Group:" ), this ) );
    gridLayout->addWidget( checkboxes_[QFile::ReadGroup] = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes_[QFile::WriteGroup] = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes_[QFile::ExeGroup] = new QCheckBox( this ), Qt::AlignHCenter );

    gridLayout->addWidget( new QLabel( tr( "Others:" ), this ) );
    gridLayout->addWidget( checkboxes_[QFile::ReadOther] = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes_[QFile::WriteOther] = new QCheckBox( this ), Qt::AlignHCenter );
    gridLayout->addWidget( checkboxes_[QFile::ExeOther] = new QCheckBox( this ), Qt::AlignHCenter );

    setPermissions( permissions );

    gridLayout->setColumnStretch( 0, 0 );
    gridLayout->setColumnStretch( 1, 1 );
    gridLayout->setColumnStretch( 2, 1 );
    gridLayout->setColumnStretch( 3, 1 );

}

//_____________________________________________________________________
void FilePermissionsWidget::setPermissions( QFile::Permissions permissions )
{

    Debug::Throw( QStringLiteral("FilePermissionsWidget::setPermissions\n") );

    // set checkboxes
    for( auto&& iter = checkboxes_.begin(); iter != checkboxes_.end(); ++iter )
    {
        iter.value()->setChecked( permissions & iter.key() );
        iter.value()->setEnabled( false );
    }

}
