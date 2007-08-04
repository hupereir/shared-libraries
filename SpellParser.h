#ifndef _SpellParser_h_
#define _SpellParser_h_

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
  \file SpellParser.h
  \brief parse text and store mispelled words and locations
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QTextCharFormat>
#include <QColor>
#include <QFont>
#include <QString>

#include <map>

#include "Counter.h"
#include "SpellInterface.h"
#include "TextFormat.h" 
#include "Word.h"

//! highlight text based on misspelled words
namespace SPELLCHECK
{
  class SpellParser: public Counter
  {

    public:

    //! constructor
    SpellParser();

    //! destructor
    virtual ~SpellParser( void )
    { Debug::Throw( "SpellParser::~SpellParser.\n" ); }
    
    //! highlight paragraph
    virtual Word::Set parse( const QString& text );

    //! highlight color
    /* returns true if changed */
    virtual bool setColor( const QColor& color )
    { 
      Debug::Throw( "SpellParser::setColor.\n" );
      if( color == color_ || !color.isValid() ) return false;
      else { 
        color_ = color; 
        return true;
      }
    }

    //! font format
    /* returns true if changed */
    virtual bool setFontFormat( const unsigned int& format )
    { 
      
      Debug::Throw( "SpellParser::setFontFormat.\n" );
      if( format_ == format ) return false;
      else {
        format_ = format;
        return true;
      }
      
    }
          
    //! formated font
    virtual QTextCharFormat format() const
    {
      
      QTextCharFormat out;
      
      out.setFontWeight( (format_&FORMAT::BOLD) ? QFont::Bold : QFont::Normal );
      out.setFontItalic( format_&FORMAT::ITALIC );
      out.setFontUnderline( format_&FORMAT::UNDERLINE );
      out.setFontOverline( format_&FORMAT::OVERLINE );
      out.setFontStrikeOut( format_&FORMAT::STRIKE );
      if( color_.isValid() ) out.setForeground( color_ );
      
      return out;
    }

    //! enabled. Returns true if changed.
    virtual bool setEnabled( const bool& value )
    { 
      if( enabled_ == value ) return false;
      enabled_ = value; 
      return true;
    }
    
    //! enabled
    virtual const bool& isEnabled( void ) const
    { return enabled_; }
          
    //! spell interface
    virtual SpellInterface& interface( void )
    { return interface_; }
    
    private:
    
    //! enabled
    bool enabled_;
    
    //! spell interface
    SpellInterface interface_;
    
    //! highlight color
    QColor color_;
    
    //! font format (is a bitwise or of TextFormatInfo bits)
    unsigned int format_;
        
  };

};
#endif
