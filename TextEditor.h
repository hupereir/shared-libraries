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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software , Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
  \file TextEditor.h
  \brief Customized QTextEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>
#include <QAction>
#include <QBasicTimer>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QFocusEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTextEdit>
#include <QTimerEvent>

#include <vector>

#include "BlockHighlight.h"
#include "BoxSelection.h"
#include "Counter.h"
#include "Debug.h"
#include "Key.h"
#include "MultipleClickCounter.h"
#include "RemoveLineBuffer.h"
#include "Str.h"
#include "TextBlockData.h"
#include "TextPosition.h"
#include "TextSelection.h"

class BaseFindDialog;
class BaseReplaceDialog;
class SelectLineDialog;

class LineNumberDisplay;

//! Customized QTextEdit object
class TextEditor: public QTextEdit, public BASE::Key, public Counter
{
  
  Q_OBJECT
  
  public:
  
  //! constructor
  TextEditor( QWidget* parent = 0 );
  
  //! destrutor
  virtual ~TextEditor( void );
    
  //! retrieve number of blocks in document
  virtual int blockCount( void ) const;
  
  //! retrieve number of blocks associated to argument
  /*! the default implementation returns 1. In QEdit, it is reimplemented accounts for collapsed blocks */
  virtual int blockCount( const QTextBlock& ) const
  { return 1; }
  
  //! retrieve current text position
  virtual TextPosition textPosition( void );
      
  //! return true if current textCursor is visible
  virtual bool isCursorVisible( void ) const;
  
  //!@name conversions between absolute index and TextPosition
  //@{
  
  //! text position from index
  virtual TextPosition positionFromIndex( const int& index ) const;
  
  //! index from text position
  virtual int indexFromPosition( const TextPosition& index ) const;
  
  //@}
      
  //! set text
  virtual void setPlainText( const QString& );
  
  //! set text
  virtual void setHtml( const QString& );

  //! select word under cursor
  virtual void selectWord( void );
  
  //! select line under cursor
  virtual void selectLine( void );
  
  //! set textChar format
  /*! 
  this overloads the base class method (although the later is not virtual)
  in order to properly handle box selection, if any 
  */
  void mergeCurrentCharFormat( const QTextCharFormat & modifier );
  
  //! clear box selection
  virtual void clearBoxSelection( void )
  {
    if( _boxSelection().state() == BoxSelection::FINISHED )
    { _boxSelection().clear(); }
  }
  
  //!@name synchronization
  //@{
  
  //! synchronization
  virtual void setSynchronized( const bool& value )
  { synchronize_ = value; }
 
  //! synchronization
  virtual const bool& isSynchronized( void ) const
  { return synchronize_; }

  //! clone (and synchronize) text editor
  virtual void synchronize( TextEditor* editor );
      
  //! activity
  virtual bool setActive( const bool& value );

  //! activity
  virtual const bool& isActive( void ) const
  { return active_; }

  //@}
 
  //! popup dialog with the number of replacement performed
  virtual void showReplacements( const unsigned int& counts );
 
  //! last searched selection
  static TextSelection& lastSelection( void );

  //!@name text wrap
  //@{
  
  //! enable/disable reading of text wrapping mode from options
  const bool& wrapFromOptions( void ) const
  { return wrap_from_options_; }
  
  //! enable/disable reading of text wrapping mode from options
  void setWrapFromOptions( const bool& value )
  { wrap_from_options_ = value; }
  
  //! enable/disable reading of line number display from options
  const bool& lineNumbersFromOptions( void ) const
  { return line_number_from_options_; }

  //! enable/disable reading of line number display from options
  void setLineNumbersFromOptions( const bool& value )
  { line_number_from_options_ = value; }

  //@}
  
  //! read-only
  virtual void setReadOnly( bool readonly );
  
  //! reset undo/redo history
  virtual void resetUndoRedoStack( void );
  
  //! put actions in context menu
  virtual void installContextMenuActions( QMenu& menu, const bool& all_actions = true );

  //!@name actions
  //@{
  
  QAction& undoAction( void )
  { return *undo_action_; }
  
  //! redo
  QAction& redoAction( void )
  { return *redo_action_; }
  
