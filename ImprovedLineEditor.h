#ifndef _ImprovedLineEditor_h_
#define _ImprovedLineEditor_h_

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
  \file ImprovedLineEditor.h
  \brief customized QLineEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>
#include <QFrame>
#include <QString>
#include <QPaintEvent>
#include <QAbstractButton>

#include "Counter.h"
#include "LineEditor.h"

/*!
   \class ImprovedLineEditor
   \brief customized line edit to have faster popup menu
*/
class ImprovedLineEditor: public QFrame, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT

  public:
  
  //! constructor;
  ImprovedLineEditor( QWidget* parent );

  LineEditor& editor( void ) const
  { 
    assert( editor_ );
    return *editor_;
  }

  QString text( void ) const
  { return editor().text(); }

  //! modification
  bool isModified( void ) const
  { return editor().isModified(); }

  //! history
  bool isUndoAvailable( void ) const
  { return editor().isUndoAvailable(); }  

  //! history
  bool isRedoAvailable( void ) const
  { return editor().isRedoAvailable(); }  
  
  //! read-only
  void setReadOnly( const bool& value ) const
  { editor().setReadOnly( value ); }
  
  //! set modified
  void setModified( const bool& value ) const
  { editor().setModified( value ); }
  
  //! cursor position
  void setCursorPosition( int value ) const
  { editor().setCursorPosition( value ); }
  
  signals:
  
  //! return pressed
  void returnPressed();

  //! text changed
  void textChanged( const QString& );
  
  //! signal emmited when the cursor changes position
  void cursorPositionChanged( int );  
  
  //! signal emmited when the cursor changes position
  void cursorPositionChanged( int, int );  

  //! modification state changed
  void modificationChanged( bool );
  
  public slots:
  
  //! select all
  void selectAll( void )
  { editor().selectAll(); }
  
  //! set text
  void setText( const QString& value )
  { editor().setText( value ); }
  
  //! undo
  void undo( void )
  { editor().undo(); }
  
  //! undo
  void redo( void )
  { editor().redo(); }

  protected:
  
  //! paint
  void paintEvent( QPaintEvent* );
  
  //! button
  class Button: public QAbstractButton
  {
    
    public: 
    
    //! constructor
    Button( QWidget* parent ):
      QAbstractButton( parent )
      {}
    
    protected:
    
    //! paint
    void paintEvent( QPaintEvent* );
    
  };
  
  // clear button
  Button& _clearButton() const
  { 
    assert( clear_button_ );
    return *clear_button_;
  }
  
  protected slots:
  
  //! show hide clear button depending on text length
  void _textChanged( const QString& );
  
  private:
  
  //! editor
  LineEditor* editor_;
  
  //! clear button
  Button* clear_button_;
  
};

#endif
