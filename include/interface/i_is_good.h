#pragma once

/**
 * @brief   Provide good() medthod.
 *
 */
class IIsGood {
    protected:
        bool            m_good;     //< Initialize result.

    public:
        /**
         * @brief   Constructor, initialize and set m_good to false.
         */
        IIsGood();

        /**
         * @brief   Get initialize result.
         *
         * @return  Initialize result.
         */
        bool        good();

        /**
         * @brief   Destructor.
         */
        virtual     ~IIsGood();

    protected:
        /**
         * @brief   Set initialize status to true.
         */
        void        setGood();
};
