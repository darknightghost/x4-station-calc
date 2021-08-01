#pragma once

#include <memory>

#include <interfaces/i_initialized.h>

/**
 * @brief	Interface for factory function \c load.
 *
 * @tparam	T		Type of the class implements this interface.
 */
template<class T>
class ICloneFactoryFuncUnique : virtual protected IInitialized {
  public:
    /**
     * @brief	Constructor.
     */
    ICloneFactoryFuncUnique() : IInitialized() {}

    /**
     * @brief		Clone object.
     *
     * @return		On success, a new object is reutnred. Otherwise returns
     *				nullptr.
     */
    virtual ::std::unique_ptr<T> clone() const;

    /**
     * @brief	Destructor..
     */
    virtual ~ICloneFactoryFuncUnique() {}
};

/**
 * @brief		Clone object.
 */
template<class T>
::std::unique_ptr<T> ICloneFactoryFuncUnique<T>::clone() const
{
    ::std::unique_ptr<T> ret(new T(*(const T *)this));

    if (ret == nullptr || ! ret->initialized()) {
        return nullptr;
    } else {
        return ret;
    }
}

#define CLONE_FUNC_UNIQUE(T) friend class ICloneFactoryFuncUnique<T>;
