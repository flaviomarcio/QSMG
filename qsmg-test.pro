QT += testlib

CONFIG += testcase
CONFIG += console
CONFIG += silent
CONFIG -= debug_and_release
QT -= gui

TEMPLATE = app
TARGET = QStmTest

INCLUDEPATH+=$$PWD/../src

QSMG_TEST_MODE=true
QMAKE_CXXFLAGS += -DQSMG_TEST_MODE=\\\"$$QSMG_TEST_MODE\\\"

LIBS += -lgmock
LIBS += -lgtest

include($$PWD/src/qsmg.pri)
include($$PWD/test/qsmg-test.pri)

