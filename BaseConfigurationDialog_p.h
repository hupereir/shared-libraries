#ifndef BaseConfigurationDialog_p_h
#define BaseConfigurationDialog_p_h

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

#include "CustomDialog.h"
#include "GridLayout.h"
#include "OptionBrowsedLineEditor.h"
#include "OptionCheckBox.h"
#include "OptionLineEditor.h"
#include "OptionWidgetList.h"

#include <QFileDialog>
#include <QLabel>

//_________________________________________________________
class IconThemeDialog: public CustomDialog, public OptionWidgetList
{

    Q_OBJECT

    public:

    //! constructor
    IconThemeDialog( QWidget* parent = 0x0 ):
        CustomDialog( parent, OkButton ),
        OptionWidgetList( this )
    {

        OptionCheckBox* checkBox;
        mainLayout().addWidget( checkBox = new OptionCheckBox( tr( "Use custom icon theme" ), this, "USE_ICON_THEME" ) );
        addOptionWidget( checkBox );

        QWidget* box = new QWidget( this );
        mainLayout().addWidget( box );

        GridLayout* gridLayout = new GridLayout();
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
        connect( checkBox, SIGNAL( toggled( bool ) ), box, SLOT( setEnabled( bool ) ) );

        read( XmlOptions::get() );

    }

    Q_SIGNALS:

    //! modified
    void modified( void );

};

#endif
