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
    _setBuddy( this );

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setMargin(0);
    vLayout->setSpacing(5);
    setLayout( vLayout );

    vLayout->addWidget( new QLabel( "Select the information to be displayed: ", this ) );

    _addCheckBox( BaseFileInfoToolTipWidget::Size, "Size" );
    _addCheckBox( BaseFileInfoToolTipWidget::Modified, "Modified" );
    _addCheckBox( BaseFileInfoToolTipWidget::User, "Owner" );
    _addCheckBox( BaseFileInfoToolTipWidget::Group, "Group" );
    _addCheckBox( BaseFileInfoToolTipWidget::Permissions, "Permissions" );

}

//_______________________________________________________
void BaseFileInfoConfigurationWidget::read( void )
{
    const unsigned int mask( XmlOptions::get().contains( optionName() ) ? XmlOptions::get().get<unsigned int>( optionName() ) : BaseFileInfoToolTipWidget::Default );
    for( CheckBoxMap::const_iterator iter = checkboxes_.constBegin(); iter != checkboxes_.constEnd(); ++iter )
    { iter.value()->setChecked( mask&iter.key() ); }
}

//_______________________________________________________
void BaseFileInfoConfigurationWidget::write( void ) const
{
    unsigned int mask(0);
    for( CheckBoxMap::const_iterator iter = checkboxes_.constBegin(); iter != checkboxes_.constEnd(); ++iter )
    { if( iter.value()->isChecked() ) mask |= iter.key(); }

    XmlOptions::get().set<unsigned int>( optionName(), mask );
}

//_______________________________________________________
void BaseFileInfoConfigurationWidget::_addCheckBox( BaseFileInfoToolTipWidget::Type type, const QString& value )
{
    Debug::Throw( "BaseFileInfoConfigurationWidget::_addCheckBox.\n" );
    QCheckBox* checkbox;
    layout()->addWidget( checkbox = new QCheckBox( value, this ) );
    checkboxes_.insert( type, checkbox );
}
