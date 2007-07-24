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
  \brief Customized QTextDocument object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QContextMenuEvent>
#include <QShortcut>
#include <QTextEdit>

#include <vector>

#include "Counter.h"
#include "Debug.h"
#include "Key.h"
#include "MultipleClickCounter.h"

//! Customized QTextEdit object
class CustomTextEdit: public QTextEdit, public BASE::Key, public Counter
{
  
  Q_OBJECT
  
  public:
  
  //! constructor
  CustomTextEdit( QWidget* parent );
  
  //! destrutor
  virtual ~CustomTextEdit( void );
  
  //! enable/disable shortcuts
  void enableShortCuts( const bool& value );
  
  public slots:
 
  //! changes selection to uppercase
  virtual void upperCase( void );
  
  //! changes selection to lower case
  virtual void lowerCase( void );
  
  //! select line from dialog
  virtual void selectLineFromDialog( void );
  
  //! select line by number
  virtual void selectLine( int index ); 
  
  protected:
  
  //! context menu event
  virtual void contextMenuEvent( QContextMenuEvent* );
  
  //!@name shortcuts management
  //@{
  
  //! install default shortcuts
  virtual void _installShortcuts( void );

  //! add shortcut
  virtual QShortcut* _addShortcut( const QKeySequence& sequence, QObject* reciever = 0, const std::string& slot = std::string() );
    
  //! clear shortcuts
  virtual void _clearShortcuts( void );
  
  //@}
  
  private:
  
  //! multiple click counter
  MultipleClickCounter click_counter_;
  
  //! shortcuts
  std::vector<QShortcut*> shortcuts_;
    
};

#endif