  //! cut selection
  QAction& cutAction( void )
  { return *cut_action_; }
  
  //! copy selection
  QAction& copyAction( void )
  { return *copy_action_; }
  
  //! paste clipboard
  QAction& pasteAction( void )
  { return *paste_action_;  }

  //! convert selection to upper case
  QAction& lowerCaseAction( void )
  { return *upper_case_action_; }
  
  //! convert selection to lower case
  QAction& upperCaseAction( void )
  { return *lower_case_action_; }
  
  //! find from dialog
  QAction& findAction( void )
  { return *find_action_; }

  //! find selection again
  QAction& findSelectionAction( void )
  { return *find_selection_action_; }

  //! find again
  QAction& findAgainAction( void )
  { return *find_again_action_; }
  
  //! replace
  QAction& replaceAction( void )
  { return *replace_action_; }

  //! replace again
  QAction& replaceAgainAction( void )
  { return *replace_again_action_; }

  //! goto line number
  QAction& gotoLineAction( void )
  { return *goto_line_action_; } 
 
  //! block highlight action
  QAction& blockHighlightAction( void )
  { return *block_highlight_action_; }
  
  //! toggle wrap mode
  QAction& wrapModeAction( void )
  { return *wrap_mode_action_; }
  
  //! toggle tab emulation
  QAction& tabEmulationAction( void )
  { return *tab_emulation_action_; }
  
  //! show line numbers
  bool hasLineNumberAction( void ) const
  { return show_line_number_action_; }
  
  //! show line numbers
  QAction& showLineNumberAction( void ) const
  { return *show_line_number_action_; }

  //@}

  //!@name tab emulation
  //@{
  
  //! tab character
  virtual const QString& tabCharacter( void ) const
  { return tab_; }
   
  //! tab character
  virtual const QString& normalTabCharacter( void ) const
  { return normal_tab_; }
  
  //! tab character
  virtual const QString& emulatedTabCharacter( void ) const
  { return emulated_tab_; }

  //@}
  
  //! block highlight object
  BlockHighlight& blockHighlight() const
  { return *block_highlight_; }
    
  //!@name line numbers
  //@{
  
  //! line number display
  bool hasLineNumberDisplay( void ) const
  { return line_number_display_; }
  
  //! line number display
  LineNumberDisplay& lineNumberDisplay( void ) const
  { return *line_number_display_; }
  
  //@}
  
  //! changes block background
  virtual void setBackground( QTextBlock, const QColor& );
  
  //! clear block background
  virtual void clearBackground( QTextBlock );

  //! clear all blocks background
  virtual void clearAllBackgrounds( void );

  //!@name widget to viewport translations
  //@{ 
 
  //! scrollbar position
  QPoint scrollbarPosition( void ) const
  { return QPoint(  horizontalScrollBar()->value(), verticalScrollBar()->value() ); }

  //! widget to viewport translation
  QRect toViewport( const QRect& rect ) const
  { return rect.translated( -scrollbarPosition() ); }
  
  //! widget to viewport translation
  QPoint toViewport( const QPoint& point ) const
  { return point - scrollbarPosition(); }
  
  //! widget from viewport translation
  QRect fromViewport( const QRect& rect ) const
  { return rect.translated( scrollbarPosition() ); }
  
  //! widget from viewport translation
  QPoint fromViewport( const QPoint& point ) const
  { return point + scrollbarPosition(); }
  
  //@}
  
  signals:
  
  //! busy signal
  void busy( int );
  
  //! progress
  void progressAvailable( int );
  
  //! idle
  void idle( void );

  //! emmitted when selection could not be found
  void noMatchFound( void );
  
  //! emmitted when selection could be found
  void matchFound( void );
  
  //! emmited when recieve focus
  void hasFocus( TextEditor* );

  //! overwrite mode changed
  void overwriteModeChanged();

  public slots:
 
  //! cut
  virtual void cut( void );
  
  //! copy
  virtual void copy( void );
  
  //! paste
  virtual void paste( void );
   
  //! changes selection to uppercase
  virtual void upperCase( void );
  
  //! changes selection to lower case
  virtual void lowerCase( void );
    
