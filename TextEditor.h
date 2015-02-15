#ifndef TextEditor_h
#define TextEditor_h

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

#include "BlockHighlight.h"
#include "BoxSelection.h"
#include "Counter.h"
#include "CursorMonitor.h"
#include "Debug.h"
#include "Key.h"
#include "MultipleClickCounter.h"
#include "RemoveLineBuffer.h"
#include "TextBlockData.h"
#include "TextPosition.h"
#include "TextSelection.h"

#include <QAction>
#include <QBasicTimer>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QFocusEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTimerEvent>

class BaseContextMenu;
class BaseFindDialog;
class BaseFindWidget;
class BaseReplaceDialog;
class BaseReplaceWidget;
class SelectLineDialog;
class SelectLineWidget;
class TextEditorMarginWidget;
class LineNumberDisplay;

#ifdef QT_USE_PLAIN_TEXT_EDIT
#include <QPlainTextEdit>
using BaseEditor = QPlainTextEdit;
#else
#include <QTextEdit>
using BaseEditor = QTextEdit;
#endif

//* Customized QTextEdit object
class TextEditor: public BaseEditor, public Base::Key, public Counter
{

    Q_OBJECT

    public:

    //* modifiers
    enum Modifier
    {
        ModifierNone = 0,
        ModifierCapsLock = 1<<0,
        ModifierNumLock = 1<<1,
        ModifierInsert = 1<<2,
        ModifierWrap = 1<<3
    };

    Q_DECLARE_FLAGS( Modifiers, Modifier );

    //* constructor
    TextEditor( QWidget* = nullptr );

    //* destrutor
    virtual ~TextEditor( void );

    //* retrieve number of blocks in document
    virtual int blockCount( void ) const;

    //* retrieve number of blocks associated to argument
    /*! the default implementation returns 1. In QEdit, it is reimplemented accounts for collapsed blocks */
    virtual int blockCount( const QTextBlock& ) const
    { return 1; }

    //* retrieve current text position
    virtual TextPosition textPosition( void );

    //* return true if current textCursor is visible
    virtual bool isCursorVisible( void ) const;

    #ifdef QT_USE_PLAIN_TEXT_EDIT
    //* set text always fall back to plain text
    virtual void setText( const QString& value )
    { setPlainText( value ); }

    //* rich text is always disabled
    virtual void setAcceptRichText( bool )
    {}
    #endif

    //* set text
    virtual void setPlainText( const QString& );

    //* set text
    virtual void setHtml( const QString& );

    //* draw margins
    virtual void paintMargin( QPainter& );

    //* select word under cursor
    virtual void selectWord( void );

    //* select line under cursor
    virtual void selectLine( void );

    //* set textChar format
    /*!
    this overloads the base class method (although the later is not virtual)
    in order to properly handle box selection, if any
    */
    void mergeCurrentCharFormat( const QTextCharFormat& );

    //* clear box selection
    virtual void clearBoxSelection( void )
    {
        if( boxSelection_.state() == BoxSelection::SelectionFinished )
        { boxSelection_.clear(); }
    }

    //*@name synchronization
    //@{

    //* synchronization
    virtual void setSynchronized( const bool& value )
    { synchronize_ = value; }

    //* synchronization
    virtual const bool& isSynchronized( void ) const
    { return synchronize_; }

    //* clone (and synchronize) text editor
    virtual void synchronize( TextEditor* );

    //* activity
    virtual bool setActive( const bool& );

    //* activity
    virtual const bool& isActive( void ) const
    { return active_; }

    //@}

    //* popup dialog with the number of replacement performed
    virtual void showReplacements( const unsigned int& counts );

    //* TextSelection object from this selection, or clipboard
    TextSelection selection( void ) const;

    //* last searched selection
    static TextSelection& lastSelection( void );

    //* last searched selection
    static void setLastSelection( const TextSelection& selection )
    { lastSelection() = selection; }

    //*@name text wrap
    //@{

    //* enable/disable reading of text wrapping mode from options
    const bool& wrapFromOptions( void ) const
    { return wrapFromOptions_; }

    //* enable/disable reading of text wrapping mode from options
    void setWrapFromOptions( const bool& value )
    { wrapFromOptions_ = value; }

