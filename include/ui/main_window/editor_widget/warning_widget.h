#pragma once

#include <QtWidgets/QLabel>

#include <common/generic_string.h>

/**
 * @brief		Warning widget.
 */
class WarningWidget : public QLabel {
    Q_OBJECT;

  private:
    ::std::shared_ptr<GenericString> m_string; ///< String to show.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	s			String to show.
     * @param[in]	parent		Parent.
     */
    WarningWidget(::std::shared_ptr<GenericString> s,
                  QWidget *                        parent = nullptr);

    /**
     * @brief		Destructor.
     */
    virtual ~WarningWidget();

  public slots:
    /**
     * @brief		Change language.
     */
    void onLanguageChanged();
};
