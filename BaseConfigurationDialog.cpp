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

/*!
\file BaseConfigurationDialog.cpp
\brief base configuration dialog
\author Hugo Pereira
\version $Revision$
\date $Date$
*/
#include <QApplication>
#include <QScrollArea>
#include <QShortcut>
#include <QToolTip>
#include <QGroupBox>
#include <QHeaderView>
#include <QLayout>
#include <QLabel>

#include "BaseIcons.h"
#include "BaseConfigurationDialog.h"
#include "GridLayout.h"
#include "CustomDialog.h"
#include "Debug.h"
#include "IconEngine.h"
#include "OptionCheckBox.h"
#include "OptionColorDisplay.h"
#include "OptionIconBrowsedButton.h"
#include "OptionFontEditor.h"
#include "OptionLineEditor.h"
#include "OptionListBox.h"
#include "OptionModel.h"
#include "OptionSlider.h"
#include "OptionSpinBox.h"
#include "QuestionDialog.h"
#include "Str.h"
#include "TreeView.h"

using namespace std;
using namespace Qt;

//_________________________________________________________
BaseConfigurationDialog::BaseConfigurationDialog( QWidget* parent ):
    TabbedDialog( parent ),
    modifiedOptions_( XmlOptions::get() ),
    backupOptions_( XmlOptions::get() )
{

    Debug::Throw( "BaseConfigurationDialog::BaseConfigurationDialog.\n" );
    setWindowTitle( "Configuration" );
    setOptionName( "CONFIGURATION_DIALOG" );

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

}

//__________________________________________________
void BaseConfigurationDialog::baseConfiguration( QWidget* parent, unsigned long flag )
{

    Debug::Throw( "BaseConfigurationDialog::baseConfiguration.\n" );

    if( !parent ) parent = &addPage( "Base", "Generic settings" );

    // base
    if( flag & BASE )
    {

        // base
        QGroupBox* box;
        box = new QGroupBox( "Generic", parent );
        parent->layout()->addWidget( box );

        QHBoxLayout* hLayout = new QHBoxLayout();
        box->setLayout( hLayout );

        // icon pixmap
        QVBoxLayout* vLayout( new QVBoxLayout() );
        vLayout->setMargin(0);
        hLayout->addLayout( vLayout );

        OptionIconBrowsedButton* editor = new OptionIconBrowsedButton( parent, "ICON_PIXMAP" );
        editor->setToolTip( "Application icon" );
        addOptionWidget( editor );
        vLayout->addWidget( editor );
        vLayout->addStretch(1);
        hLayout->addSpacing( 10 );

        // base options
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setMargin(0);
        hLayout->addLayout( layout );

        GridLayout* gridLayout = new GridLayout();
        gridLayout->setMargin(0);
        gridLayout->setMaxCount(2);
        layout->addLayout( gridLayout );

        // flat theme
        {
            OptionCheckBox* checkbox( new OptionCheckBox( "Use flat plastique theme", box, "USE_FLAT_THEME" ) );
            gridLayout->addWidget( checkbox, 0, 0, 1, 2 );
            checkbox->setToolTip( "Use customized flat plastique theme for widget appearance and layout.\nThe application must be restarted to take changes to this option into account." );
            addOptionWidget( checkbox );
        }

        // size grip
        {
            OptionCheckBox* checkbox( new OptionCheckBox( "Enable size grip", box, "SIZE_GRIP_ENABLED" ) );
            gridLayout->addWidget( checkbox, 1, 0, 1, 2 );
            checkbox->setToolTip( "Draw a small widget in bottom-right corner of windows allowing to resize the window.\nThe application must be restarted to take changes to this option into account." );
            addOptionWidget( checkbox );
        }

        // base font
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        gridLayout->addWidget( new QLabel( "Default font: ", box ) );
        OptionFontEditor *edit = new OptionFontEditor( box, "FONT_NAME" );
        edit->setToolTip( "Default font name for all widgets" );
        gridLayout->addWidget( edit );
        addOptionWidget( edit );

        // fixed font
        gridLayout->addWidget( new QLabel( "Fixed font: ", box ) );
        edit = new OptionFontEditor( box, "FIXED_FONT_NAME" );
        edit->setToolTip( "Default font name (fixed) for text widgets" );
        gridLayout->addWidget( edit );
        addOptionWidget( edit );

        // pixmap path
        gridLayout->addWidget( new QLabel( "Pixmaps: ", box ) );
        QPushButton *button = new QPushButton( "Edit Pixmap Path List", box );
        connect( button, SIGNAL( clicked() ), SLOT( _editPixmapPathList() ) );
        gridLayout->addWidget( button );

        // debug level
        gridLayout->addWidget( new QLabel( "Debug level: ", box ) );
        OptionSpinBox* spinbox = new OptionSpinBox( box, "DEBUG_LEVEL" );
        spinbox->setMinimum( 0 );
        spinbox->setMaximum( 5 );
        spinbox->setToolTip( "Debug verbosity level" );
        gridLayout->addWidget( spinbox );
        addOptionWidget( spinbox );


    }

    // list
    if( flag & LIST ) { listConfiguration( parent ); }

    // edition
    if( flag & TEXTEDIT ) { textEditConfiguration( parent ); }

    // animation (go to a new page)
    if( flag & ANIMATIONS ) { animationConfiguration(); }

    Debug::Throw( "BaseConfigurationDialog::baseConfiguration - done.\n" );
}

