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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "Counter.h"
#include "Debug.h"
#include "Option.h"

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

//! Option file parser based on Xml
class Options: public Counter
{

    public:

    //! pair
    typedef QPair< QString, Option > Pair;

    //! shortCut for option map
    typedef QMap< QString, Option > Map;

    //! shortCut for option list
    typedef QList< Option > List;

    //! shortCut for option map
    typedef QMap< QString, List > SpecialMap;

    //! constructor
    Options( void );

    //! destructor
    virtual ~Options( void )
    {}

    //! equality operator
    bool operator == ( const Options& other ) const
    {
        Debug::Throw( "Options::operator ==.\n" );
        bool out(
            ( options_ == other.options_ ) &&
            ( specialOptions_ == other.specialOptions_ ) );
        return out;
    }

    //! different operator
    bool operator != ( const Options& other ) const
    { return !( *this == other ); }

    //! install defaults
    virtual void installDefaultOptions( void );

    //!@name accessors
    //@{

    //! retrieve Option map
    virtual const Map& options( void ) const
    { return options_; }

    //! retrieve special Option map
    virtual const SpecialMap& specialOptions( void ) const
    { return specialOptions_; }

    //! returns true if option name is special
    virtual bool isSpecialOption( const QString& name ) const;

    //! retrieve list of special (i.e. kept) options matching a given name
    template <typename T> QList<T> specialOptions( const QString& name ) const
    {

        const List& optionList( specialOptions( name ) );
        QList<T> out;

        foreach( const Option& option, optionList )
        { out << option.get<T>(); }
        return out;

    }

    //! retrieve list of special (i.e. kept) options matching a given name
    virtual const List& specialOptions( const QString& name ) const
    {
        SpecialMap::const_iterator iter( specialOptions_.find( name ) );
        Q_ASSERT( iter != specialOptions_.end() );
        return iter.value();
    }

    //! returns true if option with matching name is found
    virtual bool contains( const QString& name )
    { return options_.find( name ) != options_.end(); }

    //! option matching given name
    virtual const Option& option( const QString& name ) const
    { return options_.find( name ).value(); }

    //! option value accessor
    template <typename T> T get( const QString& name ) const
    { return _find( name ).value().get<T>(); }

    //! option raw value accessor
    virtual QByteArray raw( const QString& name ) const
    { return _find( name ).value().raw(); }

    //@}

    //!@name modifiers
    //@{

    //! adds a new option. Return true if option is added
    virtual bool add( const QString&, Option, const bool& isDefault = false );

    //! clear list of special (i.e. kept) options matching a given name
    virtual void clearSpecialOptions( const QString& name );

    //! option value modifier
    void set( const QString& name, Option option, const bool& isDefault = false );

    //! option value modifier
    template <typename T>
    void set( const QString& name, const T& value, const bool& isDefault = false )
    {
        Q_ASSERT( !isSpecialOption( name ) );
        Option &option( options_[name] );
        option.set<T>( value );
        if( isDefault || _autoDefault() ) option.setDefault();

    }

    //! option raw value modifier
    virtual void setRaw( const QString& name, const QByteArray& value, const bool& isDefault = false )
    {
        Q_ASSERT( !isSpecialOption( name ) );
        Option &option( options_[name] );
        option.setRaw( value );
        if( isDefault || _autoDefault() ) option.setDefault();
    }

    //! option raw value modifier
    virtual void setRaw( const QString& name, const QString& value, const bool& isDefault = false )
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

    //! auto-default
    void setAutoDefault( const bool& value )
    { autoDefault_ = value; }

    //! restore defaults
    void restoreDefaults( void );

    //@}

    protected:

    //! find name
    Map::const_iterator _find( const QString& name ) const;

    //! auto-default
    const bool& _autoDefault( void ) const
    { return autoDefault_; }

    private:

    //! option map
    Map options_;

    //! set of option names to be kept separately
    SpecialMap specialOptions_;

    //! if true all options inserted are also set as default
    bool autoDefault_;

    //! streamer
    friend QTextStream &operator << ( QTextStream &,const Options &);

};

#endif
