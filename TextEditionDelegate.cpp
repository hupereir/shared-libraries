
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
  \file    TextEditionDelegate.cpp
  \brief   treeView item delegate for edition
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include <QAbstractItemModel>

#include "AnimatedLineEditor.h"
#include "TextEditionDelegate.h"
#include "Debug.h"

using namespace std;

//______________________________________________________________
TextEditionDelegate::TextEditionDelegate( QObject *parent ):
#if QT_VERSION >= 0x040400
  QStyledItemDelegate( parent ),
#else
  QItemDelegate( parent ),
#endif
  Counter( "TextEditionDelegate" )
{ Debug::Throw( "TextEditionDelegate::TextEditionDelegate.\n" ); }

//______________________________________________________________
QWidget* TextEditionDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
  Debug::Throw( "TextEditionDelegate::createEditor.\n" );
  AnimatedLineEditor *editor = new AnimatedLineEditor( parent );
  editor->setFrame( false );
  return editor;
}

//______________________________________________________________
void TextEditionDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  Debug::Throw( "TextEditionDelegate::setEditorData.\n" );
  QString text = index.model()->data(index, Qt::DisplayRole).toString();
  AnimatedLineEditor *line_editor = static_cast<AnimatedLineEditor*>(editor);
  line_editor->setText( text );
}

//______________________________________________________________
void TextEditionDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  Debug::Throw( "TextEditionDelegate::setModelData.\n" );
  AnimatedLineEditor *line_editor = static_cast<AnimatedLineEditor*>(editor);
  QString value( line_editor->text() );
  model->setData( index, value, Qt::EditRole);
}

//______________________________________________________________
void TextEditionDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
  Debug::Throw( "TextEditionDelegate::updateEditorGeometry.\n" );
  editor->setGeometry(option.rect);
}
