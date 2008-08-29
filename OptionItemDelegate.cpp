
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
  \file    OptionItemDelegate.cpp
  \brief   Keyword item delegate for edition
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include <QAbstractItemModel>

#include "OptionItemDelegate.h"
#include "LineEditor.h"
#include "Debug.h"

using namespace std;

//______________________________________________________________
OptionItemDelegate::OptionItemDelegate( QObject *parent ):
  QItemDelegate( parent ),
  Counter( "OptionItemDelegate" )
{ Debug::Throw( "OptionItemDelegate::OptionItemDelegate.\n" ); }

//______________________________________________________________
QWidget* OptionItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{ 
  Debug::Throw( "OptionItemDelegate::createEditor.\n" );
  LineEditor *editor = new LineEditor( parent ); 
  editor->setFrame( false );
  return editor;
}

//______________________________________________________________
void OptionItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  Debug::Throw( "OptionItemDelegate::setEditorData.\n" );
  QString text = index.model()->data(index, Qt::DisplayRole).toString();
  LineEditor *line_editor = static_cast<LineEditor*>(editor);
  line_editor->setText( text );
}

//______________________________________________________________
void OptionItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  Debug::Throw( "OptionItemDelegate::setModelData.\n" );
  LineEditor *line_editor = static_cast<LineEditor*>(editor);
  QString value( line_editor->text() );
  model->setData( index, value, Qt::EditRole);
}

//______________________________________________________________
void OptionItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{ 
  Debug::Throw( "OptionItemDelegate::updateEditorGeometry.\n" );
  editor->setGeometry(option.rect);
}
