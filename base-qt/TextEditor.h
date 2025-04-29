#ifndef TextEditor_h
#define TextEditor_h

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
#include "IntegralType.h"
#include "Key.h"
#include "MultipleClickCounter.h"
#include "RemoveLineBuffer.h"
#include "TextBlockData.h"
#include "TextPosition.h"
#include "TextSelection.h"
#include "base_qt_export.h"

#include <QAction>
#include <QBasicTimer>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QFocusEvent>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTimerEvent>
#include <QWheelEvent>

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
class BASE_QT_EXPORT TextEditor: public BaseEditor, public Base::Key, private Base::Counter<TextEditor>
{

    Q_OBJECT

    public:

    //* modifiers
    enum Modifier:uint16_t
    {
        None = 0,
        CapsLock = 1<<0,
        NumLock = 1<<1,
        Insert = 1<<2,
        Wrap = 1<<3
    };

    using Modifiers = Base::underlying_type_t<Modifier>;

    //* constructor
    explicit TextEditor( QWidget* = nullptr );

    //* destrutor
    ~TextEditor() override;

    //*@name accessors
    //@{

    //* retrieve number of blocks in document
    virtual int blockCount() const;

    //* retrieve number of blocks associated to argument
    /** the default implementation returns 1. In QEdit, it is reimplemented accounts for collapsed blocks */
    virtual int blockCount( const QTextBlock& ) const
    { return 1; }

    //* retrieve current text position
    virtual TextPosition textPosition();

    //* anchor at context menu
    virtual QString anchor() const final
    { return anchorAt( _contextMenuPosition() ); }

    //* return true if current textCursor is visible
    virtual bool isCursorVisible() const;

    //* synchronization
    virtual bool isSynchronized() const
    { return synchronize_; }

    //* active state (in case of synchronization with other editors)
    virtual bool isActive() const
    { return active_; }

    //* has selection
    bool hasSelection() const;

    //* TextSelection object from this selection, or clipboard
    TextSelection selection() const;

    //* last searched selection
    static TextSelection& lastSelection();

    //* enable/disable reading of text wrapping mode from options
    bool wrapFromOptions() const
    { return wrapFromOptions_; }

    //* enable/disable reading of line number display from options
    bool lineNumbersFromOptions() const
    { return lineNumberFromOptions_; }

    //* block highlight from options
    bool highlightBlockFromOptions() const
    { return highlightBlockFromOptions_; }

    //* tab character
    virtual const QString& tabCharacter() const
    { return tab_; }

    //* tab character
    virtual const QString& normalTabCharacter() const
    { return normalTab_; }

    //* tab character
    virtual const QString& emulatedTabCharacter() const
    { return emulatedTab_; }

    //* block highlight object
    BlockHighlight& blockHighlight() const
    { return *blockHighlight_; }

    //* find widget
    BaseFindWidget& findWidget() const
    { return *findWidget_; }

    //* replace widget
    BaseReplaceWidget& replaceWidget() const
    { return *replaceWidget_; }

    //* select line widget
    SelectLineWidget& selectLineWidget() const
    { return *selectLineWidget_; }

    //* scrollbar position
    QPoint scrollbarPosition() const
    { return QPoint(  horizontalScrollBar()->value(), verticalScrollBar()->value() ); }

    //* widget to viewport translation
    QRect toViewport( QRect rect ) const
    { return rect.translated( -scrollbarPosition() ); }

    //* widget to viewport translation
    QPoint toViewport( QPoint point ) const
    { return point - scrollbarPosition(); }

    //* widget from viewport translation
    QRect fromViewport( QRect rect ) const
    { return rect.translated( scrollbarPosition() ); }

    //* widget from viewport translation
    QPoint fromViewport( QPoint point ) const
    { return point + scrollbarPosition(); }

    //* modifiers
    Modifiers modifiers() const
    { return modifiers_; }

    //* modifiers
    bool modifier( TextEditor::Modifier key ) const
    { return modifiers_&key; }

