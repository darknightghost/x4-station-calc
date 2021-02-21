#pragma once

#include <QtWidgets/QLabel>

/**
 * @brief   Transparent label.
 */
class TransparentLabel : public QLabel
{
    Q_OBJECT;

  public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   text        Label text.
     * @param[in]   parent      Parent widget.
     */
    TransparentLabel(const QString &text, QWidget *parent = nullptr);

    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent widget.
     */
    TransparentLabel(QWidget *parent = nullptr);

    /**
     * @brief       Destructor.
     */
    virtual ~TransparentLabel();
};
