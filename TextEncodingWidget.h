#ifndef TextEncodingWidget_h
#define TextEncodingWidget_h

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

#include "CustomComboBox.h"
#include "Counter.h"

#include <QWidget>

//! allows to change default and current file encoding
class TextEncodingWidget: public QWidget, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    TextEncodingWidget( QWidget* = 0 );

    //! destructor
    virtual ~TextEncodingWidget( void )
    {}

    Q_SIGNALS:

    //! default encoding changed
    void defaultEncodingChanged( const QString& );

    //! current encoding changed
    void currentEncodingChanged( const QString& );

    protected Q_SLOTS:

    //! update default encoding
    void _updateDefaultEncoding( const QString& );

};

#endif
