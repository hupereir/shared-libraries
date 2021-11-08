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

#include "TreeView.h"
#include "BaseFindDialog.h"
#include "BaseFindWidget.h"
#include "BaseIconNames.h"
#include "Color.h"
#include "ColorDisplay.h"
#include "ColumnSelectionMenu.h"
#include "ColumnSortingMenu.h"
#include "IconEngine.h"
#include "InformationDialog.h"
#include "ItemModel.h"
#include "QtUtil.h"
#include "RegExpUtil.h"
#include "ScrollBarMonitor.h"
#include "Singleton.h"
#include "TextEditor.h"
#include "TreeViewItemDelegate.h"
#include "XmlOptions.h"



#include <QCursor>
#include <QHeaderView>
#include <QPainter>
#include <QScrollBar>
#include <QRegularExpression>
#include <QStyledItemDelegate>

//______________________________________________________________________
TreeView::TreeView( QWidget* parent ):
    QTreeView( parent ),
    Counter( QStringLiteral("TreeView") )
{
    Debug::Throw( QStringLiteral("TreeView::TreeView.\n") );

    // scrollbar monitor
    scrollBarMonitor_ = new ScrollBarMonitor( this );

    // replace item delegate
    if( itemDelegate() ) itemDelegate()->deleteLater();
    auto delegate = new TreeViewItemDelegate( this );
    setItemDelegate( delegate );

    // actions
    _installActions();

    // default configuration
    setAllColumnsShowFocus( true );
    setRootIsDecorated( false );
    setSortingEnabled( true );
    setAnimated( false );
    setExpandsOnDoubleClick( false );

    // header menu
    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    header()->setSortIndicator( 0, Qt::AscendingOrder );
    connect( header(), &QWidget::customContextMenuRequested, this, &TreeView::_raiseHeaderMenu );
    connect( header(), &QHeaderView::sortIndicatorChanged, this, &TreeView::saveSortOrder );

    // horizontal scrollbar range bug
    connect( horizontalScrollBar(), &QAbstractSlider::rangeChanged, this, &TreeView::_updateHorizontalScrollBarRange );

    // configuration
    connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &TreeView::_updateConfiguration );
    _updateConfiguration();

}

//______________________________________________________________________
TreeView::~TreeView() = default;

//______________________________________________________________________
void TreeView::setItemMargin( int value )
{
    itemMarginFromOptions_ = false;
    _setItemMargin( value );
}

//______________________________________________________________________
void TreeView::setModel( QAbstractItemModel* model )
{

    Debug::Throw( QStringLiteral("TreeView::setModel.\n") );
    QTreeView::setModel( model );

    // selected indexes
    model_ = qobject_cast<ItemModel*>( model );
    if( model_ )
    {
        connect( model_, &QAbstractItemModel::layoutAboutToBeChanged, this, &TreeView::saveSelectedIndexes );
        connect( model_, &QAbstractItemModel::layoutChanged, this, &TreeView::restoreSelectedIndexes );
    }

    // expanded indexes
    if( model_ && model_->supportsExpandedIndexes() )
    {
        connect( model_, &QAbstractItemModel::layoutAboutToBeChanged, this, &TreeView::saveExpandedIndexes );
        connect( model_, &QAbstractItemModel::layoutChanged, this, &TreeView::restoreExpandedIndexes );
    }

}

//______________________________________________________________________
void TreeView::setItemDelegate( QAbstractItemDelegate* delegate )
{

    // assign new
    QTreeView::setItemDelegate( delegate );

    // update margin to store value
    _setItemMargin( itemMargin_ );

}

//______________________________________________________________________
void TreeView::setItemDelegateForColumn( int column, QAbstractItemDelegate* delegate )
{

    // assign new
    QTreeView::setItemDelegateForColumn( column, delegate );

    // update margin to store value
    _setItemMargin( itemMargin_ );

}

//______________________________________________________________________
void TreeView::setFindWidget( AbstractFindWidget* widget )
{

    Debug::Throw( QStringLiteral("TreeView::setFindWidget.\n") );
    if( findWidget_ ) findWidget_->deleteLater();
    findWidget_ = widget;

    // connections
    if( findWidget_ )
    {
        connect( findWidget_, &AbstractFindWidget::find, this, &TreeView::find );
        connect( this, &TreeView::matchFound, findWidget_, &AbstractFindWidget::matchFound );
        connect( this, &TreeView::noMatchFound, findWidget_, &AbstractFindWidget::noMatchFound );
        connect( this, &QObject::destroyed, findWidget_, &QObject::deleteLater );
    }

}

