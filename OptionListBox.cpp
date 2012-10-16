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
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "AnimatedLineEditor.h"
#include "ContextMenu.h"
#include "Debug.h"
#include "BaseIcons.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "OptionListBox.h"
#include "TextEditionDelegate.h"
#include "TreeView.h"
#include "XmlOptions.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <algorithm>

//_______________________________________________________
OptionListBox::OptionListBox( QWidget* parent, const QString& name ):
    QWidget( parent ),
    OptionWidget( name ),
    browsable_( false ),
    fileMode_( QFileDialog::AnyFile )
{

    Debug::Throw( "OptionListBox::OptionListBox.\n" );

    QHBoxLayout* layout( new QHBoxLayout() );
    layout->setSpacing(5);
    layout->setMargin(0);
    setLayout( layout );

    // set model editable
    model_.setReadOnly( false );

    // create list
    list_ = new TreeView( this );
    list_->setSortingEnabled( false );
    list_->header()->hide();
    list_->setSelectionMode( QAbstractItemView::ExtendedSelection );
    list_->setModel( &model_ );
    list_->setRootIsDecorated( false );
    list_->setMask( 1<<OptionModel::Current|1<<OptionModel::VALUE );
    list_->setItemDelegate( new TextEditionDelegate( this ) );
    list_->setIconSize( IconSize( IconSize::Small ) );
    layout->addWidget( list_, 1 );

    QVBoxLayout* buttonLayout = new QVBoxLayout();
    buttonLayout->setMargin(0);
    buttonLayout->setSpacing( 5 );
    layout->addLayout( buttonLayout, 0 );

    // list context menu
    QMenu* menu( new ContextMenu( list_ ) );

    // Add button
    QPushButton *button;
    buttonLayout->addWidget( button = new QPushButton( "Add", this ) );
    button->setIcon( IconEngine::get( ICONS::ADD ) );
    connect( button, SIGNAL( clicked( void ) ), SLOT( _add( void ) ) );

    // Add action
    QAction* action;
    addAction( action = new QAction( IconEngine::get( ICONS::ADD ), "Add", this ) );
    connect( action, SIGNAL( triggered( void ) ), SLOT( _add( void ) ) );
    action->setShortcut( QKeySequence::New );
    menu->addAction( action );

    // remove button
    buttonLayout->addWidget( remove_ = new QPushButton( "Remove", this ) );
    connect( remove_, SIGNAL( clicked( void ) ), SLOT( _remove( void ) ) );
    remove_->setIcon( IconEngine::get( ICONS::REMOVE ) );
    remove_->setToolTip( "Remove selected value" );

    addAction( removeAction_ = new QAction( IconEngine::get( ICONS::REMOVE ), "Remove", this ) );
    connect( removeAction_, SIGNAL( triggered( void ) ), SLOT( _remove( void ) ) );
    removeAction_->setShortcut( QKeySequence::Delete );
    removeAction_->setToolTip( "Remove selected value" );
    menu->addAction( action );

    // Edit button
    buttonLayout->addWidget( edit_ = new QPushButton( "Edit", this ) );
    connect( edit_, SIGNAL( clicked( void ) ), SLOT( _edit( void ) ) );
    edit_->setIcon( IconEngine::get( ICONS::EDIT ) );
    edit_->setToolTip( "Edit selected value" );

    addAction( editAction_ = new QAction( IconEngine::get( ICONS::EDIT ),  "Edit", this ) );
    connect( editAction_, SIGNAL( triggered( void ) ), SLOT( _edit( void ) ) );
    menu->addAction( action );

    // set default button
    buttonLayout->addWidget( default_ = new QPushButton( "Default", this ) );
    connect( default_, SIGNAL( clicked( void ) ), SLOT( _setDefault( void ) ) );
    default_->setToolTip( "Set selected value as default\n(move it to the top of the list)" );
    default_->setIcon( IconEngine::get( ICONS::DIALOG_OK_APPLY ) );

    addAction( defaultAction_ = new QAction( IconEngine::get( ICONS::DIALOG_OK_APPLY ), "Default", this ) );
    connect( defaultAction_, SIGNAL( triggered( void ) ), SLOT( _setDefault( void ) ) );
    menu->addAction( action );

    buttonLayout->addStretch(1);

    // set connections
    connect( list_->selectionModel(), SIGNAL( selectionChanged( const QItemSelection& , const QItemSelection& ) ), SLOT( _updateButtons() ) );
    connect( list_->selectionModel(), SIGNAL( currentChanged( const QModelIndex& , const QModelIndex& ) ), SLOT( _updateButtons() ) );
    connect( list_, SIGNAL( activated( const QModelIndex& ) ), SLOT( _setDefault() ) );

    // update buttons
    _updateButtons();

    _setBuddy( this );

}

//_______________________________________________________
void OptionListBox::read( void )
{
    Debug::Throw( "OptionListBox::read.\n" );

    // retrieve all values from Options, insert in list
    Options::List values( XmlOptions::get().specialOptions( optionName() ) );

    // check if one option is default, set first otherwise
    if( !values.empty() && std::find_if( values.begin(), values.end(), Option::HasFlagFTor( Option::Current ) ) == values.end() )
    { values.front().setCurrent( true ); }

    // add to model.
    OptionModel::List options;
    foreach( const Option& option, values )
    { options << OptionPair( optionName(), option ); }

    model_.set( options );
    list_->resizeColumns();

}

//_______________________________________________________
void OptionListBox::write( void ) const
{
    Debug::Throw( "OptionListBox::write.\n" );

    XmlOptions::get().clearSpecialOptions( optionName() );
    XmlOptions::get().keep( optionName() );

    foreach( const OptionPair& optionPair, model_.children() )
    { XmlOptions::get().add( optionPair.first, optionPair.second ); }

}

