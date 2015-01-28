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

#include "TextEditorMarginWidget.h"

#include "Color.h"
#include "Debug.h"
#include "Singleton.h"
#include "TextEditor.h"
#include "XmlOptions.h"

#include <QPainter>

//_____________________________________________________________
TextEditorMarginWidget::TextEditorMarginWidget( TextEditor* parent ):
    QWidget( parent ),
    Counter( "TextEditorMarginWidget" ),
    editor_( parent ),
    dirty_( false )
{
    Debug::Throw( "TextEditorMarginWidget::TextEditorMarginWidget.\n" );
    resize(0,0);

    setBackgroundRole( QPalette::Window );
    setAutoFillBackground( true );

    connect( Singleton::get().application(), SIGNAL(configurationChanged()), SLOT(_updateConfiguration()) );
    _updateConfiguration();

}

//________________________________________________________________
void TextEditorMarginWidget::setDirty( void )
{
    if( !dirty_ )
    {
        dirty_ = true;
        update();
    }
}

//________________________________________________________________
void TextEditorMarginWidget::_updateConfiguration( void )
{

    Debug::Throw( "TextEditorMarginWiget::_updateConfiguration.\n" );

    // update palette using colors from options
    QPalette palette( TextEditorMarginWidget::palette() );
    QColor color;

    if( ( color = QColor( XmlOptions::get().get<Base::Color>("MARGIN_FOREGROUND") ) ).isValid() )
    { palette.setColor( QPalette::WindowText, color ); }

    if( ( color = QColor( XmlOptions::get().get<Base::Color>("MARGIN_BACKGROUND") ) ).isValid() )
    { palette.setColor( QPalette::Window, color ); }

    setPalette( palette );

}

//________________________________________________
void TextEditorMarginWidget::paintEvent( QPaintEvent* event )
{

    // paint margins
    QPainter painter( this );
    painter.setClipRect( event->rect() );
    _editor().paintMargin( painter );
    painter.end();

    // clear dirty flag
    dirty_ = false;

}