    //* enable/disable reading of line number display from options
    const bool& lineNumbersFromOptions( void ) const
    { return lineNumberFromOptions_; }

    //* enable/disable reading of line number display from options
    void setLineNumbersFromOptions( const bool& value )
    { lineNumberFromOptions_ = value; }

    //@}

    //* read-only
    virtual void setReadOnly( bool );

    //* reset undo/redo history
    virtual void resetUndoRedoStack( void );

    //* install actions in context menu
    virtual void installContextMenuActions( BaseContextMenu*, const bool& = true );

    //*@name actions
    //@{

    QAction& undoAction( void ) const
    { return *undoAction_; }

    //* redo
    QAction& redoAction( void ) const
    { return *redoAction_; }

    //* cut selection
    QAction& cutAction( void ) const
    { return *cutAction_; }

    //* copy selection
    QAction& copyAction( void ) const
    { return *copyAction_; }

    //* paste clipboard
    QAction& pasteAction( void ) const
    { return *pasteAction_;  }

    //* convert selection to upper case
    QAction& lowerCaseAction( void ) const
    { return *upperCaseAction_; }

    //* convert selection to lower case
    QAction& upperCaseAction( void ) const
    { return *lowerCaseAction_; }

    //* find from dialog
    QAction& findAction( void ) const
    { return *findAction_; }

    //* find selection again
    QAction& findSelectionAction( void ) const
    { return *findSelectionAction_; }

    //* find again
    QAction& findAgainAction( void ) const
    { return *findAgainAction_; }

    //* replace
    QAction& replaceAction( void ) const
    { return *replaceAction_; }

    //* replace again
    QAction& replaceAgainAction( void ) const
    { return *replaceAgainAction_; }

    //* goto line number
    QAction& gotoLineAction( void ) const
    { return *gotoLineAction_; }

    //* block highlight action
    QAction& blockHighlightAction( void ) const
    { return *blockHighlightAction_; }

    //* toggle wrap mode
    QAction& wrapModeAction( void ) const
    { return *wrapModeAction_; }

    //* toggle tab emulation
    QAction& tabEmulationAction( void ) const
    { return *tabEmulationAction_; }

    //* show line numbers
    bool hasLineNumberAction( void ) const
    { return showLineNumberAction_; }

    //* show line numbers
    QAction& showLineNumberAction( void ) const
    { return *showLineNumberAction_; }

    //@}

    //*@name tab emulation
    //@{

    //* tab character
    virtual const QString& tabCharacter( void ) const
    { return tab_; }

    //* tab character
    virtual const QString& normalTabCharacter( void ) const
    { return normalTab_; }

    //* tab character
    virtual const QString& emulatedTabCharacter( void ) const
    { return emulatedTab_; }

    //@}

    //* block highlight object
    BlockHighlight& blockHighlight() const
    { return *blockHighlight_; }

    //* changes block background
    virtual void setBackground( QTextBlock, const QColor& );

    //* clear block background
    virtual void clearBackground( QTextBlock );

    //* clear all blocks background
    virtual void clearAllBackgrounds( void );

    //*@name widget to viewport translations
    //@{

    //* scrollbar position
    QPoint scrollbarPosition( void ) const
    { return QPoint(  horizontalScrollBar()->value(), verticalScrollBar()->value() ); }

    //* widget to viewport translation
    QRect toViewport( const QRect& rect ) const
    { return rect.translated( -scrollbarPosition() ); }

    //* widget to viewport translation
    QPoint toViewport( const QPoint& point ) const
    { return point - scrollbarPosition(); }

    //* widget from viewport translation
    QRect fromViewport( const QRect& rect ) const
    { return rect.translated( scrollbarPosition() ); }

    //* widget from viewport translation
    QPoint fromViewport( const QPoint& point ) const
    { return point + scrollbarPosition(); }

    //@}

    //* modifiers
    Modifiers modifiers( void ) const
    { return modifiers_; }

    //* modifiers
    bool modifier( const Modifier& key ) const
    { return modifiers_&key; }

    // return true if block is an empty line
    virtual bool isEmptyBlock( const QTextBlock& ) const
    { return false; }

