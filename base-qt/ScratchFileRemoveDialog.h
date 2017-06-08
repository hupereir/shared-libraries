#ifndef ScratchFileRemoveDialog_h
#define ScratchFileRemoveDialog_h

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

#include "CustomDialog.h"
#include "FileRecord.h"
#include "FileRecordModel.h"

#include <QLayout>
#include <QPushButton>

class TreeView;

//* QDialog used to commit selected files
class ScratchFileRemoveDialog: public CustomDialog
{

    //* Qt meta object
    Q_OBJECT

    public:

    //* constructor
    explicit ScratchFileRemoveDialog( QWidget*, const FileRecordModel::List& files = FileRecordModel::List() );

    // return selected files
    FileRecordModel::List selectedFiles( void ) const;

    protected Q_SLOTS:

    //* update button states
    virtual void _updateActions( void );

    //* context menu
    virtual void _updateContextMenu( const QPoint& );

    private:

    //* install actions
    void _installActions( void );

    //* list of files
    TreeView* list_ = nullptr;

    //* model
    FileRecordModel model_;

    //*@name actions
    //@{
    QAction* selectAllAction_ = nullptr;
    QAction* clearSelectionAction_ = nullptr;
    //@}

};

#endif
