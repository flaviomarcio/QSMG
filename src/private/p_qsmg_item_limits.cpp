#include "./p_qsmg_item_limits.h"
#include <QDateTime>

namespace QSMG {

QString ItemLimit::name() const
{
    return QVariantHash::value(QStringLiteral("name")).toString();
}

ItemLimit &ItemLimit::setName(const QString &value)
{
    this->insert(QStringLiteral("name"),value);
    return*this;
}

QString ItemLimit::condition() const
{
    return QVariantHash::value(QStringLiteral("condition")).toString();
}

ItemLimit &ItemLimit::setCondition(const QString &value)
{
    QVariantHash::insert(QStringLiteral("condition"),value);
    return*this;
}

const QVariant ItemLimit::value() const
{
    auto __return=QVariantHash::value(QStringLiteral("value"));

    if(this->name().contains(QStringLiteral("mem")))
        return Util::Util::getMemoryBytes(__return);

    if(this->name().contains(QStringLiteral("rss")))
        return Util::Util::getMemoryBytes(__return);

    if(this->name().contains(QStringLiteral("cpu")))
        return Util::Util::getCPU(__return);

    if(this->name().contains(QStringLiteral("time")))
        return Util::getInterval(__return);

    return {};
}

ItemLimit &ItemLimit::setValue(const QString &value)
{
    QVariantHash::insert(QStringLiteral("value"), value);
    return*this;
}

bool ItemLimit::checkLimitExceeded(const QVariantHash &stats)
{
    Util util;

    enum StatsConditions{Eq,EqNot,EqMinor,EqMajor,Minor,Major, In, NotIn};

    static auto statsConditionConvert=QHash<QString,StatsConditions>
        {
            {QStringLiteral("="),Eq},{QStringLiteral("=="),Eq},{QStringLiteral("==="),Eq},
            {QStringLiteral("!="),EqNot},{QStringLiteral("!=="),EqNot},{QStringLiteral("<>"),EqNot},
            {QStringLiteral("<"),Minor},{QStringLiteral(">"),Major},{QStringLiteral("<="),EqMinor},{QStringLiteral(">="),EqMajor},
            {QStringLiteral("in"),In},{QStringLiteral("!in"),NotIn},{QStringLiteral("out"),NotIn}
        };


    auto sCondition=this->condition();
    if(!statsConditionConvert.contains(sCondition))
        return false;

    auto stateCondition=statsConditionConvert.value(sCondition);

    bool __return=false;

    QHashIterator<QString, QVariant> i(stats);
    while(i.hasNext()){
        i.next();
        auto key=i.key();

        if(this->name()!=key)
            continue;

        QVariant valueCheck=i.value();
        QVariant valueLimit=this->value();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        auto typeId=int(valueCheck.typeId());
#else
        auto typeId=int(valueCheck.type());
#endif

        if(key.contains(QStringLiteral("#time"))){
            switch (typeId) {
            case QMetaType::QDateTime:
            {
                auto dt=valueCheck.toDateTime();
                valueCheck=QDateTime::currentDateTime().toMSecsSinceEpoch();
                valueLimit=dt.addMSecs(valueLimit.toLongLong()).toMSecsSinceEpoch();
                break;
            }
            default:
                break;
            }
        }

        switch (stateCondition) {
        case Eq:
            __return=(valueCheck.toString()==valueLimit.toString());
            break;
        case EqNot:
            __return=(valueCheck.toString()!=valueLimit.toString());
            break;
        case EqMinor:
            __return=(valueCheck.toDouble()<=valueLimit.toDouble());
            break;
        case EqMajor:
            __return=(valueCheck.toDouble()>=valueLimit.toDouble());
            break;
        case Minor:
            __return=(valueCheck.toDouble()<valueLimit.toDouble());
            break;
        case Major:
            __return=(valueCheck.toDouble()>valueLimit.toDouble());
            break;
        case In:
            __return=(valueCheck.toStringList().contains(valueLimit.toString()));
            break;
        case NotIn:
            __return=(!valueCheck.toStringList().contains(valueLimit.toString()));
            break;
        default:
            break;
        }

        if(__return){
            qInfo()<<QStringLiteral("limit exceeded in %1: %2 %3 %4").arg(this->name(), valueCheck.toString(), sCondition, valueLimit.toString());
            break;
        }
    }
    return __return;
}

}


