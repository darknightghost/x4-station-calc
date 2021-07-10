#pragma once

#include <utility>

#include <boost/bimap.hpp>

/**
 * @brief       Make a bimap.
 *
 * @param[in]   values      Values.
 *
 * @return      Bimap.
 */
template<typename Type1, typename Type2>
constexpr inline ::boost::bimap<Type1, Type2> makeBimap(
    ::std::initializer_list<typename ::boost::bimap<Type1, Type2>::value_type>
        values)
{
    return ::boost::bimap<Type1, Type2>(values.begin(), values.end());
};

/**
 * @brief   Range.
 *
 * @tparam  T       Data type.
 * @tparam  incLB   \c true if including left boundary.
 * @tparam  incRB   \c true if including right boundary.
 */
template<typename T, bool incLB = true, bool incRB = true>
class Range {
  private:
    T m_min; ///< Minimum value.
    T m_max; ///< Maxium value.

  public:
    /**
     * @brief       Constructor.
     */
    Range() = default;

    /**
     * @brief       Constructor.
     *
     * @param[in]   min     Minimum value.
     * @param[in]   max     Maxium value.
     */
    Range(T min, T max) : m_min(::std::move(min)), m_max(::std::move(max)) {}

    /**
     * @brief       Copy constructor.
     *
     * @tparam      T1      Data type.
     * @param[in]   range   Range to copy.
     */
    template<typename T1>
    Range(const Range<T1, incLB, incRB> &range) :
        m_min(range.min()), m_max(range.max())
    {}

    /**
     * @brief       Get left boundary.
     *
     * @return      Left boundary.
     */
    const T &min() const
    {
        return m_min;
    }

    /**
     * @brief       Get right boundary.
     *
     * @return      Right boundary.
     */
    const T &max() const
    {
        return m_max;
    }

    /**
     * @brief       Set left boundary.
     *
     * @param[in]   min     Left boundary.
     */
    void setMin(T min)
    {
        m_min = min;
    }

    /**
     * @brief       Set right boundary.
     *
     * @param[in]   max     Right boundary.
     */
    void setMax(T max)
    {
        m_max = max;
    }

    /**
     * @brief       Set range.
     *
     * @param[in]   min     Left boundary.
     * @param[in]   max     Right boundary.
     */
    void setRange(T min, T max)
    {
        m_min = min;
        m_max = max;
    }

    /**
     * @brief       Set range.
     *
     * @tparam      T1      Data type.
     * @param[in]   range   Range.
     */
    template<typename T1>
    void setRange(const Range<T1, incLB, incRB> &range)
    {
        m_min = range.min();
        m_max = range.max();
    }

    /**
     * @brief       Compare value and range.
     *
     * @param[in]   value       Value to compare.
     *
     * @return      If the value ls lesser than the minimum value of the range,
     *              the method returns -1; If the value is in the range, the
     *              metod returns 0; If the value is bigger than the max value
     *              in the range, the method returns 1.
     */
    int compare(T value)
    {
        // Left boundary.
        if (incLB) {
            if (value < m_min) {
                return -1;
            };
        } else {
            if (value <= m_min) {
                return -1;
            };
        }

        // Right boundary.
        if (incRB) {
            if (value > m_max) {
                return 1;
            };
        } else {
            if (value >= m_max) {
                return 1;
            };
        }

        return 0;
    }

    /**
     * @brief       Destructor.
     */
    virtual ~Range() {}

    // Operators.
    /**
     * @brief       Operator "=".
     *
     * @tparam      T1      Data type.
     * @param[in]   range   Range to copy.
     *
     * @return      Reference to current object.
     */
    template<typename T1>
    Range &operator=(const Range<T1, incLB, incRB> &range)
    {
        m_min = range.min();
        m_max = range.max();

        return *this;
    }
};
