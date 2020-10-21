#ifndef MessageWidget_h
#define MessageWidget_h

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

#include "base_qt_export.h"
#include "Counter.h"

#include <QIcon>
#include <QLabel>
#include <QLayout>
#include <QPaintEvent>
#include <QPushButton>
#include <QString>
#include <QToolButton>
#include <QWidget>

#include <memory>

class MessageWidgetPrivate;

//* QDialog used to ask if a new file should be created
class BASE_QT_EXPORT MessageWidget: public QWidget, private Base::Counter<MessageWidget>
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
    explicit MessageWidget( QWidget* = nullptr, MessageType = MessageType::Information, const QString& = QString() );

    explicit MessageWidget( QWidget* parent, const QString& text ):
        MessageWidget( parent, MessageType::Information, text )
    {}

    //* destructor
    ~MessageWidget() override;

    //*@name accessors
    //@{

    //* message type
    MessageType messageType() const;

    //* text
    QString text() const;

    //@}

    //*@name modifiers
    //@{

    //* set message type
    void setMessageType( MessageType );

    //* icon
    void setIcon( const QIcon& );

    //* text
    void setText( const QString& );

    //* buttons
    QToolButton* addDefaultCloseButton();

    //* buttons
    QPushButton* addButton( const QIcon& = QIcon(), const QString& = QString() );

    //* buttons
    QPushButton* addButton( const QString& value )
    { return addButton( QIcon(), value ); }

    //* clear buttons
    void clearButtons();

    //* buttons
    using ButtonList = QList<QPushButton*>;
    ButtonList addButtons( const QStringList& );

    //* buttons
    ButtonList setButtons( const QStringList& );

    //* direction
    void setDirection( QBoxLayout::Direction );

    //* set duration
    void setDuration( int value );

    //* show animated
    void animatedShow();

    //* show animated
    void animatedHide();

    //@}

    Q_SIGNALS:

    //* animatedShow finished
    void showAnimationFinished();

    //* animatedHide finished
    void hideAnimationFinished();

    protected:

    //* paint
    void paintEvent( QPaintEvent* ) override;

    private:

    //* private
    std::unique_ptr<MessageWidgetPrivate> private_;


};

#endif
