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

#include "base_qt_export.h"
#include "Counter.h"
#include "BasePrintHelper.h"

#include <QHash>
#include <QAbstractButton>
#include <QPrinter>
#include <QPushButton>
#include <QWidget>

class BASE_QT_EXPORT PrinterOptionWidget: public QWidget, private Base::Counter<PrinterOptionWidget>
{

    Q_OBJECT

    public:

    //* constructor
    explicit PrinterOptionWidget( QWidget* = nullptr );

    //* orientation
    QPrinter::Orientation orientation() const
    { return orientation_; }

    //* page mode
    BasePrintHelper::PageMode pageMode() const
    { return pageMode_; }

    //* helper
    void setHelper( BasePrintHelper* helper );

    Q_SIGNALS:

    //* emitted when orientation is changed
    void orientationChanged( QPrinter::Orientation );

    //* emitted when pageMode is changed
    void pageModeChanged( BasePrintHelper::PageMode );

    private:

    //* orientation
    void _setOrientation( QAbstractButton* );

    //* page mode
    void _setPageMode( QAbstractButton* );

    //* preview
    void _preview();

    //* helper
    BasePrintHelper* helper_ = nullptr;

    //* orientation
    QPrinter::Orientation orientation_ = QPrinter::Portrait;

    //* page mode
    BasePrintHelper::PageMode pageMode_ = BasePrintHelper::PageMode::SinglePage;

    //* preview button
    QPushButton* previewButton_;

    //* Buttons maps
    using OrientationButtonMap = QHash<QAbstractButton*, QPrinter::Orientation>;
    OrientationButtonMap orientationButtons_;

    //* Buttons map
    using PageModeButtonMap = QHash<QAbstractButton*, BasePrintHelper::PageMode>;
    PageModeButtonMap pageModeButtons_;

};

#endif
