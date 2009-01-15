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
  \file TabbedDialog.h
  \brief base configuration dialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#ifndef _TabbedDialog_h_
#define _TabbedDialog_h_

#include <QCloseEvent>
#include <QDialog>
#include <QLayout>
#include <QStackedWidget>
#include <list>

#include "XmlOptions.h"
#include "BaseDialog.h"
#include "OptionWidget.h"
#include "ListModel.h"

class TreeView;

//! tabbed dialog
/*! a list of tab names appear on the left. The contents of the corresponding tag appear on the right */
class TabbedDialog: public BaseDialog, public Counter
{

  Q_OBJECT

  public:

  //! creator
  TabbedDialog( QWidget *parent );

  //! destructor
  virtual ~TabbedDialog()
  {
    Debug::Throw( "TabbedDialog::~TabbedDialog.\n" );
    return;
  }
  
  //! adds a new Item, returns associated Box
  virtual QWidget& addPage( const QString&, const QString& tooltip = QString(), const bool& expand = false );
  
  protected slots: 
  
  //! display item page
  virtual void _display( const QModelIndex& );

  protected:

  //! retrieve list
  virtual TreeView& _list( void )
  { return *list_; }

  //! retrieve stack
  virtual QStackedWidget& _stack( void )
  { return *stack_; }

  //! button layout
  QHBoxLayout& _buttonLayout( void ) const
  { return *button_layout_; }
  
  private:
  
  //! model
  class Model: public ListModel<QWidget*>
  {
    
    public:
    
    //! number of columns
    enum { n_columns = 1 };
    
    //! column type enumeration
    enum ColumnType {
      NAME
    };
    
    //!@name methods reimplemented from base class
    //@{
    
    //! flags
    virtual Qt::ItemFlags flags(const QModelIndex& ) const
    { return Qt::ItemIsEnabled |  Qt::ItemIsSelectable; }
    
    //! return data
    virtual QVariant data(const QModelIndex &index, int role) const;
    
    //! header data
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
    {   
      if( orientation == Qt::Horizontal && role == Qt::DisplayRole && section >= 0 && section < n_columns )
      { return column_titles_[section]; }
      
      // return empty
      return QVariant(); 
    
    }
    
    //! number of columns for a given index
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
    { return n_columns; }
    
    //@}
  
    protected: 
    
    //! sort
    virtual void _sort( int, Qt::SortOrder order = Qt::AscendingOrder )
    {}
  
    //! list column names
    static const QString column_titles_[n_columns];    
    
  };

  //! model
  Model& _model( void )
  { return model_; }
  
  //! model
  Model model_;
  
  //! Configuration list
  TreeView* list_;

  //! Widget stack
  QStackedWidget* stack_;
  
  //! button layout (needed to add extra buttons)
  QHBoxLayout *button_layout_;

};


#endif
