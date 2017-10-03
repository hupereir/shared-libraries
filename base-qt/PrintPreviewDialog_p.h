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

#include <QHash>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QPrintPreviewWidget>
#include <QToolButton>

namespace Private
{

    //* handles page orientation and page mode
    class OptionMenu: public QMenuBar, private Base::Counter<OptionMenu>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit OptionMenu( QWidget* );

        //* set orientation
        void setOrientation( QPrinter::Orientation ) const;

        //* set page mode
        void setPageMode( BasePrintHelper::PageMode ) const;

        Q_SIGNALS:

        void orientationChanged( QPrinter::Orientation );

        void pageModeChanged( BasePrintHelper::PageMode );

        private Q_SLOTS:

        //* orientation changed
        void _setOrientation( QAction* );

        //* page mode
        void _setPageMode( QAction* );

        private:

        //* actions maps
        using OrientationActionMap = QHash<QAction*, QPrinter::Orientation>;
        OrientationActionMap orientationActions_;

        //* actions map
        using PageModeActionMap = QHash<QAction*, BasePrintHelper::PageMode>;
        PageModeActionMap pageModeActions_;

    };

    //* handles page navigation
    class NavigationWidget: public QWidget, private Base::Counter<NavigationWidget>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit NavigationWidget( QWidget* );

        int pages()
        { return pages_; }

        //* set page
        void setPage( int );

        //* event filter
        bool eventFilter( QObject*, QEvent* ) override;

        public Q_SLOTS:

        //* set pages
        void setPages( int );

        Q_SIGNALS:

        //* new page selected
        void pageChanged( int );

        protected Q_SLOTS:

        //* previous page
        void _previousPage();

        //* next page
        void _nextPage();

        //* update page
        void _updatePage();

        //* update buttons
        void _updateButtons();

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
