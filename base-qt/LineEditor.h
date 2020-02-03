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

#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>
#include <QPointer>
#include <QProxyStyle>
#include <QStyleOptionButton>
#include <QToolButton>

class BaseContextMenu;

//* used to insert buttons into a line editor
class LineEditorButton: public QToolButton, private Base::Counter<LineEditorButton>
{

    Q_OBJECT

    public:

    //* constructor
    explicit LineEditorButton( QWidget* = nullptr );

    //*@name accessors
    //@{

    //* size hint
    QSize sizeHint() const override;

    //* flags
    enum Flag
    {
        None = 0,
        ShowWhenEmpty = 1<<0,
        ShowWhenReadOnly = 1<<1
    };

    Q_DECLARE_FLAGS( Flags, Flag );
    Flags flags() const
    { return flags_; }

    //@}

    //*@name modifiers
    //@{

    //* flags
    void setFlags( Flags value )
    { flags_ = value; }

    //@}

    protected:

    //* paint
    void paintEvent( QPaintEvent* ) override;

    private:

    //* flags
    Flags flags_ = None;

};

class LineEditor: public QLineEdit, private Base::Counter<LineEditor>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    explicit LineEditor( QWidget* parent );

    //* destructor
    ~LineEditor() override
    { delete proxyStyle_.data(); }

    //*@name accessors
    //@{

    //* clear button
    bool showClearButton() const
    { return showClearButton_; }

    //* clear button
    QAbstractButton& clearButton() const
    { return *clearButton_; }

    //* buttons width
    QSize leftButtonsSize() const;

    //* buttons width
    QSize rightButtonsSize() const;

    //* modification state
    bool isModified() const
    { return modified_; }

    //@}

    //*@name modifiers
    //@{

    //* read-only state
    void setReadOnly( bool );

    //* set editor as modified
    void setModified( bool value );

    //* set clear button
    void setShowClearButton( bool );

    //* add a button
    void addLeftWidget( QWidget* widget )
    { return _addWidget( widget, leftContainer_ ); }

    //* add a button
    void addRightWidget( QWidget* widget )
    { return _addWidget( widget, rightContainer_ ); }

    //* install actions in context menu
    virtual void installContextMenuActions( BaseContextMenu* );

    //* set text
    /** copy base class method to make it virtual */
    virtual void setText( const QString& value )
    { QLineEdit::setText( value ); }

    //* clear
    /** copy base class method to make it virtual */
    virtual void clear()
    { QLineEdit::clear(); }

    //* changes selection to uppercase
    void lowerCase();

    //* changes selection to uppercase
    void upperCase();

    //@}

    Q_SIGNALS:

    //* signal emitted when the cursor changes position
    void cursorPositionChanged( int position );

    //* modification state changed
    void modificationChanged( bool );

    //* emitted when clear button is pressed
    void cleared();

    protected:

    //*@name event handlers
    //@{

    //* generic event
    bool event( QEvent* ) override;

    //* context menu (overloaded)
    void contextMenuEvent( QContextMenuEvent* ) override;

    //* overloaded key press event
    void keyPressEvent( QKeyEvent* ) override;

    //@}

    private:

    //* update modification state
    void _modified( const QString& text );

    //* update action status
    void _updateSelectionActions();

    //* update paste action
    /** depends on clipboard status and editability */
    void _updatePasteAction();

    //* update undo/redo actions
    void _updateUndoRedoActions();

    //* add widget to parent
    void _addWidget( QWidget*, QWidget* );

    //* update inline buttons visibility
    void _updateButtonsVisibility();

    //* update inline buttons geometry
    void _updateButtonsGeometry();

    //* install actions
    void _installActions();

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

    //* true if clear button must be shown
    bool showClearButton_ = true;

    //* left buttons container
    QWidget* leftContainer_ = nullptr;

    //* right buttons container
    QWidget* rightContainer_ = nullptr;

    //* clear button
    LineEditorButton* clearButton_ = nullptr;

    //* style proxy
    QPointer<QProxyStyle> proxyStyle_;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( LineEditorButton::Flags )

#endif
