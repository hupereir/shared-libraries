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

#include "OptionListBox.h"

#include "ContextMenu.h"
#include "CustomDialog.h"
#include "Debug.h"
#include "BaseIconNames.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "LineEditor.h"
#include "OptionModel.h"
#include "TextEditionDelegate.h"
#include "TreeView.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <algorithm>

//_______________________________________________________
class EditDialog: public CustomDialog
{
    public:

    //! constructor
    explicit EditDialog( QWidget*, bool, QFileDialog::FileMode );

    //! editor
    BrowsedLineEditor::Editor& editor() const
    { return *editor_; }

    //! checkbox
    QCheckBox& checkbox() const
    { return *checkbox_; }

    private:

    //! editor
    BrowsedLineEditor::Editor* editor_;

    //! default checkbox
    QCheckBox* checkbox_;

};

//_______________________________________________________
EditDialog::EditDialog( QWidget* parent, bool browsable, QFileDialog::FileMode mode ):
    CustomDialog( parent )
{

    setOptionName( "OPTIONLISTBOX_EDIT" );
    QVBoxLayout* vLayout = new QVBoxLayout;
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
        vLayout->addWidget( editor_ );

    }

    vLayout->addWidget( checkbox_ = new QCheckBox( tr( "Set as default" ), this ) );

    setMinimumSize( QSize( 320, 0 ) );

    return;
}

//_______________________________________________________
OptionListBox::OptionListBox( QWidget* parent, const QString& name ):
    QWidget( parent ),
    OptionWidget( name, this )
{

    Debug::Throw( "OptionListBox::OptionListBox.\n" );

    QHBoxLayout* layout( new QHBoxLayout );
    layout->setSpacing(5);
    layout->setMargin(0);
    setLayout( layout );

    // set model editable
    model_ = new OptionModel( this );
    model_->setReadOnly( false );

    // create list
    list_ = new TreeView( this );
    list_->setSortingEnabled( false );
    list_->header()->hide();
    list_->setSelectionMode( QAbstractItemView::ExtendedSelection );
    list_->setModel( model_ );
    list_->setRootIsDecorated( false );
    list_->setMask( 1<<OptionModel::Current|1<<OptionModel::Value );

    // replace item delegate
    if( list_->itemDelegate() ) list_->itemDelegate()->deleteLater();
    list_->setItemDelegate( new TextEditionDelegate( this ) );

    list_->setIconSize( IconSize::get( IconSize::Small ) );
    layout->addWidget( list_, 1 );

    QVBoxLayout* buttonLayout = new QVBoxLayout;
    buttonLayout->setMargin(0);
    buttonLayout->setSpacing( 5 );
    layout->addLayout( buttonLayout, 0 );

    // list context menu
    QMenu* menu( new ContextMenu( list_ ) );

    // Add button
    QPushButton *button;
    buttonLayout->addWidget( button = new QPushButton( tr( "Add" ), this ) );
    button->setIcon( IconEngine::get( IconNames::Add ) );
    connect( button, SIGNAL(clicked()), SLOT(_add()) );

    // Add action
    QAction* action;
    addAction( action = new QAction( IconEngine::get( IconNames::Add ), tr( "Add" ), this ) );
    connect( action, SIGNAL(triggered()), SLOT(_add()) );
    action->setShortcut( QKeySequence::New );
    menu->addAction( action );

    // remove button
    buttonLayout->addWidget( remove_ = new QPushButton( tr( "Remove" ), this ) );
    connect( remove_, SIGNAL(clicked()), SLOT(_remove()) );
    remove_->setIcon( IconEngine::get( IconNames::Remove ) );
    remove_->setToolTip( tr( "Remove selected value" ) );

    addAction( removeAction_ = new QAction( IconEngine::get( IconNames::Remove ), tr( "Remove" ), this ) );
    connect( removeAction_, SIGNAL(triggered()), SLOT(_remove()) );
    removeAction_->setShortcut( QKeySequence::Delete );
    removeAction_->setToolTip( tr( "Remove selected value" ) );
    menu->addAction( action );

    // Edit button
    buttonLayout->addWidget( edit_ = new QPushButton( tr( "Edit" ), this ) );
    connect( edit_, SIGNAL(clicked()), SLOT(_edit()) );
    edit_->setIcon( IconEngine::get( IconNames::Edit ) );
    edit_->setToolTip( tr( "Edit selected value" ) );

    addAction( editAction_ = new QAction( IconEngine::get( IconNames::Edit ),  tr( "Edit" ), this ) );
    connect( editAction_, SIGNAL(triggered()), SLOT(_edit()) );
    menu->addAction( action );

    // set default button
    buttonLayout->addWidget( default_ = new QPushButton( tr( "Default" ), this ) );
    connect( default_, SIGNAL(clicked()), SLOT(_setDefault()) );
    default_->setToolTip( tr( "Set selected value as default\n(move it to the top of the list)" ) );
    default_->setIcon( IconEngine::get( IconNames::DialogAccept ) );

    addAction( defaultAction_ = new QAction( IconEngine::get( IconNames::DialogAccept ), tr( "Default" ), this ) );
    connect( defaultAction_, SIGNAL(triggered()), SLOT(_setDefault()) );
    menu->addAction( action );

    buttonLayout->addStretch(1);

    // set connections
    connect( list_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(_updateButtons()) );
    connect( list_->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(_updateButtons()) );
    connect( list_, SIGNAL(activated(QModelIndex)), SLOT(_setDefault()) );

    // update buttons
    _updateButtons();

}

