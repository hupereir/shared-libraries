#ifndef PathEditor_p_h
#define PathEditor_p_h

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
#include "Debug.h"
#include "File.h"

#include <QEvent>
#include <QList>

#include <QAbstractButton>
#include <QPaintEvent>
#include <QPainter>

class DragMonitor;

//* path editor button
class PathEditorButton: public QAbstractButton
{

    Q_OBJECT

    public:

    //* constructor
    PathEditorButton( QWidget* parent ):
        QAbstractButton( parent )
    {
        Debug::Throw( "PathEditorItem::PathEditorItem.\n" );
        setAttribute( Qt::WA_Hover );
        setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed );
        setMinimumHeight(parent->minimumHeight());
    }

    //* some styles require an item view passed to painting method to have proper selection rendered in items
    void setItemView( QWidget* widget )
    { itemView_ = widget; }

    //* set mouse over manualy
    void setMouseOver( bool value )
    { mouseOver_ = value; }

    //* border width
    static const qreal BorderWidth;

    protected:

    //* event
    virtual bool event( QEvent* );

    //* item view
    QWidget* _itemView( void ) const
    { return itemView_; }

    //* true if mouse over
    bool _mouseOver( void ) const
    { return mouseOver_; }

    private:

    //* some styles require an item view passed to painting method to have proper selection rendered in items
    QWidget* itemView_ = nullptr;

    //* mouse over
    bool mouseOver_ = false;

};

//* path item
class PathEditorItem: public PathEditorButton, public Counter
{

    Q_OBJECT

    public:

    //* constructor
    PathEditorItem( QWidget* );

    //*@name accessors
    //@{

    //* is selectable
    bool isSelectable( void ) const
    { return isSelectable_; }

    //* is last
    bool isLast( void ) const
    { return isLast_; }

    //* path
    const File& path( void ) const
    { return path_; }

    //* size hint
    virtual QSize sizeHint( void ) const
    { return minimumSize() + QSize( 4*BorderWidth, 0 ); }

    //* drag monitor
    DragMonitor& dragMonitor( void ) const
    { return *dragMonitor_; }

    //@}

    //*@name modifiers
    //@{

    //* set prefix
    void setPrefix( const QString& value )
    { prefix_ = value; }

    //* set local flag
    void setIsLocal( bool value )
    { isLocal_ = value; }

    //* set selectable flag
    void setIsSelectable( bool value )
    {
        if( isSelectable_ == value ) return;
        isSelectable_ = value;
        update();
    }

    //* set is last
    void setIsLast( bool value )
    {
        if( isLast_ == value ) return;
        isLast_ = value;
        updateMinimumSize();
        update();
    }

    //* set path
    void setPath( const File&, const QString& = QString() );

    //@}

    //* update minimum width
    void updateMinimumSize( void );

    //* shortcuts
    using List = QList<PathEditorItem*>;
    using ListIterator = QListIterator<PathEditorItem*>;

    protected:

    //* paint event
    virtual void paintEvent( QPaintEvent* );

    //* paint
    void _paint( QPainter* );

    //* arrow width
    int _arrowWidth( void ) const
    { return isLast_ ? 0:qMax<int>( 4, fontMetrics().boundingRect(text()).height()/2 + BorderWidth ); }

    protected Q_SLOTS:

    //* start drag
    void _startDrag( QPoint );

    private:

    //* path
    File path_;

    //* prefix
    QString prefix_;

    //* true if path is in local file system
    bool isLocal_ = true;

    //* true if selectable
    bool isSelectable_ = true;

    //* true if last
    bool isLast_ = false;

    //* drag monitor
    DragMonitor* dragMonitor_ = nullptr;

};

//* path editor menu button
class PathEditorMenuButton: public PathEditorButton, public Counter
{

    Q_OBJECT

    public:

    //* constructor
    PathEditorMenuButton( QWidget* parent ):
        PathEditorButton( parent ),
        Counter( "PathEditorMenuButton" )
    {
        Debug::Throw( "PathEditorMenuButton::PathEditorMenuButton.\n" );
        setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed );
        updateMinimumSize();
    }

    // minimum size
    void updateMinimumSize( void );

    protected:

    //* paint event
    virtual void paintEvent( QPaintEvent* );

};

//* path editor switch. Toggle path editor to combobox
class PathEditorSwitch: public PathEditorButton, public Counter
{

    Q_OBJECT

    public:

    //* constructor
    PathEditorSwitch( QWidget* parent ):
        PathEditorButton( parent ),
        Counter( "PathEditorSwitch" )
    {
        Debug::Throw( "PathEditorItem::PathEditorItem.\n" );
        setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
        setCursor( Qt::IBeamCursor );
    }

    protected:

    //* paint event
    virtual void paintEvent( QPaintEvent* );

};

#endif
