#ifndef CustomToolButton_h
#define CustomToolButton_h

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

#include "base_qt_export.h"
#include "Counter.h"
#include "CustomPixmap.h"
#include "Debug.h"

#include <QLabel>
#include <QToolButton>

class BASE_QT_EXPORT CustomToolButton: public QToolButton, private Base::Counter<CustomToolButton>
{

    Q_OBJECT

    public:

    //* default constructor
    explicit CustomToolButton( QWidget* = nullptr );

    //* update configuration from options
    void setUpdateFromOptions( bool value )
    { updateFromOptions_ = value; }

    //* rotation
    bool rotate( const CustomPixmap::Rotation& );

    //* size hint
    QSize sizeHint() const override;

    //* size hint
    QSize minimumSizeHint() const override
    { return sizeHint(); }

    protected:

    //* painting
    void paintEvent( QPaintEvent* ) override;

    private:

    //* tool button configuration
    void _updateConfiguration();

    //* update configuration from options
    bool updateFromOptions_ = true;

    //* rotation
    CustomPixmap::Rotation rotation_ = CustomPixmap::Rotation::None;

};

#endif
