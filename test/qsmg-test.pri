INCLUDEPATH+=$$PWD

CONFIG += console
CONFIG -= debug_and_release
CONFIG += testcase
LIBS += -L/usr/local/lib -lgmock
LIBS += -L/usr/local/lib -lgtest

Q_SMG_TEST=true
QMAKE_CXXFLAGS += -DQ_SMG_TEST=\\\"$$Q_SMG_TEST\\\"

HEADERS += \
    $$PWD/qsmg_test.h \
    $$PWD/qsmg_test_unit.h \
    $$PWD/qsmg_test_functional.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/qsmg_unit_test_guardian.cpp \
    $$PWD/qsmg_unit_test_server.cpp \
    $$PWD/qsmg_unit_test_client.cpp \