//______________________________________________________________________
void TreeView::setFindEnabled( bool value )
{
    Debug::Throw( QStringLiteral("IconView::setFindEnabled.\n") );
    findAction_->setEnabled( value );
    findSelectionAction_->setEnabled( value );
    findAgainAction_->setEnabled( value );
    findSelectionBackwardAction_->setEnabled( value );
    findAgainBackwardAction_->setEnabled( value );
}

//______________________________________________________________________
TextSelection TreeView::selection() const
{

    Debug::Throw( QStringLiteral("TreeView::selection.\n") );

    // copy last selection
    TextSelection out;
    out.setFlag( TextSelection::CaseSensitive, TextEditor::lastSelection().flag( TextSelection::CaseSensitive ) );
    out.setFlag( TextSelection::EntireWord, TextEditor::lastSelection().flag( TextSelection::EntireWord ) );

    QString text;
    if( !( text = qApp->clipboard()->text( QClipboard::Selection ) ).isEmpty() ) out.setText( text );
    else if( selectionModel() && model() && selectionModel()->currentIndex().isValid() )
    {

        const auto current( selectionModel()->currentIndex() );
        if( !(text =  model()->data( current ).toString()).isEmpty() ) out.setText( text );
        else if( allColumnsShowFocus() )
        {

            const auto parent( model()->parent( current ) );

            // loop over all columns, retrieve
            for( int column = 0; column < model()->columnCount( parent ); column++ )
            {
                // get index from column
                const auto index( model()->index( current.row(), column, parent ) );
                if( index.isValid() && !(text =  model()->data( index ).toString()).isEmpty() )
                {
                    out.setText( text );
                    break;
                }

            }

        }
    }

    // if everything else failed, retrieve last selection
    if( text.isEmpty() ) out.setText( TextEditor::lastSelection().text() );

    return out;

}

//_______________________________________________
bool TreeView::isVisible( const QModelIndex& index ) const
{
    if( !( model() && index.isValid() ) ) return false;
    const auto parent( model()->parent( index ) );
    return (!parent.isValid() ) || ( isExpanded( parent ) && isVisible( parent ) );
}

//_______________________________________________
int TreeView::visibleColumnCount() const
{
    int out(0);
    for( int index=0; model() && index < model()->columnCount(); index++ )
    { if( !isColumnHidden( index ) ) out++; }

    return out;
}

//_______________________________________________
bool TreeView::setOptionName( const QString& value )
{

    Debug::Throw() << "TreeView::setOptionName - value: " << value << Qt::endl;

    QString tmp;

    // mask
    bool maskChanged( false );
    tmp = value + "_MASK";
    if( maskOptionName_ != tmp  )
    {
        maskOptionName_ = tmp;
        maskChanged = true;
        if( !XmlOptions::get().contains( maskOptionName_ ) ) saveMask();
        else updateMask();

    }

    // sort order
    bool sortChanged( false );
    tmp = value + QStringLiteral( "_SORT_ORDER" );
    if( sortOrderOptionName_ != tmp  )
    {
        sortOrderOptionName_ = tmp;
        sortChanged = true;
    }

    // sort column
    tmp = value + QStringLiteral( "_SORT_COLUMN" );
    if( sortColumnOptionName_ != tmp  )
    {

        sortColumnOptionName_ = tmp;
        sortChanged = true;

    }

    // show header
    bool showHeaderChanged( false );
    tmp = value + QStringLiteral( "_SHOW_HEADER" );
    if( showHeaderOptionName_ != tmp )
    {
        showHeaderOptionName_ = tmp;
        showHeaderChanged = true;
        if( !XmlOptions::get().contains( showHeaderOptionName_ ) )
        {

            XmlOptions::get().set<bool>( showHeaderOptionName_, showHeaderAction().isChecked() );

        } else showHeaderAction().setChecked( XmlOptions::get().get<bool>( showHeaderOptionName_ ) );

    }

    // reload sorting
    if( sortChanged )
    {
        if( !( XmlOptions::get().contains( sortOrderOptionName_ ) && XmlOptions::get().contains( sortColumnOptionName_ ) ) ) saveSortOrder();
        else updateSortOrder();
    }

    return maskChanged || sortChanged || showHeaderChanged;

}

