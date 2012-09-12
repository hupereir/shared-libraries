#ifndef PathEditor_p_h
#define PathEditor_p_h

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
*
*******************************************************************************/

#include "Counter.h"
#include "Debug.h"
#include "File.h"

#include <QtCore/QEvent>
#include <QtCore/QList>

#include <QtGui/QAbstractButton>
#include <QtGui/QPaintEvent>

//! path editor button
class PathEditorButton: public QAbstractButton
{

    Q_OBJECT

    public:

    //! constructor
    PathEditorButton( QWidget* parent ):
        QAbstractButton( parent ),
        itemView_( 0x0 ),
        mouseOver_( false )
    {
        Debug::Throw( "PathEditorItem::PathEditorItem.\n" );
        setAttribute( Qt::WA_Hover );
        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        setMinimumHeight(parent->minimumHeight());
    }

    //! destructor
    virtual ~PathEditorButton( void )
    {}

    //! some styles require an item view passed to painting method to have proper selection rendered in items
    void setItemView( QWidget* widget )
    { itemView_ = widget; }

    //! set mouse over manualy
    void setMouseOver( bool value )
    { mouseOver_ = value; }

    //! border width
    // enum { BorderWidth = 2 };

    static const qreal BorderWidth;

    protected:

    //! event
    virtual bool event( QEvent* );

    //! item view
    QWidget* _itemView( void ) const
    { return itemView_; }

    //! true if mouse over
    bool _mouseOver( void ) const
    { return mouseOver_; }

    private:

    //! some styles require an item view passed to painting method to have proper selection rendered in items
    QWidget* itemView_;

    //! mouse over
    bool mouseOver_;

};

//! path item
class PathEditorItem: public PathEditorButton, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    PathEditorItem( QWidget* parent ):
        PathEditorButton( parent ),
        Counter( "PathEditorItem" ),
        isSelectable_( true ),
        isLast_( false )
    { Debug::Throw( "PathEditorItem::PathEditorItem.\n" ); }

    //! destructor
    virtual ~PathEditorItem( void )
    {}

    //!@name accessors
    //@{

    //! is selectable
    bool isSelectable( void ) const
    { return isSelectable_; }

    //! is last
    bool isLast( void ) const
    { return isLast_; }

    //! path
    const File& path( void ) const
    { return path_; }

    //! size hint
    virtual QSize sizeHint( void ) const
    { return minimumSize() + QSize( 4*BorderWidth, 0 ); }

    //@}

    //!@name modifiers#ifndef PathEditor_p_h
    //@{

    //! set is selectable
    void setIsSelectable( bool value )
    {
        if( isSelectable_ == value ) return;
        isSelectable_ = value;
        update();
    }

    //! set is last
    void setIsLast( bool value )
    {
        if( isLast_ == value ) return;
        isLast_ = value;
        updateMinimumSize();
        update();
    }

    //! set path
    void setPath( const File&, const QString& = QString() );

    //@}

    //! update minimum width
    void updateMinimumSize( void );

    //! shortcuts
    typedef QList<PathEditorItem*> List;
    typedef QListIterator<PathEditorItem*> ListIterator;

    protected:

    //! paint event
    virtual void paintEvent( QPaintEvent* );

    //! arrow width
    int _arrowWidth( void ) const
    { return isLast_ ? 0:qMax<int>( 4, fontMetrics().boundingRect(text()).height()/2 + BorderWidth ); }

    private:

    //! path
    File path_;

    //! true if selectable
    bool isSelectable_;

    //! true if last
    bool isLast_;

};

//! path editor menu button
class PathEditorMenuButton: public PathEditorButton, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    PathEditorMenuButton( QWidget* parent ):
        PathEditorButton( parent ),
        Counter( "PathEditorMenuButton" )
    {
        Debug::Throw( "PathEditorMenuButton::PathEditorMenuButton.\n" );
        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        updateMinimumSize();
    }

    //! destructor
    virtual ~PathEditorMenuButton( void )
    {}

    // minimum size
    void updateMinimumSize( void );

    protected:

    //! paint event
    virtual void paintEvent( QPaintEvent* );

};

//! path editor switch. Toggle path editor to combobox
class PathEditorSwitch: public PathEditorButton, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    PathEditorSwitch( QWidget* parent ):
        PathEditorButton( parent ),
        Counter( "PathEditorSwitch" )
    {
        Debug::Throw( "PathEditorItem::PathEditorItem.\n" );
        setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
        setCursor( Qt::IBeamCursor );
    }

    //! destructor
    virtual ~PathEditorSwitch( void )
    {}

    protected:

    //! paint event
    virtual void paintEvent( QPaintEvent* );

};

#endif
