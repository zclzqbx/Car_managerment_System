#-------------------------------------------------
#
# Project created by QtCreator 2015-05-08T18:59:51
#
#-------------------------------------------------

QT       += core gui
QT       += sql xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = manager
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    pieview.cpp \
    logindialog.cpp \
    pwdchangedialog.cpp

HEADERS  += widget.h \
    connection.h \
    connection.h \
    pieview.h \
    logindialog.h \
    pwdchangedialog.h

FORMS    += widget.ui \
    logindialog.ui \
    pwdchangedialog.ui
