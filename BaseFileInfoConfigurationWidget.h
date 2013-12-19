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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "BaseFileInfoToolTipWidget.h"
#include "OptionWidget.h"

#include <QWidget>
#include <QCheckBox>
#include <QMap>

class BaseFileInfoConfigurationWidget: public QWidget, public OptionWidget
{

    Q_OBJECT

    public:

    //! constructor
    BaseFileInfoConfigurationWidget( QWidget* );

    //! destructo
    virtual ~BaseFileInfoConfigurationWidget( void )
    {}

    //! set widget value from option value
    virtual void read( const Options& );

    //! set option value from widget value
    virtual void write( Options& ) const;

    Q_SIGNALS:

    //! modified
    void modified( void );

    protected:

    //! add checkbox
    void _addCheckBox( BaseFileInfoToolTipWidget::Type, const QString& );

    private:

    //! map mask to checkboxes
    typedef QMap<BaseFileInfoToolTipWidget::Type, QCheckBox* > CheckBoxMap;
    CheckBoxMap checkboxes_;

};

#endif
