#include <QtCore/QFile>
#include <QtGui/QColor>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

#include <locale/string_table.h>
#include <ui/main_window/editor_widget/warning_widget.h>

#define PADDING_H        1
#define PADDING_V        1
#define BACKGROUND_COLOR QColor(255, 255, 0)
#define FOREROUND_COLOR  QColor(0, 0, 0)

/**
 * @brief		Constructor.
 */
WarningWidget::WarningWidget(::std::shared_ptr<GenericString> s,
                             QWidget *                        parent) :
    QLabel(parent),
    m_string(s)
{
    this->connect(StringTable::instance().get(), &StringTable::languageChanged,
                  this, &WarningWidget::onLanguageChanged);

    this->setProperty("class", "WarningWidget");

    this->setAutoFillBackground(true);
    this->onLanguageChanged();
}

/**
 * @brief		Destructor.
 */
WarningWidget::~WarningWidget() {}

/**
 * @brief		Change language.
 */
void WarningWidget::onLanguageChanged()
{
    this->setText(m_string->toString());
}
