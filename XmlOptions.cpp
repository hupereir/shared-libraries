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

#include "XmlOptions.h"
#include "XmlOptions_p.h"

#include "Options.h"
#include "XmlDocument.h"
#include "XmlError.h"
#include "XmlOption.h"

#include <QtCore/QFile>

//____________________________________________________________________
XmlOptionsSingleton XmlOptions::singleton_;

//____________________________________________________________________
bool XmlOptionsSingleton::modified( void ) const
{

    // check file changed
    return
        options_.specialOptions() != backup_.specialOptions() ||
        _differs( options_, backup_ ) ||
        _differs( backup_, options_ );

}

//____________________________________________________________________
bool XmlOptionsSingleton::_differs( const Options& first, const Options& second ) const
{

    // loop over options and check existence in other map
    for( Options::Map::const_iterator firstIter = first.options().constBegin(); firstIter != first.options().constEnd(); firstIter++ )
    {

        const Options::Map::const_iterator secondIter( second.options().constFind( firstIter.key() ) );
        if( secondIter == second.options().constEnd() )
        {

            if( !firstIter.value().isDefault() ) return true;

        } else if( firstIter.value() != secondIter.value() ) return true;

    }

    return false;

}

//____________________________________________________________________
const File& XmlOptions::file()
{ return singleton_.file(); }

//____________________________________________________________________
const XmlError& XmlOptions::error()
{ return singleton_.error_; }

//____________________________________________________________________
void XmlOptions::setError( const XmlError& error )
{ singleton_.error_ = error; }

//____________________________________________________________________
void XmlOptions::setFile( const File& file )
{

    // make sure file is hidden (windows only)
    if( !singleton_.setFile( file ) ) return;
    if( singleton_.file().localName().startsWith( '.' ) )
    { singleton_.file().setHidden(); }

}

//____________________________________________________________________
Options& XmlOptions::get( void )
{ return singleton_.options_; }

//____________________________________________________________________
bool XmlOptions::read( void )
{

    Debug::Throw() << "XmlOptions::read - file: " << singleton_.file() << endl;

    /*
    need to perform backup before reading to
    install programatically set options, and default values
    */
    singleton_.backup();

    // check filename is valid
    if( singleton_.file().isEmpty() ) return false;

    // parse the file
    XmlDocument document;
    QFile qtfile( singleton_.file() );
    XmlError error( singleton_.file() );
    if ( !document.setContent( &qtfile, &error.error(), &error.line(), &error.column() ) )
    {

        setError( error );
        return false;

    } else return _read( document, singleton_.options_ );

}

//________________________________________________
bool XmlOptions::write( void )
{

    Debug::Throw() << "XmlOptions::write - file: " << singleton_.file() << endl;

    // check filename is valid
    if( singleton_.file().isEmpty() ) return false;

    // create document and read
    XmlDocument document;
    {
        QFile qtfile( singleton_.file() );
        document.setContent( &qtfile );
    }

    // read backup from xml document
    _read( document, singleton_.backup_ );

    // check modifications
    if( !singleton_.modified() ) return true;

    // create main element
    QDomElement top = document.createElement( BASE::XML::OPTIONS ).toElement();

    // write list of special option names
    for( Options::SpecialMap::const_iterator iter = singleton_.options_.specialOptions().begin(); iter != singleton_.options_.specialOptions().end(); ++iter )
    {
        // check size of options
        if( iter.value().empty() ) continue;

        // dump option name
        QDomElement element = document.createElement( BASE::XML::SPECIAL_OPTION );
        element.setAttribute( BASE::XML::VALUE, iter.key() );
        top.appendChild( element );
    }

    // write options
    for( Options::SpecialMap::const_iterator iter = singleton_.options_.specialOptions().begin(); iter != singleton_.options_.specialOptions().end(); ++iter )
    {

        Options::List option_list( iter.value() );
        for( Options::List::iterator listIter = option_list.begin(); listIter != option_list.end(); ++listIter )
        {

            if( listIter->hasFlag( Option::Recordable ) && listIter->isSet() )
            { top.appendChild( XmlOption( iter.key(), *listIter ).domElement( document ) ); }

        }

    }

    // write standard options
    for( Options::Map::const_iterator iter = singleton_.options_.options().begin(); iter != singleton_.options_.options().end(); ++iter )
    {

        if( iter.value().hasFlag( Option::Recordable ) && iter.value().isSet() && !iter.value().isDefault() )
        { top.appendChild( XmlOption( iter.key(), iter.value() ).domElement( document ) ); }

    }

    // append topNode to document and write
    document.replaceChild( top );

    // write
    {
        QFile qfile( singleton_.file() );
        if( !qfile.open( QIODevice::WriteOnly ) ) return false;
        qfile.write( document.toByteArray() );
    }

    return true;

}

//________________________________________________
bool XmlOptions::_read( const XmlDocument& document, Options& options )
{

    // look for relevant element
    QDomNodeList topNodes = document.elementsByTagName( BASE::XML::OPTIONS );
    if( topNodes.isEmpty() ) return false;

    for(QDomNode node = topNodes.at(0).firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement element = node.toElement();
        if( element.isNull() ) continue;

        // special options
        if( element.tagName() == BASE::XML::SPECIAL_OPTION )
        {

            // retrieve Value attribute
            QString value( element.attribute( BASE::XML::VALUE ) );
            if( value.size() ) options.keep( value );

        } else if( element.tagName() == BASE::XML::OPTION ) {

            XmlOption option( element );
            if( options.isSpecialOption( option.name() ) ) options.add( option.name(), option );
            else options.set( option.name(), option );

        }
    }

    return true;

}
