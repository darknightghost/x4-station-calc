#pragma once

#include <functional>

#include <QtCore/QMap>
#include <QtWidgets/QTreeWidgetItem>

#include <common/generic_string.h>
#include <game_data/game_data.h>
#include <locale/string_table.h>

/**
 * @brief		Item in imformation widget.
 */
class InfoItem : public QTreeWidgetItem {
  private:
    ::std::unique_ptr<GenericString> m_first;  ///< ID of string in column 1.
    ::std::unique_ptr<GenericString> m_second; ///< String in column 2.
    ::std::function<void(int)>       m_onDblClick; ///< Douible click callback.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	first		First column.
     */
    InfoItem(::std::unique_ptr<GenericString> first);

    /**
     * @brief		Constructor.
     *
     * @param[in]	first		First column.
     * @param[in]	second		Second column.
     */
    InfoItem(::std::unique_ptr<GenericString> first,
             ::std::unique_ptr<GenericString> second);

    /**
     * @brief		Constructor.
     *
     * @param[in]	first			First column.
     * @param[in]	onDblCkicked	Double click callback.
     */
    InfoItem(::std::unique_ptr<GenericString> first,
             ::std::function<void(int)>       onDblCkicked);

    /**
     * @brief		Constructor.
     *
     * @param[in]	first			First column.
     * @param[in]	second			Second column.
     * @param[in]	onDblCkicked	Double click callback.
     */
    InfoItem(::std::unique_ptr<GenericString> first,
             ::std::unique_ptr<GenericString> second,
             ::std::function<void(int)>       onDblCkicked);

    /**
     * @brief		Destructor.
     */
    virtual ~InfoItem();

  public:
    /**
     * @brief		Called when language changed.
     */
    void onLanguageChanged();

    /**
     * @brief		Called when double clicked.
     *
     * @param[in]	column		Column.
     */
    void onDoubleClicked(int column);
};
