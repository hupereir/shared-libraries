#ifndef PlacesWidget_p_h
#define PlacesWidget_p_h

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

#include "BaseFileInfo.h"
#include "BaseToolTipWidget.h"
#include "BrowsedLineEditor.h"
#include "Counter.h"
#include "CustomDialog.h"
#include "Functors.h"
#include "LineEditor.h"

#include <QAbstractButton>
#include <QCheckBox>
#include <QEvent>
#include <QList>
#include <QLabel>
#include <QPaintEvent>

class DragMonitor;

namespace Private
{

    //* local file info, needed to store flags
    class LocalFileInfo: public BaseFileInfo
    {

        public:

        enum Flag
        {
            ReadOnly = 1<<0,
            Hidden = 1<<1,
            Separator = 1<<2
        };

        Q_DECLARE_FLAGS( Flags, Flag );

        //* default constructor
        explicit LocalFileInfo()
        {}

        //* copy constructor
        explicit LocalFileInfo( const BaseFileInfo& other ):
            BaseFileInfo( other )
        {}

        //* constructor from DOM element
        explicit LocalFileInfo( const QDomElement& );

        //*@name accessors
        //@{

        //* dump to dom element
        QDomElement domElement( QDomDocument& ) const;

        //* flags
        Flags flags() const
        { return flags_; }

        //* has flag
        bool hasFlag( Flag flag ) const
        { return flags_&flag; }

        //@}

        //*@name modifiers
        //@{

        //* set flags
        void setFlags( Flags flags )
        { flags_ = flags; }

        //* set flag
        void setFlag( Flag flag, bool value )
        {
            if( value ) flags_ |= flag;
            else flags_ &= ~flag;
        }

        //@}


        //* read/write file info list to xml
        using List = QList<LocalFileInfo>;
        class Helper
        {

            public:

            static List list( const QDomElement& );
            static QDomElement domElement( const List&, QDomDocument& );

        };

        private:

        //* flags
        Flags flags_ = 0;

    };

    //* equal to operator
    inline bool operator == (const LocalFileInfo& first, const LocalFileInfo& second)
    {
        return ( operator == ( *static_cast<const BaseFileInfo*>(&first), *static_cast<const BaseFileInfo*>(&second) ) ) &&
            first.alias() == second.alias() &&
            first.flags() == second.flags();
    }

    //* places widget item
    class PlacesWidgetItem: public QAbstractButton
    {

        Q_OBJECT

        public:

        //* dragging
        static const QString MimeType;

        //* constructor
        explicit PlacesWidgetItem( QWidget* = nullptr );

        //*@name accessors
        //@{

        //* file info
        const BaseFileInfo& fileInfo() const
        { return fileInfo_; }

        //* true if is separator
        bool isSeparator() const
        { return hasFlag( LocalFileInfo::Separator ); }

        //* true if valid
        bool isValid() const
        { return valid_; }

        //* focus
        bool hasFocus() const
        { return hasFocus_; }

        //* size hint
        QSize sizeHint() const override
        { return minimumSize(); }

        //* flags
        LocalFileInfo::Flags flags() const
        { return flags_; }

        //* has flag
        bool hasFlag( LocalFileInfo::Flag flag ) const
        { return flags_&flag; }

        //* drag monitor
        DragMonitor& dragMonitor() const
        { return *dragMonitor_; }

        //@}

        //*@name modifiers
        //@{

        //* set is separator
        void setIsSeparator( bool value )
        { setFlag( LocalFileInfo::Separator, true ); updateMinimumSize(); update(); }

        //* icon
        void setIcon( const QIcon& icon )
        { QAbstractButton::setIcon( icon ); updateMinimumSize(); update(); }

        //* name
        void setText( const QString& text )
        { QAbstractButton::setText( text ); updateMinimumSize(); update(); }

        //* file info
        void setFileInfo( const BaseFileInfo& fileInfo )
        { fileInfo_ = fileInfo; }

        //* some styles require an item view passed to painting method to have proper selection rendered in items
        void setItemView( QWidget* widget )
        { itemView_ = widget; }

        //* set validity
        bool setIsValid( bool value )
        {
            if( valid_ == value ) return false;
            valid_ = value;
            return true;
        }

        //* set mouse over manualy
        void setMouseOver( bool value )
        { mouseOver_ = value; }

