#pragma once

#include <QProcess>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include <QDebug>
#include <QRegExp>
#include <math.h>

namespace QSMG {

//!
//! \brief The Util class
//!
class Util
{
public:
    explicit Util()
    {
    }

    virtual ~Util()
    {
    }

    //!
    //! \brief getAlpha
    //! \param v
    //! \return
    //!
    static const QVariant getAlpha(const QVariant&v)
    {
        auto num=QStringLiteral("0123456789,.");
        QString r;
        auto ss=v.toString();
        for(auto&v:ss){
            if(!num.contains(v))
                r+=v;
        }
        return r;
    }

    //!
    //! \brief getNumber
    //! \param v
    //! \return
    //!
    static const QVariant getNumber(const QVariant&v)
    {
        auto num=QStringLiteral("0123456789,.");
        QString r,ss;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        auto typeId=int(v.typeId());
#else
        auto typeId=int(v.type());
#endif
        switch (typeId) {
        case QMetaType::Double:
            ss=QString::number(v.toDouble(),'f',11);
            break;
        default:
            if(v.toLongLong()>0)
                ss=QString::number(v.toLongLong(),'f',0);
            else
                ss=v.toString();
            break;
        }


        for(auto&c:ss){
            if(num.contains(c))
                r+=c;
        }
        return r;
    }

    //!
    //! \brief getMemoryBytes
    //! \param v
    //! \param defaultV
    //! \return
    //!
    static const QVariant getMemoryBytes(const QVariant&v, const QVariant&defaultV=QVariant())
    {
        if(v.isNull() || !v.isValid() || v.toDouble()<0)
            return defaultV;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        auto typeId=int(v.typeId());
#else
        auto typeId=int(v.type());
#endif
        switch (typeId) {
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::Double:
            return v;
        default:
            break;
        }

        qlonglong scale=1;

        auto a=getAlpha(v).toString().toLower();
        if(a==QStringLiteral("kb"))
            scale=1;
        else if(a==QStringLiteral("mb"))
            scale=2;
        else if(a==QStringLiteral("gb"))
            scale=3;
        else if(a==QStringLiteral("tb"))
            scale=4;
        else if(a==QStringLiteral("pb"))
            scale=5;
        else if(a==QStringLiteral("eb"))
            scale=6;
        else if(a==QStringLiteral("zb"))
            scale=7;
        else if(a==QStringLiteral("yb"))
            scale=8;
        else
            scale=1;//ms

        auto iN=getNumber(v);
        auto i=iN.toLongLong();
        i*=pow(1024, scale);
        i/=1000;
        if(i<=0)
            i=getMemoryBytes(defaultV).toLongLong();
        return i;
    }

    //!
    //! \brief getCPU
    //! \param v
    //! \return
    //!
    static const QVariant getCPU(const QVariant&v)
    {
        auto __return=v.toString().replace(QStringLiteral("%"),"").toDouble();
        return __return;
    }

    //!
    //! \brief getInterval
    //! \param v
    //! \param defaultV
    //! \return
    //!
    static const QVariant getInterval(const QVariant&v, const QVariant&defaultV=QVariant())
    {
        if(v.isNull() || !v.isValid() || v.toDouble()<0)
            return defaultV;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        auto typeId=int(v.typeId());
#else
        auto typeId=int(v.type());
#endif
        switch (typeId) {
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::Double:
            return v;
        default:
            break;
        }

        qlonglong scale=1;
        auto a=getAlpha(v).toString().toLower();
        if(a==QStringLiteral("s") || a==QStringLiteral("sc") || a==QStringLiteral("second"))
            scale=1;
        else if(a==QStringLiteral("m") || a==QStringLiteral("mn") || a==QStringLiteral("minute"))
            scale=60;
        else if(a==QStringLiteral("h") || a==QStringLiteral("hr") || a==QStringLiteral("hour"))
            scale=60*60;
        else if(a==QStringLiteral("d") || a==QStringLiteral("dd") || a==QStringLiteral("day"))
            scale=60*60*24;
        else if(a==QStringLiteral("mo")|| a==QStringLiteral("mo") || a==QStringLiteral("month"))
            scale=(60*60*24*30);
        else if(a==QStringLiteral("y") || a==QStringLiteral("yy") || a==QStringLiteral("year"))
            scale=(60*60*12*365);
        else
            scale=1;//ms
        scale*=1000;
        auto iN=getNumber(v);
        auto i=iN.toDouble();
        i*=scale;
        if(i<=0)
            i=getInterval(defaultV).toLongLong();
        return i;
    }
};

}
