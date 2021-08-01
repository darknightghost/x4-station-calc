#pragma once

#include <functional>
#include <memory>

#include <QtCore/QMap>
#include <QtCore/QMetaEnum>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaType>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QVector>
#include <QtCore/QXmlStreamReader>

#include <common.h>
#include <game_data/game_wares.h>
#include <game_data/xml_loader/xml_loader.h>
#include <interfaces/i_load_factory_func.h>
#include <locale/string_table.h>

class GameVFS;
class GameData;
class GameMacros;
class GameTexts;
class GameWares;
class GameComponents;

/**
 * @brief	Station modules in game.
 */
class GameStationModules :
    public ILoadFactoryFunc<GameStationModules,
                            GameData *,
                            ::std::function<void(const QString &)>> {
    LOAD_FUNC(GameStationModules,
              GameData *,
              ::std::function<void(const QString &)>);

  public:
    /**
     * @brief	Informations of station module groups.
     */
    struct StationModuleGroup {
        QString macro; ///< Macro of the module group.
    };

    /**
     * @brief	Station module property.
     */
    struct Property {
        /**
         * @brief		Property type.
         */
        enum class Type {
            MTurret,          ///< Has M turret.
            MShield,          ///< Has M shield.
            LTurret,          ///< Has L turret.
            LShield,          ///< Has L shield.
            SDock,            ///< Has S docking bay.
            SShipCargo,       ///< Has S ship cargo.
            MDock,            ///< Has M docking bay.
            MShipCargo,       ///< Has M ship cargo.
            LDock,            ///< Has L docking bay.
            XLDock,           ///< Has XL docking bay.
            LXLDock,          ///< Has L/XL docking bay.
            SLaunchTube,      ///< Has S launch tube.
            MLaunchTube,      ///< Has M launch tube.
            SupplyWorkforce,  ///< Supply workforce.
            RequireWorkforce, ///< Require workforce.
            SupplyProduct,    ///< Supply product.
            Cargo,            ///< Has cargo.
        };
        const Type type; ///< Property type.

        /**
         * @brief		Constructor.
         *
         * @param[in]	propertyType	Property type.
         */
        Property(Type propertyType) : type(propertyType) {}

        /**
         * @brief		Desctructor.
         */
        virtual ~Property() {};
    };

    /**
     * @brief	Informations of station modules.
     */
    struct StationModule {
        /**
         * @brief	Class of station modules.
         */
        enum class StationModuleClass {
            Unknow,           ///< Unknow.
            BuildModule,      ///< Build module.
            ConnectionModule, ///< Connection module.
            DefenceModule,    ///< Defence module.
            Dockarea,         ///< Dockarea.
            Habitation,       ///< Habitation,
            Production,       ///< Production,
            Storage           ///< Storage.
        };

        QString            id;              ///< ID of the module.
        QString            group;           ///< Group of the module.
        QString            component;       ///< Component of the module.
        GameTexts::IDPair  name;            ///< Module name.
        StationModuleClass moduleClass;     ///< Class of station module.
        bool               playerModule;    ///< Player can build or not.
        GameTexts::IDPair  description;     ///< Module desctiption.
        QSet<QString>      races;           ///< Module race.
        quint32            hull;            ///< Hull.
        quint32            explosiondamage; ///< Explosion damage.
        QMap<Property::Type, ::std::shared_ptr<Property>>
            properties; ///< Properties.
    };

    /**
     * @brief     Has M turret.
     */
    struct HasMTurret : public Property {
        quint64 count; ///< Count of the turret.

        /**
         * @brief		Constructor.
         */
        HasMTurret() : Property(Property::Type::MTurret), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasMTurret() {}
    };

    /**
     * @brief     Has M shield.
     */
    struct HasMShield : public Property {
        quint64 count; ///< Count of the shield.

        /**
         * @brief		Constructor.
         */
        HasMShield() : Property(Property::Type::MShield), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasMShield() {}
    };

    /**
     * @brief     Has L turret.
     */
    struct HasLTurret : public Property {
        quint64 count; ///< Count of the turret.

        /**
         * @brief		Constructor.
         */
        HasLTurret() : Property(Property::Type::LTurret), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasLTurret() {}
    };

    /**
     * @brief     Has L shield.
     */
    struct HasLShield : public Property {
        quint64 count; ///< Count of the shield.

        /**
         * @brief		Constructor.
         */
        HasLShield() : Property(Property::Type::LShield), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasLShield() {}
    };

    /**
     * @brief     Has S docking bay.
     */
    struct HasSDock : public Property {
        quint64 count; ///< Count of the docking bay.

        /**
         * @brief		Constructor.
         */
        HasSDock() : Property(Property::Type::SDock), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasSDock() {}
    };

    /**
     * @brief     Has S ship capacity.
     */
    struct HasSShipCargo : public Property {
        quint64 capacity; ///< Ship capacity.

        /**
         * @brief		Constructor.
         */
        HasSShipCargo() : Property(Property::Type::SShipCargo), capacity(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasSShipCargo() {}
    };

    /**
     * @brief     Has M docking bay.
     */
    struct HasMDock : public Property {
        quint64 count; ///< Count of the docking bay.

        /**
         * @brief		Constructor.
         */
        HasMDock() : Property(Property::Type::MDock), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasMDock() {}
    };

    /**
     * @brief     Has M ship capacity.
     */
    struct HasMShipCargo : public Property {
        quint64 capacity; ///< Ship capacity.

        /**
         * @brief		Constructor.
         */
        HasMShipCargo() : Property(Property::Type::MShipCargo), capacity(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasMShipCargo() {}
    };

    /**
     * @brief     Has L docking bay.
     */
    struct HasLDock : public Property {
        quint64 count; ///< Count of the L docking bay.

        /**
         * @brief		Constructor.
         */
        HasLDock() : Property(Property::Type::LDock), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasLDock() {}
    };

    /**
     * @brief     Has XL docking bay.
     */
    struct HasXLDock : public Property {
        quint64 count; ///< Count of the XL docking bay.

        /**
         * @brief		Constructor.
         */
        HasXLDock() : Property(Property::Type::XLDock), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasXLDock() {}
    };

    /**
     * @brief     Has L/XL docking bay.
     */
    struct HasLXLDock : public Property {
        quint64 count; ///< Count of the L/XL docking bay.

        /**
         * @brief		Constructor.
         */
        HasLXLDock() : Property(Property::Type::LXLDock), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasLXLDock() {}
    };

    /**
     * @brief     Has S launch tube.
     */
    struct HasSLaunchTube : public Property {
        quint64 count; ///< Count of the S launch tube.

        /**
         * @brief		Constructor.
         */
        HasSLaunchTube() : Property(Property::Type::SLaunchTube), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasSLaunchTube() {}
    };

    /**
     * @brief     Has M launch tube.
     */
    struct HasMLaunchTube : public Property {
        quint64 count; ///< Count of the M launch tube.

        /**
         * @brief		Constructor.
         */
        HasMLaunchTube() : Property(Property::Type::MLaunchTube), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasMLaunchTube() {}
    };

    /**
     * @brief     Support workforce.
     */
    struct SupplyWorkforce : public Property {
        quint64 workforce; ///< Workforce can supply.
        ::std::shared_ptr<GameWares::ProductionInfo>
            supplyInfo; ///< Supply informations.

        /**
         * @brief		Constructor.
         */
        SupplyWorkforce() :
            Property(Property::Type::SupplyWorkforce), workforce(0)
        {}

        /**
         * @brief		Destructor.
         */
        virtual ~SupplyWorkforce() {}
    };

    /**
     * @brief     Require workforce.
     */
    struct RequireWorkforce : public Property {
        quint64 workforce; ///< Workforce required.

        /**
         * @brief		Constructor.
         */
        RequireWorkforce() :
            Property(Property::Type::RequireWorkforce), workforce(0)
        {}

        /**
         * @brief		Destructor.
         */
        virtual ~RequireWorkforce() {}
    };

    /**
     * @brief     Supply product.
     */
    struct SupplyProduct : public Property {
        QString product; ///< ID of the product.
        ::std::shared_ptr<GameWares::ProductionInfo>
            productionInfo; ///< Production information.

        /**
         * @brief		Constructor.
         */
        SupplyProduct() : Property(Property::Type::SupplyProduct) {}

        /**
         * @brief		Destructor.
         */
        virtual ~SupplyProduct() {}
    };

    /**
     * @brief     Has cargo.
     */
    struct HasCargo : public Property {
        GameWares::TransportType cargoType; ///< Type of cargo.
        quint64                  cargoSize; ///< Cargo size(m^3).

        /**
         * @brief		Constructor.
         */
        HasCargo() :
            Property(Property::Type::Cargo),
            cargoType(GameWares::TransportType::Unknow), cargoSize(0)
        {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasCargo() {}
    };

    /**
     * @brief	Temporart docking bay information.
     */
    struct TmpDockingBayInfo {
        quint64 count;                   ///< Count of the docking bay.
        quint64 capacity;                ///< Ship capacity.
        enum { S, M, L, XL, L_XL } type; ///< Type.
    };

  private:
    QVector<::std::shared_ptr<StationModule>> m_modules; ///< Station modules.
    QMap<QString, ::std::shared_ptr<StationModuleGroup>>
        m_moduleGroups; ///< Station module groups.
    QMap<QString, ::std::shared_ptr<StationModule>>
        m_modulesIndex; ///< Station modules index.
    QMap<QString, QVector<::std::shared_ptr<StationModule>>>
        m_componentTmpIndex; ///< Temporart component index.
    static QMap<QString, StationModule::StationModuleClass>
        _classMap; ///< Station module class map.

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	gameData        Game data.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameStationModules(GameData *                             gameData,
                       ::std::function<void(const QString &)> setTextFunc);

  public:
    /**
     * @brief		Get modules.
     *
     * @return		Modules.
     */
    const QVector<::std::shared_ptr<StationModule>> &modules() const;

    /**
     * @brief		Get modules.
     *
     * @return		Modules.
     */
    ::std::shared_ptr<StationModule> module(const QString &macro);

    /**
     * @brief		Destructor.
     */
    virtual ~GameStationModules();

  private:
    /**
     * @brief       Create XML loader of module groups.
     *
     * @return      XML loader.
     */
    ::std::unique_ptr<XMLLoader> createModuleGroupsXMLLoader();

    /**
     * @brief       Create XML loader of module groups.
     *
     * @param[in]	gameData        Game data.
     *
     * @return      XML loader.
     */
    ::std::unique_ptr<XMLLoader> createModulesXMLLoader(GameData *gameData);

    /**
     * @brief		Load macro.
     *
     * @param[in]	macro		Macro to load.
     * @param[in]	module      Station module.
     * @param[in]	gameData    Game data.
     */
    void loadMacro(const QString &                  macro,
                   ::std::shared_ptr<StationModule> module,
                   GameData *                       gameData);

    /**
     * @brief       Create XML loader of macros.
     *
     * @param[in]	module      Station module.
     * @param[in]	gameData        Game data.
     *
     * @return      XML loader.
     */
    ::std::unique_ptr<XMLLoader>
        createMacroXMLLoader(::std::shared_ptr<StationModule> module,
                             GameData *                       gameData);

    /**
     * @brief		Load component.
     *
     * @param[in]	component	Component to load.
     * @param[in]	module		Station module.
     * @param[in]	macroLoader XML loader of macro.
     * @param[in]	gameData    Game data.
     */
    void loadComponent(const QString &                  component,
                       ::std::shared_ptr<StationModule> module,
                       XMLLoader &                      macroLoader,
                       GameData *                       gameData);
    /**
     * @brief       Create XML loader of components.
     *
     * @param[in]	module		Station module.
     * @param[in]	macroLoader XML loader of macro.
     *
     * @return      XML loader.
     */
    ::std::unique_ptr<XMLLoader>
        createComponentXMLLoader(::std::shared_ptr<StationModule> module,
                                 XMLLoader &                      macroLoader);

    /**
     * @brief		Load macro in connections of module macro.
     *
     * @param[in]	macro		Maro to load.
     * @param[in]	module		Station module.
     * @param[in]	gameData    Game data.
     */
    void loadConnectionMacro(const QString &                  macro,
                             ::std::shared_ptr<StationModule> module,
                             GameData *                       gameData);

    /**
     * @brief       Create XML loader of connection macros.
     *
     * @param[in]	module		Station module.
     * @param[in]	gameData        Game data.
     *
     * @return      XML loader.
     */
    ::std::unique_ptr<XMLLoader>
        createConnectionMacroXMLLoader(::std::shared_ptr<StationModule> module,
                                       GameData *gameData);
};
