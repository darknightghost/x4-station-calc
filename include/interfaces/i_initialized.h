#pragma once

/**
 * @brief   Provide good() medthod.
 *
 */
class IInitialized
{
  private:
    bool m_initialized; ///< Initialize result.

  public:
    /**
     * @brief   Constructor, initialize and set m_good to false.
     */
    IInitialized();

    /**
     * @brief   Get initialize result.
     *
     * @return  Initialize result.
     */
    bool initialized();

    /**
     * @brief   Destructor.
     */
    virtual ~IInitialized();

  protected:
    /**
     * @brief   Set initialize status to true.
     */
    void setInitialized();
};