  //! find next occurence of TextSelection
  virtual void find( TextSelection selection )
  { 
    Debug::Throw( "TextEditor::find.\n" );
    bool found( selection.flag( TextSelection::BACKWARD ) ? _findBackward( selection, true ):_findForward( selection, true ) ); 
    if( found ) emit matchFound();
    else emit noMatchFound();
  }
  
  //! find current selection forward
  virtual void findSelectionForward( void )
  { 
    Debug::Throw( "TextEditor::findSelectionForward.\n" );
    _findForward( _selection(), true ); 
  }

  //! find current selection backward
  virtual void findSelectionBackward( void )
  { 
    Debug::Throw( "TextEditor::findSelectionBackward.\n" );
    _findBackward( _selection(), true ); 
  }
  
  //! find last search forward
  virtual void findAgainForward( void )
  { 
    Debug::Throw( "TextEditor::findAgainForward.\n" );
    _findForward( lastSelection(), true ); 
  }

  //! find last search forward
  virtual void findAgainBackward( void )
  { 
    Debug::Throw( "TextEditor::findAgainBackward.\n" );
    _findBackward( lastSelection(), true ); 
  }
  
  //! find next occurence of TextSelection
  virtual void replace( TextSelection selection );
  
  //! replace selection in range
  virtual unsigned int replaceInSelection( TextSelection selection, const bool& show_dialog = true );

  //! replace selection in window, returns number of replacements
  virtual unsigned int replaceInWindow( TextSelection selection, const bool& show_dialog = true );
  
  //! replace again forward
  virtual void replaceAgainForward( void )
  {
    Debug::Throw( "TextEditor::replaceAgainForward.\n" );
    TextSelection selection( lastSelection() );
    selection.setFlag( TextSelection::BACKWARD, false );
    replace( selection );
  }

  //! replace again forward
  virtual void replaceAgainBackward( void )
  {
    Debug::Throw( "TextEditor::replaceAgainBackward.\n" );
    TextSelection selection( lastSelection() );
    selection.setFlag( TextSelection::BACKWARD, true );
    replace( selection );
  }
    
  //! select line by number
  virtual void selectLine( int index ); 
    
  //! remove current line
  virtual void removeLine( void );
  
  //! clear
  virtual void clear( void );
  
  protected:
  
  //!@name event handlers
  //@{
  
  //! generic event
  virtual bool event( QEvent* );

  //! enter event handler
  void enterEvent( QEvent *event );

  //! mouse release event [overloaded]
  virtual void mousePressEvent( QMouseEvent* );
  
  //! mouse double click event [overloaded]
  /*! 
    for left button, double click events are handled 
    essentially like single click events, because the number of clicks
    is handled independently by the MultipleClickCounter object
  */
  virtual void mouseDoubleClickEvent( QMouseEvent* );
  
  //! mouse press event [overloaded]
  virtual void mouseMoveEvent( QMouseEvent* );
 
  //! mouse press event [overloaded]
  virtual void mouseReleaseEvent( QMouseEvent* );
  
  //! drop event
  virtual void dropEvent( QDropEvent* event );

  //! keypress event
  virtual void keyPressEvent( QKeyEvent* );
    
  //! focus event [overloaded]
  virtual void focusInEvent( QFocusEvent* );

  //! context menu event [overloaded]
  virtual void contextMenuEvent( QContextMenuEvent* );
  
  //! resize event
  /* need to update LineNumberWidget, when wrap is enabled */
  virtual void resizeEvent( QResizeEvent* );
  
  //! repaint event
  virtual void paintEvent( QPaintEvent* );

  //! timer event
  virtual void timerEvent( QTimerEvent* event );
  
  //@}
  
  //! install default actions
  virtual void _installActions( void );
    
  //!@name find/replace selection
  //@{
  
  //! create TextSelection object from this selection, or clipboard
  TextSelection _selection( void ) const;
 
  //! last searched selection
  static void _setLastSelection( const TextSelection& selection )
  { lastSelection() = selection; }
  
  //! find dialog
  virtual BaseFindDialog& _findDialog( void )
  {
    assert( find_dialog_ );
    return *find_dialog_;
  }
  
