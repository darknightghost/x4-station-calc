#include <locale/string_table.h>
#include <ui/locale/q_tree_widget_item_locale.h>

/**
 * @brief       Constructor.
 */
QTreeWidgetItemLocale::QTreeWidgetItemLocale(const QTreeWidgetItem &other) :
    QTreeWidgetItem(other)
{}

/**
 * @brief       Constructor.
 */
QTreeWidgetItemLocale::QTreeWidgetItemLocale(QTreeWidgetItem *parent,
                                             QTreeWidgetItem *preceding,
                                             int              type) :
    QTreeWidgetItem(parent, preceding, type)
{}

/**
 * @brief       Constructor.
 */
QTreeWidgetItemLocale::QTreeWidgetItemLocale(QTreeWidgetItem *  parent,
                                             const QStringList &strings,
                                             int                type) :
    QTreeWidgetItem(parent, strings, type)

{}

/**
 * @brief       Constructor.
 */
QTreeWidgetItemLocale::QTreeWidgetItemLocale(QTreeWidgetItem *parent,
                                             int              type) :
    QTreeWidgetItem(parent, type)

{}

/**
 * @brief       Constructor.
 */
QTreeWidgetItemLocale::QTreeWidgetItemLocale(QTreeWidget *    parent,
                                             QTreeWidgetItem *preceding,
                                             int              type) :
    QTreeWidgetItem(parent, preceding, type)

{}

/**
 * @brief       Constructor.
 */
QTreeWidgetItemLocale::QTreeWidgetItemLocale(QTreeWidget *      parent,
                                             const QStringList &strings,
                                             int                type) :
    QTreeWidgetItem(parent, strings, type)

{}

/**
 * @brief       Constructor.
 */
QTreeWidgetItemLocale::QTreeWidgetItemLocale(QTreeWidget *parent, int type) :
    QTreeWidgetItem(parent, type)
{}

/**
 * @brief       Constructor.
 */
QTreeWidgetItemLocale::QTreeWidgetItemLocale(const QStringList &strings,
                                             int                type) :
    QTreeWidgetItem(strings, type)

{}

/**
 * @brief       Constructor.
 */
QTreeWidgetItemLocale::QTreeWidgetItemLocale(int type) : QTreeWidgetItem(type)
{}

/**
 * @brief       Destructor.
 */
QTreeWidgetItemLocale::~QTreeWidgetItemLocale() {}

/**
 * @brief       Compare.
 */
bool QTreeWidgetItemLocale::operator<(const QTreeWidgetItem &other) const
{
    int       column   = treeWidget()->sortColumn();
    QCollator collator = StringTable::instance()->collator();
    return collator.compare(this->text(column), other.text(column)) < 0;
}