    // return true if block is an empty line
    virtual bool isEmptyBlock( const QTextBlock& ) const
    { return false; }

    //* return true is block is to be ignored from indentation scheme
    virtual bool ignoreBlock( const QTextBlock& ) const
    { return false; }

    //@}

    //*@name modifiers
    //@{

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
    virtual void selectWord();

    //* select line under cursor
    virtual void selectLine();

    //* set textChar format
    /**
    this overloads the base class method (although the later is not virtual)
    in order to properly handle box selection, if any
    */
    void mergeCurrentCharFormat( const QTextCharFormat& );

    //* clear box selection
    virtual void clearBoxSelection()
    {
        if( boxSelection_.state() == BoxSelection::State::Finished )
        { boxSelection_.clear(); }
    }

    //* synchronization
    virtual void setSynchronized( bool value )
    { synchronize_ = value; }

    //* clone (and synchronize) text editor
    virtual void synchronize( TextEditor* );

    //* active state (in case of synchronization with other editors)
    virtual bool setActive( bool );

    //* popup dialog with the number of replacement performed
    virtual void showReplacements( int );

    //* last searched selection
    static void setLastSelection( const TextSelection& selection )
    { lastSelection() = selection; }

    //* enable/disable reading of text wrapping mode from options
    void setWrapFromOptions( bool value )
    { wrapFromOptions_ = value; }

    //* enable/disable reading of line number display from options
    void setLineNumbersFromOptions( bool value )
    { lineNumberFromOptions_ = value; }

    //* block highlight from options
    void setHighlightBlockFromOptions( bool value )
    { highlightBlockFromOptions_ = value; }

    //* true if should track anchors (html links)
    virtual void setTrackAnchors( bool );

    //* read-only
    virtual void setReadOnly( bool );

    //* reset undo/redo history
    virtual void resetUndoRedoStack();

    //* install actions in context menu
    virtual void installContextMenuActions( BaseContextMenu*, bool = true );

    //* use embedded widgets
    //** TODO: should delete widgets in case value has changed */
    void setUseEmbeddedWidgets( bool value )
    {
        if( useEmbeddedWidgets_ == value ) return;
        useEmbeddedWidgets_ = value;
    }

    //* find widget
    virtual void createFindWidget( bool compact );

    //* replace widget
    virtual void createReplaceWidget( bool compact );

    //* select line widget
    virtual void createSelectLineWidget( bool compact );

    //* changes block background
    virtual void setBackground( const QTextBlock&, const QColor& );

    //* clear block background
    virtual void clearBackground( const QTextBlock& );

    //* clear all blocks background
    virtual void clearAllBackgrounds();

    //@}

    //*@name actions
    //@{

    QAction& undoAction() const
    { return *undoAction_; }

    //* redo
    QAction& redoAction() const
    { return *redoAction_; }

    //* cut selection
    QAction& cutAction() const
    { return *cutAction_; }

    //* copy selection
    QAction& copyAction() const
    { return *copyAction_; }

    //* paste clipboard
    QAction& pasteAction() const
    { return *pasteAction_;  }

    //* paste clipboard
    QAction& pasteUnformatedAction() const
    { return *pasteUnformatedAction_;  }

    //* select all
    QAction& selectAllAction() const
    { return *selectAllAction_; }

    //* clear
    QAction& clearAction() const
    { return *clearAction_; }

    //* convert selection to upper case
    QAction& lowerCaseAction() const
    { return *upperCaseAction_; }

    //* convert selection to lower case
    QAction& upperCaseAction() const
    { return *lowerCaseAction_; }

    //* find from dialog
    QAction& findAction() const
    { return *findAction_; }

    //* find selection again
    QAction& findSelectionAction() const
    { return *findSelectionAction_; }

    //* find again
    QAction& findAgainAction() const
    { return *findAgainAction_; }

    //* replace
    QAction& replaceAction() const
    { return *replaceAction_; }

