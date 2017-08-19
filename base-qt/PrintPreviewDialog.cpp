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

#include "PrintPreviewDialog.h"
#include "PrintPreviewDialog_p.h"

#include "CppUtil.h"
#include "Debug.h"
#include "BaseIconNames.h"
#include "IconEngine.h"
#include "IconSize.h"

#include <QComboBox>
#include <QGraphicsView>
#include <QLayout>
#include <QMenu>
#include <QScrollBar>
#include <QShortcut>
#include <QWheelEvent>

namespace Private
{

    //_________________________________________________________________
    OptionMenu::OptionMenu( QWidget* parent ):
        QMenuBar( parent ),
        Counter( "Private::OptionMenu" )
    {

        {
            QMenu* menu = addMenu( tr( "Orientation" ) );
            orientationActions_ = Base::makeT<OrientationActionMap>(
            {
                { menu->addAction( tr( "Portrait" ) ), QPrinter::Portrait },
                { menu->addAction( tr( "Landscape" ) ), QPrinter::Landscape }
            } );

            QActionGroup* actionGroup = new QActionGroup( this );
            for( auto&& iter = orientationActions_.begin(); iter != orientationActions_.end(); ++iter )
            {
                iter.key()->setCheckable( true );
                actionGroup->addAction( iter.key() );
            }

            connect( actionGroup, SIGNAL(triggered(QAction*)), SLOT(_setOrientation(QAction*)) );
            menu->actions().front()->setChecked( true );

        }

        {
            QMenu* menu = addMenu( tr( "Layout" ) );
            pageModeActions_ = Base::makeT<PageModeActionMap>(
            {
                { menu->addAction( tr( "One Page Per Sheet" ) ), BasePrintHelper::PageMode::SinglePage },
                { menu->addAction( tr( "Two Pages Per Sheet" ) ), BasePrintHelper::PageMode::TwoPages },
                { menu->addAction( tr( "Four Pages Per Sheet" ) ), BasePrintHelper::PageMode::FourPages }
            } );

            QActionGroup* actionGroup = new QActionGroup( this );
            for( auto&& iter = pageModeActions_.begin(); iter != pageModeActions_.end(); ++iter )
            {
                iter.key()->setCheckable( true );
                actionGroup->addAction( iter.key() );
            }

            menu->actions().front()->setChecked( true );
            connect( actionGroup, SIGNAL(triggered(QAction*)), SLOT(_setPageMode(QAction*)) );

        }

    }

    //_________________________________________________________________
    void OptionMenu::setOrientation( QPrinter::Orientation orientation ) const
    {
        for( OrientationActionMap::const_iterator iter = orientationActions_.constBegin(); iter != orientationActions_.constEnd(); ++iter )
        { if( iter.value() == orientation ) iter.key()->setChecked( true ); }
    }

    //_________________________________________________________________
    void OptionMenu::setPageMode( BasePrintHelper::PageMode pageMode ) const
    {
        for( PageModeActionMap::const_iterator iter = pageModeActions_.constBegin(); iter != pageModeActions_.constEnd(); ++iter )
        { if( iter.value() == pageMode ) iter.key()->setChecked( true ); }
    }

    //_________________________________________________________________
    void OptionMenu::_setOrientation( QAction* action )
    { emit orientationChanged( orientationActions_[action] ); }

    //_________________________________________________________________
    void OptionMenu::_setPageMode( QAction* action )
    { emit pageModeChanged( pageModeActions_[action] ); }

