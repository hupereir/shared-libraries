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

#include "SvgConfiguration.h"
#include "Debug.h"
#include "Dialog.h"
#include "GridLayout.h"
#include "OptionCheckBox.h"
#include "OptionComboBox.h"
#include "OptionListBox.h"
#include "QtUtil.h"
#include "TransparencyConfiguration.h"
#include "XmlOptions.h"



#include <QGroupBox>
#include <QButtonGroup>
#include <QLabel>
#include <QStackedWidget>

namespace Svg
{
    //! use to select background style
    class StyleOptionWidget: public ComboBox, public OptionWidget
    {

        Q_OBJECT

        public:

        //! constructor
        explicit StyleOptionWidget( QWidget* parent ):
            ComboBox( parent ),
            OptionWidget( QStringLiteral("DUMMY_OPTION") )
        {

            setEditable( false );

            addItem( tr( "Flat" ) );
            addItem( tr( "Decorated" ) );

            #if defined(Q_OS_LINUX)
            addItem( tr( "System (KDE only)" ) );
            #endif

            setCurrentIndex(0);

        }

        //! destructor
        virtual ~StyleOptionWidget()
        {}

        //! read
        virtual void read( const Options& options )
        {
            if( options.get<bool>( QStringLiteral("USE_SVG") ) )
            {

                #if defined(Q_OS_LINUX)
                if( options.get<bool>( QStringLiteral("SVG_USE_PLASMA_INTERFACE") ) ) setCurrentIndex(2);
                else
                #endif
                { setCurrentIndex(1); }

            } else setCurrentIndex(0);

            if( !_connected() )
            {
                connect( this, QOverload<int>::of( &StyleOptionWidget::currentIndexChanged ), [this](int){ emit modified(); } );
                _setConnected();
            }

        }

        //! write
        virtual void write( Options& options ) const
        {
            options.set<bool>( QStringLiteral("USE_SVG"), currentIndex() != 0 );
            #if defined(Q_OS_LINUX)
            options.set<bool>( QStringLiteral("SVG_USE_PLASMA_INTERFACE"), currentIndex() == 2 );
            #endif
        }

        Q_SIGNALS:

        //! modified
        void modified();

    };

}

// need to be included after definition of StyleOptionWidget
#include "SvgConfiguration.moc"

namespace Svg
{

    //______________________________________________________________________
    SvgConfiguration::SvgConfiguration( QWidget* parent ):
        QWidget( parent ),
        OptionWidgetList( this ),
        Counter( QStringLiteral("Svg::SvgConfiguration") )
    {

        Debug::Throw( QStringLiteral("SvgConfiguration::SvgConfiguration.\n") );

        QVBoxLayout* vLayout = new QVBoxLayout;
        QtUtil::setMargin(vLayout, 0);
        setLayout( vLayout );

        QHBoxLayout* hLayout = new QHBoxLayout;
        QtUtil::setMargin(hLayout, 2);
        vLayout->addLayout( hLayout );

        QLabel* label;
        hLayout->addWidget( label = new QLabel( tr( "Background style:" ), this ) );

        StyleOptionWidget* styleWidget;
        hLayout->addWidget( styleWidget = new StyleOptionWidget( this ) );
        addOptionWidget( styleWidget );
        label->setBuddy( styleWidget );

        hLayout->addStretch(1);

        // stacked widget
        QStackedWidget* stackedWidget;
        vLayout->addWidget( stackedWidget = new QStackedWidget( this ) );

        // flat background configuration
        QWidget* box;
        stackedWidget->addWidget( box = new QGroupBox( tr( "Flat Background Settings" ) ) );
        box->setLayout( vLayout = new QVBoxLayout );

        Transparency::TransparencyConfiguration* transparencyConfiguration;
        vLayout->addWidget( transparencyConfiguration = new Transparency::TransparencyConfiguration( box, Transparency::TransparencyConfiguration::Background ));
        QtUtil::setMargin(transparencyConfiguration->layout(), 0);
        addOptionWidget( transparencyConfiguration );
        vLayout->addStretch(1);

        // svg
        stackedWidget->addWidget( box = new QGroupBox( tr( "Decorated Background Settings" ) ) );
        box->setLayout( vLayout = new QVBoxLayout );

        // SVG file
        vLayout->addWidget( label = new QLabel( tr( "Svg files:" ), box ) );

        OptionListBox *listbox = new OptionListBox( box, QStringLiteral("SVG_BACKGROUND") );
        listbox->setBrowsable( true );
        listbox->setToolTip( tr( "Pathname to load background svg" ) );
        vLayout->addWidget( listbox );
        addOptionWidget( listbox );
        label->setBuddy( listbox );

        // plasma interface
        #if defined(Q_OS_LINUX)
        stackedWidget->addWidget( box = new QGroupBox( tr( "System Background Settings" ) ) );
        box->setLayout( vLayout = new QVBoxLayout );

        hLayout = new QHBoxLayout;
        QtUtil::setMargin(hLayout, 0);
        vLayout->addLayout( hLayout );

        GridLayout* gridLayout = new GridLayout;
        gridLayout->setSpacing(5);
        QtUtil::setMargin(gridLayout, 0);
        gridLayout->setMaxCount(2);
        gridLayout->setColumnAlignment( 0, Qt::AlignVCenter|Qt::AlignRight );
        hLayout->addLayout( gridLayout );
        hLayout->addStretch( 1 );

        gridLayout->addWidget( label = new QLabel( tr( "Image path:" ), box ) );

        OptionComboBox* plasmaImagePath;
        gridLayout->addWidget( plasmaImagePath = new OptionComboBox( box, QStringLiteral("SVG_PLASMA_IMAGE_PATH") ) );
        plasmaImagePath->setUseValue( false );
        plasmaImagePath->addItems( { "dialogs/background", "widgets/background", "widgets/translucentbackground" } );
        plasmaImagePath->setToolTip( tr( "Relative path of the svg file used for the background" ) );
        addOptionWidget( plasmaImagePath );
        label->setBuddy( plasmaImagePath );

        gridLayout->addWidget( label = new QLabel( tr( "Draw overlay" ), box ) );
        OptionCheckBox* checkbox;
        gridLayout->addWidget( checkbox = new OptionCheckBox( QString(), box, QStringLiteral("SVG_DRAW_OVERLAY") ) );
        checkbox->setToolTip( tr( "If checked, overlay pictures, if any, found in the SVG file, are drawn on top of the background" ) );
        addOptionWidget( checkbox );
        label->setBuddy( checkbox );

        vLayout->addStretch(1);
        #endif

        connect( styleWidget, QOverload<int>::of( &StyleOptionWidget::currentIndexChanged ), stackedWidget, &QStackedWidget::setCurrentIndex );

    }

}