    //* replace again
    QAction& replaceAgainAction() const
    { return *replaceAgainAction_; }

    //* goto line number
    QAction& gotoLineAction() const
    { return *gotoLineAction_; }

    //* block highlight action
    QAction& blockHighlightAction() const
    { return *blockHighlightAction_; }

    //* toggle wrap mode
    QAction& wrapModeAction() const
    { return *wrapModeAction_; }

    //* toggle tab emulation
    QAction& tabEmulationAction() const
    { return *tabEmulationAction_; }

    //* show line numbers
    bool hasLineNumberAction() const
    { return showLineNumberAction_; }

    //* show line numbers
    QAction& showLineNumberAction() const
    { return *showLineNumberAction_; }

    //* copy link
    virtual QAction& copyLinkAction() const final
    { return *copyLinkAction_; }

    //@}

    //* container class for embedded Find dialog
    class BASE_QT_EXPORT Container: public QWidget, private Base::Counter<Container>
    {

        public:

        //* constructor
        explicit Container( QWidget* = nullptr );

        //* constructor
        explicit Container( QWidget*, TextEditor* );

        //*@name accessors
        TextEditor& editor() const
        { return *editor_; }

        private:

        //* initialize
        void _initialize();

        //* contained editor
        TextEditor* editor_ = nullptr;

    };

    //* cut
    virtual void cut();

    //* copy
    virtual void copy();

    //* paste
    virtual void paste();

    //* paste
    virtual void pasteUnformated();

    //* changes selection to uppercase
    virtual void upperCase();

    //* changes selection to lower case
    virtual void lowerCase();

    //* find next occurence of TextSelection
    virtual void find( const TextSelection& );

    //* find current selection forward
    virtual void findSelectionForward();

    //* find current selection backward
    virtual void findSelectionBackward();

    //* find last search forward
    virtual void findAgainForward();

    //* find last search forward
    virtual void findAgainBackward();

    //* find next occurence of TextSelection
    virtual void replace( const TextSelection &selection );

    //* replace selection in range
    inline virtual int replaceInSelection( const TextSelection &selection ) { return replaceInSelection( selection, true ); }
    virtual int replaceInSelection( const TextSelection &selection, bool showDialog );

    //* replace selection in window, returns number of replacements
    inline virtual int replaceInWindow( const TextSelection &selection ) { return replaceInWindow( selection, true ); }
    virtual int replaceInWindow( const TextSelection &selection, bool showDialog );

    //* replace again forward
    virtual void replaceAgainForward();

    //* replace again forward
    virtual void replaceAgainBackward();

    //* select line by number
    virtual void selectLine( int index );

    //* remove current line
    virtual void removeLine();

    //* clear
    virtual void clear();

    Q_SIGNALS:

    //* busy signal
    void busy( int );

    //* progress
    void progressAvailable( int );

    //* idle
    void idle();

    //* emitted when selection could not be found
    void noMatchFound();

    //* emitted when selection could be found
    void matchFound();

    //* emitted when selected line is not found
    void lineNotFound();

    //* emitted when selected line is found
    void lineFound();

    //* emitted when recieve focus
    void hasFocus( TextEditor* );

    //* overwrite mode changed
    void modifiersChanged( TextEditor::Modifiers );

    //* anchor (html links) has been clicked
    void linkActivated( QString );

    //* anchor (html links) has been clicked
    void linkHovered( QString );

    protected:

    //*@name event handlers
    //@{

    //* generic event
    bool event( QEvent* ) override;

    //* enter event
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void enterEvent( QEvent* ) override;
    #else
    void enterEvent( QEnterEvent* ) override;
    #endif

    //* mouse release event [overloaded]
    void mousePressEvent( QMouseEvent* ) override;

    //* mouse double click event [overloaded]
    /**
    for left button, double click events are handled
    essentially like single click events, because the number of clicks
    is handled independently by the MultipleClickCounter object
    */
    void mouseDoubleClickEvent( QMouseEvent* ) override;

