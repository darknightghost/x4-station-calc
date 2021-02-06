#pragma once

#include <memory>

#include <interfaces/i_initialized.h>

/**
 * @brief	Interface for factory function \c load.
 *
 * @tparam	T		Type of the class implements this interface.
 * @tparam	Args	Types of the arguments of the constructor.
 */
template<class T, typename... Args>
class ILoadFactoryFunc : virtual protected IInitialized {
  public:
    /**
     * @brief	Constructor.
     */
    ILoadFactoryFunc() : IInitialized() {}

    /**
     * @brief		Load object.
     *
     * @param[in]	args		Arguments.
     *
     * @return		On success, a new object is reutnred. Otherwise returns
     *				nullptr.
     */
    static ::std::shared_ptr<T> load(Args... args);

    /**
     * @brief	Destructor..
     */
    virtual ~ILoadFactoryFunc() {}
};

/**
 * @brief		Load object.
 */
template<class T, typename... Args>
::std::shared_ptr<T> ILoadFactoryFunc<T, Args...>::load(Args... args)
{
    ::std::shared_ptr<T> ret(new T(args...));

    if (ret == nullptr || ! ret->initialized()) {
        return nullptr;
    } else {
        return ret;
    }
}

#define LOAD_FUNC(T, ...) friend class ILoadFactoryFunc<T, ##__VA_ARGS__>;