//_______________________________________________
int TreeView::mask() const
{
    Debug::Throw( QStringLiteral("TreeView::mask.\n") );
    int mask = 0;
    for( int index=0; model() && index < model()->columnCount(); index++ )
    { if( !isColumnHidden( index ) ) mask |= (1<<index); }
    return mask;
}

//_______________________________________________
void TreeView::setMask( int mask )
{

    Debug::Throw( QStringLiteral("TreeView::setMask.\n") );
    for( int index=0; index < model()->columnCount(); index++ )
    {

        // check mask
        if( lockedColumns_ & (1<<index ) ) continue;

        // see if there is a change between new and old mask
        if( isColumnHidden( index ) == !(mask & (1<<index) ) ) continue;
        setColumnHidden( index, !(mask & (1<<index) ) );

    }

    return;

}

//______________________________________________________
void TreeView::saveScrollBarPosition()
{ scrollBarMonitor_->save(); }

//______________________________________________________
void TreeView::restoreScrollBarPosition()
{ scrollBarMonitor_->restore(); }

//______________________________________________________
void TreeView::resizeColumns( int mask )
{

    // if no items present, do nothing
    if( !( model() && model()->rowCount() ) ) return;

    for( int i = 0; i < model()->columnCount(); i++ )
    { if( mask & (1<<i) ) resizeColumnToContents(i); }

}

//___________________________________
void TreeView::toggleShowHeader( bool value )
{
    if( header() ) header()->setVisible( value );
    showHeaderAction().setChecked( value );
    if( hasOptionName() ) XmlOptions::get().set<bool>( showHeaderOptionName_, value );
}

//_______________________________________________
void TreeView::updateMask()
{

    Debug::Throw( QStringLiteral("TreeView::updateMask.\n") );

    // check model and option availability
    if( !model() ) return;
    if( !hasOptionName() ) return;
    if( !XmlOptions::get().contains( maskOptionName_ ) ) return;

    // assign mask from options
    setMask( XmlOptions::get().get<int>( maskOptionName_ ) );
    resizeColumns();

}

//_______________________________________________
void TreeView::saveMask()
{

    Debug::Throw( QStringLiteral("TreeView::saveMask.\n") );
    if( !hasOptionName() ) return;
    XmlOptions::get().set<int>( maskOptionName_, mask() );

}

//_____________________________________________________________________
void TreeView::updateSortOrder()
{

    Debug::Throw( QStringLiteral("TreeView::updateSortOrder.\n") );
    if( !hasOptionName() ) return;
    if( XmlOptions::get().contains( sortColumnOptionName_ ) && XmlOptions::get().contains( sortColumnOptionName_ ) )
    {
        const bool changed(
            header()->sortIndicatorSection() != XmlOptions::get().get<int>( sortColumnOptionName_ ) ||
            header()->sortIndicatorOrder() != XmlOptions::get().get<int>( sortOrderOptionName_ ) );

        if( changed )
        { sortByColumn( XmlOptions::get().get<int>( sortColumnOptionName_ ), (Qt::SortOrder)(XmlOptions::get().get<int>( sortOrderOptionName_ ) ) ); }

    }

}

//_____________________________________________________________________
void TreeView::saveSortOrder()
{

    Debug::Throw( QStringLiteral("TreeView::saveSortOrder.\n") );

    // save option
    if( !hasOptionName() ) return;
    XmlOptions::get().set<int>( sortOrderOptionName_, header()->sortIndicatorOrder() );
    XmlOptions::get().set<int>( sortColumnOptionName_, header()->sortIndicatorSection() );

}

//______________________________________________________________________
void TreeView::find( const TextSelection &selection )
{
    Debug::Throw( QStringLiteral("TreeView::find.\n") );
    bool found( selection.flag( TextSelection::Backward ) ? _findBackward( selection, true ):_findForward( selection, true ) );
    if( found ) emit matchFound();
    else emit noMatchFound();
}

