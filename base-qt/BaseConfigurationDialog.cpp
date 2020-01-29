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

#include "BaseConfigurationDialog.h"

#include "BaseIconNames.h"
#include "GridLayout.h"
#include "CustomDialog.h"
#include "Debug.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "Operators.h"
#include "OptionBrowsedLineEditor.h"
#include "OptionCheckBox.h"
#include "OptionColorDisplay.h"
#include "OptionFontEditor.h"
#include "OptionIconSizeComboBox.h"
#include "OptionLineEditor.h"
#include "OptionListBox.h"
#include "OptionModel.h"
#include "OptionSpinBox.h"
#include "OptionSlider.h"
#include "QuestionDialog.h"
#include "TreeView.h"

#include <QApplication>
#include <QScrollArea>
#include <QShortcut>
#include <QToolTip>
#include <QGroupBox>
#include <QHeaderView>
#include <QLayout>
#include <QLabel>

namespace
{
    namespace Local
    {

        //_________________________________________________________
        class IconThemeDialog: public CustomDialog, public OptionWidgetList
        {

            Q_OBJECT

            public:

            //* constructor
            explicit IconThemeDialog( QWidget* = nullptr );

            Q_SIGNALS:

            //* modified
            void modified();

        };

        //_________________________________________________________
        IconThemeDialog::IconThemeDialog( QWidget* parent ):
            CustomDialog( parent, OkButton ),
            OptionWidgetList( this )
        {

            OptionCheckBox* checkBox;
            mainLayout().addWidget( checkBox = new OptionCheckBox( tr( "Use custom icon theme" ), this, "USE_ICON_THEME" ) );
            addOptionWidget( checkBox );

            auto box = new QWidget( this );
            mainLayout().addWidget( box );

            auto gridLayout = new GridLayout;
            gridLayout->setMaxCount(2);
            gridLayout->setMargin(0);
            gridLayout->setSpacing(5);
            box->setLayout( gridLayout );

            QLabel* label;
            gridLayout->addWidget( label = new QLabel( tr( "Theme name:" ), box ) );

            {
                OptionLineEditor* editor;
                gridLayout->addWidget( editor = new OptionLineEditor( box, "ICON_THEME" ) );
                addOptionWidget( editor );
                label->setBuddy( editor );
                label->setAlignment( Qt::AlignVCenter|Qt::AlignRight );
            }

            gridLayout->addWidget( label = new QLabel( tr( "Path:" ), box ) );
            {
                OptionBrowsedLineEditor* editor;
                gridLayout->addWidget( editor = new OptionBrowsedLineEditor( box, "ICON_THEME_PATH" ) );
                addOptionWidget( editor );
                label->setBuddy( editor );
                label->setAlignment( Qt::AlignVCenter|Qt::AlignRight );

                editor->setFileMode( QFileDialog::Directory );
                editor->setAcceptMode( QFileDialog::AcceptOpen );
            }

            gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignCenter );

            box->setEnabled( false );
            connect( checkBox, SIGNAL(toggled(bool)), box, SLOT(setEnabled(bool)) );

            read( XmlOptions::get() );

        }

    }

}