    //* return true is block is to be ignored from indentation scheme
    virtual bool ignoreBlock( const QTextBlock& ) const
    { return false; }

    //* container class for embedded Find dialog
    class Container: public QWidget, public Counter
    {

        public:

        //* constructor
        explicit Container( QWidget* = nullptr );

        //* constructor
        Container( QWidget* parent, TextEditor* editor );

        //* destructor
        ~Container( void ) = default;

        //*@name accessors
        TextEditor& editor( void ) const
        { return *editor_; }

        private:

        //* initialize
        void _initialize( void );

        //* contained editor
        TextEditor* editor_;

    };

    Q_SIGNALS:

    //* busy signal
    void busy( int );

    //* progress
    void progressAvailable( int );

    //* idle
    void idle( void );

    //* emmitted when selection could not be found
    void noMatchFound( void );

    //* emmitted when selection could be found
    void matchFound( void );

    //* emmited when recieve focus
    void hasFocus( TextEditor* );

    //* overwrite mode changed
    void modifiersChanged( TextEditor::Modifiers );

    public Q_SLOTS:

    //* cut
    virtual void cut( void );

    //* copy
    virtual void copy( void );

    //* paste
    virtual void paste( void );

    //* changes selection to uppercase
    virtual void upperCase( void );

    //* changes selection to lower case
    virtual void lowerCase( void );

    //* find next occurence of TextSelection
    virtual void find( TextSelection selection );

    //* find current selection forward
    virtual void findSelectionForward( void );

    //* find current selection backward
    virtual void findSelectionBackward( void );

    //* find last search forward
    virtual void findAgainForward( void );

    //* find last search forward
    virtual void findAgainBackward( void );

    //* find next occurence of TextSelection
    virtual void replace( TextSelection selection );

    //* replace selection in range
    virtual unsigned int replaceInSelection( TextSelection selection, const bool& show_dialog = true );

    //* replace selection in window, returns number of replacements
    virtual unsigned int replaceInWindow( TextSelection selection, const bool& show_dialog = true );

    //* replace again forward
    virtual void replaceAgainForward( void );

    //* replace again forward
    virtual void replaceAgainBackward( void );

    //* select line by number
    virtual void selectLine( int index );

    //* remove current line
    virtual void removeLine( void );

    //* clear
    virtual void clear( void );

    protected:

    //*@name event handlers
    //@{

    //* enter event handler
    void enterEvent( QEvent *event );

    //* mouse release event [overloaded]
    virtual void mousePressEvent( QMouseEvent* );

    //* mouse double click event [overloaded]
    /*!
    for left button, double click events are handled
    essentially like single click events, because the number of clicks
    is handled independently by the MultipleClickCounter object
    */
    virtual void mouseDoubleClickEvent( QMouseEvent* );

    //* mouse press event [overloaded]
    virtual void mouseMoveEvent( QMouseEvent* );

    //* mouse press event [overloaded]
    virtual void mouseReleaseEvent( QMouseEvent* );

    //* drop event
    virtual void dropEvent( QDropEvent* event );

    //* keypress event
    virtual void keyPressEvent( QKeyEvent* );

    //* focus event [overloaded]
    virtual void focusInEvent( QFocusEvent* );

    //* context menu event [overloaded]
    virtual void contextMenuEvent( QContextMenuEvent* );

    //* resize event
    /* need to update LineNumberWidget, when wrap is enabled */
    virtual void resizeEvent( QResizeEvent* );

    //* repaint event
    virtual void paintEvent( QPaintEvent* );

    //* timer event
    virtual void timerEvent( QTimerEvent* event );

    //@}

    //* scroll
    virtual void scrollContentsBy( int dx, int dy );

    //* context menu position
    const QPoint& _contextMenuPosition( void ) const
    { return contextMenuPosition_; }

    //*@name find/replace selection
    //@{

    //* find dialog
    virtual void _createFindDialog( void );

    //* find widget
    virtual void _createFindWidget( bool compact );

    //* find selection in forward direction
    virtual bool _findForward( const TextSelection& selection, const bool& rewind );

