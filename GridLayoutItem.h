#ifndef GridLayoutItem_h
#define GridLayoutItem_h

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

class GridLayout;

//_______________________________________________________
class GridLayoutItem: public QObject, public Counter
{

    Q_OBJECT

    public:

    //! flags
    enum ItemFlag
    {
        None = 0,
        Elide = 1<<0,
        Bold = 1<<1,
        Selectable = 1<<2,
        All = Elide|Bold|Selectable
    };

    Q_DECLARE_FLAGS( ItemFlags, ItemFlag );

    //! constructor
    GridLayoutItem( QWidget*, GridLayout*, ItemFlags = None );

    //! destructor
    virtual ~GridLayoutItem( void )
    {}

    //!@name accessors
    //@{

    //! visibility
    bool isVisible( void ) const
    { return visible_; }

    //! key
    QString key( void ) const
    { return key_->text(); }

    //! value
    QString text( void ) const
    { return value_->text(); }

    //! value label
    QLabel& label( void ) const
    { return *value_; }

    //!@}

    //!@name modifiers
    //@{

    //! set key
    void setKey( const QString& );

    //! set text
    void setText( const QString& );

    //@}

    //!@name visibility
    //@{

    //! set visible
    void setVisible( bool value );

    //! show
    void show( void );

    //! hide
    void hide( void );

    //@}

    Q_SIGNALS:

    //! visibility changed
    void visibilityChanged( bool );

    private:

    ItemFlags flags_;
    QLabel* key_;
    QLabel* value_;

    bool visible_;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( GridLayoutItem::ItemFlags )

#endif
