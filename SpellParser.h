#ifndef SpellParser_h
#define SpellParser_h

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

#include "Counter.h"
#include "Debug.h"
#include "SpellInterface.h"
#include "TextFormat.h"
#include "Word.h"


#include <QColor>
#include <QFont>
#include <QString>

namespace SpellCheck
{

  //! highlight text based on misspelled words
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
    virtual bool setColor( const QColor& color );

    //! color
    virtual const QColor& color( void ) const
    { return color_; }

    //! font format
    /* returns true if changed */
    virtual bool setFontFormat( Format::TextFormatFlags );

    //! font format
    virtual Format::TextFormatFlags fontFormat( void ) const
    { return fontFormat_; }

    //! enabled. Returns true if changed.
    bool setEnabled( bool value )
    {
      if( enabled_ == value ) return false;
      enabled_ = value;
      return true;
    }

    //! enabled
    bool isEnabled( void ) const
    { return enabled_; }

    //! spell interface
    virtual const SpellInterface& interface( void ) const
    { return interface_; }

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
    Format::TextFormatFlags fontFormat_;

  };

};
#endif
