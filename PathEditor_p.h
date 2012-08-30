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

//! path item
class PathEditorItem: public QAbstractButton, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    PathEditorItem( QWidget* parent ):
        QAbstractButton( parent ),
        Counter( "PathEditorItem" ),
        isRoot_( false ),
        isLast_( false ),
        mouseOver_( false )
    {
        Debug::Throw( "PathEditorItem::PathEditorItem.\n" );
        setAttribute( Qt::WA_Hover );
    }

    //! destructor
    virtual ~PathEditorItem( void )
    {}

    //! set is last
    void setIsLast( bool value )
    {
        if( isLast_ == value ) return;
        isLast_ = value;
        _updateMinimumSize();
        update();
    }

    //! is last
    bool isLast( void ) const
    { return isLast_; }

    //! set path
    void setPath( const File& );

    //! path
    const File& path( void ) const
    { return path_; }

    //! size hint
    virtual QSize sizeHint( void ) const
    { return minimumSize() + QSize( 4*BorderWidth, 0 ); }

    typedef QList<PathEditorItem*> List;
    typedef QListIterator<PathEditorItem*> ListIterator;

    //! border width
    enum { BorderWidth = 2 };

    protected:

    //! event
    virtual bool event( QEvent* );

    //! paint event
    virtual void paintEvent( QPaintEvent* );

    //! update minimum width
    void _updateMinimumSize( void );

    //! arrow width
    int _arrowWidth( void ) const
    { return isLast_ ? 0:qMax( 4, fontMetrics().boundingRect(text()).height()/2 + BorderWidth ); }

    private:

    //! path
    File path_;

    //! true if is root
    bool isRoot_;

    //! true if last
    bool isLast_;

    //! true if hover
    bool mouseOver_;

};

//! path editor switch. Toggle path editor to combobox
class PathEditorSwitch: public QAbstractButton, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    PathEditorSwitch( QWidget* parent ):
        QAbstractButton( parent ),
        Counter( "PathEditorSwitch" ),
        mouseOver_( false )
    {
        Debug::Throw( "PathEditorItem::PathEditorItem.\n" );
        setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
        setAttribute( Qt::WA_Hover );
    }

    //! destructor
    virtual ~PathEditorSwitch( void )
    {}

    protected:

    //! event
    virtual bool event( QEvent* );

    //! paint event
    virtual void paintEvent( QPaintEvent* );

    private:

    //! true if hover
    bool mouseOver_;

};

#endif
