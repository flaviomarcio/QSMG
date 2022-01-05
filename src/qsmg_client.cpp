#include "./qsmg_client.h"

namespace QSMG {

#define dPvt()\
auto&p = *reinterpret_cast<QSMG::ClientPvt*>(this->p)

class ClientPvt{
public:
    Client*parent=nullptr;
    explicit ClientPvt(Client*parent)
    {
        this->parent=parent;
    }

    virtual ~ClientPvt()
    {
    }
};

Client::Client(QObject *parent):QObject(parent)
{
    this->p=new ClientPvt(this);
}

Client::~Client()
{
    dPvt();
    delete&p;
}

}

