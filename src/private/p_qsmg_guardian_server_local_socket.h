#pragma once

#include "./p_qsmg_guardian_thread.h"
#include <QLocalServer>
#include <QLocalSocket>
#include <QTimer>

namespace QSMG {

class ServerLocalSocket:public GuardianThread
{
    Q_OBJECT
public:
    QLocalServer* server=nullptr;
    QString port;
    QHash<QLocalSocket*, QByteArray> clientsidentifier;

    //!
    //! \brief Server
    //! \param port
    //!
    explicit ServerLocalSocket(const QString&port);

    //!
    //! \brief run
    //!
    void run()override;

public slots:

    //!
    //! \brief onServerStart
    //!
    void onServerStart();

    //!
    //! \brief onServerNewConnection
    //!
    void onServerNewConnection();

    //!
    //! \brief onClientReadyRead
    //!
    void onClientReadyRead();

    //!
    //! \brief onClientDisconnected
    //!
    void onClientDisconnected();

signals:
    void dataReceived(const QByteArray identify, const QByteArray bytes);
};


}


