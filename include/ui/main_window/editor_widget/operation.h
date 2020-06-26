#pragma once

#include <interfaces/i_create_factory_func.h>

/**
 * @brief	Operation.
 */
class Operation {
  public:
    /**
     * @brief	Do operation.
     *
     * @return	On success, the method will return \c true, otherwise returns
     *			\c false.
     */
    virtual bool doOperation() = 0;

    /**
     * @brief	Undo operation.
     */
    virtual void undoOperation() = 0;

    /**
     * @brief	Destructor.
     */
    virtual ~Operation() {};
};

/**
 * @brief	Base class of all operations.
 *
 * @tpatam	T		Type of operation.
 * @tpatam	Args	Types of parameters of constructor.
 */
template<typename T, typename... Args>
class OperationBase :
    virtual public Operation,
    virtual public ICreateFactoryFunc<T(Args...)> {
  public:
    /**
     * @brief		Destructor.
     */
    virtual ~OperationBase() {};
};