    //* mouse press event [overloaded]
    void mouseMoveEvent( QMouseEvent* ) override;

    //* mouse press event [overloaded]
    void mouseReleaseEvent( QMouseEvent* ) override;

    //* wheel event
    void wheelEvent( QWheelEvent* ) override;

    //* drop event
    void dropEvent( QDropEvent* event ) override;

    //* keypress event
    void keyPressEvent( QKeyEvent* ) override;

    //* focus event [overloaded]
    void focusInEvent( QFocusEvent* ) override;

    //* context menu event [overloaded]
    void contextMenuEvent( QContextMenuEvent* ) override;

    //* resize event
    /* need to update LineNumberWidget, when wrap is enabled */
    void resizeEvent( QResizeEvent* ) override;

    //* repaint event
    void paintEvent( QPaintEvent* ) override;

    //* timer event
    void timerEvent( QTimerEvent* ) override;

    //* change event
    void changeEvent( QEvent* ) override;

    //@}

    //* scroll
    void scrollContentsBy( int dx, int dy ) override;

    //* context menu position
    const QPoint& _contextMenuPosition() const
    { return contextMenuPosition_; }

    //*@name find/replace selection
    //@{

    //* find dialog
    virtual void _createFindDialog();

    //* replace dialog
    virtual void _createReplaceDialog();

    //* select line dialog
    virtual void _createSelectLineDialog();

    //* progress dialog
    virtual void _createProgressDialog();

    //* find selection in forward direction
    virtual bool _findForward( const TextSelection& selection, bool rewind );

    //* find selection in backward direction
    virtual bool _findBackward( const TextSelection& selection, bool rewind );

    //* define how cursor should be updated while replacing
    enum class CursorMode
    {
        Expand,
        Move
    };

    //* replace selection in range refered to by cursor
    virtual int _replaceInRange( const TextSelection& selection, QTextCursor& cursor, CursorMode mode );

    //@}

    //*@name box selection
    //@{

    //* box selection
    BoxSelection& _boxSelection()
    { return boxSelection_; }

    //* box selection
    const BoxSelection& _boxSelection() const
    { return boxSelection_; }

    //* synchronize box selection with associated displays
    void _synchronizeBoxSelection() const;

    //@}

    //*@name line numbers
    //@{

    //* left margin
    int _leftMargin() const
    { return leftMargin_; }

    //* left margin
    bool _setLeftMargin( int margin );

    //@}

    //* toggle insertion mode
    virtual void _toggleOverwriteMode();

    //*@name tab emulation
    //@{

    //* set tab character size
    /** returns true if changed */
    virtual bool _setTabSize( int size );

    //* tab emulation
    virtual bool _hasTabEmulation() const
    { return hasTabEmulation_; }

    //* insert (normal or emulated) tab
    virtual void _insertTab();

    //* multi tab regular expression
    virtual const QRegularExpression& _tabRegExp() const
    { return tabRegexp_; }

    //* 'normal' tab regular expression
    virtual const QRegularExpression& _normalTabRegExp() const
    { return normalTabRegexp_; }

    //* 'emulated' tab regular expression
    virtual const QRegularExpression& _emulatedTabRegExp() const
    { return emulatedTabRegexp_; }

    //@}

    //*@name margin
    //*@{

    //* margin widget
    TextEditorMarginWidget& _marginWidget() const
    { return *marginWidget_; }

    //* update margins
    virtual bool _updateMargin();

    //* current block rect
    virtual const QRect& _currentBlockRect() const
    { return currentBlockRect_; }

    //* current block rect
    virtual bool _setCurrentBlockRect( QRect rect )
    {
        if( rect == currentBlockRect_ ) return false;
        currentBlockRect_ = std::move( rect );
        return true;
    }

    //@}

    //*@name line numbers
    //@{

    //* line number display
    bool _hasLineNumberDisplay() const
    { return lineNumberDisplay_; }

