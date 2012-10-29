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
*******************************************************************************/

#include "DetailsDialog.h"
#include "TextEditor.h"

//____________________________________________________________
void DetailsDialog::setDetails( const QString& details )
{

    Debug::Throw( "DetailsDialog::setDetails.\n" );

    // do nothing if detail string is invalid
    if( details.isEmpty() ) return;

    // create text editor assign text and set as details
    TextEditor* editor = new TextEditor();
    editor->setReadOnly( true );
    editor->setWrapFromOptions( false );
    editor->setLineWrapMode( QTextEdit::WidgetWidth );
    editor->setText( details );
    setDetails( editor );

}

//____________________________________________________________
void DetailsDialog::setDetails( QWidget* widget )
{

    Debug::Throw( "DetailsDialog::setDetails.\n" );
    if( detailsWidget_ )
    {
        detailsWidget_->hide();
        detailsWidget_->deleteLater();
    }

    // details label
    if( !detailsLabel_ )
    {
        detailsLabel_ = new QLabel( "<a href='details'>details...</a>", this );
        detailsLabel_->setAlignment( Qt::AlignRight|Qt::AlignVCenter );
        mainLayout().addWidget( detailsLabel_ );

        connect( detailsLabel_, SIGNAL( linkActivated( const QString& ) ), SLOT( showDetails( void ) ) );

    }

    // assign widget
    detailsWidget_ = widget;
    if( detailsWidget_->parentWidget() != this ) detailsWidget_->setParent( this );
    mainLayout().addWidget( detailsWidget_, 1 );
    detailsWidget_->hide();

}

//_____________________________________________________________________
void DetailsDialog::toggleDetails( bool visible )
{
    Debug::Throw( "toggleDetails::showDetails.\n" );
    if( !detailsWidget_ ) return;

    detailsLabel_->setVisible( !visible );
    detailsWidget_->setVisible( visible );

    // also hide separator
    if( hasSeparator() ) separator().setVisible( !visible );
    adjustSize();

}
