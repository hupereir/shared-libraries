#ifndef Key_h
#define Key_h

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

#include <QTextStream>
#include <QSet>
#include <algorithm>

//* base namespace
namespace Base
{

    class Key
    {

        public:

        //* constructor
        explicit Key():
            key_( _counter()++ )
        {}

        //* \brief copy constructor
        /**
        copy constructor.
        The set of associated keys is copied as is.
        The new key gets a new unique id. It gets associated to all objects
        in the list
        */
        explicit Key( const Key& key ):
            key_( _counter()++ ),
            associatedKeys_( key.associatedKeys_ )
        {
            for( const auto& associate:associatedKeys_ )
            { associate->_associate( this ); }
        }

        //* destructor
        inline virtual ~Key() = 0;

        //* equal to operator
        bool operator == (const Key& key ) const
        { return this->key() == key.key(); }

        //* lower than operator
        bool operator < (const Key& key ) const
        { return this->key() < key.key(); }

        //* shortcut for key unique id
        using Type = quint32;

        //* retrieve key
        Type key() const
        { return key_; }

        //* shortcut for set of Key
        using Set = QSet< Key* >;

        //* retrieve all associated keys
        const Set& getAssociated() const
        { return associatedKeys_; }

        //* clear associations for this key
        void clearAssociations()
        {
            for( const auto& key:associatedKeys_ )
            { key->_disassociate( this ); }
            associatedKeys_.clear();
        }

        //* clear associations of a given type for this key
        template<typename T> inline void clearAssociations();

        //* associated two Keys
        static void associate( Key* first, Key* second )
        {
            Q_ASSERT( first->key() != second->key() );
            first->_associate( second );
            second->_associate( first );
        }

        //* associated two Keys
        static void associate( Key& first, Key& second )
        {
            Q_ASSERT( first.key() != second.key() );
            first._associate( &second );
            second._associate( &first );
        }

        //* used to find keys of matching id
        class SameKeyFTor
        {

            public:

            //* constructor
            explicit SameKeyFTor( Type key ):
                key_( key )
            {}

            //* predicate
            bool operator() (const Key* key ) const
            { return key->key() == key_; }

            private:

            //* predicted key
            Type key_;

        };

        //* used to find keys of matching id
        class IsAssociatedFTor
        {

            public:

            //* constructor
            explicit IsAssociatedFTor( const Key* key ):
                key_( key )
            {}

            //* predicate
            bool operator() (const Key* key ) const
            { return key_->isAssociated( key ); }

            private:

            //* predicted key
            const Key* key_;

        };


        //* return true if keys are associated
        bool isAssociated( const Key* key ) const
        {
            return std::find_if(
                associatedKeys_.begin(),
                associatedKeys_.end(),
                SameKeyFTor( key->key() ) ) != associatedKeys_.end();
        }

        //* disassociate two Keys
        static void disassociate( Key* first, Key* second )
        {
            first->_disassociate( second );
            second->_disassociate( first );
        }

        //* disassociated two Keys
        static void disassociate( Key& first, Key& second )
        {
            first._disassociate( &second );
            second._disassociate( &first );
        }

        private:

        //* add a key to associates
        void _associate( Key* key )
        { associatedKeys_.insert( key ); }

        //* remove a key from associates
        void _disassociate( Key* key )
        { associatedKeys_.remove( key ); }

        //* unique id
        Type key_;

        //* associated keys
        Set associatedKeys_;

        //* unique id counter
        static Type& _counter();

        //* to dump Key and associations
        friend QTextStream &operator << (QTextStream &out,const Key &key)
        {

            //* dump this key uid
            out << "key=" << key.key();

            // dump associated key uid
            if( key.associatedKeys_.size() )
            {
                out << " associations:";

                for( const auto& associate:key.associatedKeys_ )
                { out << " " << associate->key(); }

            }

            out << endl;
            return out;
        }

    };

    /** \brief
    templatized unsorted set of casted keys.
    Is constructed from a pointer to a Key;
    Contains all objects of type T associated to the Key.
    */
    template<typename T>
    class KeySet final
    {

        private:

        QSet<T*> set_;

        public:

        //* default constructor
        explicit KeySet()
        {}

        /**
        \brief constructor
        fill the set with all objects of type T associated with the key
        */
        explicit KeySet( const Key* key )
        {

            for( const auto& associate:key->getAssociated() )
            {
                auto t( dynamic_cast<T*>( associate ) );
                if(t) set_.insert(t);
            }

        }

        /**
        \brief constructor
        fill the set with all objects of type T associated with the key
        */
        explicit KeySet( const Key& key )
        {

            for( const auto& associate:key.getAssociated() )
            {
                auto t( dynamic_cast<T*>( associate ) );
                if(t) set_.insert(t);
            }

        }

        //*@name accessors
        //@{

        using iterator = typename QSet<T*>::iterator;
        using const_iterator = typename QSet<T*>::const_iterator;

        const_iterator begin() const { return set_.begin(); }
        const_iterator end() const { return set_.end(); }

        iterator begin() { return set_.begin(); }
        iterator end() { return set_.end(); }

        const QSet<T*>& get() const { return set_; }

        int size() const { return set_.size(); }
        bool empty() const { return set_.empty(); }

        QList<T*> toList() const { return set_.toList(); }

        //@}

        //*@name modifiers
        //@{

        //* mutable accessor
        QSet<T*>& get() { return set_; }

        //* insert
        /** needs decltype because return type ofr set_.insert has changed with qt version */
        auto insert( T* t ) -> decltype(set_.insert( t ))
        { return set_.insert( t ); }

        //* remove
        bool remove( T* t ) { return set_.remove( t ); }

        //* find
        iterator find( T* t ) { return set_.find( t ); }

        //* unite too keysets
        KeySet& unite( const KeySet& other )
        {
            set_.unite( other.set_ );
            return *this;
        }

        //@}

    };

    template<typename T>
    using KeySetIterator = QSetIterator<T*>;

};

//______________________________________________________________
Base::Key::~Key() { clearAssociations(); }

//______________________________________________________________
template<typename T> void Base::Key::clearAssociations()
{
    for( const auto& key:KeySet<T>(this) )
    {
        key->_disassociate( this );
        _disassociate( key );
    }
}

//____________________________________________________
inline uint qHash( const Base::Key*& key )
{ return key->key(); }

#endif
