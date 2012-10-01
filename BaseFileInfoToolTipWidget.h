#ifndef BaseFileInfoToolTipWidget_h
#define BaseFileInfoToolTipWidget_h

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

#include <QtCore/QBasicTimer>
#include <QtCore/QTimerEvent>

#include <QtGui/QIcon>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QPaintEvent>
#include <QtGui/QWidget>

class GridLayout;
class Item;
class BaseFileInfoToolTipWidget: public QWidget, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    BaseFileInfoToolTipWidget( QWidget* );

    //! destructo
    virtual ~BaseFileInfoToolTipWidget( void )
    {}

    //! enable state
    void setEnabled( bool );

    //! set data
    void setFileInfo( const BaseFileInfo&, const QIcon& );

    //! adjust position
    void adjustPosition( const QRect& );

    //! mask
    void setPixmapSize( int value )
    {
        if( pixmapSize_ == value ) return;
        pixmapSize_ = value;
        _reload();
    }

    //! information mask
    enum Type
    {
        None = 0,
        Size = 1<<0,
        Modified = 1<<1,
        User = 1<<2,
        Group = 1<<3,
        Permissions = 1<<4,
        Default = Size|Modified
    };

    //! mask
    void setMask( unsigned int value )
    {
        if( mask_ == value ) return;
        mask_ = value;
        _reload();
    }


    public slots:

    //! hide
    virtual void hide( void )
    {
        timer_.stop();
        QWidget::hide();
    }

    //! show
    virtual void show( void )
    {
        timer_.stop();
        QWidget::show();
    }

    //! show delayed
    void showDelayed( unsigned int delay = 500 )
    {
        if( !enabled_ ) return;
        if( isVisible() ) hide();
        timer_.start( delay, this );
    }

    protected:

    //! paint
    virtual void paintEvent( QPaintEvent* );

    //! timer event
    virtual void timerEvent( QTimerEvent* );

    //! reload
    virtual void _reload( void )
    { setFileInfo( fileInfo_, icon_ ); }

    private slots:

    //! update configuration
    void _updateConfiguration( void );

    private:

    //! enable state
    bool enabled_;

    //! pixmap size
    int pixmapSize_;

    //! information mask
    unsigned int mask_;

    //! local icon copy
    QIcon icon_;

    //! local fileInfo copy
    BaseFileInfo fileInfo_;

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
    Item* userItem_;
    Item* groupItem_;
    Item* permissionsItem_;
    //@}

    //! timer
    QBasicTimer timer_;

};

#endif
