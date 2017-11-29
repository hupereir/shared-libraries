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

namespace Private
{

    //* path editor button
    class PathEditorButton: public QAbstractButton
    {

        Q_OBJECT

        public:

        //* constructor
        explicit PathEditorButton( QWidget* );

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
        bool event( QEvent* ) override;

        //* item view
        QWidget* _itemView() const
        { return itemView_; }

        //* true if mouse over
        bool _mouseOver() const
        { return mouseOver_; }

        private:

        //* some styles require an item view passed to painting method to have proper selection rendered in items
        QWidget* itemView_ = nullptr;

        //* mouse over
        bool mouseOver_ = false;

    };

    //* path item
    class PathEditorItem: public PathEditorButton, private Base::Counter<PathEditorItem>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit PathEditorItem( QWidget* );

        //*@name accessors
        //@{

        //* is selectable
        bool isSelectable() const
        { return isSelectable_; }

        //* is last
        bool isLast() const
        { return isLast_; }

        //* path
        const File& path() const
        { return path_; }

        //* size hint
        QSize sizeHint() const override;

        //* drag monitor
        DragMonitor& dragMonitor() const
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
        void updateMinimumSize();

        //* shortcuts
        using List = QList<PathEditorItem*>;
        using ListIterator = QListIterator<PathEditorItem*>;

        protected:

        //* paint event
        void paintEvent( QPaintEvent* ) override;

        //* paint
        void _paint( QPainter* );

        //* arrow width
        int _arrowWidth() const
        { return isLast_ ? 0:qMax<int>( 4, fontMetrics().boundingRect(text()).height()/2 + BorderWidth ); }

        private Q_SLOTS:

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
    class PathEditorMenuButton: public PathEditorButton, private Base::Counter<PathEditorMenuButton>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit PathEditorMenuButton( QWidget* parent ):
            PathEditorButton( parent ),
            Counter( "PathEditorMenuButton" )
        {
            Debug::Throw( "PathEditorMenuButton::PathEditorMenuButton.\n" );
            setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed );
            updateMinimumSize();
        }

        // minimum size
        void updateMinimumSize();

        protected:

        //* paint event
        void paintEvent( QPaintEvent* ) override;

    };

    //* path editor switch. Toggle path editor to combobox
    class PathEditorSwitch: public PathEditorButton, private Base::Counter<PathEditorSwitch>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit PathEditorSwitch( QWidget* parent ):
            PathEditorButton( parent ),
            Counter( "PathEditorSwitch" )
        {
            Debug::Throw( "PathEditorItem::PathEditorItem.\n" );
            setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
            setCursor( Qt::IBeamCursor );
        }

        protected:

        //* paint event
        void paintEvent( QPaintEvent* ) override;

    };


}
#endif
