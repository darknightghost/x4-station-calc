#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>

/**
 * @brief		Generic refence manager.
 *
 * @tparam		T		Type to reference.
 * @tparam		deleter	Callback to delete the object.
 */
template<typename T>
class GenericReference {
  private:
    /**
     * @brief		Reference infomation.
     */
    struct ReferenceInfo {
        T *                        object;   ///< Object referenced to.
        uint64_t                   refCount; ///< Reference count.
        ::std::function<void(T *)> deleter;  ///< Deleter.
    };

  private:
    static QMap<T *, ::std::shared_ptr<ReferenceInfo>>
                  _references;    ///< Reference counts.
    static QMutex _referenceLock; ///< Lock of reference counts.

  private:
    ::std::shared_ptr<ReferenceInfo>
        m_referenceInfo; ///< Reference information.

  public:
    /**
     * @brief		Check if the object has been managed.
     *
     * @param[in]	p		Pointer to the object to check.
     *
     * @return		If the object has been managed, the method returns \c true,
     *				otherwise returns \c false.
     */
    static bool managed(T *p);

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	p		Pointer of object to manage.
     * @param[in]	deleter	Callback to delete the object, if the object has
     *						already been managed, this parameter is ignored.
     */
    GenericReference(
        T *p, ::std::function<void(T *)> deleter = [](T *pt) -> void {
            delete pt;
        });

    /**
     * @brief		Get pointer of referenced object.
     *
     * @return		Pointer to the referenced object.
     */
    T *get();

    /**
     * @brief		Get reference count of the referenced object.
     *
     * @return		The reference count.
     */
    uint64_t refCount();

    /**
     * @brief		Destructor.
     */
    virtual ~GenericReference();

  public:
    // Operators.
    /**
     * @brief		Operator *.
     *
     * @return		Reference to object.
     */
    T &operator*();

    /**
     * @brief		Operator ->.
     *
     * @return		Pointer to object.
     */
    T *operator->();
};

template<typename T>
QMap<T *, ::std::shared_ptr<typename GenericReference<T>::ReferenceInfo>>
    GenericReference<T>::_references; ///< Reference counts.

template<typename T>
QMutex GenericReference<T>::_referenceLock; ///< Lock of reference counts.

/**
 * @brief		Check if the object has been managed.
 */
template<typename T>
bool GenericReference<T>::managed(T *p)
{
    QMutexLocker locker(&_referenceLock);
    auto         iter = _references.find(p);
    if (iter == _references.end()) {
        return false;

    } else {
        return true;
    }
}

/**
 * @brief		Constructor.
 */
template<typename T>
GenericReference<T>::GenericReference(T *p, ::std::function<void(T *)> deleter)
{
    QMutexLocker locker(&_referenceLock);
    auto         iter = _references.find(p);
    if (iter == _references.end()) {
        // Initialize reference count.
        m_referenceInfo = ::std::shared_ptr<ReferenceInfo>(
            new ReferenceInfo({p, 1, deleter}));
        _references[p] = m_referenceInfo;

    } else {
        // Increase referencerence count.
        _references[p]->refCount += 1;
    }
}

/**
 * @brief		Get pointer of referenced object.
 */
template<typename T>
T *GenericReference<T>::get()
{
    return m_referenceInfo->object;
}

/**
 * @brief		Get reference count of the referenced object.
 */
template<typename T>
uint64_t GenericReference<T>::refCount()
{
    QMutexLocker locker(&_referenceLock);

    return m_referenceInfo->refCount;
}

/**
 * @brief		Destructor.
 */
template<typename T>
GenericReference<T>::~GenericReference()
{
    QMutexLocker locker(&_referenceLock);

    if (m_referenceInfo->refCount == 1) {
        // Delete object.
        _references.remove(m_referenceInfo->object);
        m_referenceInfo->deleter(m_referenceInfo->object);

    } else {
        // Decrease reference count.
        m_referenceInfo->refCount -= 1;
    }
}

/**
 * @brief		Operator *.
 */
template<typename T>
T &GenericReference<T>::operator*()
{
    return *m_referenceInfo->object;
}

/**
 * @brief		Operator ->.
 */
template<typename T>
T *GenericReference<T>::operator->()
{
    return m_referenceInfo->object;
}