//______________________________________________________________________
void TreeView::findAgainForward()
{ _findForward( TextEditor::lastSelection(), true ); }

//______________________________________________________________________
void TreeView::findAgainBackward()
{ _findBackward( TextEditor::lastSelection(), true ); }

//__________________________________________________________
void TreeView::_setItemMargin( int value )
{

    Debug::Throw() << "TreeView::_setItemMargin - value: " << value << Qt::endl;
    itemMargin_ = value;

    if( TreeViewItemDelegate* delegate = qobject_cast<TreeViewItemDelegate*>( itemDelegate() ) )
    { delegate->setItemMargin( itemMargin_ ); }

    if( model() )
    {
        for( int iColumn = 0; iColumn < model()->columnCount(); ++iColumn )
        {
            if( TreeViewItemDelegate* delegate = qobject_cast<TreeViewItemDelegate*>( itemDelegateForColumn(iColumn) ) )
            { delegate->setItemMargin( itemMargin_ ); }
        }
    }

}

//__________________________________________________________
void TreeView::saveSelectedIndexes()
{
    if( selectionModel() )
    {
        model_->setSelectedIndexes( selectionModel()->selectedRows() );
        model_->setCurrentIndex( selectionModel()->currentIndex() );
    }
}

//__________________________________________________________
void TreeView::restoreSelectedIndexes()
{

    if( selectionModel() )
    {

        selectionModel()->clear();


        auto selectedIndexes( model_->selectedIndexes() );

        // try build selection ranges from selection
        if( !selectedIndexes.empty() )
        {
            QItemSelection selection;
            auto first = selectedIndexes.front();
            auto previous = first;
            for( auto iter = selectedIndexes.begin()+1; iter != selectedIndexes.end(); ++iter )
            {
                const auto current = *iter;
                if( current.row() != previous.row()+1 )
                {
                    selection.append( QItemSelectionRange( first, previous ) );
                    first = current;
                }
                previous = current;
            }
            selection.append( QItemSelectionRange( first, previous ) );
            selectionModel()->select( selection, QItemSelectionModel::Select|QItemSelectionModel::Rows );
        }


        selectionModel()->setCurrentIndex( model_->currentIndex(), QItemSelectionModel::Current );

    }

    return;
}


//__________________________________________________________
void TreeView::saveExpandedIndexes()
{

    Debug::Throw( QStringLiteral("TreeView::saveExpandedIndexes.\n") );

    // clear
    model_->clearExpandedIndexes();

    // retrieve selected indexes in list
    bool hasExpandedIndexes( false );
    for( const auto& index:model_->indexes() )
    {
        if( isExpanded( index ) )
        {
            hasExpandedIndexes = true;
            model_->setIndexExpanded( index, true );
        }
    }

    collapseAllAction_->setEnabled(hasExpandedIndexes);

    saveScrollBarPosition();

}

//________________________________________
void TreeView::restoreExpandedIndexes()
{

    auto indexes( model_->expandedIndexes() );

    collapseAll();
    for( const auto& index:indexes )
    { setExpanded( index, true ); }

    restoreScrollBarPosition();

}

//____________________________________________________________________________
bool TreeView::event( QEvent* event )
{

    switch( event->type() )
    {

        case QEvent::Leave:
        case QEvent::HoverLeave:
        {
            _setHoverIndex( QModelIndex() );
            break;
        }

        default: break;
    }

    return QTreeView::event( event );

}

//____________________________________________________________________
void TreeView::mouseMoveEvent( QMouseEvent *event )
{

    if( hasMouseTracking() && event->buttons() == Qt::NoButton )
    {
        // when mouse tracking is on, need to intercept mouseMoveEvents
        // because they break drag and drop
        _setHoverIndex( indexAt( event->pos() ) );
    } else {
        if( !indexAt( event->pos() ).isValid() )
        { _setHoverIndex( QModelIndex() ); }
        QTreeView::mouseMoveEvent( event );
    }

}

//__________________________________________________________
void TreeView::mousePressEvent( QMouseEvent* event )
{
    // clear hover index
    _setHoverIndex( QModelIndex() );
    if( (event->button() == Qt::RightButton) && selectionModel() && !indexAt( event->pos() ).isValid() )
    { selectionModel()->clear(); }
    QTreeView::mousePressEvent( event );
}

