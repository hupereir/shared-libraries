#ifndef WinUtil_h
#define WinUtil_h

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
*
*******************************************************************************/

#include <QtGui/QWidget>
#include <QtGui/QPixmap>

class WinUtil
{

    public:

    //! constructor
    WinUtil( QWidget* target ):
        target_( target )
    { Q_CHECK_PTR( target_ ); }

    //! update layered widget using pixmap
    void update( QPixmap&, double ) const;

    //! toggle hide from taskbar for Windows
    bool toggleHideFromTaskbar( bool ) const;

    //! set flag
    void setFlag( long int, bool value = true ) const;

    //! has flag
    bool hasFlag( long int ) const;

    private:

    // target widget
    QWidget& _target( void ) const
    { return *target_; }

    // returns true if widget is of type layered window
    bool _isLayered() const;

    //! set widget a layered window
    void _setLayered() const;

    //! target widget
    QWidget *target_;

};

#endif
