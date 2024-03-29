#ifndef SpellItemDialog_h
#define SpellItemDialog_h

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

#include "Dialog.h"
#include "QOrderedSet.h"
#include "SpellItemModel.h"
#include "base_spellcheck_export.h"

#include <QStringList>

class TreeView;

namespace SpellCheck
{
    //* spell checker popup dialog
    class BASE_SPELLCHECK_EXPORT SpellItemDialog: public Dialog
    {

        //* Qt meta object declaration
        Q_OBJECT

        public:

        //* constructor
        explicit SpellItemDialog( QWidget* );

        //* set items
        void setItems( const QOrderedSet<QString>& );

        //* set disabled items
        void setDisabledItems( const QString& );

        //* disabled items
        QString disabledItems() const;

        private:

        //* select item
        void _selectItem( const QModelIndex& );

        //* model
        SpellItemModel model_;

        //* list
        TreeView* list_ = nullptr;

    };

}

#endif
