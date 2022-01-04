#include "./private/p_qsmg_guardian.h"
#include <QProcess>

namespace QSMG {

#define dPvt()\
auto&p = *reinterpret_cast<QSMG::GuardianPvt*>(this->p)

Guardian::Guardian(QObject *parent):QObject(parent)
{
    this->p=new GuardianPvt(this);
}

Guardian::~Guardian()
{
    dPvt();
    delete&p;
}

const QVariantHash &Guardian::settings() const
{
    dPvt();
    return p.settings;
}

Guardian &Guardian::setSettings(const QVariantHash &v)
{
    dPvt();
    if (p.settings!=v){
        p.settings = v;
        emit settingsChanged();
    }
    return*this;
}

Guardian &Guardian::start()
{
    dPvt();
    return p.start();
}

Guardian &Guardian::start(const QByteArray &program, const QByteArray &arguments)
{
    dPvt();
    QVariantHash settings(p.settings);
    p.settings[QByteArrayLiteral("runner")]=QVariantHash{
        {QByteArrayLiteral("program"), program},
        {QByteArrayLiteral("arguments"), arguments}
    };
    return p.start(settings);
}

bool Guardian::isRunning()
{
    dPvt();
    return p.isRunning();
}

Guardian &Guardian::waitFinished(unsigned long time)
{
    dPvt();
    auto process=p.process;
    if(process!=nullptr){
        if(time>0)
            process->wait(time);
        else
            process->wait();
    }
    return*this;
}

Guardian &Guardian::terminate()
{
    dPvt();
    p.taskStop();
    p.wait();
    return*this;
}

}

