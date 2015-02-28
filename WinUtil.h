#ifndef WinUtil_h
#define WinUtil_h

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

#include "Margins.h"

#include <QWidget>
#include <QPixmap>

class WinUtilPrivate;

class WinUtil
{

    public:

    //! constructor
    WinUtil( QWidget* );

    //! destructor
    virtual ~WinUtil( void );

    //! update layered widget using pixmap
    void update( const QPixmap&, double = 1.0 ) const;

    //! enable blur behind
    void enableBlurBehind( const Base::Margins& = Base::Margins() );

    //! toggle hide from taskbar for Windows
    bool toggleHideFromTaskBar( bool ) const;

    //! set flag
    void setFlag( long int, bool value = true ) const;

    //! has flag
    bool hasFlag( long int ) const;

    protected:

    // returns true if widget is of type layered window
    bool _isLayered() const;

    //! set widget a layered window
    void _setLayered() const;

    private:

    //! private
    WinUtilPrivate* private_;

    //! target widget
    QWidget *target_;

};

#endif
