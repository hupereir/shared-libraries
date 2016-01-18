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

#ifndef QOrderedSet_h
#define QOrderedSet_h

#include <QMap>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

struct QOrderedSetDummyValue
{};

template <class T>
class QOrderedSet
{
    using Map = QMap<T, QOrderedSetDummyValue>;

    public:

    inline QOrderedSet( void )
    {}

    inline QOrderedSet(const QOrderedSet<T> &other) :
        q_map(other.q_map)
        {}

    inline QOrderedSet<T> &operator=(const QOrderedSet<T> &other)
    { q_map = other.q_map; return *this; }

    #ifdef Q_COMPILER_RVALUE_REFS
    inline QOrderedSet<T> &operator=(QOrderedSet<T> &&other)
    { qSwap(q_map, other.q_map); return *this; }
    #endif

    inline void swap(QOrderedSet<T> &other)
    { q_map.swap(other.q_map); }

    inline bool operator==(const QOrderedSet<T> &other) const
    { return q_map == other.q_map; }

    inline bool operator!=(const QOrderedSet<T> &other) const
    { return q_map != other.q_map; }

    inline int size() const
    { return q_map.size(); }

    inline bool isEmpty() const
    { return q_map.isEmpty(); }

    inline int capacity() const
    { return q_map.capacity(); }

    inline void reserve(int size);

    inline void squeeze()
    { q_map.squeeze(); }

    inline void detach()
    { q_map.detach(); }

    inline bool isDetached() const
    { return q_map.isDetached(); }

    inline void setSharable(bool sharable)
    { q_map.setSharable(sharable); }

    inline void clear()
    { q_map.clear(); }

    inline bool remove(const T &value)
    { return q_map.remove(value) != 0; }

    inline bool contains(const T &value) const
    { return q_map.contains(value); }

    bool contains(const QOrderedSet<T> &set) const;

    class const_iterator;

    class iterator
    {
        using Map = QMap<T, QOrderedSetDummyValue>;
        typename Map::iterator i;
        friend class const_iterator;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = qptrdiff;
        using value_type = T;
        using pointer = const T *;
        using reference = const T &;

        inline iterator()
        {}

        inline iterator(typename Map::iterator o) : i(o)
        {}

        inline iterator(const iterator &o) : i(o.i)
        {}

        inline iterator &operator=(const iterator &o)
        { i = o.i; return *this; }

        inline const T &operator*() const
        { return i.key(); }

        inline const T *operator->() const
        { return &i.key(); }

        inline bool operator==(const iterator &o) const
        { return i == o.i; }

        inline bool operator!=(const iterator &o) const
        { return i != o.i; }

        inline bool operator==(const const_iterator &o) const
        { return i == o.i; }

        inline bool operator!=(const const_iterator &o) const
        { return i != o.i; }

        inline iterator &operator++()
        { ++i; return *this; }

        inline iterator operator++(int)
        { iterator r = *this; ++i; return r; }

        inline iterator &operator--()
        { --i; return *this; }

        inline iterator operator--(int)
        { iterator r = *this; --i; return r; }

        inline iterator operator+(int j) const
        { return i + j; }

        inline iterator operator-(int j) const
        { return i - j; }

        inline iterator &operator+=(int j)
        { i += j; return *this; }

        inline iterator &operator-=(int j)
        { i -= j; return *this; }

    };

    class const_iterator
    {
        using Map = QMap<T, QOrderedSetDummyValue>;
        typename Map::const_iterator i;
        friend class iterator;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = qptrdiff;
        using value_type = T;
        using pointer = const T *;
        using reference = const T &;

        inline const_iterator() {}
        inline const_iterator(typename Map::const_iterator o):
            i(o)
        {}
        inline const_iterator(const const_iterator &o):
            i(o.i)
        {}

        inline const_iterator(const iterator &o):
            i(o.i)
            {}

        inline const_iterator &operator=(const const_iterator &o)
        { i = o.i; return *this; }

        inline const T &operator*() const
        { return i.key(); }

        inline const T *operator->() const
        { return &i.key(); }

        inline bool operator==(const const_iterator &o) const
        { return i == o.i; }

        inline bool operator!=(const const_iterator &o) const
        { return i != o.i; }

        inline const_iterator &operator++()
        { ++i; return *this; }

