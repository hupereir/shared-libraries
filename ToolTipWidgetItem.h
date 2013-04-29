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

#include <QLabel>
#include <QObject>

//_______________________________________________________
class ToolTipWidgetItem: public QObject, public Counter
{
    public:

    //! constructor
    ToolTipWidgetItem( QWidget* parent, GridLayout* layout ):
        QObject( parent ),
        Counter( "ToolTipWidgetItem" )
    {
        layout->addWidget( key_ = new QLabel( parent ) );
        layout->addWidget( value_ = new QLabel( parent ) );
    }

    //! destructor
    virtual ~ToolTipWidgetItem( void )
    {}

    //! show
    void show( void )
    {
        key_->show();
        value_->show();
    }

    //! hide
    void hide( void )
    {
        key_->hide();
        value_->hide();
    }

    //! set key
    void setKey( const QString& value )
    { key_->setText( value ); }

    //! set text
    void setText( const QString& value )
    {
        value_->setText( value );
        if( value.isEmpty() ) hide();
        else show();
    }

    private:

    QLabel* key_;
    QLabel* value_;

};
