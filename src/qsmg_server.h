#pragma once

#include "./qsmg_global.h"

namespace QSMG {

//!
//! \brief The Object class
//!
//! basic object class
class Q_SMG_EXPORT Server: public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject*parent=nullptr);
    ~Server();
private:
    void*p=nullptr;
};

}

