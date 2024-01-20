/*
 * Copyright (c) 2024 Eduardo Nuno Almeida
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Eduardo Nuno Almeida <enmsa@outlook.pt>
 *
 * This class is inspired by the original container classes in ns-3.
 * Thanks to their authors.
 */

#ifndef OBJECT_CONTAINER_H
#define OBJECT_CONTAINER_H

#include "names.h"
#include "ptr.h"

#include <string>
#include <vector>

namespace ns3
{

/**
 * @brief Template class to group a set of object pointers.
 * @tparam T Typename of the container items.
 */
template <class T>
class ObjectContainer
{
  public:
    /// Iterator to the list of object pointers
    using Iterator = typename std::vector<Ptr<T>>::const_iterator;

    /**
     * Create an empty ObjectContainer.
     */
    ObjectContainer() = default;

    /**
     * Create a Container with exactly one object that has been previously instantiated.
     * The single object is specified by a smart pointer Ptr<T>.
     *
     * @param ptr The object to add to the container
     */
    ObjectContainer(Ptr<T> ptr);

    /**
     * Create an ObjectContainer with exactly one object which has been previously instantiated and
     * assigned a name using the Object name service.
     * This object is specified by its assigned name.
     *
     * @param name The name of the device to add to the container
     */
    ObjectContainer(std::string name);

    /**
     * Create an object container, which is a concatenation of multiple input ObjectContainers.
     *
     * @tparam Ts @deduced Template type parameter pack for the multiple ObjectContainers
     * @param container The first ObjectContainer.
     * @param args The remaining ObjectContainers.
     *
     * @note A frequently seen idiom that uses these constructors involves the implicit conversion
     * by constructor of Ptr<T>. When used, Ptr<T> will be passed to this constructor instead of
     * ObjectContainer<T>&. C++ will notice the implicit conversion path that goes through the
     * ObjectContainer(Ptr<T> container) constructor above.  Using this conversion one may
     * optionally provide arguments of Ptr<T> to these constructors.
     */
    template <typename... Ts>
    ObjectContainer(const ObjectContainer<T>& container, Ts&&... args);

    /**
     * @brief Append a single object to this container
     * @param ptr The object to append
     */
    void Add(Ptr<T> ptr);

    /**
     * @brief Append to this container the single object referred to
     * via its object name service registered name.
     *
     * @param name The name of the object to add to the container.
     */
    void Add(std::string name);

    /**
     * @brief Append the contents of another ObjectContainer to the end of this container.
     * @param container The ObjectContainer to append
     */
    void Add(const ObjectContainer<T>& container);

    /**
     * Append the contents of another ObjectContainer to the end of this container.
     *
     * @tparam Ts @deduced Template type parameter pack for the multiple ObjectContainer
     * @param container The ObjectContainer to append
     * @param args The remaining ObjectContainers to append
     */
    template <typename... Ts>
    void Add(const ObjectContainer<T>& container, Ts&&... args);

    /**
     * @brief Create n objects and append pointers to them to the end of this ObjectContainer.
     *
     * Objects are at the heart of any ns-3 simulation.  One of the first tasks that
     * any simulation needs to do is to create a number of objects.  This method
     * automates that task.
     *
     * @param n The number of objects to create
     */
    void Create(uint32_t n);

    /**
     * @brief Removes all elements in the container.
     */
    void Clear();

    /**
     * @brief Get an iterator which refers to the first object in the container.
     *
     * Objects can be retrieved from the container in two ways.
     * First, directly by an index into the container; and second, using an iterator.
     * This method is used in the iterator method and is typically used in a
     * for-loop to run through the objects.
     *
     * @code
     *   for (auto i = container.Begin(); i != container.End(); i++)
     *   {
     *       (*i)->method(); // some method
     *   }
     * @endcode
     *
     * @return An iterator referring to the first object in the container
     */
    Iterator Begin() const;

    /**
     * @brief Get an iterator that indicates the past-the-last object in the container.
     *
     * Objects can be retrieved from the container in two ways. First,
     * directly by an index into the container; and second, using an iterator.
     * This method is used in the iterator method and is typically used in a
     * for-loop to run through the objects.
     *
     * @code
     *   for (auto i = container.Begin(); i != container.End(); i++)
     *   {
     *       (*i)->method(); // some method
     *   }
     * @endcode
     *
     * @return An iterator indicating an ending condition for a loop
     */
    Iterator End() const;

