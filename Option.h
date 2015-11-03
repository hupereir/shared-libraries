#ifndef Option_h
#define Option_h

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

#include <QString>
#include <QTextStream>

//* stream boolean
QTextStream& operator >> ( QTextStream& in, bool& value );

/**
\class   Option
\brief   Option objects for string, int, bool and double options
*/
class Option:public Counter
{

    public:

    //* flags
    enum Flag
    {
        None = 0,
        Current = 1 << 0,
        Recordable = 1<<1
    };

    Q_DECLARE_FLAGS(Flags, Flag)

    //* constructor
    Option();

    //* constructor
    Option( const char*, Flags = Recordable );

    //* constructor
    Option( const QByteArray&, Flags = Recordable );

    //* constructor
    Option( const QByteArray&, const QString&, Flags = Recordable );

    //* constructor
    Option( const QString&, Flags = Recordable );

    //* less than operator
    bool operator < (const Option& other ) const
    {
        if( value_ != other.value_ ) return value_ < other.value_;
        else if( flags_ != other.flags_ ) return flags_ < other.flags_;
        else if( defaultValue_ != other.defaultValue_ ) return defaultValue_ < other.defaultValue_;
        else if( defaultFlags_ != other.defaultFlags_ ) return defaultFlags_ < other.defaultFlags_;
        else return comments_ < other.comments_;
    }

    //* equal operator
    bool operator == (const Option& other ) const
    { return value_ == other.value_ && flags_ == other.flags_; }

    //* different operator
    bool operator != (const Option& other ) const
    { return !( *this == other ); }

    //*@name accessors
    //@{

    //* option comments
    const QString& comments( void ) const
    { return comments_; }

    //* flags
    Flags flags( void ) const
    { return flags_; }

    //* flags
    bool hasFlag( const Flag& flag ) const
    { return flags_ & flag; }

    //* current
    bool isCurrent( void ) const
    { return hasFlag( Current ); }

    //* default
    bool isDefault( void ) const
    { return defaultValue_ == value_ && defaultFlags_ == flags_; }

    //* recordable
    bool isRecordable( void ) const
    { return hasFlag( Recordable ); }

    //* raw accessor
    const QByteArray& raw( void ) const
    { return value_; }

    //* default value
    const QByteArray& defaultValue( void ) const
    { return defaultValue_; }

    //* generic accessor
    template < typename T >
        T get( void ) const
    {

        // check if option is set
        Q_ASSERT( !value_.isEmpty() );

        // cast value
        // the const-cast here is because the string should not be affected
        // (hence the ReadOnly) but Qt does not allow to pass a const pointer
        QTextStream s( const_cast<QByteArray*>(&value_), QIODevice::ReadOnly );
        T out;
        s >> out;
        Q_ASSERT( s.status() == QTextStream::Ok );
        return out;
    }

    //* check status
    bool isSet( void ) const
    {return !value_.isEmpty();}

    //@}

    //*@name modifiers
    //@{

    //* option comments
    Option& setComments( const QString& comments )
    {
        comments_ = comments;
        return *this;
    }

    //* flags
    Option& setFlags( Flags value )
    {
        flags_ = value;
        return *this;
    }

    //* flags
    Option& setFlag( Flag flag, bool value = true )
    {
        if( value ) { flags_ |= flag; }
        else { flags_ &= (~flag); }
        return *this;
    }

    //* current
    Option& setCurrent( bool value )
    { return setFlag( Current, value ); }

    //* default
    Option& setDefault( void )
    {
        defaultValue_ = value_;
        defaultFlags_ = flags_;
        return *this;
    }

    //* raw modifier
    Option& setRaw( const QByteArray& value )
    {
        value_ = value;
        return *this;
    }

    //* raw modifier
    Option& setRaw( const QString& value )
    {
        value_ = value.toUtf8();
        return *this;
    }

    //* generic modifier
    template < typename T >
        Option& set( const T& value )
    {

        value_.clear();
        QTextStream s( &value_, QIODevice::WriteOnly );
        s << value;
        return *this;

    }

    //* restore default value
    Option& restoreDefault()
    {
        value_ = defaultValue_;
        flags_ = defaultFlags_;
        return *this;
    }

    //@}

    //* used to retrieve file records that match a given flag
    class HasFlagFTor
    {

        public:

        //* constructor
        HasFlagFTor( Option::Flags flags ):
            flags_( flags )
        {}

        //* predicate
        bool operator() ( const Option& option ) const
        { return option.flags() & flags_; }

        //* sorting predicate
        /** it is used to ensure that options that have a given flag appear first in a list */
        bool operator() (const Option& first, const Option& second ) const
        { return ( (first.flags() & flags_) && !(second.flags()&flags_) ); }

        private:

        // predicted flag
        Option::Flags flags_;

    };

    //* used to get options with matching values
    class SameValueFTor
    {
        public:

        //* constructor
        SameValueFTor( const Option& option ):
            value_( option.value_ )
            {}

        //* destructor
        virtual ~SameValueFTor( void )
        {}

        //* predicate
        bool operator() (const Option& other ) const
        { return value_ == other.value_; }

        private:

        //* prediction
        QByteArray value_;

    };

    private:

    //* option value
    QByteArray value_;

    //* option default value
    QByteArray defaultValue_;

    //* option comments
    QString comments_;

    //* flags
    Flags flags_ = Recordable;

    //* default flags
    Flags defaultFlags_ = Recordable;

    //* streamer
    friend QTextStream &operator << ( QTextStream &out, const Option& option );

    //* streamer
    friend QTextStream &operator << ( QTextStream &out, const QList<Option>& options );

};

Q_DECLARE_OPERATORS_FOR_FLAGS( Option::Flags )

#endif
