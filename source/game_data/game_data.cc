#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QThread>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <config.h>
#include <game_data/game_data.h>
#include <game_data/game_texts.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 *
 * @param[in]	splash		Splash widget.
 *
 */
GameData::GameData(SplashWidget *splash) : QObject(nullptr)
{
    while (true) {
        splash->setText(STR("STR_CHECKING_GAME_PATH"));

        // Check game path
        m_gamePath = Config::instance()->getString("/gamePath", "");
        QMap<int, GameVFS::CatFileInfo> catFiles;
        if (! checkGamePath(m_gamePath, catFiles)) {
            if (splash->callFunc(::std::function<bool()>(
                    ::std::bind(&GameData::askGamePath, this)))) {
                continue;

            } else {
                return;
            }
        }

        // Load vfs
        splash->setText(STR("STR_LOADING_VFS"));
        ::std::shared_ptr<GameVFS> vfs = GameVFS::create(
            m_gamePath, catFiles,
            [&](const QString &s) -> void {
                splash->setText(STR("STR_LOADING_VFS") + "\n" + s);
            },
            [&](const QString &s) -> void {
                splash->callFunc(::std::function<void()>([&]() -> void {
                    QMessageBox::critical(splash, STR("STR_ERROR"), s);
                }));
            });
        if (vfs == nullptr) {
            Config::instance()->setString("/gamePath", "");
            continue;
        }

        // Load text
        ::std::shared_ptr<GameTexts> texts
            = GameTexts::load(vfs, [&](const QString &s) -> void {
                  splash->setText(STR("STR_LOADING_TEXTS") + "\n" + s);
              });

        if (texts == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                                      STR("STR_FAILED_LOAD_STRINGS"));
            }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }

        // Load game macros
        ::std::shared_ptr<GameMacros> macros
            = GameMacros::load(vfs, [&](const QString &s) -> void {
                  splash->setText(s);
              });

        if (macros == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                                      STR("STR_FAILED_LOAD_MACROS"));
            }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }

        // Load game components
        ::std::shared_ptr<GameComponents> components
            = GameComponents::load(vfs, [&](const QString &s) -> void {
                  splash->setText(s);
              });

        if (components == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                                      STR("STR_FAILED_LOAD_COMPONENTS"));
            }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }

        // Load game races
        ::std::shared_ptr<GameRaces> races
            = GameRaces::load(vfs, texts, [&](const QString &s) -> void {
                  splash->setText(s);
              });

        if (races == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                                      STR("STR_FAILED_LOAD_RACESS"));
            }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }

        // Set value
        m_vfs        = vfs;
        m_texts      = texts;
        m_macros     = macros;
        m_components = components;
        m_races      = races;

        break;
    }

    this->setGood();
}

/**
 * @brief		Check path of game.
 *
 * @param[in]	path	Path of the game.
 *
 * @return		True if the path of game is available, otherwise returns
 *				false.
 *
 */
bool GameData::checkGamePath(const QString &path)
{
    QMap<int, GameVFS::CatFileInfo> catFiles;
    return this->checkGamePath(path, catFiles);
}

/**
 * @brief		Check path of game.
 *
 * @param[in]	path	Path of the game.
 *
 * @return		True if the path of game is available, otherwise returns
 * false.
 *
 */
bool GameData::setGamePath(const QString &path)
{
    QMap<int, GameVFS::CatFileInfo> catFiles;

    if (! checkGamePath(path, catFiles)) {
        return false;
    }

    Config::instance()->setString("/gamePath", path);
    return true;
}

/**
 * @brief Destructor.
 */
GameData::~GameData() {}

/**
 * @brief		Check path of game.
 *
 * @param[in]	path		Path of the game.
 * @param[out]	catFiles	Cat files found.
 *
 * @return		True if the path of game is available, otherwise returns
 *				false.
 *
 */
