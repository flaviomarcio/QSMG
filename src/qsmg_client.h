#pragma once

#include "./qsmg_global.h"

namespace QSMG {

//!
//! \brief The Object class
//!
//! basic object class
class Q_SMG_EXPORT Client: public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject*parent=nullptr);
    ~Client();
private:
    void*p=nullptr;
};

}

