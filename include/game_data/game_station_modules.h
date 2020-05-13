#pragma once

#include <functional>
#include <memory>

#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QVector>
#include <QtCore/QXmlStreamReader>

#include <common.h>
#include <interfaces/i_load_factory_func.h>
#include <locale/string_table.h>

#include <game_data/game_vfs.h>

#include <game_data/game_macros.h>
#include <game_data/game_texts.h>
#include <game_data/game_wares.h>

class GameVFS;

class GameMacros;
class GameTexts;
class GameWares;

/**
 * @brief	Station modules in game.
 */
class GameStationModules :
    public ILoadFactoryFunc<GameStationModules(
        ::std::shared_ptr<GameVFS>,
        ::std::shared_ptr<GameMacros>,
        ::std::shared_ptr<GameTexts>,
        ::std::shared_ptr<GameWares>,
        ::std::function<void(const QString &)>)> {
    LOAD_FUNC(GameStationModules,
              ::std::shared_ptr<GameVFS>,
              ::std::shared_ptr<GameMacros>,
              ::std::shared_ptr<GameTexts>,
              ::std::shared_ptr<GameWares>,
              ::std::function<void(const QString &)>);

  public:
    /**
     * @brief	Class of station module.
     */
    enum StationModuleClass {
        BuildModule,      ///< Build module.
        ConnectionModule, ///< Connection module.
        DefenceModule,    ///< Defence module.
        Dockarea,         ///< Dockarea.
        Habitation,       ///< Habitation,
        Production,       ///< Production,
        Storage           ///< Storage.
    };

    /**
     * @brief	Informations of station modules.
     */
    struct StationModule {
        QString            macro;           ///< Macro of the module.
        GameTexts::IDPair  name;            ///< Module name.
        StationModuleClass moduleClass;     ///< Class of station module.
        bool               playerModule;    ///< Player can build or not.
        GameTexts::IDPair  description;     ///< Module desctiption.
        QSet<QString>      races;           ///< Module races.
        quint32            hull;            ///< Hull.
        quint32            explosiondamage; ///< Explosion damage.
    };

    /**
     * @brief	Informations of build modules.
     */
    struct BuildModule : public StationModule {
        quint64 workforce; ///< Workforce.
    };

    /**
     * @brief	Informations of connection modules.
     */
    struct ConnectionModule : public StationModule {};

    /**
     * @brief	Informations of defence modules.
     */
    struct DefenceModule : public StationModule {};

    /**
     * @brief	Informations of dockarea modules.
     */
    struct DockareaModule : public StationModule {};

    /**
     * @brief	Informations of habitation modules.
     */
    struct HabitationModule : public StationModule {
        quint64 workforce; ///< Workforce.
        ::std::shared_ptr<GameWares::ProductionInfo>
            supplyInfo; ///< Supply informations.
    };

    /**
     * @brief	Informations of production modules.
     */
    struct ProductionModule : public StationModule {
        quint64 workforce; ///< Workforce.
        QString product;   ///< ID of the product.
        QVector<::std::shared_ptr<GameWares::ProductionInfo>>
            productionInfos; ///< Production informations.
    };

    /**
     * @brief	Informations of storage modules.
     */
    struct StorageModule : public StationModule {
        GameWares::TransportType cargoType; ///< Type of cargo.
        quint64                  cargoSize; ///< Cargo size(m^3).
    };

  private:
    QVector<::std::shared_ptr<StationModule>> m_modules; ///< Station modules.

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	vfs				Virtual filesystem of the game.
     * @param[in]	macros			Game macros.
     * @param[in]	texts			Game texts.
     * @param[in]	wares			Game wares.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameStationModules(::std::shared_ptr<GameVFS>             vfs,
                       ::std::shared_ptr<GameMacros>          macros,
                       ::std::shared_ptr<GameTexts>           texts,
                       ::std::shared_ptr<GameWares>           wares,
                       ::std::function<void(const QString &)> setTextFunc);

  public:
    /**
     * @brief		Get modules.
     *
     * @return		Modules.
     */
    const QVector<::std::shared_ptr<StationModule>> &modules() const;

    /**
     * @brief		Destructor.
     */
    virtual ~GameStationModules();

  private:
    /**
     * @brief		Start element callback in group.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	vfs			Virtual filesystem of the game.
     * @param[in]	macros		Game macros.
     * @param[in]	texts		Game texts.
     * @param[in]	wares		Game wares.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool
        onStartElementInRootOfModuleGroups(XMLLoader &         loader,
                                           XMLLoader::Context &context,
                                           const QString &     name,
                                           const QMap<QString, QString> &attr,
                                           ::std::shared_ptr<GameVFS>    vfs,
                                           ::std::shared_ptr<GameMacros> macros,
                                           ::std::shared_ptr<GameTexts>  texts,
                                           ::std::shared_ptr<GameWares>  wares);

    /**
     * @brief		Start element callback in groups.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	vfs			Virtual filesystem of the game.
     * @param[in]	macros		Game macros.
     * @param[in]	texts		Game texts.
     * @param[in]	wares		Game wares.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInGroupsOfModuleGroups(
        XMLLoader &                   loader,
        XMLLoader::Context &          context,
        const QString &               name,
        const QMap<QString, QString> &attr,
        ::std::shared_ptr<GameVFS>    vfs,
        ::std::shared_ptr<GameMacros> macros,
        ::std::shared_ptr<GameTexts>  texts,
        ::std::shared_ptr<GameWares>  wares);

    /**
     * @brief		Start element callback in group.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	vfs			Virtual filesystem of the game.
     * @param[in]	macros		Game macros.
     * @param[in]	texts		Game texts.
     * @param[in]	wares		Game wares.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInGroupOfModuleGroups(
        XMLLoader &                   loader,
        XMLLoader::Context &          context,
        const QString &               name,
        const QMap<QString, QString> &attr,
        ::std::shared_ptr<GameVFS>    vfs,
        ::std::shared_ptr<GameMacros> macros,
        ::std::shared_ptr<GameTexts>  texts,
        ::std::shared_ptr<GameWares>  wares);

    /**
     * @brief		Load macro.
     *
     * @param[in]	macro		Macro to load.
     * @param[in]	vfs			Virtual filesystem of the game.
     * @param[in]	macros		Game macros.
     * @param[in]	texts		Game texts.
     * @param[in]	wares		Game wares.
     */
    void loadMacro(const QString &               macro,
                   ::std::shared_ptr<GameVFS>    vfs,
                   ::std::shared_ptr<GameMacros> macros,
                   ::std::shared_ptr<GameTexts>  texts,
                   ::std::shared_ptr<GameWares>  wares);

    /**
     * @brief		Start element callback in macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	texts		Game texts.
     * @param[in]	wares		Game wares.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInRootOfMacro(XMLLoader &                   loader,
                                     XMLLoader::Context &          context,
                                     const QString &               name,
                                     const QMap<QString, QString> &attr,
                                     ::std::shared_ptr<GameTexts>  texts,
                                     ::std::shared_ptr<GameWares>  wares);

    /**
     * @brief		Start element callback in macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	texts		Game texts.
     * @param[in]	wares		Game wares.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInMacrosOfMacro(XMLLoader &                   loader,
                                       XMLLoader::Context &          context,
                                       const QString &               name,
                                       const QMap<QString, QString> &attr,
                                       ::std::shared_ptr<GameTexts>  texts,
                                       ::std::shared_ptr<GameWares>  wares);

    /**
     * @brief		Start element callback in macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	texts		Game texts.
     * @param[in]	wares		Game wares.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInMacroOfMacro(XMLLoader &                      loader,
                                      XMLLoader::Context &             context,
                                      const QString &                  name,
                                      const QMap<QString, QString> &   attr,
                                      ::std::shared_ptr<GameWares>     wares,
                                      ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Load common data in propterties.
     *
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool loadCommonPropertiesOfMacro(XMLLoader::Context *             context,
                                     const QString &                  name,
                                     const QMap<QString, QString> &   attr,
                                     ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in propterties.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInBuiildModulePropertiesOfMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in propterties.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInConnectionModulePropertiesOfMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in propterties.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInDefenceModulePropertiesOfMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in propterties.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInDockareaPropertiesOfMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in propterties.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	wares		Game wares.
     * @param[in]	module		Module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInHabitationPropertiesOfMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<GameWares>     wares,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in propterties.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	wares		Game wares.
     * @param[in]	module		Module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInProductionPropertiesOfMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<GameWares>     wares,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in propterties.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInStoragePropertiesOfMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in production.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Module.
     * @param[in]	wares		Game wares.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInProductionOfMacro(
        XMLLoader &                                loader,
        XMLLoader::Context &                       context,
        const QString &                            name,
        const QMap<QString, QString> &             attr,
        ::std::shared_ptr<struct ProductionModule> module,
        ::std::shared_ptr<GameWares>               wares);

    /**
     * @brief		Start element callback in build.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInBuildOfMacro(XMLLoader &                      loader,
                                      XMLLoader::Context &             context,
                                      const QString &                  name,
                                      const QMap<QString, QString> &   attr,
                                      ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in sets.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInSetsOfMacro(XMLLoader &                      loader,
                                     XMLLoader::Context &             context,
                                     const QString &                  name,
                                     const QMap<QString, QString> &   attr,
                                     ::std::shared_ptr<StationModule> module);
};
