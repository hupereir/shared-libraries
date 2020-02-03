#ifndef DetailsDialog_h
#define DetailsDialog_h

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

#include <QLabel>
#include <QWidget>

#include <memory>

//* customized QDialog
class DetailsDialog: public CustomDialog
{

    Q_OBJECT

    public:

    //* constructor
    explicit DetailsDialog( QWidget* parent,
        CustomDialog::Flags flags = CustomDialog::Flags( OkButton | CancelButton ),
        Qt::WindowFlags WindowFlags = 0 ):
        CustomDialog( parent, flags, WindowFlags )
    {}

    //* set text details
    void setDetails( const QString& );

    //* set details widget
    void setDetails( QWidget* );

    //* show details
    void showDetails()
    { toggleDetails( true ); }

    //* hide details
    void hideDetails()
    { toggleDetails( false ); }

    //* toggle details
    void toggleDetails( bool );

    private:

    //* details label
    QLabel* detailsLabel_ = nullptr;

    //* details widget
    std::unique_ptr<QWidget> detailsWidget_;

};

#endif
