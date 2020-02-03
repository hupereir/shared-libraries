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

#include "LineEditor.h"
#include "LineEditor_p.h"

#include "BaseContextMenu.h"
#include "BaseIconNames.h"
#include "Debug.h"
#include "IconEngine.h"
#include "StandardAction.h"

#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QStyleOptionFrame>

#include <QEvent>

namespace Private
{

    //____________________________________________________________
    LineEditorStyle::LineEditorStyle( QStyle* parent ):
        QProxyStyle( parent ),
        Counter( "Private::LineEditorStyle" )
    {}

    //____________________________________________________________
    QRect LineEditorStyle::subElementRect( SubElement subElement, const QStyleOption* option, const QWidget* widget ) const
    {

        // check subelement
        if( subElement != SE_LineEditContents ) return QProxyStyle::subElementRect( subElement, option, widget );

        // check editor
        const auto editor( qobject_cast<const LineEditor*>( widget ) );
        if( !editor ) return QProxyStyle::subElementRect( subElement, option, widget );

        // adjust sub element rect
        QRect rect( QProxyStyle::subElementRect( subElement, option, widget ) );
        return visualRect( option->direction, rect, rect.adjusted( editor->leftButtonsSize().width(), 0, -editor->rightButtonsSize().width(), 0 ) );

    }

    //____________________________________________________________
    QSize LineEditorStyle::sizeFromContents( ContentsType contentsType, const QStyleOption* option, const QSize& contentsSize, const QWidget* widget ) const
    {

        if( contentsType != CT_LineEdit ) return QProxyStyle::sizeFromContents( contentsType, option, contentsSize, widget );

        // check editor
        const auto editor( qobject_cast<const LineEditor*>( widget ) );
        if( !editor ) return QProxyStyle::sizeFromContents( contentsType, option, contentsSize, widget );

        const auto leftButtonsSize( editor->leftButtonsSize() );
        const auto rightButtonsSize( editor->rightButtonsSize() );

        QSize size( contentsSize );
        size.rwidth() += leftButtonsSize.width() + rightButtonsSize.width();
        size.setHeight( qMax( size.height(), qMax( leftButtonsSize.height(), rightButtonsSize.height() ) ) );

        return QProxyStyle::sizeFromContents( contentsType, option, size, widget );

    }

}

//____________________________________________________________
LineEditorButton::LineEditorButton( QWidget* parent ):
    QToolButton( parent ),
    Counter( "LineEditorButton" )
{

    QStyleOptionButton option;
    option.initFrom( this );
    const int iconSize( style()->pixelMetric( QStyle::PM_SmallIconSize, &option, this ) );
    setIconSize( QSize( iconSize, iconSize ) );
}

//____________________________________________________________
QSize LineEditorButton::sizeHint() const
{
    QStyleOptionButton option;
    option.initFrom( this );

    const int iconSize( style()->pixelMetric( QStyle::PM_SmallIconSize, &option, this ) );
    const int margin( 2 );
    const int dimension = iconSize + 2*margin;
    return QSize( dimension, dimension );
}

//____________________________________________________________
void LineEditorButton::paintEvent( QPaintEvent* event )
{
    if( icon().isNull() ) return;

    QStyleOptionButton option;
    option.initFrom( this );

    const int iconWidth( style()->pixelMetric( QStyle::PM_SmallIconSize, &option, this ) );
    const QSize iconSize( iconWidth, iconWidth );
    const auto pixmap( icon().pixmap( iconSize ) );

    const auto rect( this->rect() );
    const QRect iconRect( QPoint( rect.x() + (rect.width() - iconWidth)/2, rect.y() + (rect.height() - iconWidth)/2 ), iconSize );

    QPainter painter( this );
    painter.setClipRegion( event->region() );
    painter.drawPixmap( iconRect, pixmap );
}

//____________________________________________________________
LineEditor::LineEditor( QWidget* parent ):
    QLineEdit( parent ),
    Counter( "LineEditor" ),
    proxyStyle_( new Private::LineEditorStyle() )
{

    Debug::Throw( "LineEditor::LineEditor.\n" );

    // actions
    _installActions();

    // modification state call-back
    connect( this, &QLineEdit::textChanged, this, &LineEditor::_modified );

    // rigtht buttons container
    auto createContainer = []( QWidget* parent )
    {
        auto container = new QWidget(parent);
        container->setCursor( Qt::ArrowCursor );
        container->show();

        // button layout
        auto layout = new QHBoxLayout;
        layout->setMargin(0);
        layout->setSpacing(2);

        container->setLayout( layout );
        return container;
    };

    leftContainer_ = createContainer( this );
    rightContainer_ = createContainer( this );

    // clear button
    clearButton_ = new LineEditorButton;
    clearButton_->setIcon( IconEngine::get( IconNames::EditClear ) );
    clearButton_->setToolTip( tr( "Clear text" ) );
    addRightWidget( clearButton_ );

    // setup connections
    connect( clearButton_, &LineEditorButton::clicked, [this](bool){ clear(); } );

    setStyle( proxyStyle_.data() );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    _updateButtonsVisibility();
    _updateButtonsGeometry();

}