    //_________________________________________________________________
    NavigationWidget::NavigationWidget( QWidget* parent ):
        QWidget( parent ),
        Counter( "Private::NavigationWidget" ),
        pages_( 0 )
    {
        Debug::Throw( "NavigationWidget::NavigationWidget.\n" );

        QHBoxLayout* hLayout = new QHBoxLayout;
        hLayout->setMargin(0);
        setLayout( hLayout );

        // previous page button
        hLayout->addWidget( previousPageButton_ = new QToolButton( this ) );
        previousPageButton_->setAutoRaise( true );
        previousPageButton_->setIcon( IconEngine::get( IconNames::Previous ) );
        previousPageButton_->setIconSize( IconSize::get( IconSize::Medium ) );
        connect( previousPageButton_, SIGNAL(clicked()), SLOT(_previousPage()) );

        // editor
        hLayout->addWidget( editor_ = new QLineEdit( this ) );
        editor_->installEventFilter( this );
        editor_->setText( "1" );

        // validator
        validator_.setBottom( 1 );
        validator_.setTop( 1 );
        editor_->setValidator( &validator_ );
        editor_->setAlignment( Qt::AlignCenter );
        connect( editor_, SIGNAL(textEdited(QString)), SLOT(_updatePage()) );

        // label
        hLayout->addWidget( label_ = new QLabel( this ) );

        // next page button
        hLayout->addWidget( nextPageButton_ = new QToolButton( this ) );
        nextPageButton_->setAutoRaise( true );
        nextPageButton_->setIcon( IconEngine::get( IconNames::Next ) );
        nextPageButton_->setIconSize( IconSize::get( IconSize::Medium ) );
        connect( nextPageButton_, SIGNAL(clicked()), SLOT(_nextPage()) );

        // pages
        setPages(1);

    }

    //_________________________________________________________________
    void NavigationWidget::setPage( int page )
    {

        Debug::Throw() << "NavigationWidget::setPage - " << page << endl;
        if( page == editor_->text().toInt() ) return;

        // update editor and buttons
        editor_->setText( QString::number( page ) );
        _updateButtons();

    }

    //_________________________________________________________________
    void NavigationWidget::setPages( int pages )
    {
        Debug::Throw( "NavigationWidget::setPages.\n" );

        // check value
        if( pages < 1 ) return;

        // store pages
        pages_ = pages;

        // update label
        QString buffer;
        QTextStream( &buffer ) << " of " << pages;
        label_->setText( QString( tr( " of %1" ) ).arg( pages ) );

        // update validator
        validator_.setTop( pages );

        // update editor
        if( editor_->text().toInt() > pages ) editor_->setText( QString::number(pages) );

        // update editor
        auto metrics( editor_->fontMetrics() );
        editor_->setFixedSize( metrics.maxWidth()*( editor_->text().size()+1 ), editor_->height() );

        // update buttons
        _updateButtons();

    }

    //_________________________________________________________________
    bool NavigationWidget::eventFilter( QObject* object, QEvent* event )
    {
        if( object != editor_ ) return QWidget::eventFilter( object, event );
        switch( event->type() )
        {
            case QEvent::Wheel:
            {

                // cast
                QWheelEvent* wheelEvent( static_cast<QWheelEvent*>( event ) );
                int increment( wheelEvent->delta()/120 );

                // get page
                const int page( editor_->text().toInt() );
                int newPage = page - increment;
                newPage = qMax( 1, newPage );
                newPage = qMin( newPage, pages_ );
                if( newPage != page )
                {
                    editor_->setText( QString::number( newPage ) );
                    _updateButtons();
                    emit pageChanged( newPage );
                }

                return true;

            }

            default: return QWidget::eventFilter( object, event );
        }

    }

    //_________________________________________________________________
    void NavigationWidget::_updatePage()
    {

        Debug::Throw( "NavigationWidget::_updatePage.\n" );
        int page( editor_->text().toInt() );
        if( page < 1 || page > pages_ ) return;
        _updateButtons();
        emit pageChanged( page );

    }


    //_________________________________________________________________
    void NavigationWidget::_previousPage()
    {

        Debug::Throw( "NavigationWidget::_previousPage.\n" );
        int page( editor_->text().toInt() );
        if( page < 2 ) return;
        editor_->setText( QString::number( page-1 ) );
        _updateButtons();
        emit pageChanged( page-1 );
    }

