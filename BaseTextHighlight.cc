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
* software; if not, write to the Free Software Foundation, Inc., 59 Temple     
* Place, Suite 330, Boston, MA  02111-1307 USA                           
*                         
*                         
*******************************************************************************/

/*!
  \file BaseTextHighlight.cc
  \brief syntax highlighting based on text patterns
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "BaseTextHighlight.h"
#include "TextBlockData.h"

using namespace std;

//_________________________________________________________
BaseTextHighlight::BaseTextHighlight( QTextDocument* document ):
  QSyntaxHighlighter( document ),
  Counter( "BaseTextHighlight" ),
  enabled_( false )
{ Debug::Throw( "BaseTextHighlight::BaseTextHighlight.\n" ); }

//_________________________________________________________
void BaseTextHighlight::synchronize( BaseTextHighlight* parent )
{ 
  Debug::Throw( "BaseTextHighlight::synchronize.\n" );
  setEnabled( parent->isEnabled() );
  setHighlightColor( parent->highlightColor() );
}

//_________________________________________________________
bool BaseTextHighlight::setEnabled( const bool& state )
{ 
  Debug::Throw() << "BaseTextHighlight::setEnabled - state: " << state << endl;
  if( enabled_ == state ) return false;
  enabled_ = state; 
  return true;
}

//_________________________________________________________
void BaseTextHighlight::highlightBlock( const QString& text )
{
    
  if( !isEnabled() ) return;
    
  // try retrieve data  
  TextBlockData* data = dynamic_cast<TextBlockData*>( currentBlockUserData() );  
  if( !data ) return;
  
  // highlight current block if enabled and needed
  if( isEnabled() && data->isCurrentBlock() ) setFormat( 0, text.size(), format_ );
  else if( data->hasBackground() )
  {
    // set paragraph background color if any
    QTextCharFormat format;
    format.setBackground( data->background() );
    setFormat( 0, text.size(), format );
  }
  
  return;
}
