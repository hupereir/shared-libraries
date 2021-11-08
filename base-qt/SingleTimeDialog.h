#ifndef SingleTimeDialog_h
#define SingleTimeDialog_h

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

#include "InformationDialog.h"
#include "base_qt_export.h"

#include <QCheckBox>

//* simple information dialog
class BASE_QT_EXPORT SingleTimeDialog: public InformationDialog
{

    Q_OBJECT

    public:

    //* constructor
    explicit SingleTimeDialog( QWidget*, const QString& = QString() );

    //* true if should not be shown again
    bool dontShowAgain() const
    { return singleTimeCheckBox_->isChecked(); }

    private:

    //* label
    QCheckBox* singleTimeCheckBox_ = nullptr;

};

#endif
