#ifndef TextSelection_h
#define TextSelection_h

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

#include "Debug.h"

#include <QString>

//! string to be found and options
class TextSelection
{
    public:

    //! text selection flags
    enum Flag
    {
        None = 0,
        Backward = 1<<0,
        CaseSensitive = 1<<1,
        EntireWord = 1<<2,
        RegExp = 1<<3,
        NoIncrement = 1<<4
    };

    Q_DECLARE_FLAGS( Flags, Flag );

    //! default constructor
    TextSelection(
        const QString& text = "",
        const QString& replaceText = "",
        Flags flags = None ):
        text_( text ),
        replaceText_( replaceText ),
        flags_( flags )
    {}

    //! text
    const QString& text( void ) const
    { return text_; }

    //! test
    void setText( const QString& text )
    { text_ = text; }

    //! replace text
    const QString& replaceText( void ) const
    { return replaceText_; }

    //! replace text
    void setReplaceText( const QString& text )
    { replaceText_ = text; }

    //! flags
    Flags flags( void ) const
    { return flags_; }

    //! flags
    void setFlags( Flags flags )
    { flags_ = flags; }

    //! flags
    bool flag( const Flag& flag ) const
    { return flags_ & flag; }

    //! flags
    void setFlag( const Flag& flag, const bool& value )
    {
        if( value ) flags_ |= flag;
        else flags_ &= (~flag);
    }

    private:

    //! string to be found
    QString text_;

    //! string to replace
    QString replaceText_;

    //! flags
    Flags flags_;

    //! streamer
    friend QTextStream &operator << (QTextStream &out,const TextSelection &selection)
    {
        out
            << "TextSelection -"
            << " text: " << selection.text()
            << " replacement: " << selection.replaceText()
            << " backward: " << (selection.flag(Backward) ? "true":"false")
            << " case sensitive: " << (selection.flag(CaseSensitive) ? "true":"false")
            << " entire word: " << (selection.flag(EntireWord) ? "true":"false")
            << " regex: " << (selection.flag(RegExp) ? "true":"false")
            << " no increment: " << (selection.flag(NoIncrement) ? "true":"false");
        return out;
    }

};

Q_DECLARE_OPERATORS_FOR_FLAGS( TextSelection::Flags )

#endif
