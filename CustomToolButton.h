#ifndef CustomToolButton_h
#define CustomToolButton_h

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

#include "Counter.h"
#include "CustomPixmap.h"
#include "Debug.h"

#include <QLabel>
#include <QToolButton>

/*!
\class CustomToolButton
\brief customized tool button to display tooltip in a dedicated label
*/
class CustomToolButton: public QToolButton, public Counter
{

    Q_OBJECT

    public:

    //! default creator
    CustomToolButton( QWidget* parent = 0 );

    //! destructor
    virtual ~CustomToolButton( void )
    { Debug::Throw( "CustomToolButton::~CustomToolButton.\n" ); }

    //! update configuration from options
    void setUpdateFromOptions( const bool& value )
    { updateFromOptions_ = value; }

    //! rotation
    enum Rotation
    {
        NONE,
        CLOCKWISE,
        COUNTERCLOCKWISE
    };

    //! rotation
    bool rotate( const Rotation& );

    //! size hint
    virtual QSize sizeHint( void ) const;

    protected:

    //! painting
    virtual void paintEvent( QPaintEvent* );

    private Q_SLOTS:

    //! tool button configuration
    void _updateConfiguration( void );

    private:

    //! rotation
    const Rotation& _rotation( void ) const
    { return rotation_; }

    //! update configuration from options
    const bool& _updateFromOptions( void ) const
    { return updateFromOptions_; }

    //! update configuration from options
    bool updateFromOptions_;

    //! rotation
    Rotation rotation_;

};

#endif
