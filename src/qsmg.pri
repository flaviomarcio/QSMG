QT += core
QT += network
QT += websockets

INCLUDEPATH+=$$PWD

HEADERS += \
    $$PWD/private/p_qsmg_guardian_process.h \
    $$PWD/private/p_qsmg_guardian.h \
    $$PWD/private/p_qsmg_item_limits.h \
    $$PWD/private/p_qsmg_util.h \
    $$PWD/qsmg_global.h \    
    $$PWD/qsmg_object.h \
    $$PWD/qsmg_guardian.h

SOURCES += \
    $$PWD/private/p_qsmg_item_limits.cpp \
    $$PWD/qsmg_object.cpp \
    $$PWD/qsmg_guardian.cpp
