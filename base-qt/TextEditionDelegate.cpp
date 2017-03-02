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
#include "Debug.h"
#include "TextEditionDelegate_p.h"

#include <QAbstractItemModel>

namespace Private
{

    //______________________________________________________________
    LocalEditor::LocalEditor( QWidget* parent ):
        LineEditor( parent )
    { connect( this, SIGNAL(returnPressed()), SLOT(_validate()) );}

    //______________________________________________________________
    void LocalEditor::_validate( void )
    {
        Debug::Throw( "Private::LocalEditor::_validate.\n" );
        valid_ = true;
    }

};

//______________________________________________________________
TextEditionDelegate::TextEditionDelegate( QObject *parent ):
    TreeViewItemDelegate( parent ),
    Counter( "TextEditionDelegate" )
{ Debug::Throw( "TextEditionDelegate::TextEditionDelegate.\n" ); }

//______________________________________________________________
QWidget* TextEditionDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    Debug::Throw( "TextEditionDelegate::createEditor.\n" );
    auto editor = new Private::LocalEditor( parent );
    editor->setFrame( false );
    return editor;
}

//______________________________________________________________
void TextEditionDelegate::setEditorData(QWidget *widget, const QModelIndex &index) const
{
    Debug::Throw( "TextEditionDelegate::setEditorData.\n" );
    QString text = index.model()->data(index, Qt::DisplayRole).toString();
    auto editor = qobject_cast<Private::LocalEditor*>(widget);
    if( editor ) editor->setText( text );
}

//______________________________________________________________
void TextEditionDelegate::setModelData(QWidget *widget, QAbstractItemModel *model, const QModelIndex &index) const
{
    Debug::Throw( "TextEditionDelegate::setModelData.\n" );
    Private::LocalEditor *editor = qobject_cast<Private::LocalEditor*>(widget);

    // update model only if edition is "valid", meaning that return was pressed
    if( editor && editor->isEditionValid() )
    {
        QString value( editor->text() );
        model->setData( index, value, Qt::EditRole);
    }
}

//______________________________________________________________
void TextEditionDelegate::updateEditorGeometry(QWidget *widget, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    Debug::Throw( "TextEditionDelegate::updateEditorGeometry.\n" );
    widget->setGeometry(option.rect);
}