//_________________________________________________________
BaseConfigurationDialog::BaseConfigurationDialog( QWidget* parent, Flags flags ):
    TabbedDialog( parent ),
    OptionWidgetList(this),
    backupOptions_( XmlOptions::get() )
{

    Debug::Throw( "BaseConfigurationDialog::BaseConfigurationDialog.\n" );
    setWindowTitle( tr( "Configuration" ) );
    setOptionName( "PREFERENCE_DIALOG" );

    // restore default
    buttonBox().addButton( QDialogButtonBox::RestoreDefaults );
    buttonBox().button( QDialogButtonBox::RestoreDefaults )->setText( tr( "Defaults" ) );
    buttonBox().button( QDialogButtonBox::RestoreDefaults )->setIcon( IconEngine::get( IconNames::Revert ) );
    buttonBox().button( QDialogButtonBox::RestoreDefaults )->setToolTip( tr( "Restore default value for all options" ) );

    connect( buttonBox().button( QDialogButtonBox::RestoreDefaults ), SIGNAL(clicked()), SLOT(_restoreDefaults()) );
    connect( buttonBox().button( QDialogButtonBox::RestoreDefaults ), SIGNAL(clicked()), SIGNAL(restoreDefaults()) );

    // reset
    buttonBox().addButton( QDialogButtonBox::Reset );
    buttonBox().button( QDialogButtonBox::Reset )->setToolTip( tr( "Reset configuration" ) );
    buttonBox().button( QDialogButtonBox::Reset )->setIcon( IconEngine::get( IconNames::Undo ) );
    connect( buttonBox().button( QDialogButtonBox::Reset ), SIGNAL(clicked()), SLOT(_reset()) );
    connect( buttonBox().button( QDialogButtonBox::Reset ), SIGNAL(clicked()), SIGNAL(reset()) );

    // ok button
    buttonBox().addButton( QDialogButtonBox::Ok );
    connect( buttonBox().button( QDialogButtonBox::Ok ), SIGNAL(clicked()), SLOT(_save()) );
    connect( buttonBox().button( QDialogButtonBox::Ok ), SIGNAL(clicked()), SIGNAL(ok()) );
    connect( buttonBox().button( QDialogButtonBox::Ok ), SIGNAL(clicked()), SLOT(accept()) );
    buttonBox().button( QDialogButtonBox::Ok )->setToolTip(
        tr( "Apply changes to options and close window.\n"
        "Note: the application may have to be restarted so that\n"
        "all changes are taken into account" ) );

    // apply button
    buttonBox().addButton( QDialogButtonBox::Apply );
    connect( buttonBox().button( QDialogButtonBox::Apply ), SIGNAL(clicked()), SLOT(_apply()) );
    connect( buttonBox().button( QDialogButtonBox::Apply ), SIGNAL(clicked()), SIGNAL(apply()) );
    buttonBox().button( QDialogButtonBox::Apply )->setToolTip(
        tr( "Apply changes to options.\n"
        "Note: the application may have to be restarted so that\n"
        "all changes are taken into account" ) );

    // cancel button
    buttonBox().addButton( QDialogButtonBox::Cancel );
    connect( buttonBox().button( QDialogButtonBox::Cancel ), SIGNAL(clicked()), SLOT(_cancel()) );
    connect( buttonBox().button( QDialogButtonBox::Cancel ), SIGNAL(clicked()), SIGNAL(cancel()) );
    connect( buttonBox().button( QDialogButtonBox::Cancel ), SIGNAL(clicked()), SLOT(reject()) );
    buttonBox().button( QDialogButtonBox::Cancel )->setToolTip( tr( "Discard changes to options and close window" ) );

    // connection
    connect( this, SIGNAL(modified()), SLOT(_checkModified()));

    // close accelerator
    new QShortcut( QKeySequence::Close, this, SLOT(reject()) );

    // default configuration, as set by flags
    if( flags )
    {
        baseConfiguration( 0, flags );
        read();

    }

}