//__________________________________________________
void BaseConfigurationDialog::listConfiguration( QWidget* parent )
{

    Debug::Throw( "BaseConfigurationDialog::listConfiguration.\n" );

    // make sure parent is valid
    if( !parent ) parent = &addPage( "Lists", "Look-and-feel for item lists" );

    QGroupBox* box = new QGroupBox( "Lists", parent );
    GridLayout* gridLayout = new GridLayout();
    gridLayout->setMaxCount(2);
    gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
    box->setLayout( gridLayout );
    parent->layout()->addWidget( box );

    OptionColorDisplay* color;
    gridLayout->addWidget( new QLabel( "Item alternate background color: ", box ) );
    gridLayout->addWidget( color = new OptionColorDisplay( box, "ALTERNATE_COLOR" ) );
    color->setToolTip( "Alternate item background color in lists.\n Set it to \"None\" do disable alternate item color." );
    addOptionWidget( color );

    gridLayout->addWidget( new QLabel( "Selected column background color: ", box ) );
    gridLayout->addWidget( color = new OptionColorDisplay( box, "SELECTED_COLUMN_COLOR" ) );
    color->setToolTip( "Selected column background color in lists.\n Set it to \"None\" do disable alternate item color." );
    addOptionWidget( color );

    OptionSpinBox* spinbox;
    gridLayout->addWidget( new QLabel( "List items icon size: ", box ) );
    gridLayout->addWidget( spinbox = new OptionSpinBox( box, "LIST_ICON_SIZE" ) );
    spinbox->setToolTip( "Default size of the icons displayed in lists" );
    spinbox->setMinimum(8);
    spinbox->setMaximum(96);
    addOptionWidget( spinbox );

}