        //* update minimum width
        void updateMinimumSize();

        //* focus
        void setFocus( bool value )
        {
            if( hasFocus_ == value ) return;
            hasFocus_ = value;
            update();
        }

        //* set flags
        void setFlags( LocalFileInfo::Flags flags )
        { flags_ = flags; }

        //* set flag
        void setFlag( LocalFileInfo::Flag flag, bool value )
        {
            if( value ) flags_ |= flag;
            else flags_ &= ~flag;
        }

        //@}

        // needed to find items that have a given flag
        using HasFlagFTor = Base::Functor::Unary< PlacesWidgetItem, LocalFileInfo::Flags, &PlacesWidgetItem::flags, std::bit_and<LocalFileInfo::Flags>>;

        protected:

        //* event
        bool event( QEvent* ) override;

        //* paint event
        void paintEvent( QPaintEvent* ) override;

        private Q_SLOTS:

        //* start drag
        void _startDrag( QPoint );

        //* update configuration
        void _updateConfiguration();

        private:

        //* paint
        void _paint( QPainter* );

        //* some styles require an item view passed to painting method to have proper selection rendered in items
        QWidget* itemView_ = nullptr;

        //* drag monitor
        DragMonitor* dragMonitor_ = nullptr;

        //* file info
        BaseFileInfo fileInfo_;

        //* flags
        LocalFileInfo::Flags flags_ = 0;

        //* true if valid (true by default)
        bool valid_ = false;

        //* mouse over
        bool mouseOver_ = false;

        //* focus
        bool hasFocus_ = false;

    };

    //* edit item dialog
    class PlacesWidgetItemDialog: public CustomDialog
    {

        Q_OBJECT

        public:

        //* constructor
        explicit PlacesWidgetItemDialog( QWidget* = nullptr );

        //* accessors
        //@{

        //* name
        QString name() const
        { return nameEditor_->text(); }

        //* file
        QString file() const
        { return fileEditor_->editor().text(); }

        //* remote
        bool isRemote() const
        { return remoteCheckBox_->isChecked(); }

        //@}

        //*@name modifiers
        //@{

        //* name
        void setName( const QString& value )
        { nameEditor_->setText( value ); }

        //* file
        void setFile( const BaseFileInfo& value )
        {
            fileEditor_->setFile( value.file() );
            remoteCheckBox_->setChecked( value.isRemote() );
        }

        //@}

        private:

        //* name
        LineEditor* nameEditor_ = nullptr;

        //* file
        BrowsedLineEditor* fileEditor_ = nullptr;

        //* remote checkbox
        QCheckBox* remoteCheckBox_ = nullptr;

    };

    //* tooltip widget
    class PlacesToolTipWidget: public BaseToolTipWidget
    {

        Q_OBJECT

        public:

        //* constructor
        explicit PlacesToolTipWidget( QWidget* );

        //* set data
        void setFileInfo( const QString&, const BaseFileInfo&, const QIcon& = QIcon() );

        //* information mask
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

        protected:

        //* reload
        void _reload()
        { setFileInfo( name_, fileInfo_, icon_ ); }

        private Q_SLOTS:

        //* update configuration
        void _updateConfiguration();

        private:

        //* pixmap size
        int pixmapSize_ = 0;

        //* information mask
        Types mask_ = None;

        //* name
        QString name_;

        //* local icon copy
        QIcon icon_;

        //* local fileInfo copy
        BaseFileInfo fileInfo_;

        //* icon label
        QLabel* iconLabel_ = nullptr;

        //* file name label
        QLabel* nameLabel_ = nullptr;

        //* separator
        QFrame* separator_ = nullptr;

        //*@name items
        //@{
        GridLayoutItem* pathItem_ = nullptr;
        GridLayoutItem* lastModifiedItem_ = nullptr;
        GridLayoutItem* userItem_ = nullptr;
        GridLayoutItem* groupItem_ = nullptr;
        GridLayoutItem* permissionsItem_ = nullptr;
        //@}

    };

}

Q_DECLARE_OPERATORS_FOR_FLAGS( Private::LocalFileInfo::Flags )
Q_DECLARE_OPERATORS_FOR_FLAGS( Private::PlacesToolTipWidget::Types )

#endif
