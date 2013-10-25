#ifndef PrintPreviewDialog_h
#define PrintPreviewDialog_h

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

#include "BaseDialog.h"
#include "BasePrintHelper.h"

#include <QPrintPreviewWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QLabel>
#include <QIntValidator>

namespace PRINT
{
    class OptionMenu;
    class NavigationWidget;
}

class PrintPreviewDialog: public BaseDialog
{

    Q_OBJECT

    public:

    //! constructor
    PrintPreviewDialog( QWidget* = 0 );

    //! destructor
    virtual ~PrintPreviewDialog( void )
    {}

    //! set print helper
    void setHelper( QObject* );

    //! return preview widget
    QPrintPreviewWidget& previewWidget( void ) const
    { return *previewWidget_; }

    public Q_SLOTS:

    //! show/hide menu
    void showMenu( void );

    //! show/hide menu
    void hideMenu( void );

    protected Q_SLOTS:

    //! update page
    void _updatePage( void );

    private:

    //! option widget
    PRINT::OptionMenu* optionMenu_;

    //! navigation widget
    PRINT::NavigationWidget* navigationWidget_;

    //! preview widget
    QPrintPreviewWidget* previewWidget_;

};

#endif