//_______________________________________________________
void OptionListBox::_setModel( OptionModel* model )
{
    Debug::Throw( "OptionListBox::_setModel.\n" );
    if( list_ )
    {
        list_->setModel( model );
        connect( list_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(_updateButtons()) );
        connect( list_->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(_updateButtons()) );
    }

    if( model_ ) model_->deleteLater();
    model_ = model;
    model_->setReadOnly( false );

}

//_______________________________________________________
void OptionListBox::read( const Options& options )
{
    Debug::Throw( "OptionListBox::read.\n" );

    // retrieve all values from Options, insert in list
    Options::List values( options.specialOptions( optionName() ) );

    // check if one option is default, set first otherwise
    if( !values.empty() && std::none_of( values.begin(), values.end(), Option::HasFlagFTor( Option::Flag::Current ) ) )
    { values.front().setCurrent( true ); }

    // add to model.
    OptionModel::List optionList;
    for( const auto& option:values )
    { optionList.append( Options::Pair( optionName(), option ) ); }

    model_->set( optionList );
    list_->resizeColumns();

    _setConnected();

}

//_______________________________________________________
void OptionListBox::write( Options& options ) const
{
    Debug::Throw( "OptionListBox::write.\n" );

    options.clearSpecialOptions( optionName() );
    options.keep( optionName() );

    for( const auto& optionPair:model_->children() )
    { options.add( optionPair.first, optionPair.second ); }

}

//______________________________________________________________________
void OptionListBox::_updateButtons()
{
    Debug::Throw( "OptionListBox::_updateButtons.\n" );

    QModelIndex current( list_->selectionModel()->currentIndex() );
    edit_->setEnabled( current.isValid() && model_->get( current ).second.hasFlag( Option::Flag::Recordable ) );
    editAction_->setEnabled( current.isValid() && model_->get( current ).second.hasFlag( Option::Flag::Recordable ) );

    default_->setEnabled( current.isValid() );
    defaultAction_->setEnabled( current.isValid() );

    const auto list = model_->get( list_->selectionModel()->selectedRows() );
    const bool removeEnabled( std::any_of( list.begin(), list.end(), Options::HasFlagFTor( Option::Flag::Recordable ) ) );

    removeAction_->setEnabled( removeEnabled );
    remove_->setEnabled( removeEnabled );

}

