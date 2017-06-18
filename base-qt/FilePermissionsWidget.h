#ifndef FilePermissionsWidget_h
#define FilePermissionsWidget_h

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

#include "Counter.h"

#include <QFile>
#include <QHash>
#include <QWidget>
#include <QCheckBox>

class FilePermissionsWidget: public QWidget, private Base::Counter<FilePermissionsWidget>
{

    Q_OBJECT

    public:

    //* constructor
    explicit FilePermissionsWidget( QWidget*, QFile::Permissions = QFile::Permissions() );

    //* set permissions
    void setPermissions( QFile::Permissions );

    private:

    //* checkbox map
    using CheckBoxMap = QHash< QFile::Permission, QCheckBox* >;

    //* checkbox map
    CheckBoxMap checkboxes_;



};

#endif