//__________________________________________________________
void TreeView::paintEvent( QPaintEvent* event )
{
    if(
        useSelectedColumnColor_ && visibleColumnCount() >= 2 &&
        header() && header()->isVisible() &&
        header()->isSortIndicatorShown() )
    {
        // get selected column
        int selectedColumn( header()->sortIndicatorSection() );
        QRect rect( visualRect( model()->index( 0, selectedColumn ) ) );

        QPainter painter( viewport() );
        painter.setClipRect( event->rect() );
        rect.setTop(0);
        rect.setHeight( height() );
        painter.setBrush( palette().color( QPalette::AlternateBase ) );
        painter.setPen( Qt::NoPen );
        painter.drawRect( rect );
        painter.end();
    }

    QTreeView::paintEvent( event );

}

//______________________________________________________________________
void TreeView::_createFindDialog()
{

    Debug::Throw( QStringLiteral("TreeView::_createFindDialog.\n") );
    // create dialog
    findDialog_ = new BaseFindDialog( this );
    findDialog_->setWindowTitle( tr( "Find in List" ) );

    if( !findWidget_ ) _createFindWidget( false );
    findDialog_->setBaseFindWidget( findWidget_ );

    return;

}

//______________________________________________________________________
void TreeView::_createFindWidget( bool compact )
{

    Debug::Throw( QStringLiteral("TreeView::_createFindWidget.\n") );

    // create dialog
    auto findWidget =  new BaseFindWidget( this, compact );
    findWidget->enableEntireWord( false );
    findWidget->enableRegExp( true );
    setFindWidget( findWidget );

    return;

}

//______________________________________________________________________
bool TreeView::_findForward( const TextSelection& selection, bool rewind )
{
    Debug::Throw( QStringLiteral("TreeView::_findForward.\n") );
    if( selection.text().isEmpty() ) return false;

    // store selection
    TextEditor::setLastSelection( selection );

    // check model and selection model
    if( !( model() && selectionModel() ) ) return false;

    QRegularExpression regexp;
    if( selection.flag( TextSelection::RegExp ) )
    {

        // construct regexp and check
        regexp.setPattern( selection.text() );
        if( !regexp.isValid() )
        {
            InformationDialog( this, tr( "Invalid regular expression. Find canceled" ) ).exec();
            return false;
        }

        // case sensitivity
        Base::setCaseSensitivity( regexp, selection.flag( TextSelection::CaseSensitive ) );

    }

    // set first index
    const auto current( selectionModel()->currentIndex() );
    auto index( ( selection.flag( TextSelection::NoIncrement ) ) ? current:_indexAfter( current ) );

    // if index index is invalid and rewind, set index index of the model
    if( (!index.isValid()) && rewind )
    {
        rewind = false;
        index = _firstIndex();
    }

    // no starting index found. Return
    if( !index.isValid() ) return false;

    // loop over indexes
    while( index.isValid() )
    {

        QString text;
        bool accepted( false );

        // check if column is visible
        if( !( (text = model()->data( index ).toString() ).isEmpty() || isColumnHidden( index.column() ) ) )
        {

            // check if text match
            if( regexp.isValid() && !regexp.pattern().isEmpty() ) { if( regexp.match( text ).hasMatch() ) accepted = true; }
            else if( text.indexOf( selection.text(), 0, selection.flag( TextSelection::CaseSensitive ) ? Qt::CaseSensitive : Qt::CaseInsensitive ) >= 0 )
            { accepted = true; }

        }

        if( accepted )
        {

            QItemSelectionModel::SelectionFlags command( QItemSelectionModel::Clear|QItemSelectionModel::Select );
            if( allColumnsShowFocus() ) command |= QItemSelectionModel::Rows;
            selectionModel()->select( index, command );

            // update current index
            command = QItemSelectionModel::Current;
            if( allColumnsShowFocus() ) command |= QItemSelectionModel::Rows;
            selectionModel()->setCurrentIndex( index,  command );

            // quit loop
            return true;

        } else {

            index = _indexAfter( index );
            if( rewind && !index.isValid() )
            {
                rewind = false;
                index = _firstIndex();
            }

        }

    }

    // no match found
    return false;

}

