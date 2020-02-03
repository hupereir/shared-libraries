#ifndef OptionSpinBox_h
#define OptionSpinBox_h

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

#include "OptionWidget.h"

#include <QSpinBox>
#include <QLabel>
#include <QLayout>
#include <cmath>

//* QSpinBox associated to an option for configuration dialogs
class OptionSpinBox: public QWidget, public OptionWidget
{

    Q_OBJECT

    public:

    //* constructor
    explicit OptionSpinBox( QWidget* parent, const QString& optionName, bool addStretch = true ):
        QWidget( parent ),
        OptionWidget( optionName )
    {
        auto layout = new QHBoxLayout();
        layout->setMargin(0);
        setLayout( layout );
        layout->addWidget( spinBox_ = new QSpinBox( this ) );
        if( addStretch ) layout->addStretch( 1 );
    }

    //* scale (i.e. option = value()/scale)
    void setScale( const double& scale )
    { scale_ = scale; }

    //* read value from option
    void read( const Options& options ) override
    {
        setValue( static_cast<int>(round(scale_*options.get<double>( optionName() ))));
        if( !_connected() )
        {
            connect(spinBox_, SIGNAL(valueChanged(int)), SIGNAL(modified()));
            _setConnected();
        }
    }

    //* write value to option
    void write( Options& options ) const override
    { options.set<double>( optionName(), static_cast<double>(value())/scale_ ); }

    //* special value text
    void setSpecialValueText( const QString& value )
    { spinBox_->setSpecialValueText( value ); }

    //* unit
    void setSuffix( const QString& value )
    { spinBox_->setSuffix( value ); }

    //*@name wrappers
    //@{

    void setValue( int value )
    { spinBox_->setValue( value ); }

    int value() const
    { return spinBox_->value(); }

    void setMinimum( int value )
    { spinBox_->setMinimum( value ); }

    void setMaximum( int value )
    { spinBox_->setMaximum( value ); }

    void setToolTip( const QString& value )
    { spinBox_->setToolTip( value ); }

    void setSingleStep( int value )
    { spinBox_->setSingleStep( value ); }

    //@}

    Q_SIGNALS:

    //* modified
    void modified();

    private:

    //* spinbox
    QSpinBox* spinBox_ = nullptr;

    //* scale factor (default is 1)
    double scale_ = 1;

};
#endif
