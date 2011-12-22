
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

#include "PrintPreviewDialog.h"
#include "PrintPreviewDialog_p.h"

#include "BasePrintHelper.h"
#include "Debug.h"
#include "BaseIcons.h"
#include "IconEngine.h"

#include <QtGui/QLayout>
#include <QtGui/QGraphicsView>
#include <QtGui/QScrollBar>
#include <QtGui/QShortcut>
#include <QtGui/QWheelEvent>

namespace PRINT
{

    //_________________________________________________________________
    NavigationWidget::NavigationWidget( QWidget* parent ):
        QWidget( parent ),
        Counter( "NavigationWidget" ),
        pages_( 0 )
    {
        Debug::Throw( "NavigationWidget::NavigationWidget.\n" );

        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->setMargin(0);
        setLayout( hLayout );

        // previous page button
        hLayout->addWidget( previousPageButton_ = new QToolButton( this ) );
        previousPageButton_->setAutoRaise( true );
        previousPageButton_->setIcon( IconEngine::get( ICONS::PREVIOUS ) );
        connect( previousPageButton_, SIGNAL( clicked( void ) ), SLOT( _previousPage( void ) ) );

        // editor
        hLayout->addWidget( editor_ = new QLineEdit( this ) );
        editor_->installEventFilter( this );
        editor_->setText( "1" );

        // validator
        validator_.setBottom( 1 );
        validator_.setTop( 1 );
        editor_->setValidator( &validator_ );
        editor_->setAlignment( Qt::AlignCenter );

        connect( editor_, SIGNAL( textEdited( const QString& ) ), SLOT( _updatePage( void ) ) );

        // label
        hLayout->addWidget( label_ = new QLabel( this ) );

        // next page button
        hLayout->addWidget( nextPageButton_ = new QToolButton( this ) );
        nextPageButton_->setAutoRaise( true );
        nextPageButton_->setIcon( IconEngine::get( ICONS::NEXT ) );
        connect( nextPageButton_, SIGNAL( clicked( void ) ), SLOT( _nextPage( void ) ) );

        // pages
        setPages(1);

    }

    //_________________________________________________________________
    void NavigationWidget::setPage( int page )
    {

        Debug::Throw() << "NavigationWidget::setPage - " << page << endl;
        if( page == editor_->text().toInt() ) return;

        // update editor and buttons
        editor_->setText( QString().setNum( page ) );
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
        label_->setText( buffer );

        // update validator
        validator_.setTop( pages );

        // update editor
        if( editor_->text().toInt() > pages ) editor_->setText( QString().setNum(pages) );

        // update editor
        const QFontMetrics metrics( editor_->fontMetrics() );
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
                    editor_->setText( QString().setNum( newPage ) );
                    _updateButtons();
                    emit pageChanged( newPage );
                }

                return true;

            }

            default: return QWidget::eventFilter( object, event );
        }

    }

    //_________________________________________________________________
    void NavigationWidget::_updatePage( void )
    {

        Debug::Throw( "NavigationWidget::_updatePage.\n" );
        int page( editor_->text().toInt() );
        if( page < 1 || page > pages_ ) return;
        _updateButtons();
        emit pageChanged( page );

    }


    //_________________________________________________________________
    void NavigationWidget::_previousPage( void )
    {

        Debug::Throw( "NavigationWidget::_previousPage.\n" );
        int page( editor_->text().toInt() );
        if( page < 2 ) return;
        editor_->setText( QString().setNum( page-1 ) );
        _updateButtons();
        emit pageChanged( page-1 );
    }

    //_________________________________________________________________
    void NavigationWidget::_nextPage( void )
    {

        Debug::Throw( "NavigationWidget::_nextPage.\n" );
        int page( editor_->text().toInt() );

        if( page >= pages_ ) return;
        editor_->setText( QString().setNum( page+1 ) );

        _updateButtons();
        emit pageChanged( page+1 );

    }

    //_________________________________________________________________
    void NavigationWidget::_updateButtons( void )
    {
        previousPageButton_->setEnabled( editor_->text().toInt() > 1 );
        nextPageButton_->setEnabled( editor_->text().toInt() < pages_ );
    }

}


//_________________________________________________________________
PrintPreviewDialog::PrintPreviewDialog( QWidget* parent ):
    BaseDialog( parent )
{
    Debug::Throw( "PrintPreviewDialog::PrintPreviewDialog.\n" );
    setOptionName( "PRINT_PREVIEW_DIALOG" );

    QVBoxLayout* layout( new QVBoxLayout() );
    layout->setMargin(0);
    setLayout( layout );

    layout->addWidget( previewWidget_ = new QPrintPreviewWidget( this ) );
    previewWidget_->setZoomMode( QPrintPreviewWidget::FitToWidth );

    layout->addWidget( navigationWidget_ = new PRINT::NavigationWidget( this ), 0, Qt::AlignCenter );
    connect( navigationWidget_, SIGNAL( pageChanged( int ) ), previewWidget_, SLOT( setCurrentPage( int ) ) );

    // close accelerator
    new QShortcut( Qt::CTRL+Qt::Key_W, this, SLOT( close() ) );

    // connect scrollbars
    QList<QGraphicsView*> graphicsViews( previewWidget_->findChildren<QGraphicsView*>() );
    if( !graphicsViews.isEmpty() )
    {

        // connection on vertical scrollbar
        connect( graphicsViews.front()->verticalScrollBar(), SIGNAL( valueChanged( int ) ), SLOT( _updatePage( void ) ) );

        // hide horizontal scrollbar
        graphicsViews.front()->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    }

    resize( 600, 700 );
}

//_________________________________________________________________
void PrintPreviewDialog::setHelper( BasePrintHelper& helper )
{
    Debug::Throw( "PrintPreviewDialog::setHelper.\n" );
    connect( previewWidget_, SIGNAL( paintRequested( QPrinter* ) ), &helper, SLOT( print( QPrinter* ) ) );
    connect( &helper, SIGNAL( pageCountChanged( int ) ), navigationWidget_, SLOT( setPages( int ) ) );
}
//_________________________________________________________________
void PrintPreviewDialog::_updatePage( void )
{ navigationWidget_->setPage( previewWidget_->currentPage() ); }
