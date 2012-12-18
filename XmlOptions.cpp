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

#include "Options.h"
#include "XmlDocument.h"
#include "XmlError.h"
#include "XmlOption.h"

#include <QtCore/QFile>

//____________________________________________________________________
class XmlOptionsSingleton
{
    public:

    //! constructor
    XmlOptionsSingleton( void )
    { options_.installDefaultOptions(); }

    //! options
    Options options_;

    //! backup
    Options backup_;

    //! file
    File file_;

    //! error
    XmlError error_;

};

//____________________________________________________________________
XmlOptionsSingleton XmlOptions::singleton_;

//____________________________________________________________________
const File& XmlOptions::file()
{ return singleton_.file_; }

//____________________________________________________________________
const XmlError& XmlOptions::error()
{ return singleton_.error_; }

//____________________________________________________________________
void XmlOptions::setError( const XmlError& error )
{ singleton_.error_ = error; }

//____________________________________________________________________
void XmlOptions::setFile( const File& file )
{
    if( singleton_.file_ == file ) return;
    singleton_.file_ = file;

    // make sure file is hidden (windows only)
    if( singleton_.file_.localName().startsWith( '.' ) )
    { singleton_.file_.setHidden(); }

}

//____________________________________________________________________
Options& XmlOptions::get( void )
{ return singleton_.options_; }

//____________________________________________________________________
bool XmlOptions::read( void )
{

    Debug::Throw() << "XmlOptions::read - file: " << singleton_.file_ << endl;

    // check filename is valid
    if( singleton_.file_.isEmpty() ) return false;

    // parse the file
    XmlDocument document;
    {
        QFile qtfile( singleton_.file_ );
        XmlError error( singleton_.file_ );
        if ( !document.setContent( &qtfile, &error.error(), &error.line(), &error.column() ) )
        {
            setError( error );
            return false;
        }
    }

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
            if( value.size() ) get().keep( value );

        } else if( element.tagName() == BASE::XML::OPTION ) {

            XmlOption option( element );
            if( get().isSpecialOption( option.name() ) ) get().add( option.name(), option );
            else get().set( option.name(), option );

        }

    }

    return true;

}

//________________________________________________
bool XmlOptions::write( void )
{

    Debug::Throw() << "XmlOptions::write - file: " << singleton_.file_ << endl;

    // check filename is valid
    if( singleton_.file_.isEmpty() ) return false;

    // create document and read
    XmlDocument document;
    {
        QFile qtfile( singleton_.file_ );
        document.setContent( &qtfile );
    }

    // create main element
    QDomElement top = document.createElement( BASE::XML::OPTIONS ).toElement();

    // write list of special option names
    for( Options::SpecialMap::const_iterator iter = get().specialOptions().begin(); iter != get().specialOptions().end(); ++iter )
    {
        // check size of options
        if( iter.value().empty() ) continue;

        // dump option name
        QDomElement element = document.createElement( BASE::XML::SPECIAL_OPTION );
        element.setAttribute( BASE::XML::VALUE, iter.key() );
        top.appendChild( element );
    }

    // write options
    for( Options::SpecialMap::const_iterator iter = get().specialOptions().begin(); iter != get().specialOptions().end(); ++iter )
    {

        Options::List option_list( iter.value() );
        for( Options::List::iterator listIter = option_list.begin(); listIter != option_list.end(); ++listIter )
        {

            if( listIter->hasFlag( Option::Recordable ) && listIter->set() && listIter->raw().size() )
            { top.appendChild( XmlOption( iter.key(), *listIter ).domElement( document ) ); }

        }

    }

    // write standard options
    for( Options::Map::const_iterator iter = get().options().begin(); iter != get().options().end(); ++iter )
    {

        if( iter.value().hasFlag( Option::Recordable ) &&
            iter.value().set() &&
            iter.value().raw().size() &&
            iter.value().raw() != iter.value().defaultValue() )
        { top.appendChild( XmlOption( iter.key(), iter.value() ).domElement( document ) ); }

    }

    // append topNode to document and write
    document.replaceChild( top );

    // write
    {
        QFile qfile( singleton_.file_ );
        if( !qfile.open( QIODevice::WriteOnly ) ) return false;
        qfile.write( document.toByteArray() );
    }

    return true;

}