//______________________________________________________________________
void OptionListBox::_updateButtons( void )
{
    Debug::Throw( "OptionListBox::_updateButtons.\n" );

    QModelIndex current( list_->selectionModel()->currentIndex() );
    edit_->setEnabled( current.isValid() && model_.get( current ).second.hasFlag( Option::Recordable ) );
    editAction_->setEnabled( current.isValid() && model_.get( current ).second.hasFlag( Option::Recordable ) );

    default_->setEnabled( current.isValid() );
    defaultAction_->setEnabled( current.isValid() );

    bool removeEnabled( false );
    foreach( const OptionPair& optionPair, model_.get( list_->selectionModel()->selectedRows() ) )
    {
        if( optionPair.second.hasFlag( Option::Recordable ) )
        {
            removeEnabled = true;
            break;
        }
    }

    removeAction_->setEnabled( removeEnabled );
    remove_->setEnabled( removeEnabled );

}

//_______________________________________________________
void OptionListBox::_add( void )
{
    Debug::Throw( "OptionListBox::_add.\n" );

    // map dialog
    EditDialog dialog( this, browsable_, fileMode_ );
    dialog.checkbox().setChecked( true );

    if( dialog.centerOnParent().exec() == QDialog::Rejected ) return;
    if( dialog.editor().text().isEmpty() ) return;

    // create new item
    Options::Pair option( optionName(), dialog.editor().text() );
    model_.add( option );

    // make sure item is selected
    QModelIndex index( model_.index( option ) );
    if( index != list_->selectionModel()->currentIndex() )
    {
        list_->selectionModel()->select( index,  QItemSelectionModel::Clear|QItemSelectionModel::Select|QItemSelectionModel::Rows );
        list_->selectionModel()->setCurrentIndex( index,  QItemSelectionModel::Current|QItemSelectionModel::Rows );
    }

    // update default state
    if( dialog.checkbox().isEnabled() && dialog.checkbox().isChecked() )
    { _setDefault(); }

}

//_______________________________________________________
void OptionListBox::_edit( void )
{
    Debug::Throw( "OptionListBox::_edit.\n" );

    // retrieve selection
    QModelIndex current( list_->selectionModel()->currentIndex() );
    assert( current.isValid() );

    Options::Pair option( model_.get( current ) );
    assert( option.second.hasFlag( Option::Recordable ) );

    // create dialog
    EditDialog dialog( this, browsable_, fileMode_ );
    dialog.editor().setText( option.second.raw() );
    if( option.second.isCurrent() )
    {
        dialog.checkbox().setChecked( true );
        dialog.checkbox().setEnabled( false );
    }

    // map dialog
    if( dialog.centerOnParent().exec() == QDialog::Rejected ) return;
    if( dialog.editor().text().isEmpty() ) return;

    // first remove old option
    model_.remove( option );

    // modify value
    option.second.setRaw( dialog.editor().text() );

    // re-add to model
    model_.add( option );

    // make sure item is selected
    QModelIndex index( model_.index( option ) );
    if( index != list_->selectionModel()->currentIndex() )
    {
        list_->selectionModel()->select( index,  QItemSelectionModel::Clear|QItemSelectionModel::Select|QItemSelectionModel::Rows );
        list_->selectionModel()->setCurrentIndex( index,  QItemSelectionModel::Current|QItemSelectionModel::Rows );
    }

    // update default state
    if( dialog.checkbox().isEnabled() && dialog.checkbox().isChecked() )
    { _setDefault(); }

    return;

}

//_______________________________________________________
void OptionListBox::_remove( void )
{
    Debug::Throw( "OptionListBox::_remove.\n" );

    // retrieve selected items; retrieve only recordable options
    OptionModel::List removed;
    foreach( const OptionPair& optionPair, model_.get( list_->selectionModel()->selectedRows() ) )
    { if( optionPair.second.hasFlag( Option::Recordable ) ) removed << optionPair; }

    // remove
    model_.remove( removed );

    return;

}

//_______________________________________________________
void OptionListBox::_setDefault( void )
{
    Debug::Throw( "OptionListBox::_setDefault.\n" );

    // retrieve selection
    QModelIndex current( list_->selectionModel()->currentIndex() );
    assert( current.isValid() );

    // get matching option
    Options::Pair currentOption( model_.get( current ) );

    OptionModel::List options( model_.children() );
    for( OptionModel::List::iterator iter = options.begin(); iter != options.end(); ++iter )
    { iter->second.setCurrent( false ); }

    model_.set( options );

    // model_.remove( currentOption );
    currentOption.second.setCurrent( true );
    model_.add( currentOption );

    list_->resizeColumns();

}

//_______________________________________________________
OptionListBox::EditDialog::EditDialog( QWidget* parent, bool browsable, QFileDialog::FileMode mode ):
    CustomDialog( parent )
{

    setOptionName( "OPTIONLISTBOX_EDIT" );
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setMargin(0);
    vLayout->setSpacing(5);
    mainLayout().addLayout( vLayout );

    if( browsable )
    {

        BrowsedLineEditor* browseEditor = new BrowsedLineEditor( this );
        vLayout->addWidget( browseEditor );
        browseEditor->setFileMode( mode );
        editor_ = &browseEditor->editor();

    } else {

        editor_ = new BrowsedLineEditor::Editor( this );
        vLayout->addWidget( &editor() );

    }

    vLayout->addWidget( checkbox_ = new QCheckBox( "Set as default", this ) );

    setMinimumSize( QSize( 320, 0 ) );

    return;
}
