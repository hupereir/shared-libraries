#ifndef Slider_h
#define Slider_h


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

#include "Counter.h"
#include "Debug.h"
#include "base_qt_export.h"

#include <QSlider>
#include <QSpinBox>

//* customized QSlider associated to a QSpinBox
class BASE_QT_EXPORT Slider: public QWidget, private Base::Counter<Slider>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* creator
    explicit Slider( QWidget* parent );

    //*@name accessors
    //@{

    //* retrieves value
    int value() const
    { return slider_->value(); }

    //@}

    //*@name modifiers
    //@{

    //* suffix
    void setSuffix( const QString& value )
    { spinBox_->setSuffix( value ); }

    //* changes value
    void setValue( int value );

    //* range
    void setRange( int minimum, int maximum )
    {
        setMinimum( minimum );
        setMaximum( maximum );
    }

    //* minumum
    void setMinimum( int value )
    {
        slider().setMinimum( value );
        spinBox().setMinimum( value );
    }

    //* minumum
    void setMaximum( int value )
    {
        slider().setMaximum( value );
        spinBox().setMaximum( value );
    }

    //@}

    Q_SIGNALS:

    //* value changed
    void valueChanged( int );

    protected:

    //* retrieve QSlider
    QSlider& slider()
    { return *slider_; }

    //* retrieve QSlider
    const QSlider& slider() const
    { return *slider_; }

    //* retrieve QSpinBox
    QSpinBox& spinBox()
    { return *spinBox_; }

    private:

    //* changes QSlider value according to QSpinBox
    void _updateSlider( int );

    //* changes QSpinBox value according to QSlider
    void _updateSpinBox( int );

    //* associated slider
    QSlider *slider_ = nullptr;

    //* associated LineEdit
    QSpinBox *spinBox_ = nullptr;

    //* locks
    bool spinBoxLocked_ = false;
    bool sliderLocked_ = false;

};

#endif
