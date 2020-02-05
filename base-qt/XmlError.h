#ifndef XmlError_h
#define XmlError_h

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

//* Xml parsing error container
class XmlError final: private Base::Counter<XmlError>
{

    public:

    //* constructor
    explicit XmlError( const File& file = File() ):
        Counter( QStringLiteral("XmlError") ),
        file_ ( file )
    {}

    //*@name accessors
    //@{

    //* cast to boolean
    operator bool() const
    { return !error_.isEmpty(); }

    //* File
    const File& file() const
    { return file_; }

    //* error
    const QString& error() const
    { return error_; }

    //* line
    int line() const
    { return line_; }

    //* column
    int column() const
    { return column_; }

    //* error
    QString toString() const
    {

        QString out;
        if( !file_.isEmpty() ) out = QString( "%1: " ).arg( file_ );
        out += QString( QObject::tr( "message: %1 at line %2, column %3" ) ).arg( error_ ).arg( line_ ).arg( column_ );
        return out;

    }

    //@}

    //*@name modifiers
    //@{

    //* clear
    void clear()
    {
        file_.clear();
        error_.clear();
        line_ = -1;
        column_ = -1;
    }

    //* set file
    void setFile( const File& file )
    { file_ = file; }

        //* error
    QString& error()
    { return error_; }

    //* line
    int& line()
    { return line_; }

    //* column
    int& column()
    { return column_; }

    //@}

    //* list
    using List = QList<XmlError>;

    //* write error list to string
    static QString toString( const List& list )
    {
        QStringList out;
        for( const auto& error:list )
        { out.append( error.toString() ); }

        return out.join( "\n" );
    }

    private:

    //* file
    File file_;

    //* error
    QString error_;

    //* line
    int line_ = 0;

    //* column
    int column_ = 0;

    //* dumper
    friend QTextStream& operator << ( QTextStream &out, const XmlError& error )
    {
        out << error.toString();
        return out;
    }

    //* dumper
    friend QTextStream& operator << ( QTextStream &out, const List& errors )
    {
        out << XmlError::toString( errors ) << endl;
        return out;
    }

};

#endif
