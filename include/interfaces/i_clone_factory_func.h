#pragma once

#include <memory>

#include <interfaces/i_initialized.h>

/**
 * @brief	Interface for factory function \c load.
 *
 * @tparam	T		Type of the class implements this interface.
 */
template<class T>
class ICloneFactoryFunc : virtual protected IInitialized {
  public:
    /**
     * @brief	Constructor.
     */
    ICloneFactoryFunc() : IInitialized() {}

    /**
     * @brief		Clone object.
     *
     * @return		On success, a new object is reutnred. Otherwise returns
     *				nullptr.
     */
    virtual ::std::shared_ptr<T> clone();

    /**
     * @brief	Destructor..
     */
    virtual ~ICloneFactoryFunc() {}
};

/**
 * @brief		Clone object.
 */
template<class T>
::std::shared_ptr<T> ICloneFactoryFunc<T>::clone()
{
    ::std::shared_ptr<T> ret(new T(*(const T *)this));

    if (ret == nullptr || ! ret->initialized()) {
        return nullptr;
    } else {
        return ret;
    }
}

#define CLONE_FUNC(T, ...) friend class ICloneFactoryFunc<T(__VA_ARGS__)>;