//__________________________________________________
QWidget* BaseConfigurationDialog::baseConfiguration( QWidget* parent, Flags flags )
{

    Debug::Throw( "BaseConfigurationDialog::baseConfiguration.\n" );

    if( !parent ) parent = &addPage(
        IconEngine::get( IconNames::PreferencesGeneral ),
        tr( "General" ),
        tr( "General application settings" ) );

    // base
    QWidget* out( nullptr );
    if( flags&Flag::Base )
    {

        // base
        auto box = new QGroupBox( tr( "General" ), parent );
        parent->layout()->addWidget( box );

        {
            auto gridLayout = new QGridLayout;
            box->setLayout( gridLayout );

            // widget style
            int row = 0;

            // pixmap path
            QLabel* label;
            gridLayout->addWidget( label = new QLabel( tr( "Pixmaps:" ), box ), ++row, 0, 1, 1 );
            label->setAlignment( Qt::AlignVCenter|Qt::AlignRight );

            auto button = new QPushButton( IconEngine::get( IconNames::Edit ), tr( "Edit Pixmap Path" ), box );
            connect( button, SIGNAL(clicked()), SLOT(_editPixmapPathList()) );
            gridLayout->addWidget( button, row, 1, 1, 1 );

            // icon path
            gridLayout->addWidget( label = new QLabel( tr( "Icons:" ), box ), ++row, 0, 1, 1 );
            label->setAlignment( Qt::AlignVCenter|Qt::AlignRight );

            button = new QPushButton( IconEngine::get( IconNames::Edit ), tr( "Edit Icon Theme" ), box );
            connect( button, SIGNAL(clicked()), SLOT(_editIconTheme()) );
            gridLayout->addWidget( button, row, 1, 1, 1 );

            // debug level
            gridLayout->addWidget( label = new QLabel( tr( "Verbosity:" ), box ), ++row, 0, 1, 1 );
            label->setAlignment( Qt::AlignVCenter|Qt::AlignRight );

            auto spinbox = new OptionSpinBox( box, "DEBUG_LEVEL" );
            spinbox->setSpecialValueText( tr( "Silent" ) );
            spinbox->setMinimum( 0 );
            spinbox->setMaximum( 5 );
            spinbox->setToolTip( tr( "Debug verbosity level" ) );
            gridLayout->addWidget( spinbox, row, 1, 1, 2 );
            addOptionWidget( spinbox );

            gridLayout->setColumnStretch( 2, 1 );

        }

        // fonts
        box = new QGroupBox( tr( "Fonts" ), parent );
        parent->layout()->addWidget( box );

        // icon pixmap
        auto vLayout = new QVBoxLayout;
        box->setLayout( vLayout );

        auto checkbox( new OptionCheckBox( tr( "Use system font" ), box, "USE_SYSTEM_FONT" ) );
        vLayout->addWidget( checkbox );
        addOptionWidget( checkbox );

        auto gridLayout = new GridLayout;
        gridLayout->setMargin(0);
        gridLayout->setMaxCount(2);
        vLayout->addLayout( gridLayout );

        // base font
        QLabel* label;
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        gridLayout->addWidget( label = new QLabel( tr( "Default font:" ), box ) );
        auto edit = new OptionFontEditor( box, "FONT_NAME" );
        edit->setToolTip( tr( "Default font name for all widgets" ) );
        gridLayout->addWidget( edit );
        addOptionWidget( edit );

        connect( checkbox, SIGNAL(toggled(bool)), label, SLOT(setDisabled(bool)) );
        connect( checkbox, SIGNAL(toggled(bool)), edit, SLOT(setDisabled(bool)) );

        // fixed font
        gridLayout->addWidget( label = new QLabel( tr( "Fixed font:" ), box ) );
        edit = new OptionFontEditor( box, "FIXED_FONT_NAME" );
        edit->setToolTip( tr( "Default font name (fixed) for text widgets" ) );
        gridLayout->addWidget( edit );
        addOptionWidget( edit );

        connect( checkbox, SIGNAL(toggled(bool)), label, SLOT(setDisabled(bool)) );
        connect( checkbox, SIGNAL(toggled(bool)), edit, SLOT(setDisabled(bool)) );

        out = box;

    }

    // list
    if( flags&Flag::List ) { out = listConfiguration( parent ); }

    // edition
    if( flags&Flag::TextEdition ) { out = textEditConfiguration( parent ); }

    return out;

}

//__________________________________________________
QWidget* BaseConfigurationDialog::listConfiguration( QWidget* parent )
{

    Debug::Throw( "BaseConfigurationDialog::listConfiguration.\n" );

    // make sure parent is valid
    QWidget* box( nullptr );
    if( !parent )
    {

        parent = &addPage(
            IconEngine::get( IconNames::PreferencesLists ),
            tr( "Lists" ), tr( "Configure the appearance of item lists" ) );
        box = new QWidget( parent );

    } else {

        box = new QGroupBox( tr( "Lists" ), parent );
    }

    auto vLayout = new QVBoxLayout;
    box->setLayout( vLayout );
    parent->layout()->addWidget( box );

    {
        auto checkbox = new OptionCheckBox( tr( "Use alternate color for items in list" ), box, "USE_ALTERNATE_COLOR" );
        vLayout->addWidget( checkbox );
        addOptionWidget( checkbox );

        const auto palette( this->palette() );
        if( palette.color( QPalette::AlternateBase ) == palette.color( QPalette::Base ) )
        { checkbox->setEnabled( false ); }
    }

    {
        auto checkbox = new OptionCheckBox( tr( "Use different backround for selected column" ), box, "USE_SELECTED_COLUMN_COLOR" );
        vLayout->addWidget( checkbox );
        addOptionWidget( checkbox );
    }

    QLabel* label;
    OptionSpinBox* spinbox;

    // icon size in lists
    auto gridLayout = new GridLayout;
    gridLayout->setMargin(0);
    gridLayout->setMaxCount(3);
    gridLayout->setColumnStretch( 2, 1 );
    vLayout->addLayout( gridLayout );
    gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );

    {
        // icon size in lists
        gridLayout->addWidget( label = new QLabel( tr( "List items icon size:" ), box ) );
        auto comboBox = new OptionIconSizeComboBox( box, "LIST_ICON_SIZE" );
        gridLayout->addWidget( comboBox );
        comboBox->setToolTip( tr( "Default size of the icons displayed in lists" ) );
        addOptionWidget( comboBox );
        label->setBuddy( comboBox );

        gridLayout->increment();

    }


    {
        // list item margins
        gridLayout->addWidget( label = new QLabel( tr( "List items margin:" ), box ) );
        gridLayout->addWidget( spinbox = new OptionSpinBox( box, "LIST_ITEM_MARGIN" ) );
        spinbox->setToolTip( tr( "Default margin around items in lists" ) );
        spinbox->setSuffix( tr( "px" ) );
        spinbox->setMinimum(0);
        spinbox->setMaximum(10);
        addOptionWidget( spinbox );
        label->setBuddy( spinbox );
    }

    return box;

}

