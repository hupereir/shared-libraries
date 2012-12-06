#ifndef FavoritesWidget_p_h
#define FavoritesWidget_p_h

// $Id$

/*******************************************************************************
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

#include <QtCore/QEvent>
#include <QtCore/QList>

#include <QtGui/QAbstractButton>
#include <QtGui/QPaintEvent>

//! favorites widget item
class FavoritesWidgetItem: public QAbstractButton
{

    Q_OBJECT

    public:

    //! constructor
    FavoritesWidgetItem( QWidget* parent ):
        QAbstractButton( parent ),
        itemView_( 0x0 ),
        mouseOver_( false )
    {
        Debug::Throw( "PathEditorItem::PathEditorItem.\n" );
        setAttribute( Qt::WA_Hover );
    }

    //! destructor
    virtual ~FavoritesWidgetItem( void )
    {}

    //!@name accessors
    //@{

    //! file info
    const BaseFileInfo& fileInfo( void ) const
    { return fileInfo_; }

    //! size hint
    virtual QSize sizeHint( void ) const
    { return minimumSize(); }

    //@}

    //!@name modifiers
    //@{

    //! icon
    void setIcon( const QIcon& icon )
    { QAbstractButton::setIcon( icon ); updateMinimumSize(); update(); }

    //! name
    void setText( const QString& text )
    { QAbstractButton::setText( text ); updateMinimumSize(); update(); }

    //! file info
    void setFileInfo( const BaseFileInfo& fileInfo )
    { fileInfo_ = fileInfo; }

    //! some styles require an item view passed to painting method to have proper selection rendered in items
    void setItemView( QWidget* widget )
    { itemView_ = widget; }

    //! set mouse over manualy
    void setMouseOver( bool value )
    { mouseOver_ = value; }

    //! update minimum width
    void updateMinimumSize( void );

    //@}

    //! border width
    static const qreal BorderWidth;

    protected:

    //! event
    virtual bool event( QEvent* );

    //! paint event
    virtual void paintEvent( QPaintEvent* );

    private:

    //! some styles require an item view passed to painting method to have proper selection rendered in items
    QWidget* itemView_;

    //! file info
    BaseFileInfo fileInfo_;

    //! mouse over
    bool mouseOver_;

};

#endif
