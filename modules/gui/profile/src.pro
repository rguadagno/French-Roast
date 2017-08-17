######################################################################
# Automatically generated by qmake (3.0) Mon Mar 14 11:54:36 2016
######################################################################

TEMPLATE = app
TARGET = roaster
INCLUDEPATH += \
../include \
../../network/include \
../../agent/include \
../../instrumentation/include \
../../monitor/include

QT += core gui widgets
# Input
SOURCES += \
../src/*.cpp \
../../agent/src/HookValidator.cpp  \
../../network/src/ConnectorWindows.cpp  \
../../network/src/Connector.cpp \
../../instrumentation/src/Util.cpp \
../../monitor/src/Monitor.cpp \
../../monitor/src/StackTrace.cpp \
../../monitor/src/MonitorUtil.cpp \
../../monitor/src/MethodStats.cpp \
../../monitor/src/StackReport.cpp \
../../monitor/src/ClassDetail.cpp \
../../monitor/src/JammedReport.cpp \
../../monitor/src/StackFrame.cpp

HEADERS += ../include/*.h
LIBS +=  ws2_32.lib
CONFIG += console