    /**
     * @brief Get the number of objects stored in this container.
     *
     * Objects can be retrieved from the container in two ways.
     * First, directly by an index into the container, and second, using an iterator.
     * This method is used in the direct method and is typically used to
     * define an ending condition in a for-loop that runs through the stored
     * Nodes
     *
     * @code
     *   for (uint32_t i = 0; i < container.GetN(); i++)
     *   {
     *       auto p = container.Get(i);
     *       i->method(); // some Node method
     *   }
     * @endcode
     *
     * @return The number of objects stored in this container
     */
    uint32_t GetN() const;

    /**
     * @brief Get the object stored in this container at a given index.
     *
     * Nodes can be retrieved from the container in two ways.
     * First, directly by an index into the container; and second, using an iterator.
     * This method is used in the direct method and is used to retrieve the
     * indexed object.
     *
     * @code
     *   for (uint32_t i = 0; i < container.GetN(); ++i)
     *   {
     *       auto p = container.Get(i);
     *       i->method(); // some Node method
     *   }
     * @endcode
     *
     * @param i The index of the requested pointer
     * @return The requested pointer
     *
     * @{
     */
    Ptr<T> Get(uint32_t i) const;
    Ptr<T> operator[](uint32_t i) const;
    /** @} */

    /**
     * @brief Retrieve a vector of all the Applications associated to this Node.
     * @returns a vector of all the Applications associated to this Node.
     */
    const std::vector<Ptr<T>>& GetAllItems() const;

    /**
     * @brief Return true if the container contains a given item.
     *
     * @param item Item to search.
     * @return Whether the container contains the given item.
     */
    bool Contains(const T& item) const;

  protected:
    std::vector<Ptr<T>> m_objects; //!< Object pointers
};

///////////////////////////////////////////////////////////
// Implementation of the templates declared above
///////////////////////////////////////////////////////////

template <class T>
ObjectContainer<T>::ObjectContainer(Ptr<T> ptr)
{
    Add(ptr);
}

template <class T>
ObjectContainer<T>::ObjectContainer(std::string name)
{
    Add(name);
}

template <class T>
template <typename... Ts>
ObjectContainer<T>::ObjectContainer(const ObjectContainer<T>& container, Ts&&... args)
{
    static_assert(std::conjunction_v<std::is_convertible<Ts, ObjectContainer<T>>...>,
                  "Variable types are not convertible to ObjectContainer<T>");

    Add(container, std::forward<Ts>(args)...);
}

template <class T>
void
ObjectContainer<T>::Add(Ptr<T> ptr)
{
    m_objects.emplace_back(ptr);
}

template <class T>
void
ObjectContainer<T>::Add(std::string name)
{
    Ptr<T> ptr = Names::Find<T>(name);
    Add(ptr);
}

template <class T>
void
ObjectContainer<T>::Add(const ObjectContainer<T>& container)
{
    for (Iterator i = container.Begin(); i != container.End(); i++)
    {
        Add(*i);
    }
}

template <class T>
template <typename... Ts>
void
ObjectContainer<T>::Add(const ObjectContainer<T>& container, Ts&&... args)
{
    static_assert(std::conjunction_v<std::is_convertible<Ts, ObjectContainer<T>>...>,
                  "Variable types are not convertible to ObjectContainer<T>");

    Add(container);
    Add(std::forward<Ts>(args)...);
}

template <class T>
void
ObjectContainer<T>::Create(uint32_t n)
{
    m_objects.reserve(n);

    for (uint32_t i = 0; i < n; i++)
    {
        Add(CreateObject<T>());
    }
}

template <class T>
void
ObjectContainer<T>::Clear()
{
    m_objects.clear();
}

template <class T>
typename ObjectContainer<T>::Iterator
ObjectContainer<T>::Begin() const
{
    return m_objects.begin();
}

template <class T>
typename ObjectContainer<T>::Iterator
ObjectContainer<T>::End() const
{
    return m_objects.end();
}

template <class T>
uint32_t
ObjectContainer<T>::GetN() const
{
    return m_objects.size();
}

template <class T>
Ptr<T>
ObjectContainer<T>::Get(uint32_t i) const
{
    return m_objects.at(i);
}

template <class T>
Ptr<T>
ObjectContainer<T>::operator[](uint32_t i) const
{
    return m_objects[i];
}

template <class T>
const std::vector<Ptr<T>>&
ObjectContainer<T>::GetAllItems() const
{
    return m_objects;
}

template <class T>
bool
ObjectContainer<T>::Contains(const T& item) const
{
    for (const auto& i : m_objects)
    {
        if (i == item)
        {
            return true;
        }
    }
    return false;
}

} // namespace ns3

#endif // OBJECT_CONTAINER_H
