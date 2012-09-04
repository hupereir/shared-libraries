#ifndef ToolTipWidget_h
#define ToolTipWidget_h

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

#include "BaseFileInfo.h"
#include "Counter.h"

#include <QtGui/QIcon>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QPaintEvent>
#include <QtGui/QWidget>

class GridLayout;

class ToolTipWidget: public QWidget, public Counter
{

    public:

    //! constructor
    ToolTipWidget( QWidget* );

    //! destructo
    virtual ~ToolTipWidget( void )
    {}

    //! set data
    void setBaseFileInfo( const BaseFileInfo&, const QIcon& );

    class Item: public QObject, public Counter
    {
        public:

        //! constructor
        Item( QWidget*, GridLayout* );

        //! destructor
        virtual ~Item( void )
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

    protected:

    //! paint
    virtual void paintEvent( QPaintEvent* event );

    private:

    //! pixmap size
    int pixmapSize_;

    //! icon label
    QLabel* iconLabel_;

    //! file name label
    QLabel* fileLabel_;

    //! separator
    QFrame* separator_;

    //!@name items
    //@{

    Item* typeItem_;
    Item* sizeItem_;
    Item* lastModifiedItem_;

    //@}

};

#endif
