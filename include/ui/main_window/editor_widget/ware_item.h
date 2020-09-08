#pragma once

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTreeWidgetItem>

#include <common.h>

/**
 * @brief	Item of ware.
 */
class WareItem : public QTreeWidgetItem {
  private:
    QString       m_ware;  ///< Ware.
    Range<qint64> m_range; ///< Range of the amount each hour.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   ware        Macro of the ware.
     * @param[in]   range       Range of the amount each hour.
     */
    WareItem(const QString &      ware,
             const Range<qint64> &range = Range<qint64>({0, 0}));

    /**
     * @brief       Get the macro of the ware.
     *
     * @return      Macro of the ware.
     */
    const QString &ware() const;

    /**
     * @brief       Set range.
     *
     * @param[in]   range       Range of the amount each hour.
     */
    void setRange(const Range<qint64> &range);

    /**
     * @brief		Change language.
     */
    void onLanguageChanged();

    /**
     * @brief       Destructor.
     */
    virtual ~WareItem();
};
