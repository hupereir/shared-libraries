
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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "TextEditionDelegate.h"
#include "AnimatedLineEditor.h"
#include "Debug.h"

#include <QAbstractItemModel>

//______________________________________________________________
TextEditionDelegate::TextEditionDelegate( QObject *parent ):
    TreeViewItemDelegate( parent ),
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
    AnimatedLineEditor *lineEditor = static_cast<AnimatedLineEditor*>(editor);
    lineEditor->setText( text );
}

//______________________________________________________________
void TextEditionDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    Debug::Throw( "TextEditionDelegate::setModelData.\n" );
    AnimatedLineEditor *lineEditor = static_cast<AnimatedLineEditor*>(editor);
    QString value( lineEditor->text() );
    model->setData( index, value, Qt::EditRole);
}

//______________________________________________________________
void TextEditionDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    Debug::Throw( "TextEditionDelegate::updateEditorGeometry.\n" );
    editor->setGeometry(option.rect);
}
