#pragma once

#include <QProcess>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include <QDebug>
#include <QFile>
#include <QTime>

namespace QSMG {

class GuardianThread: public QThread
{
    Q_OBJECT
public:
    explicit GuardianThread():QThread(nullptr)
    {
        this->moveToThread(this);
    }

    void run() override
    {
        this->exec();
    }

    void start()
    {
        if(this->isRunning())
            return;
        QThread::start();
        while(this->eventDispatcher()==nullptr)
            QThread::msleep(1);
    }
};

class GuardianProcess: public GuardianThread
{
    Q_OBJECT
public:
    QProcess*process=nullptr;

    explicit GuardianProcess():GuardianThread()
    {
        this->moveToThread(this);
    }

    ~GuardianProcess()
    {
        this->onProcessTerminate();
    }

    void run() override
    {
        this->process=new QProcess(this);

        //QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred)

        QObject::connect(this->process, &QProcess::started, [=](){this->qprocess_started();});
        QObject::connect(this->process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](int e, QProcess::ExitStatus s){ this->qprocess_finished(e, s);});
        QObject::connect(this->process, &QProcess::errorOccurred, [=](QProcess::ProcessError e){this->qprocess_errorOccurred(e);});
        QObject::connect(this->process, &QProcess::readyReadStandardOutput, [=](){this->qprocess_readyReadStandardOutput();});
        QObject::connect(this->process, &QProcess::readyReadStandardError, [=](){this->qprocess_readyReadStandardError();});

        QObject::connect(this, &GuardianProcess::processStart, this, &GuardianProcess::onProcessStart);
        GuardianThread::run();
        QObject::disconnect(this, &GuardianProcess::processStart, this, &GuardianProcess::onProcessStart);
        if(this->process!=nullptr){
            if(this->process->isOpen())
                this->process->terminate();
            this->process->terminate();
            this->process->deleteLater();
            this->process=nullptr;
        }
    }

signals:
    void processStart(const QVariantHash settings);
    void processStats(const QVariantHash stats);
    void processFinished();
    void processStandardOutput(const QByteArray output);
    void processStandardError(const QByteArray output);
public slots:
    void qprocess_started()
    {
        if(this->process==nullptr)
            return;
        qDebug()<<__PRETTY_FUNCTION__;
    }

    void qprocess_finished(int exitCode, QProcess::ExitStatus exitStatus)
    {
        if(this->process==nullptr)
            return;
        Q_UNUSED(exitCode)
        Q_UNUSED(exitStatus)
        qDebug()<<__PRETTY_FUNCTION__<<" "<<exitCode<<" "<<exitStatus;
    }

    void qprocess_errorOccurred(QProcess::ProcessError/* error*/)
    {
        this->onProcessTerminate();
    }

    void qprocess_readyReadStandardOutput()
    {
        if(this->process==nullptr)
            return;
        auto lines=this->process->readAllStandardOutput().split('\n');
        for(auto&line:lines){
            qDebug()<<line;
        }
    }

    void qprocess_readyReadStandardError()
    {
        if(this->process==nullptr)
            return;
        auto lines=this->process->readAllStandardError().split('\n');
        for(auto&line:lines){
            qDebug()<<line;
        }
    }

public slots:
    void onProcessStart(const QVariantHash settings)
    {
        if(process==nullptr)
            return;

        auto vRunner=settings.value(QByteArrayLiteral("runner")).toHash();
        auto vProgram=vRunner.value(QByteArrayLiteral("program")).toByteArray();
        auto vArguments=vRunner.value(QByteArrayLiteral("arguments")).toStringList();

        process->start(vProgram, vArguments);
        if(!process->waitForStarted())
            this->onProcessTerminate();
    }

    void onProcessStats()
    {
        if(this->process==nullptr)
            return;

        QProcess process;

        qint64 pid=this->process->processId();

        if(pid<=0)
            return;

        static const QStringList staticParams={"-h", "-r", "-u", "-v", "-p"};

        QStringList params(staticParams);
        params<<QString::number(pid);

        process.start(QStringLiteral("pidstat"), params);
        if(!process.waitForStarted(1000)){
            qWarning()<<process.readAllStandardError();
            return;
        }

        if(!process.waitForFinished()){
            qWarning()<<process.readAllStandardError();
            process.terminate();
            return;
        }

        auto output=process.readAllStandardOutput().trimmed().split('\n');

        if(output.size()<4)
            return;

        auto line1=output[output.length()-2].simplified().toLower().split(' ');
        auto line2=output[output.length()-1].simplified().split(' ');

        if(line1.size()<2 || line2.size()<2)
            return;

        {//compact column 0 and 1
            line1[1]=line1[0]+line1[1];
            line2[1]=line2[0]+' '+line2[1];
            line1.removeAt(0);
            line2.removeAt(0);
        }

        static QStringList columnInt={"uid", "pid", "cpu", "vsz", "threads"};
        static QStringList columnDecimal={"%usr", "%system", "%guest", "%wait", "%cpu", "minflt/s", "majflt/s", "rss", "%mem"};
        static QStringList columnTime={"#time"};

        QVariantHash vResponse;
        while(line1.size()>0){
            auto key=line1.takeFirst();
            QVariant value;
            if(line1.isEmpty())
                value=line2.join(' ');
            else
                value=line2.takeFirst();
            if(columnInt.contains(key))
                value=value.toLongLong();
            if(columnDecimal.contains(key))
                value=value.toDouble();
            if(columnTime.contains(key))
                value=QTime::fromString(value.toString());
            vResponse[key]=value;
        }

        emit processStats(vResponse);
    }

    void onProcessTerminate()
    {
        if(this->process!=nullptr)
            process->terminate();

        if(this->isRunning()){
            emit processFinished();
            this->quit();
        }
    }
};

}


