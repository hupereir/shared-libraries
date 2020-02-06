#ifndef RecentFilesConfiguration_h
#define RecentFilesConfiguration_h

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
#include "Counter.h"
#include "FileRecordModel.h"
#include "OptionWidgetList.h"

#include <QAction>
#include <QPushButton>
#include <QWidget>

class FileList;
class TreeView;

//* configure files in recent files menu
class BASE_QT_EXPORT RecentFilesConfiguration: public QWidget, public OptionWidgetList<RecentFilesConfiguration>, private Base::Counter<RecentFilesConfiguration>
{

    Q_OBJECT

    public:

    //* constructor
    explicit RecentFilesConfiguration( QWidget*, FileList& );

    // parent class implementation
    using OptionWidgetList::read;
    using OptionWidgetList::write;

    //* read files
    void read();

    // write files
    void write();

    //* reload
    void reload();

    Q_SIGNALS:

    //* modified
    void modified();

    private:

    //* update actions
    void _updateButtons();

    //* clean
    void _clean();

    //* remove
    void _remove();

    //* file list
    FileList* recentFiles_ = nullptr;

    //* file record model
    FileRecordModel model_;

    //* file record list
    TreeView* list_ = nullptr;

    //* clean
    QPushButton *cleanButton_ = nullptr;

    //* remove
    QPushButton *removeButton_ = nullptr;

    //* reload
    QPushButton *reloadButton_ = nullptr;

    //*@name actions
    //@{

    //* clean action
    QAction* cleanAction_ = nullptr;

    //* remove action
    QAction* removeAction_ = nullptr;

    //* remove action
    QAction* reloadAction_ = nullptr;

    //@}

};

#endif