    //* line number display
    LineNumberDisplay& _lineNumberDisplay() const
    { return *lineNumberDisplay_; }

    //@}

    //* modifiers
    bool _setModifier( TextEditor::Modifier key, bool value )
    {
        if( modifier( key ) == value ) return false;
        if( value ) modifiers_ |= key;
        else modifiers_ &= ~static_cast<Modifiers>(key);
        return true;
    }

    //* synchronize selection
    virtual void _synchronizeSelection();

    //* update action status
    virtual void _updateContentActions();

    //* update action status
    virtual void _updateReadOnlyActions( bool );

    //* update undo/redo actions
    virtual void _updateUndoRedoActions();

    //* update action status
    /** this works only if you have qt > 4.2 */
    virtual void _updateSelectionActions( bool );

    //* update action status
    virtual void _updateClipboardActions( QClipboard::Mode );

    //* update selection clipboard
    virtual void _updateClipboard();

    //* copy anchor to clipboard
    virtual void _copyLinkLocation();

    //* update paste action
    /** depends on clipboard status and editability */
    virtual void _updatePasteAction();

    //* toggle block highlight
    virtual void _toggleBlockHighlight( bool );

    //* wrap mode
    /** returns true if changed */
    virtual bool _toggleWrapMode( bool );

    //* tab emulation
    /** returns true if changed */
    virtual bool _toggleTabEmulation( bool );

    //* toggle line number display
    virtual void _toggleShowLineNumbers( bool state );

    //* block count changed
    /** needed to adjust width of line number display */
    void _blockCountChanged( int );

    //* find text from dialog
    virtual void _findFromDialog();

    //* replace text from dialog
    virtual void _replaceFromDialog();

    //* select line from dialog
    virtual void _selectLineFromDialog();

    //* update replace in selection action
    void _updateReplaceInSelection();

    private:

    //* update configuration
    void _updateConfiguration();

    //* increment font size
    void _incrementFontSize( int = 1 );

    //* decrement font size
    void _decrementFontSize()
    { _incrementFontSize( -1 ); }

    //* restore default font
    void _restoreDefaultFont();

    //* install default actions
    void _installActions();

    //* margin widget
    TextEditorMarginWidget* marginWidget_ = nullptr;

    //* current block rect
    /** needed for block highlighting in margin */
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

    //* set to false when the wrapping must not get loaded via the options
    bool wrapFromOptions_ = true;

    //* set to false when the display of line numbers must not get loaded via the options.
    bool lineNumberFromOptions_ = true;

    //* set to false when block highlighting should not be set from options
    bool highlightBlockFromOptions_ = true;

    //* true if should track anchors (html links)
    bool trackAnchors_ = false;

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
    QRegularExpression emulatedTabRegexp_;

    //* normale tab regular expression
    QRegularExpression normalTabRegexp_;

    //* multi tab regExp
    QRegularExpression tabRegexp_;

    //* min font size
    int minimumFontSize_ = 4;

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

    //* paste clipboard
    QAction* pasteUnformatedAction_ = nullptr;

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

    //* copy link
    QAction* copyLinkAction_ = nullptr;

    //* increment font size
    QAction* incrementFontSizeAction_ = nullptr;

    //* decrement font size
    QAction* decrementFontSizeAction_ = nullptr;

    //* restore default font size
    QAction* restoreDefaultFontAction_ = nullptr;

    //@}

    //* synchronization flag
    bool synchronize_ = false;

    //* true if use dialog for finding
    bool useEmbeddedWidgets_ = false;

    //* wheel offset accumulated
    double wheelOffsetAccumulated_ = 0;

    //* box selection
    BoxSelection boxSelection_;

    //* cursor monitor
    CursorMonitor cursorMonitor_;

    //* remove line buffer
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
    /** this is a bitwise or of the Modifiers enumeration */
    Modifiers modifiers_ = Modifier::None;

    friend BASE_QT_EXPORT class Container;

};

#endif