    //* find selection in backward direction
    virtual bool _findBackward( const TextSelection& selection, const bool& rewind );

    //* replace dialog
    virtual void _createReplaceDialog( void );

    //* replace widget
    virtual void _createReplaceWidget( bool compact );

    //* select line dialog
    virtual void _createSelectLineDialog( void );

    //* select line widget
    virtual void _createSelectLineWidget( bool compact );

    //* progress dialog
    virtual void _createProgressDialog( void );

    //* define how cursor should be updated while replacing
    enum CursorMode
    {
        ExpandCursor,
        MoveCursor
    };

    //* replace selection in range refered to by cursor
    virtual unsigned int _replaceInRange( const TextSelection& selection, QTextCursor& cursor, CursorMode mode );

    //@}

    //*@name box selection
    //@{

    //* box selection
    BoxSelection& _boxSelection( void )
    { return boxSelection_; }

    //* box selection
    const BoxSelection& _boxSelection( void ) const
    { return boxSelection_; }

    //* synchronize box selection with associated displays
    void _synchronizeBoxSelection() const;

    //@}

    //*@name line numbers
    //@{

    //* left margin
    int _leftMargin( void ) const
    { return leftMargin_; }

    //* left margin
    bool _setLeftMargin( const int& margin );

    //@}

    //* toggle insertion mode
    virtual void _toggleOverwriteMode( void );

    //*@name tab emulation
    //@{

    //* set tab character size
    /*! returns true if changed */
    virtual bool _setTabSize( const int& size );

    //* tab emulation
    virtual const bool& _hasTabEmulation( void ) const
    { return hasTabEmulation_; }

    //* insert (normal or emulated) tab
    virtual void _insertTab( void );

    //* multi tab regular expression
    virtual const QRegExp& _tabRegExp( void ) const
    { return tabRegexp_; }

    //* 'normal' tab regular expression
    virtual const QRegExp& _normalTabRegExp( void ) const
    { return normalTabRegexp_; }

    //* 'emulated' tab regular expression
    virtual const QRegExp& _emulatedTabRegExp( void ) const
    { return emulatedTabRegexp_; }

    //@}

    //*@name margin
    //*@{

    //* margin widget
    TextEditorMarginWidget& _marginWidget( void ) const
    { return *marginWidget_; }

    //* update margins
    virtual bool _updateMargin( void );

    //* current block rect
    virtual const QRect& _currentBlockRect( void ) const
    { return currentBlockRect_; }

    //* current block rect
    virtual bool _setCurrentBlockRect( const QRect& rect )
    {
        if( rect == currentBlockRect_ ) return false;
        currentBlockRect_ = rect;
        return true;
    }

    //@}

    //*@name line numbers
    //@{

    //* line number display
    bool _hasLineNumberDisplay( void ) const
    { return lineNumberDisplay_; }

    //* line number display
    LineNumberDisplay& _lineNumberDisplay( void ) const
    { return *lineNumberDisplay_; }

    //@}

    //* modifiers
    bool _setModifier( const Modifier& key, bool value )
    {
        if( modifier( key ) == value ) return false;
        if( value ) modifiers_ |= key;
        else modifiers_ &= ~key;
        return true;
    }

    protected Q_SLOTS:

    //* synchronize selection
    virtual void _synchronizeSelection( void );

    //* update action status
    virtual void _updateReadOnlyActions( bool );

    //* update undo/redo actions
    virtual void _updateUndoRedoActions( void );

    //* update action status
    /*! this works only if you have qt > 4.2 */
    virtual void _updateSelectionActions( bool );

    //* update action status
    virtual void _updateClipboardActions( QClipboard::Mode );

    //* update selection clipboard
    virtual void _updateClipboard( void );

    //* update paste action
    /*! depends on clipboard status and editability */
    virtual void _updatePasteAction( void );

    //* toggle block highlight
    virtual void _toggleBlockHighlight( bool );

    //* wrap mode
    /*! returns true if changed */
    virtual bool _toggleWrapMode( bool );

    //* tab emulation
    /*! returns true if changed */
    virtual bool _toggleTabEmulation( bool );

    //* toggle line number display
    virtual void _toggleShowLineNumbers( bool state );