//__________________________________________________
void BaseConfigurationDialog::textEditConfiguration( QWidget* parent, unsigned long flag )
{

    Debug::Throw( "BaseConfigurationDialog::textEditConfiguration.\n" );

    // make sure parent is valid
    if( !parent ) parent = &addPage( "Text display/edition", "Settings for text display and edition" );

    // tab emulation
    if( flag & TAB_EMULATION )
    {
        QGroupBox* box = new QGroupBox( "Tab emulation", parent );
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

        hLayout->addWidget(new QLabel( "Tab size: ", box ) );
        OptionSpinBox* spinbox = new OptionSpinBox( box, "TAB_SIZE" );
        spinbox->setMinimum( 2 );
        spinbox->setMaximum( 20 );
        spinbox->setToolTip( "Tab size (in unit of space characters)." );
        hLayout->addWidget( spinbox );
        hLayout->addStretch( 1 );

        addOptionWidget( spinbox );

    }

    // paragraph highlighting
    if( flag & PARAGRAPH_HIGHLIGHT )
    {
        QGroupBox* box = new QGroupBox( "Paragrap highlighting", parent );
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

        gridLayout->addWidget( new QLabel( "Paragraph highlight color: " ) );
        OptionColorDisplay* color = new OptionColorDisplay( box, "HIGHLIGHT_COLOR" );
        gridLayout->addWidget( color );
        addOptionWidget( color );

        checkbox->setChecked( false );
        color->setEnabled( false );
        connect( checkbox, SIGNAL( toggled( bool ) ), color, SLOT( setEnabled( bool ) ) );

    }

    // box selection
    if( flag & BOX_SELECTION )
    {
        QGroupBox* box = new QGroupBox( "Box selection", parent );
        GridLayout* gridLayout = new GridLayout();
        gridLayout->setMaxCount(2);
        box->setLayout( gridLayout );
        parent->layout()->addWidget( box );

        gridLayout->addWidget( new QLabel( "Alpha channel" ) );
        OptionSlider *slider = new OptionSlider( box, "BOX_SELECTION_ALPHA" );
        slider->setRange( 0, 100 );
        slider->setToolTip( "Alpha channel (i.e. opacity) of the box selection highlight color" );
        gridLayout->addWidget( slider );
        addOptionWidget( slider );

        if( XmlOptions::get().find( "BOX_SELECTION_COLOR" ) )
        {
            gridLayout->addWidget( new QLabel( "Box selection color" ) );
            OptionColorDisplay* color = new OptionColorDisplay( box, "BOX_SELECTION_COLOR" );
            gridLayout->addWidget( color );
            addOptionWidget( color );
        }

        gridLayout->addWidget( new QLabel(
            "Note: box selection is enabled in text editors\n"
            "only if the corresponding font has fixed pitch."), 2, 0, 1, 2 );
    }

    // margins
    if( flag & MARGINS )
    {
        QGroupBox* box;
        parent->layout()->addWidget( box = new QGroupBox( "Margin appearance", parent ) );

        box->setLayout( new QVBoxLayout() );

        OptionCheckBox* checkbox;
        box->layout()->addWidget( checkbox = new OptionCheckBox( "Draw vertical line", box, "MARGIN_VERTICAL_LINE" ) );
        checkbox->setToolTip(
            "Draw vertical line between margin and body text. "
            "Margins are used to display additional informations such as tags and line numbers."
            );
        addOptionWidget( checkbox );

        GridLayout* gridLayout = new GridLayout();
        gridLayout->setMargin(0);
        gridLayout->setMaxCount(2);
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        box->layout()->addItem( gridLayout );

        OptionColorDisplay* color_display;
        gridLayout->addWidget( new QLabel( "Margin foreground: ", box ) );
        gridLayout->addWidget( color_display = new OptionColorDisplay( box, "MARGIN_FOREGROUND" ) );
        addOptionWidget( color_display );
        color_display->setToolTip( "Margins foreground color" );

        gridLayout->addWidget( new QLabel( "Margin background: ", box ) );
        gridLayout->addWidget( color_display = new OptionColorDisplay( box, "MARGIN_BACKGROUND" ) );
        addOptionWidget( color_display );
        color_display->setToolTip( "Margins background color" );

    }

    if( flag & TEXT_EDITION_FLAGS )
    {

        // misc
        QGroupBox* box;
        parent->layout()->addWidget( box = new QGroupBox( "Flags", parent ) );
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

        QLabel* label;
        OptionSpinBox* spinbox;
        hLayout->addWidget( spinbox = new OptionSpinBox( box, "AUTOHIDE_CURSOR_DELAY" ) );
        hLayout->addWidget( label = new QLabel( " seconds", box ) );
        addOptionWidget( spinbox );

        spinbox->setMinimum( 0 );
        spinbox->setMaximum( 10 );

        spinbox->setEnabled( false );
        connect( checkbox, SIGNAL( toggled( bool ) ), spinbox, SLOT( setEnabled( bool ) ) );

    }

    Debug::Throw( "BaseConfigurationDialog::textEditConfiguration - done.\n" );

}

