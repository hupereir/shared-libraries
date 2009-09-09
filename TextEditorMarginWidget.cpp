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
* software; if not, write to the Free Software , Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
  \file TextEditorMarginWidget.cpp
  \brief widget used to draw margins in TextEditors
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPainter>

#include "Debug.h"
#include "Singleton.h"
#include "TextEditor.h"
#include "TextEditorMarginWidget.h"
#include "XmlOptions.h"

//_____________________________________________________________
TextEditorMarginWidget::TextEditorMarginWidget( TextEditor* parent ):
  QWidget( parent ),
  Counter( "TextEditorMarginWidget" ),
  editor_( parent ),
  dirty_( false ),
  draw_vertical_line_( false )
{
  Debug::Throw( "TextEditorMarginWidget::TextEditorMarginWidget.\n" );
  resize(0,0);

  setBackgroundRole( QPalette::Window );
  setAutoFillBackground( true );

  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
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

  if( ( color = QColor( XmlOptions::get().raw("MARGIN_FOREGROUND") ) ).isValid() )
  { palette.setColor( QPalette::WindowText, color ); }

  if( ( color = QColor( XmlOptions::get().raw("MARGIN_BACKGROUND") ) ).isValid() )
  { palette.setColor( QPalette::Window, color ); }

  setPalette( palette );

  // draw vertical line
  draw_vertical_line_ = XmlOptions::get().get<bool>( "MARGIN_VERTICAL_LINE" );

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
