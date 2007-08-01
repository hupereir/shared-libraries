#ifndef CustomTextEdit_h
#define CustomTextEdit_h

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
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
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

#include <vector>

#include "Counter.h"
#include "Debug.h"
#include "Key.h"
#include "MultipleClickCounter.h"
#include "RemoveLineBuffer.h"
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
    
  //! enable/disable highlight
  virtual void setHighlightEnabled( const bool& );
  
  //! retrieve current text position
  virtual TextPosition textPosition() const;
      
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
   
  //! clone (and synchronize) text editor
  virtual void synchronize( CustomTextEdit& editor );

  //! synchronization flag
  const bool& synchronize( void ) const
  { return synchronize_; }
  
  //! synchronization flag
  virtual void setSynchronize( const bool& value )
  { synchronize_ = value; }
  
  //! popup dialog with the number of replacement performed
  virtual void showReplacements( const unsigned int& counts );
  
  signals:
  
  //! emmitted when selection could not be found
  void noMatchFound( void );
  
  //! emmitted when selection could be found
  void matchFound( void );
    
  public slots:
 
  //! update configuration
  virtual void updateConfiguration( void );
  
  //! wrap mode
  virtual void toggleWrapMode( bool );
  
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
  
  protected slots:
  
  //! highlight current block
  virtual void _highlightCurrentBlock( void );
  
  //! synchronize selection
  virtual void _synchronizeSelection( void );
  
  //! update action status
  virtual void _updateSelectionActions( bool );
  
  //! update paste action 
  /*! depends on clipboard status and editability */
  virtual void _updatePasteAction( void );
  
  protected:
  
  //!@name event handlers
  //@{
  
  //! mouse release event [overloaded]
  virtual void mousePressEvent( QMouseEvent* );
  
  //! mouse press event [overloaded]
  virtual void mouseReleaseEvent( QMouseEvent* );
  
  //! mouse double click event [overloaded]
  /*! 
    for left button, double click events are handled 
    essentially like single click events, because the number of clicks
    is handled independently by the MultipleClickCounter object
  */
  virtual void mouseDoubleClickEvent( QMouseEvent* );
  
  //! keypress event
  virtual void keyPressEvent( QKeyEvent* );
  
  //! context menu event [overloaded]
  virtual void contextMenuEvent( QContextMenuEvent* );
  
  //@}
  
  //! install default actions
  virtual void _installActions( void );
  
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

  //! replace selection in range refered to by cursor
  virtual unsigned int _replaceInRange( const TextSelection& selection, const QTextCursor& cursor );
  
  //@}
  
  //! clear highlighted block
  virtual void _clearHighlightedBlock( void );
  
  //! toggle insertion mode
  virtual void _toggleInsertMode( void );

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
  
  //! synchronization flag
  bool synchronize_;
  
  //!@name paragraph highlighting
  //@{
  
  //! current block highlighting is enabled
  bool highlight_enabled_;
  
  //! current block highlighting is available
  bool highlight_available_;
  
  //! current block highlighting format
  QTextBlockFormat highlight_format_;

  //! default block format
  QTextBlockFormat default_format_;
  
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
  
  //@}
  
  //! remove_line buffer
  RemoveLineBuffer remove_line_buffer_;
  
  //! multiple click counter
  MultipleClickCounter click_counter_;
      
};

#endif
