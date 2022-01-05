QT += core
QT += network
QT += websockets

INCLUDEPATH+=$$PWD

HEADERS += \
    $$PWD/private/p_qsmg_guardian_server_local_socket.h \
    $$PWD/private/p_qsmg_guardian_thread.h \
    $$PWD/private/p_qsmg_guardian_process.h \
    $$PWD/private/p_qsmg_guardian.h \
    $$PWD/private/p_qsmg_item_limits.h \
    $$PWD/private/p_qsmg_util.h \
    $$PWD/qsmg_global.h \    
    $$PWD/qsmg_object.h \
    $$PWD/qsmg_guardian.h \
    $$PWD/qsmg_server.h \
    $$PWD/qsmg_client_connection.h \
    $$PWD/qsmg_client_instance.h \
    $$PWD/qsmg_client.h \

SOURCES += \
    $$PWD/private/p_qsmg_guardian_server_local_socket.cpp \
    $$PWD/private/p_qsmg_item_limits.cpp \
    $$PWD/qsmg_object.cpp \
    $$PWD/qsmg_guardian.cpp \
    $$PWD/qsmg_server.cpp \
    $$PWD/qsmg_client_connection.cpp \
    $$PWD/qsmg_client_instance.cpp \
    $$PWD/qsmg_client.cpp \
