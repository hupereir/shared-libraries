#ifndef Option_h
#define Option_h

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

#include <cassert>
#include <QString>
#include <QTextStream>

#include "Counter.h"
#include "Debug.h"


//! stream boolean
/*! not sure if this is a valid writing or not */
QTextStream& operator >> ( QTextStream& in, bool& value );

/*!
\class   Option
\brief   Option objects for string, int, bool and double options
*/
class Option:public Counter
{

    public:

    //! constructor
    Option();

    //! constructor
    Option( const char*, const unsigned int& = RECORDABLE );

    //! constructor
    Option( const QByteArray&, const unsigned int& = RECORDABLE );

    //! constructor
    Option( const QByteArray&, const QString&, const unsigned int& = RECORDABLE );

    //! constructor
    Option( const QString&, const unsigned int& = RECORDABLE );

    //! less than operator
    bool operator < (const Option& option ) const
    { return value_ < option.value_; }

    //! equal operator
    bool operator == (const Option& option ) const
    { return value_ == option.value_; }

    //! option comments
    const QString& comments( void ) const
    { return comments_; }

    //! option comments
    Option& setComments( const QString& comments )
    {
        comments_ = comments;
        return *this;
    }

    //!@name flags
    //@{

    //! flags
    enum Flag
    {
        NONE = 0,
        CURRENT = 1 << 0,
        RECORDABLE = 1<<1
    };

    //! flags
    Option& setFlags( unsigned int value )
    {
        flags_ = value;
        return *this;
    }

    //! flags
    Option& setFlag( Flag flag, const bool& value = true )
    {
        if( value ) { flags_ |= flag; }
        else { flags_ &= (~flag); }
        return *this;
    }

    //! flags
    const unsigned int& flags( void ) const
    { return flags_; }

    //! flags
    bool hasFlag( const Flag& flag ) const
    { return flags_ & flag; }

    //! used to retrieve file records that match a given flag
    class HasFlagFTor
    {

        public:

        //! constructor
        HasFlagFTor( const unsigned int& flag ):
            flag_( flag )
        {}

        //! predicate
        bool operator() ( const Option& option ) const
        { return option.flags() & flag_; }

        //! sorting predicate
        /*! it is used to ensure that options that have a given flag appear first in a list */
        bool operator() (const Option& first, const Option& second ) const
        { return ( (first.flags() & flag_) && !(second.flags()&flag_) ); }

        private:

        // predicted flag
        unsigned int flag_;

    };

    //! current
    bool isCurrent( void ) const
    { return hasFlag( CURRENT ); }

    //! current
    Option& setCurrent( const bool& value )
    { return setFlag( CURRENT, value ); }

    //@}

    //! default
    Option& setDefault( void )
    {
        defaultValue_ = value_;
        defaultFlags_ = flags_;
        return *this;
    }


    //! raw accessor
    const QByteArray& raw( void ) const
    { return value_; }

    //! raw modifier
    Option& setRaw( const QByteArray& value )
    {
        value_ = value;
        return *this;
    }

    //! raw modifier
    Option& setRaw( const QString& value )
    {
        value_ = value.toUtf8();
        return *this;
    }

    //! default value
    const QByteArray& defaultValue( void ) const
    { return defaultValue_; }

    //! accessor
    template < typename T >
        T get( void ) const
    {

        // check if option is set
        assert( !value_.isEmpty() );

        // cast value
        // the const-cast here is because the string should not be affected
        // (hence the ReadOnly) but Qt does not allow to pass a const pointer
        QTextStream s( const_cast<QByteArray*>(&value_), QIODevice::ReadOnly );
        T out;
        s >> out;
        assert( s.status() == QTextStream::Ok );
        return out;
    }

    //! modifier
    template < typename T >
        Option& set( const T& value )
    {

        value_.clear();
        QTextStream s( &value_, QIODevice::WriteOnly );
        s << value;
        return *this;

    }

    //! check status
    bool set( void ) const
    {return !value_.isEmpty();}

    //! restore default value
    Option& restoreDefault()
    {
        value_ = defaultValue_;
        flags_ = defaultFlags_;
        return *this;
    }

    private:

    //! option value
    QByteArray value_;

    //! option default value
    QByteArray defaultValue_;

    //! option comments
    QString comments_;

    //! flags
    unsigned int flags_;

    //! default flags
    unsigned int defaultFlags_;

    //! streamer
    friend QTextStream &operator << ( QTextStream &out, const Option &option )
    {
        if( !option.set() ) out << "not set";
        else out << option.raw();
        return out;
    }

};

#endif
