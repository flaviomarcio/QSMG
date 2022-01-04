#ifndef Q_SMG_GuardianTestUnit_H
#define Q_SMG_GuardianTestUnit_H

#include <QFile>
#include "./qsmg_test_unit.h"
#include "../src/qsmg_guardian.h"

namespace QSMG {

class Q_SMG_GuardianTestUnit : public SDKGoogleTestUnit {
public:
    vh vLimitsCPU=vh{{"%cpu", vh{{"value","30%"},{"condition",">"}}}};
    vh vLimitsMem=vh{{"rss", vh{{"value","1gb"},{"condition",">"}}}};
    vh vIntervalFull=vh{{"running", vh{{"interval",5000}}}, {"stats", vh{{"interval",3000}}}};
    vh vIntervalRuning=vh{{"running", vh{{"interval",5000}}}};
    vh vIntervalStats=vh{{"stats", vh{{"interval",3000}}}};

    QString saveBash(const QStringList&lines)
    {
        QFile file(QStringLiteral("/tmp/%1.sh").arg(qAppName()));
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

};

//TEST_F(Q_SMG_GuardianTestUnit, checkStartAndTerminate)
//{
//    Guardian guardian;
//    QVariantHash settings;

//    auto bashFile=saveBash(sl{"ls -l > /dev/null","sleep 30"});
//    auto vRunner=vh{{"program","/bin/sh"},{"arguments",bashFile}};
//    settings["runner"]=vRunner;
//    settings["interval"]=vIntervalRuning;
//    settings["limits"]=QVariant();
//    guardian.setSettings(settings);

//    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
//    guardian.start();
//    EXPECT_TRUE(guardian.isRunning())<<"invalid state running";
//    guardian.waitFinished();
//    guardian.terminate();
//    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
//}

TEST_F(Q_SMG_GuardianTestUnit, checkLoadCPU)
{
    Guardian guardian;
    QVariantHash settings;

    auto bashFile=saveBash(sl{"#!/bin/bash \n echo {1..100000000}"});
    auto vRunner=vh{{"program","/bin/bash"},{"arguments", bashFile}};

    settings["runner"]=vRunner;
    settings["interval"]=vIntervalStats;
    settings["limits"]=vLimitsCPU;
    guardian.setSettings(settings);

    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
    guardian.start();
    EXPECT_TRUE(guardian.isRunning())<<"invalid state running";
    guardian.waitFinished();
    guardian.terminate();
    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
}

TEST_F(Q_SMG_GuardianTestUnit, checkLoadMemory)
{
    Guardian guardian;
    QVariantHash settings;

    auto bashFile=saveBash(sl{"#!/bin/bash \n echo {1..100000000} > /dev/null"});
    auto vRunner=vh{{"program","/bin/bash"},{"arguments", bashFile}};

    settings["runner"]=vRunner;
    settings["interval"]=vIntervalStats;
    settings["limits"]=vLimitsMem;
    guardian.setSettings(settings);

    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
    guardian.start();
    EXPECT_TRUE(guardian.isRunning())<<"invalid state running";
    guardian.waitFinished();
    guardian.terminate();
    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
}

}


#endif // GuardianTestUnit_H
