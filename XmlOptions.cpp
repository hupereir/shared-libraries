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


/*!
\file XmlOptions.cpp
\brief Option file parser based on xml
\author Hugo Pereira
\version $Revision$
\date $Date$
*/


#include "File.h"
#include "Options.h"
#include "XmlError.h"
#include "XmlOption.h"
#include "XmlOptions.h"

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtCore/QFile>

//____________________________________________________________________
QString& XmlOptions::file( void )
{
    static QString file;
    return file;
}

//____________________________________________________________________
XmlError& XmlOptions::error( void )
{
    static XmlError error;
    return error;
}

//____________________________________________________________________
Options& XmlOptions::get( void )
{
    static Options singleton( true );
    return singleton;
}

//____________________________________________________________________
bool XmlOptions::read( QString file )
{

    Debug::Throw() << "XmlOptions::read - file=\"" << file << "\"\n";

    // check filename is valid
    if( file.isEmpty() ) file = XmlOptions::file();
    if( file.isEmpty() ) return false;

    // store requested file
    XmlOptions::file() = file;

    // parse the file
    QFile qtfile( file );
    if ( !qtfile.open( QIODevice::ReadOnly ) )
    {
        Debug::Throw( "XmlOptions::read - cannot open file.\n" );
        return false;
    }

    // dom document
    QDomDocument document;
    error() = XmlError( file );
    if ( !document.setContent( &qtfile, &error().error(), &error().line(), &error().column() ) )
    {
        qtfile.close();
        return false;
    }

    QDomElement doc_element = document.documentElement();
    QDomNode node = doc_element.firstChild();
    for(QDomNode node = doc_element.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement element = node.toElement();
        if( element.isNull() ) continue;

        // special options
        if( element.tagName() == OPTIONS::SPECIAL_OPTION )
        {

            Debug::Throw( "XmlOptions::read - special options" );

            // retrieve Value attribute
            QString value( element.attribute( OPTIONS::VALUE ) );
            if( value.size() ) get().keep( value );

        } else if( element.tagName() == OPTIONS::OPTION ) {

            XmlOption option( element );
            if( get().isSpecialOption( option.name() ) ) get().add( option.name(), option );
            else get().set( option.name(), option );

        } else {

            // old implementation, kept for backward compatibility
            // element name is the option name
            XmlOption option( element );
            if( get().isSpecialOption( option.name() ) ) get().add( option.name(), option );
            else get().set( option.name(), option );

        }

    }


    return true;

}

//________________________________________________
bool XmlOptions::write( QString file )
{

    Debug::Throw() << "XmlOptions::write - " << file << endl;

    // check filename is valid
    if( file.isEmpty() ) file = XmlOptions::file();
    if( file.isEmpty() )
    {
        Debug::Throw( "XmlOptions::write - file is empty.\n" );
        return false;
    }

    // output file
    QFile out( file );
    if( !out.open( QIODevice::WriteOnly ) )
    {
        Debug::Throw() << "XmlOptions::write - cannot write to file " << file << endl;
        return false;
    }

    // create document
    QDomDocument document;

    // create main element
    QDomElement top = document.appendChild( document.createElement( OPTIONS::OPTIONS ) ).toElement();

    // write list of special option names
    for( Options::SpecialMap::const_iterator iter = get().specialOptions().begin(); iter != get().specialOptions().end(); ++iter )
    {
        // check size of options
        if( iter->second.empty() ) continue;

        // dump option name
        QDomElement element = document.createElement( OPTIONS::SPECIAL_OPTION );
        element.setAttribute( OPTIONS::VALUE, iter->first );
        top.appendChild( element );
    }

    // write options
    for( Options::SpecialMap::const_iterator iter = get().specialOptions().begin(); iter != get().specialOptions().end(); ++iter )
    {

        Options::List option_list( iter->second );
        for( Options::List::iterator listIter = option_list.begin(); listIter != option_list.end(); ++listIter )
        {

            if( !listIter->hasFlag( Option::RECORDABLE ) )
            { Debug::Throw(0) << "XmlOptions::write - option " << iter->first << " is not recordable" << endl; }

            if( listIter->hasFlag( Option::RECORDABLE ) && listIter->set() && listIter->raw().size() )
            {

                top.appendChild( XmlOption( iter->first, *listIter ).domElement( document ) );

            } else Debug::Throw(0) << "XmlOptions::write - skipping option " << iter->first << endl;

        }

    }

    // write standard options
    for( Options::Map::const_iterator iter = get().options().begin(); iter != get().options().end(); ++iter )
    {

        if( iter->second.hasFlag( Option::RECORDABLE ) && iter->second.set() && iter->second.raw().size() )
        { top.appendChild( XmlOption( iter->first, iter->second ).domElement( document ) ); }

    }

    out.write( document.toByteArray() );
    out.close();

    Debug::Throw( "XmlOptions::write - done.\n" );
    return true;

}
