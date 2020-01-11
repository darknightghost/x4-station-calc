#pragma once

#include <memory>

#include <interface/i_is_good.h>

template<class>
class ILoadFactoryFunc;

/**
 * @brief	Interface for factory function \c loac.
 */
template<class T, typename... Args>
class ILoadFactoryFunc<T(Args...)> : virtual protected IIsGood {
  public:
    /**
     * @brief	Constructor.
     */
    ILoadFactoryFunc() : IIsGood() {}

    /**
     * @brief		Load object.
     *
     * @param[in]	args		Arguments.
     *
     * @return		On success, a new object is reutnred. Otherwise returns
     *				nullptr.
     */
    static ::std::shared_ptr<T> load(Args... args)
    {
        ::std::shared_ptr<T> ret(new T(args...));

        if (ret == nullptr || ! ret->good()) {
            return nullptr;

        } else {
            return ret;
        }
    }

    /**
     * @brief	Destructor..
     */
    virtual ~ILoadFactoryFunc() {}
};

#define LOAD_FUNC(T, ...) friend class ILoadFactoryFunc<T(__VA_ARGS__)>;
