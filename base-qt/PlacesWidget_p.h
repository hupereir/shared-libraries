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
#include "LineEditor.h"

#include <QAbstractButton>
#include <QCheckBox>
#include <QEvent>
#include <QList>
#include <QLabel>
#include <QPaintEvent>

class DragMonitor;

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
    LocalFileInfo( void )
    {}

    //* copy constructor
    LocalFileInfo( const BaseFileInfo& other ):
        BaseFileInfo( other )
    {}

    //* constructor from DOM element
    LocalFileInfo( const QDomElement& );

    //* equal to operator
    bool operator == (const LocalFileInfo& other ) const
    {
        return
            (BaseFileInfo::operator == (other)) &&
            alias() == other.alias() &&
            flags_ == other.flags_;
    }

    //*@name accessors
    //@{

    //* dump to dom element
    virtual QDomElement domElement( QDomDocument& ) const;

    //* flags
    Flags flags( void ) const
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

    class List: public QList<LocalFileInfo>
    {
        public:

        //* constructor
        List( void )
        {}

        //* constructor from DOM element
        List( const QDomElement& );

        //* dump to dom element
        virtual QDomElement domElement( QDomDocument& ) const;

    };

    private:

    //* flags
    Flags flags_ = 0;


};

//* places widget item
class PlacesWidgetItem: public QAbstractButton
{

    Q_OBJECT

    public:

    //* dragging
    static const QString MimeType;

    //* constructor
    PlacesWidgetItem( QWidget* = nullptr );

    //*@name accessors
    //@{

    //* file info
    const BaseFileInfo& fileInfo( void ) const
    { return fileInfo_; }

    //* true if is separator
    bool isSeparator( void ) const
    { return hasFlag( LocalFileInfo::Separator ); }

    //* true if valid
    bool isValid( void ) const
    { return valid_; }

    //* focus
    bool hasFocus( void ) const
    { return hasFocus_; }

    //* size hint
    virtual QSize sizeHint( void ) const
    { return minimumSize(); }

    //* flags
    LocalFileInfo::Flags flags( void ) const
    { return flags_; }

    //* has flag
    bool hasFlag( LocalFileInfo::Flag flag ) const
    { return flags_&flag; }

    //* drag monitor
    DragMonitor& dragMonitor( void ) const
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
    void updateMinimumSize( void );

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

    protected:

    //* event
    virtual bool event( QEvent* );

    //* paint event
    virtual void paintEvent( QPaintEvent* );

    //* paint
    virtual void _paint( QPainter* );

    protected Q_SLOTS:

    //* start drag
    void _startDrag( QPoint );

    private Q_SLOTS:

    //* update configuration
    void _updateConfiguration( void );

    private:

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
    PlacesWidgetItemDialog( QWidget* = nullptr );

    //* accessors
    //@{

    //* name
    QString name( void ) const
    { return nameEditor_->text(); }

    //* file
    QString file( void ) const
    { return fileEditor_->editor().text(); }

    //* remote
    bool isRemote( void ) const
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
    PlacesToolTipWidget( QWidget* );

    //* set data
    void setFileInfo( const QString&, const BaseFileInfo&, const QIcon& = QIcon() );

    //* mask
    void setPixmapSize( int value )
    {
        if( pixmapSize_ == value ) return;
        pixmapSize_ = value;
        _reload();
    }

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

    //* mask
    void setMask( Types value )
    {
        if( mask_ == value ) return;
        mask_ = value;
        _reload();
    }

    protected:

    //* reload
    virtual void _reload( void )
    { setFileInfo( name_, fileInfo_, icon_ ); }

    private Q_SLOTS:

    //* update configuration
    void _updateConfiguration( void );

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

Q_DECLARE_OPERATORS_FOR_FLAGS( LocalFileInfo::Flags )
Q_DECLARE_OPERATORS_FOR_FLAGS( PlacesToolTipWidget::Types )

#endif