//______________________________________________________________________
bool TreeView::_findBackward( const TextSelection& selection, bool rewind )
{

    Debug::Throw( QStringLiteral("TreeView::_findBackward.\n") );
    if( selection.text().isEmpty() ) return false;

    // store selection
    TextEditor::setLastSelection( selection );

    // check model and selection model
    if( !( model() && selectionModel() ) ) return false;

    QRegularExpression regexp;
    if( selection.flag( TextSelection::RegExp ) )
    {

        // construct regexp and check
        regexp.setPattern( selection.text() );
        if( !regexp.isValid() )
        {
            InformationDialog( this, tr( "Invalid regular expression. Find canceled" ) ).exec();
            return false;
        }

        // case sensitivity
        Base::setCaseSensitivity( regexp, selection.flag( TextSelection::CaseSensitive ) );

    }

    // set first index
    const auto current( selectionModel()->currentIndex() );
    auto index( ( selection.flag( TextSelection::NoIncrement ) ) ? current:_indexBefore( current ) );

    // if index index is invalid and rewind, set index index of the model
    if( (!index.isValid()) && rewind )
    {
        rewind = false;
        index = _lastIndex();
    }

    // no starting index found. Return
    if( !index.isValid() ) return false;

    // loop over indexes
    while( index.isValid() )
    {

        QString text;
        bool accepted( false );

        // check if column is visible
        if( !( (text = model()->data( index ).toString() ).isEmpty() || isColumnHidden( index.column() ) ) )
        {

            // check if text match
            if( regexp.isValid() && !regexp.pattern().isEmpty() ) { if( regexp.match( text ).hasMatch() ) accepted = true; }
            else if( text.indexOf( selection.text(), 0, selection.flag( TextSelection::CaseSensitive ) ? Qt::CaseSensitive : Qt::CaseInsensitive ) >= 0 )
            { accepted = true; }

        }

        if( accepted )
        {

            QItemSelectionModel::SelectionFlags command( QItemSelectionModel::Clear|QItemSelectionModel::Select );
            if( allColumnsShowFocus() ) command |= QItemSelectionModel::Rows;
            selectionModel()->select( index, command );

            // update current index
            command = QItemSelectionModel::Current;
            if( allColumnsShowFocus() ) command |= QItemSelectionModel::Rows;
            selectionModel()->setCurrentIndex( index,  command );

            // quit loop
            return true;

        } else {

            index = _indexBefore( index );
            if( rewind && !index.isValid() )
            {
                rewind = false;
                index = _lastIndex();
            }

        }

    }

    return false;

}

//__________________________________________________________
void TreeView::_updateHorizontalScrollBarRange()
{
    /* this hack fixes a bug that the header lenght is not properly calculated
    Qt version 5.10, resulting in horizontal scrollbars being shown in all tree views */

    Debug::Throw( QStringLiteral("TreeView::_installActions.\n") );
    if( scrollBarRecursionLock_ ) return;
    if( !( model() && header() ) ) return;

    scrollBarRecursionLock_ = true;

    // get viewport size
    auto viewportWidth( viewport()->size().width() );

    // get section width
    int headerWidth = 0;
    for( int i = 0; i < model()->columnCount(); i++ )
    { if( !isColumnHidden(i) ) headerWidth += columnWidth(i); }

    // update scrollbar range
    horizontalScrollBar()->setRange( 0, qMax(headerWidth - viewportWidth, 0) );

    scrollBarRecursionLock_ = false;

}

