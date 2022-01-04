#pragma once

#include <QThread>

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


}


