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
#include "BaseToolTipWidget.h"

#include <QIcon>
#include <QFrame>
#include <QLabel>

class ToolTipWidgetItem;

class BaseFileInfoToolTipWidget: public BaseToolTipWidget
{

    Q_OBJECT

    public:

    //! constructor
    BaseFileInfoToolTipWidget( QWidget* );

    //! destructo
    virtual ~BaseFileInfoToolTipWidget( void )
    {}

    //! set data
    void setFileInfo( const BaseFileInfo&, const QIcon& = QIcon() );

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

    Q_DECLARE_FLAGS(Types, Type)

    //! mask
    void setMask( Types value )
    {
        if( mask_ == value ) return;
        mask_ = value;
        _reload();
    }

    protected:

    //! reload
    virtual void _reload( void )
    { setFileInfo( fileInfo_, icon_ ); }

    private slots:

    //! update configuration
    void _updateConfiguration( void );

    private:

    //! pixmap size
    int pixmapSize_;

    //! information mask
    Types mask_;

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
    ToolTipWidgetItem* typeItem_;
    ToolTipWidgetItem* sizeItem_;
    ToolTipWidgetItem* lastModifiedItem_;
    ToolTipWidgetItem* userItem_;
    ToolTipWidgetItem* groupItem_;
    ToolTipWidgetItem* permissionsItem_;
    //@}

};

Q_DECLARE_OPERATORS_FOR_FLAGS( BaseFileInfoToolTipWidget::Types )

#endif
