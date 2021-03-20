#pragma once

#include <game_data/game_data.h>
#include <locale/string_table.h>

/**
 * @brief		Generic string.
 */
class GenericString
{
  protected:
    /**
     * @brief	Constructor.
     */
    GenericString() {};

  public:
    /**
     * @brief	Get string.
     *
     * @return	String.
     */
    virtual const QString toString() const = 0;

    /**
     * @brief	Destructor.
     */
    virtual ~GenericString() {}
};

/**
 * @brief		Qt string.
 */
class QtString : public GenericString
{
  private:
    QString m_string; ///< String.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	s	String.
     */
    QtString(const QString &s) : m_string(s) {}

    /**
     * @brief	Get string.
     *
     * @return	String.
     */
    virtual const QString toString() const override
    {
        return m_string;
    }

    /**
     * @brief	Destructor.
     */
    virtual ~QtString() {}
};

/**
 * @brief		Game string.
 */
class GameString : public GenericString
{
  private:
    GameTexts::IDPair m_id; ///< ID of the string.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	pair	ID pair.
     */
    GameString(const GameTexts::IDPair &pair) : m_id(pair) {}

    /**
     * @brief	Get string.
     *
     * @return	String.
     */
    virtual const QString toString() const override
    {
        return GameData::instance()->texts()->text(m_id);
    }

    /**
     * @brief	Destructor.
     */
    virtual ~GameString() {}
};

/**
 * @brief		Locale string.
 */
class LocaleString : public GenericString
{
  private:
    QString m_id; ///< String ID.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	id		String ID.
     */
    LocaleString(const QString &id) : m_id(id) {}

    /**
     * @brief	Get string.
     *
     * @return	String.
     */
    virtual const QString toString() const override
    {
        return STR(m_id);
    }

    /**
     * @brief	Destructor.
     */
    virtual ~LocaleString() {}
};
