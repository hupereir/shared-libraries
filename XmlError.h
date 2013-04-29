#ifndef XmlError_h
#define XmlError_h

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
#include "File.h"

#include <QList>
#include <QString>
#include <QStringList>
#include <QTextStream>

//! Xml parsing error container
class XmlError: public Counter
{

    public:

    //! constructor
    XmlError( const File& file = File() ):
        Counter( "XmlError" ),
        file_ ( file ),
        line_(0),
        column_(0)
    {}

    //!@name accessors
    //@{

    //! cast to boolean
    operator bool(void) const
    { return error_.length() != 0; }

    //! File
    const QString& file( void ) const
    { return file_; }

    //! error
    const QString& error( void ) const
    { return error_; }

    //! line
    const int& line( void ) const
    { return line_; }

    //! column
    const int& column( void ) const
    { return column_; }

    //! error
    QString toString( void ) const
    {

        QString out;
        if( !file_.isEmpty() ) out = QString( "%1: " ).arg( file_ );
        out += QString( QObject::tr( "message: %1 at line %2, column %3" ) ).arg( error_ ).arg( line_ ).arg( column_ );
        return out;

    }

    //@}

    //!@name modifiers
    //@{

    //! clear
    void clear( void )
    {
        file_.clear();
        error_.clear();
        line_ = -1;
        column_ = -1;
    }

    //! set file
    void setFile( const File& file )
    { file_ = file; }

        //! error
    QString& error( void )
    { return error_; }

    //! line
    int& line( void )
    { return line_; }

    //! column
    int& column( void )
    { return column_; }

    //@}


    class List: public QList<XmlError>
    {

        public:

        //! constructor
        List( void )
        {}

        //! constructor
        List( const QList<XmlError>& other ):
            QList<XmlError>( other )
        {}

        //! destructor
        virtual ~List( void )
        {}

        //! convert to string
        QString toString( void ) const
        {
            QStringList out;
            foreach( const XmlError& error, *this )
            { out << error.toString(); }
            return out.join( "\n" );
        }

    };

    private:

    //! file
    File file_;

    //! error
    QString error_;

    //! line
    int line_;

    //! column
    int column_;

    //! dumper
    friend QTextStream& operator << ( QTextStream &out, const XmlError& error )
    {
        out << error.toString();
        return out;
    }

    //! dumper
    friend QTextStream& operator << ( QTextStream &out, const List& errors )
    {
        out << errors.toString() << endl;
        return out;
    }

};

#endif
