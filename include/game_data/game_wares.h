#pragma once

#include <array>
#include <functional>
#include <memory>

#include <QtCore/QMap>
#include <QtCore/QMetaEnum>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QXmlStreamReader>

#include <common.h>
#include <game_data/game_texts.h>
#include <interfaces/i_load_factory_func.h>
#include <locale/string_table.h>

class GameVFS;
class GameData;

/**
 * @brief	Wares in game.
 */
class GameWares :
    virtual public QObject,
    public ILoadFactoryFunc<GameWares,
                            ::std::shared_ptr<GameVFS>,
                            ::std::shared_ptr<GameTexts>,
                            ::std::function<void(const QString &)>> {
    LOAD_FUNC(GameWares,
              ::std::shared_ptr<GameVFS>,
              ::std::shared_ptr<GameTexts>,
              ::std::function<void(const QString &)>);

    Q_OBJECT

  public:
    /**
     * @brief	Information of ware group.
     */
    struct WareGroup {
        QString           id;   ///< Ware ID.
        GameTexts::IDPair name; ///< Name.
        QStringList       tags; ///< Tags.
    };

    /**
     * @brief	Resource.
     */
    struct Resource {
        QString id;     ///< Ware ID.
        quint32 amount; ///< Amount.
    };

    /**
     * @brief	Production information.
     */
    struct ProductionInfo {
        QString id;         ///< Ware ID.
        quint32 time;       ///< Time per round(s).
        quint32 amount;     ///< Amount per round.
        QString method;     ///< Method.
        double  workEffect; ///< Effect of works.
        QMap<QString, ::std::shared_ptr<Resource>> resources; ///< Resource.
    };

    /**
     * @brief	Transport type.
     */
    enum class TransportType {
        Container, ///< Container.
        Solid,     ///< Solid.
        Liquid,    ///< Liquid.
        Unknow     ///< Unknow type.
    };
    Q_ENUM(TransportType);

    /**
     * @brief	Information of ware.
     */
    struct Ware {
        QString           id;            ///< Ware ID.
        GameTexts::IDPair name;          ///< Name.
        GameTexts::IDPair description;   ///< Description.
        QString           group;         ///< Group.
        TransportType     transportType; ///< Transport type.
        quint32           volume;        ///< Volume(m^3).
        QStringList       tags;          ///< Tags.
        quint32           minPrice;      ///< Minimun price(CR).
        quint32           averagePrice;  ///< Average price(CR).
        quint32           maxPrice;      ///< Maxium price(CR).
        QMap<QString, ::std::shared_ptr<ProductionInfo>>
            productionInfos; ///< Production informations.
    };

  private:
    QMap<QString, ::std::shared_ptr<WareGroup>> m_wareGroups; ///< Ware groups.
    QMap<QString, ::std::shared_ptr<Ware>>      m_wares;      ///< Wares.
    QAtomicInt m_unknowWareIndex;      ///< Unknow ware index.
    QAtomicInt m_unknowWareGroupIndex; ///< Unknow ware group index.

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	vfs				Virtual filesystem of the game.
     * @param[in]	texts			Game texts.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameWares(::std::shared_ptr<GameVFS>             vfs,
              ::std::shared_ptr<GameTexts>           texts,
              ::std::function<void(const QString &)> setTextFunc);

  public:
    /**
     * @brief	Get ware group information.
     *
     * @param[in]   id              Ware group ID.
     * @param[in]	texts			Game texts.
     *
     * @return	Information of ware group.
     */
    ::std::shared_ptr<WareGroup> wareGroup(const QString &              id,
                                           ::std::shared_ptr<GameTexts> texts
                                           = nullptr);

    /**
     * @brief	Get ware information.
     *
     * @param[in]   id              Ware ID.
     * @param[in]	texts			Game texts.
     *
     * @return	Information of ware.
     */
    ::std::shared_ptr<Ware> ware(const QString &              id,
                                 ::std::shared_ptr<GameTexts> texts = nullptr);

    /**
     * @brief		Destructor.
     */
    virtual ~GameWares();
};
