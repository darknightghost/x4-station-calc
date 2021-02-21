#include <QtCore/QDebug>

#include <ui/main_window/language_menu.h>

/**
 * @brief		Constructor.
 */
LanguageMenu::LanguageMenu(QWidget *parent) : QMenu(parent)
{
    m_actionGroup = new QActionGroup(this);
    m_actionGroup->setExclusive(true);

    auto &languages = StringTable::instance()->getStrings("STR_LANGUAGE_TYPE");
    for (auto iter = languages.begin(); iter != languages.end(); iter++) {
        QAction *action = new QAction(*iter, m_actionGroup);
        action->setCheckable(true);
        m_localeMap[action] = iter.key();
        if (iter.key() == StringTable::instance()->language()) {
            action->setChecked(true);
        } else {
            action->setChecked(false);
        }
        this->addAction(action);
    }

    this->connect(m_actionGroup, &QActionGroup::triggered, this,
                  &LanguageMenu::onSelectedChanged);
}

/**
 * @brief		Destructor.
 */
LanguageMenu::~LanguageMenu() {}

/**
 * @brief		Selected language changed.
 */
void LanguageMenu::onSelectedChanged(QAction *action)
{
    qDebug() << m_localeMap[action];
    StringTable::instance()->setLanguage(m_localeMap[action]);
}
