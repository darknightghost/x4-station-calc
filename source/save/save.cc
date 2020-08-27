#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

#include <save/save.h>

/// Current version.
const SaveVersion Save::_currentVersion(1, 0, 0);

/**
 * @brief		Create a save.
 */
Save::Save()
{
    this->setInitialized();
}

/**
 * @brief		Load a save file.
 */
Save::Save(const QString &path) : m_path(path)
{
    // Read file.
    QFile file(path);

    if (! file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file :" << path << ".";
        return;
    }

    // Parse data.
    QByteArray      jsonStr = file.readAll();
    QJsonParseError jsonError;
    QJsonDocument   doc = QJsonDocument::fromJson(jsonStr, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << jsonError.errorString();
        return;
    }
    QJsonObject root = doc.object();

    // Load version.
    if (! root.contains("version")) {
        qDebug() << "Missing version.";
        return;
    }
    QJsonValue versionValue = root.value("version");
    if (! versionValue.isString()) {
        qDebug() << "Version must be a string .";
        return;
    }
    SaveVersion version(versionValue.toString());
    if (version > _currentVersion) {
        qDebug() << "Version too big.";
        return;
    }

    // Load groups.
    if (! root.contains("groups")) {
        qDebug() << "Missing groups.";
        return;
    }
    QJsonValue groupsValue = root.value("groups");
    if (! groupsValue.isArray()) {
        qDebug() << "Groups must be a array .";
        return;
    }
    for (QJsonValue value : groupsValue.toArray()) {
        if (value.isObject()) {
            QJsonObject                  obj   = value.toObject();
            ::std::shared_ptr<SaveGroup> group = SaveGroup::load(obj, version);
            if (group != nullptr) {
                m_groups.append(group);
                m_groupsIndex[group] = m_groups.size() - 1;
            }
        }
    }

    this->setInitialized();
}

/**
 * @brief		Get file path.
 */
const QString &Save::path() const
{
    return m_path;
}

/**
 * @brief		Set file path.
 */
void Save::setPath(const QString &path)
{
    if (m_path != "") {
        m_path = path;
    }
}

/**
 * @brief		Get groups.
 */
const QVector<::std::shared_ptr<SaveGroup>> &Save::groups() const
{
    return m_groups;
}

/**
 * @brief		Get group.
 */
::std::shared_ptr<SaveGroup> Save::group(int index)
{
    return m_groups[index];
}

/**
 * @brief		Set group index.
 */
void Save::setIndex(int oldIndex, int index)
{
    if (index < 0 || index >= m_groups.size()) {
        index = m_groups.size() - 1;
    }
    ::std::shared_ptr<SaveGroup> group = m_groups[oldIndex];

    if (oldIndex < index) {
        for (int i = oldIndex; i < index; ++i) {
            m_groups[i]                = m_groups[i + 1];
            m_groupsIndex[m_groups[i]] = i;
        }
        m_groups[index]      = group;
        m_groupsIndex[group] = index;

    } else {
        for (int i = oldIndex; i > index; --i) {
            m_groups[i]                = m_groups[i - 1];
            m_groupsIndex[m_groups[i]] = i;
        }
        m_groups[index]      = group;
        m_groupsIndex[group] = index;
    }
}

/**
 * @brief		Add group.
 */
int Save::insertGroup(int index, ::std::shared_ptr<SaveGroup> group)
{
    if (index < 0) {
        index = m_groups.size();
    }

    m_groups.push_back(::std::shared_ptr<SaveGroup>());
    for (int i = m_groups.size() - 1; i > index; --i) {
        m_groups[i]                = m_groups[i - 1];
        m_groupsIndex[m_groups[i]] = i;
    }

    m_groups[index]      = group;
    m_groupsIndex[group] = index;

    return index;
}

/**
 * @brief		Remove module.
 */
void Save::removeGroup(int index)
{
    m_groupsIndex.remove(m_groups[index]);
    for (int i = index; i < m_groups.size() - 1; ++i) {
        m_groups[i]                = m_groups[i + 1];
        m_groupsIndex[m_groups[i]] = i;
    }
    m_groups.pop_back();
}

/**
 * @brief		Write save file.
 */
bool Save::write() const
{
    if (m_path == "") {
        return false;
    }

    // Open file.
    QFile file(m_path);
    if (! file.open(QIODevice::OpenModeFlag::WriteOnly)) {
        return false;
    }

    // Make json document.
    QJsonDocument doc;
    QJsonObject   root;
    root.insert("version", (QString)_currentVersion);

    QJsonArray groups;
    for (auto &group : m_groups) {
        groups.append(group->toJson());
    }
    root.insert("groups", groups);

    doc.setObject(root);
    file.write(doc.toJson(QJsonDocument::JsonFormat::Indented));

    file.close();

    return true;
}

/**
 * @brief		Write save file.
 */
bool Save::write(const QString &path)
{
    // Open file.
    QFile file(path);
    if (! file.open(QIODevice::OpenModeFlag::WriteOnly)) {
        return false;
    }

    // Make json document.
    QJsonDocument doc;
    QJsonObject   root;
    root.insert("version", (QString)_currentVersion);

    QJsonArray groups;
    for (auto &group : m_groups) {
        groups.append(group->toJson());
    }
    root.insert("groups", groups);

    doc.setObject(root);
    file.write(doc.toJson(QJsonDocument::JsonFormat::Indented));

    file.close();

    m_path = path;

    return true;
}

/**
 * @brief		Destructor.
 */
Save::~Save() {}
