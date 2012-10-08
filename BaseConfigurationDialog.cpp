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
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "BaseIcons.h"
#include "BaseConfigurationDialog.h"
#include "GridLayout.h"
#include "CustomDialog.h"
#include "Debug.h"
#include "IconEngine.h"
#include "OptionCheckBox.h"
#include "OptionColorDisplay.h"
#include "OptionFontEditor.h"
#include "OptionLineEditor.h"
#include "OptionListBox.h"
#include "OptionModel.h"
#include "OptionSpinBox.h"
#include "OptionSlider.h"
#include "QuestionDialog.h"
#include "Str.h"
#include "TreeView.h"

#include <QtGui/QApplication>
#include <QtGui/QScrollArea>
#include <QtGui/QShortcut>
#include <QtGui/QToolTip>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLayout>
#include <QtGui/QLabel>

//_________________________________________________________
BaseConfigurationDialog::BaseConfigurationDialog( QWidget* parent ):
    TabbedDialog( parent ),
    modifiedOptions_( XmlOptions::get() ),
    backupOptions_( XmlOptions::get() )
{

    Debug::Throw( "BaseConfigurationDialog::BaseConfigurationDialog.\n" );
    setWindowTitle( "Configuration" );
    setOptionName( "PREFERENCE_DIALOG" );

    // add restore default button to layout
    QPushButton* button;
    button = new QPushButton( IconEngine::get( ICONS::RELOAD ), "Restore &Defaults", this );
    button->setToolTip( "Restore default value for all options.");
    button->setAutoDefault( false );

    connect( button, SIGNAL( clicked() ), SLOT( _restoreDefaults() ) );
    _buttonLayout().addWidget( button );
    _buttonLayout().addStretch( 1 );

    // apply button
    _buttonLayout().addWidget( button = new QPushButton(IconEngine::get( ICONS::DIALOG_OK_APPLY ), "&Apply", this ) );
    connect( button, SIGNAL( clicked() ), SLOT( _update() ) );
    connect( button, SIGNAL( clicked() ), SIGNAL( apply() ) );
    button->setToolTip(
        "Apply changes to options.\n"
        "Note: the application may have to be restarted so that\n"
        "all changes are taken into account." );

    // ok button
    _buttonLayout().addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_OK ), "&Ok", this ) );
    connect( button, SIGNAL( clicked() ), SLOT( _save() ) );
    connect( button, SIGNAL( clicked() ), SIGNAL( ok() ) );
    connect( button, SIGNAL( clicked() ), SLOT( accept() ) );
    button->setToolTip(
        "Apply changes to options and close window.\n"
        "Note: the application may have to be restarted so that\n"
        "all changes are taken into account." );
    button->setAutoDefault( false );

    // cancel button
    _buttonLayout().addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_CANCEL ), "&Cancel", this ) );
    connect( button, SIGNAL( clicked() ), SLOT( _restore() ) );
    connect( button, SIGNAL( clicked() ), SIGNAL( cancel() ) );
    connect( button, SIGNAL( clicked() ), SLOT( reject() ) );
    button->setToolTip( "Discard changes to options and close window" );
    button->setAutoDefault( false );

    // close accelerator
    new QShortcut( QKeySequence::Close, this, SLOT( reject() ) );

}

