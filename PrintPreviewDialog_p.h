#ifndef PrintPreviewDialog_p_h
#define PrintPreviewDialog_p_h

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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
****************************************************************************/

#include "BasePrintHelper.h"
#include "CustomDialog.h"

#include <QtGui/QPrintPreviewWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>
#include <QtGui/QLabel>
#include <QtGui/QIntValidator>


namespace PRINT
{

    //! handles page orientation and page mode
    class OptionWidget: public QWidget, public Counter
    {

        Q_OBJECT

        public:

        //! constructor
        OptionWidget( QWidget* );

        //! destructor
        virtual ~OptionWidget( void )
        {}

        signals:
        void orientationChanged( QPrinter::Orientation );
        void pageModeChanged( BasePrintHelper::PageMode );

        protected slots:

        //! orientation changed
        void _setOrientation( int );

        //! page mode
        void _setPageMode( int );

    };

    //! handles page navigation
    class NavigationWidget: public QWidget, public Counter
    {

        Q_OBJECT

        public:

        //! constructor
        NavigationWidget( QWidget* );

        //! destructor
        virtual ~NavigationWidget( void )
        {}

        int pages( void )
        { return pages_; }

        //! set page
        void setPage( int );

        //! event filter
        bool eventFilter( QObject*, QEvent* );

        public slots:

        //! set pages
        void setPages( int );

        signals:

        //! new page selected
        void pageChanged( int );

        protected slots:

        //! previous page
        void _previousPage( void );

        //! next page
        void _nextPage( void );

        //! update page
        void _updatePage( void );

        //! update buttons
        void _updateButtons( void );

        private:

        //! number of pages
        int pages_;

        //! editor
        QLineEdit* editor_;

        //! previous page button
        QToolButton* previousPageButton_;

        //! next page button
        QToolButton* nextPageButton_;

        //! label
        QLabel* label_;

        //! valudator
        QIntValidator validator_;

    };

}

#endif
