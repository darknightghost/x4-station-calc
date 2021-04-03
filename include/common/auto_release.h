#pragma once

#include <functional>

/**
 * @brief   Auto release resource.
 *
 * @tparam  RefType     Reference type.
 */
template<typename RefType>
class AutoRelease {
  public:
    /**
     * @brief     Release callback.
     *
     * @param[in] data        Data to release.
     */
    typedef ::std::function<void(RefType &)> ReleaseCallback;

  private:
    RefType &       m_ref;             ///< Referenced object.
    ReleaseCallback m_releaseCallback; ///< Release callback.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   ref         Reference to data.
     * @param[in]   callback    Release callback.
     */
    AutoRelease(RefType &ref, ReleaseCallback callback) :
        m_ref(ref), m_releaseCallback(callback)
    {}

    AutoRelease(const AutoRelease &) = delete;
    AutoRelease(AutoRelease &&)      = delete;

    /**
     * @brief       Stop manage the reference.
     */
    void deattach()
    {
        m_releaseCallback = nullptr;
    }

    /**
     * @brief       Destructor.
     */
    virtual ~AutoRelease()
    {
        if (m_releaseCallback) {
            m_releaseCallback(m_ref);
        }
    }
};
