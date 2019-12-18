#pragma once

#include <memory>
#include <mutex>

#include <interface/i_is_good.h>

/**
 * @brief   Base class of signleton object.
 *
 */
template<class T, typename... Args>
class Singleton : virtual public IIsGood {
  protected:
    static ::std::shared_ptr<T> _instance;     //< Instance.
    static ::std::mutex         _instanceLock; //< Instance lock.

  protected:
    /**
     * @brief   Constructor.
     */
    Singleton();

  public:
    /**
     * @brief   Destructor.
     */
    virtual ~Singleton();

  public:
    /**
     * @brief       Initialize the instance.
     *
     * @param[in]   args        Arguments of constructor.
     *
     * @return      On success or the instance exists, the method returns
     *              the initialized instance, otherwise returns nullptr.
     */
    static ::std::shared_ptr<T> initialize(Args... args);

    /**
     * @brief       Get.
     *
     * @return      If the instalce has been initialized, the method
     *              returns the instance, otherwise returns nullptr.
     */
    static ::std::shared_ptr<T> instance();
};

#define SIGNLETON_OBJECT(T, ...) friend class Singleton<T, ##__VA_ARGS__>;

template<class T, typename... Args>
::std::shared_ptr<T> Singleton<T, Args...>::_instance = nullptr;

template<class T, typename... Args>
::std::mutex Singleton<T, Args...>::_instanceLock;

/**
 * @brief   Constructor.
 */
template<class T, typename... Args>
Singleton<T, Args...>::Singleton()
{}

/**
 * @brief   Destructor.
 */
template<class T, typename... Args>
Singleton<T, Args...>::~Singleton()
{}

/**
 * @brief       Initialize the instance.
 *
 * @param[in]   args        Arguments of constructor.
 *
 * @return      On success or the instance exists, the method returns
 *              the initialized instance, otherwise returns nullptr.
 */
template<class T, typename... Args>
::std::shared_ptr<T> Singleton<T, Args...>::initialize(Args... args)
{
    ::std::lock_guard<::std::mutex> locker(_instanceLock);

    if (_instance == nullptr) {
        /// Create new instance
        _instance = ::std::shared_ptr<T>(new T(args...));

        if (! _instance->good()) {
            _instance = nullptr;
        }
    }

    return _instance;
}

/**
 * @brief       Get.
 *
 * @param[in]   args        Arguments of constructor.
 *
 * @return      If the instalce has been initialized, the method
 *              returns the instance, otherwise returns nullptr.
 */
template<class T, typename... Args>
::std::shared_ptr<T> Singleton<T, Args...>::instance()
{
    return _instance;
}
