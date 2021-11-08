#ifndef PrintPreviewDialog_h
#define PrintPreviewDialog_h
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

#include "BasePrintHelper.h"
#include "Dialog.h"
#include "base_qt_export.h"

#include <QPrintPreviewWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QLabel>
#include <QIntValidator>

namespace Private
{
    class OptionMenu;
    class NavigationWidget;
}

class BASE_QT_EXPORT PrintPreviewDialog: public Dialog
{

    Q_OBJECT

    public:

    //* constructor
    explicit PrintPreviewDialog( QWidget* = nullptr, Dialog::Flags = {} );

    //* set print helper
    void setHelper( BasePrintHelper* );

    //* return preview widget
    QPrintPreviewWidget& previewWidget() const
    { return *previewWidget_; }

    //* show/hide menu
    void showMenu();

    //* show/hide menu
    void hideMenu();

    private:

    //* update page
    void _updatePage();

    //* option widget
    Private::OptionMenu* optionMenu_ = nullptr;

    //* navigation widget
    Private::NavigationWidget* navigationWidget_ = nullptr;

    //* preview widget
    QPrintPreviewWidget* previewWidget_ = nullptr;

};

#endif
