#pragma once

#include "./qsmg_global.h"

namespace QSMG {

//!
//! \brief The Object class
//!
//! basic object class
class Q_SMG_EXPORT ClientConnection: public QObject
{
    Q_OBJECT
public:
    explicit ClientConnection(QObject*parent=nullptr);
    ~ClientConnection();
private:
    void*p=nullptr;
};

}