//_____________________________________________________________________
QSize LineEditor::leftButtonsSize() const
{ return leftContainer_->sizeHint(); }

//_____________________________________________________________________
QSize LineEditor::rightButtonsSize() const
{ return rightContainer_->sizeHint(); }

//_____________________________________________________________________
void LineEditor::setReadOnly( bool value )
{
    QLineEdit::setReadOnly( value );
    _updateButtonsVisibility();
    _updateButtonsGeometry();
    return;
}

//_____________________________________________________________________
void LineEditor::setModified( bool value )
{
    Debug::Throw( "LineEditor::setModified.\n" );
    if( value != modified_ )
    {
        modified_ = value;
        if( !value ) backup_ = text();
        emit modificationChanged( value );
    }
}

//______________________________________________________________
void LineEditor::setShowClearButton( bool value )
{

    showClearButton_ = value;
    _updateButtonsVisibility();
    _updateButtonsGeometry();

}

//______________________________________________________________________________
void LineEditor::installContextMenuActions( BaseContextMenu* menu )
{
    Debug::Throw( "TextEditor::installContextMenuActions.\n" );
    menu->addAction( undoAction_ );
    menu->addAction( redoAction_ );
    menu->addSeparator();

    menu->addAction( cutAction_ );
    menu->addAction( copyAction_ );
    menu->addAction( pasteAction_ );
    menu->addAction( clearAction_ );
    menu->addSeparator();

    menu->addAction( selectAllAction_ );
    menu->addAction( upperCaseAction_ );
    menu->addAction( lowerCaseAction_ );
}

//_____________________________________________________________________
void LineEditor::lowerCase()
{

    // do nothing if object is read only
    if( isReadOnly() ) return;

    // do nothing if selection is not valid
    if( !hasSelectedText() ) return;

    // do nothing if selection is not valid
    QString selection = selectedText().toLower();
    cut();
    insert( selection );
}

//_____________________________________________________________________
void LineEditor::upperCase()
{

    // do nothing if object is read only
    if( isReadOnly() ) return;

    // do nothing if selection is not valid
    if( !hasSelectedText() ) return;

    // get uppercased selection
    QString selection = selectedText().toUpper();
    cut();
    insert( selection );
}

//_______________________________________________________
bool LineEditor::event( QEvent* event )
{

    switch( event->type() )
    {

        case QEvent::StyleChange:
        {

            QStyle* currentStyle( style() );

            // do nothing is current style already match or is a style sheet
            if( qobject_cast<Private::LineEditorStyle*>( currentStyle ) ||
                currentStyle->inherits( "QStyleSheetStyle" ) )
                { break; }

            // create proxy style
            if( !proxyStyle_ ) proxyStyle_ = new Private::LineEditorStyle( currentStyle );
            else proxyStyle_.data()->setBaseStyle( currentStyle );

            // assign
            setStyle( proxyStyle_.data() );
            break;

        }

        case QEvent::Show:
        case QEvent::Resize:
        {
            _updateButtonsGeometry();
            break;
        }

        default: break;
    }

    return QLineEdit::event( event );

}

//_______________________________________________________________
void LineEditor::contextMenuEvent(QContextMenuEvent *event)
{
    Debug::Throw( "TextEditor::contextMenuEvent.\n" );

    // menu
    BaseContextMenu menu( this );
    menu.setHideDisabledActions( true );
    installContextMenuActions( &menu );
    if( !menu.isEmpty() )
    { menu.exec( event->globalPos() ); }

}

//_____________________________________________
void LineEditor::keyPressEvent( QKeyEvent* event )
{

    // process base class function
    QLineEdit::keyPressEvent( event );

    // emit signal
    emit cursorPositionChanged( cursorPosition() );

}

//______________________________________________________________________________
void LineEditor::_addWidget( QWidget* widget, QWidget* parent )
{
    widget->setParent( parent );
    parent->layout()->addWidget( widget );

    widget->setAutoFillBackground( false );
    widget->setFocusPolicy( Qt::NoFocus );

    if( auto button = qobject_cast<LineEditorButton*>( widget ) )
    {
        const bool empty( text().isEmpty() );
        const bool readOnly( isReadOnly() );
        button->setVisible(
            ((button->flags()&LineEditorButton::ShowWhenEmpty) || !empty ) &&
            ((button->flags()&LineEditorButton::ShowWhenReadOnly) || !readOnly ) );
    }

}

//____________________________________________________________
void LineEditor::_updateButtonsVisibility()
{

    const bool empty( text().isEmpty() );
    const bool readOnly( isReadOnly() );

    for( auto& button:findChildren<LineEditorButton*>() )
    {
        bool isVisible =
            ((button->flags()&LineEditorButton::ShowWhenEmpty) || !empty ) &&
            ((button->flags()&LineEditorButton::ShowWhenReadOnly) || !readOnly );

        if( button == clearButton_ ) isVisible &= showClearButton_;
        button->setVisible( isVisible );
    }

}

