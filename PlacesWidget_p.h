#ifndef PlacesWidget_p_h
#define PlacesWidget_p_h

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
#include "BrowsedLineEditor.h"
#include "Counter.h"
#include "CustomDialog.h"
#include "AnimatedLineEditor.h"

#include <QtCore/QEvent>
#include <QtCore/QList>

#include <QtGui/QAbstractButton>
#include <QtGui/QCheckBox>
#include <QtGui/QPaintEvent>

//! places widget item
class PlacesWidgetItem: public QAbstractButton
{

    Q_OBJECT

    public:

    //! dragging
    static const QString MimeType;

    //! constructor
    PlacesWidgetItem( QWidget* = 0x0 );

    //! destructor
    virtual ~PlacesWidgetItem( void )
    {}

    //!@name accessors
    //@{

    //! file info
    const BaseFileInfo& fileInfo( void ) const
    { return fileInfo_; }

    //! focus
    bool hasFocus( void ) const
    { return hasFocus_; }

    //! drag in progress
    bool isDragged( void ) const
    { return dragInProgress_; }

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

    //! focus
    void setFocus( bool value )
    {
        if( hasFocus_ == value ) return;
        hasFocus_ = value;
        update();
    }

    //@}

    //! border width
    static const qreal BorderWidth;

    protected:

    //! event
    virtual bool event( QEvent* );

    //! mouse press
    virtual void mousePressEvent( QMouseEvent* );

    //! mouse press
    virtual void mouseMoveEvent( QMouseEvent* );

    //! mouse press
    virtual void mouseReleaseEvent( QMouseEvent* );

    //! paint event
    virtual void paintEvent( QPaintEvent* );

    //! paint
    virtual void _paint( QPainter* );

    private slots:

    //! update configuration
    void _updateConfiguration( void );

    private:

    //! some styles require an item view passed to painting method to have proper selection rendered in items
    QWidget* itemView_;

    //! file info
    BaseFileInfo fileInfo_;

    //! mouse over
    bool mouseOver_;

    //! focus
    bool hasFocus_;

    //! drag
    bool dragInProgress_;

    //! drag position
    QPoint dragOrigin_;

};

//! edit item dialog
class PlacesWidgetItemDialog: public CustomDialog
{

    public:

    //! constructor
    PlacesWidgetItemDialog( QWidget* = 0x0 );

    //! destructor
    virtual ~PlacesWidgetItemDialog( void )
    {}

    //! accessors
    //@{

    //! name
    QString name( void ) const
    { return nameEditor_->text(); }

    //! file
    QString file( void ) const
    { return fileEditor_->editor().text(); }

    //! remote
    bool isRemote( void ) const
    { return remoteCheckBox_->isChecked(); }

    //@}

    //!@name modifiers
    //@{

    //! name
    void setName( const QString& value )
    { nameEditor_->setText( value ); }

    //! file
    void setFile( const BaseFileInfo& value )
    {
        fileEditor_->setFile( value.file() );
        remoteCheckBox_->setChecked( value.isRemote() );
    }

    //@}

    private:

    //! name
    AnimatedLineEditor* nameEditor_;

    //! file
    BrowsedLineEditor* fileEditor_;

    //! remote checkbox
    QCheckBox* remoteCheckBox_;

};

//! local file info
class LocalFileInfo: public BaseFileInfo
{

    public:

    //! constructor
    LocalFileInfo( const BaseFileInfo& other, const QString& name ):
        BaseFileInfo( other ),
        name_( name )
    {}

    //! constructor from DOM element
    LocalFileInfo( const QDomElement& );

    //! destructor
    virtual ~LocalFileInfo( void )
    {}

    //! dump to dom element
    virtual QDomElement domElement( QDomDocument& ) const;

    //! name
    const QString& name( void ) const
    { return name_; }

    private:

    //! name
    QString name_;

};

#endif
