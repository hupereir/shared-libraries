#ifndef OptionDialog_h
#define OptionDialog_h

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
#include "OptionModel.h"
#include "XmlOptions.h"
#include "base_qt_export.h"

// forward declaration
class TreeView;

//* displays Counter names and counts
class BASE_QT_EXPORT OptionDialog: public Dialog
{

    Q_OBJECT

    public:

    //* constructor
    explicit OptionDialog( QWidget* );

    Q_SIGNALS:

    //* emitted when options are modified
    void configurationChanged();

    private:

    //* reload options from backup
    void _reload();

    //* option modified
    void _optionModified( const Options::Pair &);

    //* option modified
    void _specialOptionModified( const Options::Pair &);

    //* option model
    OptionModel model_;

    //* list
    TreeView* list_ = nullptr;

    //* pointer to original options set
    /**
    it is needed to keep track of the changes
    so that initial set is restored when pressing the cancel button
    */
    Options backupOptions_;

};

#endif