bool GameData::checkGamePath(const QString &                  path,
                             QMap<int, GameVFS::CatFileInfo> &catFiles)
{
    // Prepare
    QDir                            dir(path);
    bool                            execFound = false;
    QMap<int, GameVFS::CatFileInfo> catsFound;
    qDebug() << "Checking game path...";

    // List files.
    // Filters
    QRegExp execFilter("x4|x4\\.exe");
    execFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    QRegExp catFilter("\\d+\\.cat");
    catFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    QRegExp catSigFilter("\\d+_sig\\.cat");
    catSigFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    QRegExp datFilter("\\d+\\.dat");
    datFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    QRegExp datSigFilter("\\d+_sig\\.dat");
    datSigFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);

    for (auto &f :
         dir.entryList(QDir::Filter::NoFilter, QDir::SortFlag::Name)) {
        if (execFilter.exactMatch(f)) {
            qDebug() << "Game executable file :" << f << ".";
            execFound = true;

        } else if (catFilter.exactMatch(f)) {
            qDebug() << "Found cat file file :" << f << ".";
            int  key  = this->getNumberFromStr(f);
            auto iter = catsFound.find(key);
            if (iter == catsFound.end()) {
                catsFound[key] = GameVFS::CatFileInfo();
            }
            catsFound[key].cat = f;

        } else if (catSigFilter.exactMatch(f)) {
            qDebug() << "Found signature file of dat file :" << f << ".";
            int  key  = this->getNumberFromStr(f);
            auto iter = catsFound.find(key);
            if (iter == catsFound.end()) {
                catsFound[key] = GameVFS::CatFileInfo();
            }
            catsFound[key].catSig = f;

        } else if (datFilter.exactMatch(f)) {
            qDebug() << "Found dat file file :" << f << ".";
            int  key  = this->getNumberFromStr(f);
            auto iter = catsFound.find(key);
            if (iter == catsFound.end()) {
                catsFound[key] = GameVFS::CatFileInfo();
            }
            catsFound[key].dat = f;

        } else if (datSigFilter.exactMatch(f)) {
            qDebug() << "Found signature file of dat file :" << f << ".";
            int  key  = this->getNumberFromStr(f);
            auto iter = catsFound.find(key);
            if (iter == catsFound.end()) {
                catsFound[key] = GameVFS::CatFileInfo();
            }
            catsFound[key].datSig = f;
        }
    }

    // Check result
    if (! execFound) {
        qDebug() << "Missing main program.";
        qDebug() << "Failed.";
        return false;
    }

    if (catsFound.size() < MIN_CAT_FILE_NUM) {
        qDebug() << "Number of cat files si not enough.";
        qDebug() << "Failed.";
        return false;
    }

    for (int i = 1; i < catsFound.size() + 1; i++) {
        if (catsFound[i].cat == "" || catsFound[i].catSig == ""
            || catsFound[i].dat == "" || catsFound[i].datSig == "") {
            qDebug() << "Failed.";
            return false;
        }
    }

    qDebug() << "OK.";
    catFiles = ::std::move(catsFound);
    return true;
}

/**
 * @brief		Ask game path.
 *
 * @return		True if the path of game is selected, otherwise returns
 *				false.
 *
 */
bool GameData::askGamePath()
{
    QFileDialog fileDialog(nullptr, STR("STR_TITLE_SELECT_GAME_PATH"),
                           m_gamePath, "*");
    fileDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::FileMode::Directory);
    fileDialog.setFilter(QDir::Filter::Dirs | QDir::Filter::Hidden
                         | QDir::Filter::System);
    if (fileDialog.exec() != QDialog::DialogCode::Accepted
        || fileDialog.selectedFiles().empty()) {
        return false;
    }
    QString str = QDir(fileDialog.selectedFiles()[0]).absolutePath();
    qDebug() << "Selected:" << str;

    m_gamePath = str;
    Config::instance()->setString("/gamePath", m_gamePath);

    return true;
}

/**
 * @brief		Get number from string.
 *
 * @param[in]	str		String to get number.
 *
 * @return		Number got.
 */
int GameData::getNumberFromStr(const QString &str)
{
    int ret = 0;
    for (auto &c : str) {
        if (c >= '0' && c <= '9') {
            ret = ret * 10 + (c.toLatin1() - '0');
        } else {
            break;
        }
    }

    return ret;
}
