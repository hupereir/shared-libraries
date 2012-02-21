#ifndef _TreeViewConfiguration_h_
#define _TreeViewConfiguration_h_

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

#include "OptionWidget.h"

#include <QtGui/QGroupBox>
#include <QtGui/QCheckBox>
#include <QtGui/QTreeView>
#include <QtCore/QList>

//! Configuration vbox for TreeView columns
class TreeViewConfiguration: public QGroupBox, public OptionWidget
{

    public:

    //! constructor
    TreeViewConfiguration( QWidget* parent, QTreeView* target, const QString& option_name  );

    //! read value from option
    void read( void );

    //! write value to option
    void write( void ) const;

    private:

    //! checkboxes
    QList< QCheckBox* > checkbox_;

};
#endif
