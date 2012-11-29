#ifndef WarningDialog_h
#define WarningDialog_h

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
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*******************************************************************************/

#include "DetailsDialog.h"

#include <QtCore/QString>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

//! simple information dialog
class WarningDialog: public DetailsDialog
{

    public:

    //! constructor
    WarningDialog( QWidget* parent, const QString& = QString() );

    //! destructor
    virtual ~WarningDialog( void )
    {}

    //! set text
    virtual void setText( const QString& text ) const
    { _label().setText( text ); }

    protected:

    //! label
    QLabel& _label( void ) const
    { return *label_; }

    private:

    //! label
    QLabel* label_;

};

#endif
