#ifndef _LineEditor_h_
#define _LineEditor_h_

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
 
/*!
  \file LineEditor.h
  \brief customized QLineEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>

#include <string>
#include <vector>

#include "Counter.h"

/*!
   \class LineEditor
   \brief customized line edit to have faster popup menu
*/
class LineEditor: public QLineEdit, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT

  public:
  
  //! constructor;
  LineEditor( QWidget* parent );
      
  //! set editor as modified
  void setModified( const bool& value );
  
  //! modification state
  const bool& isModified( void ) const
  { return modified_; }
  
  signals:
  
  //! signal emmited when the cursor changes position
  void cursorPositionChanged( int position );  
  
  //! modification state changed
  void modificationChanged( bool );
  
  public slots:
  
  //! set text
  void setTextString( const std::string& value )
  { setText( QString( value.c_str() ) ); }
    
  //! changes selection to uppercase
  void lowerCase( void );
    
  //! changes selection to uppercase
  void upperCase( void );
    
  protected:
  
  //! context menu (overloaded)
  virtual void contextMenuEvent( QContextMenuEvent* );
  
  //! overloaded key press event
  virtual void keyPressEvent( QKeyEvent* );
    
  //! overloaded mouse event handler
  virtual void mouseReleaseEvent( QMouseEvent* );
  
  //! install actions
  virtual void _installActions( void );

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
  
  //! modification state
  bool modified_;
  
  //! backup string to track modifications
  QString backup_; 
  
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
    
  //@}

};

#endif
