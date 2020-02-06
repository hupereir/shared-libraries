#ifndef OpenWithComboBox_h
#define OpenWithComboBox_h

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

#include "base_qt_export.h"
#include "Counter.h"
#include "File.h"

#include <QComboBox>
#include <QStringList>

//* Customized QCombobox to handle case sensitive auto completion
class BASE_QT_EXPORT OpenWithComboBox: public QComboBox, private Base::Counter<OpenWithComboBox>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    explicit OpenWithComboBox( QWidget* = nullptr );

    //*@name accessors
    //@{

    //* new items
    const QStringList& newItems() const
    { return newItems_; }

    using QComboBox::count;

    //* return current command
    File command() const;

    //* true if selected item is valid
    bool isItemValid() const
    { return currentIndex() != count()-1; }

    //@}

    //*@name modifiers
    //@{

    void addItem( const QString&, const File& );
    void addItem( const File& );

    //@}

    private:

    //* process item selection
    void _indexActivated( int );

    //* list of new items
    QStringList newItems_;

};

#endif
