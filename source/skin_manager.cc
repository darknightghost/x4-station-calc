#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include <config.h>
#include <skin_manager.h>

/**
 * @brief       Constructor.
 */
SkinManager::SkinManager()
{
    qDebug() << "Loading skins:";
    for (auto &fileInfo : QDir(":/Skins").entryInfoList()) {
        if (fileInfo.isDir()) {
            QFile file(QString(":/Skins/%1/skin.qss").arg(fileInfo.fileName()));
            if (! file.open(QIODevice::OpenModeFlag::ReadOnly)) {
                continue;
            }
            m_skins[fileInfo.fileName()] = file.readAll();
            qDebug() << "    " << fileInfo.fileName();
        }
    }

    m_currentSkin = Config::instance()->getString("/skin", "default");
    if (m_skins.find(m_currentSkin) == m_skins.end()) {
        m_currentSkin = "default";
    }
    Config::instance()->setString("/skin", m_currentSkin);

    this->setInitialized();
}

/**
 * @brief       Get the names of all skins.
 */
QVector<QString> SkinManager::skins() const
{
    return m_skins.keys().toVector();
}

/**
 * @brief       Get the name of  current skin.
 */
const QString &SkinManager::currentSkin() const
{
    return m_currentSkin;
}

/**
 * @brief       Get the stype sheet of  current skin.
 */
QString SkinManager::currentSkinStyleSheet() const
{
    return m_skins[m_currentSkin];
}

/**
 * @brief       Set current skin.
 */
void SkinManager::setCurrentSkin(const QString &skin)
{
    if (m_currentSkin != skin && m_skins.find(skin) != m_skins.end()) {
        m_currentSkin = skin;
        emit this->skinChanged(m_currentSkin);
    }
}

/**
 * @brief       Destructor.
 */
SkinManager::~SkinManager() {}
