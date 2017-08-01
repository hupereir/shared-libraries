#ifndef HelpDialog_h
#define HelpDialog_h

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
#include "CustomDialog.h"
#include "HelpModel.h"

#include <QPushButton>
#include <QStackedWidget>

class TextEditor;
class TreeView;

namespace Base
{

    //* reference manual display dialog
    class HelpDialog:public CustomDialog
    {

        Q_OBJECT

        public:

        //* constructor
        explicit HelpDialog( QWidget* = nullptr );

        //* clear items
        void clear()
        { model_.clear(); }

        //* set items
        void setItems( const HelpItem::List& items );

        private Q_SLOTS:

        //* display selected help text
        void _display( const QModelIndex&, const QModelIndex& );

        //* update configuration
        void _updateConfiguration();

        private:

        //* model
        HelpModel model_;

        //* list of help items
        TreeView *list_ = nullptr;

        //* html edition frame
        QWidget* htmlFrame_ = nullptr;

        //* read-only text editor
        TextEditor *htmlEditor_ = nullptr;

    };

};

#endif
