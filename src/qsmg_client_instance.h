#pragma once

#include "./qsmg_global.h"

namespace QSMG {

//!
//! \brief The Object class
//!
//! basic object class
class Q_SMG_EXPORT ClientInstance: public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief ClientInstance
    //! \param parent
    //!
    explicit ClientInstance(QObject*parent=nullptr);

    //!
    ~ClientInstance();

    //!
    //! \brief connect
    //! \return
    //!
    virtual bool connect();

    //!
    //! \brief disconnect
    //! \return
    //!
    virtual bool disconnect();

    //!
    //! \brief isConnected
    //! \return
    //!
    virtual bool isConnected();

    //!
    //! \brief sendMessage
    //! \param message
    //! \return
    //!
    virtual bool sendMessage(const QByteArray&message);

    //!
    //! \brief start
    //! \return
    //!
    virtual bool start();

    //!
    //! \brief restart
    //! \return
    //!
    virtual bool restart();

    //!
    //! \brief stop
    //! \return
    //!
    virtual bool stop();


Q_SIGNALS:
    void messages(const QByteArray&message);
    void stats(const QByteArray&message);
private:
    void*p=nullptr;
};

}