//__________________________________________________
QWidget* BaseConfigurationDialog::baseConfiguration( QWidget* parent, unsigned long flag )
{

    Debug::Throw( "BaseConfigurationDialog::baseConfiguration.\n" );

    if( !parent ) parent = &addPage( IconEngine::get( ICONS::PREFERENCE_GENERAL ), "General", "General application settings" );

    // base
    QWidget* out(0);
    if( flag & BASE )
    {

        // base
        QGroupBox* box;
        box = new QGroupBox( "General", parent );
        parent->layout()->addWidget( box );

        GridLayout* gridLayout = new GridLayout();
        gridLayout->setMaxCount(2);
        box->setLayout( gridLayout );

        // pixmap path
        QLabel* label;
        gridLayout->addWidget( label = new QLabel( "Pixmaps:", box ) );
        label->setAlignment( Qt::AlignVCenter|Qt::AlignRight );

        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->setMargin(0);
        gridLayout->addLayout( hLayout );
        QPushButton *button = new QPushButton( IconEngine::get( ICONS::EDIT ), "Edit Pixmap Path List", box );
        connect( button, SIGNAL( clicked() ), SLOT( _editPixmapPathList() ) );
        hLayout->addWidget( button );
        hLayout->addStretch(1);

        // debug level
        gridLayout->addWidget( label = new QLabel( "Debug level:", box ) );
        label->setAlignment( Qt::AlignVCenter|Qt::AlignRight );

        OptionSpinBox* spinbox = new OptionSpinBox( box, "DEBUG_LEVEL" );
        spinbox->setMinimum( 0 );
        spinbox->setMaximum( 5 );
        spinbox->setToolTip( "Debug verbosity level" );
        gridLayout->addWidget( spinbox );
        addOptionWidget( spinbox );

        // fonts
        box = new QGroupBox( "Fonts", parent );
        parent->layout()->addWidget( box );

        // icon pixmap
        QVBoxLayout* vLayout = new QVBoxLayout();
        box->setLayout( vLayout );

        OptionCheckBox* checkbox( new OptionCheckBox( "Use system font", box, "USE_SYSTEM_FONT" ) );
        vLayout->addWidget( checkbox );
        addOptionWidget( checkbox );

        gridLayout = new GridLayout();
        gridLayout->setMargin(0);
        gridLayout->setMaxCount(2);
        vLayout->addLayout( gridLayout );

        // base font
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        gridLayout->addWidget( label = new QLabel( "Default font:", box ) );
        OptionFontEditor *edit = new OptionFontEditor( box, "FONT_NAME" );
        edit->setToolTip( "Default font name for all widgets" );
        gridLayout->addWidget( edit );
        addOptionWidget( edit );

        connect( checkbox, SIGNAL( toggled( bool ) ), label, SLOT( setDisabled( bool ) ) );
        connect( checkbox, SIGNAL( toggled( bool ) ), edit, SLOT( setDisabled( bool ) ) );

        // fixed font
        gridLayout->addWidget( label = new QLabel( "Fixed font:", box ) );
        edit = new OptionFontEditor( box, "FIXED_FONT_NAME" );
        edit->setToolTip( "Default font name (fixed) for text widgets" );
        gridLayout->addWidget( edit );
        addOptionWidget( edit );

        connect( checkbox, SIGNAL( toggled( bool ) ), label, SLOT( setDisabled( bool ) ) );
        connect( checkbox, SIGNAL( toggled( bool ) ), edit, SLOT( setDisabled( bool ) ) );

        out = box;

    }

    // list
    if( flag & LIST ) { out = listConfiguration( parent ); }

    // edition
    if( flag & TEXTEDIT ) { out = textEditConfiguration( parent ); }

    // animation (go to a new page)
    if( flag & ANIMATIONS ) { out = animationConfiguration(); }

    Debug::Throw( "BaseConfigurationDialog::baseConfiguration - done.\n" );
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

        parent = &addPage( IconEngine::get( ICONS::PREFERENCE_LISTS ), "Lists", "Configure the appearance of item lists" );
        box = new QWidget( parent );

    } else {

        box = new QGroupBox( "Lists", parent );
    }

    QVBoxLayout* vLayout = new QVBoxLayout();
    box->setLayout( vLayout );
    parent->layout()->addWidget( box );

    {
        OptionCheckBox* checkbox;
        checkbox = new OptionCheckBox( "Use alternate color for items in list", box, "USE_ALTERNATE_COLOR" );
        vLayout->addWidget( checkbox );
        addOptionWidget( checkbox );

        const QPalette palette( this->palette() );
        if( palette.color( QPalette::AlternateBase ) == palette.color( QPalette::Base ) )
        { checkbox->setEnabled( false ); }

    }

    GridLayout* gridLayout = new GridLayout();
    gridLayout->setMargin(0);
    gridLayout->setMaxCount(2);
    vLayout->addLayout( gridLayout );

    {
        OptionCheckBox* checkbox;
        checkbox = new OptionCheckBox( "Use different backround for selected column:", box, "USE_SELECTED_COLUMN_COLOR" );
        gridLayout->addWidget( checkbox );
        addOptionWidget( checkbox );

        OptionColorDisplay* color;
        gridLayout->addWidget( color = new OptionColorDisplay( box, "SELECTED_COLUMN_COLOR" ) );
        addOptionWidget( color );

        connect( checkbox, SIGNAL( toggled( bool ) ), color, SLOT( setEnabled( bool ) ) );

    }

    QLabel* label;
    OptionSpinBox* spinbox;
    gridLayout->addWidget( label = new QLabel( "List items icon size:", box ) );
    label->setAlignment( Qt::AlignRight|Qt::AlignVCenter );
    gridLayout->addWidget( spinbox = new OptionSpinBox( box, "LIST_ICON_SIZE" ) );
    spinbox->setToolTip( "Default size of the icons displayed in lists" );
    spinbox->setSuffix( "px" );
    spinbox->setMinimum(8);
    spinbox->setMaximum(96);
    addOptionWidget( spinbox );

    return box;

}

