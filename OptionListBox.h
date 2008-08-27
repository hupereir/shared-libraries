#ifndef OptionListBox_h
#define OptionListBox_h

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
   \file OptionListBox.h
   \brief ListBox for Special options, and buttons to add/remove values
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QAction>
#include <QFileDialog> 
#include <QPushButton> 

#include <cmath>

#include "OptionWidget.h"
#include "OptionModel.h"
#include "Options.h"

class TreeView;

//! ListBox for Special options, and buttons to add/remove values
class OptionListBox: public QWidget, public OptionWidget
{

  //! Qt meta object declaration
  Q_OBJECT

  public:

  //! constructor
  OptionListBox( QWidget* parent, const std::string& option_name );

  //! read value from option
  void read( void );

  //! write value to option
  void write( void ) const;

  //! browsability
  void setBrowsable( const bool& value )
  { browsable_ = value; }

  //! set file dialog mode
  void setFileMode( const QFileDialog::FileMode& mode )
  { file_mode_ = mode; }
  
  private slots:

  //! update buttons
  void _updateButtons( void );
  
  //! add a value
  void _add( void );

  //! add a value
  void _edit( void );

  //! remove a value
  void _remove();

  //! set value as default
  void _setDefault();

  private:

  //! list
  TreeView& _list( void ) const
  { return *list_; }    
  
  //! if true, use browsable line editor for Add
  bool browsable_;
  
  //! browsable dialog mode
  QFileDialog::FileMode file_mode_;
          
  //! model
  OptionModel model_;
  
  //! value list
  TreeView* list_;
  
  //! edit button
  QPushButton* edit_;
  
  //! remove button
  QPushButton* remove_;
      
  //! remove action
  QAction* remove_action_;

    //! default button
  QPushButton* default_;

};
#endif
