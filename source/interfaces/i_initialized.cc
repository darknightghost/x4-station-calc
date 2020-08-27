#include <interfaces/i_initialized.h>

/**
 * @brief   Constructor, initialize and set m_initialized to false.
 */
IInitialized::IInitialized() : m_initialized(false) {}

/**
 * @brief   Get initialize result.
 */
bool IInitialized::initialized()
{
    return m_initialized;
}

/**
 * @brief   Destructor.
 */
IInitialized::~IInitialized() {}

/**
 * @brief   Set initialize status to true.
 */
void IInitialized::setInitialized()
{
    m_initialized = true;
}
