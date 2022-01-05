#include "./qsmg_server.h"

namespace QSMG {

#define dPvt()\
auto&p = *reinterpret_cast<QSMG::ServerPvt*>(this->p)

class ServerPvt{
public:
    Server*parent=nullptr;
    explicit ServerPvt(Server*parent)
    {
        this->parent=parent;
    }

    virtual ~ServerPvt()
    {
    }
};

Server::Server(QObject *parent):QObject(parent)
{
    this->p=new ServerPvt(this);
}

Server::~Server()
{
    dPvt();
    delete&p;
}

}

