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
Private::XmlOptionsSingleton XmlOptions::singleton_;
namespace Private
{

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
            for( const auto& option:firstIter.value() )
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
Options& XmlOptions::get()
{ return singleton_.options_; }

//____________________________________________________________________
bool XmlOptions::read()
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
bool XmlOptions::write()
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
    auto top = document.createElement( Base::Xml::Options ).toElement();

    // write options
    for( auto&& iter = singleton_.options_.specialOptions().begin(); iter != singleton_.options_.specialOptions().end(); ++iter )
    {

        for( const auto& option:iter.value() )
        {

            if( option.hasFlag( Option::Flag::Recordable ) && option.isSet() )
            { top.appendChild( XmlOption( iter.key(), option ).domElement( document ) ); }

        }

    }

    // write standard options
    for( auto&& iter = singleton_.options_.options().begin(); iter != singleton_.options_.options().end(); ++iter )
    {

        if( iter.value().hasFlag( Option::Flag::Recordable ) && iter.value().isSet() && !iter.value().isDefault() )
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
    QDomNodeList topNodes = document.elementsByTagName( Base::Xml::Options );
    if( topNodes.isEmpty() ) return false;

    for(QDomNode node = topNodes.at(0).firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement element = node.toElement();
        if( element.isNull() ) continue;

        // special options
        if( element.tagName() == Base::Xml::SpecialOption )
        {

            // retrieve Value attribute
            QString value( element.attribute( Base::Xml::Value ) );
            if( value.size() ) options.keep( value );

        } else if( element.tagName() == Base::Xml::Option ) {

            XmlOption option( element );
            if( options.isSpecialOption( option.name() ) )
            {

                static QRegExp invalidOptionRegExp( "\\(0b\\d+\\)$" );
                if( invalidOptionRegExp.indexIn( option.raw() ) < 0 )
                { options.add( option.name(), (Option) option ); }

            } else options.set( option.name(), (Option) option );

        }
    }

    return true;

}
