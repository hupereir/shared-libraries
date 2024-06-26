#ifndef CommandLineArguments_h
#define CommandLineArguments_h

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
#include "base_export.h"

#include <QStringList>

class BASE_EXPORT CommandLineArguments: private Base::Counter<CommandLineArguments>
{

    public:

    //* constructor
    explicit CommandLineArguments( int argc = 0, char* argv[] = nullptr );

    //* constructor
    explicit CommandLineArguments( const QStringList& );

    //* constructor
    explicit CommandLineArguments( QStringList&& );

    //* constructor
    explicit CommandLineArguments( std::initializer_list<QString>&& );

    //* destructor
    virtual ~CommandLineArguments() = default;

    //*@name accessors
    //@{

    const QStringList& get() const { return arguments_; }

    //* empty
    bool isEmpty() const { return arguments_.isEmpty(); }

    //* size
    int size() const { return arguments_.size(); }

    const QString& operator[] (int i) const { return arguments_[i]; }

    //@}

    //*@name modifiers
    //@{

    QStringList& get() { return arguments_; }

    //* append
    template<class T>
    void append( const T& t ) { arguments_.append(t); }

    //* streamers
    template<class T>
    CommandLineArguments & operator<< ( const T& t )
    {
        arguments_ << t;
        return *this;
    }

    //@}

    private:

    //* arguments
    QStringList arguments_;

    //* streamer
    friend BASE_EXPORT QTextStream& operator << (QTextStream& out, const CommandLineArguments& argument )
    {
        for( const auto& arg:argument.arguments_ )
        { out << arg << " "; }
        return out;
    }

};

#endif
