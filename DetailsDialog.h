#ifndef DetailsDialog_h
#define DetailsDialog_h

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

#include "CustomDialog.h"

#include <QtGui/QLabel>
#include <QtGui/QWidget>

//! customized QDialog
class DetailsDialog: public CustomDialog
{

    Q_OBJECT

    public:

    //! constructor
    DetailsDialog( QWidget* parent,
        const unsigned int& flags = OkButton | CancelButton,
        Qt::WFlags wflags = 0 ):
        CustomDialog( parent, flags, wflags ),
        detailsLabel_( 0x0 ),
        detailsWidget_( 0x0 )
    {}

    //! destructor
    virtual ~DetailsDialog( void )
    {}

    //! set text details
    virtual void setDetails( const QString& );

    //! set details widget
    virtual void setDetails( QWidget* );

    public slots:

    //! show details
    void showDetails( void )
    { toggleDetails( true ); }

    //! hide details
    void hideDetails( void )
    { toggleDetails( false ); }

    //! toggle details
    void toggleDetails( bool );

    private:

    //! details label
    QLabel* detailsLabel_;

    //! details widget
    QWidget* detailsWidget_;

};

#endif
