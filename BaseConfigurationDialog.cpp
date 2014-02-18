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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "BaseConfigurationDialog.h"
#include "BaseConfigurationDialog.moc"
#include "BaseConfigurationDialog_p.h"

#include "BaseIconNames.h"
#include "GridLayout.h"
#include "CustomDialog.h"
#include "Debug.h"
#include "IconEngine.h"
#include "OptionBrowsedLineEditor.h"
#include "OptionCheckBox.h"
#include "OptionColorDisplay.h"
#include "OptionFontEditor.h"
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

//_________________________________________________________
BaseConfigurationDialog::BaseConfigurationDialog( QWidget* parent, ConfigurationFlags flags ):
    TabbedDialog( parent ),
    OptionWidgetList(this),
    backupOptions_( XmlOptions::get() ),
    pixmapPathDialog_( 0L ),
    iconThemeDialog_( 0L )
{

    Debug::Throw( "BaseConfigurationDialog::BaseConfigurationDialog.\n" );
    setWindowTitle( "Configuration" );
    setOptionName( "PREFERENCE_DIALOG" );

    // restore default
    restoreDefaultsButton_ = new QPushButton( IconEngine::get( IconNames::Revert ), tr( "Defaults" ), this );
    restoreDefaultsButton_->setToolTip( tr( "Restore default value for all options" ) );
    restoreDefaultsButton_->setAutoDefault( false );

    connect( restoreDefaultsButton_, SIGNAL(clicked()), SLOT(_restoreDefaults()) );
    connect( restoreDefaultsButton_, SIGNAL(clicked()), SIGNAL(restoreDefaults()) );
    _buttonLayout().addWidget( restoreDefaultsButton_ );

    // reset
    resetButton_ = new QPushButton( IconEngine::get( IconNames::Revert ), tr( "Reset" ), this );
    resetButton_->setToolTip( tr( "Reset configuration" ) );
    resetButton_->setAutoDefault( false );

    connect( resetButton_, SIGNAL(clicked()), SLOT(_reset()) );
    connect( resetButton_, SIGNAL(clicked()), SIGNAL(reset()) );
    _buttonLayout().addWidget( resetButton_ );
    _buttonLayout().addStretch( 1 );

    // ok button
    _buttonLayout().addWidget( okButton_ = new QPushButton( IconEngine::get( IconNames::DialogOk ), tr( "Ok" ), this ) );
    connect( okButton_, SIGNAL(clicked()), SLOT(_save()) );
    connect( okButton_, SIGNAL(clicked()), SIGNAL(ok()) );
    connect( okButton_, SIGNAL(clicked()), SLOT(accept()) );
    okButton_->setToolTip(
        tr( "Apply changes to options and close window.\n"
        "Note: the application may have to be restarted so that\n"
        "all changes are taken into account" ) );
    okButton_->setAutoDefault( false );

    // apply button
    _buttonLayout().addWidget( applyButton_ = new QPushButton(IconEngine::get( IconNames::DialogAccept ), tr( "Apply" ), this ) );
    connect( applyButton_, SIGNAL(clicked()), SLOT(_apply()) );
    connect( applyButton_, SIGNAL(clicked()), SIGNAL(apply()) );
    applyButton_->setToolTip(
        tr( "Apply changes to options.\n"
        "Note: the application may have to be restarted so that\n"
        "all changes are taken into account" ) );

    // cancel button
    _buttonLayout().addWidget( cancelButton_ = new QPushButton( IconEngine::get( IconNames::DialogCancel ), tr( "Cancel" ), this ) );
    cancelButton_->setShortcut( Qt::Key_Escape );
    connect( cancelButton_, SIGNAL(clicked()), SLOT(_cancel()) );
    connect( cancelButton_, SIGNAL(clicked()), SIGNAL(cancel()) );
    connect( cancelButton_, SIGNAL(clicked()), SLOT(reject()) );
    cancelButton_->setToolTip( tr( "Discard changes to options and close window" ) );
    cancelButton_->setAutoDefault( false );

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
QWidget* BaseConfigurationDialog::baseConfiguration( QWidget* parent, ConfigurationFlags flags )
{

    Debug::Throw( "BaseConfigurationDialog::baseConfiguration.\n" );

    if( !parent ) parent = &addPage(
        IconEngine::get( IconNames::PreferencesGeneral ),
        tr( "General" ),
        tr( "General application settings" ) );

    // base
    QWidget* out(0);
    if( flags&Base )
    {

        // base
        QGroupBox* box;
        box = new QGroupBox( tr( "General" ), parent );
        parent->layout()->addWidget( box );

        {
            QGridLayout* gridLayout = new QGridLayout();
            box->setLayout( gridLayout );

            // pixmap path
            QLabel* label;
            gridLayout->addWidget( label = new QLabel( tr( "Pixmaps:" ), box ), 0, 0, 1, 1 );
            label->setAlignment( Qt::AlignVCenter|Qt::AlignRight );

            QPushButton *button = new QPushButton( IconEngine::get( IconNames::Edit ), tr( "Edit Pixmap Path" ), box );
            connect( button, SIGNAL(clicked()), SLOT(_editPixmapPathList()) );
            gridLayout->addWidget( button, 0, 1, 1, 1 );

            // icon path
            #if QT_VERSION >= 0x040600
            gridLayout->addWidget( label = new QLabel( tr( "Icons:" ), box ), 1, 0, 1, 1 );
            label->setAlignment( Qt::AlignVCenter|Qt::AlignRight );

            button = new QPushButton( IconEngine::get( IconNames::Edit ), tr( "Edit Icon Theme" ), box );
            connect( button, SIGNAL(clicked()), SLOT(_editIconTheme()) );
            gridLayout->addWidget( button, 1, 1, 1, 1 );
            #endif

            // debug level
            gridLayout->addWidget( label = new QLabel( tr( "Verbosity:" ), box ), 2, 0, 1, 1 );
            label->setAlignment( Qt::AlignVCenter|Qt::AlignRight );

            OptionSpinBox* spinbox = new OptionSpinBox( box, "DEBUG_LEVEL" );
            spinbox->setSpecialValueText( tr( " Silent" ) );
            spinbox->setMinimum( 0 );
            spinbox->setMaximum( 5 );
            spinbox->setToolTip( tr( "Debug verbosity level" ) );
            gridLayout->addWidget( spinbox, 2, 1, 1, 2 );
            addOptionWidget( spinbox );

            gridLayout->setColumnStretch( 2, 1 );

        }

        // fonts
        box = new QGroupBox( tr( "Fonts" ), parent );
        parent->layout()->addWidget( box );

        // icon pixmap
        QVBoxLayout* vLayout = new QVBoxLayout();
        box->setLayout( vLayout );

        OptionCheckBox* checkbox( new OptionCheckBox( tr( "Use system font" ), box, "USE_SYSTEM_FONT" ) );
        vLayout->addWidget( checkbox );
        addOptionWidget( checkbox );

        GridLayout* gridLayout = new GridLayout();
        gridLayout->setMargin(0);
        gridLayout->setMaxCount(2);
        vLayout->addLayout( gridLayout );

        // base font
        QLabel* label;
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        gridLayout->addWidget( label = new QLabel( tr( "Default font:" ), box ) );
        OptionFontEditor *edit = new OptionFontEditor( box, "FONT_NAME" );
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
    if( flags&List ) { out = listConfiguration( parent ); }

    // edition
    if( flags&TextEdition ) { out = textEditConfiguration( parent ); }

    // animation (go to a new page)
    if( flags&Animations ) { out = animationConfiguration(); }

    return out;

}

//__________________________________________________
QWidget* BaseConfigurationDialog::listConfiguration( QWidget* parent )
{

    Debug::Throw( "BaseConfigurationDialog::listConfiguration.\n" );

    // make sure parent is valid
    QWidget* box(0);
    if( !parent )
    {

        parent = &addPage(
            IconEngine::get( IconNames::PreferencesLists ),
            tr( "Lists" ), tr( "Configure the appearance of item lists" ) );
        box = new QWidget( parent );

    } else {

        box = new QGroupBox( tr( "Lists" ), parent );
    }

    QVBoxLayout* vLayout = new QVBoxLayout();
    box->setLayout( vLayout );
    parent->layout()->addWidget( box );

    {
        OptionCheckBox* checkbox;
        checkbox = new OptionCheckBox( tr( "Use alternate color for items in list" ), box, "USE_ALTERNATE_COLOR" );
        vLayout->addWidget( checkbox );
        addOptionWidget( checkbox );

        const QPalette palette( this->palette() );
        if( palette.color( QPalette::AlternateBase ) == palette.color( QPalette::Base ) )
        { checkbox->setEnabled( false ); }

    }

    {
        OptionCheckBox* checkbox;
        checkbox = new OptionCheckBox( tr( "Use different backround for selected column" ), box, "USE_SELECTED_COLUMN_COLOR" );
        vLayout->addWidget( checkbox );
        addOptionWidget( checkbox );

    }

    QLabel* label;
    OptionSpinBox* spinbox;

    // icon size in lists
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    vLayout->addLayout( hLayout );
    hLayout->addWidget( label = new QLabel( tr( "List items icon size:" ), box ) );
    label->setAlignment( Qt::AlignRight|Qt::AlignVCenter );
    hLayout->addWidget( spinbox = new OptionSpinBox( box, "LIST_ICON_SIZE" ) );
    spinbox->setToolTip( tr( "Default size of the icons displayed in lists" ) );
    spinbox->setSuffix( tr( "px" ) );
    spinbox->setMinimum(8);
    spinbox->setMaximum(96);
    addOptionWidget( spinbox );

    label->setBuddy( spinbox );

    return box;

}

//__________________________________________________
QWidget* BaseConfigurationDialog::textEditConfiguration( QWidget* parent, ConfigurationFlags flags )
{

    Debug::Throw( "BaseConfigurationDialog::textEditConfiguration.\n" );

    // make sure parent is valid
    if( !parent ) parent = &addPage(
        IconEngine::get( IconNames::PreferencesEdition ),
        tr( "Text Edition" ),
        tr( "Settings for text display and edition" ) );

    QWidget* out(0);

    // tab emulation
    if( flags&TabEmulation )
    {
        QGroupBox* box = new QGroupBox( tr( "Tab Emulation" ), parent );
        QVBoxLayout* layout = new QVBoxLayout();
        box->setLayout( layout );
        parent->layout()->addWidget( box );

        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->setMargin(0);
        layout->addLayout( hLayout );

        OptionCheckBox* checkbox = new OptionCheckBox( tr( "Emulate tabs" ), box, "TAB_EMULATION" );
        checkbox->setToolTip( tr( "Turn on/off tab emulation using space characters" ) );
        layout->addWidget( checkbox );
        addOptionWidget( checkbox );

        hLayout->addWidget(new QLabel( tr( "Tab size:" ), box ) );
        OptionSpinBox* spinbox = new OptionSpinBox( box, "TAB_SIZE" );
        spinbox->setMinimum( 2 );
        spinbox->setMaximum( 20 );
        spinbox->setToolTip( tr( "Tab size (in unit of space characters)." ) );
        hLayout->addWidget( spinbox );
        hLayout->addStretch( 1 );

        addOptionWidget( spinbox );
        out = box;
    }

    // paragraph highlighting
    if( flags&ParagraphHighlight )
    {
        QGroupBox* box = new QGroupBox( tr( "Paragraph Highlighting" ), parent );
        QVBoxLayout* layout = new QVBoxLayout();
        box->setLayout( layout );
        parent->layout()->addWidget( box );

        OptionCheckBox* checkbox = new OptionCheckBox( tr( "Highlight current paragraph" ), box, "HIGHLIGHT_PARAGRAPH" );
        checkbox->setToolTip( tr( "Turn on/off current paragraph highlighting" ) );
        layout->addWidget( checkbox );
        addOptionWidget( checkbox );

        checkbox->setChecked( false );
        out = box;
    }

    // box selection
    if( false )
    {
        QGroupBox* box = new QGroupBox( tr( "Box Selection" ), parent );
        GridLayout* gridLayout = new GridLayout();
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

    // margins
    if( flags&Margins )
    {
        QGroupBox* box;
        parent->layout()->addWidget( box = new QGroupBox( tr( "Margin" ), parent ) );

        GridLayout* gridLayout = new GridLayout();
        gridLayout->setMaxCount(2);
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        box->setLayout( gridLayout );

        QLabel* label;
        OptionColorDisplay* colorDisplay;
        gridLayout->addWidget( label = new QLabel( tr( "Foreground:" ), box ) );
        gridLayout->addWidget( colorDisplay = new OptionColorDisplay( box, "MARGIN_FOREGROUND" ) );
        addOptionWidget( colorDisplay );
        colorDisplay->setToolTip( tr( "Margins foreground color" ) );
        label->setBuddy( colorDisplay );

        gridLayout->addWidget( label = new QLabel( tr( "Background:" ), box ) );
        gridLayout->addWidget( colorDisplay = new OptionColorDisplay( box, "MARGIN_BACKGROUND" ) );
        addOptionWidget( colorDisplay );
        colorDisplay->setToolTip( tr( "Margins background color" ) );
        label->setBuddy( colorDisplay );
        out = box;
    }

    if( flags&TextEditionFlags )
    {

        // misc
        QGroupBox* box;
        parent->layout()->addWidget( box = new QGroupBox( tr( "Options" ), parent ) );
        QVBoxLayout* layout = new QVBoxLayout();
        box->setLayout( layout );

        OptionCheckBox* checkbox = new OptionCheckBox( tr( "Wrap text " ), box, "WRAP_TEXT" );
        checkbox->setToolTip( tr( "Turn on/off line wrapping at editor border" ) );
        layout->addWidget( checkbox );
        addOptionWidget( checkbox );
        layout->addWidget( checkbox = new OptionCheckBox( tr( "Show line numbers" ), box, "SHOW_LINE_NUMBERS" ) );
        checkbox->setToolTip( tr( "Turn on/off display of line numbers" ) );
        addOptionWidget( checkbox );

        QHBoxLayout* hLayout = new QHBoxLayout();
        layout->addLayout( hLayout );
        hLayout->setMargin(0);
        hLayout->addWidget( checkbox = new OptionCheckBox( tr( "Auto hide mouse cursor after " ), box, "AUTOHIDE_CURSOR" ) );
        addOptionWidget( checkbox );

        OptionSpinBox* spinbox;
        hLayout->addWidget( spinbox = new OptionSpinBox( box, "AUTOHIDE_CURSOR_DELAY" ) );
        spinbox->setSuffix( tr( "s" ) );
        addOptionWidget( spinbox );

        spinbox->setMinimum( 0 );
        spinbox->setMaximum( 10 );

        spinbox->setEnabled( false );
        connect( checkbox, SIGNAL(toggled(bool)), spinbox, SLOT(setEnabled(bool)) );
        out = box;
    }

    Debug::Throw( "BaseConfigurationDialog::textEditConfiguration - done.\n" );
    return out;

}

//__________________________________________________
QWidget* BaseConfigurationDialog::animationConfiguration( QWidget* parent )
{

    Debug::Throw( "BaseConfigurationDialog::animationConfiguration.\n" );

    // make sure parent is valid
    QWidget* box(0);

    if( !parent )
    {

        parent = &addPage(
            IconEngine::get( IconNames::PreferencesAnimations ),
            tr( "Animations" ),
            tr( "Animations configuration" ) );
        box = new QWidget( parent );
        box->setLayout( new QVBoxLayout() );
        box->layout()->setMargin(0);

    } else {

        box = new QGroupBox( tr( "Animations" ), parent );
        box->setLayout( new QVBoxLayout() );
        box->layout()->setMargin(5);

    }

    parent->layout()->addWidget( box );

    GridLayout* gridLayout = new GridLayout();
    gridLayout->setSpacing(5);
    gridLayout->setMaxCount(2);
    gridLayout->setMargin(0);
    box->layout()->addItem( gridLayout );

    gridLayout->addWidget( new QLabel( tr( "Type" ), box ), 0, 0, Qt::AlignCenter );
    gridLayout->addWidget( new QLabel( tr( "Duration" ), box ), 0, 1, Qt::AlignLeft|Qt::AlignVCenter );

    OptionCheckBox* checkbox;
    OptionSpinBox* spinbox;
    gridLayout->addWidget( checkbox = new OptionCheckBox( tr( "Smooth transitions" ), box, "SMOOTH_TRANSITION_ENABLED" ) );
    checkbox->setToolTip( tr( "Enables fading transition when changing display contents." ) );
    addOptionWidget( checkbox );

    gridLayout->addWidget( spinbox = new OptionSpinBox( box, "SMOOTH_TRANSITION_DURATION" ) );
    spinbox->setSuffix( tr( "ms" ) );
    spinbox->setMinimum( 10 );
    spinbox->setMaximum( 5000 );
    spinbox->setToolTip( tr( "Smooth transitions duration" ) );
    addOptionWidget( spinbox );

    checkbox->setChecked( false );
    spinbox->setEnabled( false );
    connect( checkbox, SIGNAL(toggled(bool)), spinbox, SLOT(setEnabled(bool)) );

    gridLayout->addWidget( checkbox = new OptionCheckBox( tr( "Smooth scrolling" ), box, "SMOOTH_SCROLLING_ENABLED" ) );
    checkbox->setToolTip( tr( "Enables smooth scrolling when using page-up/page-down buttons, or mouse wheel." ) );
    addOptionWidget( checkbox );

    gridLayout->addWidget( spinbox = new OptionSpinBox( box, "SMOOTH_SCROLLING_DURATION" ) );
    spinbox->setSuffix( tr( "ms" ) );
    spinbox->setMinimum( 10 );
    spinbox->setMaximum( 5000 );
    spinbox->setToolTip( tr( "Smooth scrolling duration" ) );
    addOptionWidget( spinbox );

    checkbox->setChecked( false );
    spinbox->setEnabled( false );
    connect( checkbox, SIGNAL(toggled(bool)), spinbox, SLOT(setEnabled(bool)) );

    QLabel* label;
    gridLayout->addWidget( label = new QLabel( tr( "Frames:" ), box ) );
    label->setAlignment( Qt::AlignVCenter|Qt::AlignRight );
    spinbox = new OptionSpinBox( box, "ANIMATION_FRAMES" );
    spinbox->setMinimum( 0 );
    spinbox->setMaximum( 1000 );
    spinbox->setToolTip(
        tr( "Maximum number of frames shown for one animation.\n"
        "A large number is recomanded, since frames are \n"
        "dropped whenever the system is too slow anyway." ) );
    gridLayout->addWidget( spinbox );
    addOptionWidget( spinbox );
    label->setBuddy( spinbox );

    return box;

}

//__________________________________________________
void BaseConfigurationDialog::debugModification( void )
{
    QObject* sender( this->sender() );
    Debug::Throw(0)
        << "BaseConfigurationDialog::debugModification -"
        << " sender: " << ( sender ? sender->metaObject()->className() : "none" )
        << endl;
}

//__________________________________________________
void BaseConfigurationDialog::_editPixmapPathList( void )
{

    Debug::Throw( "BaseConfigurationDialog::_editPixmapPathList.\n" );

    if( !pixmapPathDialog_ )
    {
        pixmapPathDialog_ = new CustomDialog( this, CustomDialog::CloseButton );
        OptionListBox *listbox = new OptionListBox( pixmapPathDialog_, "PIXMAP_PATH" );
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
void BaseConfigurationDialog::_editIconTheme( void )
{
    Debug::Throw( "BaseConfigurationDialog::_editIconTheme.\n" );
    if( !iconThemeDialog_ )
    {
        iconThemeDialog_ = new IconThemeDialog( this );
        iconThemeDialog_->read( XmlOptions::get() );
        addOptionWidget( iconThemeDialog_ );
    }

    iconThemeDialog_->exec();
    return;
}

//__________________________________________________
bool BaseConfigurationDialog::_checkModified( void )
{
    Options modifiedOptions( XmlOptions::get() );
    OptionWidgetList::write( modifiedOptions );
    const bool modified( !(modifiedOptions == XmlOptions::get() ) );

    // update button state
    applyButton_->setEnabled( modified );
    resetButton_->setEnabled( modified );

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
    for( Options::SpecialMap::const_iterator firstIter = first.specialOptions().constBegin(); firstIter != first.specialOptions().constEnd(); ++firstIter )
    {

        // get matching options in the second set
        if( !second.isSpecialOption( firstIter.key() ) )
        {
            Debug::Throw(0) << "BaseConfigurationDialog::_findModification - special option " << firstIter.key() << " not found in second list" << endl;
            return true;
        }

        // loop over options in first list
        const Options::List options( second.specialOptions( firstIter.key() ) );
        foreach( const Option& option, firstIter.value() )
        {
            // find in second list
            if( options.indexOf( option ) < 0 )
            {

                Debug::Throw(0) << "BaseConfigurationDialog::_findModification - special option " << firstIter.key() << " does not match" << endl;
                Debug::Throw(0) << "BaseConfigurationDialog::_findModification - could not find: " << option << endl;
                Debug::Throw(0) << "BaseConfigurationDialog::_findModification -  first values: " << firstIter.value() << endl;
                Debug::Throw(0) << "BaseConfigurationDialog::_findModification - second values: " << options << endl;
                return true;
            }

        }

    }

    // loop over options and check existence in other map
    for( Options::Map::const_iterator firstIter = first.options().constBegin(); firstIter != first.options().constEnd(); ++firstIter )
    {

        const Options::Map::const_iterator secondIter( second.options().constFind( firstIter.key() ) );
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
void BaseConfigurationDialog::_apply( void )
{
    Debug::Throw( "BaseConfigurationDialog::_apply.\n" );
    if( _checkModified() )
    {

        // write current configuration to XML
        write( XmlOptions::get() );

        // emit signal
        emit configurationChanged();

        // disable buttons
        applyButton_->setEnabled( false );
        resetButton_->setEnabled( false );

    }

}

//__________________________________________________
void BaseConfigurationDialog::_save( void )
{
    Debug::Throw( "BaseConfigurationDialog::_save.\n" );
    _apply();

    // write options to disk
    XmlOptions::write();

}

//__________________________________________________
void BaseConfigurationDialog::_cancel( void )
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
void BaseConfigurationDialog::_reset( void )
{
    Debug::Throw( "BaseConfigurationDialog::_reset.\n" );

    // restore options from XML
    read( XmlOptions::get() );

}

//__________________________________________________
void BaseConfigurationDialog::_restoreDefaults( void )
{
    Debug::Throw( "BaseConfigurationDialog::_restoreDefaults.\n" );

    // list options that have no default values
    const Options::Map& options( XmlOptions::get().options() );
    OptionModel::List local;
    for( Options::Map::const_iterator iter = options.begin(); iter != options.end(); ++iter )
    { if( iter.value().defaultValue().isEmpty() ) local << OptionPair( iter.key(), iter.value() ); }

    QuestionDialog dialog( this );
    dialog.setOptionName( "RESTORE_DEFAULT_DIALOG" );
    if( !local.empty() )
    {

        dialog.setText( tr( "Following options have no default values. Restore default anyway ? " ) );
        TreeView* view = new TreeView( &dialog );
        OptionModel* model = new OptionModel( &dialog );
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