        inline const_iterator operator++(int)
        { const_iterator r = *this; ++i; return r; }

        inline const_iterator &operator--()
        { --i; return *this; }

        inline const_iterator operator--(int)
        { const_iterator r = *this; --i; return r; }

        inline const_iterator operator+(int j) const
        { return i + j; }

        inline const_iterator operator-(int j) const
        { return i - j; }

        inline const_iterator &operator+=(int j)
        { i += j; return *this; }

        inline const_iterator &operator-=(int j)
        { i -= j; return *this; }
    };

    // STL style
    inline iterator begin()
    { return q_map.begin(); }

    inline const_iterator begin() const
    { return q_map.begin(); }

    inline const_iterator constBegin() const
    { return q_map.constBegin(); }

    inline iterator end()
    { return q_map.end(); }

    inline const_iterator end() const
    { return q_map.end(); }

    inline const_iterator constEnd() const
    { return q_map.constEnd(); }

    iterator erase(iterator i)
    { return q_map.erase(reinterpret_cast<typename Map::iterator &>(i)); }

    // more Qt
    using Iterator = iterator;
    using ConstIterator = const_iterator;

    inline int count() const
    { return q_map.count(); }

    inline const_iterator insert(const T &value) // ### Qt 5: should return an 'iterator'
    { return static_cast<typename Map::const_iterator>(q_map.insert(value, QOrderedSetDummyValue())); }

    iterator find(const T &value)
    { return q_map.find(value); }

    const_iterator find(const T &value) const
    { return q_map.find(value); }

    inline const_iterator constFind(const T &value) const
    { return find(value); }

    QOrderedSet<T> &unite(const QOrderedSet<T> &other);

    QOrderedSet<T> &intersect(const QOrderedSet<T> &other);

    QOrderedSet<T> &subtract(const QOrderedSet<T> &other);

    // STL compatibility
    using key_type = T;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using difference_type = qptrdiff;
    using size_type = int;

    inline bool empty() const
    { return isEmpty(); }

    // comfort
    inline QOrderedSet<T> &operator<<(const T &value)
    { insert(value); return *this; }

    inline QOrderedSet<T> &operator|=(const QOrderedSet<T> &other)
    { unite(other); return *this; }

    inline QOrderedSet<T> &operator|=(const T &value)
    { insert(value); return *this; }

    inline QOrderedSet<T> &operator&=(const QOrderedSet<T> &other)
    { intersect(other); return *this; }

    inline QOrderedSet<T> &operator&=(const T &value)
    { QOrderedSet<T> result; if (contains(value)) result.insert(value); return (*this = result); }

    inline QOrderedSet<T> &operator+=(const QOrderedSet<T> &other)
    { unite(other); return *this; }

    inline QOrderedSet<T> &operator+=(const T &value)
    { insert(value); return *this; }

    inline QOrderedSet<T> &operator-=(const QOrderedSet<T> &other)
    { subtract(other); return *this; }

    inline QOrderedSet<T> &operator-=(const T &value)
    { remove(value); return *this; }

    inline QOrderedSet<T> operator|(const QOrderedSet<T> &other) const
    { QOrderedSet<T> result = *this; result |= other; return result; }

    inline QOrderedSet<T> operator&(const QOrderedSet<T> &other) const
    { QOrderedSet<T> result = *this; result &= other; return result; }

    inline QOrderedSet<T> operator+(const QOrderedSet<T> &other) const
    { QOrderedSet<T> result = *this; result += other; return result; }

    inline QOrderedSet<T> operator-(const QOrderedSet<T> &other) const
    { QOrderedSet<T> result = *this; result -= other; return result; }

    #if QT_VERSION < 0x050000
    // ### Qt 5: remove
    inline QOrderedSet<T> operator|(const QOrderedSet<T> &other)
    { QOrderedSet<T> result = *this; result |= other; return result; }

    inline QOrderedSet<T> operator&(const QOrderedSet<T> &other)
    { QOrderedSet<T> result = *this; result &= other; return result; }

    inline QOrderedSet<T> operator+(const QOrderedSet<T> &other)
    { QOrderedSet<T> result = *this; result += other; return result; }

    inline QOrderedSet<T> operator-(const QOrderedSet<T> &other)
    { QOrderedSet<T> result = *this; result -= other; return result; }
#endif