//__________________________________________________________
void TreeView::_installActions()
{
    Debug::Throw( QStringLiteral("TreeView::_installActions.\n") );

    addAction( selectAllAction_ = new QAction( tr("Select All"), this ) );
    selectAllAction_->setShortcut( QKeySequence::SelectAll );
    selectAllAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( selectAllAction_, &QAction::triggered, this, &TreeView::selectAll );

    addAction( collapseAllAction_ = new QAction( tr("Collapse All Entries"), this ) );
    connect( collapseAllAction_, &QAction::triggered, this, &QTreeView::collapseAll );

    addAction( findAction_ = new QAction( IconEngine::get( IconNames::Find ), tr("Find"), this ) );
    findAction_->setShortcut( QKeySequence::Find );
    findAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findAction_, &QAction::triggered, this, &TreeView::_findFromDialog );

    addAction( findAgainAction_ = new QAction( tr("Find Again"), this ) );
    findAgainAction_->setShortcut( Qt::CTRL + Qt::Key_G );
    findAgainAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findAgainAction_, &QAction::triggered, this, &TreeView::findAgainForward );

    addAction( findAgainBackwardAction_ = new QAction( this ) );
    findAgainBackwardAction_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_G );
    findAgainBackwardAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findAgainBackwardAction_, &QAction::triggered, this, &TreeView::findAgainBackward );

    addAction( findSelectionAction_ = new QAction( tr("Find Selection"), this ) );
    findSelectionAction_->setShortcut( Qt::CTRL + Qt::Key_H );
    findSelectionAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findSelectionAction_, &QAction::triggered, this, &TreeView::findSelectionForward );

    addAction( findSelectionBackwardAction_ = new QAction( this ) );
    findSelectionBackwardAction_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_H );
    findSelectionBackwardAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findSelectionBackwardAction_, &QAction::triggered, this, &TreeView::findSelectionBackward );


    // show header action
    showHeaderAction_ = new QAction( tr("Show List Header"), this );
    showHeaderAction_->setCheckable( true );
    showHeaderAction_->setChecked( true );
    connect( showHeaderAction_, &QAction::toggled, this, &TreeView::toggleShowHeader );

}

//___________________________________
void TreeView::_raiseHeaderMenu( QPoint pos )
{
    Debug::Throw( QStringLiteral("TreeView::_raiseHeaderMenu.\n") );

    // check number of columns
    if( header()->count() <= 1 ) return;

    // create menu and raise.
    ColumnSelectionMenu menu( this, this );
    menu.setCurrentColumn( header()->logicalIndexAt( pos ) );
    if( isSortingEnabled() )
    {
        menu.addSeparator();
        menu.addMenu( new ColumnSortingMenu( &menu, this ) );
    }

    menu.adjustSize();
    menu.exec( header()->mapToGlobal( pos ) );

    // save mask after menu execution,
    // to keep visible columns in sync with option
    saveMask();

}

//_____________________________________________________________________
void TreeView::_findFromDialog()
{
    Debug::Throw( QStringLiteral("TreeView::_findFromDialog.\n") );

    // set default text
    // update find text
    QString text( selection().text() );
    if( !text.isEmpty() )
    {
        const int maxLength( 1024 );
        text.truncate( maxLength );
    }

    // create
    if( useEmbeddedWidgets_ )
    {

        if( !findWidget_ ) _createFindWidget( true );
        findWidget_->show();

    } else {

        if( !findDialog_ ) _createFindDialog();
        findDialog_->centerOnParent();
        findDialog_->show();
        findDialog_->activateWindow();

    }

    findWidget_->editor().setFocus();
    findWidget_->synchronize();
    findWidget_->matchFound();
    findWidget_->setText( text );

    return;
}

//____________________________________________________________________
void TreeView::_setHoverIndex( const QModelIndex& index )
{
    if( hoverIndex_ == index ) return;
    hoverIndex_ = index;

    // emit signal
    emit hovered( index );

}

//_____________________________________________________________________
void TreeView::_updateConfiguration()
{
    Debug::Throw( QStringLiteral("TreeView::_updateConfiguration.\n") );

    // load mask from option, if any
    updateMask();
    updateSortOrder();

    // alternate color
    const QPalette palette( this->palette() );
    setAlternatingRowColors(
        ( XmlOptions::get().get<bool>( QStringLiteral("USE_ALTERNATE_COLOR") ) || forceAlternatingRowColors_ ) &&
        palette.color( QPalette::AlternateBase ) != palette.color( QPalette::Base ) );

    // try load selected column color from option
    useSelectedColumnColor_ = XmlOptions::get().get<bool>( QStringLiteral("USE_SELECTED_COLUMN_COLOR") );

    // item margin
    if( itemMarginFromOptions_ && XmlOptions::get().contains( QStringLiteral("LIST_ITEM_MARGIN") ) )
    { _setItemMargin( XmlOptions::get().get<int>( QStringLiteral("LIST_ITEM_MARGIN") ) ); }

    // icon size
    if( iconSizeFromOptions_ && XmlOptions::get().contains( QStringLiteral("LIST_ICON_SIZE") )  )
    {
        int iconSize( XmlOptions::get().get<int>( QStringLiteral("LIST_ICON_SIZE") ) );
        QTreeView::setIconSize( QSize( iconSize, iconSize )  );
    }

}

