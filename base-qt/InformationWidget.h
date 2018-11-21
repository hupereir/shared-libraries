#ifndef InformationWidget_h
#define InformationWidget_h

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

#include <QIcon>
#include <QLabel>
#include <QLayout>
#include <QPaintEvent>
#include <QPushButton>
#include <QString>
#include <QWidget>

#include <memory>

class InformationWidgetPrivate;

//* QDialog used to ask if a new file should be created
class InformationWidget: public QWidget, private Base::Counter<InformationWidget>
{

    Q_OBJECT
    Q_PROPERTY( qreal height READ height WRITE setFixedHeight )

    public:

    enum class MessageType
    {
        Positive,
        Information,
        Warning,
        Error
    };

    //* constructor
    explicit InformationWidget( QWidget*, MessageType = MessageType::Information, const QString& = QString() );

    explicit InformationWidget( QWidget* parent, const QString& text ):
        InformationWidget( parent, MessageType::Information, text )
    {}

    //* destructor
    ~InformationWidget() override;

    //*@name modifiers
    //@{

    //* set message type
    void setMessageType( MessageType );

    //* icon
    void setIcon( const QIcon& );

    //* text
    void setText( const QString& );

    //* buttons
    QPushButton* addButton( const QIcon& = QIcon(), const QString& = QString() );

    //* buttons
    QPushButton* addButton( const QString& value )
    { return addButton( QIcon(), value ); }

    //* direction
    void setDirection( QBoxLayout::Direction );

    //@}

    Q_SIGNALS:

    //* animatedShow finished
    void showAnimationFinished();

    //* animatedHide finished
    void hideAnimationFinished();

    public Q_SLOTS:

    //* show animated
    void animatedShow();

    //* show animated
    void animatedHide();

    protected:

    //* paint
    void paintEvent( QPaintEvent* ) override;

    private:

    //* private
    std::unique_ptr<InformationWidgetPrivate> private_;

    #if QT_VERSION < 0x050000
    friend class InformationWidgetPrivate;
    #endif

};

#endif
