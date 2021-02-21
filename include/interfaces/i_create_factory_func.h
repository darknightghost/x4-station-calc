#pragma once

#include <memory>

#include <interfaces/i_initialized.h>

/**
 * @brief	Interface for factory function \c create.
 *
 * @tparam	T		Type of the class implements this interface.
 * @tparam	Args	Types of the arguments of the constructor.
 */
template<class T, typename... Args>
class ICreateFactoryFunc : virtual protected IInitialized
{
  public:
    /**
     * @brief	Constructor.
     */
    ICreateFactoryFunc() : IInitialized() {}

    /**
     * @brief		Create object.
     *
     * @param[in]	args		Arguments.
     *
     * @return		On success, a new object is reutnred. Otherwise returns
     *				nullptr.
     */
    static ::std::shared_ptr<T> create(Args... args);

    /**
     * @brief	Destructor..
     */
    virtual ~ICreateFactoryFunc() {}
};

/**
 * @brief		Create object.
 */
template<class T, typename... Args>
::std::shared_ptr<T> ICreateFactoryFunc<T, Args...>::create(Args... args)
{
    ::std::shared_ptr<T> ret(new T(args...));

    if (ret == nullptr || ! ret->initialized())
    {
        return nullptr;
    }
    else
    {
        return ret;
    }
}

#define CREATE_FUNC(T, ...) friend class ICreateFactoryFunc<T, ##__VA_ARGS__>;