    //_________________________________________________________________
    void NavigationWidget::_nextPage()
    {

        Debug::Throw( "NavigationWidget::_nextPage.\n" );
        int page( editor_->text().toInt() );

        if( page >= pages_ ) return;
        editor_->setText( QString::number( page+1 ) );

        _updateButtons();
        emit pageChanged( page+1 );

    }

    //_________________________________________________________________
    void NavigationWidget::_updateButtons()
    {
        previousPageButton_->setEnabled( editor_->text().toInt() > 1 );
        nextPageButton_->setEnabled( editor_->text().toInt() < pages_ );
    }

}

//_________________________________________________________________
PrintPreviewDialog::PrintPreviewDialog( QWidget* parent, CustomDialog::Flags flags ):
    CustomDialog( parent, flags )
{
    Debug::Throw( "PrintPreviewDialog::PrintPreviewDialog.\n" );
    setOptionName( "PRINT_PREVIEW_DIALOG" );

    layout()->setMargin(0);
    buttonLayout().setMargin(5);

    mainLayout().setMargin(0);
    mainLayout().setSpacing(0);

    mainLayout().addWidget( optionMenu_ = new Private::OptionMenu( this ) );
    mainLayout().addWidget( previewWidget_ = new QPrintPreviewWidget( this ) );
    previewWidget_->setZoomMode( QPrintPreviewWidget::FitToWidth );

    mainLayout().addWidget( navigationWidget_ = new Private::NavigationWidget( this ), 0, Qt::AlignCenter );
    connect( navigationWidget_, SIGNAL(pageChanged(int)), previewWidget_, SLOT(setCurrentPage(int)) );

    // close accelerator
    new QShortcut( QKeySequence::Close, this, SLOT(close()) );

    // connect scrollbars
    QList<QGraphicsView*> graphicsViews( previewWidget_->findChildren<QGraphicsView*>() );
    if( !graphicsViews.isEmpty() )
    {

        // connection on vertical scrollbar
        connect( graphicsViews.front()->verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(_updatePage()) );

        // hide horizontal scrollbar
        graphicsViews.front()->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    }

    // customize buttons
    if( hasOkButton() )
    {
        okButton().setText( tr( "Print ..." ) );
        okButton().setIcon( IconEngine::get( IconNames::Print ) );
    }

    resize( 600, 700 );
}

//_________________________________________________________________
void PrintPreviewDialog::setHelper( QObject* helper )
{
    Debug::Throw( "PrintPreviewDialog::setHelper.\n" );

    if( !helper ) return;

    connect( previewWidget_, SIGNAL(paintRequested(QPrinter*)), helper, SLOT(print(QPrinter*)) );
    connect( optionMenu_, SIGNAL(orientationChanged(QPrinter::Orientation)), helper, SLOT(setOrientation(QPrinter::Orientation)) );
    connect( optionMenu_, SIGNAL(orientationChanged(QPrinter::Orientation)), previewWidget_, SLOT(updatePreview()) );
    connect( optionMenu_, SIGNAL(pageModeChanged(BasePrintHelper::PageMode)), helper, SLOT(setPageMode(BasePrintHelper::PageMode)) );
    connect( optionMenu_, SIGNAL(pageModeChanged(BasePrintHelper::PageMode)), previewWidget_, SLOT(updatePreview()) );
    connect( helper, SIGNAL(pageCountChanged(int)), navigationWidget_, SLOT(setPages(int)) );

}

//_________________________________________________________________
void PrintPreviewDialog::showMenu()
{ if( optionMenu_ ) optionMenu_->show(); }

//_________________________________________________________________
void PrintPreviewDialog::hideMenu()
{ if( optionMenu_ ) optionMenu_->hide(); }

//_________________________________________________________________
void PrintPreviewDialog::_updatePage()
{ navigationWidget_->setPage( previewWidget_->currentPage() ); }