//__________________________________________________
QWidget* BaseConfigurationDialog::textEditConfiguration( QWidget* parent, Flags flags )
{

    Debug::Throw( "BaseConfigurationDialog::textEditConfiguration.\n" );

    // make sure parent is valid
    if( !parent ) parent = &addPage(
        IconEngine::get( IconNames::PreferencesEdition ),
        tr( "Text Edition" ),
        tr( "Settings for text display and edition" ) );

    QWidget* out = nullptr;

    // tab emulation
    if( flags&Flag::TabEmulation )
    {
        auto box = new QGroupBox( tr( "Tab Emulation" ), parent );
        auto layout = new QVBoxLayout;
        box->setLayout( layout );
        parent->layout()->addWidget( box );

        auto hLayout = new QHBoxLayout;
        hLayout->setMargin(0);
        layout->addLayout( hLayout );

        auto checkbox = new OptionCheckBox( tr( "Emulate tabs" ), box, "TAB_EMULATION" );
        checkbox->setToolTip( tr( "Turn on/off tab emulation using space characters" ) );
        layout->addWidget( checkbox );
        addOptionWidget( checkbox );

        hLayout->addWidget(new QLabel( tr( "Tab size:" ), box ) );
        auto spinbox = new OptionSpinBox( box, "TAB_SIZE" );
        spinbox->setMinimum( 2 );
        spinbox->setMaximum( 20 );
        spinbox->setToolTip( tr( "Tab size (in unit of space characters)." ) );
        hLayout->addWidget( spinbox );
        hLayout->addStretch( 1 );

        addOptionWidget( spinbox );
        out = box;
    }

    // paragraph highlighting
    if( flags&Flag::ParagraphHighlight )
    {
        auto box = new QGroupBox( tr( "Paragraph Highlighting" ), parent );
        auto layout = new QVBoxLayout;
        box->setLayout( layout );
        parent->layout()->addWidget( box );

        auto checkbox = new OptionCheckBox( tr( "Highlight current paragraph" ), box, "HIGHLIGHT_PARAGRAPH" );
        checkbox->setToolTip( tr( "Turn on/off current paragraph highlighting" ) );
        layout->addWidget( checkbox );
        addOptionWidget( checkbox );

        checkbox->setChecked( false );
        out = box;
    }

    // box selection
    if( flags&Flag::BoxSelection )
    {
        auto box = new QGroupBox( tr( "Box Selection" ), parent );
        auto gridLayout = new GridLayout;
        gridLayout->setMaxCount(2);
        box->setLayout( gridLayout );
        parent->layout()->addWidget( box );

        gridLayout->addWidget( new QLabel( tr( "Opacity" ) ) );
        OptionSlider *slider = new OptionSlider( box, "BOX_SELECTION_ALPHA" );
        slider->setRange( 0, 100 );
        slider->setToolTip( tr( "Alpha channel (i.e. opacity) of the box selection highlight color" ) );
        gridLayout->addWidget( slider );
        addOptionWidget( slider );

        QLabel* label;
        gridLayout->addWidget( label = new QLabel(
            tr( "Note: box selection is enabled in text editors "
            "only if the corresponding font has fixed pitch." ) ), 2, 0, 1, 2 );

        // wrap
        label->setWordWrap( true );
        out = box;
    }

    if( flags&Flag::TextEditionFlags )
    {

        // misc
        QGroupBox* box;
        parent->layout()->addWidget( box = new QGroupBox( tr( "Options" ), parent ) );
        auto layout = new QVBoxLayout;
        box->setLayout( layout );

        auto checkbox = new OptionCheckBox( tr( "Wrap text " ), box, "WRAP_TEXT" );
        checkbox->setToolTip( tr( "Turn on/off line wrapping at editor border" ) );
        layout->addWidget( checkbox );
        addOptionWidget( checkbox );
        layout->addWidget( checkbox = new OptionCheckBox( tr( "Show line numbers" ), box, "SHOW_LINE_NUMBERS" ) );
        checkbox->setToolTip( tr( "Turn on/off display of line numbers" ) );
        addOptionWidget( checkbox );

        QLabel* label;
        OptionSpinBox* spinbox;
        auto hLayout = new QHBoxLayout;
        layout->addLayout( hLayout );
        hLayout->setMargin(0);
        hLayout->addWidget( label = new QLabel( tr( "Automatically hide mouse cursor after: " ), box ) );
        hLayout->addWidget( spinbox = new OptionSpinBox( box, "AUTOHIDE_CURSOR_DELAY" ) );
        spinbox->setSuffix( tr( "s" ) );
        addOptionWidget( spinbox );

        spinbox->setSpecialValueText( tr( "Never" ) );
        spinbox->setMinimum( 0 );
        spinbox->setMaximum( 10 );
        out = box;
    }

    Debug::Throw( "BaseConfigurationDialog::textEditConfiguration - done.\n" );
    return out;

}

