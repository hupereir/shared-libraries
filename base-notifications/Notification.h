#ifndef Notification_h
#define Notification_h

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

#include "base_notifications_export.h"
#include <QIcon>
#include <QString>
#include <QStringList>

class BASE_NOTIFICATIONS_EXPORT Notification
{

    public:

    //* default constructor
    explicit Notification( const QString& summary = QString(), const QString& body = QString() ):
        summary_( summary ),
        body_( body )
    {}

    //* flags
    enum Flag
    {
        None = 0,
        Transient = 1<< 0
    };

    Q_DECLARE_FLAGS( Flags, Flag );

    //*@name accessors
    //@{

    const QString& applicationName() const
    { return applicationName_; }

    const QIcon& icon() const
    { return icon_; }

    const QString& category() const
    { return category_; }

    const QString& summary() const
    { return summary_; }

    const QString& body() const
    { return body_; }

    const QStringList& actionList() const
    { return actionList_; }

    //* flags
    Flags flags() const
    { return flags_; }

    //* true if valid
    bool isValid() const
    { return !body_.isEmpty(); }

    //* return true if two notifications can be merged
    bool canMerge( const Notification& other ) const
    {
        return
            applicationName_ == other.applicationName_ &&
            icon_.isNull() && other.icon_.isNull() &&
            summary_ == other.summary_ &&
            actionList_ == other.actionList_;
    }

    //@}

    //*@name modifiers
    //@{

    void setApplicationName( const QString& value )
    { applicationName_ = value; }

    void setIcon( const QIcon& value )
    { icon_ = value; }

    void setCategory( const QString& value )
    { category_ = value; }

    void setSummary( const QString& value )
    { summary_ = value; }

    void setBody( const QString& value )
    { body_ = value; }

    void setActionList( const QStringList& value )
    { actionList_ = value; }

    //* merge two notifications
    void merge( const Notification& other )
    { body_+= QStringLiteral( "\n" ) + other.body_; }

    //* flags
    void setFlag( Flag flag, bool value = true )
    {
        if( value ) flags_ |= flag;
        else flags_ &= ~flag;
    }

    //@}

    private:

    //* application name
    QString applicationName_;

    //* icon
    QIcon icon_;

    //* category
    QString category_;

    //* summary
    QString summary_;

    //* text
    QString body_;

    //* actiosn
    QStringList actionList_;

    //* flags
    Flags flags_ = None;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( Notification::Flags );

#endif
