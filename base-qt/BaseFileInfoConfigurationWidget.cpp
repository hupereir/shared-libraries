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

#include "BaseFileInfoConfigurationWidget.h"
#include "Debug.h"
#include "XmlOptions.h"

#include <QLabel>
#include <QLayout>

//_______________________________________________________
BaseFileInfoConfigurationWidget::BaseFileInfoConfigurationWidget( QWidget* parent ):
    QWidget( parent ),
    OptionWidget( "TOOLTIPS_MASK" )
{

    Debug::Throw( "BaseFileInfoConfigurationWidget::BaseFileInfoConfigurationWidget.\n" );

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->setMargin(0);
    vLayout->setSpacing(5);
    setLayout( vLayout );

    vLayout->addWidget( new QLabel( tr( "Select the information to be displayed:" ), this ) );

    _addCheckBox( BaseFileInfoToolTipWidget::Size, tr( "Size" ) );
    _addCheckBox( BaseFileInfoToolTipWidget::Modified, tr( "Modified" ) );
    _addCheckBox( BaseFileInfoToolTipWidget::User, tr( "Owner" ) );
    _addCheckBox( BaseFileInfoToolTipWidget::Group, tr( "Group" ) );
    _addCheckBox( BaseFileInfoToolTipWidget::Permissions, tr( "Permissions" ) );

}

//_______________________________________________________
void BaseFileInfoConfigurationWidget::read( const Options& options )
{
    const int mask( options.contains( optionName() ) ? options.get<int>( optionName() ) : BaseFileInfoToolTipWidget::Default );
    for( auto&& iter = checkboxes_.constBegin(); iter != checkboxes_.constEnd(); ++iter )
    {
        iter.value()->setChecked( mask&iter.key() );
        if( !_connected() )
        { connect( iter.value(), &QAbstractButton::toggled, this, &BaseFileInfoConfigurationWidget::modified ); }
    }

    _setConnected();

}

//_______________________________________________________
void BaseFileInfoConfigurationWidget::write( Options& options ) const
{
    int mask(0);
    for( auto&& iter = checkboxes_.constBegin(); iter != checkboxes_.constEnd(); ++iter )
    { if( iter.value()->isChecked() ) mask |= iter.key(); }

    options.set<int>( optionName(), mask );
}

//_______________________________________________________
void BaseFileInfoConfigurationWidget::_addCheckBox( BaseFileInfoToolTipWidget::Type type, const QString& value )
{
    Debug::Throw( "BaseFileInfoConfigurationWidget::_addCheckBox.\n" );
    QCheckBox* checkbox;
    layout()->addWidget( checkbox = new QCheckBox( value, this ) );
    checkboxes_.insert( type, checkbox );

    if( _connected() )
    { connect( checkbox, &QAbstractButton::toggled, this, &BaseFileInfoConfigurationWidget::modified ); }

}
