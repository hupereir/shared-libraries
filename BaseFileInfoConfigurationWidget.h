#ifndef BaseFileInfoConfigurationWidget_h
#define BaseFileInfoConfigurationWidget_h

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
*******************************************************************************/

#include "BaseFileInfoToolTipWidget.h"
#include "OptionWidget.h"

#include <QtGui/QWidget>
#include <QtGui/QCheckBox>
#include <QtCore/QMap>

class BaseFileInfoConfigurationWidget: public QWidget, public OptionWidget
{

    public:

    //! constructor
    BaseFileInfoConfigurationWidget( QWidget* );

    //! destructo
    virtual ~BaseFileInfoConfigurationWidget( void )
    {}

    //! set widget value from option value
    virtual void read( void );

    //! set option value from widget value
    virtual void write( void ) const;

    protected:

    //! add checkbox
    void _addCheckBox( BaseFileInfoToolTipWidget::Type, const QString& );

    private:

    //! map mask to checkboxes
    typedef QMap<BaseFileInfoToolTipWidget::Type, QCheckBox* > CheckBoxMap;
    CheckBoxMap checkboxes_;

};

#endif
