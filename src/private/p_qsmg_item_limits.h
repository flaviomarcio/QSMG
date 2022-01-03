#pragma once

#include "./p_qsmg_util.h"

#include <QProcess>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include <QDebug>
#include <QRegExp>
#include <math.h>

namespace QSMG {

//!
//! \brief The ItemLimit class
//!
class ItemLimit:public QVariantHash
{
    Q_GADGET
public:

    //!
    //! \brief ItemLimit
    //! \param itemLimit
    //!
    explicit ItemLimit(const QVariant&itemLimit):QVariantHash(itemLimit.toHash())
    {
    }

    //!
    //! \brief ~ItemLimit
    //!
    virtual ~ItemLimit()
    {
    }

    //!
    //! \brief name
    //! \return
    //!
    QString name() const;
    ItemLimit&setName(const QString &value);

    //!
    //! \brief condition
    //! \return
    //!
    QString condition() const;
    ItemLimit&setCondition(const QString &value);

    //!
    //! \brief value
    //! \return
    //!
    const QVariant value() const;
    ItemLimit&setValue(const QString &value);

    //!
    //! \brief checkLimitExceeded
    //! \param stats
    //! \return
    //!
    bool checkLimitExceeded(const QVariantHash &stats);
};


class ItemLimitCollection:QVariantList{
public:
    explicit ItemLimitCollection(const QVariantHash&settings=QVariantHash()):QVariantList()
    {
        QHashIterator<QString, QVariant> i(settings);
        while(i.hasNext()){
            i.next();
            auto vItem=i.value().toHash();
            vItem[QStringLiteral("name")]=i.key();
            this->append(vItem);
        }
    }

    virtual ~ItemLimitCollection()
    {
    }

    //!
    //! \brief checkLimitExceeded
    //! \param stats
    //! \return
    //!
    bool checkLimitExceeded(const QVariantHash &stats)
    {
        if(!this->isEmpty()){
            for(auto&v:*this){
                ItemLimit item(v);

                auto statValue=stats.value(item.name());
                if(!statValue.isValid() || statValue.isNull())
                    continue;

                auto vStat=QVariantHash{{item.name(), statValue}};
                if(item.checkLimitExceeded(vStat))
                    return true;
            }
        }
        return false;
    }
};

}