//__________________________________________________
QWidget* BaseConfigurationDialog::textEditConfiguration( QWidget* parent, unsigned long flag )
{

    Debug::Throw( "BaseConfigurationDialog::textEditConfiguration.\n" );

    // make sure parent is valid
    if( !parent ) parent = &addPage( IconEngine::get( ICONS::PREFERENCE_EDITION ), "Text Edition", "Settings for text display and edition" );

    QWidget* out(0);

    // tab emulation
    if( flag & TAB_EMULATION )
    {
        QGroupBox* box = new QGroupBox( "Tab Emulation", parent );
        QVBoxLayout* layout = new QVBoxLayout();
        box->setLayout( layout );
        parent->layout()->addWidget( box );

        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->setMargin(0);
        layout->addLayout( hLayout );

        OptionCheckBox* checkbox = new OptionCheckBox( "Emulate tabs", box, "TAB_EMULATION" );
        checkbox->setToolTip( "Turn on/off tab emulation using space characters" );
        layout->addWidget( checkbox );
        addOptionWidget( checkbox );

        hLayout->addWidget(new QLabel( "Tab size:", box ) );
        OptionSpinBox* spinbox = new OptionSpinBox( box, "TAB_SIZE" );
        spinbox->setMinimum( 2 );
        spinbox->setMaximum( 20 );
        spinbox->setToolTip( "Tab size (in unit of space characters)." );
        hLayout->addWidget( spinbox );
        hLayout->addStretch( 1 );

        addOptionWidget( spinbox );
        out = box;
    }

    // paragraph highlighting
    if( flag & PARAGRAPH_HIGHLIGHT )
    {
        QGroupBox* box = new QGroupBox( "Paragrap Highlighting", parent );
        QVBoxLayout* layout = new QVBoxLayout();
        box->setLayout( layout );
        parent->layout()->addWidget( box );

        OptionCheckBox* checkbox = new OptionCheckBox( "Highlight current paragraph", box, "HIGHLIGHT_PARAGRAPH" );
        checkbox->setToolTip( "Turn on/off current paragraph highlighting" );
        layout->addWidget( checkbox );
        addOptionWidget( checkbox );

        GridLayout* gridLayout = new GridLayout();
        gridLayout->setMargin(0);
        gridLayout->setMaxCount(2);
        layout->addLayout( gridLayout );

        gridLayout->addWidget( new QLabel( "Paragraph highlight color:" ) );
        OptionColorDisplay* color = new OptionColorDisplay( box, "HIGHLIGHT_COLOR" );
        gridLayout->addWidget( color );
        addOptionWidget( color );

        checkbox->setChecked( false );
        color->setEnabled( false );
        connect( checkbox, SIGNAL( toggled( bool ) ), color, SLOT( setEnabled( bool ) ) );
        out = box;
    }

    // box selection
    if( false )
    {
        QGroupBox* box = new QGroupBox( "Box Selection", parent );
        GridLayout* gridLayout = new GridLayout();
        gridLayout->setMaxCount(2);
        box->setLayout( gridLayout );
        parent->layout()->addWidget( box );

        gridLayout->addWidget( new QLabel( "Opacity" ) );
        OptionSlider *slider = new OptionSlider( box, "BOX_SELECTION_ALPHA" );
        slider->setRange( 0, 100 );
        slider->setToolTip( "Alpha channel (i.e. opacity) of the box selection highlight color" );
        gridLayout->addWidget( slider );
        addOptionWidget( slider );

        QLabel* label;
        gridLayout->addWidget( label = new QLabel(
            "Note: box selection is enabled in text editors "
            "only if the corresponding font has fixed pitch."), 2, 0, 1, 2 );

        // wrap
        label->setWordWrap( true );
        out = box;
    }

    // margins
    if( flag & MARGINS )
    {
        QGroupBox* box;
        parent->layout()->addWidget( box = new QGroupBox( "Margin", parent ) );

        GridLayout* gridLayout = new GridLayout();
        gridLayout->setMaxCount(2);
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        box->setLayout( gridLayout );

        OptionColorDisplay* colorDisplay;
        gridLayout->addWidget( new QLabel( "Foreground:", box ) );
        gridLayout->addWidget( colorDisplay = new OptionColorDisplay( box, "MARGIN_FOREGROUND" ) );
        addOptionWidget( colorDisplay );
        colorDisplay->setToolTip( "Margins foreground color" );

        gridLayout->addWidget( new QLabel( "Background:", box ) );
        gridLayout->addWidget( colorDisplay = new OptionColorDisplay( box, "MARGIN_BACKGROUND" ) );
        addOptionWidget( colorDisplay );
        colorDisplay->setToolTip( "Margins background color" );
        out = box;
    }

    if( flag & TEXT_EDITION_FLAGS )
    {

        // misc
        QGroupBox* box;
        parent->layout()->addWidget( box = new QGroupBox( "Options", parent ) );
        QVBoxLayout* layout = new QVBoxLayout();
        box->setLayout( layout );

        OptionCheckBox* checkbox = new OptionCheckBox( "Wrap text", box, "WRAP_TEXT" );
        checkbox->setToolTip( "Turn on/off line wrapping at editor border" );
        layout->addWidget( checkbox );
        addOptionWidget( checkbox );
        layout->addWidget( checkbox = new OptionCheckBox( "Show line numbers", box, "SHOW_LINE_NUMBERS" ) );
        checkbox->setToolTip( "Turn on/off line numbers" );
        addOptionWidget( checkbox );

        QHBoxLayout* hLayout = new QHBoxLayout();
        layout->addLayout( hLayout );
        hLayout->setMargin(0);
        hLayout->addWidget( checkbox = new OptionCheckBox( "Auto-hide mouse cursor after ", box, "AUTOHIDE_CURSOR" ) );
        addOptionWidget( checkbox );

        OptionSpinBox* spinbox;
        hLayout->addWidget( spinbox = new OptionSpinBox( box, "AUTOHIDE_CURSOR_DELAY" ) );
        spinbox->setSuffix( "s" );
        addOptionWidget( spinbox );

        spinbox->setMinimum( 0 );
        spinbox->setMaximum( 10 );

        spinbox->setEnabled( false );
        connect( checkbox, SIGNAL( toggled( bool ) ), spinbox, SLOT( setEnabled( bool ) ) );
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

        parent = &addPage( IconEngine::get( ICONS::PREFERENCE_ANIMATIONS ), "Animations", "Animations configuration" );
        box = new QWidget( parent );
        box->setLayout( new QVBoxLayout() );
        box->layout()->setMargin(0);

    } else {

        box = new QGroupBox( "Animations", parent );
        box->setLayout( new QVBoxLayout() );
        box->layout()->setMargin(5);

    }

    parent->layout()->addWidget( box );

    GridLayout* gridLayout = new GridLayout();
    gridLayout->setSpacing(5);
    gridLayout->setMaxCount(2);
    gridLayout->setMargin(0);
    box->layout()->addItem( gridLayout );

    gridLayout->addWidget( new QLabel( "type", box ), 0, 0, Qt::AlignCenter );
    gridLayout->addWidget( new QLabel( "duration (ms)", box ), 0, 1, Qt::AlignLeft|Qt::AlignVCenter );

    OptionCheckBox* checkbox;
    OptionSpinBox* spinbox;
    gridLayout->addWidget( checkbox = new OptionCheckBox( "Smooth Transitions ", box, "SMOOTH_TRANSITION_ENABLED" ) );
    checkbox->setToolTip( "Enables fading transition when changing display contents." );
    addOptionWidget( checkbox );

    gridLayout->addWidget( spinbox = new OptionSpinBox( box, "SMOOTH_TRANSITION_DURATION" ) );
    spinbox->setSuffix( "ms" );
    spinbox->setMinimum( 10 );
    spinbox->setMaximum( 5000 );
    spinbox->setToolTip( "Smooth transitions duration" );
    addOptionWidget( spinbox );

    checkbox->setChecked( false );
    spinbox->setEnabled( false );
    connect( checkbox, SIGNAL( toggled( bool ) ), spinbox, SLOT( setEnabled( bool ) ) );

    gridLayout->addWidget( checkbox = new OptionCheckBox( "Smooth Scrolling ", box, "SMOOTH_SCROLLING_ENABLED" ) );
    checkbox->setToolTip( "Enables smooth scrolling when using page-up/page-down buttons, or mouse wheel." );
    addOptionWidget( checkbox );

    gridLayout->addWidget( spinbox = new OptionSpinBox( box, "SMOOTH_SCROLLING_DURATION" ) );
    spinbox->setSuffix( "ms" );
    spinbox->setMinimum( 10 );
    spinbox->setMaximum( 5000 );
    spinbox->setToolTip( "Smooth scrolling duration" );
    addOptionWidget( spinbox );

    checkbox->setChecked( false );
    spinbox->setEnabled( false );
    connect( checkbox, SIGNAL( toggled( bool ) ), spinbox, SLOT( setEnabled( bool ) ) );

    QLabel* label;
    gridLayout->addWidget( label = new QLabel( "Frames:", box ) );
    label->setAlignment( Qt::AlignVCenter|Qt::AlignRight );
    spinbox = new OptionSpinBox( box, "ANIMATION_FRAMES" );
    spinbox->setMinimum( 0 );
    spinbox->setMaximum( 1000 );
    spinbox->setToolTip(
        "Maximum number of frames shown for one animation.\n"
        "A large number is recomanded, since frames are \n"
        "dropped whenever the system is too slow anyway." );
    gridLayout->addWidget( spinbox );
    addOptionWidget( spinbox );
    return box;

}

