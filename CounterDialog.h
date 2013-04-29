#ifndef CounterDialog_h
#define CounterDialog_h

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

#include "CustomDialog.h"
#include "CounterModel.h"

class TreeView;

//! displays Counter names and counts
class CounterDialog: public CustomDialog
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    // constructor
    CounterDialog( QWidget* );

    //! destructor
    virtual ~CounterDialog( void )
    {}

    public slots:

    //! update Counter list
    void update();

    private:

    //! model
    CounterModel model_;

    //! list
    TreeView* list_;

};

#endif
