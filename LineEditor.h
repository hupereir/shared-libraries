#ifndef LineEditor_h
#define LineEditor_h

// $Id$

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

class LineEditor: public QLineEdit, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    LineEditor( QWidget* parent );

    //! destructor
    virtual ~LineEditor( void )
    { delete proxyStyle_.data(); }

    //@!name accessors
    //@{

    bool hasClearButton( void ) const
    { return clearButton_ && clearButton_->isVisible(); }

    //! return clear button width
    int clearButtonWidth( void ) const;

    //! modification state
    const bool& isModified( void ) const
    { return modified_; }

    //@}

    //!@name modifiers
    //@{

    //! read-only state
    void setReadOnly( bool );

    //! set editor as modified
    void setModified( const bool& value );

    //! set clear button
    void setHasClearButton( const bool& );

    //@}

    Q_SIGNALS:

    //! signal emmited when the cursor changes position
    void cursorPositionChanged( int position );

    //! modification state changed
    void modificationChanged( bool );

    //! emmited when clear button is pressed
    void cleared( void );

    public Q_SLOTS:

    //! set text
    /*! copy base class method to make it virtual */
    virtual void setText( const QString& value )
    { QLineEdit::setText( value ); }

    //! clear
    /*! copy base class method to make it virtual */
    virtual void clear( void )
    { QLineEdit::clear(); }

    //! changes selection to uppercase
    void lowerCase( void );

    //! changes selection to uppercase
    void upperCase( void );

    protected:

    //!@name event handlers
    //@{

    //! generic event
    virtual bool event( QEvent* );

    //! context menu (overloaded)
    virtual void contextMenuEvent( QContextMenuEvent* );

    //! overloaded key press event
    virtual void keyPressEvent( QKeyEvent* );

    //@}

    //! toggle clear button
    virtual void _updateClearButton( void ) const;

    protected Q_SLOTS:

    //! update modification state
    virtual void _modified( const QString& text );

    //! update action status
    virtual void _updateSelectionActions( void );

    //! update paste action
    /*! depends on clipboard status and editability */
    virtual void _updatePasteAction( void );

    //! update undo/redo actions
    virtual void _updateUndoRedoActions( void );

    protected:

    //! paint event
    virtual void paintEvent( QPaintEvent* );

    private:

    //! install actions
    void _installActions( void );

    //! modification state
    bool modified_;

    //! backup string to track modifications
    QString backup_;

    //!@name default actions
    //@{

    //! undo
    QAction* undoAction_;

    //! redo
    QAction* redoAction_;

    //! cut selection
    QAction* cutAction_;

    //! copy selection
    QAction* copyAction_;

    //! paste clipboard
    QAction* pasteAction_;

    //! clear document
    QAction* clearAction_;

    //! select all document
    QAction* selectAllAction_;

    //! convert selection to upper case
    QAction* upperCaseAction_;

    //! convert selection to lower case
    QAction* lowerCaseAction_;

    //@}

    //! clear button
    QWidget* clearButton_;

    //! style proxy
    Base::WeakPointer<QProxyStyle> proxyStyle_;

};

#endif
