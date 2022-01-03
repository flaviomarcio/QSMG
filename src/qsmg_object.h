#pragma once

#include "./qsmg_global.h"

namespace QSMG {

//!
//! \brief The Object class
//!
//! basic object class
class Q_SMG_EXPORT Object: public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit Object(QObject*parent=nullptr);
    ~Object();
};

}

