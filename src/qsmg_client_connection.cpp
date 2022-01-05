#include "./qsmg_client_connection.h"

#define dPvt()\
auto&p = *reinterpret_cast<QSMG::ClientConnectionPvt*>(this->p)

namespace QSMG {

class ClientConnectionPvt{
public:
    ClientConnection*parent=nullptr;
    explicit ClientConnectionPvt(ClientConnection*parent)
    {
        this->parent=parent;
    }

    virtual ~ClientConnectionPvt()
    {
    }
};

ClientConnection::ClientConnection(QObject *parent):QObject(parent)
{
    this->p=new ClientConnectionPvt(this);
}

ClientConnection::~ClientConnection()
{
    dPvt();
    delete&p;
}

}

