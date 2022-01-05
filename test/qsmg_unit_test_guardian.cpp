#ifndef Q_SMG_GuardianTestFunctional_H
#define Q_SMG_GuardianTestFunctional_H

#include <QFile>
#include "./qsmg_test_functional.h"
#include "../src/qsmg_guardian.h"

namespace QSMG {

class Q_SMG_GuardianTestFunctional : public SDKGoogleTestFunctional {
public:
    const QString tmpBashFile=QStringLiteral("/tmp/%1.sh").arg(qAppName());
    vh vLimitsCPU=vh{{"%cpu", vh{{"value","30%"},{"condition",">"}}}};
    vh vLimitsMem=vh{{"rss", vh{{"value","6gb"},{"condition",">"}}}};
    vh vLimitsRuning=vh{{"#time", vh{{"value","10s"},{"condition",">"}}}};
    vh vIntervalFull=vh{{"running", vh{{"interval",5000}}}, {"stats", vh{{"interval",3000}}}};
    vh vIntervalRuning=vh{{"running", vh{{"interval",5000}}}};
    vh vIntervalStats=vh{{"stats", vh{{"interval",1500}}}};

    QString saveBash(const QStringList&lines)
    {
        QFile file(this->tmpBashFile);
        if(file.exists())
            file.remove();
        if(file.open(QFile::WriteOnly | QFile::Truncate)){
            QTextStream out(&file);
            for(auto&line:lines){
                out<<line<<";\n";
            }
            file.close();
            file.flush();
            file.close();
            file.setPermissions(file.fileName(),
                                QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner |
                                    QFile::ReadUser  | QFile::WriteUser  | QFile::ExeUser  |
                                    QFile::ReadGroup | QFile::WriteGroup | QFile::ExeGroup |
                                    QFile::ReadOther | QFile::WriteOther | QFile::ExeOther
                                );
        }
        return file.fileName();
    }

    QVariantHash makeRunner(const QVariantHash&vRunner)const
    {
        QVariantHash settings{{"interval",vIntervalStats}};
        settings["runner"]=vRunner;
        return settings;
    }

};

TEST_F(Q_SMG_GuardianTestFunctional, checkStartAndStop)
{
    Guardian guardian;
    auto bashFile=saveBash(sl{"#!/bin/bash","sleep 30"});
    auto settings=this->makeRunner(vh{{"program","/bin/bash"},{"arguments",bashFile}});
    settings["limits"]=QVariant();
    guardian.setSettings(settings);
    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
    guardian.start();
    EXPECT_TRUE(guardian.isRunning())<<"invalid state running";
    guardian.terminate();
    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
}

TEST_F(Q_SMG_GuardianTestFunctional, checkLimitRuning)
{
    Guardian guardian;
    auto bashFile=saveBash(sl{"#!/bin/bash","sleep 60"});
    auto settings=this->makeRunner(vh{{"program","/bin/bash"},{"arguments",bashFile}});
    settings["limits"]=vLimitsRuning;
    guardian.setSettings(settings);

    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
    guardian.start();
    EXPECT_TRUE(guardian.isRunning())<<"invalid state running";
    guardian.waitFinished();
    guardian.terminate();
    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
}

TEST_F(Q_SMG_GuardianTestFunctional, checkLimitCPU)
{
    Guardian guardian;
    auto bashFile=saveBash(sl{"#!/bin/bash \n echo {1..100000000}"});
    auto settings=this->makeRunner(vh{{"program","/bin/bash"},{"arguments", bashFile}});
    settings["limits"]=vLimitsCPU;
    guardian.setSettings(settings);

    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
    guardian.start();
    EXPECT_TRUE(guardian.isRunning())<<"invalid state running";
    guardian.waitFinished();
    guardian.terminate();
    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
}

TEST_F(Q_SMG_GuardianTestFunctional, checkLimitMemory)
{
    Guardian guardian;
    auto bashFile=saveBash(sl{"#!/bin/bash \n echo {1..100000000} > /dev/null"});
    auto settings=this->makeRunner(vh{{"program","/bin/bash"},{"arguments", bashFile}});
    settings["limits"]=vLimitsMem;
    guardian.setSettings(settings);

    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
    guardian.start();
    EXPECT_TRUE(guardian.isRunning())<<"invalid state running";
    guardian.waitFinished();
    guardian.terminate();
    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
}


TEST_F(Q_SMG_GuardianTestFunctional, cleanup)
{
    QFile file(this->tmpBashFile);
    if(file.exists())
        file.remove();
}

}


#endif // GuardianTestFunctional_H
