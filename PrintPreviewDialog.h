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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
****************************************************************************/

#include "BaseDialog.h"
#include "BasePrintHelper.h"

#include <QtGui/QPrintPreviewWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>
#include <QtGui/QLabel>
#include <QtGui/QIntValidator>

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

    public slots:

    //! show/hide menu
    void showMenu( void );

    //! show/hide menu
    void hideMenu( void );

    protected slots:

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
