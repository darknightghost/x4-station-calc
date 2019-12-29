#pragma once

/**
 * @brief		Get minimum value.
 *
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
