#pragma once

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTreeWidgetItem>

/**
 * @brief	Item of ware.
 */
class WareItem : public QTreeWidgetItem {
  private:
    QString m_ware; ///< Ware.
    qint64  m_min;  ///< Minimum amount each hour.
    qint64  m_max;  ///< Maxium amount each hour.

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   ware        Macro of the ware.
     * @param[in]   min         Minimum amount each hour.
     * @param[in]   max         Maxium amount each hour.
     */
    WareItem(const QString &ware, qint64 min = 0, qint64 max = 0);

    /**
     * @brief       Get the macro of the ware.
     *
     * @return      Macro of the ware.
     */
    const QString &ware() const;

    /**
     * @brief       Set range.
     *
     * @param[in]   min         Minimum amount each hour.
     * @param[in]   max         Maxium amount each hour.
     */
    void setRange(qint64 min, qint64 max);

    /**
     * @brief		Change language.
     */
    void onLanguageChanged();

    /**
     * @brief       Destructor.
     */
    virtual ~WareItem();
};
