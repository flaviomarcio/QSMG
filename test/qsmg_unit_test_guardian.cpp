#ifndef Q_SMG_GuardianTestUnit_H
#define Q_SMG_GuardianTestUnit_H

#include <QFile>
#include "./qsmg_test_unit.h"
#include "../src/qsmg_guardian.h"

namespace QSMG {

class Q_SMG_GuardianTestUnit : public SDKGoogleTestUnit {
public:
    vh vLimits=vh{{"%cpu", vh{{"value","5%"},{"condition",">"}}},{"rss", vh{{"value","5m"},{"condition",">"}}}};
    vh vRunner=vh{{"program",qAppName()},{"arguments", sl{"-loadmemory"}}};
    vh vIntervalFull=vh{{"running", vh{{"interval",5000}}}, {"stats", vh{{"interval",3000}}}};
    vh vIntervalRuning=vh{{"running", vh{{"interval",5000}}}};
    vh vIntervalStats=vh{{"stats", vh{{"interval",3000}}}};

    QString makeFile500MB()
    {
        QFile file("/tmp/buffer.tmp");
        if(file.open(QFile::WriteOnly | QFile::Truncate)){
            qlonglong size=0;
            qlonglong sizeMax=1024*1000*500;
            while(size<=sizeMax){
                auto bytes=QUuid::createUuid().toByteArray();
                size+=bytes.length();
                file.write(bytes);
            }
            file.flush();
            file.close();
        }
        return file.fileName();
    }

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

TEST_F(Q_SMG_GuardianTestUnit, checkStartAndTerminate)
{
    Guardian guardian;
    QVariantHash settings;

    auto bashFile=saveBash(sl{"ls -l","sleep 30"});
    auto vRunner=vh{{"program","/bin/sh"},{"arguments",bashFile}};
    settings["runner"]=vRunner;
    settings["interval"]=vIntervalRuning;
    settings["limits"]=vLimits;
    guardian.setSettings(settings);

    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
    guardian.start();
    EXPECT_TRUE(guardian.isRunning())<<"invalid state running";
    guardian.waitFinished();
    guardian.terminate();
    EXPECT_FALSE(guardian.isRunning())<<"invalid state running";
}

TEST_F(Q_SMG_GuardianTestUnit, checkLoadCPU)
{
    Guardian guardian;
    QVariantHash settings;

    auto bashFile=saveBash(sl{"#!/bin/bash \n while : \n do \n echo \"infinite\" \n done"});
    auto vRunner=vh{{"program","/bin/sh"},{"arguments", bashFile}};

    settings["runner"]=vRunner;
    settings["interval"]=vIntervalStats;
    settings["limits"]=vLimits;
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

    auto file500MB=this->makeFile500MB();
    auto bashFile=saveBash(sl{QStringLiteral("export testCase=$(cat %1)").arg(file500MB),"ls -l","sleep 30"});
    auto vRunner=vh{{"program","/bin/sh"},{"arguments", bashFile}};

    settings["runner"]=vRunner;
    settings["interval"]=vIntervalStats;
    settings["limits"]=vLimits;
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