//__________________________________________________
void BaseConfigurationDialog::_editPixmapPathList( void )
{

    CustomDialog dialog( this );

    // store backup
    Options::List backupOptions = XmlOptions::get().specialOptions( "PIXMAP_PATH" );

    OptionListBox *listbox = new OptionListBox( &dialog, "PIXMAP_PATH" );
    listbox->setBrowsable( true );
    listbox->setFileMode( QFileDialog::Directory );
    listbox->setToolTip( "Pathname to load toolbar pixmaps" );
    listbox->read();
    dialog.mainLayout().addWidget( listbox );

    // customize layout
    dialog.layout()->setMargin(0);
    dialog.buttonLayout().setMargin(5);

    if( dialog.exec() ) listbox->write();
    else {

        // restore old values
        XmlOptions::get().clearSpecialOptions( "PIXMAP_PATH" );
        foreach( const Option& option, backupOptions )
        { XmlOptions::get().add( "PIXMAP_PATH", option ); }

    }
    return;

}

//__________________________________________________
void BaseConfigurationDialog::_restore( void )
{
    Debug::Throw( "BaseConfigurationDialog::_restore.\n" );
    if( XmlOptions::get() == backupOptions_ ) return;
    XmlOptions::get() = backupOptions_;
    modifiedOptions_ = backupOptions_;
    read();
    emit configurationChanged();
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

        dialog.setText( "Following options have no default values. Restore default anyway ? " );
        TreeView* view = new TreeView( &dialog );
        OptionModel* model = new OptionModel( &dialog );
        model->add( local );
        view->setModel( model );
        view->setMask( (1<<OptionModel::NAME) | (1<<OptionModel::VALUE) );
        view->resizeColumns();
        dialog.mainLayout().addWidget( view );
        dialog.resize( 450, 300 );

    } else { dialog.setText( "Restore all options to their default values ?" ); }

    // ask confirmation
    if( !dialog.centerOnWidget( this ).exec() ) return;

    XmlOptions::get().restoreDefaults();
    read();
    emit configurationChanged();
}

//__________________________________________________
void BaseConfigurationDialog::_save( void )
{

    Debug::Throw( "BaseConfigurationDialog::_save.\n" );
    _update();
    XmlOptions::write();

}