//_______________________________________________________
void OptionListBox::_add()
{
    Debug::Throw( "OptionListBox::_add.\n" );

    // map dialog
    EditDialog dialog( this, browsable_, fileMode_ );
    dialog.checkbox().setChecked( true );

    if( dialog.centerOnParent().exec() == QDialog::Rejected ) return;
    if( dialog.editor().text().isEmpty() ) return;

    // create new item
    Options::Pair option( optionName(), Option(dialog.editor().text()) );
    model_->add( option );

    // make sure item is selected
    QModelIndex index( model_->index( option ) );
    if( index != list_->selectionModel()->currentIndex() )
    {
        list_->selectionModel()->select( index,  QItemSelectionModel::Clear|QItemSelectionModel::Select|QItemSelectionModel::Rows );
        list_->selectionModel()->setCurrentIndex( index,  QItemSelectionModel::Current|QItemSelectionModel::Rows );
    }

    // update default state
    if( dialog.checkbox().isEnabled() && dialog.checkbox().isChecked() )
    {

        _setDefault();

    } else if( _connected() ) {

        emit modified();

    }

}

//_______________________________________________________
void OptionListBox::_edit()
{
    Debug::Throw( "OptionListBox::_edit.\n" );

    // retrieve selection
    QModelIndex current( list_->selectionModel()->currentIndex() );
    auto source( model_->get( current ) );

    // create dialog
    EditDialog dialog( this, browsable_, fileMode_ );
    dialog.editor().setText( source.second.raw() );
    if( source.second.isCurrent() )
    {
        dialog.checkbox().setChecked( true );
        dialog.checkbox().setEnabled( false );
    }

    // map dialog
    if( dialog.centerOnParent().exec() == QDialog::Rejected ) return;
    if( dialog.editor().text().isEmpty() ) return;

    // modify value
    auto destination( source );
    destination.second.setRaw( dialog.editor().text() );

    // re-add to model
    model_->replace( source, destination );

    // make sure item is selected
    QModelIndex index( model_->index( destination ) );
    if( index != list_->selectionModel()->currentIndex() )
    {
        list_->selectionModel()->select( index,  QItemSelectionModel::Clear|QItemSelectionModel::Select|QItemSelectionModel::Rows );
        list_->selectionModel()->setCurrentIndex( index,  QItemSelectionModel::Current|QItemSelectionModel::Rows );
    }

    // update default state
    if( dialog.checkbox().isEnabled() && dialog.checkbox().isChecked() )
    {

        _setDefault();

    } else if( _connected() ) {

        emit modified();

    }

    return;

}

//_______________________________________________________
void OptionListBox::_remove()
{
    Debug::Throw( "OptionListBox::_remove.\n" );

    // retrieve selected items; retrieve only recordable options
    OptionModel::List removed;
    for( const auto& optionPair:model_->get( list_->selectionModel()->selectedRows() ) )
    { if( optionPair.second.hasFlag( Option::Flag::Recordable ) ) removed.append( optionPair ); }

    // remove
    model_->remove( removed );
    if( _connected() ) emit modified();

    return;

}

//_______________________________________________________
void OptionListBox::_setDefault()
{
    Debug::Throw( "OptionListBox::_setDefault.\n" );

    // retrieve selection
    QModelIndex current( list_->selectionModel()->currentIndex() );
    Q_ASSERT( current.isValid() );

    // get matching option
    auto currentOption( model_->get( current ) );

    OptionModel::List options( model_->children() );
    for( auto& option:options ) option.second.setCurrent( false );

    model_->set( options );

    // model_->remove( currentOption );
    currentOption.second.setCurrent( true );
    model_->add( currentOption );

    list_->resizeColumns();

    // emit signal
    if( _connected() ) emit modified();

}
