#pragma once

#include <QtCore/QMap>
#include <QtWidgets/QAction>
#include <QtWidgets/QActionGroup>
#include <QtWidgets/QMenu>

#include <locale/string_table.h>

class LanguageMenu : public QMenu {
    Q_OBJECT
  protected:
    QMap<QAction *, QString> m_localeMap;   ///< Map action and value.
    QActionGroup *           m_actionGroup; ///< Action group of the languages.

  public:
    /**
     * @brief		Constructor.
     */
    LanguageMenu();

    /**
     * @brief		Destructor.
     */
    virtual ~LanguageMenu();

  private slots:
    /**
     * @brief		Selected language changed.
     *
     * @param[in]	action		Selected action.
     */
    void onSelectedChanged(QAction *action);
};