    QList<T> toList() const;
    inline QList<T> values() const { return toList(); }

    static QOrderedSet<T> fromList(const QList<T> &list);

private:
    Map q_map;
};

template <class T>
Q_INLINE_TEMPLATE void QOrderedSet<T>::reserve(int asize) { q_map.reserve(asize); }

template <class T>
Q_INLINE_TEMPLATE QOrderedSet<T> &QOrderedSet<T>::unite(const QOrderedSet<T> &other)
{
    QOrderedSet<T> copy(other);
    typename QOrderedSet<T>::const_iterator i = copy.constEnd();
    while (i != copy.constBegin()) {
        --i;
        insert(*i);
    }
    return *this;
}

template <class T>
Q_INLINE_TEMPLATE QOrderedSet<T> &QOrderedSet<T>::intersect(const QOrderedSet<T> &other)
{
    QOrderedSet<T> copy1(*this);
    QOrderedSet<T> copy2(other);
    typename QOrderedSet<T>::const_iterator i = copy1.constEnd();
    while (i != copy1.constBegin()) {
        --i;
        if (!copy2.contains(*i))
            remove(*i);
    }
    return *this;
}

template <class T>
Q_INLINE_TEMPLATE QOrderedSet<T> &QOrderedSet<T>::subtract(const QOrderedSet<T> &other)
{
    QOrderedSet<T> copy1(*this);
    QOrderedSet<T> copy2(other);
    typename QOrderedSet<T>::const_iterator i = copy1.constEnd();
    while (i != copy1.constBegin()) {
        --i;
        if (copy2.contains(*i))
            remove(*i);
    }
    return *this;
}

template <class T>
Q_INLINE_TEMPLATE bool QOrderedSet<T>::contains(const QOrderedSet<T> &other) const
{
    typename QOrderedSet<T>::const_iterator i = other.constBegin();
    while (i != other.constEnd()) {
        if (!contains(*i))
            return false;
        ++i;
    }
    return true;
}

template <typename T>
Q_OUTOFLINE_TEMPLATE QList<T> QOrderedSet<T>::toList() const
{
    QList<T> result;
    result.reserve(size());
    typename QOrderedSet<T>::const_iterator i = constBegin();
    while (i != constEnd()) {
        result.append(*i);
        ++i;
    }
    return result;
}

template <typename T>
QOrderedSet<T> QOrderedSet<T>::fromList(const QList<T> &list)
{
    QOrderedSet<T> out;
    foreach( const T& t, list )
    { out.insert( t ); }

    return out;
}

Q_DECLARE_SEQUENTIAL_ITERATOR(OrderedSet)

template <typename T>
class QMutableOrderedSetIterator
{
    using iterator = typename QOrderedSet<T>::iterator;
    QOrderedSet<T> *c;
    iterator i, n;
    inline bool item_exists() const { return c->constEnd() != n; }

public:
    inline QMutableOrderedSetIterator(QOrderedSet<T> &container)
        : c(&container)
    { c->setSharable(false); i = c->begin(); n = c->end(); }
    inline ~QMutableOrderedSetIterator()
    { c->setSharable(true); }
    inline QMutableOrderedSetIterator &operator=(QOrderedSet<T> &container)
    { c->setSharable(true); c = &container; c->setSharable(false);
      i = c->begin(); n = c->end(); return *this; }
    inline void toFront() { i = c->begin(); n = c->end(); }
    inline void toBack() { i = c->end(); n = i; }
    inline bool hasNext() const { return c->constEnd() != i; }
    inline const T &next() { n = i++; return *n; }
    inline const T &peekNext() const { return *i; }
    inline bool hasPrevious() const { return c->constBegin() != i; }
    inline const T &previous() { n = --i; return *n; }
    inline const T &peekPrevious() const { iterator p = i; return *--p; }
    inline void remove()
    { if (c->constEnd() != n) { i = c->erase(n); n = c->end(); } }
    inline const T &value() const { Q_ASSERT(item_exists()); return *n; }
    inline bool findNext(const T &t)
    { while (c->constEnd() != (n = i)) if (*i++ == t) return true; return false; }
    inline bool findPrevious(const T &t)
    { while (c->constBegin() != i) if (*(n = --i) == t) return true;
      n = c->end(); return false;  }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSET_H
