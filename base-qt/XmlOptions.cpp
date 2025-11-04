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
#include "Operators.h"
#include "Options.h"
#include "XmlDocument.h"
#include "XmlOption.h"
#include "XmlOptions_p.h"


#include <QFile>
#include <QRegularExpression>

//____________________________________________________________________
Private::XmlOptionsSingleton& XmlOptions::_singleton()
{
    static Private::XmlOptionsSingleton singleton;
    return singleton;
}

namespace Private
{

    //____________________________________________________________________
    bool XmlOptionsSingleton::_differs( const Options& first, const Options& second ) const
    {

        // check special options
        for( auto&& firstIter = first.specialOptions().constBegin(); firstIter != first.specialOptions().constEnd(); ++firstIter )
        {

            // skip empty special options
            if( firstIter.value().isEmpty() ) continue;

            // get matching options in the second set
            if( !second.isSpecialOption( firstIter.key() ) ) return true;
            auto options( second.specialOptions( firstIter.key() ) );

            // loop over options in first list
            for( const auto& option:firstIter.value() )
            {
                // skip non recordable options
                if( !option.isRecordable() ) continue;

                // find in second list
                if( !options.contains( option ) ) return true;

            }

        }

        // loop over options and check existence in other map
        for( auto&& firstIter = first.options().constBegin(); firstIter != first.options().constEnd(); ++firstIter )
        {

            // skip non recordable options
            if( !firstIter.value().isRecordable() ) continue;

            const auto secondIter( second.options().constFind( firstIter.key() ) );
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
{ return _singleton().file(); }

//____________________________________________________________________
const XmlError& XmlOptions::error()
{ return _singleton().error_; }

//____________________________________________________________________
void XmlOptions::setFile( const File& file )
{

    // make sure file is hidden (windows only)
    if( !_singleton().setFile( file ) ) return;
    if( _singleton().file().localName().startsWith( '.' ) )
    { _singleton().file().setHidden(); }

}

//____________________________________________________________________
Options& XmlOptions::get()
{ return _singleton().options_; }

//____________________________________________________________________
bool XmlOptions::read()
{

    // check filename is valid
    if( _singleton().file().isEmpty() ) return false;

    // parse the file
    XmlDocument document;
    QFile qtfile(_singleton().file());
    if ( !qtfile.open(QIODevice::ReadOnly) ) return false;
    if ( !document.setContent( &qtfile, _singleton().error_ ) ) return false;
    else return _read( document, _singleton().options_ );

}

//________________________________________________
bool XmlOptions::write()
{

    // check filename is valid
    if( _singleton().file().isEmpty() ) return false;

    // create document and read
    XmlDocument document;
    {
        QFile qtfile(_singleton().file());
        if( !qtfile.open(QIODevice::ReadOnly) ) return false;
        document.setContent( &qtfile );
    }

    // read old options from xml document
    Options options;
    _read( document, options );
    if( !_singleton().differs( options ) ) return true;

    // create main element
    auto top = document.createElement( Base::Xml::Options ).toElement();

    // write options
    for( auto&& iter = _singleton().options_.specialOptions().begin(); iter != _singleton().options_.specialOptions().end(); ++iter )
    {

        for( const auto& option:iter.value() )
        {

            if( option.hasFlag( Option::Flag::Recordable ) && option.isSet() )
            { top.appendChild( XmlOption( iter.key(), option ).domElement( document.get() ) ); }

        }

    }

    // write standard options
    for( auto&& iter = _singleton().options_.options().begin(); iter != _singleton().options_.options().end(); ++iter )
    {

        if( iter.value().hasFlag( Option::Flag::Recordable ) && iter.value().isSet() && !iter.value().isDefault() )
        { top.appendChild( XmlOption( iter.key(), iter.value() ).domElement( document.get() ) ); }

    }

    // append topNode to document and write
    document.replaceChild( top );

    // write
    {
        QFile qfile( _singleton().file() );
        if( !qfile.open( QIODevice::WriteOnly ) ) return false;
        qfile.write( document.toByteArray() );
    }

    return true;

}

//________________________________________________
bool XmlOptions::_read( const XmlDocument& document, Options& options )
{

    // look for relevant element
    auto topNodes = document.elementsByTagName( Base::Xml::Options );
    if( topNodes.isEmpty() ) return false;

    for(auto node = topNodes.at(0).firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        auto element = node.toElement();
        if( element.isNull() ) continue;

        // special options
        if( element.tagName() == Base::Xml::SpecialOption )
        {

            // retrieve Value attribute
            auto value( element.attribute( Base::Xml::Value ) );
            if( value.size() ) options.keep( value );

        } else if( element.tagName() == Base::Xml::Option ) {

            XmlOption option( element );
            if( options.isSpecialOption( option.name() ) )
            {

                static const QRegularExpression invalidOptionRegExp( QStringLiteral("\\(0b\\d+\\)$") );
                if( !invalidOptionRegExp.match( option.raw() ).hasMatch() )
                { options.add( option.name(), (Option) option ); }

            } else options.set( option.name(), (Option) option );

        }
    }

    return true;

}
