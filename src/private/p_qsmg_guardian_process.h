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
    QDateTime started;

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
        this->started=QDateTime();

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

    //!
    //! \brief qprocess_started
    //!
    //! slots task in QProcess
    void qprocess_started()
    {
        if(this->process==nullptr)
            return;
    }

    //!
    //! \brief qprocess_finished
    //! \param exitCode
    //! \param exitStatus
    //!
    //! slots task in QProcess
    void qprocess_finished(int exitCode, QProcess::ExitStatus exitStatus)
    {
        if(this->process==nullptr || !this->process->isOpen())
            return;
        this->onProcessTerminate();
        Q_UNUSED(exitCode)
        Q_UNUSED(exitStatus)
    }

    //!
    //! \brief qprocess_errorOccurred
    //!
    //! slots task in QProcess
    void qprocess_errorOccurred(QProcess::ProcessError/* error*/)
    {
        this->onProcessTerminate();
    }

    //!
    //! \brief qprocess_readyReadStandardOutput
    //!
    //! slots task in QProcess
    void qprocess_readyReadStandardOutput()
    {
        if(this->process==nullptr || !this->process->isOpen())
            return;
        auto lines=this->process->readAllStandardOutput().split('\n');
        for(auto&line:lines){
            qInfo()<<line;
        }
    }

    //!
    //! \brief qprocess_readyReadStandardError
    //! slots task in QProcess
    void qprocess_readyReadStandardError()
    {
        if(this->process==nullptr || !this->process->isOpen())
            return;
        auto lines=this->process->readAllStandardError().split('\n');
        for(auto&line:lines){
            qInfo()<<line;
        }
    }

public slots:

    //!
    //! \brief onProcessStart
    //! \param settings
    //!
    void onProcessStart(const QVariantHash settings)
    {
        if(process==nullptr)
            return;

        this->started=QDateTime();

        auto vRunner=settings.value(QByteArrayLiteral("runner")).toHash();
        auto vProgram=vRunner.value(QByteArrayLiteral("program")).toByteArray();
        auto vArguments=vRunner.value(QByteArrayLiteral("arguments")).toStringList();

        process->start(vProgram, vArguments);
        if(!process->waitForStarted())
            this->onProcessTerminate();
        else
            started=QDateTime::currentDateTime();
    }

    void onProcessStats()
    {
        if(this->process==nullptr)
            return;

        if(!this->process->isOpen())
            return;

        qint64 pid=this->process->processId();

        if(pid<=0){
            qWarning()<<QStringLiteral("invalid pid for %1 %2").arg(this->process->program(), this->process->arguments().join(' '));
            return;
        }

        static const QStringList staticParams={"-h", "-r", "-u", "-v", "-r", "1", "-p"};

        auto params=QStringList(staticParams)<<QString::number(pid);

        QProcess process;
        process.start(QStringLiteral("pidstat"), params);
        if(!process.waitForStarted(1000)){
            qWarning()<<process.readAllStandardError();
            return;
        }

        QList<QByteArray> output;
        if(!process.waitForFinished(1500)){
            if(!process.isOpen()){
                qWarning()<<process.readAllStandardError();
            }
            else{
                output=process.readAllStandardOutput().trimmed().split('\n');
                process.close();
            }
        }
        else{
            output=process.readAllStandardOutput().trimmed().split('\n');
        }

        if(output.size()<2)
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
                value=this->started;
            vResponse[key]=value;
        }

        emit processStats(vResponse);
    }

    void onProcessTerminate()
    {
        if(this->isRunning())
            this->quit();
        emit processFinished();
    }
};

}