//____________________________________________________________
void LineEditor::_updateButtonsGeometry()
{
    QStyleOptionFrame option;
    option.initFrom( this );
    initStyleOption( &option );
    const QRect textRect( style()->subElementRect( QStyle::SE_LineEditContents, &option, this ) );

    {
        // left container
        const auto buttonsSize = leftContainer_->sizeHint();
        const auto buttonsRect =  style()->visualRect(
            option.direction,
            option.rect,
            QRect( textRect.topLeft() - QPoint( buttonsSize.width(), 0 ), QSize( buttonsSize.width(), textRect.height() ) ) );

        // assign
        leftContainer_->setGeometry( buttonsRect );
    }

    {
        // right container
        const auto buttonsSize = rightContainer_->sizeHint();
        const auto buttonsRect = style()->visualRect(
            option.direction,
            option.rect,
            QRect( textRect.topRight()+QPoint( 1, 0 ), QSize( buttonsSize.width(), textRect.height() ) ) );

        // assign
        rightContainer_->setGeometry( buttonsRect );
    }

}

//____________________________________________________________
void LineEditor::_modified( const QString& text )
{
    // modification signal
    bool modified( text != backup_ );
    if( modified != modified_ )
    {
        modified_ = modified;
        emit modificationChanged( modified_ );
    }

    // clear actions enable state
    clearAction_->setEnabled( isReadOnly() || text.isEmpty() );

    // update buttons
    _updateButtonsVisibility();
    _updateButtonsGeometry();

    // emit cleared signal
    if( text.isEmpty() ) emit cleared();

}

//__________________________________________________________
void LineEditor::_installActions()
{
    Debug::Throw( "LineEditor::_installActions.\n" );

    // create actions
    addAction( undoAction_ = new StandardAction( StandardAction::Type::Undo, this ) );
    undoAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( undoAction_, &QAction::triggered, this, &QLineEdit::undo );

    addAction( redoAction_ = new StandardAction( StandardAction::Type::Redo, this ) );
    redoAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( redoAction_, &QAction::triggered, this, &QLineEdit::redo );

    addAction( cutAction_ = new StandardAction( StandardAction::Type::Cut, this ) );
    cutAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( cutAction_, &QAction::triggered, this, &QLineEdit::cut );

    addAction( copyAction_ = new StandardAction( StandardAction::Type::Copy, this ) );
    copyAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( copyAction_, &QAction::triggered, this, &QLineEdit::copy );

    addAction( pasteAction_ = new StandardAction( StandardAction::Type::Paste, this ) );
    pasteAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( pasteAction_, &QAction::triggered, this, &QLineEdit::paste );
    connect( qApp->clipboard(), &QClipboard::dataChanged, this, &LineEditor::_updatePasteAction );
    pasteAction_->setEnabled( !isReadOnly() );

    addAction( clearAction_ = new QAction( tr( "Clear" ), this ) );
    connect( clearAction_, &QAction::triggered, this, &LineEditor::clear );

    addAction( selectAllAction_ = new QAction( tr( "Select All" ), this ) );
    selectAllAction_->setShortcut( QKeySequence::SelectAll );
    selectAllAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( selectAllAction_, &QAction::triggered, this, &QLineEdit::selectAll );

    addAction( upperCaseAction_ = new QAction( tr( "Upper Case" ), this ) );
    upperCaseAction_->setShortcut( Qt::CTRL+Qt::Key_U );
    connect( upperCaseAction_, &QAction::triggered, this, &LineEditor::upperCase );

    addAction( lowerCaseAction_ = new QAction( tr( "Lower Case" ), this ) );
    lowerCaseAction_->setShortcut( Qt::SHIFT+Qt::CTRL+Qt::Key_U );
    connect( lowerCaseAction_, &QAction::triggered, this, &LineEditor::lowerCase );

    // update actions that depend on the presence of a selection
    connect( this, &QLineEdit::textChanged, this, &LineEditor::_updateUndoRedoActions );
    connect( this, &QLineEdit::selectionChanged, this, &LineEditor::_updateSelectionActions );
    _updateUndoRedoActions();
    _updateSelectionActions();

}

//________________________________________________
void LineEditor::_updateUndoRedoActions()
{
    Debug::Throw( "LineEditor::_updateUndoRedoActions.\n" );
    undoAction_->setEnabled( isUndoAvailable() );
    redoAction_->setEnabled( isRedoAvailable() );
}

//________________________________________________
void LineEditor::_updateSelectionActions()
{

    Debug::Throw( "LineEditor::_updateSelectionActions.\n" );

    bool hasSelection( hasSelectedText() );
    bool editable( !isReadOnly() );

    cutAction_->setEnabled( hasSelection && editable );
    copyAction_->setEnabled( hasSelection );
    upperCaseAction_->setEnabled( hasSelection && editable );
    lowerCaseAction_->setEnabled( hasSelection && editable );

}

//________________________________________________
void LineEditor::_updatePasteAction()
{

    Debug::Throw( "LineEditor::_updatePasteAction.\n" );
    bool editable( !isReadOnly() );
    bool hasClipboard( !qApp->clipboard()->text().isEmpty() );
    pasteAction_->setEnabled( editable && hasClipboard );

}
