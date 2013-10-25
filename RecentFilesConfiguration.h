#ifndef _RecentFilesConfiguration_h_
#define _RecentFilesConfiguration_h_

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

#include "Counter.h"
#include "FileRecordModel.h"
#include "OptionWidgetList.h"

#include <QAction>
#include <QPushButton>
#include <QWidget>

class FileList;
class TreeView;

//! configure files in recent files menu
class RecentFilesConfiguration: public QWidget, public OptionWidgetList, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    RecentFilesConfiguration( QWidget*, FileList& );

    //! read options
    virtual void read( void );

    //! read options
    virtual void write( void ) const;

    private Q_SLOTS:

    //! update actions
    void _updateButtons( void );

    //! clean
    void _clean( void );

    //! remove
    void _remove( void );

    //! reload
    void _reload( void );

    private:

    //! file list
    FileList* recentFiles_;

    //! file record model
    FileRecordModel model_;

    //! file record list
    TreeView* list_;

    //! clean
    QPushButton *cleanButton_;

    //! remove
    QPushButton *removeButton_;

    //! reload
    QPushButton *reloadButton_;

    //!@name actions
    //@{

    //! clean action
    QAction* cleanAction_;

    //! remove action
    QAction* removeAction_;

    //! remove action
    QAction* reloadAction_;

    //@}

};

#endif
