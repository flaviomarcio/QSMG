#include "./qsmg_client_instance.h"

namespace QSMG {

#define dPvt()\
auto&p = *reinterpret_cast<QSMG::ClientInstancePvt*>(this->p)

class ClientInstancePvt{
public:
    ClientInstance*parent=nullptr;
    explicit ClientInstancePvt(ClientInstance*parent)
    {
        this->parent=parent;
    }

    virtual ~ClientInstancePvt()
    {
    }
};

ClientInstance::ClientInstance(QObject *parent):QObject(parent)
{
    this->p=new ClientInstancePvt(this);
}

ClientInstance::~ClientInstance()
{
    dPvt();
    delete&p;
}

bool ClientInstance::connect()
{
    return false;
}

bool ClientInstance::disconnect()
{
    return false;
}

bool ClientInstance::isConnected()
{
    return false;
}

bool ClientInstance::sendMessage(const QByteArray &message)
{
    Q_UNUSED(message);
    return false;
}

bool ClientInstance::start()
{
    return false;
}

bool ClientInstance::restart()
{
    return false;
}

bool ClientInstance::stop()
{
    return false;
}

}

