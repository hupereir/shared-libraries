#ifndef PrintPreviewDialog_p_h
#define PrintPreviewDialog_p_h
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

#include "BasePrintHelper.h"
#include "CustomDialog.h"

#include <QMap>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QPrintPreviewWidget>
#include <QToolButton>

namespace Print
{

    //* handles page orientation and page mode
    class OptionMenu: public QMenuBar, public Counter
    {

        Q_OBJECT

        public:

        //* constructor
        OptionMenu( QWidget* );

        //* set orientation
        void setOrientation( QPrinter::Orientation ) const;

        //* set page mode
        void setPageMode( BasePrintHelper::PageMode ) const;

        Q_SIGNALS:

        void orientationChanged( QPrinter::Orientation );

        void pageModeChanged( BasePrintHelper::PageMode );

        protected Q_SLOTS:

        //* orientation changed
        void _setOrientation( QAction* );

        //* page mode
        void _setPageMode( QAction* );

        private:

        //* actions maps
        using OrientationActionMap = QMap<QAction*, QPrinter::Orientation>;
        OrientationActionMap orientationActions_;

        //* actions map
        using PageModeActionMap = QMap<QAction*, BasePrintHelper::PageMode>;
        PageModeActionMap pageModeActions_;

    };

    //* handles page navigation
    class NavigationWidget: public QWidget, public Counter
    {

        Q_OBJECT

        public:

        //* constructor
        NavigationWidget( QWidget* );

        int pages( void )
        { return pages_; }

        //* set page
        void setPage( int );

        //* event filter
        bool eventFilter( QObject*, QEvent* );

        public Q_SLOTS:

        //* set pages
        void setPages( int );

        Q_SIGNALS:

        //* new page selected
        void pageChanged( int );

        protected Q_SLOTS:

        //* previous page
        void _previousPage( void );

        //* next page
        void _nextPage( void );

        //* update page
        void _updatePage( void );

        //* update buttons
        void _updateButtons( void );

        private:

        //* number of pages
        int pages_ = 0;

        //* editor
        QLineEdit* editor_ = nullptr;

        //* previous page button
        QToolButton* previousPageButton_ = nullptr;

        //* next page button
        QToolButton* nextPageButton_ = nullptr;

        //* label
        QLabel* label_ = nullptr;

        //* valudator
        QIntValidator validator_;

    };

}

#endif