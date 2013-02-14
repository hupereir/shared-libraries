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

#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QPaintEvent>

/*!
\class LineEditor
\brief customized line edit to have faster popup menu
*/
class LineEditor: public QLineEdit, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    LineEditor( QWidget* parent );

    //! read-only state
    void setReadOnly( bool );

    //! set editor as modified
    void setModified( const bool& value );

    //! modification state
    const bool& isModified( void ) const
    { return modified_; }

    //! set clear button
    void setHasClearButton( const bool& );

    //! set frame
    void setFrame( const bool& );

    //! has frame
    bool hasFrame( void ) const
    { return hasFrame_; }

    signals:

    //! signal emmited when the cursor changes position
    void cursorPositionChanged( int position );

    //! modification state changed
    void modificationChanged( bool );

    //! emmited when clear button is pressed
    void cleared( void );

    public slots:

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

    //! paint
    void paintEvent( QPaintEvent* );

    //! context menu (overloaded)
    virtual void contextMenuEvent( QContextMenuEvent* );

    //! overloaded key press event
    virtual void keyPressEvent( QKeyEvent* );

    //! overloaded mouse event handler
    virtual void mouseMoveEvent( QMouseEvent* );

    //! overloaded mouse event handler
    virtual void mousePressEvent( QMouseEvent* );

    //! mouse move
    void mouseReleaseEvent( QMouseEvent* );

    //@}

    //! has clear button
    const bool& _hasClearButton( void )
    { return hasClearButton_; }

    //! clear button rect
    const QRect& _clearButtonRect( void ) const
    { return clearButtonRect_; }

    //! clear button rect
    virtual void _setClearButtonRect( const QRect& rect )
    { clearButtonRect_ = rect; }

    //! toggle clear button
    virtual bool _toggleClearButton( const bool& );

    //! clear button visibility
    virtual const bool& _clearButtonVisible( void ) const
    { return clearButtonVisible_; }

    //! set clear button visibility
    virtual void _setClearButtonVisible( const bool& value )
    { clearButtonVisible_ = value; }

    //! paint clear button
    virtual void _paintClearButton( QPainter& painter )
    { _paintClearButton( painter, true ); }

    //! paint clear button
    virtual void _paintClearButton( QPainter&, const bool& );

    //! framewidth
    int _frameWidth( void ) const;

    //! clear icon
    virtual const QIcon& _clearIcon( void ) const
    { return clearIcon_; }

    protected slots:

    //! update modification state
    virtual void _modified( const QString& text );

    //! update action status
    virtual void _updateSelectionActions( void );

    //! update paste action
    /*! depends on clipboard status and editability */
    virtual void _updatePasteAction( void );

    //! update undo/redo actions
    virtual void _updateUndoRedoActions( void );

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

    //!@name properties
    //@{

    //! true when clear button should be drawn
    bool hasClearButton_;

    //! true when clear button is visible
    bool clearButtonVisible_;

    //! clear button rect
    QRect clearButtonRect_;

    //! frame
    bool hasFrame_;

    //! true if clear button is triggered
    bool triggered_;

    //! clear pixmap
    QIcon clearIcon_;

    //@}

};

#endif