    //* block count changed
    /*! needed to adjust width of line number display */
    void _blockCountChanged( int );

    //* find text from dialog
    virtual void _findFromDialog( void );

    //* replace text from dialog
    virtual void _replaceFromDialog( void );

    //* select line from dialog
    virtual void _selectLineFromDialog( void );

    private Q_SLOTS:

    //* update configuration
    void _updateConfiguration( void );

    private:

    //* install default actions
    void _installActions( void );

    //* margin widget
    TextEditorMarginWidget* marginWidget_ = nullptr;

    //* current block rect
    /*! needed for block highlighting in margin */
    QRect currentBlockRect_;

    //* context menu event position
    QPoint contextMenuPosition_;

    //*@name dialogs
    ///@{

    //* find dialog
    BaseFindDialog* findDialog_ = nullptr;

    //* find dialog
    BaseReplaceDialog* replaceDialog_ = nullptr;

    //* line number dialog
    SelectLineDialog* selectLineDialog_ = nullptr;

    //* find widget
    BaseFindWidget* findWidget_ = nullptr;

    //* replace widget
    BaseReplaceWidget* replaceWidget_ = nullptr;

    //* select line widget
    SelectLineWidget* selectLineWidget_ = nullptr;

    //@}

    //*@name line number display
    //@{

    //* line numbers
    LineNumberDisplay* lineNumberDisplay_ = nullptr;

    //* left margin width
    int leftMargin_ = 0;

    //@}

    //* true if this display is the active display
    bool active_ = false;

    /*! set to false when the wrapping must not get loaded via the options */
    bool wrapFromOptions_ = true;

    /*! set to false when the display of line numbers must not get loaded via the options. */
    bool lineNumberFromOptions_ = true;

    //*@name tab emulation and empty lines
    //@{

    //* tab emulation flag
    bool hasTabEmulation_ = false;

    //* tab string
    QString emulatedTab_;

    //* tab string
    QString normalTab_;

    //* current tab string
    QString tab_;

    //* emulated tab regular expression
    QRegExp emulatedTabRegexp_;

    //* normale tab regular expression
    QRegExp normalTabRegexp_;

    //* multi tab regExp
    QRegExp tabRegexp_;

    //@}

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

    //* find from dialog
    QAction* findAction_ = nullptr;

    //* find selection again
    QAction* findSelectionAction_ = nullptr;

    //* find selection backward
    QAction* findSelectionBackwardAction_ = nullptr;

    //* find again
    QAction* findAgainAction_ = nullptr;

    //* find again backward
    QAction* findAgainBackwardAction_ = nullptr;

    //* replace
    QAction* replaceAction_ = nullptr;

    //* replace again
    QAction* replaceAgainAction_ = nullptr;

    //* replace again backward
    QAction* replaceAgainBackwardAction_ = nullptr;

    //* goto line number
    QAction* gotoLineAction_ = nullptr;

    //* toggle highlight block
    QAction* blockHighlightAction_ = nullptr;

    //* toggle wrap mode
    QAction* wrapModeAction_ = nullptr;

    //* toggle tab emulation
    QAction* tabEmulationAction_ = nullptr;

    //* line number
    QAction* showLineNumberAction_ = nullptr;

    //@}

    //* synchronization flag
    bool synchronize_ = false;

    //* true if use dialog for finding
    bool useEmbeddedDialogs_ = false;

    //* box selection
    BoxSelection boxSelection_;

    //* cursor monitor
    CursorMonitor cursorMonitor_;

    //* remove_line buffer
    RemoveLineBuffer removeLineBuffer_;

    //* multiple click counter
    MultipleClickCounter clickCounter_;

    //* current block highlight
    BlockHighlight* blockHighlight_;

    //* current block highlight color
    QColor highlightColor_;

    //* store possible mouse drag start position
    QPoint dragStart_;

    //* mimics QTextEdit autoscroll timer to update box selection */
    QBasicTimer autoScrollTimer_;

    //* keyboard modifiers
    /*! this is a bitwise or of the Modifiers enumeration */
    Modifiers modifiers_ = ModifierNone;

    friend class Container;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( TextEditor::Modifiers )

#endif
