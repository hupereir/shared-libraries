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
\file CustomSlider.cpp
\brief customized QSlider associated to a LineEditor
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <QHBoxLayout>


#include "CustomSlider.h"
#include "Debug.h"
#include "InformationDialog.h"
#include "Str.h"



//_________________________________________________________
CustomSlider::CustomSlider( QWidget* parent ):
    QWidget( parent ),
    Counter( "CustomSlider" ),
    spinBoxLocked_( false ),
    sliderLocked_( false )
{
    Debug::Throw( "CustomSlider::CustomSlider.\n" );

    setLayout( new QHBoxLayout() );
    layout()->setMargin(0);
    layout()->setSpacing(5);

    layout()->addWidget( slider_ = new QSlider( Qt::Horizontal, this ) );
    layout()->addWidget( spinBox_ = new QSpinBox( this ) );

    connect( spinBox_, SIGNAL( valueChanged( int ) ),   SLOT( _updateSlider( int ) ) );
    connect( slider_, SIGNAL( valueChanged( int ) ),  SLOT( _updateSpinBox( int ) ) );

//     spinBox_->setMaximumSize( QSize(
//         spinBox_->fontMetrics().width("  -100  "),
//         spinBox_->height()) );

}

//_________________________________________________________
void CustomSlider::setValue( int value )
{
    if( value < slider_->minimum() || value > slider_->maximum() ) {
        InformationDialog( this, "invalid value" ).exec();
        return;
    }

    slider_->setValue( value );
    _updateSpinBox( value );
}

//_________________________________________________________
void CustomSlider::_updateSlider( int value )
{
    Debug::Throw( "CustomSlider::_updateSlider.\n" );

    if( sliderLocked_ )
    {
        sliderLocked_ = false;
        return;
    }

    if( value < slider_->minimum() || value > slider_->maximum() )
    {
        InformationDialog( this, "invalid value" ).exec();
        return;
    }

    spinBoxLocked_ = true;
    slider_->setValue( value );
    return;
}

//_________________________________________________________
void CustomSlider::_updateSpinBox( int value )
{

    if( spinBoxLocked_ )
    {
        spinBoxLocked_ = false;
        return;
    }

    sliderLocked_ = true;
    spinBox_->setValue( value );
    return;
}
