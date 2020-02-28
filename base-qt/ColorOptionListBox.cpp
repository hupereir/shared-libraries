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

#include "ColorOptionListBox.h"

#include "Color.h"
#include "ColorDisplay.h"
#include "ColorOptionModel.h"
#include "Dialog.h"
#include "TreeView.h"

//_______________________________________________________
class EditDialog: public Dialog
{
    Q_OBJECT

    public:

    //* constructor
    explicit EditDialog( QWidget* );

    //* editor
    ColorDisplay& editor() const
    { return *editor_; }

    //* checkbox
    QCheckBox& checkbox() const
    { return *checkbox_; }

    private:

    //* editor
    ColorDisplay* editor_;

    //* default checkbox
    QCheckBox* checkbox_;

};

//_______________________________________________________
EditDialog::EditDialog( QWidget* parent ):
    Dialog( parent )
{

    setOptionName( QStringLiteral("OPTIONLISTBOX_EDIT") );
    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->setMargin(0);
    vLayout->setSpacing(5);
    mainLayout().addLayout( vLayout );

    vLayout->addWidget( editor_ = new ColorDisplay( this ) );
    vLayout->addWidget( checkbox_ = new QCheckBox( tr( "Set as default" ), this ) );

    setMinimumSize( QSize( 320, 0 ) );

    return;
}

//_______________________________________________________
ColorOptionListBox::ColorOptionListBox( QWidget* parent, const QString& name ):
    OptionListBox( parent, name )
{ _setModel( new ColorOptionModel( this ) ); }

//_______________________________________________________
void ColorOptionListBox::_add()
{
    Debug::Throw( QStringLiteral("OptionListBox::_add.\n") );

    // map dialog
    EditDialog dialog( this );
    dialog.checkbox().setChecked( true );

    if( dialog.centerOnParent().exec() == QDialog::Rejected ) return;
    if( !dialog.editor().color().isValid() ) return;

    // create new item
    Options::Pair option( optionName(), Option().set<Base::Color>( Base::Color(dialog.editor().color()) ) );
    _model().add( option );

    // make sure item is selected
    QModelIndex index( _model().index( option ) );
    if( index != list().selectionModel()->currentIndex() )
    {
        list().selectionModel()->select( index,  QItemSelectionModel::Clear|QItemSelectionModel::Select|QItemSelectionModel::Rows );
        list().selectionModel()->setCurrentIndex( index,  QItemSelectionModel::Current|QItemSelectionModel::Rows );
    }

    // update default state
    if( dialog.checkbox().isEnabled() && dialog.checkbox().isChecked() )
    { _setDefault(); }

}

//_______________________________________________________
void ColorOptionListBox::_edit()
{
    Debug::Throw( QStringLiteral("OptionListBox::_edit.\n") );

    // retrieve selection
    QModelIndex current( list().selectionModel()->currentIndex() );
    auto source( _model().get( current ) );

    // create dialog
    EditDialog dialog( this );
    dialog.editor().setColor( source.second.get<Base::Color>() );
    if( source.second.isCurrent() )
    {
        dialog.checkbox().setChecked( true );
        dialog.checkbox().setEnabled( false );
    }

    // map dialog
    if( dialog.centerOnParent().exec() == QDialog::Rejected ) return;
    if( !dialog.editor().color().isValid() ) return;

    // modify value
    auto destination( source );
    destination.second.set<Base::Color>( Base::Color(dialog.editor().color()) );

    // re-add to model
    _model().replace( source, destination );

    // make sure item is selected
    QModelIndex index( _model().index( destination ) );
    if( index != list().selectionModel()->currentIndex() )
    {
        list().selectionModel()->select( index,  QItemSelectionModel::Clear|QItemSelectionModel::Select|QItemSelectionModel::Rows );
        list().selectionModel()->setCurrentIndex( index,  QItemSelectionModel::Current|QItemSelectionModel::Rows );
    }

    // update default state
    if( dialog.checkbox().isEnabled() && dialog.checkbox().isChecked() )
    { _setDefault(); }

    return;

}

#include "ColorOptionListBox.moc"
