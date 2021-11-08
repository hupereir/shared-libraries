#ifndef TreeViewConfiguration_h
#define TreeViewConfiguration_h

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

#include "OptionWidget.h"
#include "base_qt_export.h"

#include <QGroupBox>
#include <QCheckBox>
#include <QTreeView>
#include <QList>

//* Configuration vbox for TreeView columns
class BASE_QT_EXPORT TreeViewConfiguration: public QGroupBox, public OptionWidget
{

    Q_OBJECT

    public:

    //* constructor
    explicit TreeViewConfiguration( QWidget*, QTreeView*, const QString&  );

    //* read value from option
    void read( const Options& ) override;

    //* write value to option
    void write( Options& ) const override;

    Q_SIGNALS:

    void modified();

    private:

    //* checkboxes
    QList< QCheckBox* > checkbox_;

};
#endif
