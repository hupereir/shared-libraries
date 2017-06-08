#ifndef LineEditor_h
#define LineEditor_h

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
#include "WeakPointer.h"

#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QProxyStyle>

class BaseContextMenu;

class LineEditor: public QLineEdit, private Base::Counter<LineEditor>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    explicit LineEditor( QWidget* parent );

    //* destructor
    virtual ~LineEditor( void )
    { delete proxyStyle_.data(); }

    //@!name accessors
    //@{

    bool hasClearButton( void ) const
    { return clearButton_ && clearButton_->isVisible(); }

    //* return clear button width
    int clearButtonWidth( void ) const;

    //* modification state
    bool isModified( void ) const
    { return modified_; }

    //@}

    //*@name modifiers
    //@{

    //* read-only state
    void setReadOnly( bool );

    //* set editor as modified
    void setModified( bool value );

    //* set clear button
    void setHasClearButton( bool );

    //* install actions in context menu
    virtual void installContextMenuActions( BaseContextMenu* );

    //@}

    Q_SIGNALS:

    //* signal emitted when the cursor changes position
    void cursorPositionChanged( int position );

    //* modification state changed
    void modificationChanged( bool );

    //* emitted when clear button is pressed
    void cleared( void );

    public Q_SLOTS:

    //* set text
    /** copy base class method to make it virtual */
    virtual void setText( const QString& value )
    { QLineEdit::setText( value ); }

    //* clear
    /** copy base class method to make it virtual */
    virtual void clear( void )
    { QLineEdit::clear(); }

    //* changes selection to uppercase
    void lowerCase( void );

    //* changes selection to uppercase
    void upperCase( void );

    protected:

    //*@name event handlers
    //@{

    //* generic event
    virtual bool event( QEvent* );

    //* context menu (overloaded)
    virtual void contextMenuEvent( QContextMenuEvent* );

    //* overloaded key press event
    virtual void keyPressEvent( QKeyEvent* );

    //@}

    //* toggle clear button
    virtual void _updateClearButton( void ) const;

    protected Q_SLOTS:

    //* update modification state
    virtual void _modified( const QString& text );

    //* update action status
    virtual void _updateSelectionActions( void );

    //* update paste action
    /** depends on clipboard status and editability */
    virtual void _updatePasteAction( void );

    //* update undo/redo actions
    virtual void _updateUndoRedoActions( void );

    protected:

    //* paint event
    virtual void paintEvent( QPaintEvent* );

    private:

    //* install actions
    void _installActions( void );

    //* modification state
    bool modified_ = false;

    //* backup string to track modifications
    QString backup_;

    //*@name default actions
    //@{

    //* undo
    QAction* undoAction_ = nullptr;

    //* redo
    QAction* redoAction_ = nullptr;

    //* cut selection
    QAction* cutAction_ = nullptr;

    //* copy selection
    QAction* copyAction_ = nullptr;

    //* paste clipboard
    QAction* pasteAction_ = nullptr;

    //* clear document
    QAction* clearAction_ = nullptr;

    //* select all document
    QAction* selectAllAction_ = nullptr;

    //* convert selection to upper case
    QAction* upperCaseAction_ = nullptr;

    //* convert selection to lower case
    QAction* lowerCaseAction_ = nullptr;

    //@}

    //* clear button
    QWidget* clearButton_ = nullptr;

    //* style proxy
    Base::WeakPointer<QProxyStyle> proxyStyle_;

};

#endif
