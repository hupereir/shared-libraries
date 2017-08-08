#ifndef BasePrintHelper_h
#define BasePrintHelper_h
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

#include "TimeStamp.h"
#include "Debug.h"
#include "File.h"

#include <QObject>
#include <QPainter>
#include <QPrinter>

//* printing utility
class BasePrintHelper: public QObject
{

    Q_OBJECT

    public:

    //* constructor
    explicit BasePrintHelper( QObject* = nullptr );

    //* multipage mode
    enum class PageMode
    {
        SinglePage,
        TwoPages,
        FourPages
    };

    //* setup pages
    void setupPage( QPrinter* );

    //*@name accessors
    //@{
        //* orientation
    QPrinter::Orientation orientation() const
    { return orientation_; }

    //* page mode
    PageMode pageMode() const
    { return pageMode_; }

    //* page number
    int pageNumber() const
    { return pageNumber_; }

    //* page number
    int sheetNumber() const
    { return sheetNumber_; }

    //* file
    const File& file() const
    { return file_; }

    //@}

    //*@name modifiers
    //@{

    //* file
    void setFile( const File& file )
    { file_ = file; }

    //* set page number manually
    void setPageNumber( int value )
    { pageNumber_ = value; }

    //* set sheet number manually
    void setSheetNumber( int value )
    { sheetNumber_ = value; }

    //@}

    public Q_SLOTS:

    //* orientation
    void setOrientation( QPrinter::Orientation value )
    { orientation_ = value; }

    //* mode
    void setPageMode( BasePrintHelper::PageMode value )
    { pageMode_ = value; }

    Q_SIGNALS:

    //* emitted when number of pages has changed
    void pageCountChanged( int );

    protected:

    //* return pageRect
    const QRect& _pageRect() const
    { return pageRect_; }

    //* increment page
    void _newPage( QPrinter*, QPainter* );

    private:

    //* orientation
    QPrinter::Orientation orientation_ = QPrinter::Portrait;

    //* page mode
    PageMode pageMode_ = PageMode::SinglePage;

    //* rects
    QRect pageRect_;
    QRect headerRect_;
    QRect footerRect_;

    //* list of pages viewports
    using QRectList = QList<QRect>;
    QRectList pages_;

    //* page
    int pageNumber_ = 0;

    //* sheet
    int sheetNumber_ = 0;

    //* time stamp
    TimeStamp now_;

    //* file
    File file_;

};

#endif
