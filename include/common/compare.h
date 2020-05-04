#pragma once

#include <cstdint>
#include <type_traits>

/**
 * @brief		Get minimum value.
 *
 * @tparam		T		Type of the value.
 * @param[in]	v1		First value.
 * @param[in]	v2		Second value.
 *
 * @return		Minimum value.
 */
template<typename T>
T min(T v1, T v2)
{
    return v1 > v2 ? v2 : v1;
}

/**
 * @brief		Get minimum value.
 *
 * @tparam		T		Type of the value.
 * @tparam		Args	Type of other argumets.
 * @param[in]	v1		First value.
 * @param[in]	v2		Second value.
 * @param[in]	v3		Third value.
 * @param[in]	args	Other values.
 *
 * @return		Minimum value.
 */
template<typename T, typename... Args>
T min(T v1, T v2, T v3, Args... args)
{
    T v = v1 > v2 ? v2 : v1;
    return min(v, v3, args...);
}

/**
 * @brief		Get maxium value.
 *
 * @tparam		T		Type of the value.
 * @param[in]	v1		First value.
 * @param[in]	v2		Second value.
 *
 * @return		Maxium value.
 */
template<typename T>
T max(T v1, T v2)
{
    return v1 > v2 ? v1 : v2;
}

/**
 * @brief		Get maxium value.
 *
 * @tparam		T		Type of the value.
 * @tparam		Args	Type of other argumets.
 * @param[in]	v1		First value.
 * @param[in]	v2		Second value.
 * @param[in]	v3		Third value.
 * @param[in]	args	Other values.
 *
 * @return		Maxium value.
 */
template<typename T, typename... Args>
T max(T v1, T v2, T v3, Args... args)
{
    T v = v1 > v2 ? v1 : v2;
    return max(v, v3, args...);
}

/**
 * @brief		Implement of between.
 *
 * @tparam		T		Type of the value.
 * @param[in]	value	Value.
 * @param[in]	b1		Border1.
 * @param[in]	b2		Border2.
 *
 * @return		If the value between b1 and b2, \c true is returned. Otherwise
 *				returns \c false.
 */
template<typename T>
bool betweenImpl(T value, T b1, T b2)
{
    T minBorder = min(b1, b2);
    T maxBorder = max(b1, b2);

    return value >= minBorder && value <= minBorder;
}

/**
 * @brief		Check if the value between b1 and b2.
 *
 * @tparam		T		Type of the value.
 * @param[in]	value	Value.
 * @param[in]	b1		Border1.
 * @param[in]	b2		Border2.
 *
 * @return		If the value between b1 and b2, \c true is returned. Otherwise
 *				returns \c false.
 */
template<typename T>
bool between(
    T value,
    typename ::std::conditional<
        ::std::is_integral<T>::value,
        typename ::std::
            conditional<::std::is_signed<T>::value, int64_t, uint64_t>::type,
        typename ::std::conditional<::std::is_floating_point<T>::value,
                                    long double,
                                    T>::type>::type b1,
    typename ::std::conditional<
        ::std::is_integral<T>::value,
        typename ::std::
            conditional<::std::is_signed<T>::value, int64_t, uint64_t>::type,
        typename ::std::conditional<::std::is_floating_point<T>::value,
                                    long double,
                                    T>::type>::type b2)
{
    return betweenImpl<typename ::std::conditional<
        ::std::is_integral<T>::value,
        typename ::std::conditional<::std::is_signed<T>::value, int64_t,
                                    uint64_t>::type,
        typename ::std::conditional<::std::is_floating_point<T>::value,
                                    long double, T>::type>::type>(value, b1,
                                                                  b2);
}