//__________________________________________________
void BaseConfigurationDialog::_editPixmapPathList()
{

    Debug::Throw( "BaseConfigurationDialog::_editPixmapPathList.\n" );

    if( !pixmapPathDialog_ )
    {
        pixmapPathDialog_ = new CustomDialog( this, CustomDialog::CloseButton );
        auto listbox = new OptionListBox( pixmapPathDialog_, "PIXMAP_PATH" );
        listbox->setBrowsable( true );
        listbox->setFileMode( QFileDialog::Directory );
        pixmapPathDialog_->mainLayout().addWidget( listbox );

        addOptionWidget( listbox );
        listbox->read( XmlOptions::get() );

        // customize layout
        pixmapPathDialog_->layout()->setMargin(0);
        pixmapPathDialog_->buttonLayout().setMargin(5);

    }

    pixmapPathDialog_->exec();
    return;

}

//__________________________________________________
void BaseConfigurationDialog::_editIconTheme()
{
    Debug::Throw( "BaseConfigurationDialog::_editIconTheme.\n" );
    if( !iconThemeDialog_ )
    {
        // create
        auto dialog =  new Local::IconThemeDialog( this );
        dialog->read( XmlOptions::get() );
        addOptionWidget( dialog );

        // assign
        iconThemeDialog_ = dialog;

    }

    iconThemeDialog_->exec();
    return;
}

//__________________________________________________
bool BaseConfigurationDialog::_checkModified()
{
    // make a copy of current options
    auto modifiedOptions( XmlOptions::get() );

    // update with current settings
    OptionWidgetList::write( modifiedOptions );

    // check modifications
    const bool modified( !(modifiedOptions == XmlOptions::get() ) );

    // update button state
    buttonBox().button( QDialogButtonBox::Apply )->setEnabled( modified );
    buttonBox().button( QDialogButtonBox::Reset )->setEnabled( modified );

    Debug::Throw()
        << "BaseConfigurationDialog::_checkModified -"
        << " modified: " << modified
        << endl;

    if( modified && Debug::level() > 0 )
    {
        _findModification( modifiedOptions, XmlOptions::get() );
        _findModification( XmlOptions::get(), modifiedOptions );
    }

    return modified;
}

