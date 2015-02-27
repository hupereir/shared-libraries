#ifndef PrinterOptionWidget_h
#define PrinterOptionWidget_h
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
#include "BasePrintHelper.h"

#include <QMap>
#include <QAbstractButton>
#include <QPrinter>
#include <QPushButton>
#include <QWidget>

class PrinterOptionWidget: public QWidget, public Counter
{

    Q_OBJECT

    public:

    //* constructor
    PrinterOptionWidget( QWidget* = 0 );

    //* destructor
    virtual ~PrinterOptionWidget( void )
    {}

    //* orientation
    QPrinter::Orientation orientation( void ) const
    { return orientation_; }

    //* page mode
    BasePrintHelper::PageMode pageMode( void ) const
    { return pageMode_; }

    //* helper
    void setHelper( BasePrintHelper* helper );

    Q_SIGNALS:

    //* emmited when orientation is changed
    void orientationChanged( QPrinter::Orientation );

    //* emmited when pageMode is changed
    void pageModeChanged( BasePrintHelper::PageMode );

    protected Q_SLOTS:

    //* orientation
    void _setOrientation( QAbstractButton* );

    //* page mode
    void _setPageMode( QAbstractButton* );

    //* preview
    void _preview( void );

    private:

    //* helper
    BasePrintHelper* helper_;

    //* orientation
    QPrinter::Orientation orientation_;

    //* page mode
    BasePrintHelper::PageMode pageMode_;

    //* preview button
    QPushButton* previewButton_;

    //* Buttons maps
    using OrientationButtonMap = QMap<QAbstractButton*, QPrinter::Orientation>;
    OrientationButtonMap orientationButtons_;

    //* Buttons map
    using PageModeButtonMap = QMap<QAbstractButton*, BasePrintHelper::PageMode>;
    PageModeButtonMap pageModeButtons_;

};

#endif
