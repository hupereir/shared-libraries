#ifndef CustomTextEdit_h
#define CustomTextEdit_h

// $Id$

/******************************************************************************
*
* Copyright (C) 2002 Hugo PEREIRA <mailto: hugo.pereira@free.fr>
*
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; eitherFoundation version 2 of the License, or (at your option) any later
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
  \file CustomTextEdit.h
  \brief Customized QTextEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QClipboard>
#include <QContextMenuEvent>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTextEdit>
#include <QDragMoveEvent>

#include <vector>

#include "BaseTextHighlight.h"
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

class SelectLineDialog;
class FindDialog;
class ReplaceDialog;

//! Customized QTextEdit object
class CustomTextEdit: public QTextEdit, public BASE::Key, public Counter
{
  
  Q_OBJECT
  
  public:
  
  //! constructor
  CustomTextEdit( QWidget* parent );
  
  //! destrutor
  virtual ~CustomTextEdit( void );
    
  //! retrieve number of blocks in document
  int blockCount( void ) const;
  
  //! retrieve current text position
  virtual TextPosition textPosition();
      
  //!@name conversions between absolute index and TextPosition
  //@{
  
  //! text position from index
  virtual TextPosition positionFromIndex( const int& index ) const;
  
  //! index from text position
  virtual int indexFromPosition( const TextPosition& index ) const;
  
  //@}
      
  //! select word under cursor
  virtual void selectWord( void );
  
  //! select line under cursor
  virtual void selectLine( void );
   
 //!@name synchronization
  //@{
  
  //! synchronization
  virtual void setSynchronized( const bool& value )
  { synchronize_ = value; }
 
  //! synchronization
  virtual const bool& isSynchronized( void ) const
  { return synchronize_; }

  //! clone (and synchronize) text editor
  virtual void synchronize( CustomTextEdit* editor );
  
  //@}
 
  //! popup dialog with the number of replacement performed
  virtual void showReplacements( const unsigned int& counts );

  //!@name text wrap
  //@{
  
  //! enable/disable reading of text wrapping mode from options
  const bool& wrapFromOptions( void ) const
  { return wrap_from_options_; }
  
  //! enable/disable reading of text wrapping mode from options
  void setWrapFromOptions( const bool& value )
  { wrap_from_options_ = value; }
  
  //@}
  
  //! read-only
  virtual void setReadOnly( bool readonly )
  {
    Debug::Throw( "CustomTextEdit::setReadOnly.\n" );
    QTextEdit::setReadOnly( readonly );
    _updateReadOnlyActions( readonly ); 
  }
  
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

  //! text highlight
  /*! it is needed to ensure only one text highlight works at a time */
  void setTextHighlight( BaseTextHighlight* highlight );
    
  //! text highlight
  BaseTextHighlight& textHighlight( void ) const
  {
    BASE::KeySet<BaseTextHighlight> highlights( dynamic_cast<Key*>( document() ) );
    Exception::check( highlights.size() == 1, DESCRIPTION( Str("invalid association to text-highlight - count: " ).append<unsigned int>( highlights.size() ) ) );
    return **highlights.begin();
  }
  
  //! block highlight object
  BlockHighlight& blockHighlight() const
  { return *block_highlight_; }
    
  //! changes block background
  void setBackground( QTextBlock, const QColor& );
  
  //! clear block background
  void clearBackground( QTextBlock );

  //! clear all blocks background
  void clearAllBackgrounds( void );
  
  signals:
  
  //! emmitted when selection could not be found
  void noMatchFound( void );
  
  //! emmitted when selection could be found
  void matchFound( void );
    
  public slots:
  
  //! update configuration
  virtual void updateConfiguration( void );
 
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
  
  //! find text from dialog
  virtual void findFromDialog( void );
  
  //! find next occurence of TextSelection
  virtual void find( TextSelection selection )
  { 
    Debug::Throw( "CustomTextEdit::find.\n" );
    bool found( selection.flag( TextSelection::BACKWARD ) ? _findBackward( selection, true ):_findForward( selection, true ) ); 
    if( found ) emit matchFound();
    else emit noMatchFound();
  }
  
  //! find current selection forward
  virtual void findSelectionForward( void )
  { 
    Debug::Throw( "CustomTextEdit::findSelectionForward.\n" );
    _findForward( _selection(), true ); 
  }

  //! find current selection backward
  virtual void findSelectionBackward( void )
  { 
    Debug::Throw( "CustomTextEdit::findSelectionBackward.\n" );
    _findBackward( _selection(), true ); 
  }
  
  //! find last search forward
  virtual void findAgainForward( void )
  { 
    Debug::Throw( "CustomTextEdit::findAgainForward.\n" );
    _findForward( _lastSelection(), true ); 
  }

  //! find last search forward
  virtual void findAgainBackward( void )
  { 
    Debug::Throw( "CustomTextEdit::findAgainBackward.\n" );
    _findBackward( _lastSelection(), true ); 
  }
  
  //! replace text from dialog
  virtual void replaceFromDialog( void );
  
  //! find next occurence of TextSelection
  virtual void replace( TextSelection selection );
  
  //! replace selection in range
  virtual unsigned int replaceInSelection( TextSelection selection, const bool& show_dialog = true );

  //! replace selection in window, returns number of replacements
  virtual unsigned int replaceInWindow( TextSelection selection, const bool& show_dialog = true );
  
  //! replace again forward
  virtual void replaceAgainForward( void )
  {
    Debug::Throw( "CustomTextEdit::replaceAgainForward.\n" );
    TextSelection selection( _lastSelection() );
    selection.setFlag( TextSelection::BACKWARD, false );
    replace( selection );
  }

  //! replace again forward
  virtual void replaceAgainBackward( void )
  {
    Debug::Throw( "CustomTextEdit::replaceAgainBackward.\n" );
    TextSelection selection( _lastSelection() );
    selection.setFlag( TextSelection::BACKWARD, true );
    replace( selection );
  }
  
  //! select line from dialog
  virtual void selectLineFromDialog( void );
  
  //! select line by number
  virtual void selectLine( int index ); 
    
  //! remove current line
  virtual void removeLine( void );

  protected:
  
  //!@name event handlers
  //@{

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

  //! keypress event
  virtual void keyPressEvent( QKeyEvent* );
  
  //! context menu event [overloaded]
  virtual void contextMenuEvent( QContextMenuEvent* );
    
  //! repaint event
  virtual void paintEvent( QPaintEvent* e );

  //! drag move event [overloaded]
  // void dragMoveEvent( QDragMoveEvent* event );  
  
  //@}
  
  //! install default actions
  virtual void _installActions( void );
  
  //! retrieve context menu. Create it on first call
  void _installContextMenuActions( QMenu& menu );
  
  //!@name find/replace selection
  //@{
  
  //! create TextSelection object from this selection, or clipboard
  TextSelection _selection( void ) const;
  
  //! last searched selection
  static const TextSelection& _lastSelection( void )
  { return last_selection_; }

  //! last searched selection
  static void _setLastSelection( const TextSelection& selection )
  { last_selection_ = selection; }
  
  //! find dialog
  virtual FindDialog& _findDialog( void )
  {
    Exception::checkPointer( find_dialog_, DESCRIPTION( "find_dialog_ not initialized.\n" ) );
    return *find_dialog_;
  }
  
  //! find dialog
  virtual void _createFindDialog( void );
  
  //! find selection in forward direction
  virtual bool _findForward( const TextSelection& selection, const bool& rewind );

  //! find selection in backward direction
  virtual bool _findBackward( const TextSelection& selection, const bool& rewind );
  
  //! replace dialog
  virtual ReplaceDialog& _replaceDialog( void )
  {
    Exception::checkPointer( replace_dialog_, DESCRIPTION( "replace_dialog_ not initialized.\n" ) );
    return *replace_dialog_;
  }
  
  //! find dialog
  virtual void _createReplaceDialog( void );

  //! define how cursor should be updated while replacing
  enum CursorMode
  { 
    
    //! cursor selection range is expanded
    EXPAND,
    
    //! cursor position is moved
    MOVE
    
  };
  
  //! replace selection in range refered to by cursor
  virtual unsigned int _replaceInRange( const TextSelection& selection, QTextCursor& cursor, CursorMode mode );
  
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
  
  //! toggle insertion mode
  virtual void _toggleInsertMode( void );
  
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
  
  protected slots:
    
  //! synchronize selection
  virtual void _synchronizeSelection( void );
 
  //! update action status
  virtual void _updateReadOnlyActions( bool );

  //! update action status
  virtual void _updateSelectionActions( bool );
  
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

  private:
    
  //!@name replace/find selection
  ///@{
  
  //! find dialog
  FindDialog* find_dialog_;

  //! find dialog
  ReplaceDialog* replace_dialog_;
  
  //! line number dialog
  SelectLineDialog* select_line_dialog_;
  
  //! last used selection
  static TextSelection last_selection_;
  
  //@}
  
  //!@name text wrap
  //@{
  
  /*! 
  set to false when the wrapping has been modified once.
  This done, it does not get loaded via the options 
  any more
  */
  bool wrap_from_options_;
  
  //@}
  
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
  
  int offset_;
  
};

#endif