  //! find dialog
  virtual void _createBaseFindDialog( void );
  
  //! find selection in forward direction
  virtual bool _findForward( const TextSelection& selection, const bool& rewind );

  //! find selection in backward direction
  virtual bool _findBackward( const TextSelection& selection, const bool& rewind );
  
  //! replace dialog
  virtual BaseReplaceDialog& _replaceDialog( void )
  {
    assert( replace_dialog_ );
    return *replace_dialog_;
  }
  
  //! find dialog
  virtual void _createBaseReplaceDialog( void );

  //! define how cursor should be updated while replacing
  enum CursorMode
  { 
    
    //! cursor selection range is expanded
    EXPAND,
    
    //! cursor position is moved
    MOVE
    
  };
  
  //! replace selection in range refered to by cursor
  virtual unsigned int _replaceInRange( const TextSelection& selection, QTextCursor& cursor, CursorMode mode, bool show_progress );
  
  //@}

  //!@name box selection
  //@{
  
  //! box selection
  BoxSelection& _boxSelection( void )
  { return box_selection_; }

  //! box selection
  const BoxSelection& _boxSelection( void ) const
  { return box_selection_; }
  
  //! synchronize box selection with associated displays
  void _synchronizeBoxSelection() const;
  
  //@}
  
  //!@name line numbers
  //@{
    
  //! left margin
  const int& _leftMargin( void ) const
  { return left_margin_; }
  
  //! left margin
  bool _setLeftMargin( const int& margin );
  
  //! vertical line display
  const bool& _drawVerticalLine( void ) const
  { return draw_vertical_line_; }
  
  //! vertical line display
  bool _setDrawVerticalLine( const bool& value )
  { 
    if( value == _drawVerticalLine() ) return false;
    draw_vertical_line_ = value; 
    return true;
  }
  
  //! margin foreground
  const QColor& _marginForegroundColor( void ) const
  { return margin_foreground_color_; }

  //! margin foreground
  void _setMarginForegroundColor( const QColor& color )
  { margin_foreground_color_ = color; }
  
  //! margin background
  const QColor& _marginBackgroundColor( void ) const
  { return margin_background_color_; }

  //! margin background
  void _setMarginBackgroundColor( const QColor& color )
  { margin_background_color_ = color; }

  //@}
  
  //! toggle insertion mode
  virtual void _toggleOverwriteMode( void );
  
  //!@name tab emulation
  //@{

  //! set tab character size
  /*! returns true if changed */
  virtual bool _setTabSize( const int& size );  
  
  //! tab emulation
  virtual const bool& _hasTabEmulation( void ) const
  { return has_tab_emulation_; }
  
  //! insert (normal or emulated) tab
  virtual void _insertTab( void );
  
  //! multi tab regular expression
  virtual const QRegExp& _tabRegExp( void ) const
  { return tab_regexp_; }  

  //! 'normal' tab regular expression
  virtual const QRegExp& _normalTabRegExp( void ) const 
  { return normal_tab_regexp_; }
  
  //! 'emulated' tab regular expression
  virtual const QRegExp& _emulatedTabRegExp( void ) const
  { return emulated_tab_regexp_; }
  
  //@}

  //! update margins
  virtual bool _updateMargins( void );
    
  //! draw margins
  virtual void _drawMargins( QPainter& );
  
  private slots:
  
  //! update configuration
  void _updateConfiguration( void );
  
  protected slots:
    
  //! synchronize selection
  virtual void _synchronizeSelection( void );
 
  //! update action status
  virtual void _updateReadOnlyActions( bool );

  //! update action status
  /*! this works only if you have qt > 4.2 */
  virtual void _updateSelectionActions( bool );
  
  //! update action status
  virtual void _updateClipboardActions( QClipboard::Mode );
  
  //! update selection clipboard
  virtual void _updateClipboard( void );
  
  //! update paste action 
  /*! depends on clipboard status and editability */
  virtual void _updatePasteAction( void );

  //! toggle block highlight
  virtual void _toggleBlockHighlight( bool );
  
  //! wrap mode
  /*! returns true if changed */
  virtual bool _toggleWrapMode( bool );

