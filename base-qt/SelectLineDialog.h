#ifndef SelectLineDialog_h
#define SelectLineDialog_h

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

#include "BaseDialog.h"
#include "Counter.h"
#include "base_qt_export.h"

#include <memory>

class LineEditor;
class SelectLineWidget;

//* find_text dialog for text editor widgets
class BASE_QT_EXPORT SelectLineDialog: public BaseDialog, private Base::Counter<SelectLineDialog>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    explicit SelectLineDialog( QWidget* parent = nullptr, Qt::WindowFlags WindowFlags = {} );

    //* destructor
    ~SelectLineDialog() override;

    //*@name accessors
    //@{

    //* retrieve editor
    LineEditor& editor() const;

    //@}

    //*@name modifiers
    //@{

    //* set select line widget
    void setSelectLineWidget( SelectLineWidget* );

    //@}

    Q_SIGNALS:

    //* emitted when pressing the Ok button
    void lineSelected( int );

    private:

    //* select line widget
    std::unique_ptr<SelectLineWidget> selectLineWidget_;

};
#endif
