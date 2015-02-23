#ifndef Options_h
#define Options_h

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
#include "Option.h"

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTextStream>

//* Option file parser based on Xml
class Options: public Counter
{

    public:

    //* pair
    using Pair = QPair<QString,Option>;

    //* shortCut for option map
    using Map = QMap<QString,Option>;

    //* shortCut for option list
    using List = QList<Option>;

    //* shortCut for option map
    using SpecialMap = QMap<QString,List>;

    //* constructor
    Options( void );

    //* destructor
    virtual ~Options( void )
    {}

    //* equality operator
    bool operator == ( const Options& other ) const
    {
        Debug::Throw( "Options::operator ==.\n" );
        bool out(
            ( options_ == other.options_ ) &&
            ( specialOptions_ == other.specialOptions_ ) );
        return out;
    }

    //* different operator
    bool operator != ( const Options& other ) const
    { return !( *this == other ); }

    //* install defaults
    virtual void installDefaultOptions( void );

    //*@name accessors
    //@{

    //* retrieve Option map
    virtual const Map& options( void ) const
    { return options_; }

    //* retrieve special Option map
    virtual const SpecialMap& specialOptions( void ) const
    { return specialOptions_; }

    //* returns true if option name is special
    virtual bool isSpecialOption( const QString& name ) const;

    //* retrieve list of special (i.e. kept) options matching a given name
    template <typename T> QList<T> specialOptions( const QString& name ) const
    {

        const List& optionList( specialOptions( name ) );
        QList<T> out;

        foreach( const Option& option, optionList )
        { out << option.get<T>(); }
        return out;

    }

    //* retrieve list of special (i.e. kept) options matching a given name
    virtual const List& specialOptions( const QString& name ) const
    {
        SpecialMap::const_iterator iter( specialOptions_.find( name ) );
        Q_ASSERT( iter != specialOptions_.end() );
        return iter.value();
    }

    //* returns true if option with matching name is found
    virtual bool contains( const QString& name ) const
    { return options_.find( name ) != options_.end(); }

    //* option matching given name
    virtual const Option& option( const QString& name ) const
    { return options_.find( name ).value(); }

    //* option value accessor
    template <typename T> T get( const QString& name ) const
    { return _find( name ).value().get<T>(); }

    //* option raw value accessor
    virtual QByteArray raw( const QString& name ) const
    { return _find( name ).value().raw(); }

    //@}

    //*@name modifiers
    //@{

    //* adds a new special option. Return true if option is added
    virtual bool add( const QString&, const Option&, bool isDefault = false );

    //* clear list of special (i.e. kept) options matching a given name
    virtual void clearSpecialOptions( const QString& name );

    //* option value modifier
    template <typename T>
    void set( const QString& name, const T& value, bool isDefault = false )
    {
        Q_ASSERT( !isSpecialOption( name ) );
        Option &option( options_[name] );
        option.set<T>( value );
        if( isDefault || _autoDefault() ) option.setDefault();

    }

    //* assign an option to given name
    void set( const QString&, const Option&, bool isDefault = false );

    //* option raw value modifier
    virtual void setRaw( const QString& name, const QByteArray& value, bool isDefault = false )
    {
        Q_ASSERT( !isSpecialOption( name ) );
        Option &option( options_[name] );
        option.setRaw( value );
        if( isDefault || _autoDefault() ) option.setDefault();
    }

    //* option raw value modifier
    virtual void setRaw( const QString& name, const QString& value, bool isDefault = false )
    {
        Q_ASSERT( !isSpecialOption( name ) );
        Option &option( options_[name] );
        option.setRaw( value.toUtf8() );
        if( isDefault || _autoDefault() ) option.setDefault();
    }

    /*! \brief
    tags a given option to be kept in separate list as a "special" option.
    These options are not stored unique.
    They are stored in a list, with no unicity check. Accessing these therefore
    takes more time with respect to the "standard" options, which are stored
    in a map.
    */
    virtual void keep( const QString& name )
    {
        if( specialOptions_.find( name ) == specialOptions_.end() )
        { specialOptions_.insert( name, List() ); }
    }

    //* auto-default
    void setAutoDefault( bool value )
    { autoDefault_ = value; }

    //* restore defaults
    void restoreDefaults( void );

    //@}

    protected:

    //* find name
    Map::const_iterator _find( const QString& name ) const;

    //* auto-default
    bool _autoDefault( void ) const
    { return autoDefault_; }

    private:

    //* option map
    Map options_;

    //* set of option names to be kept separately
    SpecialMap specialOptions_;

    //* if true all options inserted are also set as default
    bool autoDefault_ = false;

    //* streamer
    friend QTextStream &operator << ( QTextStream &,const Options &);

};

#endif
