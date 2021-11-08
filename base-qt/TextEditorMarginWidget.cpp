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

#include "TextEditorMarginWidget.h"
#include "Color.h"
#include "Debug.h"
#include "TextEditor.h"


#include <QPainter>

//_____________________________________________________________
TextEditorMarginWidget::TextEditorMarginWidget( TextEditor* parent ):
    QWidget( parent ),
    Counter( QStringLiteral("TextEditorMarginWidget") ),
    editor_( parent )
{
    Debug::Throw( QStringLiteral("TextEditorMarginWidget::TextEditorMarginWidget.\n") );
    resize(0,0);

    setBackgroundRole( QPalette::AlternateBase );
    setForegroundRole( QPalette::Text );
    setAutoFillBackground( true );
}

//________________________________________________________________
QColor TextEditorMarginWidget::backgroundColor() const
{ return palette().color( QPalette::AlternateBase ); }

//________________________________________________________________
QColor TextEditorMarginWidget::foregroundColor() const
{ return Base::Color( palette().color( QPalette::Text ) ).merge( palette().color( QPalette::AlternateBase ), 0.6 ); }

//________________________________________________________________
void TextEditorMarginWidget::setDirty()
{
    if( !dirty_ )
    {
        dirty_ = true;
        update();
    }
}

//________________________________________________
void TextEditorMarginWidget::paintEvent( QPaintEvent* event )
{

    // paint margins
    QPainter painter( this );
    painter.setClipRect( event->rect() );
    editor_->paintMargin( painter );
    painter.end();

    // clear dirty flag
    dirty_ = false;

}
