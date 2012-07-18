#ifndef OptionSpinBox_h
#define OptionSpinBox_h

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
#include "OptionWidget.h"
#include "XmlOptions.h"

#include <QtGui/QSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <cmath>

//! QSpinBox associated to an option for configuration dialogs
class OptionSpinBox: public QWidget, public OptionWidget
{

    public:

    //! constructor
    OptionSpinBox( QWidget* parent, const QString& optionName, bool addStretch = true ):
        QWidget( parent ),
        OptionWidget( optionName ),
        spinBox_( 0 ),
        scale_( 1 )
    {
        QHBoxLayout *layout = new QHBoxLayout();
        layout->setMargin(0);
        setLayout( layout );
        layout->addWidget( spinBox_ = new QSpinBox( this ) );
        if( addStretch ) layout->addStretch( 1 );
        _setBuddy( this );
    }

    //! scale (i.e. option = value()/scale)
    void setScale( const double& scale )
    { scale_ = scale; }

    //! read value from option
    void read( void )
    { setValue( static_cast<int>(round(scale_*XmlOptions::get().get<double>( optionName() )))); }

    //! write value to option
    void write( void ) const
    { XmlOptions::get().set<double>( optionName(), static_cast<double>(value())/scale_ ); }

    //! unit
    void setSuffix( const QString& value )
    { spinBox_->setSuffix( value ); }

    //!@name wrappers
    //@{

    void setValue( int value )
    { spinBox_->setValue( value ); }

    int value( void ) const
    { return spinBox_->value(); }

    void setMinimum( int value )
    { spinBox_->setMinimum( value ); }

    void setMaximum( int value )
    { spinBox_->setMaximum( value ); }

    void setToolTip( const QString& value )
    { spinBox_->setToolTip( value ); }

    //@}

    private:

    //! spinbox
    QSpinBox* spinBox_;

    //! scale factor (default is 1)
    double scale_;

};
#endif
