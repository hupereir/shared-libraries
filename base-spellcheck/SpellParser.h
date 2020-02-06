#ifndef SpellParser_h
#define SpellParser_h

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

#include "base_spellcheck_export.h"
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

  //* highlight text based on misspelled words
  class BASE_SPELLCHECK_EXPORT SpellParser final: private Base::Counter<SpellParser>
  {

    public:

    //* constructor
    explicit SpellParser();

    //* highlight paragraph
    Word::Set parse( const QString& );

    //* highlight color
    /* returns true if changed */
    bool setColor( const QColor& );

    //* color
    const QColor& color() const
    { return color_; }

    //* font format
    /* returns true if changed */
    bool setFontFormat( TextFormat::Flags );

    //* font format
    TextFormat::Flags fontFormat() const
    { return fontFormat_; }

    //* enabled. Returns true if changed.
    bool setEnabled( bool value )
    {
      if( enabled_ == value ) return false;
      enabled_ = value;
      return true;
    }

    //* enabled
    bool isEnabled() const
    { return enabled_; }

    //* spell interface
    const SpellInterface& interface() const
    { return interface_; }

    //* spell interface
    SpellInterface& interface()
    { return interface_; }

    private:

    //* enabled
    bool enabled_ = false;

    //* spell interface
    SpellInterface interface_;

    //* highlight color
    QColor color_;

    //* font format (is a bitwise or of TextFormatInfo bits)
    TextFormat::Flags fontFormat_ = TextFormat::Default;

  };

};
#endif
