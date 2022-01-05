#include "./p_qsmg_guardian_server_local_socket.h"

namespace QSMG {

ServerLocalSocket::ServerLocalSocket(const QString &port): GuardianThread()
{
    this->port=port;
    this->moveToThread(this);
}

void ServerLocalSocket::run()
{
    this->server = new QLocalServer(this);
    QObject::connect(this->server, &QLocalServer::newConnection,this, &ServerLocalSocket::onServerNewConnection);

    //start server
    QTimer::singleShot(10, this, &ServerLocalSocket::onServerStart);

    this->exec();

    QHashIterator<QLocalSocket*, QByteArray> i(this->clientsidentifier);
    while(i.hasNext()){
        i.next();
        auto client=i.key();
        client->close();
        client->deleteLater();
    }

    if(server!=nullptr){
        QObject::disconnect(this->server, &QLocalServer::newConnection,this, &ServerLocalSocket::onServerNewConnection);
        server->disconnect();
        delete server;
        server=nullptr;
    }
}

void ServerLocalSocket::onServerStart()
{
    if(server==nullptr)
        return;

    if(server->isListening())
        return;

    if (!server->listen(port)) {
        qWarning()<<tr("Server: Cannot bind on port %1: %2").arg(port, server->errorString());
        this->quit();
        return;
    }

    if(!server->isListening()){
        qWarning()<<tr("Server: Cannot bind on port %1: %2").arg(port, server->errorString());
        this->quit();
        return;
    }
}

void ServerLocalSocket::onServerNewConnection()
{
    auto server=dynamic_cast<QLocalServer*>(QObject::sender());
    if(server==nullptr)
        return;

    auto socket = server->nextPendingConnection();
    QObject::connect(socket, &QLocalSocket::readyRead, this, &ServerLocalSocket::onClientReadyRead);
    QObject::connect(socket, &QLocalSocket::disconnected, this, &ServerLocalSocket::onClientDisconnected);
    if(socket!=nullptr){
        QByteArray identify;
        while(socket->waitForBytesWritten())
            identify+=socket->readAll();
        clientsidentifier[socket]=identify;
    }
}

void ServerLocalSocket::onClientReadyRead()
{
    auto socket = qobject_cast<QLocalSocket*>(QObject::sender());
    if(socket==nullptr)
        return;
    QByteArray bytes;
    while(socket->waitForBytesWritten())
        bytes+=socket->readAll();
    if(!bytes.isEmpty()){
        auto identifier=this->clientsidentifier.value(socket);
        emit dataReceived(identifier, bytes);
    }
}

void ServerLocalSocket::onClientDisconnected()
{
    auto socket = qobject_cast<QLocalSocket*>(QObject::sender());
    if (socket==nullptr)
        return;
    QObject::disconnect(socket, &QLocalSocket::readyRead, this, &ServerLocalSocket::onClientReadyRead);
    QObject::disconnect(socket, &QLocalSocket::disconnected, this, &ServerLocalSocket::onClientDisconnected);

    this->clientsidentifier.remove(socket);
    socket->close();
    socket->deleteLater();
}

}


