#ifndef FileRecordToolTipWidget_h
#define FileRecordToolTipWidget_h

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

#include "base_qt_export.h"
#include "BaseToolTipWidget.h"
#include "FileRecord.h"

#include <QIcon>
#include <QFrame>
#include <QLabel>

class GridLayoutItem;

class BASE_QT_EXPORT FileRecordToolTipWidget: public BaseToolTipWidget
{

    Q_OBJECT

    public:

    //* constructor
    explicit FileRecordToolTipWidget( QWidget* );

    //* set data
    void setRecord( const FileRecord&, const QIcon& = QIcon() );

    //* information mask
    enum Type
    {
        None = 0,
        Size = 1<<0,
        Modified = 1<<1,
        User = 1<<2,
        Group = 1<<3,
        Permissions = 1<<4,
        Default = Size|Modified
    };

    Q_DECLARE_FLAGS(Types, Type)

    protected:

    //* reload
    void _reload()
    { setRecord( record_, icon_ ); }

    private:

    //* update configuration
    void _updateConfiguration();

    //* pixmap size
    int pixmapSize_ = 96;

    //* information mask
    int mask_ = Default;

    //* local icon copy
    QIcon icon_;

    //* local fileInfo copy
    FileRecord record_;

    //* icon label
    QLabel* iconLabel_ = nullptr;

    //* file name label
    QLabel* fileLabel_ = nullptr;

    //* separator
    QFrame* separator_ = nullptr;

    //*@name items
    //@{
    GridLayoutItem* pathItem_ = nullptr;
    GridLayoutItem* sizeItem_ = nullptr;
    GridLayoutItem* lastModifiedItem_ = nullptr;
    GridLayoutItem* userItem_ = nullptr;
    GridLayoutItem* groupItem_ = nullptr;
    GridLayoutItem* permissionsItem_ = nullptr;
    //@}

};

Q_DECLARE_OPERATORS_FOR_FLAGS( FileRecordToolTipWidget::Types )

#endif
