#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

#include <locale/string_table.h>
#include <save/save_group.h>

/**
 * @brief		Create a module group information.
 */
SaveGroup::SaveGroup() : m_name(STR("STR_NEW_GROUP_NAME"))
{
    this->setInitialized();
}

/**
 * @brief		Load a module group information.
 */
SaveGroup::SaveGroup(QJsonObject &entry, const SaveVersion &version)
{
    // Name
    if (! entry.contains("name")) {
        return;
    }

    QJsonValue nameValue = entry.value("name");
    if (! nameValue.isString()) {
        return;
    }

    m_name = nameValue.toString();

    // Modules
    if (! entry.contains("modules")) {
        return;
    }

    QJsonValue modulesValue = entry.value("modules");
    if (! modulesValue.isArray()) {
        return;
    }
    QJsonArray array = modulesValue.toArray();

    // Load modules.
    for (auto value : array) {
        if (value.isObject()) {
            QJsonObject                   obj = value.toObject();
            ::std::shared_ptr<SaveModule> module
                = SaveModule::load(obj, version);
            if (module == nullptr) {
                continue;
            }

            // Insert modules.
            auto macroIter = m_modulesMacroIndex.find(module->module());
            if (macroIter == m_modulesMacroIndex.end()) {
                m_modules.append(module);
                m_modulesMacroIndex[module->module()] = m_modules.size() - 1;
            } else {
                m_modules[*macroIter]->setAmount(m_modules[*macroIter]->amount()
                                                 + module->amount());
            }
        }
    }
    this->setInitialized();
}

/**
 * @brief		Get name of the group.
 */
const QString &SaveGroup::name() const
{
    return m_name;
}

/**
 * @brief		Set name of the group.
 */
void SaveGroup::setName(QString name)
{
    m_name = ::std::move(name);
}

/**
 * @brief		Get modules.
 */
const QVector<::std::shared_ptr<SaveModule>> &SaveGroup::modules() const
{
    return m_modules;
}

/**
 * @brief		Get modules.
 */
std::shared_ptr<SaveModule> SaveGroup::module(int index)
{
    return m_modules[index];
}

/**
 * @brief		Set module index.
 */
void SaveGroup::setIndex(int oldIndex, int index)
{
    if (index < 0 || index >= m_modules.size()) {
        index = m_modules.size() - 1;
    }
    ::std::shared_ptr<SaveModule> module = m_modules[oldIndex];

    if (oldIndex < index) {
        for (int i = oldIndex; i < index; ++i) {
            m_modules[i]                                = m_modules[i + 1];
            m_modulesMacroIndex[m_modules[i]->module()] = i;
        }
        m_modules[index]                      = module;
        m_modulesMacroIndex[module->module()] = index;
    } else {
        for (int i = oldIndex; i > index; --i) {
            m_modules[i]                                = m_modules[i - 1];
            m_modulesMacroIndex[m_modules[i]->module()] = i;
        }
        m_modules[index]                      = module;
        m_modulesMacroIndex[module->module()] = index;
    }
}

/**
 * @brief		Add module.
 */
int SaveGroup::insertModule(int index, const QString &macro, quint64 count)
{
    // Insert modules.
    auto macroIter = m_modulesMacroIndex.find(macro);
    if (macroIter == m_modulesMacroIndex.end()) {
        // Make module information.
        ::std::shared_ptr<SaveModule> module = SaveModule::create(macro);
        module->setAmount(count);

        // Insert.
        if (index < 0) {
            index = m_modules.size();
        }

        m_modules.push_back(::std::shared_ptr<SaveModule>());
        for (int i = m_modules.size() - 1; i > index; --i) {
            m_modules[i]                                = m_modules[i - 1];
            m_modulesMacroIndex[m_modules[i]->module()] = i;
        }
        m_modules[index]                      = module;
        m_modulesMacroIndex[module->module()] = index;
        return index;
    } else {
        m_modules[*macroIter]->setAmount(m_modules[*macroIter]->amount()
                                         + count);
        return *macroIter;
    }
}

/**
 * @brief		Remove module.
 */
void SaveGroup::removeModule(int index)
{
    m_modulesMacroIndex.remove(m_modules[index]->module());
    for (int i = index; i < m_modules.size() - 1; ++i) {
        m_modules[i]                                = m_modules[i + 1];
        m_modulesMacroIndex[m_modules[i]->module()] = index;
    }

    m_modules.pop_back();
}

/**
 * @brief		Parse to json object.
 */
QJsonObject SaveGroup::toJson() const
{
    QJsonObject ret;
    ret.insert("name", m_name);

    QJsonArray modules;
    for (auto &module : m_modules) {
        modules.append(module->toJson());
    }
    ret.insert("modules", modules);

    return ret;
}

/**
 * @brief		Destructor.
 */
SaveGroup::~SaveGroup() {}