//__________________________________________________
void BaseConfigurationDialog::animationConfiguration( QWidget* parent )
{

    Debug::Throw( "BaseConfigurationDialog::animationConfiguration.\n" );

    // make sure parent is valid
    QGroupBox* box;
    if( !parent ) {
        parent = &addPage( "Animations", "Generic animation settings" );
        box = new QGroupBox( parent );
    } else box = new QGroupBox( "Animations", parent );

    box->setLayout( new QVBoxLayout() );
    parent->layout()->addWidget( box );

    GridLayout* gridLayout = new GridLayout();
    gridLayout->setMargin(0);
    gridLayout->setMaxCount(2);
    box->layout()->addItem( gridLayout );

    gridLayout->addWidget( new QLabel( "type", box ), 0, 0, Qt::AlignHCenter );
    gridLayout->addWidget( new QLabel( "duration (ms)", box ), 0, 1, Qt::AlignHCenter );

    OptionCheckBox* checkbox;
    OptionSpinBox* spinbox;
    gridLayout->addWidget( checkbox = new OptionCheckBox( "Smooth Transitions", box, "SMOOTH_TRANSITION_ENABLED" ) );
    checkbox->setToolTip( "Enables fading transition when changing display contents." );
    addOptionWidget( checkbox );

    gridLayout->addWidget( spinbox = new OptionSpinBox( box, "SMOOTH_TRANSITION_DURATION" ) );
    spinbox->setMinimum( 10 );
    spinbox->setMaximum( 5000 );
    spinbox->setToolTip( "Smooth transitions duration (ms)." );
    addOptionWidget( spinbox );

    checkbox->setChecked( false );
    spinbox->setEnabled( false );
    connect( checkbox, SIGNAL( toggled( bool ) ), spinbox, SLOT( setEnabled( bool ) ) );

    gridLayout->addWidget( checkbox = new OptionCheckBox( "Smooth Scrolling", box, "SMOOTH_SCROLLING_ENABLED" ) );
    checkbox->setToolTip( "Enables smooth scrolling when using page-up/page-down buttons, or mouse wheel." );
    addOptionWidget( checkbox );

    gridLayout->addWidget( spinbox = new OptionSpinBox( box, "SMOOTH_SCROLLING_DURATION" ) );
    spinbox->setMinimum( 10 );
    spinbox->setMaximum( 5000 );
    spinbox->setToolTip( "Smooth scrolling duration (ms)." );
    addOptionWidget( spinbox );

    checkbox->setChecked( false );
    spinbox->setEnabled( false );
    connect( checkbox, SIGNAL( toggled( bool ) ), spinbox, SLOT( setEnabled( bool ) ) );

    gridLayout->addWidget( new QLabel( "Frames: ", box ) );
    spinbox = new OptionSpinBox( box, "ANIMATION_FRAMES" );
    spinbox->setMinimum( 0 );
    spinbox->setMaximum( 1000 );
    spinbox->setToolTip(
        "Maximum number of frames shown for one animation.\n"
        "A large number is recomanded, since frames are \n"
        "dropped whenever the system is too slow anyway." );
    gridLayout->addWidget( spinbox );
    addOptionWidget( spinbox );



}

//__________________________________________________
void BaseConfigurationDialog::_editPixmapPathList( void )
{

    CustomDialog dialog( this );

    // store backup
    Options::List backup_options = XmlOptions::get().specialOptions( "PIXMAP_PATH" );

    dialog.mainLayout().addWidget( new QLabel("Pixmap pathname: ", &dialog ) );
    OptionListBox *listbox = new OptionListBox( &dialog, "PIXMAP_PATH" );
    listbox->setBrowsable( true );
    listbox->setFileMode( QFileDialog::Directory );
    listbox->setToolTip( "Pathname to load toolbar pixmaps" );
    listbox->read();
    dialog.mainLayout().addWidget( listbox );

    //
    if( dialog.exec() ) listbox->write();
    else {
        // restore old values
        XmlOptions::get().clearSpecialOptions( "PIXMAP_PATH" );
        for( Options::List::iterator iter = backup_options.begin(); iter != backup_options.end(); iter++ )
        { XmlOptions::get().add( "PIXMAP_PATH", *iter ); }
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
    _read();
    emit configurationChanged();
}

//__________________________________________________
void BaseConfigurationDialog::_restoreDefaults( void )
{
    Debug::Throw( "BaseConfigurationDialog::_restoreDefaults.\n" );

    // list options that have no default values
    const Options::Map& options( XmlOptions::get().options() );
    OptionModel::Set local;
    for( Options::Map::const_iterator iter = options.begin(); iter != options.end(); iter++ )
    { if( iter->second.defaultValue().isEmpty() ) local.insert( OptionPair( *iter ) ); }

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
    _read();
    emit configurationChanged();
}

//__________________________________________________
void BaseConfigurationDialog::_read( void )
{
    Debug::Throw( "BaseConfigurationDialog::_read.\n" );
    OptionWidgetList::read();
}

//__________________________________________________
void BaseConfigurationDialog::_save( void )
{

    Debug::Throw( "BaseConfigurationDialog::_save.\n" );
    _update();
    XmlOptions::write();

}
