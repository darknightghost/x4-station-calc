#include <memory>

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

#include <config.h>
#include <ui/customized_widgets/customized_message_box.h>

/**
 * @brief       Constructor.
 */
CustomizedMessageBox::CustomizedMessageBox(QMessageBox::Icon            icon,
                                           const QString &              title,
                                           const QString &              text,
                                           QMessageBox::StandardButtons buttons,
                                           QWidget *                    parent,
                                           Qt::WindowFlags) :
    CustomizedDialog(CustomizedDialog::BorderType::Fixed,
                     TitleBar::TitleBarButton::CloseButton,
                     nullptr,
                     parent),
    m_msgBox(nullptr)
{
    m_msgBox = new QMessageBox(icon, title, text, buttons, this,
                               Qt::WindowType::Widget);
    this->setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground, true);
    m_msgBox->setWindowFlags(Qt::WindowType::Widget);
    this->setWidget(m_msgBox);
    this->setWindowTitle(title);
    this->connect(m_msgBox, &QMessageBox::accepted, this,
                  &CustomizedMessageBox::accept);
    this->connect(m_msgBox, &QMessageBox::finished, this,
                  &CustomizedMessageBox::done);
    this->connect(m_msgBox, &QMessageBox::rejected, this,
                  &CustomizedMessageBox::reject);
}

/**
 * @brief       Constructor.
 */
CustomizedMessageBox::CustomizedMessageBox(QWidget *parent) :
    CustomizedDialog(CustomizedDialog::BorderType::Fixed,
                     TitleBar::TitleBarButton::CloseButton,
                     nullptr,
                     parent)
{
    m_msgBox = new QMessageBox(this);
    this->setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground, true);
    m_msgBox->setWindowFlags(Qt::WindowType::Widget);
    this->setWidget(m_msgBox);
    this->connect(m_msgBox, &QMessageBox::accepted, this,
                  &CustomizedMessageBox::accept);
    this->connect(m_msgBox, &QMessageBox::finished, this,
                  &CustomizedMessageBox::done);
    this->connect(m_msgBox, &QMessageBox::rejected, this,
                  &CustomizedMessageBox::reject);
}

/**
 * @brief       Set default button.
 */
void CustomizedMessageBox::setDefaultButton(QMessageBox::StandardButton button)
{
    m_msgBox->setDefaultButton(button);
}

/**
 * @brief       Get clicked standard button.
 */
QMessageBox::StandardButton CustomizedMessageBox::clickedStandardButton() const
{
    return m_msgBox->standardButton(m_msgBox->clickedButton());
}

/**
 * @brief       Destructor.
 */
CustomizedMessageBox::~CustomizedMessageBox() {}

/**
 * @brief       Show message box critical.
 */
QMessageBox::StandardButton
    CustomizedMessageBox::critical(QWidget *                    parent,
                                   const QString &              title,
                                   const QString &              text,
                                   QMessageBox::StandardButtons buttons,
                                   QMessageBox::StandardButton  defaultButton)
{
    ::std::unique_ptr<CustomizedMessageBox> msgBox(new CustomizedMessageBox(
        QMessageBox::Icon::Critical, title, text, buttons, parent));
    msgBox->setDefaultButton(defaultButton);
    msgBox->exec();
    return msgBox->clickedStandardButton();
}

/**
 * @brief       Show message box information.
 */
QMessageBox::StandardButton
    CustomizedMessageBox::information(QWidget *                    parent,
                                      const QString &              title,
                                      const QString &              text,
                                      QMessageBox::StandardButtons buttons,
                                      QMessageBox::StandardButton defaultButton)
{
    ::std::unique_ptr<CustomizedMessageBox> msgBox(new CustomizedMessageBox(
        QMessageBox::Icon::Information, title, text, buttons, parent));
    msgBox->setDefaultButton(defaultButton);
    msgBox->exec();
    return msgBox->clickedStandardButton();
}

/**
 * @brief       Show message box question.
 */
QMessageBox::StandardButton
    CustomizedMessageBox::question(QWidget *                    parent,
                                   const QString &              title,
                                   const QString &              text,
                                   QMessageBox::StandardButtons buttons,
                                   QMessageBox::StandardButton  defaultButton)
{
    ::std::unique_ptr<CustomizedMessageBox> msgBox(new CustomizedMessageBox(
        QMessageBox::Icon::Question, title, text, buttons, parent));
    msgBox->setDefaultButton(defaultButton);
    msgBox->exec();
    return msgBox->clickedStandardButton();
}

/**
 * @brief       Show message box warning.
 */
QMessageBox::StandardButton
    CustomizedMessageBox::warning(QWidget *                    parent,
                                  const QString &              title,
                                  const QString &              text,
                                  QMessageBox::StandardButtons buttons,
                                  QMessageBox::StandardButton  defaultButton)
{
    ::std::unique_ptr<CustomizedMessageBox> msgBox(new CustomizedMessageBox(
        QMessageBox::Icon::Warning, title, text, buttons, parent));
    msgBox->setDefaultButton(defaultButton);
    msgBox->exec();
    return msgBox->clickedStandardButton();
}