//__________________________________________________
bool BaseConfigurationDialog::_findModification( const Options& first, const Options& second ) const
{

    // this is a slightly modified version of XmlOptions::_differs
    // check special options
    for( auto&& firstIter = first.specialOptions().constBegin(); firstIter != first.specialOptions().constEnd(); ++firstIter )
    {

        // get matching options in the second set
        if( !second.isSpecialOption( firstIter.key() ) )
        {
            Debug::Throw(0) << "BaseConfigurationDialog::_findModification - special option " << firstIter.key() << " not found in second list" << endl;
            return true;
        }

        // loop over options in first list
        auto&& options( second.specialOptions( firstIter.key() ) );
        for( const auto& option:firstIter.value() )
        {
            // find in second list
            if( options.indexOf( option ) < 0 )
            {

                Debug::Throw(0) << "BaseConfigurationDialog::_findModification - special option " << firstIter.key() << " does not match" << endl;
                Debug::Throw(0) << "BaseConfigurationDialog::_findModification - could not find: " << option << endl;
                Debug::Throw(0) << "BaseConfigurationDialog::_findModification - first values: " << firstIter.value() << endl;
                Debug::Throw(0) << "BaseConfigurationDialog::_findModification - second values: " << options << endl;
                return true;
            }

        }

    }

    // loop over options and check existence in other map
    for( auto&& firstIter = first.options().constBegin(); firstIter != first.options().constEnd(); ++firstIter )
    {

        auto&& secondIter( second.options().constFind( firstIter.key() ) );
        if( secondIter == second.options().constEnd() )
        {

            Debug::Throw(0) << "BaseConfigurationDialog::_findModification - option " << firstIter.key() << " not found." << endl;
            return true;

        } else if( firstIter.value() != secondIter.value() ) {

            Debug::Throw(0) << "BaseConfigurationDialog::_findModification - option " << firstIter.key() << " does not match." << endl;
            Debug::Throw(0) << "BaseConfigurationDialog::_findModification - first: " << firstIter.value() << " second: " << secondIter.value() << endl;
            return true;

        }

    }

    return false;

}

//__________________________________________________
void BaseConfigurationDialog::_apply()
{
    Debug::Throw( "BaseConfigurationDialog::_apply.\n" );
    if( _checkModified() )
    {

        // write current configuration to XML
        write( XmlOptions::get() );

        // emit signal
        emit configurationChanged();

        // disable buttons
        buttonBox().button( QDialogButtonBox::Apply )->setEnabled( false );
        buttonBox().button( QDialogButtonBox::Reset )->setEnabled( false );

    }

}

//__________________________________________________
void BaseConfigurationDialog::_save()
{
    Debug::Throw( "BaseConfigurationDialog::_save.\n" );
    _apply();

    // write options to disk
    XmlOptions::write();

}

//__________________________________________________
void BaseConfigurationDialog::_cancel()
{
    Debug::Throw( "BaseConfigurationDialog::_cancel.\n" );
    if( XmlOptions::get() == backupOptions_ ) return;

    // restore backup
    XmlOptions::get() = backupOptions_;
    read( XmlOptions::get() );
    emit configurationChanged();

    // write options to disk
    XmlOptions::write();

}

//__________________________________________________
void BaseConfigurationDialog::_reset()
{
    Debug::Throw( "BaseConfigurationDialog::_reset.\n" );

    // restore options from XML
    read( XmlOptions::get() );

}

//__________________________________________________
void BaseConfigurationDialog::_restoreDefaults()
{
    Debug::Throw( "BaseConfigurationDialog::_restoreDefaults.\n" );

    // list options that have no default values
    OptionModel::List local;
    auto&& options( XmlOptions::get().options() );
    for( auto&& iter = options.begin(); iter != options.end(); ++iter )
    { if( iter.value().defaultValue().isEmpty() ) local.append( Options::Pair( iter.key(), iter.value() ) ); }

    QuestionDialog dialog( this );
    dialog.setOptionName( "RESTORE_DEFAULT_DIALOG" );
    if( !local.empty() )
    {

        dialog.setText( tr( "Following options have no default values. Restore default anyway ? " ) );
        auto view = new TreeView( &dialog );
        auto model = new OptionModel( &dialog );
        model->add( local );
        view->setModel( model );
        view->setMask( (1<<OptionModel::Name) | (1<<OptionModel::Value) );
        view->resizeColumns();
        dialog.mainLayout().addWidget( view );
        dialog.resize( 450, 300 );

    } else { dialog.setText( tr( "Restore all options to their default values ?" ) ); }

    // ask confirmation
    if( !dialog.centerOnWidget( this ).exec() ) return;

    XmlOptions::get().restoreDefaults();
    read();
    emit configurationChanged();
}

#include "BaseConfigurationDialog.moc"