  //! tab emulation
  /*! returns true if changed */
  virtual bool _toggleTabEmulation( bool );

  //! toggle line number display
  virtual void _toggleShowLineNumbers( bool state );
  
  //! block count changed
  /*! needed to adjust width of line number display */
  void _blockCountChanged( int );

  //! find text from dialog
  virtual void _findFromDialog( void );
  
  //! replace text from dialog
  virtual void _replaceFromDialog( void );

  //! select line from dialog
  virtual void _selectLineFromDialog( void );

  private:
    
  //! true if rect is different from argument, and update
  bool _rectChanged( const QRect& rect )
  { 
    if( rect_ == rect ) return false;
    rect_ = rect;
    return true;
  }
  
  //!@name replace/find selection
  ///@{
  
  //! find dialog
  BaseFindDialog* find_dialog_;

  //! find dialog
  BaseReplaceDialog* replace_dialog_;
  
  //! line number dialog
  SelectLineDialog* select_line_dialog_;
  
  //@}
  
  //!@name line number display
  //@{
       
  //! line numbers
  LineNumberDisplay* line_number_display_;
  
  //! left margin width
  int left_margin_;

  //! vertical line
  bool draw_vertical_line_;

  //@}
  
  //! true if this display is the active display
  bool active_;
  
  /*! set to false when the wrapping must not get loaded via the options */
  bool wrap_from_options_;
  
  /*! set to false when the display of line numbers must not get loaded via the options. */
  bool line_number_from_options_;

  //!@name tab emulation and empty lines
  //@{
  
  //! tab emulation flag
  bool has_tab_emulation_;
  
  //! tab string
  QString emulated_tab_;

  //! tab string
  QString normal_tab_;
  
  //! current tab string
  QString tab_;
  
  //! emulated tab regular expression
  QRegExp emulated_tab_regexp_;
  
  //! normale tab regular expression
  QRegExp normal_tab_regexp_;
  
  //! multi tab regExp
  QRegExp tab_regexp_;

  //@}

  //!@name default actions
  //@{
  
  //! undo
  QAction* undo_action_;
  
  //! redo
  QAction* redo_action_;
  
  //! cut selection
  QAction* cut_action_;
  
  //! copy selection
  QAction* copy_action_;
  
  //! paste clipboard
  QAction* paste_action_;

  //! clear document
  QAction* clear_action_;
  
  //! select all document
  QAction* select_all_action_;

  //! convert selection to upper case
  QAction* upper_case_action_;
  
  //! convert selection to lower case
  QAction* lower_case_action_;
  
  //! find from dialog
  QAction* find_action_;

  //! find selection again
  QAction* find_selection_action_;

  //! find selection backward
  QAction* find_selection_backward_action_;

  //! find again
  QAction* find_again_action_;
  
  //! find again backward
  QAction* find_again_backward_action_;

  //! replace
  QAction* replace_action_;

  //! replace again
  QAction* replace_again_action_;

  //! replace again backward
  QAction* replace_again_backward_action_;
    
  //! goto line number
  QAction* goto_line_action_;

  //! toggle highlight block
  QAction* block_highlight_action_;
  
  //! toggle wrap mode
  QAction* wrap_mode_action_;
  
  //! toggle tab emulation
  QAction* tab_emulation_action_;
  
  //! line number
  QAction* show_line_number_action_;
  
  //@}
  
  //! synchronization flag
  bool synchronize_;
  
  //! box selection
  BoxSelection box_selection_;
  
  //! remove_line buffer
  RemoveLineBuffer remove_line_buffer_;
  
  //! multiple click counter
  MultipleClickCounter click_counter_;

  //! current block highlight
  BlockHighlight* block_highlight_;
  
  //! current block highlight color
  QColor highlight_color_;

  //! margin color
  QColor margin_foreground_color_;
  
  //! margin color
  QColor margin_background_color_;

  //! store rect of last update
  QRect rect_;
  
  //! store possible mouse drag start position
  QPoint drag_start_;
  
  //! box selection timer
  /*! it mimics QTextEdit autoscroll timer to update box selection */
  QBasicTimer box_selection_timer_;
  
};

#endif
