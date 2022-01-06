#pragma once

#include "../qsmg_guardian.h"
#include "./p_qsmg_guardian_process.h"
#include "./p_qsmg_util.h"
#include "./p_qsmg_item_limits.h"

#include <QProcess>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include <QDebug>
#include <math.h>

namespace QSMG {

class GuardianPvt:public GuardianThread
{
    Q_OBJECT
public:
    ItemLimitCollection limits;
    QVariantHash settings;
    Guardian*parent=nullptr;
    GuardianProcess*process=nullptr;
    QTimer*processStats=nullptr;
    QTimer*processTimeOut=nullptr;
    QMutex mutexStart;

    explicit GuardianPvt(Guardian*parent):GuardianThread()
    {
        this->parent=parent;
        this->moveToThread(this);
    }

    virtual ~GuardianPvt()
    {
        this->wait();
        tasksFree();
    }

    void run()
    {
        QTimer::singleShot(5, this, [=](){this->taskStart();});
        this->exec();
        this->taskStop();
        if(!this->mutexStart.tryLock(100))
            this->mutexStart.unlock();
    }

    Guardian&start(const QVariantHash&vSettings=QVariantHash())
    {
        if(!this->mutexStart.tryLock(10)){
            qWarning()<<QStringLiteral("Guardian(%1) is running").arg(this->objectName());
            return*this->parent;
        }
        if(!vSettings.isEmpty())
            this->settings=vSettings;

        QVariantHash newSettings;
        QHashIterator<QString, QVariant> i(this->settings);
        while(i.hasNext()){
            i.next();
            newSettings[i.key().toLower()]=i.value();
        }
        this->settings=newSettings;

        auto vLimits=this->settings.value(QStringLiteral("limits")).toHash();
        this->limits=ItemLimitCollection(vLimits);

        GuardianThread::start();
        QThread::sleep(1);
        return*this->parent;
    }
public slots:

    void tasksFree()
    {
        auto timer=this->processTimeOut;
        this->processTimeOut=nullptr;
        if(timer!=nullptr){
            QObject::disconnect(timer, &QTimer::timeout, this, &GuardianPvt::taskStop);
            timer->stop();
            timer->deleteLater();
        }

        timer=this->processStats;
        this->processStats=nullptr;
        if(timer!=nullptr){
            QObject::disconnect(timer, &QTimer::timeout, this->process, &GuardianProcess::onProcessStats);
            timer->stop();
            timer->deleteLater();
        }


        auto process=this->process;
        this->process=nullptr;
        if(process!=nullptr){
            QObject::disconnect(process, &GuardianProcess::processStats, this, &GuardianPvt::onProcessStatsCheck);
            QObject::disconnect(process, &GuardianProcess::processFinished, this, &GuardianPvt::onProcessFinished);
            QObject::disconnect(process, &GuardianProcess::processStart, this->parent, &Guardian::processStart);
            QObject::disconnect(process, &GuardianProcess::processStats, this->parent, &Guardian::processStats);
            QObject::disconnect(process, &GuardianProcess::processFinished, this->parent, &Guardian::processFinished);
            QObject::disconnect(process, &GuardianProcess::processStandardOutput, this->parent, &Guardian::processStandardOutput);
            QObject::disconnect(process, &GuardianProcess::processStandardError, this->parent, &Guardian::processStandardError);
            process->quit();
            process->wait();
            process->deleteLater();
        }
    };

    void taskInit()
    {
        this->process=new GuardianProcess();
        QObject::connect(this->process, &GuardianProcess::processStats, this, &GuardianPvt::onProcessStatsCheck);
        QObject::connect(this->process, &GuardianProcess::processFinished, this, &GuardianPvt::onProcessFinished);
        QObject::connect(this->process, &GuardianProcess::processStart, this->parent, &Guardian::processStart);
        QObject::connect(this->process, &GuardianProcess::processStats, this->parent, &Guardian::processStats);
        QObject::connect(this->process, &GuardianProcess::processFinished, this->parent, &Guardian::processFinished);
        QObject::connect(this->process, &GuardianProcess::processStandardOutput, this->parent, &Guardian::processStandardOutput);
        QObject::connect(this->process, &GuardianProcess::processStandardError, this->parent, &Guardian::processStandardError);

        this->processTimeOut=new QTimer(nullptr);
        this->processTimeOut->setInterval(0);
        QObject::connect(this->processTimeOut, &QTimer::timeout, this, &GuardianPvt::taskStop);

        this->processStats=new QTimer(nullptr);
        this->processStats->setInterval(0);
        QObject::connect(this->processStats, &QTimer::timeout, this->process, &GuardianProcess::onProcessStats);

    };

    void taskStart()
    {
        this->taskInit();
        int statsInterval=15000;
        int runningInterval=0;

        auto vInterval=settings.value(QStringLiteral("interval")).toHash();
        if(!vInterval.isEmpty()){//if is valid
            auto vStats=vInterval.value(QStringLiteral("stats")).toHash();

            if(!vStats.isEmpty()){//if is valid
                if(vStats.contains(QStringLiteral("interval")))//if contanis
                    statsInterval=vStats.value(QStringLiteral("interval")).toInt();

                if(vStats.contains(QStringLiteral("enabled")) && !vStats.value(QStringLiteral("enabled")).toBool())//if contains enabled and enabled is false
                    statsInterval=0;
            }

            auto vRunning=vInterval.value(QStringLiteral("running")).toHash();
            if(!vRunning.isEmpty()){//if is valid
                if(vRunning.contains(QStringLiteral("interval")))//if contanis
                    runningInterval=vRunning.value(QStringLiteral("interval")).toInt();

                if(vRunning.contains(QStringLiteral("enabled")) && !vRunning.value(QStringLiteral("enabled")).toBool())//if contains enabled and enabled is false
                    runningInterval=0;
            }
        }

        this->processTimeOut->setInterval(runningInterval);
        if(this->processTimeOut->interval()>1000)
            this->processTimeOut->start();

        this->processStats->setInterval(statsInterval);
        if(this->processStats->interval()>1000)
            this->processStats->start();

        this->process->start();
        emit this->process->processStart(settings);
    }

    void taskStop()
    {
        if(this->processTimeOut!=nullptr && this->processTimeOut->isActive())
            this->processTimeOut->stop();

        if(this->processStats!=nullptr && this->processStats->isActive())
            this->processStats->stop();

        if(this->process!=nullptr){
            this->process->quit();
        }
        GuardianThread::quit();
    }

    void onProcessStatsCheck(const QVariantHash stats)
    {
        if(!this->limits.checkLimitExceeded(stats))
            return;
        this->taskStop();
    }

    void onProcessFinished()
    {
        this->taskStop();
        GuardianThread::quit();
    }

};

}