//_________________________________________________________
QModelIndex TreeView::_firstIndex() const
{ return model()->index( 0, 0 ); }

//_________________________________________________________
QModelIndex TreeView::_lastIndex() const
{

    if( !model()->rowCount() ) return QModelIndex();

    auto out( model()->index( model()->rowCount()-1, 0 ) );
    while( out.isValid() && isExpanded( out ) && model()->hasChildren( out ) )
    { out = model()->index( model()->rowCount( out )-1, 0, out ); }

    // select last column
    auto parent = model()->parent( out );
    out = model()->index( out.row(), model()->columnCount( parent )-1, parent );

    return out;

}

//_________________________________________________________
QModelIndex TreeView::_indexAfter( const QModelIndex& current ) const
{

    QModelIndex out;
    if( !current.isValid() ) return out;

    // get parent
    auto parent( model()->parent( current ) );

    // assign next column if valid
    if( current.column()+1 < model()->columnCount( parent ) ) out = model()->index( current.row(), current.column()+1, parent );
    else {

        auto firstColumnIndex( model()->index( current.row(), 0, parent ) );

        // assign first children if current is expanded and has children
        if( firstColumnIndex.isValid() && isExpanded( firstColumnIndex ) && model()->hasChildren( firstColumnIndex ) )
        { out = model()->index( 0, 0, firstColumnIndex ); }

        // assign next row if valid
        if( !out.isValid() ) out = model()->index( current.row()+1, 0, parent );

        // assign next row from parent if valid
        if( (!out.isValid()) && parent.isValid() ) out = model()->index( parent.row()+1, 0, model()->parent( parent ) );

    }

    return out;

}

//_________________________________________________________
QModelIndex TreeView::_indexBefore( const QModelIndex& current ) const
{
    Debug::Throw() << "TreeView::_indexBefore - " << current.row() << "," << current.column() << "," << current.isValid() << Qt::endl;
    QModelIndex out;
    if( !current.isValid() ) return out;

    // get parent
    auto parent( model()->parent( current ) );

    // assign next column if valid
    if( current.column() > 0 ) out = model()->index( current.row(), current.column()-1, parent );
    else {

        if( current.row() > 0 )
        {

            out = model()->index( current.row()-1, 0, parent );
            while( out.isValid() && isExpanded( out ) && model()->hasChildren( out ) )
            { out = model()->index( model()->rowCount( out )-1, 0, out ); }

            // select last column
            parent = model()->parent( out );
            out = model()->index( out.row(), model()->columnCount( parent )-1, parent );

        } else {

            // select last column of parent
            if( parent.isValid() )
            { out = model()->index( parent.row(), model()->columnCount( model()->parent( parent ) )-1, model()->parent( parent ) ); }

        }

    }

    return out;

}

//_________________________________________________________
TreeView::Container::Container( QWidget* parent ):
    Container( parent, new TreeView )
{}

//_________________________________________________________
TreeView::Container::Container( QWidget* parent, TreeView* treeView ):
    QWidget( parent ),
    Counter( QStringLiteral("TreeView::Container") ),
    treeView_( treeView )
{ _initialize(); }

//_________________________________________________________
void TreeView::Container::_initialize()
{
    Debug::Throw( QStringLiteral("TreeView::Container::_initialize.\n") );
    treeView_->setParent( this );

    // setup layout
    auto vLayout = new QVBoxLayout;
    QtUtil::setMargin(vLayout, 0);
    vLayout->setSpacing(2);
    setLayout( vLayout );

    // treeview
    vLayout->addWidget( treeView_ );
    treeView_->useEmbeddedWidgets_ = true;

    // find widget
    treeView_->_createFindWidget( true );
    treeView_->findWidget_->setParent( this );
    vLayout->addWidget( treeView_->findWidget_ );
    treeView_->findWidget_->hide();

    connect( &treeView_->findWidget_->closeButton(), &QAbstractButton::clicked, this, [this](bool){ treeView_->setFocus(); } );
}
