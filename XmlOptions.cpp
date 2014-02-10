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

#include "XmlOptions.h"
#include "XmlOptions_p.h"

#include "Options.h"
#include "XmlDocument.h"
#include "XmlOption.h"

#include <QFile>

//____________________________________________________________________
XmlOptionsSingleton XmlOptions::singleton_;

//____________________________________________________________________
bool XmlOptionsSingleton::_differs( const Options& first, const Options& second ) const
{

    // check special options
    for( Options::SpecialMap::const_iterator firstIter = first.specialOptions().constBegin(); firstIter != first.specialOptions().constEnd(); ++firstIter )
    {

        // skip empty special options
        if( firstIter.value().isEmpty() ) continue;

        // get matching options in the second set
        if( !second.isSpecialOption( firstIter.key() ) ) return true;
        Options::List options( second.specialOptions( firstIter.key() ) );

        // loop over options in first list
        foreach( const Option& option, firstIter.value() )
        {
            // skip non recordable options
            if( !option.isRecordable() ) continue;

            // find in second list
            if( options.indexOf( option ) < 0 ) return true;

        }

    }

    // loop over options and check existence in other map
    for( Options::Map::const_iterator firstIter = first.options().constBegin(); firstIter != first.options().constEnd(); ++firstIter )
    {

        // skip non recordable options
        if( !firstIter.value().isRecordable() ) continue;

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

    // check filename is valid
    if( singleton_.file().isEmpty() ) return false;

    // parse the file
    XmlDocument document;
    QFile qtfile( singleton_.file() );
    if ( !document.setContent( &qtfile, singleton_.error_ ) ) return false;
    else return _read( document, singleton_.options_ );

}

//________________________________________________
bool XmlOptions::write( void )
{

    // check filename is valid
    if( singleton_.file().isEmpty() ) return false;

    // create document and read
    XmlDocument document;
    {
        QFile qtfile( singleton_.file() );
        document.setContent( &qtfile );
    }

    // read old options from xml document
    Options options;
    _read( document, options );
    if( !singleton_.differs( options ) ) return true;

    // create main element
    QDomElement top = document.createElement( Base::XML::OPTIONS ).toElement();

    // write list of special option names
    for( Options::SpecialMap::const_iterator iter = singleton_.options_.specialOptions().begin(); iter != singleton_.options_.specialOptions().end(); ++iter )
    {
        // check size of options
        if( iter.value().empty() ) continue;

        // dump option name
        QDomElement element = document.createElement( Base::XML::SPECIAL_OPTION );
        element.setAttribute( Base::XML::VALUE, iter.key() );
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
    QDomNodeList topNodes = document.elementsByTagName( Base::XML::OPTIONS );
    if( topNodes.isEmpty() ) return false;

    for(QDomNode node = topNodes.at(0).firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement element = node.toElement();
        if( element.isNull() ) continue;

        // special options
        if( element.tagName() == Base::XML::SPECIAL_OPTION )
        {

            // retrieve Value attribute
            QString value( element.attribute( Base::XML::VALUE ) );
            if( value.size() ) options.keep( value );

        } else if( element.tagName() == Base::XML::OPTION ) {

            XmlOption option( element );
            if( options.isSpecialOption( option.name() ) ) options.add( option.name(), option );
            else options.set( option.name(), (Option) option );

        }
    }

    return true;

}
