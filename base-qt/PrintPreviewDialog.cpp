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
#include "BaseIconNames.h"
#include "Debug.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "PrintPreviewDialog_p.h"
#include "QtUtil.h"

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
        Counter( QStringLiteral("Private::OptionMenu") )
    {

        {
            auto menu = addMenu( tr( "Orientation" ) );
            orientationActions_ =
            {
                { menu->addAction( tr( "Portrait" ) ), QPageLayout::Portrait },
                { menu->addAction( tr( "Landscape" ) ), QPageLayout::Landscape }
            };

            auto actionGroup = new QActionGroup( this );
            for( auto&& iter = orientationActions_.begin(); iter != orientationActions_.end(); ++iter )
            {
                iter.key()->setCheckable( true );
                actionGroup->addAction( iter.key() );
            }

            connect( actionGroup, &QActionGroup::triggered, this, &OptionMenu::_setOrientation );
            (*menu->actions().begin())->setChecked( true );

        }

        {
            auto menu = addMenu( tr( "Layout" ) );
            pageModeActions_ =
            {
                { menu->addAction( tr( "One Page Per Sheet" ) ), BasePrintHelper::PageMode::SinglePage },
                { menu->addAction( tr( "Two Pages Per Sheet" ) ), BasePrintHelper::PageMode::TwoPages },
                { menu->addAction( tr( "Four Pages Per Sheet" ) ), BasePrintHelper::PageMode::FourPages }
            };

            auto actionGroup = new QActionGroup( this );
            for( auto&& iter = pageModeActions_.begin(); iter != pageModeActions_.end(); ++iter )
            {
                iter.key()->setCheckable( true );
                actionGroup->addAction( iter.key() );
            }

            (*menu->actions().begin())->setChecked( true );
            connect( actionGroup, &QActionGroup::triggered, this, &OptionMenu::_setPageMode );

        }

    }

    //_________________________________________________________________
    void OptionMenu::setOrientation( QPageLayout::Orientation orientation ) const
    {
        for( OrientationActionMap::const_iterator iter = orientationActions_.constBegin(); iter != orientationActions_.constEnd(); ++iter )
        { if( iter.value() == orientation ) { iter.key()->setChecked( true ); break; } }
    }

    //_________________________________________________________________
    void OptionMenu::setPageMode( BasePrintHelper::PageMode pageMode ) const
    {
        for( PageModeActionMap::const_iterator iter = pageModeActions_.constBegin(); iter != pageModeActions_.constEnd(); ++iter )
        { if( iter.value() == pageMode ) { iter.key()->setChecked( true ); break; } }
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
        Counter( QStringLiteral("Private::NavigationWidget") ),
        pages_( 0 )
    {
        Debug::Throw( QStringLiteral("NavigationWidget::NavigationWidget.\n") );

        QHBoxLayout* hLayout = new QHBoxLayout;
        QtUtil::setMargin(hLayout, 0);
        setLayout( hLayout );

        // previous page button
        hLayout->addWidget( previousPageButton_ = new QToolButton( this ) );
        previousPageButton_->setAutoRaise( true );
        previousPageButton_->setIcon( IconEngine::get( IconNames::Previous ) );
        previousPageButton_->setIconSize( IconSize::get( IconSize::Medium ) );
        connect( previousPageButton_, &QAbstractButton::clicked, this, &NavigationWidget::_previousPage );

        // editor
        hLayout->addWidget( editor_ = new QLineEdit( this ) );
        editor_->installEventFilter( this );
        editor_->setText( QStringLiteral("1") );

        // validator
        validator_.setBottom( 1 );
        validator_.setTop( 1 );
        editor_->setValidator( &validator_ );
        editor_->setAlignment( Qt::AlignCenter );
        connect( editor_, &QLineEdit::textEdited, this, &NavigationWidget::_updatePage );

        // label
        hLayout->addWidget( label_ = new QLabel( this ) );

        // next page button
        hLayout->addWidget( nextPageButton_ = new QToolButton( this ) );
        nextPageButton_->setAutoRaise( true );
        nextPageButton_->setIcon( IconEngine::get( IconNames::Next ) );
        nextPageButton_->setIconSize( IconSize::get( IconSize::Medium ) );
        connect( nextPageButton_, &QAbstractButton::clicked, this, &NavigationWidget::_nextPage );

        // pages
        setPages(1);

    }

    //_________________________________________________________________
    void NavigationWidget::setPage( int page )
    {

        Debug::Throw() << "NavigationWidget::setPage - " << page << Qt::endl;
        if( page == editor_->text().toInt() ) return;

        // update editor and buttons
        editor_->setText( QString::number( page ) );
        _updateButtons();

    }

    //_________________________________________________________________
    void NavigationWidget::setPages( int pages )
    {
        Debug::Throw( QStringLiteral("NavigationWidget::setPages.\n") );

        // check value
        if( pages < 1 ) return;

        // store pages
        pages_ = pages;

        // update label
        label_->setText( tr( " of %1" ).arg( pages ) );

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
                int increment( wheelEvent->angleDelta().y()/120 );

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

        Debug::Throw( QStringLiteral("NavigationWidget::_updatePage.\n") );
        int page( editor_->text().toInt() );
        if( page < 1 || page > pages_ ) return;
        _updateButtons();
        emit pageChanged( page );

    }


    //_________________________________________________________________
    void NavigationWidget::_previousPage()
    {

        Debug::Throw( QStringLiteral("NavigationWidget::_previousPage.\n") );
        int page( editor_->text().toInt() );
        if( page < 2 ) return;
        editor_->setText( QString::number( page-1 ) );
        _updateButtons();
        emit pageChanged( page-1 );
    }

    //_________________________________________________________________
    void NavigationWidget::_nextPage()
    {

        Debug::Throw( QStringLiteral("NavigationWidget::_nextPage.\n") );
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
PrintPreviewDialog::PrintPreviewDialog( QWidget* parent, Dialog::Flags flags ):
    Dialog( parent, flags )
{
    Debug::Throw( QStringLiteral("PrintPreviewDialog::PrintPreviewDialog.\n") );
    setOptionName( QStringLiteral("PRINT_PREVIEW_DIALOG") );

    QtUtil::setMargin(layout(), 0);
    QtUtil::setMargin(&buttonLayout(), 5);

    QtUtil::setMargin(&mainLayout(), 0);
    mainLayout().setSpacing(0);

    mainLayout().addWidget( optionMenu_ = new Private::OptionMenu( this ) );
    mainLayout().addWidget( previewWidget_ = new QPrintPreviewWidget( this ) );
    previewWidget_->setZoomMode( QPrintPreviewWidget::FitToWidth );

    mainLayout().addWidget( navigationWidget_ = new Private::NavigationWidget( this ), 0, Qt::AlignCenter );
    connect( navigationWidget_, &Private::NavigationWidget::pageChanged, previewWidget_, &QPrintPreviewWidget::setCurrentPage );

    // close accelerator
    connect( new QShortcut( QKeySequence::Close, this ), &QShortcut::activated, this, &PrintPreviewDialog::close );

    // connect scrollbars
    QList<QGraphicsView*> graphicsViews( previewWidget_->findChildren<QGraphicsView*>() );
    if( !graphicsViews.isEmpty() )
    {

        // connection on vertical scrollbar
        connect( graphicsViews.front()->verticalScrollBar(), &QAbstractSlider::valueChanged, this, &PrintPreviewDialog::_updatePage );

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
void PrintPreviewDialog::setHelper( BasePrintHelper* helper )
{
    Debug::Throw( QStringLiteral("PrintPreviewDialog::setHelper.\n") );

    if( !helper ) return;

    connect( previewWidget_, &QPrintPreviewWidget::paintRequested, helper, &BasePrintHelper::print );
    connect( optionMenu_, &Private::OptionMenu::orientationChanged, helper, &BasePrintHelper::setOrientation );
    connect( optionMenu_, &Private::OptionMenu::orientationChanged, previewWidget_, &QPrintPreviewWidget::updatePreview );
    connect( optionMenu_, &Private::OptionMenu::pageModeChanged, helper, &BasePrintHelper::setPageMode );
    connect( optionMenu_, &Private::OptionMenu::pageModeChanged, previewWidget_, &QPrintPreviewWidget::updatePreview );
    connect( helper, &BasePrintHelper::pageCountChanged, navigationWidget_, &Private::NavigationWidget::setPages );

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
